/** Created by D.Kabzhan, 30.04.2021 */
#include <iostream>
#include "ConvertingHandler.h"
HTTPRequestHandler*
ConvertingHandlerFactory::createRequestHandler(HTTPServerRequest const& req){
    Poco::URI uri(req.getURI());
    if(uri.getPath()=="/converter" &&
        req.getMethod()=="POST"){
        return new ConvertingRequestHandler(params);
    }
    return nullptr;
}
void
ConvertingRequestHandler::handleRequest(HTTPServerRequest& req, HTTPServerResponse& resp) {
    resp.setContentType("html");
    auto content = getContent(req);
    auto parser = converter->createParser();
    try {
        auto [left, right] = parser.createProductEquality(content);
        auto coef = converter->convertProducts(left, right);
        if (coef != std::nullopt) {
            resp.setStatus(HTTPResponse::HTTP_OK);
            std::ostream& send = resp.send();
            send << formatConversionResult(coef.value());
            send.flush();
        }
        else {
            resp.setStatus(HTTPResponse::HTTP_NOT_FOUND);
            resp.setReason("Conversion is impossible");
            resp.send().flush();
        }
    }
    catch (ParseException&) {
        resp.setStatus(HTTPResponse::HTTP_BAD_REQUEST);
        resp.setReason("");
        resp.send().flush();
    }
    catch (nlohmann::json::parse_error& ) {
        resp.setStatus(HTTPResponse::HTTP_BAD_REQUEST);
        resp.setReason("Incorrect format: not a json");
        resp.send().flush();
    }
    catch (nlohmann::detail::out_of_range&) {
        resp.setStatus(HTTPResponse::HTTP_BAD_REQUEST);
        resp.setReason(" \"From\" or \"to\" field was not provided");
        resp.send().flush();
    }
    catch (std::exception& ex) {
        resp.setStatus(HTTPResponse::HTTP_BAD_REQUEST);
        resp.setReason(ex.what());
        resp.send().flush();
    }
}

std::string
ConvertingRequestHandler::getContent(HTTPServerRequest& req) const{
    auto& stream = req.stream();
    size_t const contentLength = req.getContentLength();
    std::string buffer(contentLength, 0);
    stream.read(buffer.data(), contentLength);
    return buffer;
}
std::string
ConvertingRequestHandler::formatConversionResult(mpf_class const& x) const{
    std::stringstream ss;
    ss << std::setprecision(resultSignificantDigits);
    ss << std::defaultfloat;
    ss << x;
    return ss.str();
}
