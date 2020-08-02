#ifndef IMPLICIT_OCTREE_NNS_EQUATION_HPP
#define IMPLICIT_OCTREE_NNS_EQUATION_HPP

#include <optional>

namespace implicit_octree_nns::detail {

template <typename coordinate_type, int dimension>
struct equation {
    static_assert(std::is_floating_point_v<coordinate_type>);
    static_assert(dimension == 2 || dimension == 3, "Only dimensions 2 and 3 are supported");
};

/**
 * @brief Linear function of 1 variable, represented as y = (slope) x + (constant)
 */
template <typename coordinate_type>
struct equation<coordinate_type, 2> {
    auto evaluate_at(const coordinate_type& position) const { return slope * position + constant; }

    /**
     * @return nullopt if there is zero or infinite intersections (lines are parallel/equal),
     * otherwise returns the unique position at which both lines evaluate to be the same value
     */
    auto intersection_with(const equation& other) const -> std::optional<coordinate_type> {
        if (slope == other.slope) {
            return std::nullopt;
        } else {
            return (other.constant - constant) / (slope - other.slope);
        }
    }

    auto operator==(const equation& other) const {
        return slope == other.slope && constant == other.constant;
    }

    coordinate_type slope;
    coordinate_type constant;
};

/**
 * @brief Linear function of 2 variables, represented as z = (slope1) x + (slope2) y + (constant)
 */
template <typename coordinate_type>
struct equation<coordinate_type, 3> {
    auto evaluate_at(const coordinate_type& position1, const coordinate_type& position2) const {
        return slope1 * position1 + slope2 * position2 + constant;
    }

    // [TODO]: Add implementation and documentation for 3d function intersection
    auto intersection_with(const equation& other) const -> std::optional<coordinate_type> {
        return std::nullopt;
    }

    auto operator==(const equation& other) const {
        return slope1 == other.slope1 && slope2 == other.slope2 && constant == other.constant;
    }

    coordinate_type slope1;
    coordinate_type slope2;
    coordinate_type constant;
};

template <typename T>
equation(T, T) -> equation<T, 2>;

template <typename T>
equation(T, T, T) -> equation<T, 3>;

}  // namespace implicit_octree_nns::detail

#endif  // IMPLICIT_OCTREE_NNS_EQUATION_HPP
