#ifndef IMPLICIT_OCTREE_NNS_EQUATION_HULL_HPP
#define IMPLICIT_OCTREE_NNS_EQUATION_HULL_HPP

#include <algorithm>
#include <cassert>
#include <deque>
#include <iostream>
#include <limits>
#include <numeric>
#include <utility>

#include "implicit_octree_nns/detail/equation.hpp"

namespace implicit_octree_nns::detail {

template <typename coordinate_type, int dimension>
class equation_hull {
    static_assert(std::is_floating_point_v<coordinate_type>);
    static_assert(dimension == 2 || dimension == 3);
};

/**
 * Maintains or constructs the lower hull for a set of 2d equations. If equations are inserted
 * dynamically rather than provided offline, these equations must be sorted by decreasing slope
 *
 * @pre All of the equations must be distinct (slope and constant cannot both be same)
 */
template <typename coordinate_type>
class equation_hull<coordinate_type, 2> {
    static constexpr auto dimension = 2;

   public:
    using equation_type = equation<coordinate_type, dimension>;
    template <typename RandomAccessIterator>
    equation_hull(RandomAccessIterator begin, RandomAccessIterator end) {
        std::vector<int> indices(std::distance(begin, end));
        std::iota(std::begin(indices), std::end(indices), 0);
        std::sort(std::begin(indices), std::end(indices), [&begin](const auto& i, const auto& j) {
            const auto a = *std::next(begin, i);
            const auto b = *std::next(begin, j);
            return std::tie(a.slope, a.constant) > std::tie(b.slope, b.constant);
        });
        std::for_each(std::begin(indices), std::end(indices),
                      [&](int i) { push(*std::next(begin, i), i); });
    }
    auto push(const equation_type& eq, int tag) {
        assert(std::empty(hull) || eq.slope <= min_slope);
        min_slope = eq.slope;
        if (!std::empty(hull) && eq.slope == hull.back().slope) {
            hull.pop_back();
            indices_on_hull.pop_back();
        }
        while (std::size(hull) >= 2 && makes_back_redundant(eq)) {
            hull.pop_back();
            indices_on_hull.pop_back();
        }
        hull.push_back(eq);
        indices_on_hull.push_back(tag);
    }
    /**
     * @return the indices of the equations that are on the lower envelope of all equations. If the
     * constructor was used to create this object, the indices are 0-indexed offsets from the
     * beginning of the input range (ie. the 'begin' parameter)
     */
    auto lower_envelope(coordinate_type lo = -std::numeric_limits<coordinate_type>::max(),
                        coordinate_type hi = std::numeric_limits<coordinate_type>::max()) const {
        size_t begin = 0;
        size_t end = std::size(hull);
        while (end - begin >= 2) {
            const auto& head = hull[begin];
            const auto& next = hull[begin + 1];
            if (head.intersection_with(next) < lo) {
                begin++;
            } else {
                break;
            }
        }
        while (end - begin >= 2) {
            const auto& tail = hull[end - 1];
            const auto& prev = hull[end - 2];
            if (prev.intersection_with(tail) > hi) {
                end--;
            } else {
                break;
            }
        }
        return std::deque(std::begin(indices_on_hull) + begin, std::begin(indices_on_hull) + end);
    }

   private:
    auto makes_back_redundant(const equation_type& eq) {
        auto last = hull[std::size(hull) - 1];
        auto second_last = hull[std::size(hull) - 2];
        auto intersection_eq_with_second_last = eq.intersection_with(second_last);
        auto intersection_current_last_two = last.intersection_with(second_last);

        assert(intersection_eq_with_second_last != std::nullopt);
        assert(intersection_current_last_two != std::nullopt);
        return *intersection_eq_with_second_last < *intersection_current_last_two;
    }

    std::deque<equation_type> hull;
    std::deque<int> indices_on_hull;
    coordinate_type min_slope;
};

/**
 * Maintains or constructs the lower hull for a set of 3d equations.
 *
 * @pre All of the equations must be distinct (slope and constant cannot both be same)
 */
template <typename coordinate_type>
class equation_hull<coordinate_type, 3> {
    static constexpr auto dimension = 3;

   public:
    using equation_type = equation<coordinate_type, dimension>;
    template <typename RandomAccessIterator>
    equation_hull(RandomAccessIterator begin, RandomAccessIterator end) {}
    auto push(const equation_type& eq, int tag) {}
    auto lower_envelope(coordinate_type lo = -std::numeric_limits<coordinate_type>::max(),
                        coordinate_type hi = std::numeric_limits<coordinate_type>::max()) const {
        return std::deque{0};
    }
    // [TODO]: This needs to either be handled better since 3d has more than just [lo, hi] (it will
    // need two lo's and two hi's), or prove that these extra extraneous intersections "at infinity"
    // don't increase # of cells drastically
};

// [TODO]: Add implementation for equation hull<coordinate_type, 3> specialization

}  // namespace implicit_octree_nns::detail

#endif  // IMPLICIT_OCTREE_NNS_EQUATION_HULL_HPP
