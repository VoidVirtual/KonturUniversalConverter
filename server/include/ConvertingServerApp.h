/** Created by D.Kabzhan, 30.04.2021 */
#pragma once
#include "Poco/Net/HTTPServer.h"
#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"
#include "Poco/Net/ServerSocket.h"
#include "Poco/Util/ServerApplication.h"
#include "algorithm/AccurateMultiplier.h"
#include "converter/Converter.h"
#include "ConvertingHandler.h"
template<class T>
    using OptionCallback = Poco::Util::OptionCallback<T>;
using Option = Poco::Util::Option;
using OptionSet = Poco::Util::OptionSet;
using HTTPServer = Poco::Net::HTTPServer;
using HTTPServerParams = Poco::Net::HTTPServerParams;
using ServerSocket = Poco::Net::ServerSocket;
using ServerApplication = Poco::Util::ServerApplication;

/**
 *  Starts a server on port 80 by default
 *  Requires a --rules option for conversion rules file path
 *  gets a POST request with a single json in the request body
 *  Responce would be the result of corresponding conversion, or "404 Not Found"
 *  "400 Bad Request" is send if required quantites don't exist, or provided data isn't a json,
 *  or if it doestn't contain "from" or "to" keys.
 *  If a request method isn't "POST" or URI path is not "/converter", 501 "Not Implemented is returned"
 *  
 */
class ConvertingServerApp : public ServerApplication
{
public:
    ConvertingServerApp() {
        setUnixOptions(true); // follows from the task conditions
    }
protected:
    int main(std::vector<std::string> const&);
    void defineOptions(OptionSet& options);
    void handleRulesFile(std::string const&, std::string const& value);
    void handleSetPort(std::string const&, std::string const& value);
private:
    std::string rulesFilePath_;
    uint16_t port_ = 80;
private:
    inline static const uint8_t significantDigits_ = 15;
};
