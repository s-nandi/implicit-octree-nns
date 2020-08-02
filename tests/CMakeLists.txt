find_package(Catch2 CONFIG REQUIRED)

set(TESTING_TARGET_NAME ${PROJECT_NAME}_testing)
set(TEST_SOURCE_FILES
        # List test source files below
        construct_point.cpp
        construct_nearest_neighbor.cpp
        test_nearest_neighbor_locate.cpp
        test_octree_cell.cpp
        test_equation_2.cpp
        test_equation_hull.cpp
        test_axis_aligned_2.cpp
        construct_model_hash.cpp
        test_kd_tree.cpp
        test_bounding_box.cpp)

# Create executable containing all tests
add_executable(${TESTING_TARGET_NAME} ${TEST_SOURCE_FILES} test_main.cpp)
target_link_libraries(${TESTING_TARGET_NAME} PRIVATE ${PROJECT_NAME} Catch2::Catch2)
#
## Craete separate ctest run for each test file
#foreach (SINGLE_TEST_FILE ${TEST_SOURCE_FILES})
#    make_test(${SINGLE_TEST_FILE})
#endforeach ()