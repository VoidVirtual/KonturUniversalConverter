/** Created by D.Kabzhan, 30.04.2021 */
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
    std::ostream& send = resp.send(); 
    auto content = getContent(req);
    auto parser = converter->createParser();
    try {
        auto [left, right] = parser.createProductEquality(content);
        auto coef = converter->convertProducts(left, right);
        if (coef != std::nullopt) {
            resp.setStatus(HTTPResponse::HTTP_OK);
            send << formatResult(coef.value());
        }
        else {
            resp.setStatus(HTTPResponse::HTTP_NOT_FOUND);
        }
    }
    catch (ParseException& ex) {
        resp.setStatus(HTTPResponse::HTTP_BAD_REQUEST);
    }
    catch (nlohmann::json::parse_error& ex) {
        resp.setStatus(HTTPResponse::HTTP_BAD_REQUEST);
    }
    catch (nlohmann::detail::out_of_range& ex) {
        resp.setStatus(HTTPResponse::HTTP_BAD_REQUEST);
    }
    send.flush();
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
ConvertingRequestHandler::formatResult(mpf_class const& x) const{
    std::stringstream ss;
    ss << std::setprecision(resultSignificantDigits);
    ss << std::defaultfloat;
    ss << x;
    return ss.str();
}
