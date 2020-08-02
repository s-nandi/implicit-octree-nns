#include "catch2/catch.hpp"
#include "implicit_octree_nns/detail/axis_aligned.hpp"
#include "implicit_octree_nns/detail/equation.hpp"
#include "implicit_octree_nns/model_point.hpp"
#include "implicit_octree_nns/point_traits.hpp"

using implicit_octree_nns::point_traits;
using implicit_octree_nns::detail::axes;
using implicit_octree_nns::detail::axis_aligned;
using implicit_octree_nns::detail::equation;
using implicit_octree_nns::model::make_point;
using implicit_octree_nns::model::point;

using coordinate_type = double;
static constexpr auto dimension = 2;
using point_type = point<coordinate_type, dimension>;

auto get_point_on_line(const axis_aligned<point_type>& line, const coordinate_type& position) {
    auto pt = point<double, 2>{};
    for (size_t dim = 0; dim < 2; dim++) {
        if (dim == line.fixed_dimension) {
            point_traits<point_type>::set(pt, dim, line.position);
        } else {
            point_traits<point_type>::set(pt, dim, position);
        }
    }
    return pt;
}

TEST_CASE("Generate 2d equations with horizontal axis-aligned line") {
    auto split_line = axis_aligned<point_type>{axes::y, 2.};
    auto points = std::vector{make_point(-5., 3.), make_point(100., -1.)};
    auto expecteds = std::vector{equation{10., 26.}, equation{-200., 10009.}};
    assert(std::size(points) == std::size(expecteds));
    for (size_t it = 0; it < std::size(points); it++) {
        const auto& pt = points[it];
        const auto& expected = expecteds[it];
        auto equation = split_line.make_distance_equation(pt);
        CHECK(equation == expected);
        for (const auto& free_dimension_position : {-100., 10., 0., 20., 30., 500.}) {
            auto pt_on_line = get_point_on_line(split_line, free_dimension_position);
            auto ignored_square_term = free_dimension_position * free_dimension_position;
            auto expected_distance = point_traits<point_type>::distance_squared(pt_on_line, pt);
            auto equation_distance = equation.evaluate_at(free_dimension_position);
            CHECK(expected_distance == equation_distance + ignored_square_term);
        }
    }
}

TEST_CASE("Generate 2d equations with vertical axis-aligned line") {
    auto split_line = axis_aligned<point_type>{axes::x, -5.};
    auto points = std::vector{make_point(0., 0.), make_point(-20., -30.)};
    auto expecteds = std::vector{equation{0., 25.}, equation{60., 1125.}};
    assert(std::size(points) == std::size(expecteds));
    for (size_t it = 0; it < std::size(points); it++) {
        const auto& pt = points[it];
        const auto& expected = expecteds[it];
        auto equation = split_line.make_distance_equation(pt);
        CHECK(equation == expected);
        for (const auto& free_dimension_position : {-100., 10., 0., 20., 30., 500.}) {
            auto pt_on_line = get_point_on_line(split_line, free_dimension_position);
            auto ignored_square_term = free_dimension_position * free_dimension_position;
            auto expected_distance = point_traits<point_type>::distance_squared(pt_on_line, pt);
            auto equation_distance = equation.evaluate_at(free_dimension_position);
            CHECK(expected_distance == equation_distance + ignored_square_term);
        }
    }
}
