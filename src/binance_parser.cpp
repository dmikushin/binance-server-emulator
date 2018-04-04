#include "binance_parser.h"

#include <sstream>

using namespace binance;
using namespace std;

void binance::Parser::readValue(const pair<string, string>& arg, string& value, string& result)
{
	if (value != "")
	{
		result = "{\"code\":-1101,\"msg\":\"Duplicate values for a parameter detected.\"}";
		return;
	}
	
	value = arg.second;
}

void binance::Parser::checkSymbol(const string& symbol, string& result)
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

void binance::Parser::checkInterval(const string& interval, string& result)
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

