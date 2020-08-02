#ifndef IMPLICIT_OCTREE_NNS_AXIS_ALIGNED_HPP
#define IMPLICIT_OCTREE_NNS_AXIS_ALIGNED_HPP

#include <stdexcept>
#include <vector>

#include "implicit_octree_nns/detail/equation.hpp"
#include "implicit_octree_nns/point_traits.hpp"

namespace implicit_octree_nns::detail {

enum class axes { x, y, z };

/**
 * @brief An axis-aligned line represented by its fixed dimension, and the offset/position of this
 * fixed dimension
 */
template <typename point_type>
class axis_aligned {
    using coordinate_type = typename point_traits<point_type>::coordinate_type;
    static constexpr auto dimension = point_traits<point_type>::dimension;

   public:
    axis_aligned(axes axis_, coordinate_type position_)
        : axis_aligned(index_of(axis_), position_) {}

    /**
     * @return equation representing the distance function between the input point and the
     * axis-aligned line as a function of the free dimension(s) position
     *
     * The common quadratic term for each free dimension is left out of the equation since we merely
     * care about the relative ordering of these equations
     */
    auto make_distance_equation(const point_type& point) -> equation<coordinate_type, dimension> {
        std::vector<coordinate_type> slope;
        coordinate_type constant{0};
        for (size_t dim = 0; dim < dimension; dim++) {
            auto point_coordinate = point_traits<point_type>::get(point, dim);
            if (dim == fixed_dimension) {
                auto delta = (point_coordinate - position);
                constant += delta * delta;
            } else {
                slope.push_back(-2 * point_coordinate);
                constant += point_coordinate * point_coordinate;
            }
        }
        if constexpr (dimension == 2) {
            return equation{slope[0], constant};
        } else {
            return equation{slope[0], slope[1], constant};
        }
    }

    coordinate_type position;
    int fixed_dimension;

   private:
    axis_aligned(int fixed_dimension_, coordinate_type position_)
        : position{position_}, fixed_dimension{fixed_dimension_} {
        if (fixed_dimension < 0 || fixed_dimension >= dimension) {
            throw std::invalid_argument("Invalid dimension");
        }
    }

    static auto index_of(axes axis) { return static_cast<int>(axis); }
};

}  // namespace implicit_octree_nns::detail

#endif  // IMPLICIT_OCTREE_NNS_AXIS_ALIGNED_HPP
