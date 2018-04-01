#if defined(TESTING)

#include <curl/curl.h>
#include <jsoncpp/json/json.h>
#include <pthread.h>
#include <string>

#include "binance.h"
#include "gtest/gtest.h"

class BinanceGlobalInit
{
public :

	BinanceGlobalInit()
	{
		curl_global_init(CURL_GLOBAL_DEFAULT);
	}

	~BinanceGlobalInit()
	{
		curl_global_cleanup();
	}
};

// Initialize globals.
static BinanceGlobalInit binanceGlobalInit;

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

static string hostnames[] = { "http://localhost:8080", "https://api.binance.com" };

static const string marketErrors[][2] =
{
	{ "1/timea",
	  "<html><body><h2>404 Not found</h2></body></html>" },

	{ "1/time??",
	  "{\"code\":-1101,\"msg\":\"Too many parameters; expected '0' and received '1'.\"}" },

	{ "1/ticker/24hr?limit=1",
	  "{\"code\":-1104,\"msg\":\"Not all sent parameters were read; read '0' parameter(s) but was sent '1'.\"}" },
	
	{ "1/ticker/24hr?symbola=BNBBTC",
	  "{\"code\":-1104,\"msg\":\"Not all sent parameters were read; read '0' parameter(s) but was sent '1'.\"}" },

	{ "1/ticker/24hr?symbol=BNBBTC&limit=1",
	  "{\"code\":-1101,\"msg\":\"Too many parameters; expected '1' and received '2'.\"}" },

	{ "1/ticker/24hr?symbol=BNBBTC&symbol=BNBBTC",
	  "{\"code\":-1101,\"msg\":\"Duplicate values for a parameter detected.\"}" },
	
	{ "1/ticker/24hr?symbol=BNBBTC?symbol=BNBBTC",
	  "{\"code\":-1100,\"msg\":\"Illegal characters found in parameter 'symbol'; legal range is '^[A-Z0-9_]{1,20}$'.\"}" },

	{ "1/aggTrades?symbol=BNBBTC&startTime=1",
	  "{\"code\":-1128,\"msg\":\"Combination of optional parameters invalid.\"}" },
	
	{ "1/aggTrades?symbol=BNBBTC&limit=1&startTime=1",
	  "{\"code\":-1128,\"msg\":\"Combination of optional parameters invalid.\"}" },
	
	{ "1/aggTrades?symbol=BNBBTC&limit=-1",
	  "{\"code\":-1100,\"msg\":\"Illegal characters found in parameter 'limit'; legal range is '^[0-9]{1,20}$'.\"}" },

	{ "1/aggTrades?symbol=BNBBTC&fromId=-1",
	  "{\"code\":-1100,\"msg\":\"Illegal characters found in parameter 'fromId'; legal range is '^[0-9]{1,20}$'.\"}" },

	{ "1/aggTrades?symbol=BNBBTC&symbol=BNBBBC",
	  "{\"code\":-1101,\"msg\":\"Duplicate values for a parameter detected.\"}" },
	
	{ "1/aggTrades?symbol=BNBBTC&endTime=1",
	  "{\"code\":-1128,\"msg\":\"Combination of optional parameters invalid.\"}" },
	
	{ "1/aggTrades",
	  "{\"code\":-1102,\"msg\":\"Mandatory parameter 'symbol' was not sent, was empty/null, or malformed.\"}" },

	{ "1/klines?symbol=BNBBTC",
	  "{\"code\":-1102,\"msg\":\"Mandatory parameter 'interval' was not sent, was empty/null, or malformed.\"}" },

	{ "1/klines?fromId=1",
	  "{\"code\":-1102,\"msg\":\"Mandatory parameter 'symbol' was not sent, was empty/null, or malformed.\"}" },
	
	{ "1/klines?fromId=1&symbol=BNBBTC&interval=1h",
	  "{\"code\":-1104,\"msg\":\"Not all sent parameters were read; read '2' parameter(s) but was sent '3'.\"}" },
};

TEST(market, errors)
{
	for (int i = 0, e = sizeof(marketErrors) / sizeof(marketErrors[0]); i < e; i++)
	{
		const string* const& marketError = marketErrors[i];
	
		for (int j = 0; j < sizeof(hostnames) / sizeof(hostnames[0]); j++)
		{
			const string& hostname = hostnames[j];
			
			string url(hostname + "/api/v" + marketError[0]);
			string result;
			Server::getCurl(result, url);
			
			if (result != marketError[1])

			EXPECT_TRUE(result.size() != 0);

			Json::Reader reader;
			Json::Value jsonActualResult;
			EXPECT_NO_THROW(reader.parse(result, jsonActualResult));

printf("test = %s\n", marketError[0].c_str());
printf("result = %s\n", result.c_str());
printf("expected = %s\n", marketError[1].c_str());

			Json::Value jsonExpectedResult;
			EXPECT_NO_THROW(reader.parse(marketError[1], jsonExpectedResult));
			
			EXPECT_EQ(jsonActualResult, jsonExpectedResult);
		}
	}
}

void* biserver_thread(void* arg);

int main(int argc, char* argv[])
{
	::testing::InitGoogleTest(&argc, argv);

	// Start the server emulation.
	int port = 8080;
	pthread_t thread;
	EXPECT_EQ(pthread_create(&thread, NULL, &biserver_thread, &port), 0);

	int result = RUN_ALL_TESTS();
	
	EXPECT_EQ(pthread_cancel(thread), 0);
	EXPECT_EQ(pthread_join(thread, NULL), 0);
	
	return result;
}

#endif // TESTING

