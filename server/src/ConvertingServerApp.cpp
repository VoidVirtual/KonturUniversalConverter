#include "ConvertingServerApp.h"
#include <thread>
int ConvertingServerApp::main(std::vector<std::string> const&){
    try {
        auto converter = std::make_shared<Converter>(rulesFilePath_, significantDigits_);
        auto conversionParams = ConvertingHandlerParams{converter, significantDigits_};
        auto handlerFactory = new ConvertingHandlerFactory(conversionParams);
        auto serverParams = new HTTPServerParams;
        serverParams->setMaxThreads(threadNum_);
        HTTPServer s(handlerFactory, ServerSocket(port_), serverParams);
        s.start();
        waitForTerminationRequest();
        s.stop();
        delete handlerFactory;
        return Application::EXIT_OK;
    }
    catch (std::exception& ex) {
        std::cerr<<"Fatal error: " << ex.what() << "\n";
        return Application::EXIT_DATAERR;
    }
}
void ConvertingServerApp::defineOptions(OptionSet& options) {
    Application::defineOptions(options);
    options.addOption(
        Option("rules", "r", "sets the path to conversion rules file")
        .required(true)
        .argument("rules", true)
        .callback(OptionCallback<ConvertingServerApp>(this, &ConvertingServerApp::handleSetOption))
    );
    options.addOption(
         Option("port", "p", "sets the server's port")
        .required(false)
        .argument("port", true)
        .callback(OptionCallback<ConvertingServerApp>(this, &ConvertingServerApp::handleSetOption))
    );
    options.addOption(
        Option("threads", "t", "sets the number of threads")
        .required(false)
        .argument("threads", true)
        .callback(OptionCallback<ConvertingServerApp>(this, &ConvertingServerApp::handleSetOption))
    );
}
void ConvertingServerApp::handleSetOption(std::string const& optionName, std::string const& value) {
    if (optionName == "rules") {
        rulesFilePath_ = value;
    }
    else if (optionName == "port") {
        port_ = std::stoi(value);
    }
    else if (optionName == "threads") {
        threadNum_ = std::stoi(value);
    }
}

