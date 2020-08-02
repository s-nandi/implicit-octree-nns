#ifndef IMPLICIT_OCTREE_NNS_MODEL_POINT_HPP
#define IMPLICIT_OCTREE_NNS_MODEL_POINT_HPP

#include <array>

#include "implicit_octree_nns/point_traits.hpp"

namespace implicit_octree_nns::model {

/**
 * A model point whose underlying storage is a std::array
 *
 * The point is specialized for the required point traits as follows:
 * @snippet model_point.hpp Specializing the model point
 */
template <typename coordinate_type, size_t dimension>
struct point {
    std::array<coordinate_type, dimension> coordinates_;
};

/** Convenience function for creating a 2D model point */
template <typename coordinate_type>
auto make_point(coordinate_type x, coordinate_type y) {
    return point<coordinate_type, 2>{{x, y}};
}

/** Convenience function for creating a 3D model point */
template <typename coordinate_type>
auto make_point(coordinate_type x, coordinate_type y, coordinate_type z) {
    return point<coordinate_type, 3>{{x, y, z}};
}

/** Uses point_traits::close to provide convenient operator overload */
template <typename coordinate_type, size_t dimension>
auto operator==(point<coordinate_type, dimension> p, point<coordinate_type, dimension> q) {
    return point_traits<point<coordinate_type, dimension>>::close(p, q);
}

using fpoint2 = point<double, 2>;
using fpoint3 = point<double, 3>;

}  // namespace implicit_octree_nns::model

/**
 * The specialization of the model point on the required point traits
 */
//! [Specializing the model point]
template <typename coordinate_type_, size_t dimension_>
struct implicit_octree_nns::point_traits<
    implicit_octree_nns::model::point<coordinate_type_, dimension_>> {
    using point_type = model::point<coordinate_type_, dimension_>;
    using coordinate_type = coordinate_type_;
    static constexpr auto dimension = dimension_;

    static constexpr auto get(const point_type& pt, size_t index) { return pt.coordinates_[index]; }

    static constexpr auto set(point_type& pt, size_t index, const coordinate_type& value) {
        pt.coordinates_[index] = value;
    }

    static constexpr auto distance_squared(const point_type& p, const point_type& q) {
        auto deltas_squared = coordinate_type{0};
        for (int dim = 0; dim < dimension; dim++) {
            auto delta = get(p, dim) - get(q, dim);
            deltas_squared += delta * delta;
        }
        return deltas_squared;
    }

    static auto close(const point_type& p, const point_type& q) {
        constexpr auto epsilon = 1e-9;
        return distance_squared(p, q) < epsilon;
    }

    struct hasher {
        std::size_t operator()(const std::pair<point_type, int>& pt_depth) const {
            static constexpr auto initial_seed = 137;
            const auto& p = pt_depth.first;
            auto hash_value = size_t{initial_seed};
            for (int dim = 0; dim < dimension; dim++) {
                hash_combine(hash_value, get(p, dim));
            }
            hash_combine(hash_value, pt_depth.second);
            return hash_value;
        }
    };

   private:
    // Taken from the boost implementation of the function with the same name
    template <typename T>
    static auto hash_combine(std::size_t& seed, const T& v) {
        std::hash<T> hasher{};
        seed ^= hasher(v) + 0x9e3779b9 + (seed << 6u) + (seed >> 2u);
    }
};

//! [Specializing the model point]

template <typename coordinate_type, size_t dimension>
std::ostream& operator<<(std::ostream& os,
                         const implicit_octree_nns::model::point<coordinate_type, dimension>& pt) {
    os << "(";
    for (size_t dim = 0; dim < dimension; dim++) {
        os << implicit_octree_nns::point_traits<decltype(pt)>::get(dim);
        if (dim + 1 < dimension) {
            os << ", ";
        }
    }
    os << ")";
    return os;
}

#endif  // IMPLICIT_OCTREE_NNS_MODEL_POINT_HPP
