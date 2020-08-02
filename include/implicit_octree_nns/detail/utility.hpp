#ifndef IMPLICIT_OCTREE_NNS_UTILITY_HPP
#define IMPLICIT_OCTREE_NNS_UTILITY_HPP

#include <cstdint>
#include <random>
#include <vector>

#include "implicit_octree_nns/model_point.hpp"
#include "implicit_octree_nns/point_traits.hpp"

namespace implicit_octree_nns::detail {

enum class bsearch_result { too_shallow, too_deep, just_right };

template <typename Callback>
void for_each_submask(uint8_t bitmask, Callback callback) {
    for (unsigned int submask = bitmask;; submask = (submask - 1) & bitmask) {
        callback(submask);
        if (submask == 0) {
            break;
        }
    }
}

template <typename ForwardIterator>
auto get_max_magnitude(ForwardIterator point_set_begin, ForwardIterator point_set_end) {
    using point_type = typename std::iterator_traits<ForwardIterator>::value_type;
    using coordinate_type = typename point_traits<point_type>::coordinate_type;
    using std::abs;

    if (point_set_begin == point_set_end) {
        return coordinate_type{0};
    }

    auto max_magnitude_of_point = [](const point_type& p) {
        auto max_magnitude = coordinate_type{0};
        for (auto dim = 0; dim < point_traits<point_type>::dimension; dim++) {
            auto magnitude_for_dim = abs(point_traits<point_type>::get(p, dim));
            max_magnitude = std::max(max_magnitude, magnitude_for_dim);
        }
        return max_magnitude;
    };

    auto point_with_max_magnitude = *std::max_element(
        point_set_begin, point_set_end, [&](const point_type& p, const point_type& q) {
            auto p_magnitude = max_magnitude_of_point(p);
            auto q_magnitude = max_magnitude_of_point(q);
            return p_magnitude < q_magnitude;
        });
    return max_magnitude_of_point(point_with_max_magnitude);
}

/** Random model point generator */

template <int dimension, typename distribution_type>
auto generate_random_points(int n, std::mt19937& generator, distribution_type distribution) {
    using coordinate_type = double;
    using point_type = implicit_octree_nns::model::point<coordinate_type, dimension>;
    auto result = std::vector<point_type>(n);
    for (auto& pt : result) {
        std::array<coordinate_type, dimension> coordinates{};
        for (int dim = 0; dim < dimension; dim++) {
            coordinates[dim] = static_cast<coordinate_type>(distribution(generator));
        }
        pt = point_type{coordinates};
    }
    return result;
}

}  // namespace implicit_octree_nns::detail

#endif  // IMPLICIT_OCTREE_NNS_UTILITY_HPP
