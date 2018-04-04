#ifndef BINANCE_SERVER_H
#define BINANCE_SERVER_H

#include <memory>
#include <string>
#include <vector>
#include <utility>

namespace binance
{
	class Server
	{
		// The initial time is the initial time setting of server,
		// that could be real or simulated (in the past or in the future).
		uint64_t initial;
		
		// The real time in the real Earth time corresponding to
		// the server initial time at the moment of time setting.
		uint64_t real;
		
		// The time scale denotes how fast the server time is running,
		// i.e. slower or faster than real Earth time.
		double scale;
		
		static std::unique_ptr<Server> server;

	public :
	
		Server();
	
		static uint64_t getTime();
		static void setTime(const uint64_t initial, const double scale = 1.0);

		static binanceError_t getTime(std::string& result);
		static binanceError_t setTime(std::string& result, const std::vector<std::pair<std::string, std::string> >& args);

		static binanceError_t getCurl(std::string &result_json, const std::string& url);

		static binanceError_t getCurlWithHeader(std::string& result_json, const std::string& url,
			const std::vector<std::string>& extra_http_header, const std::string& post_data, const std::string& action);
	};
}

#endif // BINANCE_SERVER_H

