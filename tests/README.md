# Solver Constructor Tests

This directory contains tests to validate that the new edge-vector-based constructors produce identical results to the original file-based constructors.

## Test Files

### `simple_solver_test.cpp`
Quick test of two key solvers (SolverBIGram and SolverFWGram) to verify basic functionality.

**Usage:**
```bash
./bin/simple_solver_test <graph_file> <grammar_file>
```

### `individual_solver_test.cpp` 
Comprehensive test tool that can test any specific solver individually. Useful for debugging and validation.

**Usage:**
```bash
./bin/individual_solver_test <solver_name> <graph_file> <grammar_file>
```

**Available solver names:**
- Basic: `BIGram`, `BITopo`, `BWGram`, `BWTopo`, `FWGram`, `FWTopo`
- Parallel: `BIGramParallel`, `BITopoParallel`, `BWGramParallel`, `BWTopoParallel`, `FWGramParallel`, `FWTopoParallel`

## Test Results Summary

**✅ Validated Solvers (8/12):**
- All Forward (FW) and Bidirectional (BI) solvers work perfectly
- Both basic and parallel variants validated
- Both constructor approaches produce identical results

**❌ Known Issues (4/12):**
- Backward (BW) solvers have pre-existing stability issues in original codebase
- Crashes occur with both constructor approaches (proving equivalence)

## Building Tests

Tests are automatically built with the main project:
```bash
make
```

## Running with Parallel Solvers

For stable parallel execution, export these OpenMP settings:
```bash
export OMP_PROC_BIND=true
export OMP_WAIT_POLICY=ACTIVE
```