#include <catch2/catch.hpp>
#include "algorithm/DirectedWeightedGraph.h"
class Adder: public IAlgebraicOperator<int>{
public:
    virtual int getNeutralElement()const{
        return 0;
    }
    virtual int operator()(const int& x, const int& y)const{
        return x+y;
    }
};
TEST_CASE("Add edge", "[DirectedWeightedGraph]")
{
    DirectedWeightedGraph<std::string, int> graph;

    SECTION("Multiple edges between vertices")
    {
         graph.addEdge("A","B", 1);
         graph.addEdge("A","B", 2);
         auto distance = graph.getBfsDistance("A","B", Adder());
         REQUIRE(distance==1);
    }
    SECTION("No one-vertex loops")
    {
        graph.addEdge("C","C", 42);
        auto distance = graph.getBfsDistance("C","C", Adder());
        REQUIRE(distance==0);
    }
}
TEST_CASE("BFS", "[DirectedWeightedGraph]")
{
    DirectedWeightedGraph<std::string, int> graph;
    SECTION("Loop")
    {
         graph.addEdge("A","B", 1);
         graph.addEdge("B","C", 1);
         graph.addEdge("C","D", 1);
         graph.addEdge("D","A", 1);
         REQUIRE( graph.getBfsDistance("A","D", Adder()) == 3);
    }
    SECTION("Bidirectional")
    {
        graph.addEdge("A","B", 1);
        graph.addEdge("B","A", 2);
        REQUIRE( graph.getBfsDistance("A","B", Adder()) == 1);
        REQUIRE( graph.getBfsDistance("B","A", Adder()) == 2);
    }
    SECTION("Minimal edges number path")
    {
        graph.addEdge("A","B", 1);
        graph.addEdge("B","D", 1);

        graph.addEdge("A","C", 1);
        graph.addEdge("C","D", 1);

        graph.addEdge("A","D", 1);
        graph.addEdge("A","E", 1);

        graph.addEdge("E", "D", 1);
        REQUIRE(graph.getBfsDistance("A","D", Adder()) == 1);
    }
}
