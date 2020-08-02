#include "catch2/catch.hpp"

#include "implicit_octree_nns/model_point.hpp"
#include "implicit_octree_nns/point_traits.hpp"

using implicit_octree_nns::point_traits;

using implicit_octree_nns::model::fpoint2;
using implicit_octree_nns::model::fpoint3;
using implicit_octree_nns::model::make_point;

using point_2_traits = point_traits<fpoint2>;
using point_3_traits = point_traits<fpoint3>;

TEST_CASE("Testing a default 2D point's coordinates") {
    auto pt = fpoint2{};
    auto x_coordinate = point_2_traits::get(pt, 0);
    auto y_coordinate = point_2_traits::get(pt, 1);
    CHECK(x_coordinate == 0.0);
    CHECK(y_coordinate == 0.0);
}

TEST_CASE("Testing a default 3D point's coordinates") {
    auto pt = fpoint3{};
    auto x_coordinate = point_3_traits::get(pt, 0);
    auto y_coordinate = point_3_traits::get(pt, 1);
    auto z_coordinate = point_3_traits::get(pt, 2);
    CHECK(x_coordinate == 0.0);
    CHECK(y_coordinate == 0.0);
    CHECK(z_coordinate == 0.0);
}

TEST_CASE("Testing a 2D point's accessors") {
    auto pt = make_point(3.0, 4.0);
    CHECK(point_2_traits::get(pt, 0) == 3.0);
    CHECK(point_2_traits::get(pt, 1) == 4.0);
    point_2_traits::set(pt, 0, 5.0);
    point_2_traits::set(pt, 1, -10.0);
    CHECK(point_2_traits::get(pt, 0) == 5.0);
    CHECK(point_2_traits::get(pt, 1) == -10.0);
}

TEST_CASE("Testing a 3D point's accessors") {
    auto pt = make_point(3.0, 4.0, 5.0);
    CHECK(point_3_traits::get(pt, 0) == 3.0);
    CHECK(point_3_traits::get(pt, 1) == 4.0);
    CHECK(point_3_traits::get(pt, 2) == 5.0);
    point_3_traits::set(pt, 0, 0.0);
    point_3_traits::set(pt, 1, -10.0);
    point_3_traits::set(pt, 2, -20.0);
    CHECK(point_3_traits::get(pt, 0) == 0.0);
    CHECK(point_3_traits::get(pt, 1) == -10.0);
    CHECK(point_3_traits::get(pt, 2) == -20.0);
}
