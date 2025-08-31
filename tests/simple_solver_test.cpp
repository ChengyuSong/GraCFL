#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cassert>
#include <unordered_set>

// Include solver headers
#include "solvers/SolverBIGram.hpp"
#include "solvers/SolverFWGram.hpp"
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
    
    std::cout << "Loaded " << edges.size() << " edges from file" << std::endl;
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
 * @brief Test a specific solver with both constructors
 */
template<typename SolverType>
void testSolver(const std::string& solverName, 
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
        std::cout << " (edges: " << count1 << " vs " << count2 << ")";
        assert(count1 == count2);
        assert(compareGraphs(graph1, graph2));
        
        std::cout << " PASSED" << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << " FAILED: " << e.what() << std::endl;
        throw;
    }
}

} // namespace gracfl

/**
 * @brief Main function to run selected tests
 */
int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <graph_file_path> <grammar_file_path>" << std::endl;
        return 1;
    }
    
    std::string graphfilepath = argv[1];
    std::string grammarfilepath = argv[2];
    
    std::cout << "=== Simple Solver Constructor Comparison Tests ===" << std::endl;
    std::cout << "Graph file: " << graphfilepath << std::endl;
    std::cout << "Grammar file: " << grammarfilepath << std::endl;
    std::cout << std::endl;
    
    try {
        // Test a few key solvers to avoid segfaults
        gracfl::testSolver<gracfl::SolverBIGram>("SolverBIGram", graphfilepath, grammarfilepath);
        gracfl::testSolver<gracfl::SolverFWGram>("SolverFWGram", graphfilepath, grammarfilepath);
        
        std::cout << std::endl;
        std::cout << "=== ALL TESTS PASSED ===" << std::endl;
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Tests failed: " << e.what() << std::endl;
        return 1;
    }
}