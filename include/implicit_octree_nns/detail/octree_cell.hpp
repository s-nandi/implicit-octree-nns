#ifndef IMPLICIT_OCTREE_NNS_OCTREE_CELL_HPP
#define IMPLICIT_OCTREE_NNS_OCTREE_CELL_HPP

#include <algorithm>
#include <array>
#include <cassert>
#include <cstdint>
#include <stdexcept>
#include <vector>

#include "implicit_octree_nns/detail/axis_aligned.hpp"
#include "implicit_octree_nns/detail/bounding_box.hpp"
#include "implicit_octree_nns/detail/equation_hull.hpp"
#include "implicit_octree_nns/detail/utility.hpp"
#include "implicit_octree_nns/point_traits.hpp"

namespace implicit_octree_nns::detail {

template <typename point_type>
struct octree_cell {
    using coordinate_type = typename point_traits<point_type>::coordinate_type;
    static constexpr auto dimension = point_traits<point_type>::dimension;

    octree_cell() = default;
    template <typename ForwardIterator>
    explicit octree_cell(ForwardIterator point_set_begin, ForwardIterator point_set_end,
                         coordinate_type max_magnitude = 0)
        : points(point_set_begin, point_set_end) {
        if (max_magnitude == 0) {
            max_magnitude = get_max_magnitude(point_set_begin, point_set_end);
        }
        box = bounding_box<point_type>(max_magnitude);
    }

    auto split_into_children() const -> std::array<octree_cell, (1u << dimension)> {
        std::array<octree_cell, (1u << dimension)> children{};
        auto octants = box.sub_boxes();
        assert(octants.size() == children.size());
        for (size_t it = 0; it < octants.size(); it++) {
            children[it].box = octants[it];
            children[it].depth_ = depth() + 1;
        }
        std::vector<uint8_t> initial_bitmask(points.size());
        std::vector<uint8_t> transition_bitmask(points.size());
        for (size_t it = 0; it < points.size(); it++) {
            initial_bitmask[it] = box.octant_index(points[it]);
        }
        for (int dim = 0; dim < dimension; dim++) {
            axis_aligned<point_type> split_line{static_cast<axes>(dim), box.mid(dim)};
            std::vector<equation<coordinate_type, dimension>> equations(points.size());
            std::transform(std::begin(points), std::end(points), std::begin(equations),
                           [&split_line](const point_type& point) {
                               return split_line.make_distance_equation(point);
                           });
            equation_hull<coordinate_type, dimension> hull(std::begin(equations),
                                                           std::end(equations));
            int next_dim = (dim + 1) == dimension ? 0 : dim + 1;
            const auto& crossed_split_line =
                hull.lower_envelope(box.min(next_dim), box.max(next_dim));
            for (const auto& point_index : crossed_split_line) {
                transition_bitmask[point_index] |= (1 << dim);
            }
        }
        for (size_t it = 0; it < points.size(); it++) {
            for_each_submask(transition_bitmask[it], [&](int transition) {
                auto end_mask = initial_bitmask[it] ^ transition;
                children[end_mask].points.push_back(points[it]);
            });
        }
        return children;
    }

    auto closest_point(const point_type& query_point) const -> point_type {
        assert(!points.empty());
        return *std::min_element(std::begin(points), std::end(points),
                                 [&](const auto& cell_point_1, const auto& cell_point_2) {
                                     auto distance_1 = point_traits<point_type>::distance_squared(
                                         query_point, cell_point_1);
                                     auto distance_2 = point_traits<point_type>::distance_squared(
                                         query_point, cell_point_2);
                                     return distance_1 < distance_2;
                                 });
    }

    auto length(int dim) const { return box.length(dim); }
    auto size() const { return points.size(); }
    auto depth() const { return depth_; }

    bounding_box<point_type> box{};
    std::vector<point_type> points{};
    int depth_{0};
};

}  // namespace implicit_octree_nns::detail

#endif  // IMPLICIT_OCTREE_NNS_OCTREE_CELL_HPP
