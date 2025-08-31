#include <iostream>
#include <omp.h>
#include "solvers/SolverFWTopoParallel.hpp"

namespace gracfl 
{
    SolverFWTopoParallel::SolverFWTopoParallel(std::string graphfilepath, Grammar& grammar, uint numOfThreads)
    : SolverFWTopo(graphfilepath, grammar)
    {
        numOfThreads_ = numOfThreads;
    }

    SolverFWTopoParallel::SolverFWTopoParallel(std::vector<Edge>& edges, Grammar& grammar, uint numOfThreads)
    : SolverFWTopo(edges, grammar)
    {
        numOfThreads_ = numOfThreads;
    }

    void SolverFWTopoParallel::runCFL()
    {
        uint itr = 0;
        bool terminate;
        auto& outEdges = graph_->outEdges_;
        auto& hashset = graph_->hashset_;
        auto& grammar2index = grammar_.grammar2index_;
        auto& grammar3index = grammar_.grammar3index_;
        auto labelSize = grammar_.getLabelSize();
        auto nodeSize = graph_->getNodeSize();

        addSelfEdges(); // Add epsilon edges
        do {
            itr++;
            terminate = true;
            runSingleIterationParallel(
                outEdges,
                hashset, 
                grammar2index,
                grammar3index,
                labelSize,
                nodeSize,
                terminate
            );
            std::cout << "Iteration " << itr << std::endl;
        } while (!terminate);
    }

    void SolverFWTopoParallel::runSingleIterationParallel(
        std::vector<TemporalVectorWithLbldVtx>& outEdges,
        std::vector<std::vector<std::unordered_set<ull>>>& hashset,
        std::vector<std::vector<uint>>& grammar2index,
        std::vector<std::vector<uint>>& grammar3index,
        uint labelSize,
        uint nodeSize,
        bool& terminate)
    {
        #pragma omp parallel for schedule(static, 512) num_threads(numOfThreads_)
        for (uint i = 0; i < nodeSize; i++) {
            LbldVtx nbr;
            // The valid index range is [START_NEW, END_NEW-1]
            uint START_NEW_OUT = outEdges[i].OLD_END;
            uint END_NEW_OUT = outEdges[i].NEW_END;

            // For each new edge
            for (uint j = START_NEW_OUT; j < END_NEW_OUT; j++) {
                nbr = outEdges[i].vertexList[j];
                // If the edge to the neighbor is labeled with B
                std::vector<uint> leftLabels = grammar2index[nbr.label];

                for (uint g = 0; g < leftLabels.size(); g++) {
                    Edge newEdge(i, nbr.vtx, leftLabels[g]);
                    // Check if the edge already exists and add to graph
                    graph_->checkAndAddEdge(newEdge, terminate);
                }

                uint START_OLD = 0;
                uint END_NEW = outEdges[nbr.vtx].NEW_END;
                for (uint h = START_OLD; h < END_NEW; h++) {
                    LbldVtx outNbr = outEdges[nbr.vtx].vertexList[h];
                    std::vector<uint> leftLabels = grammar3index[nbr.label * labelSize + outNbr.label];

                    for (uint g = 0; g < leftLabels.size(); g++) {
                        Edge newEdge(i, outNbr.vtx, leftLabels[g]);
                        // Check if the edge already exists and add to graph
                        graph_->checkAndAddEdge(newEdge, terminate);
                    }
                }
            }

            uint OLD_START_OUT = 0;
            uint OLD_END_OUT = outEdges[i].OLD_END;
            for (uint j = OLD_START_OUT; j < OLD_END_OUT; j++) {
                LbldVtx nbr = outEdges[i].vertexList[j];

                uint NEW_START_OUT = outEdges[nbr.vtx].OLD_END;
                uint NEW_END_OUT = outEdges[nbr.vtx].NEW_END;
                for (uint h = NEW_START_OUT; h < NEW_END_OUT; h++) {
                    LbldVtx outNbr = outEdges[nbr.vtx].vertexList[h];
                    std::vector<uint> leftLabels = grammar3index[nbr.label * labelSize + outNbr.label];

                    for (uint g = 0; g < leftLabels.size(); g++) {
                        Edge newEdge(i, outNbr.vtx, leftLabels[g]);
                        // Check if the edge already exists and add to graph
                        graph_->checkAndAddEdge(newEdge, terminate);
                    }
                }
            }
        }

        // Update the pointers
        #pragma omp parallel for schedule(static, 512) num_threads(numOfThreads_)
        for (int i = 0; i < nodeSize; i++) {
            outEdges[i].OLD_END = outEdges[i].NEW_END;
            outEdges[i].NEW_END = outEdges[i].vertexList.size();
        }
    }
}