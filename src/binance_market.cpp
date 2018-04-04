#include "binance.h"
#include "binance_parser.h"

#include <map>

using namespace binance;
using namespace std;

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
			Parser::readValue(arg, symbol, result);
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

	Parser::checkSymbol(symbol, result);
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
			Parser::readValue(arg, symbol, result);
			if (result != "")
				return binanceSuccess;
		}	
		else if (arg.first == "fromId")
		{
			Parser::readValue(arg, fromId, result);
			if (result != "")
				return binanceSuccess;
		}	
		else if (arg.first == "limit")
		{
			Parser::readValue(arg, limit, result);
			if (result != "")
				return binanceSuccess;
		}	
		else if (arg.first == "startTime")
		{
			Parser::readValue(arg, startTime, result);
			if (result != "")
				return binanceSuccess;
		}	
		else if (arg.first == "endTime")
		{
			Parser::readValue(arg, endTime, result);
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

	Parser::checkSymbol(symbol, result);
	if (result != "")
		return binanceSuccess;
	
	if ((fromId != "") && ((startTime == "") && (endTime == "")))
	{
		int fromIdValue;
		Parser::parsePositiveInteger("fromId", fromId, fromIdValue, result);
		if (result != "")
			return binanceSuccess;		
	
		if (limit == "")
			return getAggTrades(result, symbol.c_str(), fromIdValue);

		int limitValue;
		Parser::parsePositiveInteger("limit", limit, limitValue, result);
		if (result != "")
			return binanceSuccess;		

		return getAggTrades(result, symbol.c_str(), fromIdValue, limitValue);
	}
	
	if ((fromId == "") && ((startTime != "") && (endTime != "")))
	{
		int startTimeValue;
		Parser::parsePositiveInteger("startTime", startTime, startTimeValue, result);
		if (result != "")
			return binanceSuccess;

		int endTimeValue;
		Parser::parsePositiveInteger("endTime", endTime, endTimeValue, result);
		if (result != "")
			return binanceSuccess;

		if (limit == "")
			return getAggTrades(result, symbol.c_str(), startTimeValue, endTimeValue);

		int limitValue;
		Parser::parsePositiveInteger("limit", limit, limitValue, result);
		if (result != "")
			return binanceSuccess;
	
		return getAggTrades(result, symbol.c_str(), startTimeValue, endTimeValue, limitValue);
	}

	if ((fromId == "") && ((startTime == "") && (endTime == "")))
	{
		int limitValue;
		Parser::parsePositiveInteger("limit", limit, limitValue, result);
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
	string symbol = "";
	string limit = "";

	map<string, string> unknownArgs;
	for (int i = 0, e = args.size(); i != e; i++)
	{
		const pair<string, string>& arg = args[i];

		if (arg.first == "symbol")
		{
			Parser::readValue(arg, symbol, result);
			if (result != "")
				return binanceSuccess;
		}
		else if (arg.first == "limit")
		{
			Parser::readValue(arg, limit, result);
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
	
	Parser::checkSymbol(symbol, result);
	if (result != "")
		return binanceSuccess;
	
	if (limit == "")
		return getDepth(result, symbol.c_str());

	int limitValue;
	Parser::parsePositiveInteger("limit", limit, limitValue, result);
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
			Parser::readValue(arg, symbol, result);
			if (result != "")
				return binanceSuccess;
		}
		else if (arg.first == "interval")
		{
			Parser::readValue(arg, interval, result);
			if (result != "")
				return binanceSuccess;
		}
		else if (arg.first == "limit")
		{
			Parser::readValue(arg, limit, result);
			if (result != "")
				return binanceSuccess;
		}	
		else if (arg.first == "startTime")
		{
			Parser::readValue(arg, startTime, result);
			if (result != "")
				return binanceSuccess;
		}
		else if (arg.first == "endTime")
		{
			Parser::readValue(arg, endTime, result);
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

	Parser::checkSymbol(symbol, result);
	if (result != "")
		return binanceSuccess;
	
	if ((interval == "") && (startTime != "") && (endTime != ""))
	{
		int startTimeValue;
		Parser::parsePositiveInteger("startTime", startTime, startTimeValue, result);
		if (result != "")
			return binanceSuccess;

		int endTimeValue;
		Parser::parsePositiveInteger("endTime", endTime, endTimeValue, result);
		if (result != "")
			return binanceSuccess;

		if (limit == "")
			return getKlines(result, symbol.c_str(), interval.c_str(), startTimeValue, endTimeValue);

		int limitValue;
		Parser::parsePositiveInteger("limit", limit, limitValue, result);
		if (result != "")
			return binanceSuccess;
	
		return getKlines(result, symbol.c_str(), interval.c_str(), startTimeValue, endTimeValue, limitValue);
	}
	
	if ((interval != "") && (startTime == "") && (endTime == ""))
	{
		Parser::checkInterval(interval, result);
		if (result != "")
			return binanceSuccess;

		if (limit == "")
			return getKlines(result, symbol.c_str(), interval.c_str());

		int limitValue;
		Parser::parsePositiveInteger("limit", limit, limitValue, result);
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

