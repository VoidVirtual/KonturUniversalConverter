#include "converter/Converter.h"
std::optional<mpf_class>
AbstractConverter::convertProducts(Product const& left, Product& right)const {
    if (left.size() != right.size()) {
        return std::nullopt; // bijetction from left to right is required
    }
    mpf_class result = multiplier_.getOne();
    for (auto leftQuantity : left) {
        bool wasConverted = false; // a flag, indicates that a conversion happened during the loop
        for (auto it = right.begin(); it != right.end();) {
            auto coef = convertQuantities(leftQuantity, *it);
            if (coef != std::nullopt) {
                result = multiplier_(result, coef.value());
                it = right.erase(it); // injetction from left to right is required
                wasConverted = true;
                break;
            }
            else {
                ++it;
            }
        }
        if (!wasConverted) {
            return std::nullopt; // surjetction from left to right is required
        }
    }
    return result;
}
Converter::Converter(std::string const& filePath, size_t significantDigits)noexcept(false):
                                AbstractConverter(significantDigits){
    io::CSVReader<3> in(filePath.c_str());
    std::string source;
    std::string target;
    std::string value;
    while (in.read_row(source, target, value)) {
        try{
            addRule(source, target, value);
        }catch(std::exception& ex) { //mpf_set_str() function throws an unknown std::exception child
            std::cerr<<"WARNING! The rule '"
                     <<source<<","<<target<<","<<value
                     <<"' is incorrect and has not been added. Reason: "
                     <<ex.what()<<"\n";
        }
    }
}
std::optional<mpf_class>
Converter::convertQuantities(Quantity const& from, Quantity const& to)const {
    return ruleGraph_.getBfsDistance(from.getName(), to.getName(), multiplier_);
}
void
Converter::addRule(std::string const& from, std::string const& to, std::string const& valueStr)
                                                                                    noexcept(false) {
    auto value = multiplier_.getNeutralElement();
    value.set_str(valueStr, 10);
    if(value==0){
        throw ZeroCoeffException();
    }
    ruleGraph_.addEdge(from, to, value);
    ruleGraph_.addEdge(to, from, 1.0 / value);
}
