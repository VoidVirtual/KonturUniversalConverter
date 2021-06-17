#include <catch2/catch.hpp>
#include "converter/Converter.h"
#include "exception/Exception.h"
TEST_CASE("Add rules","[Converter]")
{
    FastConverter converter(15);
    SECTION("Not a number")
    {
        try{
            converter.addRule("A", "B", "Not a value");
        }catch(std::exception&){
            REQUIRE(true);
        }
    }
    SECTION("Zero value")
    {
        try{
            converter.addRule("A", "B", "0");
        }catch(InvalidRuleException&){
            REQUIRE(true);
        }

    }
}
TEST_CASE("Convert quantities", "[Converter]")
{
    FastConverter converter(15);
    converter.addRule("A","B","1.0");
    converter.addRule("B","C","1.0");
    converter.addRule("D","E","1.0");

    auto parser = converter.createParser();
    Quantity a = parser.createQuantity("A");
    Quantity c = parser.createQuantity("C");
    Quantity e = parser.createQuantity("E");
    try{
        Quantity nonsence = parser.createQuantity("Nonsence"); // if quantity existence validation is required
        CHECK(false);
    }catch(UnknownQuantityException&){
        CHECK(true);
    }

    //user can create a quantity by constructor,
    //if the difference between "unknown" and "conversion impossible" results is insignificant
    Quantity directlyConstructedNonsence("Nonsence");
    auto correct = converter.convertQuantities(a,c);
    auto noConversion = converter.convertQuantities(a,e);
    auto unknownQuantity = converter.convertQuantities(a,directlyConstructedNonsence);
    CHECK(correct!=std::nullopt);
    CHECK(noConversion==std::nullopt);
    CHECK(unknownQuantity==std::nullopt);
}
TEST_CASE("Convert products", "[Converter]")
{
    FastConverter converter(15);
    converter.addRule("A1","B1","1.0");
    converter.addRule("A2","B2","2.0");
    converter.addRule("A3","B3","3.0");
    converter.addRule("A4","B4","4.0");
    converter.addRule("A5","B5","5.0");
    auto parser = converter.createParser();
    SECTION("Odd size products")
    {
        Product a = parser.createProduct("A1*A2*A3*A4*A5");
        Product b = parser.createProduct("B5*B4*B3*B2*B1");
        auto coef = converter.convertProducts(a,b);
        REQUIRE(coef!=std::nullopt);
        REQUIRE(coef==120.0);
    }
    SECTION("Even size products")
    {
        Product a = parser.createProduct("A1*A2*A3*A4");
        Product b = parser.createProduct("B4*B3*B2*B1");
        auto coef = converter.convertProducts(a,b);
        REQUIRE(coef!=std::nullopt);
        REQUIRE(coef.value()==24.0);
    }
    SECTION("Different size products")
    {
        Product a = parser.createProduct("A1*A2*A3*A4");
        Product b = parser.createProduct("B5*B4*B3*B2*B1");
        auto coef = converter.convertProducts(a,b);
        REQUIRE(coef==std::nullopt);
    }
}
