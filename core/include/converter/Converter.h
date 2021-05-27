/** Created by D.Kabzhan, 25.05.2021 */
#pragma once
#include "algorithm/AccurateMultiplier.h"
#include "converter/ConvertableParser.h"
#include "algorithm/DirectedWeightedGraph.h"
#include <csv.h>
#include <functional>
inline std::unordered_multiset<std::string>
createSet(Product const& product){
    std::unordered_multiset<std::string> result;
    for(auto& quantity: product){
        result.insert(quantity.getName());
    }
    return result;
}
class AbstractConverter{
public:

    /** "SignificantDigits" - the number of significant digits that */
    AbstractConverter(size_t significantDigits):
            multiplier_(significantDigits)
    {}

    /**
     *  Creates a parser of convertable entities.
     *  It is initialized with a VertexMap const reference of ruleGraph member.
    */
    virtual ConvertableExpressionParser createParser()const = 0;

    /**
        Computes the a coefficient of the "q1 = a*q2" equality
        returns std::nullopt if conversion is impossible
    */
    virtual std::optional<mpf_class>
        convertQuantities(Quantity const&, Quantity const&)const = 0;

    /**
        Computes the c coefficient of "p1 = c*p2" equality
        where p1, p2 are a "q1*...*qn" products.
        depends on convertQuantities() implementation
        returns std::nullopt if conversion is impossible
    */
    virtual std::optional<mpf_class>
        convertProducts(Product const& left, Product& right)const;
protected:
    AccurateMultiplier multiplier_;
};
class Converter : public AbstractConverter
{
public:
    virtual ~Converter() = default;
    Converter(size_t significantDigits):
            AbstractConverter(significantDigits)
    {}
    /**
      * Initializes with CSV-format rule file,
      * throws exception if path is invalid
      */
    Converter(std::string const& filePath, size_t significantDigits) noexcept(false);

    /** See the AbstractConverter::convertQuantities description*/
    virtual std::optional<mpf_class>
        convertQuantities(Quantity const& from, Quantity const& to)const override;
    /** See the AbstractConverter::createParser description*/
    virtual ConvertableExpressionParser createParser() const override {
        return ConvertableExpressionParser(ruleGraph_.getVertexMap());
    }
    /**
     * Adds a conversion rule
     * Throws an InvalidRuleException if the value is 0
     * Or unknown std::exception child by mpf_set_str() function, if the string isn't numeric
    */
    void addRule(std::string const& from, std::string const& to, std::string const& valueStr)noexcept(false);

    /** Quantity map getter*/
    RuleGraph::VertexMap const& getExistingQuantities()const {
        return ruleGraph_.getVertexMap();
    }
protected:
    RuleGraph ruleGraph_;
};
class FastConverter: public Converter{

public:
   FastConverter(size_t significantDigits):
            Converter(significantDigits)
    {
    }
   FastConverter(std::string const& filePath, size_t significantDigits) noexcept(false):
        Converter(filePath, significantDigits)
    {
    }
   virtual std::optional<mpf_class>
        convertProducts(Product const& left, Product& right)const override;

};
