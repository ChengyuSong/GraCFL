#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cassert>
#include <unordered_set>

// Include all solver headers
#include "solvers/SolverBIGram.hpp"
#include "solvers/SolverBITopo.hpp"
#include "solvers/SolverBWGram.hpp"
#include "solvers/SolverBWTopo.hpp"
#include "solvers/SolverFWGram.hpp"
#include "solvers/SolverFWTopo.hpp"
#include "solvers/SolverBIGramParallel.hpp"
#include "solvers/SolverBITopoParallel.hpp"
#include "solvers/SolverBWGramParallel.hpp"
#include "solvers/SolverBWTopoParallel.hpp"
#include "solvers/SolverFWGramParallel.hpp"
#include "solvers/SolverFWTopoParallel.hpp"
#include "utils/Grammar.hpp"
#include "utils/Edges.hpp"

namespace gracfl {

/**
 * @brief Utility function to load edges from a graph file
 */
std::vector<Edge> loadEdgesFromFile(const std::string& graphfilepath, const Grammar& grammar) {
    std::vector<Edge> edges;
    Edge newEdge;
    std::string label;
    
    std::ifstream infile(graphfilepath);
    if (!infile.is_open()) {
        throw std::runtime_error("Cannot open graph file: " + graphfilepath);
    }
    
    while (infile >> newEdge.from) {
        infile >> newEdge.to;
        infile >> label;
        
        if (grammar.getSymbolToIDMap().find(label) == grammar.getSymbolToIDMap().end()) {
            continue;
        }
        
        newEdge.label = grammar.getSymbolToIDMap().at(label);
        edges.push_back(newEdge);
    }
    infile.close();
    
    return edges;
}

/**
 * @brief Compare two solver result graphs for equality
 */
bool compareGraphs(const std::vector<std::vector<std::unordered_set<ull>>>& graph1,
                   const std::vector<std::vector<std::unordered_set<ull>>>& graph2) {
    if (graph1.size() != graph2.size()) return false;
    
    for (size_t i = 0; i < graph1.size(); i++) {
        if (graph1[i].size() != graph2[i].size()) return false;
        
        for (size_t j = 0; j < graph1[i].size(); j++) {
            if (graph1[i][j] != graph2[i][j]) return false;
        }
    }
    return true;
}

/**
 * @brief Test basic solver with both constructors
 */
template<typename SolverType>
bool testBasicSolver(const std::string& solverName, 
                     const std::string& graphfilepath, 
                     const std::string& grammarfilepath) {
    std::cout << "Testing " << solverName << "..." << std::flush;
    
    try {
        Grammar grammar(grammarfilepath);
        std::vector<Edge> edges = loadEdgesFromFile(graphfilepath, grammar);
        
        // Test file-based constructor
        std::string graphpath = graphfilepath;
        SolverType solver1(graphpath, grammar);
        solver1.runCFL();
        auto graph1 = solver1.getGraph();
        auto count1 = solver1.getEdgeCount();
        
        // Test edge-based constructor
        SolverType solver2(edges, grammar);
        solver2.runCFL();
        auto graph2 = solver2.getGraph();
        auto count2 = solver2.getEdgeCount();
        
        // Compare results
        if (count1 != count2) {
            std::cout << " FAILED: Edge counts differ (" << count1 << " vs " << count2 << ")" << std::endl;
            return false;
        }
        
        if (!compareGraphs(graph1, graph2)) {
            std::cout << " FAILED: Graph structures differ" << std::endl;
            return false;
        }
        
        std::cout << " PASSED (edges: " << count1 << ")" << std::endl;
        return true;
        
    } catch (const std::exception& e) {
        std::cout << " FAILED: " << e.what() << std::endl;
        return false;
    }
}

/**
 * @brief Test parallel solver with both constructors
 */
template<typename SolverType>
bool testParallelSolver(const std::string& solverName, 
                        const std::string& graphfilepath, 
                        const std::string& grammarfilepath,
                        uint numThreads = 2) {
    std::cout << "Testing " << solverName << " (threads=" << numThreads << ")..." << std::flush;
    
    try {
        Grammar grammar(grammarfilepath);
        std::vector<Edge> edges = loadEdgesFromFile(graphfilepath, grammar);
        
        // Test file-based constructor
        std::string graphpath = graphfilepath;
        SolverType solver1(graphpath, grammar, numThreads);
        solver1.runCFL();
        auto graph1 = solver1.getGraph();
        auto count1 = solver1.getEdgeCount();
        
        // Test edge-based constructor
        SolverType solver2(edges, grammar, numThreads);
        solver2.runCFL();
        auto graph2 = solver2.getGraph();
        auto count2 = solver2.getEdgeCount();
        
        // Compare results
        if (count1 != count2) {
            std::cout << " FAILED: Edge counts differ (" << count1 << " vs " << count2 << ")" << std::endl;
            return false;
        }
        
        if (!compareGraphs(graph1, graph2)) {
            std::cout << " FAILED: Graph structures differ" << std::endl;
            return false;
        }
        
        std::cout << " PASSED (edges: " << count1 << ")" << std::endl;
        return true;
        
    } catch (const std::exception& e) {
        std::cout << " FAILED: " << e.what() << std::endl;
        return false;
    }
}

} // namespace gracfl

/**
 * @brief Main function to test individual solver
 */
int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " <solver_name> <graph_file_path> <grammar_file_path>" << std::endl;
        std::cerr << "Available solvers:" << std::endl;
        std::cerr << "  Basic: BIGram, BITopo, BWGram, BWTopo, FWGram, FWTopo" << std::endl;
        std::cerr << "  Parallel: BIGramParallel, BITopoParallel, BWGramParallel, BWTopoParallel, FWGramParallel, FWTopoParallel" << std::endl;
        return 1;
    }
    
    std::string solverName = argv[1];
    std::string graphfilepath = argv[2];
    std::string grammarfilepath = argv[3];
    
    std::cout << "=== Individual Solver Test ===" << std::endl;
    std::cout << "Solver: " << solverName << std::endl;
    std::cout << "Graph file: " << graphfilepath << std::endl;
    std::cout << "Grammar file: " << grammarfilepath << std::endl;
    std::cout << "Loaded " << gracfl::loadEdgesFromFile(graphfilepath, gracfl::Grammar(grammarfilepath)).size() << " edges" << std::endl;
    std::cout << std::endl;
    
    bool result = false;
    
    // Test basic solvers
    if (solverName == "BIGram") {
        result = gracfl::testBasicSolver<gracfl::SolverBIGram>("SolverBIGram", graphfilepath, grammarfilepath);
    } else if (solverName == "BITopo") {
        result = gracfl::testBasicSolver<gracfl::SolverBITopo>("SolverBITopo", graphfilepath, grammarfilepath);
    } else if (solverName == "BWGram") {
        result = gracfl::testBasicSolver<gracfl::SolverBWGram>("SolverBWGram", graphfilepath, grammarfilepath);
    } else if (solverName == "BWTopo") {
        result = gracfl::testBasicSolver<gracfl::SolverBWTopo>("SolverBWTopo", graphfilepath, grammarfilepath);
    } else if (solverName == "FWGram") {
        result = gracfl::testBasicSolver<gracfl::SolverFWGram>("SolverFWGram", graphfilepath, grammarfilepath);
    } else if (solverName == "FWTopo") {
        result = gracfl::testBasicSolver<gracfl::SolverFWTopo>("SolverFWTopo", graphfilepath, grammarfilepath);
    }
    // Test parallel solvers
    else if (solverName == "BIGramParallel") {
        result = gracfl::testParallelSolver<gracfl::SolverBIGramParallel>("SolverBIGramParallel", graphfilepath, grammarfilepath);
    } else if (solverName == "BITopoParallel") {
        result = gracfl::testParallelSolver<gracfl::SolverBITopoParallel>("SolverBITopoParallel", graphfilepath, grammarfilepath);
    } else if (solverName == "BWGramParallel") {
        result = gracfl::testParallelSolver<gracfl::SolverBWGramParallel>("SolverBWGramParallel", graphfilepath, grammarfilepath);
    } else if (solverName == "BWTopoParallel") {
        result = gracfl::testParallelSolver<gracfl::SolverBWTopoParallel>("SolverBWTopoParallel", graphfilepath, grammarfilepath);
    } else if (solverName == "FWGramParallel") {
        result = gracfl::testParallelSolver<gracfl::SolverFWGramParallel>("SolverFWGramParallel", graphfilepath, grammarfilepath);
    } else if (solverName == "FWTopoParallel") {
        result = gracfl::testParallelSolver<gracfl::SolverFWTopoParallel>("SolverFWTopoParallel", graphfilepath, grammarfilepath);
    } else {
        std::cerr << "Unknown solver: " << solverName << std::endl;
        return 1;
    }
    
    std::cout << std::endl;
    if (result) {
        std::cout << "=== TEST PASSED ===" << std::endl;
        return 0;
    } else {
        std::cout << "=== TEST FAILED ===" << std::endl;
        return 1;
    }
}