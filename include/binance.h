#ifndef BINANCE_H
#define BINANCE_H

#include <string>

namespace binance
{
	enum binanceError_t
	{
		binanceSuccess = 0,
		binanceErrorInvalidServerResponse,
		binanceErrorEmptyServerResponse,
		binanceErrorParsingServerResponse,
		binanceErrorInvalidSymbol,
		binanceErrorMissingAccountKeys,
		binanceErrorCurlFailed,
		binanceErrorCurlOutOfMemory,
	};

	const char* binanceGetErrorString(const binanceError_t err);

	class Market
	{
	public :
	
		static binanceError_t get24hr(std::string& result, const std::string& url);
		static binanceError_t getAggTrades(std::string& result, const std::string& url);
		static binanceError_t getAllBookTickers(std::string& result);
		static binanceError_t getAllPrices(std::string& result);
		static binanceError_t getDepth(std::string& result, const std::string& url);
		static binanceError_t getKlines(std::string& result, const std::string& url);
		static binanceError_t getTrades(std::string& result, const std::string& url);
	};
	
	class Account
	{
	public :
	
		static binanceError_t getInfo(std::string& result, const std::string& url);
		static binanceError_t getAllOrders(std::string& result, const std::string& url);
		static binanceError_t getHistoricalTrades(std::string& result, const std::string& url);
		static binanceError_t getTrades(std::string& result, const std::string& url);
		static binanceError_t getOpenOrders(std::string& result, const std::string& url);
		static binanceError_t sendOrder(std::string& result, const std::string& url);
		static binanceError_t startUserDataStream(std::string& result, const std::string& url);
	};
}

#endif // BINANCE_H

