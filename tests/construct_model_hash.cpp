#include <optional>
#include <utility>

#include "catch2/catch.hpp"
#include "implicit_octree_nns/hash_table_traits.hpp"
#include "implicit_octree_nns/model_hash_table.hpp"
#include "implicit_octree_nns/model_point.hpp"

using implicit_octree_nns::hash_table_traits;
using implicit_octree_nns::point_traits;
using implicit_octree_nns::model::hash_table;
using implicit_octree_nns::model::make_point;

using point_type = decltype(make_point(0., 0.));
using hash_table_type = hash_table<point_type>;

TEST_CASE("Check insert and at for model hash table of point depth pairs") {
    auto pairs =
        std::vector{std::pair{make_point(0., 3.), 3}, std::pair{make_point(13., -500.), 12},
                    std::pair{make_point(0., 3.), 0}, std::pair{make_point(0., 0.), 0}};
    auto values = std::vector{0, 1, 2, 3};
    assert(std::size(pairs) == std::size(values));
    hash_table_type ht{};
    for (size_t it = 0; it < std::size(pairs); it++) {
        hash_table_traits<hash_table_type>::insert(ht, pairs[it], values[it]);
    }
    for (size_t it = 0; it < std::size(pairs); it++) {
        auto got = hash_table_traits<hash_table_type>::at(ht, pairs[it]);
        REQUIRE(got != std::nullopt);
        CHECK(*got == values[it]);
    }
}

TEST_CASE("Check model hash table's at with missing values") {
    hash_table_type ht{};
    CHECK(hash_table_traits<hash_table_type>::at(ht, {{1., 2.}, 0}) == std::nullopt);
    hash_table_traits<hash_table_type>::insert(ht, {{1., 2.}, 0}, 5);
    auto got = hash_table_traits<hash_table_type>::at(ht, {{1.0, 2.0}, 0});
    REQUIRE(got != std::nullopt);
    CHECK(*got == 5);
    CHECK(hash_table_traits<hash_table_type>::at(ht, {{1., 2.}, 1}) == std::nullopt);
    CHECK(hash_table_traits<hash_table_type>::at(ht, {{1., 2.}, -1}) == std::nullopt);
    CHECK(hash_table_traits<hash_table_type>::at(ht, {{1.001, 2.}, 0}) == std::nullopt);
    CHECK(hash_table_traits<hash_table_type>::at(ht, {{1., 1.999}, 0}) == std::nullopt);
    CHECK(hash_table_traits<hash_table_type>::at(ht, {{1., -2.}, 0}) == std::nullopt);
    CHECK(hash_table_traits<hash_table_type>::at(ht, {{-1., 2.}, 0}) == std::nullopt);
    CHECK(hash_table_traits<hash_table_type>::at(ht, {{-1., -2.}, 0}) == std::nullopt);
    CHECK(hash_table_traits<hash_table_type>::at(ht, {{237., 59000.}, 0}) == std::nullopt);
}