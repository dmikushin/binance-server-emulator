#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <microhttpd.h>
#include <string>
#include <sstream>

#include "binance.h"
#include "timing.h"

#define BINANCE_ERR_CHECK(x) do { \
	binanceError_t error = x; if (error != binanceSuccess) return MHD_HTTP_BAD_REQUEST; \
} while (0)

const char APIprefix[] = "/api/v";

using namespace binance;
using namespace std;

static bool startsWith(const string& what, const string& with)
{
	return (what.compare(0, with.length(), with) == 0);
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
		return MHD_HTTP_NOT_FOUND;

	{
		string url(curl, curl + sizeof(APIprefix) - 1);
		if (url != APIprefix)
			return MHD_HTTP_NOT_FOUND;
	}
	
	string url(curl + sizeof(APIprefix) - 1);
	string result;
	if (url[0] == '1')
	{
		if (url == "1/time")
		{
			uint64_t value; get_time(&value);
			stringstream ss;
			ss << "{\"serverTime\":";
			ss << value;
			ss << "}";
			result = ss.str();
		}
		else if (startsWith(url, "1/ticker/24hr"))
		{
			BINANCE_ERR_CHECK(binance::Market::get24hr(result, url));
		}
		else if (startsWith(url, "1/aggTrades"))
		{
			BINANCE_ERR_CHECK(binance::Market::getAggTrades(result, url));
		}
		else if (url == "1/ticker/allBookTickers")
		{
			BINANCE_ERR_CHECK(binance::Market::getAllBookTickers(result));
		}
		else if (url == "1/ticker/allPrices")
		{
			BINANCE_ERR_CHECK(binance::Market::getAllPrices(result));
		}
		else if (startsWith(url, "1/depth"))
		{
			BINANCE_ERR_CHECK(binance::Market::getDepth(result, url));
		}
		else if (startsWith(url, "1/historicalTrades"))
		{
			BINANCE_ERR_CHECK(binance::Account::getHistoricalTrades(result, url));
		}
		else if (startsWith(url, "1/klines"))
		{
			BINANCE_ERR_CHECK(binance::Market::getKlines(result, url));
		}
		else if (startsWith(url, "1/trades"))
		{
			BINANCE_ERR_CHECK(binance::Account::getTrades(result, url));
		}
		else if (startsWith(url, "1/userDataStream"))
		{
			BINANCE_ERR_CHECK(binance::Account::startUserDataStream(result, url));
		}
		else
		{
			return MHD_HTTP_NOT_FOUND;
		}
	}
	else if (url[0] == '3')
	{
		if (startsWith(url, "3/account"))
		{
			BINANCE_ERR_CHECK(binance::Account::getInfo(result, url));
		}
		else if (startsWith(url, "3/allOrders"))
		{
			BINANCE_ERR_CHECK(binance::Account::getAllOrders(result, url));
		}
		else if (startsWith(url, "3/myTrades"))
		{
			BINANCE_ERR_CHECK(binance::Account::getTrades(result, url));
		}
		else if (startsWith(url, "3/openOrders"))
		{
			BINANCE_ERR_CHECK(binance::Account::getOpenOrders(result, url));
		}
		else if (startsWith(url, "3/order"))
		{
			BINANCE_ERR_CHECK(binance::Account::sendOrder(result, url));
		}
		else
		{
			return MHD_HTTP_NOT_FOUND;
		}
	}
	else
	{
		return MHD_HTTP_NOT_FOUND;
	}

	result = "<html><head></head><body><pre style=\"word-wrap: break-word; white-space: pre-wrap;\">"
		+ result + "</pre></body></html>";

    // Reset when done.
    *ptr = NULL;
    struct MHD_Response* response = MHD_create_response_from_buffer(
    	result.size(), &result[0], MHD_RESPMEM_MUST_COPY);
    int ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
    MHD_destroy_response(response);
    
    return ret;
}

int main(int argc, char* argv[])
{
	if (argc != 2)
	{
		printf("%s <port>\n", argv[0]);
		return 1;
	}

	struct MHD_Daemon* d = MHD_start_daemon(
		MHD_USE_SELECT_INTERNALLY | MHD_USE_DEBUG,
		atoi(argv[1]), NULL, NULL, &callback, NULL,
		MHD_OPTION_CONNECTION_TIMEOUT, (unsigned int)120,
		MHD_OPTION_END);

	if (d == NULL)
	{
		fprintf(stderr, "Error starting server %s with port %d\n", argv[0], atoi(argv[1]));
		return 1;
	}

	(void)getc(stdin);

	MHD_stop_daemon(d);

	return 0;
}

