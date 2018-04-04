#ifndef BINANCE_H
#define BINANCE_H

#include <string>
#include <sys/time.h>
#include <vector>

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
		static binanceError_t get24hr(std::string& result);
		static binanceError_t get24hr(std::string& result, const char *symbol);
		static binanceError_t getAggTrades(std::string& result, const char *symbol, int fromId, int limit = 500);
		static binanceError_t getAggTrades(std::string& result, const char *symbol,
			time_t startTime, time_t endTime, int limit = 500);
		static binanceError_t getDepth(std::string& result, const char *symbol, int limit = 100);
		static binanceError_t getKlines(std::string& result, const char *symbol, const char *interval,
			time_t startTime = 0, time_t endTime = 0, int limit = 500);

	public :
	
		static binanceError_t get24hr(std::string& result, const std::vector<std::pair<std::string, std::string> >& args);
		static binanceError_t getAggTrades(std::string& result, const std::vector<std::pair<std::string, std::string> >& args);
		static binanceError_t getAllBookTickers(std::string& result);
		static binanceError_t getAllPrices(std::string& result);
		static binanceError_t getDepth(std::string& result, const std::vector<std::pair<std::string, std::string> >& args);
		static binanceError_t getKlines(std::string& result, const std::vector<std::pair<std::string, std::string> >& args);
		static binanceError_t getTrades(std::string& result, const std::vector<std::pair<std::string, std::string> >& args);
	};
	
	class Account
	{
		static binanceError_t getInfo(std::string& result, long timestamp, long recvWindow = 0);
		static binanceError_t getAllOrders(std::string& result, const char *symbol, long timestamp,
			long orderId = 0, int limit = 0, long recvWindow = 0);
		static binanceError_t getHistoricalTrades(std::string& result, const char *symbol, long fromId = -1, int limit = 500);
		static binanceError_t getTrades(std::string& result, const char *symbol, int limit = 500);
		static binanceError_t getTradesSigned(std::string& result, const char *symbol, long timestamp, long fromId = -1,
			long recvWindow = 0, int limit = 500);
		static binanceError_t getOpenOrders(std::string& result, long timestamp, long recvWindow = 0);
		static binanceError_t getOpenOrdersSigned(std::string& result, const char* symbol, long timestamp, long recvWindow = 0);
		static binanceError_t sendOrder(std::string& result, const char *symbol, const char *side, const char *type,
			const char *timeInForce, double quantity, double price, const char *newClientOrderId, double stopPrice,
			double icebergQty, long recvWindow);

	public :
	
		static binanceError_t getInfo(std::string& result, const std::vector<std::pair<std::string, std::string> >& args,
			const std::vector<std::pair<std::string, std::string> >& headers);
		static binanceError_t getAllOrders(std::string& result, const std::vector<std::pair<std::string, std::string> >& args,
			const std::vector<std::pair<std::string, std::string> >& headers);
		static binanceError_t getHistoricalTrades(std::string& result, const std::vector<std::pair<std::string, std::string> >& args,
			const std::vector<std::pair<std::string, std::string> >& headers);
		static binanceError_t getTrades(std::string& result, const std::vector<std::pair<std::string, std::string> >& args,
			const std::vector<std::pair<std::string, std::string> >& headers);
		static binanceError_t getTradesSigned(std::string& result, const std::vector<std::pair<std::string, std::string> >& args,
			const std::vector<std::pair<std::string, std::string> >& headers);
		static binanceError_t getOpenOrders(std::string& result, const std::vector<std::pair<std::string, std::string> >& args,
			const std::vector<std::pair<std::string, std::string> >& headers);
		static binanceError_t getOpenOrdersSigned(std::string& result, const std::vector<std::pair<std::string, std::string> >& args,
			const std::vector<std::pair<std::string, std::string> >& headers);
		static binanceError_t sendOrder(std::string& result, const std::vector<std::pair<std::string, std::string> >& args,
			const std::vector<std::pair<std::string, std::string> >& headers);
		static binanceError_t startUserDataStream(std::string& result,
			const std::vector<std::pair<std::string, std::string> >& headers);
	};
}

#endif // BINANCE_H

