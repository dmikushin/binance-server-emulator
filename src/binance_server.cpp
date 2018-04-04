#include "binance.h"
#include "binance_parser.h"
#include "binance_server.h"
#include "timing.h"

#include <curl/curl.h>
#include <jsoncpp/json/json.h>
#include <mutex>
#include <sstream>

using namespace binance;
using namespace std;

static mutex serverMutex1, serverMutex2;

unique_ptr<Server> binance::Server::server = NULL;

binance::Server::Server()
{
	// Initialize time on server. Initially, server time is real time.
	get_time(&initial);
	get_time(&real);
	scale = 1;
}

uint64_t binance::Server::getTime()
{
	if (!server)
	{
		serverMutex1.lock();
		if (!server)
			server.reset(new Server());
		serverMutex1.unlock();
	}

	uint64_t current; get_time(&current);

	return server->initial + (current - server->real) * server->scale;
}

void binance::Server::setTime(const uint64_t initial_, const double scale_)
{
	if (!server)
	{
		serverMutex1.lock();
		if (!server)
			server.reset(new Server());
		serverMutex1.unlock();
	}
	
	serverMutex2.lock();
	server->initial = initial_; server->scale = scale_;
	serverMutex2.unlock();
}

binanceError_t binance::Server::getTime(string& result)
{
	long value = Server::getTime();
	stringstream ss;
	ss << "{\"serverTime\":";
	ss << value;
	ss << "}";
	result = ss.str();
	
	return binanceSuccess;
}

binanceError_t binance::Server::setTime(string& result, const vector<pair<string, string> >& args)
{
	string initial = "";
	string scale = "";

	map<string, string> unknownArgs;
	for (int i = 0, e = args.size(); i != e; i++)
	{
		const pair<string, string>& arg = args[i];

		if (arg.first == "initial")
		{
			Parser::readValue(arg, initial, result);
			if (result != "")
				return binanceSuccess;
		}
		else if (arg.first == "scale")
		{
			Parser::readValue(arg, scale, result);
			if (result != "")
				return binanceSuccess;
		}
		else
		{
			if (unknownArgs[arg.first] != "")
			{
				result = "{\"code\":-1101,\"msg\":\"Duplicate values for a parameter detected.\"}";
				return binanceSuccess;
			}
			
			unknownArgs[arg.first] = arg.second;
		}
	}

	if (initial == "")
	{
		result = "{\"code\":-1102,\"msg\":\"Mandatory parameter 'initial' was not sent, was empty/null, or malformed.\"}";
		return binanceSuccess;
	}

	if (unknownArgs.size())
	{
		stringstream ss;
		ss << "{\"code\":-1104,\"msg\":\"Not all sent parameters were read; read '";
		ss << args.size() - unknownArgs.size();
		ss << "' parameter(s) but was sent '";
		ss << args.size();
		ss << "'.\"}";
		result = ss.str();
		return binanceSuccess;
	}

	int initialValue;
	Parser::parsePositiveInteger("initial", initial, initialValue, result);
	if (result != "")
		return binanceSuccess;

	double scaleValue = 1.0;
	if (scale != "")
	{
		Parser::parseFloatingPoint("scale", scale, scaleValue, result);
		if (result != "")
			return binanceSuccess;
	}

	setTime(initialValue, scaleValue);
	return binanceSuccess;
}

// Curl's callback
static size_t getCurlCb(void *content, size_t size, size_t nmemb, string *buffer)
{
	size_t newLength = size * nmemb;
	size_t oldLength = buffer->size();

	buffer->resize(oldLength + newLength);

	copy((char*)content, (char*)content + newLength, buffer->begin() + oldLength);

	return newLength;
}

binanceError_t binance::Server::getCurl(string& result_json, const string& url)
{
	vector<string> v;
	string action = "GET";
	string post_data = "";
	return getCurlWithHeader(result_json, url, v, post_data, action);
}

// Do the curl
binanceError_t binance::Server::getCurlWithHeader(string& str_result, 
	const string& url, const vector<string>& extra_http_header, const string& post_data, const string& action)
{
	binanceError_t status = binanceSuccess;
	
	CURL* curl = curl_easy_init();

	if (curl)
	{
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, getCurlCb);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &str_result);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);

		if (extra_http_header.size() > 0)
		{
			struct curl_slist *chunk = NULL;
			for (int i = 0; i < extra_http_header.size(); i++)
				chunk = curl_slist_append(chunk, extra_http_header[i].c_str());

 			curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
 		}

 		if (post_data.size() > 0 || action == "POST" || action == "PUT" || action == "DELETE")
 		{
 			if (action == "PUT" || action == "DELETE")
 				curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, action.c_str());
 			curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_data.c_str());
 		}

		CURLcode res;

		try
		{
			res = curl_easy_perform(curl);
		}
		catch (bad_alloc &e)
		{
			status = binanceErrorCurlOutOfMemory;
		}
		
		if (status == binanceSuccess)
		{
			// Check for errors.
			if (res != CURLE_OK)
			{
				status = binanceErrorCurlFailed;
			}
		}

		// Always cleanup.
		curl_easy_cleanup(curl);
	}

	return status;
}

