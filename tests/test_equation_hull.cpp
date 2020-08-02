#include "catch2/catch.hpp"
#include "implicit_octree_nns/detail/equation.hpp"
#include "implicit_octree_nns/detail/equation_hull.hpp"

using implicit_octree_nns::detail::equation;
using implicit_octree_nns::detail::equation_hull;

TEST_CASE("Check if trivial hull containing all equations is constructed") {
    equation a{-3., 9.};
    equation b{3., 0.};
    equation c{0., 2.};
    std::vector<decltype(a)> equations{a, b, c};
    equation_hull<double, 2> hull(std::begin(equations), std::end(equations));
    auto envelope = hull.lower_envelope();
    REQUIRE(envelope == std::deque{1, 2, 0});
}

TEST_CASE("Check if lines are ever excluded when creating equation hull") {
    equation a{1., 0.};
    equation b{0.2, 3.};
    equation c{0.1, 3.};
    std::vector<decltype(a)> equations{c, b, a};
    equation_hull<double, 2> hull(std::begin(equations), std::end(equations));

    auto envelope = hull.lower_envelope();
    REQUIRE(envelope == std::deque{2, 0});
}

TEST_CASE("Check if lines are ever excluded when creating equation hull 2") {
    equation a{1., 0.};
    equation b{-1., 1.};
    equation c{-5., 2.};
    std::vector<decltype(a)> equations{b, c, a};
    equation_hull<double, 2> hull(std::begin(equations), std::end(equations));
    auto envelope = hull.lower_envelope();
    REQUIRE(envelope == std::deque{2, 1});
}

TEST_CASE("Check if lines that are on the lower envelope at exactly one point are kept") {
    equation a{0., 0.};
    equation b{-1., 0.};
    equation c{1., 0.};
    std::vector<decltype(a)> equations{a, b, c};
    equation_hull<double, 2> hull(std::begin(equations), std::end(equations));
    auto envelope = hull.lower_envelope();
    REQUIRE(envelope == std::deque{2, 0, 1});
}

TEST_CASE("Check if equivalent slopes are handled properly") {
    equation a{0., 0.};
    equation b{0., 5.};
    equation c{-2., 0.};
    std::vector<decltype(a)> equations{a, b, c};
    equation_hull<double, 2> hull(std::begin(equations), std::end(equations));
    auto envelope = hull.lower_envelope();
    REQUIRE(envelope == std::deque{0, 2});
}

TEST_CASE("Check if equivalent slopes are handled properly 2") {
    equation a{0., 0.};
    equation b{0., -5.};
    equation c{2., 0.};
    std::vector<decltype(a)> equations{a, b, c};
    equation_hull<double, 2> hull(std::begin(equations), std::end(equations));
    auto envelope = hull.lower_envelope();
    REQUIRE(envelope == std::deque{2, 1});
}

TEST_CASE("Check if equivalent slopes are handled properly even if only one equation remains") {
    equation a{0., 0.};
    equation b{0., 5.};
    equation c{0., -3.};
    std::vector<decltype(a)> equations{a, b, c};
    equation_hull<double, 2> hull(std::begin(equations), std::end(equations));
    auto envelope = hull.lower_envelope();
    REQUIRE(envelope == std::deque{2});
}
