#include "catch2/catch.hpp"
#include "implicit_octree_nns/detail/axis_aligned.hpp"
#include "implicit_octree_nns/detail/bounding_box.hpp"
#include "implicit_octree_nns/detail/equation.hpp"
#include "implicit_octree_nns/detail/equation_hull.hpp"
#include "implicit_octree_nns/detail/octree_cell.hpp"
#include "implicit_octree_nns/detail/utility.hpp"
#include "implicit_octree_nns/model_point.hpp"
#include "implicit_octree_nns/point_traits.hpp"

using implicit_octree_nns::point_traits;

using implicit_octree_nns::detail::axes;
using implicit_octree_nns::detail::axis_aligned;
using implicit_octree_nns::detail::bounding_box;
using implicit_octree_nns::detail::equation;
using implicit_octree_nns::detail::equation_hull;
using implicit_octree_nns::detail::generate_random_points;
using implicit_octree_nns::detail::octree_cell;

using implicit_octree_nns::model::point;

TEST_CASE("Initialize 3D bounding box") {
    auto box3 = bounding_box<point<double, 3>>(16.0);
    for (int dim = 0; dim < 3; dim++) {
        CHECK(box3.min(dim) == -16.0);
        CHECK(box3.max(dim) == 16.0);
    }
}

TEST_CASE("Split 2D bounding box") {
    auto box2 = bounding_box<point<double, 2>>(16.0);
    auto children = box2.sub_boxes();
    CHECK(children.size() == 4);

    CHECK(children[0].min(0) == -16.0);
    CHECK(children[0].max(0) == 0.0);
    CHECK(children[0].min(1) == -16.0);
    CHECK(children[0].max(1) == 0.0);

    CHECK(children[1].min(0) == 0.0);
    CHECK(children[1].max(0) == 16.0);
    CHECK(children[1].min(1) == -16.0);
    CHECK(children[1].max(1) == 0.0);

    CHECK(children[2].min(0) == -16.0);
    CHECK(children[2].max(0) == 0.0);
    CHECK(children[2].min(1) == 0.0);
    CHECK(children[2].max(1) == 16.0);

    CHECK(children[3].min(0) == 0.0);
    CHECK(children[3].max(0) == 16.0);
    CHECK(children[3].min(1) == 0.0);
    CHECK(children[3].max(1) == 16.0);
}

TEST_CASE("Test smallest corner of 3D bounding box") {
    using point_type = point<double, 3>;

    auto box3 = bounding_box<point_type>(128.0);
    auto corner = box3.smallest_corner();
    for (int dim = 0; dim < 3; dim++) {
        CHECK(point_traits<point_type>::get(corner, dim) == -128.0);
    }
    auto children = box3.sub_boxes();
    auto child_corner = children[5].smallest_corner();
    CHECK(point_traits<point_type>::get(child_corner, 0) == 0.0);
    CHECK(point_traits<point_type>::get(child_corner, 1) == -128.0);
    CHECK(point_traits<point_type>::get(child_corner, 2) == 0.0);
}
