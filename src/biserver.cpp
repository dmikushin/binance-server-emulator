#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <microhttpd.h>
#include <string>
#include <sstream>

#include "binance.h"
#include "binance_server.h"

#define BINANCE_ERR_CHECK(x) do { \
	binanceError_t error = x; if (error != binanceSuccess) return MHD_HTTP_BAD_REQUEST; \
} while (0)

const char APIprefix[] = "/api/v";

using namespace binance;
using namespace std;

static int callback_arguments(void *cls, enum MHD_ValueKind kind, const char *key, const char *value)
{
	vector<pair<string, string> >* args = (vector<pair<string, string> >*)cls;
	
	if (key && value)
		args->push_back(pair<string, string>(key, value));
	else
	{
		if (!key)
			args->push_back(pair<string, string>("", value));
		else if (!value)
			args->push_back(pair<string, string>(key, ""));
	}
	
	return MHD_YES;
}

static int callback_headers(void *cls, enum MHD_ValueKind kind, const char *key, const char *value)
{
	vector<pair<string, string> >* headers = (vector<pair<string, string> >*)cls;
	
	if (key && value)
		headers->push_back(pair<string, string>(key, value));
	else
	{
		if (!key)
			headers->push_back(pair<string, string>("", value));
		else if (!value)
			headers->push_back(pair<string, string>(key, ""));
	}
	
	return MHD_YES;
}

static int result_404(struct MHD_Connection* connection)
{
	static string message_404 = "<html><body><h2>404 Not found</h2></body></html>";
	
	struct MHD_Response* response = MHD_create_response_from_buffer(
		message_404.size(), &message_404[0], MHD_RESPMEM_MUST_COPY);
	int ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
	MHD_destroy_response(response);

	return ret;	
}

static int callback(void* cls, struct MHD_Connection* connection,
	const char* curl, const char* method, const char* version,
	const char* upload_data, size_t* upload_data_size, void** ptr)
{
	if (strcmp(method, "GET"))
	{
		// Unexpected method.
		return MHD_NO;
	}

	static int aptr;
	if (&aptr != *ptr)
	{
		// Do never respond on first call.
		*ptr = &aptr;
		return MHD_YES;
    }

	if (strlen(curl) <= sizeof(APIprefix) - 1)
		return result_404(connection);

	{
		string url(curl, curl + sizeof(APIprefix) - 1);
		if (url != APIprefix)
			return result_404(connection);
	}

	vector<pair<string, string> > args;
	MHD_get_connection_values(connection, MHD_GET_ARGUMENT_KIND, &callback_arguments, (void*)&args);

	vector<pair<string, string> > headers;
	MHD_get_connection_values(connection, MHD_HEADER_KIND, &callback_headers, (void*)&headers);

	string url(curl + sizeof(APIprefix) - 1);
	string result;
	if (url[0] == '1')
	{
		if (url == "1/time")
		{
			if (args.size() != 0)
				BINANCE_ERR_CHECK(binance::Server::setTime(result, args));

			if (result == "")
				BINANCE_ERR_CHECK(binance::Server::getTime(result));
		}
		else if (url == "1/ticker/24hr")
		{
			BINANCE_ERR_CHECK(binance::Market::get24hr(result, args));
		}
		else if (url == "1/aggTrades")
		{
			BINANCE_ERR_CHECK(binance::Market::getAggTrades(result, args));
		}
		else if (url == "1/ticker/allBookTickers")
		{
			BINANCE_ERR_CHECK(binance::Market::getAllBookTickers(result));
		}
		else if (url == "1/ticker/allPrices")
		{
			BINANCE_ERR_CHECK(binance::Market::getAllPrices(result));
		}
		else if (url == "1/depth")
		{
			BINANCE_ERR_CHECK(binance::Market::getDepth(result, args));
		}
		else if (url == "1/historicalTrades")
		{
			BINANCE_ERR_CHECK(binance::Account::getHistoricalTrades(result, args, headers));
		}
		else if (url == "1/klines")
		{
			BINANCE_ERR_CHECK(binance::Market::getKlines(result, args));
		}
		else if (url == "1/trades")
		{
			BINANCE_ERR_CHECK(binance::Account::getTrades(result, args, headers));
		}
		else if (url == "1/userDataStream")
		{
			BINANCE_ERR_CHECK(binance::Account::startUserDataStream(result, headers));
		}
		else
		{
			return result_404(connection);
		}
	}
	else if (url[0] == '3')
	{
		if (url == "3/account")
		{
			BINANCE_ERR_CHECK(binance::Account::getInfo(result, args, headers));
		}
		else if (url == "3/allOrders")
		{
			BINANCE_ERR_CHECK(binance::Account::getAllOrders(result, args, headers));
		}
		else if (url == "3/myTrades")
		{
			BINANCE_ERR_CHECK(binance::Account::getTrades(result, args, headers));
		}
		else if (url == "3/openOrders")
		{
			BINANCE_ERR_CHECK(binance::Account::getOpenOrders(result, args, headers));
		}
		else if (url == "3/order")
		{
			BINANCE_ERR_CHECK(binance::Account::sendOrder(result, args, headers));
		}
		else
		{
			return result_404(connection);
		}
	}
	else
	{
		return result_404(connection);
	}

    // Reset when done.
    *ptr = NULL;
    struct MHD_Response* response = MHD_create_response_from_buffer(
    	result.size(), &result[0], MHD_RESPMEM_MUST_COPY);
    int ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
    MHD_destroy_response(response);
    
    return ret;
}

static int biserver(int port)
{
	struct MHD_Daemon* d = MHD_start_daemon(
		MHD_USE_SELECT_INTERNALLY | MHD_USE_DEBUG,
		port, NULL, NULL, &callback, NULL,
		MHD_OPTION_CONNECTION_TIMEOUT, (unsigned int)120,
		MHD_OPTION_END);

	if (d == NULL)
	{
		fprintf(stderr, "Error starting server with port %d\n", port);
		return 1;
	}

#if defined(TESTING)
	while (1) usleep(1);
#else
	(void)getc(stdin);
#endif // TESTING

	MHD_stop_daemon(d);

	return 0;
}

#if !defined(TESTING)
int main(int argc, char* argv[])
{
	if (argc != 2)
	{
		printf("%s <port>\n", argv[0]);
		return 1;
	}
	
	return biserver(atoi(argv[1]));
}
#else
void* biserver_thread(void* arg)
{
	if (pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL) != 0)
		exit(EXIT_FAILURE);

	int port = *(int*)arg;

	biserver(port);
	
	return NULL;
}
#endif // TESTING

