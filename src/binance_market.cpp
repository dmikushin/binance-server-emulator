#include "binance.h"

#include <cstring>
#include <iterator>
#include <limits>
#include <map>
#include <sstream>

using namespace binance;
using namespace std;

static void readValue(const pair<string, string>& arg, string& value, string& result)
{
	if (value != "")
	{
		result = "{\"code\":-1101,\"msg\":\"Duplicate values for a parameter detected.\"}";
		return;
	}
	
	value = arg.second;
}

static void checkSymbol(const string& symbol, string& result)
{
	while (1)
	{
		if ((symbol.size() < 1) || (symbol.size() > 20)) goto failure;

		for (int i = 0, e = symbol.size(); i != e; i++)
		{
			if ((isupper(symbol[i])) || (isdigit(symbol[i])))
				continue;
	
			goto failure;
		}

		break;

	failure :

		result = "{\"code\":-1100,\"msg\":\"Illegal characters found in parameter 'symbol'; "
			"legal range is '^[A-Z0-9_]{1,20}$'.\"}";
		return;
	}
}

static void checkInterval(const string& interval, string& result)
{
	const static string validIntervals[] =
	{
		"1m", "3m", "5m", "15m", "30m", "1h", "2h", "4h", "6h", "8h", "12h", "1d", "3d", "1w", "1M"
	};

	bool found = false;
	for (int i = 0, e = sizeof(validIntervals) / sizeof(validIntervals[0]); i < e; i++)
	{
		if (interval == validIntervals[i])
		{
			found = true;
			break;
		}
	}

	if (!found)
	{
		result = "{\"code\":-1120,\"msg\":\"Invalid interval.\"}";
		return;
	}
}	

template<typename T>
static void checkInteger(const string& sname, const string& svalue, T& value, string& result)
{
	while (1)
	{
		if ((svalue.size() < 1) || (svalue.size() > 20)) goto failure;

		for (int i = 0, e = svalue.size(); i != e; i++)
		{
			if (isdigit(svalue[i]))
				continue;
	
			goto failure;
		}

		if ((stringstream(svalue) >> value))
			break;

	failure :

		stringstream ss;
		ss << "{\"code\":-1100,\"msg\":\"Illegal characters found in parameter '";
		ss << sname;
		ss << "'; legal range is '^[0-9]{1,20}$'.\"}";
		result = ss.str();
		return;
	}
}

binanceError_t binance::Market::get24hr(string& result, const vector<pair<string, string> >& args)
{
	if (args.size() == 0) return get24hr(result);
	
	string symbol = "";
	
	map<string, string> unknownArgs;
	for (int i = 0, e = args.size(); i != e; i++)
	{
		const pair<string, string>& arg = args[i];

		if (arg.first == "symbol")
		{
			readValue(arg, symbol, result);
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

	if (args.size() != 1)
	{
		stringstream ss;
		ss << "{\"code\":-1101,\"msg\":\"Too many parameters; expected '1' and received '";
		ss << args.size();
		ss << "'.\"}";
		result = ss.str();
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

	checkSymbol(symbol, result);
	if (result != "")
		return binanceSuccess;

	return get24hr(result, symbol.c_str());

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
	string fromId = "";
	string limit = "";
	string startTime = "";
	string endTime = "";

	map<string, string> unknownArgs;
	for (int i = 0, e = args.size(); i != e; i++)
	{
		const pair<string, string>& arg = args[i];

		if (arg.first == "symbol")
		{
			readValue(arg, symbol, result);
			if (result != "")
				return binanceSuccess;
		}	
		else if (arg.first == "fromId")
		{
			readValue(arg, fromId, result);
			if (result != "")
				return binanceSuccess;
		}	
		else if (arg.first == "limit")
		{
			readValue(arg, limit, result);
			if (result != "")
				return binanceSuccess;
		}	
		else if (arg.first == "startTime")
		{
			readValue(arg, startTime, result);
			if (result != "")
				return binanceSuccess;
		}	
		else if (arg.first == "endTime")
		{
			readValue(arg, endTime, result);
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

	if (symbol == "")
	{
		result = "{\"code\":-1102,\"msg\":\"Mandatory parameter 'symbol' was not sent, was empty/null, or malformed.\"}";
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

	checkSymbol(symbol, result);
	if (result != "")
		return binanceSuccess;
	
	if ((fromId != "") && ((startTime == "") && (endTime == "")))
	{
		int fromIdValue;
		checkInteger("fromId", fromId, fromIdValue, result);
		if (result != "")
			return binanceSuccess;		
	
		if (limit == "")
			return getAggTrades(result, symbol.c_str(), fromIdValue);

		int limitValue;
		checkInteger("limit", limit, limitValue, result);
		if (result != "")
			return binanceSuccess;		

		return getAggTrades(result, symbol.c_str(), fromIdValue, limitValue);
	}
	
	if ((fromId == "") && ((startTime != "") && (endTime != "")))
	{
		int startTimeValue;
		checkInteger("startTime", startTime, startTimeValue, result);
		if (result != "")
			return binanceSuccess;

		int endTimeValue;
		checkInteger("endTime", endTime, endTimeValue, result);
		if (result != "")
			return binanceSuccess;

		if (limit == "")
			return getAggTrades(result, symbol.c_str(), startTimeValue, endTimeValue);

		int limitValue;
		checkInteger("limit", limit, limitValue, result);
		if (result != "")
			return binanceSuccess;
	
		return getAggTrades(result, symbol.c_str(), startTimeValue, endTimeValue, limitValue);
	}

	if ((fromId == "") && ((startTime == "") && (endTime == "")))
	{
		int limitValue;
		checkInteger("limit", limit, limitValue, result);
		if (result != "")
			return binanceSuccess;		

		return getAggTrades(result, symbol.c_str(), limitValue);
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
	string limit = "";

	map<string, string> unknownArgs;
	for (int i = 0, e = args.size(); i != e; i++)
	{
		const pair<string, string>& arg = args[i];

		if (arg.first == "symbol")
		{
			readValue(arg, symbol, result);
			if (result != "")
				return binanceSuccess;
		}
		else if (arg.first == "limit")
		{
			readValue(arg, limit, result);
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

	if (symbol == "")
	{
		result = "{\"code\":-1102,\"msg\":\"Mandatory parameter 'symbol' was not sent, was empty/null, or malformed.\"}";
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
	
	checkSymbol(symbol, result);
	if (result != "")
		return binanceSuccess;
	
	if (limit == "")
		return getDepth(result, symbol.c_str());

	int limitValue;
	checkInteger("limit", limit, limitValue, result);
	if (result != "")
		return binanceSuccess;

	return getDepth(result, symbol.c_str(), limitValue);
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
	string limit = "";
	string startTime = "";
	string endTime = "";

	map<string, string> unknownArgs;
	for (int i = 0, e = args.size(); i != e; i++)
	{
		const pair<string, string>& arg = args[i];

		if (arg.first == "symbol")
		{
			readValue(arg, symbol, result);
			if (result != "")
				return binanceSuccess;
		}
		else if (arg.first == "interval")
		{
			readValue(arg, interval, result);
			if (result != "")
				return binanceSuccess;
		}
		else if (arg.first == "limit")
		{
			readValue(arg, limit, result);
			if (result != "")
				return binanceSuccess;
		}	
		else if (arg.first == "startTime")
		{
			readValue(arg, startTime, result);
			if (result != "")
				return binanceSuccess;
		}
		else if (arg.first == "endTime")
		{
			readValue(arg, endTime, result);
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

	checkSymbol(symbol, result);
	if (result != "")
		return binanceSuccess;
	
	if ((interval == "") && (startTime != "") && (endTime != ""))
	{
		int startTimeValue;
		checkInteger("startTime", startTime, startTimeValue, result);
		if (result != "")
			return binanceSuccess;

		int endTimeValue;
		checkInteger("endTime", endTime, endTimeValue, result);
		if (result != "")
			return binanceSuccess;

		if (limit == "")
			return getKlines(result, symbol.c_str(), interval.c_str(), startTimeValue, endTimeValue);

		int limitValue;
		checkInteger("limit", limit, limitValue, result);
		if (result != "")
			return binanceSuccess;
	
		return getKlines(result, symbol.c_str(), interval.c_str(), startTimeValue, endTimeValue, limitValue);
	}
	
	if ((interval != "") && (startTime == "") && (endTime == ""))
	{
		checkInterval(interval, result);
		if (result != "")
			return binanceSuccess;

		if (limit == "")
			return getKlines(result, symbol.c_str(), interval.c_str());

		int limitValue;
		checkInteger("limit", limit, limitValue, result);
		if (result != "")
			return binanceSuccess;

		return getKlines(result, symbol.c_str(), interval.c_str(), limitValue);
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

