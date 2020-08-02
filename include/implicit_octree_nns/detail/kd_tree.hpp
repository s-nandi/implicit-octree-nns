#ifndef IMPLICIT_OCTREE_NNS_KD_TREE_HPP
#define IMPLICIT_OCTREE_NNS_KD_TREE_HPP

#include <algorithm>
#include <cassert>
#include <limits>
#include <optional>
#include <random>
#include <vector>

#include "implicit_octree_nns/point_traits.hpp"

namespace implicit_octree_nns::detail {

template <typename point_type>
struct cmp_by_dim {
    size_t dim;
    cmp_by_dim(size_t dim_) : dim(dim_) {}
    bool operator()(const point_type &a, const point_type &b) {
        return point_traits<point_type>::get(a, dim) < point_traits<point_type>::get(b, dim);
    }
};

template <typename point_type>
class kd_tree {
   public:
    using coordinate_type = typename point_traits<point_type>::coordinate_type;
    constexpr static auto dimension = point_traits<point_type>::dimension;
    constexpr static auto INF = std::numeric_limits<coordinate_type>::max() / 4;

    kd_tree() = default;
    template <typename Container>
    kd_tree(Container container, unsigned seed)
        : kd_tree(std::begin(container), std::end(container), seed) {}

    auto find_nearest_neighbor(const point_type &query_point) -> point_type {
        auto distance_nearest_neighbor = std::pair{INF, point_type{}};
        query_helper(query_point, 0, distance_nearest_neighbor);
        return distance_nearest_neighbor.second;
    }

    auto size() const { return std::size(nodes_); }

   private:
    template <typename RandomAccessIterator>
    kd_tree(RandomAccessIterator begin, RandomAccessIterator end, unsigned seed) : nodes_{} {
        std::shuffle(begin, end, std::mt19937(seed));
        auto root = build(begin, end, 0);
        assert(root == 0);
    }

    auto next_dimension(size_t dim) const { return dim + 1 == dimension ? 0 : dim + 1; }

    template <typename RandomAccessIterator>
    auto build(RandomAccessIterator begin, RandomAccessIterator end, size_t dim)
        -> std::optional<size_t> {
        if (begin == end) return std::nullopt;

        auto mid = begin + std::distance(begin, end) / 2;
        nth_element(begin, mid, end, cmp_by_dim<point_type>(dim));

        auto curr_index = std::size(nodes_);
        auto &curr = nodes_.emplace_back();

        auto left_child_index = build(begin, mid, next_dimension(dim));
        auto right_child_index = build(mid + 1, end, next_dimension(dim));

        nodes_[curr_index] = node{*mid, left_child_index, right_child_index, dim};
        // pause here
        assert(left_child_index != std::optional{1000000});
        return curr_index;
    }

    auto query_helper(const point_type &query_point, std::optional<size_t> curr_index,
                      std::pair<coordinate_type, point_type> &distance_nearest_neighbor) {
        if (curr_index == std::nullopt) return;

        auto curr_point = nodes_[*curr_index].point;
        auto distance_to_query =
            point_traits<point_type>::distance_squared(curr_point, query_point);
        auto candidate_nearest_neighbor = std::pair{distance_to_query, curr_point};

        distance_nearest_neighbor = distance_to_query < distance_nearest_neighbor.first
                                        ? candidate_nearest_neighbor
                                        : distance_nearest_neighbor;

        auto split_axis = nodes_[*curr_index].split_axis;
        std::optional<size_t> first_child;
        std::optional<size_t> second_child;
        auto cmp = cmp_by_dim<point_type>{split_axis};
        if (cmp(curr_point, query_point)) {
            first_child = nodes_[*curr_index].right_child_index;
            second_child = nodes_[*curr_index].left_child_index;
        } else {
            first_child = nodes_[*curr_index].left_child_index;
            second_child = nodes_[*curr_index].right_child_index;
        }

        query_helper(query_point, first_child, distance_nearest_neighbor);
        if (second_child != std::nullopt) {
            auto distance_along_axis = point_traits<point_type>::get(query_point, split_axis) -
                                       point_traits<point_type>::get(curr_point, split_axis);
            if (distance_along_axis * distance_along_axis < distance_nearest_neighbor.first) {
                query_helper(query_point, second_child, distance_nearest_neighbor);
            }
        }
    }

    struct node {
        point_type point{};
        std::optional<size_t> left_child_index{};
        std::optional<size_t> right_child_index{};
        size_t split_axis{};
    };

    std::vector<node> nodes_{};
};

}  // namespace implicit_octree_nns::detail

#endif  // IMPLICIT_OCTREE_NNS_KD_TREE_HPP
