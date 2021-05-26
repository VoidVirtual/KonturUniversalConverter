/** Created by D.Kabzhan, 30.04.2021 */
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
    public:
        /** Gets the distance between vertices, corresponding to the provided values
         *  std::nullopt is returned if the graph does not contain such vertices.
        */
        std::optional<W> getBfsDistance(V const& source, V const& target,IAlgebraicOperator<W> const& distanceCalculator)const{
            auto sourceIt = vertexMap.find(source);
            auto targetIt = vertexMap.find(target);
            if (sourceIt == vertexMap.end() || targetIt == vertexMap.end()) {
                return std::nullopt;
            }
            else {
                return getBfsDistance(sourceIt->second, targetIt->second, distanceCalculator);
            }
        }
        /**
        *  Gets the distance between two vertices
         * The breadth first search algorithm is used
         * The way of calculating distances (sum, multiplication, etc) depends on distanceCalculator parameter
         * std::nullopt is returned if there is no path between vertices
         *
        */
        std::optional<W> getBfsDistance(Vertex const& sourceVertex,
                                        Vertex const& targetVertex,
                                        IAlgebraicOperator<W>
                                        const& distanceCalculator)const{
            W distance = distanceCalculator.getNeutralElement();
            PredecessorsMap predecessorsMap;
            PredecessorsRecorder recorder(predecessorsMap, targetVertex);
            try {
                boost::breadth_first_search(*this, sourceVertex, boost::visitor(recorder));
            }
            catch (TargetReachedException& ex) {
                auto currentVertex = targetVertex;
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
        /**
          *  Adds and returns a vertex with provided value to the graph
          *  If such vertex already exists, just returns it
          */
        Vertex addVertex(V value) {
            auto it = vertexMap.find(value);
            if (it != vertexMap.end()) {
                return it->second;
            }
            auto vertex = boost::add_vertex(*this);
            vertexMap.insert(std::make_pair<>(value, vertex));
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
    };
