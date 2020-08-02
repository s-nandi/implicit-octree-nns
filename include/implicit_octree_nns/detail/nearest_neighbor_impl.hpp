#include <algorithm>
#include <cassert>
#include <exception>
#include <iterator>
#include <utility>

#include "implicit_octree_nns/detail/cgal_voronoi.hpp"
#include "implicit_octree_nns/detail/utility.hpp"
#include "implicit_octree_nns/visualize/drawing.hpp"

namespace implicit_octree_nns {

template <typename point_type, typename hash_type>
template <typename ForwardIterator>
nearest_neighbor<point_type, hash_type>::nearest_neighbor(ForwardIterator begin,
                                                          ForwardIterator end,
                                                          coordinate_type max_coord,
                                                          std::ostream& visualize_ostream,
                                                          splitting_condition condition)
    : max_depth_used_{0}, drawer_{visualize_ostream, dimension}, condition_{condition} {
    construction_set_size_ = std::distance(begin, end);
    initialize_bounding_box(begin, end, max_coord);
    for (int depth = 0; !octree_cells_.empty(); depth++) {
        if constexpr (do_visualize) {
            drawer_.draw_voronoi_edges(
                detail::voronoi_segments<coordinate_type>::generate(begin, end, max_coord));
        }
        split_saturated_cells(depth);
        max_depth_used_ = depth;
    }
}

template <typename point_type, typename hash_type>
auto nearest_neighbor<point_type, hash_type>::construction_set_size() const {
    return construction_set_size_;
}

template <typename point_type, typename hash_type>
auto nearest_neighbor<point_type, hash_type>::size() const {
    return std::size(octree_leaves_);
}

template <typename point_type, typename hash_type>
auto nearest_neighbor<point_type, hash_type>::depth() const {
    return max_depth_used_ + 1;
}

template <typename point_type, typename hash_type>
auto nearest_neighbor<point_type, hash_type>::find_nearest_neighbor(
    const point_type& query_point) const {
    using detail::bsearch_result;

    if (!root_cell_.box.contains(query_point)) {
        throw std::invalid_argument("Query point outside bounding box of construction point set");
    }
    int lo_depth = 0;
    int hi_depth = max_depth_used_ + 1;
    int leaf = -1;
    while (lo_depth <= hi_depth) {
        auto depth = (lo_depth + hi_depth) / 2;
        auto corner = root_cell_.box.floored_corner(query_point, depth);
        auto corner_depth = std::pair{corner, depth};
        auto hash_entry = hash_table_traits<hash_type>::at(implicit_octree_, corner_depth);
        bsearch_result result;
        if (hash_entry == std::nullopt) {
            result = bsearch_result::too_deep;
        } else if (int leaf_index = *hash_entry; leaf_index == -1) {
            result = bsearch_result::too_shallow;
        } else {
            leaf = leaf_index;
            result = bsearch_result::just_right;
        }
        switch (result) {
            case bsearch_result::too_deep:
                hi_depth = depth - 1;
                break;
            case bsearch_result::too_shallow:
                lo_depth = depth + 1;
                break;
            case bsearch_result::just_right:
                lo_depth = 1;
                hi_depth = -1;
                break;
            default:
                assert(false);
        }
        if constexpr (do_visualize) {
            auto query_box = root_cell_.box.floored_box(query_point, depth);
            drawer_.draw_query(query_point, depth, result, query_box);
            drawer_.write_end_query();
        }
    }
    assert(leaf != -1);
    const auto& cell = octree_leaves_[leaf];
    assert(cell.box.contains(query_point));
    return cell.closest_point(query_point);
}

template <typename point_type, typename hash_type>
template <typename ForwardIterator>
auto nearest_neighbor<point_type, hash_type>::initialize_bounding_box(ForwardIterator begin,
                                                                      ForwardIterator end,
                                                                      coordinate_type max_coord) {
    root_cell_ = octree_cells_.emplace_back(begin, end, max_coord);
}

template <typename point_type, typename hash_type>
auto nearest_neighbor<point_type, hash_type>::should_split(
    const detail::octree_cell<point_type>& cell, const splitting_condition& condition) {
    // Make sure that the cell isn't too deep or too small to split further
    if (cell.depth() >= condition.max_depth) return false;
    for (int dim = 0; dim < dimension; dim++) {
        if (cell.length(dim) <= condition.min_box_length) {
            return false;
        }
    }
    // If the cell is shallow/big enough, check if there are enough points to warrant splitting
    return cell.size() > condition.max_points;
}

template <typename point_type, typename hash_type>
auto nearest_neighbor<point_type, hash_type>::split_saturated_cells(int depth) {
    std::vector<octree_cell_type> split_cells;

    if constexpr (do_visualize) {
        for (const auto& cell : octree_cells_) {
            drawer_.draw_cell(cell, true);
        }
        for (const auto& cell : octree_leaves_) {
            drawer_.draw_cell(cell, true);
        }
        drawer_.write_end_frame();
    }
    for (const auto& cell : octree_cells_) {
        auto bottom_left = cell.box.smallest_corner();
        auto corner_depth = std::pair{root_cell_.box.floored_corner(bottom_left, depth), depth};
        bool is_leaf = !should_split(cell, condition_);
        int leaf_index = is_leaf ? static_cast<int>(std::size(octree_leaves_)) : -1;
        hash_table_traits<hash_type>::insert(implicit_octree_, corner_depth, leaf_index);

        // breakpoint
        if (!is_leaf) {
            auto child_cells = cell.split_into_children();
            std::move(std::begin(child_cells), std::end(child_cells),
                      std::back_inserter(split_cells));
        } else {
            octree_leaves_.emplace_back(std::move(cell));
        }
    }
    std::swap(split_cells, octree_cells_);
}

}  // namespace implicit_octree_nns
