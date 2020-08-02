#ifndef IMPLICIT_OCTREE_NNS_NAIVE_NEAREST_NEIGHBOR_HPP
#define IMPLICIT_OCTREE_NNS_NAIVE_NEAREST_NEIGHBOR_HPP

#include "implicit_octree_nns/point_traits.hpp"

namespace implicit_octree_nns::detail {
template <typename point_type_>
class naive_nearest_neighbor {
   public:
    using point_type = point_type_;

    naive_nearest_neighbor() = default;
    naive_nearest_neighbor(const std::vector<point_type>& point_set) : point_set_{point_set} {}

    auto find_nearest_neighbor(const point_type& query_point) {
        return *min_element(std::begin(point_set_), std::end(point_set_),
                            [&query_point](const point_type& p, const point_type& q) {
                                auto distance_of_p_to_query =
                                    point_traits<point_type>::distance_squared(p, query_point);
                                auto distance_of_q_to_query =
                                    point_traits<point_type>::distance_squared(q, query_point);
                                return distance_of_p_to_query < distance_of_q_to_query;
                            });
    }

   private:
    const std::vector<point_type> point_set_;
};
}  // namespace implicit_octree_nns::detail

#endif  // IMPLICIT_OCTREE_NNS_NAIVE_NEAREST_NEIGHBOR_HPP
