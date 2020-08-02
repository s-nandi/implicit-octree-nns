#include <algorithm>
#include <fstream>
#include <iostream>
#include <random>
#include <vector>

#include "catch2/catch.hpp"
#include "implicit_octree_nns/detail/kd_tree.hpp"
#include "implicit_octree_nns/detail/naive_nearest_neighbor.hpp"
#include "implicit_octree_nns/detail/utility.hpp"
#include "implicit_octree_nns/model_point.hpp"
#include "implicit_octree_nns/point_traits.hpp"

using implicit_octree_nns::point_traits;
using implicit_octree_nns::detail::kd_tree;

using implicit_octree_nns::detail::generate_random_points;
using implicit_octree_nns::detail::naive_nearest_neighbor;

using implicit_octree_nns::model::make_point;
using implicit_octree_nns::model::point;

static constexpr auto MAX_COORD = 1e2;
static const auto uniform_distribution =
    std::uniform_real_distribution<std::remove_cv_t<decltype(MAX_COORD)>>(-MAX_COORD, MAX_COORD);

TEST_CASE("Testing 2D KD Tree by comparing to naive nearest neighbor") {
    constexpr auto dimension = 2;
    using point_type = point<double, dimension>;

    constexpr auto num_points = 1000;
    constexpr auto num_queries = 100;
    auto generator_seed = 5u;
    auto kdtree_seed = 23u;

    auto generator = std::mt19937{generator_seed};  // NOLINT
    auto point_set = generate_random_points<dimension>(num_points, generator, uniform_distribution);
    auto queries = generate_random_points<dimension>(num_queries, generator, uniform_distribution);

    auto naive_locator = naive_nearest_neighbor{point_set};
    auto locator = kd_tree<point_type>(point_set, kdtree_seed);
    std::cout << "Built" << std::endl;
    std::for_each(std::begin(queries), std::end(queries), [&](const auto& query_point) {
        auto expected_nearest_neighbor = naive_locator.find_nearest_neighbor(query_point);
        auto found_nearest_neighbor = locator.find_nearest_neighbor(query_point);
        auto expected_distance =
            point_traits<point_type>::distance_squared(expected_nearest_neighbor, query_point);
        auto found_distance =
            point_traits<point_type>::distance_squared(found_nearest_neighbor, query_point);
        CHECK(found_distance == Approx(expected_distance));
    });
}

TEST_CASE("Testing 3D KD Tree by comparing to naive nearest neighbor") {
    constexpr auto dimension = 3;
    using point_type = point<double, dimension>;

    constexpr auto num_points = 1000;
    constexpr auto num_queries = 10;
    auto generator_seed = 5u;
    auto kdtree_seed = 23u;

    auto generator = std::mt19937{generator_seed};  // NOLINT
    auto point_set = generate_random_points<dimension>(num_points, generator, uniform_distribution);
    auto queries = generate_random_points<dimension>(num_queries, generator, uniform_distribution);

    auto naive_locator = naive_nearest_neighbor{point_set};
    auto locator = kd_tree<point_type>(point_set, kdtree_seed);

    std::for_each(std::begin(queries), std::end(queries), [&](const auto& query_point) {
        auto expected_nearest_neighbor = naive_locator.find_nearest_neighbor(query_point);
        auto found_nearest_neighbor = locator.find_nearest_neighbor(query_point);
        auto expected_distance =
            point_traits<point_type>::distance_squared(expected_nearest_neighbor, query_point);
        auto found_distance =
            point_traits<point_type>::distance_squared(found_nearest_neighbor, query_point);
        CHECK(found_distance == Approx(expected_distance));
    });
}
