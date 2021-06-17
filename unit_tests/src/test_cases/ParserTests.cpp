#include <catch2/catch.hpp>
#include "converter/Converter.h"
#include "converter/ConvertableParser.h"
#include "exception/Exception.h"
TEST_CASE("Parse json", "[ConvertableParser]")
{
    RuleGraph::VertexMap vertexMap;
    vertexMap["A"] = 0;
    vertexMap["B"] = 0;
    vertexMap["C"] = 0;
    vertexMap["D"] = 0;
    ConvertableExpressionParser parser(vertexMap);

    SECTION("Not a json")
    {
        try{
            parser.createProductEquality("Not a json");
            REQUIRE(false);
        }catch(nlohmann::json::parse_error&){
            REQUIRE(true);
        }
    }
    SECTION("No required key")
    {
        nlohmann::json j;
        j["from"] = "A*B";
        j["nonsence"] = "D/C";
        try{
            parser.createProductEquality(j.dump());
            REQUIRE(false);
        }catch(nlohmann::detail::out_of_range&){
            REQUIRE(true);
        }

    }
}
TEST_CASE("Parse products", "[ConvertableParser]")
{
    SECTION("White spaces")
    {
        try{
            RuleGraph::VertexMap vertexMap;
            vertexMap["A"] = 0;
            vertexMap["B"] = 0;
            vertexMap["C"] = 0;
            vertexMap["D"] = 0;
            ConvertableExpressionParser parser(vertexMap);
            Product p1 = parser.createProduct("A*B");
            Product p2 = parser.createProduct("A * B");
            REQUIRE(p1.getString() == p2.getString());
            Product p3 = parser.createProduct("A * * B");
        }catch(UnknownQuantityException&){
            REQUIRE(true);
        }
    }
}
TEST_CASE("Parse fractions", "[ConvertableParser]")
{
    RuleGraph::VertexMap vertexMap;
    vertexMap["A"] = 0;
    vertexMap["B"] = 0;
    vertexMap["C"] = 0;
    vertexMap["D"] = 0;
    ConvertableExpressionParser parser(vertexMap);
    SECTION("Not a fraction")
    {
        try{
            parser.createFraction("A / B / C");
            REQUIRE(false);
        }catch(FractionParseException&){
            REQUIRE(true);
        }
    }
    SECTION("Unit numenator")
    {
        try{
            parser.createFraction("1 / A");
            REQUIRE(true);
        }catch(FractionParseException&){
            REQUIRE(false);
        }
    }
    SECTION("Unit numenator & denumenator")
    {
        try{
            auto f = parser.createFraction("1 / 1");
            REQUIRE(f.size()==0);
        }catch(FractionParseException&){
            REQUIRE(false);
        }catch(UnknownQuantityException&){
            REQUIRE(false);
        }
    }
    SECTION("Empty numenator")
    {
        try{
            parser.createFraction(" / A");
            REQUIRE(false);
        }catch(FractionParseException&){
            REQUIRE(false);
        }catch(UnknownQuantityException&){
            REQUIRE(true);
        }
    }
    SECTION("Empty string")
    {
        try{
            auto f = parser.createFraction("");
            REQUIRE(f.size()==0);
        }catch(FractionParseException&){
            REQUIRE(false);
        }catch(UnknownQuantityException&){
            REQUIRE(false);
        }
    }
    SECTION("Whitespaces")
    {
        try{
            auto f = parser.createFraction("   ");
            REQUIRE(f.size()==0);
        }catch(FractionParseException&){
            REQUIRE(false);
        }catch(UnknownQuantityException&){
            REQUIRE(false);
        }
    }
}
TEST_CASE("Create product equality", "[ConvertableParser]")
{
    RuleGraph::VertexMap vertexMap;
    vertexMap["A"] = 0;
    vertexMap["B"] = 0;
    vertexMap["C"] = 0;
    vertexMap["D"] = 0;
    ConvertableExpressionParser parser(vertexMap);
    SECTION("Simple fractions")
    {
        Fraction f1 = parser.createFraction("A / B");
        Fraction f2 = parser.createFraction("C / D");
        auto[left, right] = parser.createProductEquality(std::move(f1),std::move(f2));
        REQUIRE(left.getString()=="A * D");
        REQUIRE(right.getString()=="B * C");
        REQUIRE(f1.size()==0);
        REQUIRE(f2.size()==0);
    }
    SECTION("Empty right fraction")
    {
        Fraction f1 = parser.createFraction("A / B");
        Fraction f2 = parser.createFraction("");
        REQUIRE(f2.getString()=="1");
        auto[left, right] = parser.createProductEquality(std::move(f1),std::move(f2));
        REQUIRE(left.getString()=="A");
        REQUIRE(right.getString()=="B");
    }
}
