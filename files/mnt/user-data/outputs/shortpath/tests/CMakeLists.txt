add_executable(shortpath_tests
    test_graph.cpp
    test_dijkstra.cpp
    test_astar.cpp
)

target_link_libraries(shortpath_tests
    PRIVATE
        shortpath
        Catch2::Catch2WithMain
        shortpath_compile_options
)

include(Catch)
catch_discover_tests(shortpath_tests)
