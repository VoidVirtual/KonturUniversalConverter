#pragma once
#pragma warning(push,0)
#include <nlohmann/json.hpp>
#include <list>
#include <initializer_list>
#include <iostream>
#include <boost/algorithm/string.hpp>
#include <boost/tokenizer.hpp>
#include <gmpxx.h>
#pragma warning(pop)
#include "algorithm/DirectedWeightedGraph.h"
#include "exception/Exception.h"
/** Forward declaration*/
class Quantity;
class Product;
class Fraction;
struct ProductEquality;
using json = nlohmann::json;
using RuleGraph = DirectedWeightedGraph<std::string, mpf_class>;


/** A factory class for convertable items */
class ConvertableExpressionParser
{
public:
    /**
     *
    */
    ConvertableExpressionParser(RuleGraph::VertexMap const& _existingQuantities) :existingQuantities(_existingQuantities)
    {}
    virtual ~ConvertableExpressionParser() = default;
    /** main method, throws ParseException, UnknownQuantityException, nlohmann::json library exceptions */
    ProductEquality createProductEquality(std::string const& line)const noexcept(false);
    ProductEquality createProductEquality(Fraction&& leftF, Fraction&& rightF)const noexcept(false);
    /** supporting methods */
    Fraction createFraction(std::string const& s)const noexcept(false);
    Product createProduct(std::string const& s)const noexcept(false);
    Quantity createQuantity(std::string const& s)const noexcept(false);


private:
    RuleGraph::VertexMap const& existingQuantities;
};


/** A basic quantity class*/
class Quantity /*: public Convertable*/ {
public:
    Quantity() :mName("") {}
    Quantity(std::string const& name) :mName(name) {}
    std::string getName()const { return mName; }
   // Fraction toFraction();
private:
    std::string mName;
};

/** Basic quantity product class for
    "q1*...*qn" expressions
*/
class Product: public std::list<Quantity>{
public:
    Product() : std::list<Quantity>()
    {}
    Product(std::initializer_list<Quantity> const& l) :
        std::list<Quantity>(l)
    {}
    Product(Product const&) = delete;
    Product(Product&& other) :
        std::list<Quantity>(std::move(other))
    {}
    Product& operator*=(Product&& other) {
        if (this == &other) {
            insert(end(), begin(), end());
        }
        else {
            splice(end(), other);
        }
        return *this;
    }
    /** string representation, */
    std::string getString()const;
};
/** Common struct instead of std::pair for more convinient usage*/
struct ProductEquality{
    Product left;
    Product right;
};
/** Simple fraction class for
 *  "a1*...an / b1*...*bn" expressions
*/
class Fraction{
public:
    Fraction() = default;
    /** A fraction can be created only from temporal products*/
    Fraction(Product&& numenator, Product&& denumenator):
        numenator_(std::move(numenator)),
        denumenator_(std::move(denumenator))
    {}
    /**Fraction with no denumenator */
      Fraction(Product&& numenator) : Fraction(std::move(numenator), std::move(Product{}))
      {}
public:
    Product& getNumenatorRef() { return numenator_; }
    Product& getDenumenatorRef() { return denumenator_; }
    size_t size() { return numenator_.size() + denumenator_.size(); }
    /**string representation*/
    std::string getString()const;
private:
    Product numenator_;
    Product denumenator_;
};


