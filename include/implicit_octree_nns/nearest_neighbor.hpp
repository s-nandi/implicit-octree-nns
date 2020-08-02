#ifndef IMPLICIT_OCTREE_NNS_NEAREST_NEIGHBOR_HPP
#define IMPLICIT_OCTREE_NNS_NEAREST_NEIGHBOR_HPP

#include <iterator>
#include <unordered_set>
#include <vector>

#include "implicit_octree_nns/detail/octree_cell.hpp"
#include "implicit_octree_nns/hash_table_traits.hpp"
#include "implicit_octree_nns/model_hash_table.hpp"
#include "implicit_octree_nns/point_traits.hpp"
#include "implicit_octree_nns/visualize/drawing.hpp"

namespace implicit_octree_nns {

// Splitting condition
struct splitting_condition {
    long double min_box_length{0};
    int max_depth{200};
    int max_points{20};
};

/**
 * @brief Data structure for efficiently computing nearest neighbor queries
 *
 * After being initialized with a point set, the data structure is able to
 * efficiently answer multiple queries of the form:
 * Which point in the initial point set is closest to a given query point?
 *
 * @pre Duplicate points cannot exist in the initialization point set
 *
 * @tparam point_type_ The underlying point type; must specialize point_traits
 * @tparam hash_table_type_ The underlying hash table type; must specialize hash_table_traits. The
 * default type is std::unordered_type, and a specialization is already provided
 */
template <typename point_type_, typename hash_table_type_ = model::hash_table<point_type_>>
class nearest_neighbor {
   public:
    // Point type traits
    /** Underlying point type */
    using point_type = point_type_;
    /** Coordinate type of underlying point type */
    using coordinate_type = typename point_traits<point_type>::coordinate_type;
    /** Dimension of the point set */
    static constexpr auto dimension = point_traits<point_type>::dimension;

    // Point type requirements
    static_assert(dimension == 2 || dimension == 3,
                  "nearest_neighbor only works for 2D and 3D points");
    static_assert(std::is_floating_point_v<coordinate_type>,
                  "Only floating point coordinates are supported");

    // Hash table type traits
    using hash_table_type = hash_table_type_;
    using hash_table_key_type = typename hash_table_traits<hash_table_type>::key_type;
    using hash_table_value_type = typename hash_table_traits<hash_table_type>::value_type;

    // Constructors

    nearest_neighbor() = default;

    /**
     * Initialize the data structure with a point set
     *
     * @param begin An iterator pointing to the start of a range corresponding to the initial point
     * set
     * @param end An iterator pointing to the end of a range corresponding to the initial point set
     */
    template <typename ForwardIterator>
    nearest_neighbor(ForwardIterator begin, ForwardIterator end, coordinate_type max_coordinate = 0,
                     std::ostream& visualize_ostream = std::cout,
                     splitting_condition condition = splitting_condition{});

    auto should_split(const detail::octree_cell<point_type>& cell,
                      const splitting_condition& condition);

    // Member functions

    /**
     * @returns The closest point to query_point out of the point set the data structure was
     * initialized with
     */
    auto find_nearest_neighbor(const point_type& query_point) const;

    /**
     * Places the initial point set into a square bounding box centered on the origin
     */
    template <typename ForwardIterator>
    auto initialize_bounding_box(ForwardIterator begin, ForwardIterator end,
                                 coordinate_type max_coord);

    /**
     * Splits octree cells that contain too many points into children cells, each child cell
     * contains all points that are the nearest neighbor to some point in the cell, which means that
     * a single point might be part of multiple child cells
     */
    auto split_saturated_cells(int depth);

    /** The number of points used to construct this data structure */
    auto construction_set_size() const;

    /** The number of octree cells in the data structure */
    auto size() const;

    /** The maximum depth for any octree cell in the data structure */
    auto depth() const;

   private:
    // Octree cell typedefs
    using octree_cell_type = detail::octree_cell<point_type>;

    // Data members
    std::vector<octree_cell_type> octree_cells_;
    std::vector<octree_cell_type> octree_leaves_;
    hash_table_type implicit_octree_{};
    octree_cell_type root_cell_;
    int max_depth_used_;
    size_t construction_set_size_;
    visualize::geometry_drawer drawer_;
    /**
     * The variable that determines when an octree cell can/must be split further
     * Based on minimum cell length, max cell depth, and the max number of points in a cell
     */
    splitting_condition condition_{};
};

// Constructor template deduction guides
template <typename ForwardIterator>
nearest_neighbor(ForwardIterator begin, ForwardIterator end)
    -> nearest_neighbor<typename std::iterator_traits<ForwardIterator>::value_type>;

}  // namespace implicit_octree_nns

#include "implicit_octree_nns/detail/nearest_neighbor_impl.hpp"

#endif  // IMPLICIT_OCTREE_NNS_NEAREST_NEIGHBOR_HPP
