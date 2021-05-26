#include "ConvertingServerApp.h"
#include <thread>
int ConvertingServerApp::main(std::vector<std::string> const&){
    try {
        auto converter = std::make_shared<FastConverter>(rulesFilePath_, significantDigits_);
        auto conversionParams = ConvertingHandlerParams{converter, significantDigits_};
        auto handlerFactory = new ConvertingHandlerFactory(conversionParams);
        auto serverParams = new HTTPServerParams;
        //serverParams->setMaxThreads(std::thread::hardware_concurency());
        HTTPServer s(handlerFactory, ServerSocket(port_), serverParams);
        s.start();
        waitForTerminationRequest();
        s.stop();
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
        .argument("File Path", true)
        .callback(OptionCallback<ConvertingServerApp>(this, &ConvertingServerApp::handleRulesFile))
    );
    options.addOption(
         Option("port", "p", "sets the server's port")
        .required(false)
        .argument("port", true)
        .callback(OptionCallback<ConvertingServerApp>(this, &ConvertingServerApp::handleSetPort))
    );
}
void ConvertingServerApp::handleRulesFile(std::string const&, std::string const& filePath) {
    rulesFilePath_ = filePath;
}
void ConvertingServerApp::handleSetPort(std::string const&, std::string const& value) {
    port_ = std::stoi(value);
}
