#include <iostream>
#include <stdexcept>
#include <vector>
#include <unordered_set>

#include "solvers/Solver.hpp"
#include "utils/Config.hpp"

int main(int argc, char* argv[]) {
    try {
        gracfl::Config config("ConfigGraCFL");
        config.printConfigs();
        gracfl::Solver* solver = new gracfl::Solver(config);
        solver->printLabelIDToSymbolMap();
        solver->solve();
        auto gmap = solver->getGrammarMap();
        std::cout << "Grammar map:\n";
        for (const auto &[id,str] : gmap) {
            std::cout << id << ":" << str << "\n";
        }
        std::cout << "\n\nGraph dump:\n";
        std::vector<std::vector<std::unordered_set<gracfl::ull>>> out = solver->getGraph();
        for (size_t i = 0; i < out.size(); ++i) {
            for (size_t j = 0; j < out[i].size(); ++j) {
                for (const auto& element : out[i][j]) {
                    std::cout << i << " " << element << " " << j << "\n";
                }
            }
        }
        delete solver;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n\n";
        gracfl::Config::printUsage(argv[0]);
        return 1;
    }

    return 0;
}

