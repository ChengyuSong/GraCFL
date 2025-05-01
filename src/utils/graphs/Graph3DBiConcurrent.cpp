#include "utils/graphs/Graph3DBiConcurrent.hpp"

namespace gracfl {

    static ull countEdgeHelperConcurrent(std::vector<std::vector<tbb::concurrent_unordered_set<ull>>>& hashset)
    {
        ull size = 0;
        for (uint i = 0; i < hashset.size(); i++)
        {
            for (uint j = 0; j < hashset[i].size(); j++)
            {
                size += hashset[i][j].size();
            }
        }
        return size;
    }

    Graph3DBiConcurrent::Graph3DBiConcurrent(std::string& graphfilepath, const Grammar& grammar)
        : Graph(graphfilepath, grammar) 
    {
        initContainers();
        addInitialEdges();
    }

    void Graph3DBiConcurrent::initContainers()
    {
        outEdges_.assign(getNodeSize(), std::vector<TemporalVectorConcurrent>(getLabelSize()));
        inEdges_.assign(getNodeSize(), std::vector<TemporalVectorConcurrent>(getLabelSize()));
        hashset_.assign(getNodeSize(), std::vector<tbb::concurrent_unordered_set<ull>>(getLabelSize(), tbb::concurrent_unordered_set<ull>()));
    }

    void Graph3DBiConcurrent::addInitialEdges()
    {
        for (Edge edge : getEdges())
        {
            hashset_[edge.from][edge.label].insert(edge.to);
            outEdges_[edge.from][edge.label].vertexList.push_back(edge.to);
            inEdges_[edge.to][edge.label].vertexList.push_back(edge.from);

            // update the sliding pointers
            outEdges_[edge.from][edge.label].NEW_END++;
            inEdges_[edge.to][edge.label].NEW_END++;
        }
    }

    void Graph3DBiConcurrent::clearContainers()
    {
        outEdges_.clear();
        inEdges_.clear();
        hashset_.clear();
    }


    void Graph3DBiConcurrent::checkAndAddEdge(Edge& edge, bool& terminate)
    {
        if (hashset_[edge.from][edge.label].find(edge.to) == hashset_[edge.from][edge.label].end()) {
            hashset_[edge.from][edge.label].insert(edge.to);
            outEdges_[edge.from][edge.label].vertexList.push_back(edge.to);
            inEdges_[edge.to][edge.label].vertexList.push_back(edge.from);
            terminate = false;
        }
    }

    void Graph3DBiConcurrent::addSelfEdge(Edge& edge)
    {
        if (hashset_[edge.from][edge.label].find(edge.to) == hashset_[edge.from][edge.label].end())
        {
            hashset_[edge.from][edge.label].insert(edge.to);
            outEdges_[edge.from][edge.label].vertexList.push_back(edge.to);
            outEdges_[edge.from][edge.label].NEW_END++;
            inEdges_[edge.to][edge.label].vertexList.push_back(edge.from);
            inEdges_[edge.to][edge.label].NEW_END++;
        }
    }

    ull Graph3DBiConcurrent::countEdge()
    {
        return countEdgeHelperConcurrent(hashset_);
    }
}