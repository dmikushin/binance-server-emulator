#if defined(TESTING)

#include <curl/curl.h>
#include <fstream>
#include <jsoncpp/json/json.h>
#include <openssl/hmac.h>
#include <openssl/sha.h>
#include <pthread.h>
#include <string>
#include <unistd.h>
#include <wordexp.h>

#include "binance.h"
#include "binance_server.h"
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

void* biserver_thread(void* arg);

using namespace binance;
using namespace std;

namespace binance
{
	string b2a_hex( char *byte_arr, int n )
	{
		const static std::string HexCodes = "0123456789abcdef";
		string HexString;
		for ( int i = 0; i < n ; ++i ) {
			unsigned char BinValue = byte_arr[i];
			HexString += HexCodes[( BinValue >> 4 ) & 0x0F];
			HexString += HexCodes[BinValue & 0x0F];
		}

		return HexString;
	}

	string hmac_sha256( const char *key, const char *data)
	{
		unsigned char* digest;
		digest = HMAC(EVP_sha256(), key, strlen(key), (unsigned char*)data, strlen(data), NULL, NULL);	
		return b2a_hex( (char *)digest, 32 );
	}   

	string sha256( const char *data )
	{
		unsigned char digest[32];
		SHA256_CTX sha256;
		SHA256_Init(&sha256);
		SHA256_Update(&sha256, data, strlen(data) );
		SHA256_Final(digest, &sha256);
		return b2a_hex( (char *)digest, 32 );	
	}
}

static string hostnames[] = { "http://localhost:8080", "https://api.binance.com" };

static const string marketErrors[][2] =
{
	{ "1/timea",
	  "<html><body><h2>404 Not found</h2></body></html>" },

	{ "1/ticker/24hr?limit=1",
	  "{\"code\":-1104,\"msg\":\"Not all sent parameters were read; read '0' parameter(s) but was sent '1'.\"}" },
	
	{ "1/ticker/24hr?limit=1&limit=2",
	  "{\"code\":-1101,\"msg\":\"Duplicate values for a parameter detected.\"}" },
	
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

static const string accountErrors[][2] =
{
	{ "3/account?timestamp=9223372036854775807",
	  "{\"code\":-1021,\"msg\":\"Timestamp for this request was 1000ms ahead of the server's time.\"}" },
};

static const string default_api_key_path = "$HOME/.bitrader/key";
static const string default_secret_key_path = "$HOME/.bitrader/secret";

TEST(time, value)
{
	const string hostname = "http://localhost:8080";

	string url, result;
	
	url = hostname + "/api/v1/time";
	Server::getCurl(result, url);
	
	EXPECT_TRUE(result.size() != 0);

	Json::Reader reader;
	Json::Value jsonCurrentTime;
	EXPECT_NO_THROW(reader.parse(result, jsonCurrentTime));

	url = hostname + "/api/v1/time?initial=12345678&scale=0";
	result = "";
	Server::getCurl(result, url);
	
	EXPECT_TRUE(result.size() != 0);

	Json::Value jsonModifiedTime;
	EXPECT_NO_THROW(reader.parse(result, jsonModifiedTime));
	
	uint64_t oldTime;
	EXPECT_TRUE(stringstream(jsonCurrentTime["serverTime"].asString()) >> oldTime);
	printf("oldTime = %lu\n", oldTime);
	EXPECT_TRUE(oldTime != 12345678);
	
	uint64_t newTime;
	EXPECT_TRUE(stringstream(jsonModifiedTime["serverTime"].asString()) >> newTime);	
	printf("newTime = %lu\n", newTime);
	EXPECT_TRUE(newTime == 12345678);
	
	oldTime = newTime;

	url = hostname + "/api/v1/time?initial=12345678&scale=1";
	result = "";
	Server::getCurl(result, url);

	url = hostname + "/api/v1/time";
	result = "";
	Server::getCurl(result, url);
	EXPECT_NO_THROW(reader.parse(result, jsonModifiedTime));

	EXPECT_TRUE(stringstream(jsonModifiedTime["serverTime"].asString()) >> newTime);	
	printf("oldTime = %lu\n", oldTime);
	printf("newTime = %lu\n", newTime);
	EXPECT_TRUE(newTime > oldTime);	

	oldTime = newTime;

	url = hostname + "/api/v1/time?initial=0";
	result = "";
	Server::getCurl(result, url);
	EXPECT_NO_THROW(reader.parse(result, jsonModifiedTime));

	EXPECT_TRUE(stringstream(jsonModifiedTime["serverTime"].asString()) >> newTime);	
	printf("oldTime = %lu\n", oldTime);
	printf("newTime = %lu\n", newTime);
	EXPECT_TRUE(newTime < oldTime);
	
	oldTime = newTime;

	url = hostname + "/api/v1/time?initial=0&scale=0";
	result = "";
	Server::getCurl(result, url);
	EXPECT_NO_THROW(reader.parse(result, jsonModifiedTime));

	EXPECT_TRUE(stringstream(jsonModifiedTime["serverTime"].asString()) >> newTime);	
	printf("oldTime = %lu\n", oldTime);
	printf("newTime = %lu\n", newTime);
	EXPECT_TRUE(newTime < oldTime);
}

static void testTimeScale(const string& url0, const string& url1, uint64_t& time0, uint64_t& time1)
{
	string result0, result1;
	
	Server::getCurl(result0, url0);	
	EXPECT_TRUE(result0.size() != 0);

	Server::getCurl(result1, url1);	
	EXPECT_TRUE(result1.size() != 0);

	unsigned int microseconds = 1000 * 1000; // 1 sec
	usleep(microseconds);

	result0 = "";
	Server::getCurl(result0, string(url0, 0, url0.find('?')));
	EXPECT_TRUE(result0.size() != 0);

	result1 = "";
	Server::getCurl(result1, string(url1, 0, url1.find('?')));	
	EXPECT_TRUE(result1.size() != 0);

	Json::Reader reader;
	Json::Value jsonResult0;
	Json::Value jsonResult1;
	EXPECT_NO_THROW(reader.parse(result0, jsonResult0));
	EXPECT_NO_THROW(reader.parse(result1, jsonResult1));

	EXPECT_TRUE(stringstream(jsonResult0["serverTime"].asString()) >> time0);
	EXPECT_TRUE(stringstream(jsonResult1["serverTime"].asString()) >> time1);	
}

TEST(time, scale)
{
	const string hostname0 = "http://localhost:8080";
	const string hostname1 = "http://localhost:8081";

	// Launch a second server to test scale behavior.
	int port = 8081;
	pthread_t thread;
	EXPECT_EQ(pthread_create(&thread, NULL, &biserver_thread, &port), 0);

	uint64_t time0, time1;

	// Test time on server with larger scale is running faster.
	testTimeScale(
		hostname0 + "/api/v1/time?initial=1&scale=2",
		hostname1 + "/api/v1/time?initial=1&scale=4",
		time0, time1);

	printf("time0 = %lu\n", time0);
	printf("time1 = %lu\n", time1);
	
	EXPECT_TRUE(time0 != 0);
	EXPECT_TRUE(time1 != 0);
	EXPECT_TRUE(time0 < time1);

	// Test the inverse behavior in case of negative scale.
	testTimeScale(
		hostname0 + "/api/v1/time?initial=12345678&scale=-2",
		hostname1 + "/api/v1/time?initial=12345678&scale=-4",
		time0, time1);

	printf("time0 = %lu\n", time0);
	printf("time1 = %lu\n", time1);
	
	EXPECT_TRUE(time0 != 0);
	EXPECT_TRUE(time1 != 0);
	EXPECT_TRUE(time0 > time1);

	EXPECT_EQ(pthread_cancel(thread), 0);
	EXPECT_EQ(pthread_join(thread, NULL), 0);
	
	// Test with zero scale the time freezes.
	string url, result;
	url = hostname0 + "/api/v1/time?initial=1&scale=0";
	Server::getCurl(result, url);

	Json::Reader reader;
	Json::Value jsonResult;
	EXPECT_NO_THROW(reader.parse(result, jsonResult));

	EXPECT_TRUE(stringstream(jsonResult["serverTime"].asString()) >> time0);	

	unsigned int microseconds = 1000 * 1000; // 1 sec
	usleep(microseconds);

	url = hostname0 + "/api/v1/time";
	result = "";
	Server::getCurl(result, url);

	EXPECT_NO_THROW(reader.parse(result, jsonResult));

	EXPECT_TRUE(stringstream(jsonResult["serverTime"].asString()) >> time1);

	printf("time0 = %lu\n", time0);
	printf("time1 = %lu\n", time1);
	
	EXPECT_TRUE(time0 != 0);
	EXPECT_TRUE(time0 == time1);
}

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
			
			EXPECT_TRUE(result.size() != 0);

			Json::Reader reader;
			Json::Value jsonActualResult;
			EXPECT_NO_THROW(reader.parse(result, jsonActualResult));

			printf("test = %s\n", url.c_str());
			printf("result = %s\n", result.c_str());
			printf("expected = %s\n", marketError[1].c_str());

			Json::Value jsonExpectedResult;
			EXPECT_NO_THROW(reader.parse(marketError[1], jsonExpectedResult));
			
			EXPECT_TRUE(jsonActualResult == jsonExpectedResult);
		}
	}
}

TEST(account, errors)
{
	string api_key = "";
	string secret_key = "";

	const string action = "GET";
	bool keysAreSet = false;
	for (int i = 0, e = sizeof(accountErrors) / sizeof(accountErrors[0]); i < e; i++)
	{
		if (!keysAreSet)
		{
			{
				wordexp_t p;
				char** w;
				wordexp(default_api_key_path.c_str(), &p, 0);
				w = p.we_wordv;
				ifstream binanceapi(w[0]);
				if (binanceapi.is_open())
				{
					binanceapi >> api_key;
					binanceapi.close();
				}
				wordfree(&p);
			}

			{
				wordexp_t p;
				char** w;
				wordexp(default_secret_key_path.c_str(), &p, 0);
				w = p.we_wordv;
				ifstream binanceapi(w[0]);
				if (binanceapi.is_open())
				{
					binanceapi >> secret_key;
					binanceapi.close();
				}
				wordfree(&p);
			}
	
			EXPECT_TRUE((api_key != "") && (secret_key != ""));

			keysAreSet = true;
		}

		const string* const& accountError = accountErrors[i];
	
		for (int j = 0; j < sizeof(hostnames) / sizeof(hostnames[0]); j++)
		{
			const string& hostname = hostnames[j];
			
			string url(hostname + "/api/v" + accountError[0]);
			string result;

			string querystring = "";
			int pos = url.find('?');
			if (pos == -1)
				url.append("?");
			else
			{
				if (url.size() > pos + 1)
					querystring = url.substr(pos + 1);
				url.append("&");
			}

			string signature = hmac_sha256(secret_key.c_str(), querystring.c_str());
			url.append("signature=");
			url.append(signature);

			vector <string> extra_http_header;
			string header_chunk("X-MBX-APIKEY: ");
			header_chunk.append(api_key);
			extra_http_header.push_back(header_chunk);

			string post_data = "";	
			Server::getCurlWithHeader(result, url, extra_http_header, post_data, action);

			EXPECT_TRUE(result.size() != 0);

			Json::Reader reader;
			Json::Value jsonActualResult;
			EXPECT_NO_THROW(reader.parse(result, jsonActualResult));

			printf("test = %s\n", url.c_str());
			printf("result = %s\n", result.c_str());
			printf("expected = %s\n", accountError[1].c_str());

			Json::Value jsonExpectedResult;
			EXPECT_NO_THROW(reader.parse(accountError[1], jsonExpectedResult));
			
			EXPECT_TRUE(jsonActualResult == jsonExpectedResult);
			
			if (jsonActualResult.size() == 0)
				EXPECT_TRUE(result == accountError[1]);			
		}
	}
}

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

