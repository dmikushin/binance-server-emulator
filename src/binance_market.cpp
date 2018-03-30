#include "binance.h"

#include <cstring>
#include <iterator>
#include <limits>
#include <sstream>

using namespace binance;
using namespace std;

static bool startsWith(const string& what, const string& with)
{
	return (what.compare(0, with.length(), with) == 0);
}

binanceError_t binance::Market::get24hr(string& result, const vector<pair<string, string> >& args)
{
	if (args.size() == 0) return get24hr(result);
	
	if (args.size() != 1)
	{
		stringstream ss;
		ss << "{\"code\":-1101,\"msg\":\"Too many parameters; expected '1' and received '";
		ss << args.size();
		ss << "'.\"}";
		result = ss.str();
		return binanceSuccess;
	}
	
	if (args[0].first == "symbol")
	{
		return get24hr(result, args[0].second.c_str());
	}
	else
	{
		result = "{\"code\":-1104,\"msg\":\"Not all sent parameters were read; read '0' parameter(s) but was sent '1'.\"}";
		return binanceSuccess;
	}

	return binanceSuccess;
}

// Get 24hr ticker price change statistics
//
// Name	Type	Mandatory	Description
// symbol	STRING	YES
//
binanceError_t binance::Market::get24hr(string& result)
{
	return binanceSuccess;
}

// Get 24hr ticker price change statistics
//
// Name	Type	Mandatory	Description
// symbol	STRING	YES
//
binanceError_t binance::Market::get24hr(string& result, const char *symbol)
{
	return binanceSuccess;
}

binanceError_t binance::Market::getAggTrades(string& result, const vector<pair<string, string> >& args)
{
	if (args.size() == 0)
	{
		result = "{\"code\":-1102,\"msg\":\"Mandatory parameter 'symbol' was not sent, was empty/null, or malformed.\"}";
		return binanceSuccess;
	}

	string symbol = "";
	int fromId = -1;
	int limit = -1;
	time_t startTime = numeric_limits<time_t>::max();
	time_t endTime = numeric_limits<time_t>::max();

	for (int i = 0, e = args.size(); i != e; i++)
	{
		const pair<string, string> arg = args[i];
	
		if (arg.first == "symbol")
		{
			if (symbol != "")
			{
				result = "{\"code\":-1101,\"msg\":\"Duplicate values for a parameter detected.\"}";
				return binanceSuccess;
			}
			
			symbol = arg.second; 
		}
		else if (arg.first == "fromId")
		{
			if (fromId != -1)
			{
				result = "{\"code\":-1101,\"msg\":\"Duplicate values for a parameter detected.\"}";
				return binanceSuccess;
			}

			if (!(stringstream(arg.second) >> fromId))
			{
				result = "{\"code\":-1100,\"msg\":\"Illegal characters found in parameter 'fromId'; "
					" legal range is '^[0-9]{1,20}$'.\"}";
				return binanceSuccess;
			}
		}
		else if (arg.first == "limit")
		{
			if (limit != -1)
			{
				result = "{\"code\":-1101,\"msg\":\"Duplicate values for a parameter detected.\"}";
				return binanceSuccess;
			}

			if (!(stringstream(arg.second) >> limit))
			{
				result = "{\"code\":-1100,\"msg\":\"Illegal characters found in parameter 'limit'; "
					" legal range is '^[0-9]{1,20}$'.\"}";
				return binanceSuccess;
			}
		}
		else if (arg.first == "startTime")
		{
			if (startTime != numeric_limits<time_t>::max())
			{
				result = "{\"code\":-1101,\"msg\":\"Duplicate values for a parameter detected.\"}";
				return binanceSuccess;
			}

			if (!(stringstream(arg.second) >> startTime))
			{
				result = "{\"code\":-1100,\"msg\":\"Illegal characters found in parameter 'startTime'; "
					" legal range is '^[0-9]{1,20}$'.\"}";
				return binanceSuccess;
			}
		}
		else if (arg.first == "endTime")
		{
			if (endTime != numeric_limits<time_t>::max())
			{
				result = "{\"code\":-1101,\"msg\":\"Duplicate values for a parameter detected.\"}";
				return binanceSuccess;
			}

			if (!(stringstream(arg.second) >> endTime))
			{
				result = "{\"code\":-1100,\"msg\":\"Illegal characters found in parameter 'endTime'; "
					" legal range is '^[0-9]{1,20}$'.\"}";
				return binanceSuccess;
			}
		}
		else
		{
			result = "{\"code\":-1104,\"msg\":\"Not all sent parameters were read; read '0' parameter(s) but was sent '1'.\"}";
			return binanceSuccess;
		}
	}

	if (symbol == "")
	{
		result = "{\"code\":-1102,\"msg\":\"Mandatory parameter 'symbol' was not sent, was empty/null, or malformed.\"}";
		return binanceSuccess;
	}
	
	if ((fromId != -1) && ((startTime == numeric_limits<time_t>::max()) && (endTime == numeric_limits<time_t>::max())))
	{
		if (limit == -1)
			return getAggTrades(result, symbol.c_str(), fromId);

		return getAggTrades(result, symbol.c_str(), fromId, limit);
	}
	
	if ((fromId == -1) && ((startTime != numeric_limits<time_t>::max()) && (endTime != numeric_limits<time_t>::max())))
	{
		if (limit == -1)
			return getAggTrades(result, symbol.c_str(), startTime, endTime);
	
		return getAggTrades(result, symbol.c_str(), startTime, endTime, limit);
	}

	result = "{\"code\":-1128,\"msg\":\"Combination of optional parameters invalid.\"}";
	return binanceSuccess;
}

// Get Aggregated Trades list
//
// GET /api/v1/aggTrades
//
// Name		Type	Mandatory	Description
// symbol		STRING	YES
// fromId		LONG	NO		ID to get aggregate trades from INCLUSIVE.
// startTime	LONG	NO		Timestamp in ms to get aggregate trades from INCLUSIVE.
// endTime		LONG	NO		Timestamp in ms to get aggregate trades until INCLUSIVE.
// limit		INT	NO		Default 500;max 500.
//
binanceError_t binance::Market::getAggTrades(string& result, const char *symbol, int fromId, int limit)
{
	return binanceSuccess;
}

// Get Aggregated Trades list
//
// GET /api/v1/aggTrades
//
// Name		Type	Mandatory	Description
// symbol		STRING	YES
// fromId		LONG	NO		ID to get aggregate trades from INCLUSIVE.
// startTime	LONG	NO		Timestamp in ms to get aggregate trades from INCLUSIVE.
// endTime		LONG	NO		Timestamp in ms to get aggregate trades until INCLUSIVE.
// limit		INT	NO		Default 500;max 500.
//
binanceError_t binance::Market::getAggTrades(string& result, const char *symbol, time_t startTime, time_t endTime, int limit)
{
	return binanceSuccess;
}

// Get Best price/qty on the order book for all symbols.
// GET /api/v1/ticker/allBookTickers
binanceError_t binance::Market::getAllBookTickers(string& result)
{
	return binanceSuccess;
}

// Get Latest price for all symbols.
// GET /api/v1/ticker/allPrices
binanceError_t binance::Market::getAllPrices(string& result)
{
	return binanceSuccess;
}

// Get Best price/qty on the order book for all symbols.
// GET /api/v1/ticker/allBookTickers
binanceError_t binance::Market::getDepth(string& result, const vector<pair<string, string> >& args)
{
	if (args.size() == 0)
	{
		result = "{\"code\":-1102,\"msg\":\"Mandatory parameter 'symbol' was not sent, was empty/null, or malformed.\"}";
		return binanceSuccess;
	}

	string symbol = "";
	int limit = -1;

	for (int i = 0, e = args.size(); i != e; i++)
	{
		const pair<string, string> arg = args[i];
	
		if (arg.first == "symbol")
		{
			if (symbol != "")
			{
				result = "{\"code\":-1101,\"msg\":\"Duplicate values for a parameter detected.\"}";
				return binanceSuccess;
			}
			
			symbol = arg.second; 
		}
		else if (arg.first == "limit")
		{
			if (limit != -1)
			{
				result = "{\"code\":-1101,\"msg\":\"Duplicate values for a parameter detected.\"}";
				return binanceSuccess;
			}

			if (!(stringstream(arg.second) >> limit))
			{
				result = "{\"code\":-1100,\"msg\":\"Illegal characters found in parameter 'limit'; "
					" legal range is '^[0-9]{1,20}$'.\"}";
				return binanceSuccess;
			}
		}
		else
		{
			result = "{\"code\":-1104,\"msg\":\"Not all sent parameters were read; read '0' parameter(s) but was sent '1'.\"}";
			return binanceSuccess;
		}
	}

	if (symbol == "")
	{
		result = "{\"code\":-1102,\"msg\":\"Mandatory parameter 'symbol' was not sent, was empty/null, or malformed.\"}";
		return binanceSuccess;
	}
	
	if (limit == -1)
		return getDepth(result, symbol.c_str());

	return getDepth(result, symbol.c_str(), limit);
}

// Get Market Depth
//
// GET /api/v1/depth
//
// Name	Type		Mandatory	Description
// symbol	STRING		YES
// limit	INT		NO		Default 100;max 100.
//
binanceError_t binance::Market::getDepth(string& result, const char *symbol, int limit)
{
	return binanceSuccess;
}

binanceError_t binance::Market::getKlines(string& result, const vector<pair<string, string> >& args)
{
	if (args.size() == 0)
	{
		result = "{\"code\":-1102,\"msg\":\"Mandatory parameter 'symbol' was not sent, was empty/null, or malformed.\"}";
		return binanceSuccess;
	}

	string symbol = "";
	string interval = "";
	int limit = -1;
	time_t startTime = numeric_limits<time_t>::max();
	time_t endTime = numeric_limits<time_t>::max();

	for (int i = 0, e = args.size(); i != e; i++)
	{
		const pair<string, string> arg = args[i];
	
		if (arg.first == "symbol")
		{
			if (symbol != "")
			{
				result = "{\"code\":-1101,\"msg\":\"Duplicate values for a parameter detected.\"}";
				return binanceSuccess;
			}
			
			symbol = arg.second; 
		}
		else if (arg.first == "interval")
		{
			if (interval != "")
			{
				result = "{\"code\":-1101,\"msg\":\"Duplicate values for a parameter detected.\"}";
				return binanceSuccess;
			}
			
			interval = arg.second; 
		}
		else if (arg.first == "limit")
		{
			if (limit != -1)
			{
				result = "{\"code\":-1101,\"msg\":\"Duplicate values for a parameter detected.\"}";
				return binanceSuccess;
			}

			if (!(stringstream(arg.second) >> limit))
			{
				result = "{\"code\":-1100,\"msg\":\"Illegal characters found in parameter 'limit'; "
					" legal range is '^[0-9]{1,20}$'.\"}";
				return binanceSuccess;
			}
		}
		else if (arg.first == "startTime")
		{
			if (startTime != numeric_limits<time_t>::max())
			{
				result = "{\"code\":-1101,\"msg\":\"Duplicate values for a parameter detected.\"}";
				return binanceSuccess;
			}

			if (!(stringstream(arg.second) >> startTime))
			{
				result = "{\"code\":-1100,\"msg\":\"Illegal characters found in parameter 'startTime'; "
					" legal range is '^[0-9]{1,20}$'.\"}";
				return binanceSuccess;
			}
		}
		else if (arg.first == "endTime")
		{
			if (endTime != numeric_limits<time_t>::max())
			{
				result = "{\"code\":-1101,\"msg\":\"Duplicate values for a parameter detected.\"}";
				return binanceSuccess;
			}

			if (!(stringstream(arg.second) >> endTime))
			{
				result = "{\"code\":-1100,\"msg\":\"Illegal characters found in parameter 'endTime'; "
					" legal range is '^[0-9]{1,20}$'.\"}";
				return binanceSuccess;
			}
		}
		else
		{
			result = "{\"code\":-1104,\"msg\":\"Not all sent parameters were read; read '0' parameter(s) but was sent '1'.\"}";
			return binanceSuccess;
		}
	}

	if (symbol == "")
	{
		result = "{\"code\":-1102,\"msg\":\"Mandatory parameter 'symbol' was not sent, was empty/null, or malformed.\"}";
		return binanceSuccess;
	}

	if (interval == "")
	{
		result = "{\"code\":-1102,\"msg\":\"Mandatory parameter 'interval' was not sent, was empty/null, or malformed.\"}";
		return binanceSuccess;
	}
	
	if ((startTime != numeric_limits<time_t>::max()) && (endTime != numeric_limits<time_t>::max()))
	{
		if (limit == -1)
			return getKlines(result, symbol.c_str(), interval.c_str(), startTime, endTime);
	
		return getKlines(result, symbol.c_str(), interval.c_str(), startTime, endTime, limit);
	}
	
	if ((startTime == numeric_limits<time_t>::max()) && (endTime == numeric_limits<time_t>::max()))
	{
		if (limit == -1)
			return getKlines(result, symbol.c_str(), interval.c_str());

		return getKlines(result, symbol.c_str(), interval.c_str(), limit);
	}

	result = "{\"code\":-1128,\"msg\":\"Combination of optional parameters invalid.\"}";
	return binanceSuccess;
}

// Get KLines(Candle stick / OHLC)
//
// GET /api/v1/klines
//
// Name		Type	Mandatory	Description
// symbol		STRING	YES
// interval	ENUM	YES
// startTime	LONG	NO
// endTime		LONG	NO
// limit		INT		NO	Default 500;max 500.
//
binanceError_t binance::Market::getKlines(string& result, const char *symbol, const char *interval,
	time_t startTime, time_t endTime, int limit)
{
	return binanceSuccess;
}

