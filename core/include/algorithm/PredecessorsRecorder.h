/** Created by D.Kabzhan, 30.04.2021 */
#pragma once
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/breadth_first_search.hpp>
/**
 *  Some conventionally weird solutions are dictated by Boost Graph Library design.
 *  They are marked as (<!!!>).
 *  Detals: https://www.boost.org/doc/libs/1_59_0/libs/graph/doc/faq.html
*/
class TargetReachedException : public std::exception {}; //(<!!!>)


template<class G>
    using _Vertex = typename boost::graph_traits<G>::vertex_descriptor;
template<class G>
    using _Edge = typename boost::graph_traits<G>::edge_descriptor;

template<class G> //for more understandable names instead of "first and second" in std::pair
    struct _EdgeVertexPair{
        _Edge<G> edge;
        _Vertex<G> vertex;
    };
template<class G>
    using _PredecessorsMap = std::unordered_map<_Vertex<G>, _EdgeVertexPair<G> >;

template<class V, class G>
    using _VertexMap = std::unordered_map<V, _Vertex<G> >;

/**
 * An adapter for breadth_first_search function
 * Records a nearest precessors (both vertex and edge) of all traversed during breadth_firs_search vertices to
 * Predecessors Map object, which is required to be passed in constructor by non-const reference(<!!!>)
 *
*/
template<class G>
    class PredecessorsRecorder : public boost::default_bfs_visitor {
    public:
        PredecessorsRecorder(_PredecessorsMap<G>& predessorsMap, _Vertex<G> const& _target) :
            predcessorsMap_(predessorsMap), target(_target){
        
        }
        void examine_vertex(_Vertex<G> const& v, G const&) {
            if (v == target) {
                throw TargetReachedException(); //<!!!>
            }
        }
        void examine_edge(_Edge<G> const& e, G const& graph) {
            auto target = boost::target(e, graph);
            auto source = boost::source(e, graph);
            auto edgeAndSource = _EdgeVertexPair<G>{e, source};
            predcessorsMap_.insert(std::make_pair<>(target, edgeAndSource));
        }
        _PredecessorsMap<G>& predcessorsMap_;
        _Vertex<G> target;
    };
