#include "catch2/catch.hpp"
#include "implicit_octree_nns/detail/equation.hpp"

using implicit_octree_nns::detail::equation;

TEST_CASE("Testing equation 2 intersect with non-parallel lines") {
    equation eq1{3., 0.};
    equation eq2{0., 6.};
    equation eq3{-3., 9.};
    CHECK(*eq1.intersection_with(eq2) == 2.0);
    CHECK(*eq2.intersection_with(eq1) == 2.0);
    CHECK(*eq1.intersection_with(eq3) == 1.5);
    CHECK(*eq3.intersection_with(eq1) == 1.5);
    CHECK(*eq2.intersection_with(eq3) == 1.0);
    CHECK(*eq3.intersection_with(eq2) == 1.0);
}

TEST_CASE("Testing equation 2 intersect with non-parallel lines 2") {
    equation eq1{1., 0.};
    equation eq2{0.2, 3.};
    equation eq3{0.1, 3.};
    CHECK(*eq1.intersection_with(eq2) == Approx(3.75));
    CHECK(*eq2.intersection_with(eq1) == Approx(3.75));
    CHECK(*eq1.intersection_with(eq3) == Approx(10. / 3));
    CHECK(*eq3.intersection_with(eq1) == Approx(10. / 3));
    CHECK(*eq2.intersection_with(eq3) == Approx(0.));
    CHECK(*eq3.intersection_with(eq2) == Approx(0.));
}

TEST_CASE("Testing equation 2 intersect with parallel lines") {
    equation eq1{1000., 5.};
    equation eq2{1000., -100.};
    equation eq3{1000., 5.};
    CHECK(eq1.intersection_with(eq2) == std::nullopt);
    CHECK(eq2.intersection_with(eq1) == std::nullopt);
    CHECK(eq1.intersection_with(eq3) == std::nullopt);
    CHECK(eq3.intersection_with(eq1) == std::nullopt);
    CHECK(eq2.intersection_with(eq3) == std::nullopt);
    CHECK(eq3.intersection_with(eq2) == std::nullopt);
    CHECK(eq1.intersection_with(eq1) == std::nullopt);
    CHECK(eq2.intersection_with(eq2) == std::nullopt);
    CHECK(eq3.intersection_with(eq3) == std::nullopt);
}