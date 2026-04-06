add_executable(shortpath_benchmarks
    bench_dijkstra.cpp
)

target_link_libraries(shortpath_benchmarks
    PRIVATE
        shortpath
        benchmark::benchmark_main
        shortpath_compile_options
)
