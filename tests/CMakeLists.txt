# Tests CMakeLists.txt

# Create the essential test executables
add_executable(simple_solver_test simple_solver_test.cpp)
add_executable(individual_solver_test individual_solver_test.cpp)

# Link against the main library
target_link_libraries(simple_solver_test graCFLlib)
target_link_libraries(individual_solver_test graCFLlib)

# Include directories
target_include_directories(simple_solver_test PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_include_directories(individual_solver_test PRIVATE ${CMAKE_SOURCE_DIR}/include)

# Set up properties
set_target_properties(simple_solver_test PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

set_target_properties(individual_solver_test PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)