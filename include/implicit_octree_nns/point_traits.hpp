#ifndef IMPLICIT_OCTREE_NNS_POINT_TRAITS_HPP
#define IMPLICIT_OCTREE_NNS_POINT_TRAITS_HPP

namespace implicit_octree_nns {

/**
 * A collection of parameters and functions that must be specialized for a
 * point before it can be used with this project's data structures
 */
template <typename point_type_>
struct point_traits {
    /**
     * As alias to the type of the point, this provides a uniform syntax for
     * accessing the point's type and its underlying coordinates' type
     */
    using point_type = point_type_;
    /**
     * The type of the point's coordinates, typically a floating-point type
     */
    using coordinate_type = typename point_type::coordinate_type;
    /**
     * The dimension of the point, typically 2 or 3
     */
    static constexpr auto dimension = point_type::dimension;

    /**
     * @pre @f$ 0 \leq index \leq dimension - 1 @f$
     * @returns The index-th coordinate of pt
     */
    static auto get(const point_type& pt, size_t index) { return pt.get(index); }

    /**
     * Sets the index-th coordinate of pt to value
     * @returns The updated value of pt
     */
    static auto set(point_type& pt, size_t index, const coordinate_type& value) {
        pt.set(index, value);
    }

    /**
     * @returns The squared distance between p and q
     */
    static auto distance_squared(const point_type& p, const point_type& q) {
        return p.distance_squared(q);
    }

    /**
     * @returns If p and q can be considered to be the same point
     */
    static auto close(const point_type& p, const point_type& q) {
        return distance_squared(p, q) == 0.0;
    }

    /**
     * @brief A hash functor for (point, depth) pairs
     */
    struct hasher {
        std::size_t operator()(const std::pair<point_type, int>& pt_depth) const {
            return get(pt_depth.first, 0);
        }
    };
};

}  // namespace implicit_octree_nns

#endif  // IMPLICIT_OCTREE_NNS_POINT_TRAITS_HPP
