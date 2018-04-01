#if defined(TESTING)

#include "binance.h"

#include <curl/curl.h>
#include <jsoncpp/json/json.h>
#include <string>
#include <vector>

using namespace binance;
using namespace std;

namespace binance
{
	class Server
	{
	public :

		static binanceError_t getCurl(std::string &result_json, const std::string& url);

		static binanceError_t getCurlWithHeader(std::string& result_json, const std::string& url,
			const std::vector<std::string>& extra_http_header, const std::string& post_data, const std::string& action);
	};
}

using namespace binance;
using namespace std;

// Curl's callback
static size_t getCurlCb(void *content, size_t size, size_t nmemb, std::string *buffer)
{
	size_t newLength = size * nmemb;
	size_t oldLength = buffer->size();

	buffer->resize(oldLength + newLength);

	std::copy((char*)content, (char*)content + newLength, buffer->begin() + oldLength);

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
		catch (std::bad_alloc &e)
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

#endif // TESTING

