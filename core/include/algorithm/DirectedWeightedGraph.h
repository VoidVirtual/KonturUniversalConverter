/** Created by D.Kabzhan, 25.05.2021 */
#pragma once
#include <vector>
#include <iostream>
#include <optional>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/breadth_first_search.hpp>
#include <boost/core/demangle.hpp>
#include "algorithm/BinaryOperator.h"
#include "algorithm/PredecessorsRecorder.h"
/** A wrapper class over boost::graph::adjacency_list
 *  V - vertex value type
 *  W - edge weight type
*/
template<class V, class W>
    class DirectedWeightedGraph: public boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS, boost::no_property, W> {
    public:
        using Edge = _Edge<DirectedWeightedGraph>;
        using Vertex = _Vertex<DirectedWeightedGraph> ;
        using EdgeVertexPair = _EdgeVertexPair<DirectedWeightedGraph>;
        using PredecessorsMap = _PredecessorsMap<DirectedWeightedGraph>;
        using VertexMap = _VertexMap<V,DirectedWeightedGraph>;
        using ValueMap = _ValueMap<V,DirectedWeightedGraph>;
    public:
        /** Gets the distance between source vertex and the nearest (in terms of the edges number)
          * vertex, which has a value that satisfies "valueSearchcndition"
         *  std::nullopt is returned if the graph does not contain such vertices.
        */
        std::optional<W> 
        getBfsDistanceIf(V const& source, 
                std::function<bool(V const&)> const& targetValueCondition,
                                 IAlgebraicOperator<W> const& distanceCalculator)const{
            auto sourceIt = vertexMap.find(source);
            if (sourceIt == vertexMap.end()) {
                return std::nullopt;
            }
            else {
                return getBfsDistanceIf(sourceIt->second, targetValueCondition, distanceCalculator);
            }
        }
        /** Gets the distance between vertices, corresponding to the provided values
         *  std::nullopt is returned if the graph does not contain such vertices.
        */
        std::optional<W> 
        getBfsDistance(V const& source, V const& target, IAlgebraicOperator<W> const& distanceCalculator)const{
            auto sourceIt = vertexMap.find(source);
            auto targetIt = vertexMap.find(target);
            if (sourceIt == vertexMap.end() || targetIt == vertexMap.end()) {
                return std::nullopt;
            }
            else {
                return getBfsDistance(sourceIt->second, targetIt->second, distanceCalculator);
            }
        }
    protected:
        /**
         *  Gets the distance between two vertices
         *  The breadth first search algorithm is used
         *  The way of calculating distances (sum, multiplication, etc) depends on distanceCalculator parameter
         *  std::nullopt is returned if there is no path between verticess
        */
         std::optional<W> 
         getBfsDistanceIf(Vertex const& sourceVertex,
                                             std::function<bool(V const&)> const& targetValueCondition,
                                                    IAlgebraicOperator<W> const& distanceCalculator)const{
          
            auto targetVertexCondition = [this, &targetValueCondition](Vertex const& vertex){
                    auto it =  valueMap.find(vertex);
                    if(it == valueMap.end()){
                         return false;
                    }
                    return targetValueCondition(it->second);
            };
            PredecessorsMap predecessorsMap;
            PredecessorsRecorder recorder(predecessorsMap, targetVertexCondition);
            try {
                boost::breadth_first_search(*this, sourceVertex, boost::visitor(recorder));
            }
            catch (TargetReachedException<DirectedWeightedGraph>& target) {
                W distance = distanceCalculator.getNeutralElement();
                auto currentVertex = target.getVertex();
                while (currentVertex != sourceVertex) {
                    auto const& predecessor = predecessorsMap[currentVertex];
                    auto currentDistance = (*this)[predecessor.edge];
                    distance = distanceCalculator(distance,currentDistance);
                    currentVertex = predecessor.vertex;
                }
                return distance;
            }
            return std::nullopt;
        }
       std::optional<W> 
       getBfsDistance(Vertex const& sourceVertex,
                            Vertex const& targetVertex,
                                   IAlgebraicOperator<W> const& distanceCalculator)const {

               auto equalsTarget = [&targetVertex, this](V const& v){
                        auto it = valueMap.find(targetVertex);
                        return (bool)(it!=valueMap.end()) && (it->second == v);
                }; 
                return getBfsDistanceIf(sourceVertex,equalsTarget, distanceCalculator);
        }
        /**
          *  Adds and returns a vertex with provided value to the graph
          *  If such vertex already exists, just returns it
          */
    public:
        Vertex addVertex(V value) {
            auto it = vertexMap.find(value);
            if (it != vertexMap.end()) {
                return it->second;
            }
            auto vertex = boost::add_vertex(*this);
            vertexMap.insert(std::make_pair<>(value, vertex));
            valueMap.insert(std::make_pair<>(vertex, value));
            return vertex;
        }
      /**
        *  Adds up to two vertices of provided values, depending on their existence
        *  Adds an edge with provided weight between them
        *  Nothing happens if some edge between those vertices already exists
        */
        void addEdge(V const& source, V const& target, W const& weight) {
            auto sourceVertex = addVertex(source);
            auto targetVertex = addVertex(target);
            boost::add_edge(sourceVertex, targetVertex, weight, *this);
        }
        /** Vertex map getter*/
        VertexMap const& getVertexMap() const{
            return vertexMap;
        }
    private:
        VertexMap vertexMap;
        ValueMap valueMap;
    };
