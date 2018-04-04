#ifndef BINANCE_PARSER_H
#define BINANCE_PARSER_H

#include "binance.h"

#include <sstream>
#include <string>
#include <utility>

namespace binance
{
	class Parser
	{
	public :
	
		static void readValue(const std::pair<std::string, std::string>& arg, std::string& value, std::string& result);
		static void checkSymbol(const std::string& symbol, std::string& result);
		static void checkInterval(const std::string& interval, std::string& result);

		template<typename T>
		static void parseInteger(const std::string& sname, const std::string& svalue, T& value, std::string& result)
		{
			while (1)
			{
				if ((svalue.size() < 1) || (svalue.size() > 20)) goto failure;

				for (int i = 0, e = svalue.size(); i != e; i++)
				{
					if (isdigit(svalue[i]))
						continue;
					
					if (svalue[i] == '-')
						continue;
	
					goto failure;
				}

				if ((std::stringstream(svalue) >> value))
					break;

			failure :

				std::stringstream ss;
				ss << "{\"code\":-1100,\"msg\":\"Illegal characters found in parameter '";
				ss << sname;
				ss << "'; legal range is '^[0-9]{1,20}$'.\"}";
				result = ss.str();
				return;
			}
		}

		template<typename T>
		static void parsePositiveInteger(const std::string& sname, const std::string& svalue, T& value, std::string& result)
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

				if ((std::stringstream(svalue) >> value))
					break;

			failure :

				std::stringstream ss;
				ss << "{\"code\":-1100,\"msg\":\"Illegal characters found in parameter '";
				ss << sname;
				ss << "'; legal range is '^[0-9]{1,20}$'.\"}";
				result = ss.str();
				return;
			}
		}

		template<typename T>
		static void parseFloatingPoint(const std::string& sname, const std::string& svalue, T& value, std::string& result)
		{
			while (1)
			{
				if ((std::stringstream(svalue) >> value))
					break;

				std::stringstream ss;
				ss << "{\"code\":-1100,\"msg\":\"Illegal characters found in parameter '";
				ss << sname;
				ss << "'; legal range is '^[0-9]{1,20}$'.\"}";
				result = ss.str();
				return;
			}
		}
	};
}

#endif // BINANCE_PARSER_H

