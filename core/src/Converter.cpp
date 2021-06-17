#include "converter/Converter.h"
#include <unordered_set>
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
std::optional<mpf_class>
FastConverter::convertProducts(Product const& left, Product& right)const { 
    if (left.size() != right.size()) {
        return std::nullopt; // bijetction from left to right is required
    }
    mpf_class result = multiplier_.getOne();
    auto rightSet = createSet(right);
    auto searchCondition = [&rightSet](std::string const& name){ 
        auto it = rightSet.find(name); //searching any quantity from right product that is reachable by bfs.  
        if(it==rightSet.end()){
            return false;
        }
        rightSet.erase(it); // injetction from left to right is required
        return true;
    };
    for(auto leftQuantity: left){
        auto coef = ruleGraph_.getBfsDistanceIf(leftQuantity.getName(), searchCondition, multiplier_);
        if(coef == std::nullopt){
            return std::nullopt;
        }
        result = multiplier_(result,coef.value());
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
