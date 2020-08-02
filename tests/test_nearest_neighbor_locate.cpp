#include <algorithm>
#include <fstream>
#include <iostream>
#include <random>
#include <vector>

#include "catch2/catch.hpp"
#include "implicit_octree_nns/detail/naive_nearest_neighbor.hpp"
#include "implicit_octree_nns/detail/utility.hpp"
#include "implicit_octree_nns/model_point.hpp"
#include "implicit_octree_nns/nearest_neighbor.hpp"
#include "implicit_octree_nns/point_traits.hpp"

using implicit_octree_nns::nearest_neighbor;
using implicit_octree_nns::point_traits;

using implicit_octree_nns::detail::bounding_box;
using implicit_octree_nns::detail::generate_random_points;
using implicit_octree_nns::detail::naive_nearest_neighbor;

using implicit_octree_nns::model::make_point;
using implicit_octree_nns::model::point;

static constexpr auto MAX_COORD = 1e2;
static const auto uniform_distribution =
    std::uniform_real_distribution<std::remove_cv_t<decltype(MAX_COORD)>>(-MAX_COORD, MAX_COORD);

TEST_CASE("Testing naive nearest neighbor with our model point") {
    auto bottom_left = make_point(-5.0, -5.0);
    auto bottom_right = make_point(5.0, -5.0);
    auto top_left = make_point(-5.0, 5.0);
    auto top_right = make_point(5.0, 5.0);

    auto query_outside_bottom_left = make_point(-10.0, -4.9);
    auto query_outside_bottom_right = make_point(6.3, -8.5);
    auto query_outside_top_left = make_point(-5.0, 6.0);
    auto query_outside_top_right = make_point(10.0, 10.0);

    auto query_inside_bottom_left = make_point(-4.0, -4.0);
    auto query_inside_bottom_right = make_point(4.9999, -4.9999);
    auto query_inside_top_left = make_point(-5.0, 5.0);
    auto query_inside_top_right = make_point(4.51, 4.5);

    auto points = std::vector{bottom_left, bottom_right, top_left, top_right};
    auto locator = naive_nearest_neighbor{points};

    CHECK(locator.find_nearest_neighbor(query_outside_bottom_left) == bottom_left);
    CHECK(locator.find_nearest_neighbor(query_outside_bottom_right) == bottom_right);
    CHECK(locator.find_nearest_neighbor(query_outside_top_left) == top_left);
    CHECK(locator.find_nearest_neighbor(query_outside_top_right) == top_right);
    CHECK(locator.find_nearest_neighbor(query_inside_bottom_left) == bottom_left);
    CHECK(locator.find_nearest_neighbor(query_inside_bottom_right) == bottom_right);
    CHECK(locator.find_nearest_neighbor(query_inside_top_left) == top_left);
    CHECK(locator.find_nearest_neighbor(query_inside_top_right) == top_right);
}

TEST_CASE("Testing 2D octree based nearest neighbor by comparing to naive nearest neighbor") {
    constexpr auto dimension = 2;
    using point_type = point<double, dimension>;

    constexpr auto num_points = 1000;
    constexpr auto num_queries = 100;
    auto generator_seed = 5u;

    auto generator = std::mt19937{generator_seed};  // NOLINT
    auto point_set = generate_random_points<dimension>(num_points, generator, uniform_distribution);
    auto queries = generate_random_points<dimension>(num_queries, generator, uniform_distribution);

    auto naive_locator = naive_nearest_neighbor{point_set};
    auto locator =
        nearest_neighbor<point_type>(std::begin(point_set), std::end(point_set), MAX_COORD);

    std::for_each(std::begin(queries), std::end(queries), [&](const auto& query_point) {
        auto expected_nearest_neighbor = naive_locator.find_nearest_neighbor(query_point);
        auto found_nearest_neighbor = locator.find_nearest_neighbor(query_point);
        auto expected_distance =
            point_traits<point_type>::distance_squared(expected_nearest_neighbor, query_point);
        auto found_distance =
            point_traits<point_type>::distance_squared(found_nearest_neighbor, query_point);
        REQUIRE(found_distance == Approx(expected_distance));
    });
}

TEST_CASE("Testing 3D octree based nearest neighbor by comparing to naive nearest neighbor") {
    constexpr auto dimension = 3;
    using point_type = point<double, dimension>;

    constexpr auto num_points = 1000;
    constexpr auto num_queries = 10;
    auto generator_seed = 5u;

    auto generator = std::mt19937{generator_seed};  // NOLINT
    auto point_set = generate_random_points<dimension>(num_points, generator, uniform_distribution);
    auto queries = generate_random_points<dimension>(num_queries, generator, uniform_distribution);

    auto naive_locator = naive_nearest_neighbor{point_set};
    auto locator =
        nearest_neighbor<point_type>{std::begin(point_set), std::end(point_set), MAX_COORD};

    std::for_each(std::begin(queries), std::end(queries), [&](const auto& query_point) {
        auto expected_nearest_neighbor = naive_locator.find_nearest_neighbor(query_point);
        auto found_nearest_neighbor = locator.find_nearest_neighbor(query_point);
        auto expected_distance =
            point_traits<point_type>::distance_squared(expected_nearest_neighbor, query_point);
        auto found_distance =
            point_traits<point_type>::distance_squared(found_nearest_neighbor, query_point);
        REQUIRE(found_distance == Approx(expected_distance));
    });
}

TEST_CASE("Testing nearest_neighbor's size() function") {
    constexpr auto dimension = 2;

    auto generator_seed = 5u;
    auto generator = std::mt19937{generator_seed};  // NOLINT
    for (int num_points : std::vector{0, 1, 2, 10}) {
        auto point_set =
            generate_random_points<dimension>(num_points, generator, uniform_distribution);
        auto locator = nearest_neighbor{std::begin(point_set), std::end(point_set)};
        REQUIRE(locator.construction_set_size() == num_points);
    }
}
