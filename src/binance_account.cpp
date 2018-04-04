#include "binance.h"
#include "binance_parser.h"
#include "binance_server.h"

#include <map>

using namespace binance;
using namespace std;

binanceError_t binance::Account::getInfo(string& result, const vector<pair<string, string> >& args,
	const vector<pair<string, string> >& headers)
{
	string signature = "";
	string timestamp = "";
	string recvWindow = "";

	map<string, string> unknownArgs;
	for (int i = 0, e = args.size(); i != e; i++)
	{
		const pair<string, string>& arg = args[i];

		if (arg.first == "timestamp")
		{
			Parser::readValue(arg, timestamp, result);
			if (result != "")
				return binanceSuccess;
		}	
		else if (arg.first == "recvWindow")
		{
			Parser::readValue(arg, recvWindow, result);
			if (result != "")
				return binanceSuccess;
		}
		else if (arg.first == "signature")
		{
			Parser::readValue(arg, signature, result);
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

	if (timestamp == "")
	{
		result = "{\"code\":-1102,\"msg\":\"Mandatory parameter 'timestamp' was not sent, was empty/null, or malformed.\"}";
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

	long timestampValue;
	Parser::parsePositiveInteger("timestamp", timestamp, timestampValue, result);
	if (result != "")
		return binanceSuccess;
	
	if (recvWindow == "")
		return getInfo(result, timestampValue);

	int recvWindowValue;
	Parser::parsePositiveInteger("recvWindow", recvWindow, recvWindowValue, result);
	if (result != "")
		return binanceSuccess;		

	return getInfo(result, timestampValue, recvWindowValue);
}

// Get current account information. (SIGNED)
//
// GET /api/v3/account
//
// Parameters:
// Name		Type	Mandatory	Description
// recvWindow	LONG	NO	
// timestamp	LONG	YES
//
binanceError_t binance::Account::getInfo(std::string& result, long timestamp, long recvWindow) 
{
	if (timestamp - 1000 > Server::getTime())
	{
		result = "{\"code\":-1021,\"msg\":\"Timestamp for this request was 1000ms ahead of the server's time.\"}";
		return binanceSuccess;
	}

	return binanceSuccess;
}

binanceError_t binance::Account::getAllOrders(string& result, const vector<pair<string, string> >& args,
	const vector<pair<string, string> >& headers)
{
	string signature = "";
	string symbol = "";
	string timestamp = "";
	string orderId = "";
	string limit = "";
	string recvWindow = "";

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
		else if (arg.first == "timestamp")
		{
			Parser::readValue(arg, timestamp, result);
			if (result != "")
				return binanceSuccess;
		}	
		else if (arg.first == "orderId")
		{
			Parser::readValue(arg, orderId, result);
			if (result != "")
				return binanceSuccess;
		}	
		else if (arg.first == "limit")
		{
			Parser::readValue(arg, limit, result);
			if (result != "")
				return binanceSuccess;
		}	
		else if (arg.first == "recvWindow")
		{
			Parser::readValue(arg, recvWindow, result);
			if (result != "")
				return binanceSuccess;
		}	
		else if (arg.first == "signature")
		{
			Parser::readValue(arg, signature, result);
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

	if (timestamp == "")
	{
		result = "{\"code\":-1102,\"msg\":\"Mandatory parameter 'timestamp' was not sent, was empty/null, or malformed.\"}";
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

	long timestampValue;
	Parser::parsePositiveInteger("timestamp", timestamp, timestampValue, result);
	if (result != "")
		return binanceSuccess;

	long orderIdValue = 0;
	int limitValue = 0;
	long recvWindowValue = 0;
	
	if (orderId != "")
	{
		Parser::parsePositiveInteger("orderId", orderId, orderIdValue, result);
		if (result != "")
			return binanceSuccess;		
	}
	
	if (limit != "")
	{
		Parser::parsePositiveInteger("limit", limit, limitValue, result);
		if (result != "")
			return binanceSuccess;		
	}
	
	if (recvWindow != "")
	{
		Parser::parsePositiveInteger("recvWindow", recvWindow, recvWindowValue, result);
		if (result != "")
			return binanceSuccess;
	}

	return getAllOrders(result, symbol.c_str(), timestampValue, orderIdValue, limitValue, recvWindowValue);
}

// All Orders (SIGNED)
//
// GET /api/v3/allOrders
//
// Name		Type	Mandatory	Description
// symbol		STRING	YES	
// orderId		LONG	NO	
// limit		INT		NO		Default 500; max 500.
// recvWindow	LONG	NO	
// timestamp	LONG	YES	
//
binanceError_t binance::Account::getAllOrders(std::string& result, const char *symbol, long timestamp,
	long orderId, int limit, long recvWindow) 
{
	return binanceSuccess;
}

binanceError_t binance::Account::getHistoricalTrades(string& result, const vector<pair<string, string> >& args,
	const vector<pair<string, string> >& headers)
{
	string signature = "";
	string symbol = "";
	string fromId = "";
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
		else if (arg.first == "signature")
		{
			Parser::readValue(arg, signature, result);
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

	int fromIdValue = -1;
	int limitValue = 500;
	
	if (fromId != "")
	{
		Parser::parsePositiveInteger("fromId", fromId, fromIdValue, result);
		if (result != "")
			return binanceSuccess;		
	}
		
	if (limit != "")
	{
		Parser::parsePositiveInteger("limit", limit, limitValue, result);
		if (result != "")
			return binanceSuccess;		
	}

	return getHistoricalTrades(result, symbol.c_str(), fromIdValue, limitValue);
}

// Old trade lookup (MARKET_DATA)
//
// GET /api/v1/historicalTrades
//
// Name	Type	Mandatory	Description
// symbol	STRING	YES	
// limit	INT	NO	Default 500; max 500.
// fromId	LONG	NO	TradeId to fetch from. Default gets most recent trades.
//
binanceError_t binance::Account::getHistoricalTrades(std::string& result, const char *symbol, long fromId, int limit)
{
	return binanceSuccess;
}

binanceError_t binance::Account::getTrades(string& result, const vector<pair<string, string> >& args,
	const vector<pair<string, string> >& headers)
{
	string signature = "";
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
		else if (arg.first == "signature")
		{
			Parser::readValue(arg, signature, result);
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

	if (limit != "")
	{
		int limitValue;
		Parser::parsePositiveInteger("limit", limit, limitValue, result);
		if (result != "")
			return binanceSuccess;		

		return getTrades(result, symbol.c_str(), limitValue);
	}
	
	return getTrades(result, symbol.c_str());
}

// Recent trades list
//
// GET /api/v1/trades
// Get recent trades (up to last 500).
//
// Name	Type	Mandatory	Description
// symbol	STRING	YES	
// limit	INT	NO	Default 500; max 500.
//
binanceError_t binance::Account::getTrades(std::string& result, const char *symbol, int limit)
{
	return binanceSuccess;
}

binanceError_t binance::Account::getTradesSigned(string& result, const vector<pair<string, string> >& args,
	const vector<pair<string, string> >& headers)
{
	string signature = "";
	string symbol = "";
	string timestamp = "";
	string fromId = "";
	string limit = "";
	string recvWindow = "";

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
		else if (arg.first == "timestamp")
		{
			Parser::readValue(arg, timestamp, result);
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
		else if (arg.first == "recvWindow")
		{
			Parser::readValue(arg, recvWindow, result);
			if (result != "")
				return binanceSuccess;
		}	
		else if (arg.first == "signature")
		{
			Parser::readValue(arg, signature, result);
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

	if (timestamp == "")
	{
		result = "{\"code\":-1102,\"msg\":\"Mandatory parameter 'timestamp' was not sent, was empty/null, or malformed.\"}";
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

	long timestampValue;
	Parser::parsePositiveInteger("timestamp", timestamp, timestampValue, result);
	if (result != "")
		return binanceSuccess;		

	long fromIdValue = 0;
	int limitValue = 0;
	long recvWindowValue = 0;
	
	if (fromId != "")
	{
		Parser::parsePositiveInteger("fromId", fromId, fromIdValue, result);
		if (result != "")
			return binanceSuccess;		
	}
	
	if (limit != "")
	{
		Parser::parsePositiveInteger("limit", limit, limitValue, result);
		if (result != "")
			return binanceSuccess;		
	}
	
	if (recvWindow != "")
	{
		Parser::parsePositiveInteger("recvWindow", recvWindow, recvWindowValue, result);
		if (result != "")
			return binanceSuccess;
	}

	return getTradesSigned(result, symbol.c_str(), timestampValue, fromIdValue, limitValue, recvWindowValue);
}

// Get trades for a specific account and symbol. (SIGNED)
//
// GET /api/v3/myTrades
// Name		Type	Mandatory	Description
// symbol		STRING	YES	
// limit		INT		NO	Default 500; max 500.
// fromId		LONG	NO	TradeId to fetch from. Default gets most recent trades.
// recvWindow	LONG	NO	
// timestamp	LONG	YES
//
binanceError_t binance::Account::getTradesSigned(std::string& result, const char *symbol, long timestamp, long fromId, long recvWindow, int limit)
{
	return binanceSuccess;
}

binanceError_t binance::Account::getOpenOrders(string& result, const vector<pair<string, string> >& args,
	const vector<pair<string, string> >& headers)
{
	string signature = "";
	string timestamp = "";
	string recvWindow = "";

	map<string, string> unknownArgs;
	for (int i = 0, e = args.size(); i != e; i++)
	{
		const pair<string, string>& arg = args[i];

		if (arg.first == "timestamp")
		{
			Parser::readValue(arg, timestamp, result);
			if (result != "")
				return binanceSuccess;
		}	
		else if (arg.first == "recvWindow")
		{
			Parser::readValue(arg, recvWindow, result);
			if (result != "")
				return binanceSuccess;
		}	
		else if (arg.first == "signature")
		{
			Parser::readValue(arg, signature, result);
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

	if (timestamp == "")
	{
		result = "{\"code\":-1102,\"msg\":\"Mandatory parameter 'timestamp' was not sent, was empty/null, or malformed.\"}";
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

	long timestampValue;
	Parser::parsePositiveInteger("timestamp", timestamp, timestampValue, result);
	if (result != "")
		return binanceSuccess;
	
	if (recvWindow == "")
		return getOpenOrders(result, timestampValue);

	int recvWindowValue;
	Parser::parsePositiveInteger("recvWindow", recvWindow, recvWindowValue, result);
	if (result != "")
		return binanceSuccess;		

	return getOpenOrders(result, timestampValue, recvWindowValue);
}

// Open Orders
//
// GET /api/v3/openOrders
//
// Name		Type	Mandatory	Description
// recvWindow	LONG	NO	
// timestamp	LONG	YES	
//
binanceError_t binance::Account::getOpenOrders(std::string& result, long timestamp, long recvWindow) 
{
	return binanceSuccess;
}

binanceError_t binance::Account::getOpenOrdersSigned(string& result, const vector<pair<string, string> >& args,
	const vector<pair<string, string> >& headers)
{
	string signature = "";
	string symbol = "";
	string timestamp = "";
	string orderId = "";
	string limit = "";
	string recvWindow = "";

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
		else if (arg.first == "timestamp")
		{
			Parser::readValue(arg, timestamp, result);
			if (result != "")
				return binanceSuccess;
		}	
		else if (arg.first == "orderId")
		{
			Parser::readValue(arg, orderId, result);
			if (result != "")
				return binanceSuccess;
		}	
		else if (arg.first == "limit")
		{
			Parser::readValue(arg, limit, result);
			if (result != "")
				return binanceSuccess;
		}	
		else if (arg.first == "recvWindow")
		{
			Parser::readValue(arg, recvWindow, result);
			if (result != "")
				return binanceSuccess;
		}	
		else if (arg.first == "signature")
		{
			Parser::readValue(arg, signature, result);
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

	if (timestamp == "")
	{
		result = "{\"code\":-1102,\"msg\":\"Mandatory parameter 'timestamp' was not sent, was empty/null, or malformed.\"}";
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

	long timestampValue;
	Parser::parsePositiveInteger("timestamp", timestamp, timestampValue, result);
	if (result != "")
		return binanceSuccess;

	if (recvWindow == "")
		return getOpenOrdersSigned(result, symbol.c_str(), timestampValue);

	long recvWindowValue = 0;
	Parser::parsePositiveInteger("recvWindow", recvWindow, recvWindowValue, result);
	if (result != "")
		return binanceSuccess;

	return getOpenOrdersSigned(result, symbol.c_str(), timestampValue, recvWindowValue);
}

// Open Orders (SIGNED)
//
// GET /api/v3/openOrders
//
// Name		Type	Mandatory	Description
// symbol		STRING	YES	
// recvWindow	LONG	NO	
// timestamp	LONG	YES	
//
binanceError_t binance::Account::getOpenOrdersSigned(std::string& result, const char *symbol, long timestamp, long recvWindow) 
{
	return binanceSuccess;
}

binanceError_t binance::Account::sendOrder(string& result, const vector<pair<string, string> >& args,
	const vector<pair<string, string> >& headers)
{
	return binanceSuccess;
}

// Send order (SIGNED)
//
// POST /api/v3/order
//
// Name				Type		Mandatory	Description
// symbol				STRING		YES	
// side				ENUM		YES	
// type				ENUM		YES	
// timeInForce			ENUM		YES	
// quantity			DECIMAL		YES	
// price				DECIMAL		YES	
// newClientOrderId		STRING		NO		A unique id for the order. Automatically generated by default.
// stopPrice			DECIMAL		NO		Used with STOP orders
// icebergQty			DECIMAL		NO		Used with icebergOrders
// recvWindow			LONG		NO	
// timestamp			LONG		YES	
//
binanceError_t binance::Account::sendOrder(std::string& result, const char *symbol,
	const char *side, const char *type, const char *timeInForce, double quantity, double price,
	const char *newClientOrderId, double stopPrice, double icebergQty, long recvWindow) 
{
	return binanceSuccess;
}

binanceError_t binance::Account::startUserDataStream(string& result, const vector<pair<string, string> >& headers)
{
	return binanceSuccess;
}

