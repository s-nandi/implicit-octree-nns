#ifndef IMPLICIT_OCTREE_NNS_BOUNDING_BOX_HPP
#define IMPLICIT_OCTREE_NNS_BOUNDING_BOX_HPP

#include <array>
#include <cassert>
#include <cmath>
#include <cstdint>

#include "implicit_octree_nns/point_traits.hpp"

namespace implicit_octree_nns::detail {
template <typename point_type>
class bounding_box {
   public:
    using coordinate_type = typename point_traits<point_type>::coordinate_type;
    static constexpr auto dimension = point_traits<point_type>::dimension;

    constexpr bounding_box() = default;
    explicit constexpr bounding_box(const coordinate_type& magnitude) {
        for (size_t i = 0; i < 2 * dimension; i += 2) {
            minmax_per_dimension[i] = -magnitude;
            minmax_per_dimension[i + 1] = magnitude;
        }
    }
    // [TODO]: Add unit tests for mid/length (tests for other functions are in test_octree_cell
    constexpr auto min(size_t dim) const { return minmax_per_dimension[2 * dim]; }
    constexpr auto& min(size_t dim) { return minmax_per_dimension[2 * dim]; }
    constexpr auto max(size_t dim) const { return minmax_per_dimension[2 * dim + 1]; }
    constexpr auto& max(size_t dim) { return minmax_per_dimension[2 * dim + 1]; }
    constexpr auto mid(size_t dim) const {
        return min(dim) / coordinate_type{2} + max(dim) / coordinate_type{2};
    }
    constexpr auto length(size_t dim) const { return max(dim) - min(dim); }

    constexpr auto all_lengths_same() const {
        constexpr auto EPS = 1e-6;
        auto first = length(0);
        for (size_t dim = 0; dim < dimension; dim++) {
            if (abs(length(dim) - first) > EPS) {
                return false;
            }
        }
        return true;
    };

    constexpr auto smallest_corner() const {
        point_type corner{};
        for (size_t dim = 0; dim < dimension; dim++) {
            point_traits<point_type>::set(corner, dim, min(dim));
        }
        return corner;
    }

    constexpr auto step_at_depth(size_t dim, int depth) const {
        return length(dim) / (1LL << depth);
    }

    // [TODO]: Add unit tests for floored_corner
    constexpr auto floored_corner(const point_type& point, int depth) const {
        constexpr auto EPS = 1e-10;
        point_type corner{};
        for (size_t dim = 0; dim < dimension; dim++) {
            auto point_coordinate = point_traits<point_type>::get(point, dim);
            auto step = step_at_depth(dim, depth);
            auto delta = point_coordinate - min(dim);
            auto num_steps = delta / step;
            auto num_steps_floored = std::floor(num_steps + EPS);
            point_traits<point_type>::set(corner, dim, num_steps_floored);
        }
        return corner;
    }

    // [TODO]: Add unit tests for floored_box
    constexpr auto floored_box(const point_type& point, int depth) const {
        auto corner = floored_corner(point, depth);
        bounding_box<point_type> result{};
        for (size_t dim = 0; dim < dimension; dim++) {
            auto step_size = step_at_depth(dim, depth);
            result.min(dim) = point_traits<point_type>::get(corner, dim) * step_size + min(dim);
            result.max(dim) = result.min(dim) + step_size;
        }
        return result;
    }

    /**
     * @return All sub-boxes when this box is bisected by each dimension. The sub-boxes are
     * returned in lexicographical order of their smallest corner (based on coordinate values)
     */
    constexpr auto sub_boxes() const {
        constexpr auto num_children = 1u << dimension;
        std::array<bounding_box, num_children> children{};
        for (auto bitmask = 0u; bitmask < num_children; bitmask++) {
            for (size_t dim = 0; dim < dimension; dim++) {
                // If the dim-th bit of the bitmask is on, then we take the upper half of the box
                // when split by dimension dim, otherwise we take the lower half
                auto upper_half = ((1u << dim) & bitmask) != 0;
                children[bitmask].min(dim) = upper_half ? mid(dim) : min(dim);
                children[bitmask].max(dim) = upper_half ? max(dim) : mid(dim);
            }
            assert(children[bitmask].all_lengths_same());
        }
        return children;
    }

    constexpr auto contains(const point_type& point) const -> bool {
        for (size_t dim = 0; dim < dimension; dim++) {
            auto point_coord = point_traits<point_type>::get(point, dim);
            if (point_coord < min(dim) || point_coord > max(dim)) {
                return false;
            }
        }
        return true;
    }

    /**
     * @return A bitmask representing the octant/quadtrant of the bounding_box that point lies in.
     * The i-th bit of the bitmask will be on if the point is in the upper half (or on the halfway
     * point) of the bounding box for the i-th dimension.
     *
     * For example, with the bounding box spanning [-1, 1] in all 3 dimensions,
     * the octant index for point p = (-1,1,1) is (110) base 2 = 6
     */
    constexpr auto octant_index(const point_type& point) const {
        uint8_t octant = 0;
        for (size_t dim = 0; dim < dimension; dim++) {
            auto point_coord = point_traits<point_type>::get(point, dim);
            int upper_half = point_coord >= mid(dim);
            octant |= (upper_half << dim);
        }
        return octant;
    }  // [TODO]: Add unit tests for octant_index

   private:
    std::array<coordinate_type, 2u * dimension> minmax_per_dimension{};
};
}  // namespace implicit_octree_nns::detail

#endif  // IMPLICIT_OCTREE_NNS_BOUNDING_BOX_HPP
