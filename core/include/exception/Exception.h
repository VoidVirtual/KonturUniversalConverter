/** Created by D.Kabzhan, 30.04.2021 */
#pragma once
/**Parser exceptions*/
class ParseException: public std::exception{

};
class FractionParseException: public ParseException{
public:
    virtual const char* what()const noexcept override {
        return "Not a simple fraction!";
    }
};
class UnknownQuantityException : public ParseException {
public:
    UnknownQuantityException(std::string const& _name) :
        name(_name)
    {}
    virtual const char* what()const noexcept override {
        return std::string("No such quantity: " + name).c_str();
    }
private:
    std::string name;
};


/**Converter exceptions*/
class InvalidRuleException: public std::exception{

};
class ZeroCoeffException: public InvalidRuleException {
public:
    virtual const char* what()const noexcept override {
        return "Conversion rule coefficient can't be zero!";
    }
};

