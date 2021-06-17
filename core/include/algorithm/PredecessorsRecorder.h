/** Created by D.Kabzhan, 25.05.2021 */
#pragma once
#pragma warning(push,0)
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/breadth_first_search.hpp>
#include <functional>
<<<<<<< HEAD
=======
#pragma warning(pop)
>>>>>>> main
/**
 *  Some conventionally weird solutions are dictated by Boost Graph Library design.
 *  They are marked as (<!!!>).
 *  Detals: https://www.boost.org/doc/libs/1_59_0/libs/graph/doc/faq.html
*/
<<<<<<< HEAD



=======
>>>>>>> main
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
template<class V, class G>
    using _ValueMap = std::unordered_map<_Vertex<G>, V >;

template<class G>
<<<<<<< HEAD
class TargetReachedException : public std::exception {//(<!!!>)
    public: 
        TargetReachedException(_Vertex<G> const& v):
=======
class TargetReached  {//(<!!!>)
    public: 
        TargetReached(_Vertex<G> const& v):
>>>>>>> main
                                vertex_(v)
        {
        }
        _Vertex<G> getVertex(){return vertex_;};
    private:
        _Vertex<G> vertex_;
}; 

/**
 * An adapter for breadth_first_search function
 * Records a nearest precessors (both vertex and edge) of all traversed during breadth_firs_search vertices to
 * Predecessors Map object, which is required to be passed in constructor by non-const reference(<!!!>)
 *
*/
template<class G>
    class PredecessorsRecorder : public boost::default_bfs_visitor {
    public:
        PredecessorsRecorder(_PredecessorsMap<G>& predessorsMap,
                                                   std::function<bool(_Vertex<G> const&)> const& searchCondition) :
                         predcessorsMap_(predessorsMap), searchCondition_(searchCondition){
        }
         /**Constructor with default search condition */
         PredecessorsRecorder(_PredecessorsMap<G>& predessorsMap,
                                                   _Vertex<G> const& target) :
                         predcessorsMap_(predessorsMap) {
             searchCondition_ = [&target](_Vertex<G> const& v){return v == target;}; 
        }
        void examine_vertex(_Vertex<G> const& v, G const&) {
            if (searchCondition_(v)) {
<<<<<<< HEAD
                throw TargetReachedException<G>(v); //<!!!>
=======
                throw TargetReached<G>(v); //<!!!>
>>>>>>> main
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
        std::function<bool(_Vertex<G>)> searchCondition_;
    };
