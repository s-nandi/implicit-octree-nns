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

TEST_CASE(
    "Check if the bounding box of an initialized octree cell contains the entire point set (3D)") {
    constexpr auto dimension = 3;
    using coordinate_type = double;
    using point_type = point<coordinate_type, dimension>;

    constexpr auto num_points = 10000;
    auto generator_seed = 10u;

    auto generator = std::mt19937{generator_seed};  // NOLINT
    auto point_set = generate_random_points<dimension>(
        num_points, generator, std::uniform_real_distribution<coordinate_type>(-1e6, 1e6));

    auto cell = octree_cell<point_type>(std::begin(point_set), std::end(point_set));

    auto point_in_box = [&](const point_type& pt, const bounding_box<point_type>& box) {
        for (int dim = 0; dim < dimension; dim++) {
            auto pt_coordinate = point_traits<point_type>::get(pt, dim);
            auto box_min = box.min(dim);
            auto box_max = box.max(dim);
            if (pt_coordinate < box_min || pt_coordinate > box_max) {
                return false;
            }
        }
        return true;
    };

    std::for_each(std::begin(cell.points), std::end(cell.points), [&](const auto& pt) {
        auto pt_contained_in_bounding_box = point_in_box(pt, cell.box);
        REQUIRE(pt_contained_in_bounding_box);
    });
}

TEST_CASE("Check lower envelope with upper/lower bounds for splitting points") {
    constexpr auto dimension = 2;
    using coordinate_type = double;
    using point_type = point<coordinate_type, dimension>;
    axis_aligned<point_type> split_line{axes::y, 0.};
    std::vector<point_type> points = {
        point_type{{-198.676, -174.693}},
        point_type{{-183.147, -43.9825}},
        point_type{{-195.802, -66.6654}},
        point_type{{-186.523, 13.5014}},
    };
    std::vector<equation<coordinate_type, dimension>> equations(points.size());
    std::transform(std::begin(points), std::end(points), std::begin(equations),
                   [&split_line](const point_type& point) {
                       return split_line.make_distance_equation(point);
                   });
    equation_hull<coordinate_type, dimension> hull(std::begin(equations), std::end(equations));
    const auto& crossed_split_line = hull.lower_envelope(-200., -100.);
    CHECK(std::find(std::begin(crossed_split_line), std::end(crossed_split_line), 0) ==
          std::end(crossed_split_line));
}