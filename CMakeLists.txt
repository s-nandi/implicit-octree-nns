cmake_minimum_required(VERSION 3.2)
project(implicit_octree_nns)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/scripts/cmake" ${CMAKE_MODULE_PATH})

# Sets ${PROJECT_NAME}_COMPILE_OPTIONS to a list of compile options
include(scripts/cmake/SetProjectCompileOptions.cmake)
set_project_compile_options(${PROJECT_NAME}_COMPILE_OPTIONS)

find_package(CGAL)
include(${CGAL_USE_FILE})
# Configure main target(s)
set(${PROJECT_NAME}_SOURCE_FILES
        # List source files below
        src/nearest_neighbor.cpp
        include/implicit_octree_nns/nearest_neighbor.hpp
        include/implicit_octree_nns/detail/nearest_neighbor_impl.hpp
        include/implicit_octree_nns/point_traits.hpp
        include/implicit_octree_nns/model_point.hpp
        include/implicit_octree_nns/hash_table_traits.hpp
        include/implicit_octree_nns/detail/naive_nearest_neighbor.hpp
        include/implicit_octree_nns/detail/octree_cell.hpp
        include/implicit_octree_nns/model_hash_table.hpp
        include/implicit_octree_nns/detail/bounding_box.hpp
        include/implicit_octree_nns/detail/axis_aligned.hpp
        include/implicit_octree_nns/detail/equation.hpp
        include/implicit_octree_nns/detail/equation_hull.hpp
        include/implicit_octree_nns/detail/utility.hpp
        include/implicit_octree_nns/visualize/visualize_flag.hpp
        include/implicit_octree_nns/visualize/drawing.hpp
        include/implicit_octree_nns/detail/cgal_voronoi.hpp
        include/implicit_octree_nns/detail/kd_tree.hpp)
add_library(${PROJECT_NAME} STATIC ${${PROJECT_NAME}_SOURCE_FILES})
target_compile_options(${PROJECT_NAME} PRIVATE ${${PROJECT_NAME}_COMPILE_OPTIONS})
target_link_libraries(${PROJECT_NAME} PRIVATE CGAL::CGAL)
target_include_directories(${PROJECT_NAME}
        PUBLIC ${PROJECT_SOURCE_DIR}/include
        PRIVATE ${PROJECT_SOURCE_DIR}/src)

add_executable(${PROJECT_NAME}_experiment src/experiment.cpp include/implicit_octree_nns/detail/experiment_setup.hpp)
target_compile_options(${PROJECT_NAME}_experiment PRIVATE ${${PROJECT_NAME}_COMPILE_OPTIONS})
target_link_libraries(${PROJECT_NAME}_experiment PRIVATE ${PROJECT_NAME})
target_include_directories(${PROJECT_NAME}_experiment
        PUBLIC ${PROJECT_SOURCE_DIR}/include
        PRIVATE ${PROJECT_SOURCE_DIR}/src)

add_executable(${PROJECT_NAME}_experiment_distribution src/experiment_distribution.cpp include/implicit_octree_nns/detail/experiment_setup.hpp)
target_compile_options(${PROJECT_NAME}_experiment_distribution PRIVATE ${${PROJECT_NAME}_COMPILE_OPTIONS})
target_link_libraries(${PROJECT_NAME}_experiment_distribution PRIVATE ${PROJECT_NAME})
target_include_directories(${PROJECT_NAME}_experiment_distribution
        PUBLIC ${PROJECT_SOURCE_DIR}/include
        PRIVATE ${PROJECT_SOURCE_DIR}/src)

add_executable(${PROJECT_NAME}_experiment_benchmark src/nearest_neighbor_benchmarks.cpp include/implicit_octree_nns/detail/experiment_setup.hpp)
target_compile_options(${PROJECT_NAME}_experiment_benchmark PRIVATE ${${PROJECT_NAME}_COMPILE_OPTIONS})
target_link_libraries(${PROJECT_NAME}_experiment_benchmark PRIVATE ${PROJECT_NAME})
target_include_directories(${PROJECT_NAME}_experiment_benchmark
        PUBLIC ${PROJECT_SOURCE_DIR}/include
        PRIVATE ${PROJECT_SOURCE_DIR}/src)

# Determine if this is being used as a subproject
set(${PROJECT_NAME}_IS_ROOT_PROJECT FALSE)
if (${PROJECT_SOURCE_DIR} STREQUAL ${CMAKE_SOURCE_DIR})
    set(${PROJECT_NAME}_IS_ROOT_PROJECT TRUE)
endif ()

option(BUILD_VISUALIZATION "Build visualization for project" OFF)
if (${BUILD_VISUALIZATION} AND ${${PROJECT_NAME}_IS_ROOT_PROJECT})
    target_compile_definitions(${PROJECT_NAME} PUBLIC -DVISUALIZE_ENABLED)
    target_compile_definitions(${PROJECT_NAME}_experiment PUBLIC -DVISUALIZE_ENABLED)
endif ()

# Unit testing configuration
option(BUILD_TESTS "Build tests for project" OFF)
if (${BUILD_TESTS} AND ${${PROJECT_NAME}_IS_ROOT_PROJECT})
    enable_testing()
    add_subdirectory(tests)
endif ()