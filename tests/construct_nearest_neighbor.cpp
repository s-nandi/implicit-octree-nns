#include <array>
#include <deque>
#include <type_traits>
#include <vector>

#include "catch2/catch.hpp"
#include "implicit_octree_nns/model_point.hpp"
#include "implicit_octree_nns/nearest_neighbor.hpp"

using implicit_octree_nns::nearest_neighbor;

using implicit_octree_nns::model::fpoint2;
using implicit_octree_nns::model::fpoint3;
using implicit_octree_nns::model::make_point;

TEST_CASE("Testing 2D nearest neighbor's iterator range constructor from std::array") {
    auto p = make_point(0.0, 0.0);
    auto q = make_point(5.0, 0.0);
    auto r = make_point(5.0, 5.0);
    auto s = make_point(0.0, 5.0);

    std::array points{p, q, r, s};

    auto nns = nearest_neighbor(points.begin(), points.end());
    CHECK(nns.dimension == 2);
    CHECK(nns.construction_set_size() == 4);
}

TEMPLATE_TEST_CASE(
    "Testing 2D nearest neighbor's iterator range constructor from resizable stl containers", "",
    std::vector<fpoint2>, std::deque<fpoint2>) {
    auto p = make_point(0.0, 0.0);
    auto q = make_point(5.0, 0.0);
    auto r = make_point(5.0, 5.0);
    auto s = make_point(0.0, 5.0);

    TestType points{p, q, r, s};

    auto nns = nearest_neighbor(points.begin(), points.end());
    CHECK(nns.dimension == 2);
    CHECK(nns.construction_set_size() == 4);
}

TEST_CASE("Testing 3D nearest neighbor's iterator range constructor from std::array") {
    auto p = make_point(0.0, 0.0, 0.0);
    auto q = make_point(5.0, 0.0, 5.0);
    auto r = make_point(5.0, 5.0, -5.0);
    auto s = make_point(0.0, 5.0, 0.0);

    std::array points{p, q, r, s};

    auto nns = nearest_neighbor(points.begin(), points.end());
    CHECK(nns.dimension == 3);
    CHECK(nns.construction_set_size() == 4);
}

TEMPLATE_TEST_CASE(
    "Testing 3D nearest neighbor's iterator range constructor from resizable stl containers", "",
    std::vector<fpoint3>, std::deque<fpoint3>) {
    auto p = make_point(0.0, 0.0, 0.0);
    auto q = make_point(5.0, 0.0, 5.0);
    auto r = make_point(5.0, 5.0, -5.0);
    auto s = make_point(0.0, 5.0, 0.0);

    TestType points{p, q, r, s};

    auto nns = nearest_neighbor(points.begin(), points.end());
    CHECK(nns.dimension == 3);
    CHECK(nns.construction_set_size() == 4);
}
