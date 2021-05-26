#include "converter/ConvertableParser.h"
Quantity
ConvertableExpressionParser::createQuantity(std::string const& s)const noexcept(false){
    if (existingQuantities.find(s) == existingQuantities.end()) {
        throw UnknownQuantityException(s);
    }
    return Quantity(s);
}
Product
ConvertableExpressionParser::createProduct(std::string const& s)const noexcept(false) {
    boost::char_separator<char> sep("*");
    boost::tokenizer<boost::char_separator<char> > tokens(s, sep);
    Product result;
    for (auto token : tokens) {
        boost::trim(token);
        if (token != "1") { // isn't a quantity, but a legit symbol
            result.emplace_back(createQuantity(token));
        }
    }
    return result;
}
Fraction
ConvertableExpressionParser::createFraction(std::string const& s)const noexcept(false) {
    if(s.find_first_not_of(' ') == std::string::npos){ //Empty or whitespace string is interpreted as "1" expression
        return Fraction();
    }
    std::vector<std::string> fractionSplitted;
    boost::algorithm::split(fractionSplitted, s, boost::is_any_of("/"));
    switch (fractionSplitted.size()){
        case 2:
            return Fraction(createProduct(fractionSplitted[0]), createProduct(fractionSplitted[1]));
        case 1:
            return Fraction(createProduct(fractionSplitted[0]), Product());
        default:
            throw FractionParseException();  //greater then 2 means more then one '/' symbol
    }
}
ProductEquality
ConvertableExpressionParser::createProductEquality(std::string const& line)const noexcept(false) {
    auto json = nlohmann::json::parse(line);
    Fraction from = createFraction(json.at("from"));
    Fraction to = createFraction(json.at("to"));
    return createProductEquality(std::move(from), std::move(to));
}
ProductEquality
ConvertableExpressionParser::createProductEquality(Fraction&& left, Fraction&& right)const noexcept(false) {
    auto& rightDenumenator = right.getDenumenatorRef(); //disassembling temporary fractions into 4 temporary products
    auto& rightNumenator = right.getNumenatorRef();
    auto& leftDenumenator = left.getDenumenatorRef();
    auto& leftNumenator = left.getNumenatorRef();
    auto& leftProduct = leftNumenator *= std::move(rightDenumenator); // assembling into new 2 temporary products
    auto& rightProduct = leftDenumenator *= std::move(rightNumenator);
    return ProductEquality {std::move(leftProduct), std::move(rightProduct)}; // moving temporary products to constrctors
}
std::string Product::getString()const{
    std::stringstream ss;
    if(size()==0){
        return "1";
    }
    for(auto const& q: *this){
        ss<<q.getName()<<" * ";
    }
    std::string res = ss.str();
    res.pop_back();
    res.pop_back();
    res.pop_back();
    return res;
}
std::string Fraction::getString()const{
    std::stringstream ss;
    ss<<numenator_.getString();
    if(denumenator_.size()!=0){
        ss<<" / "<<denumenator_.getString();
    }
    return ss.str();
}
