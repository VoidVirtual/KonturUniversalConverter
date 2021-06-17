/** Created by D.Kabzhan, 30.04.2021 */
#pragma once
#include <iostream>
#include <iomanip>
#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPRequestHandlerFactory.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>
#include <Poco/URI.h>
#include "converter/Converter.h"
using HTTPRequestHandler = Poco::Net::HTTPRequestHandler;
using HTTPRequestHandlerFactory = Poco::Net::HTTPRequestHandlerFactory;
using HTTPServerRequest = Poco::Net::HTTPServerRequest;
using HTTPServerResponse = Poco::Net::HTTPServerResponse;
using HTTPResponse = Poco::Net::HTTPResponse;

struct ConvertingHandlerParams {
    std::shared_ptr<AbstractConverter const> converter;
    size_t significantDigits;
};

/** A handler class, the functionality is described in "ConvertingServerApp.h"
  *
 */
class ConvertingRequestHandler : public HTTPRequestHandler
{
public:
    ConvertingRequestHandler(ConvertingHandlerParams const& params) :
        converter(params.converter) , resultSignificantDigits(params.significantDigits)
    {
    }
    virtual void handleRequest(HTTPServerRequest& req, HTTPServerResponse& resp);

    /** formats the conversion result according to resultSignificantDigits value */
    std::string formatConversionResult(mpf_class const& x) const;

    /** formats the conversion result according to resultSignificantDigits value */
    std::string getContent(HTTPServerRequest& req) const;
private:
    std::shared_ptr<AbstractConverter const> const converter; //thread safe
    size_t resultSignificantDigits;
};
/** 
 *  Creates ConvertingRequestHandler class if the request method is "POST" 
 * and URI's path is /converter
 * Otherwise nullptr will be returned
*/
class ConvertingHandlerFactory : public HTTPRequestHandlerFactory
{
public:
    ConvertingHandlerFactory(ConvertingHandlerParams const& _params) :
        params(_params) {}
    virtual HTTPRequestHandler* createRequestHandler(HTTPServerRequest const& req);
private:
    ConvertingHandlerParams params;
};
