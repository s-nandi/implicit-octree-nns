#ifndef IMPLICIT_OCTREE_NNS_DRAWING_HPP
#define IMPLICIT_OCTREE_NNS_DRAWING_HPP

#include <cassert>
#include <iostream>
#include <ostream>

#include "implicit_octree_nns/detail/octree_cell.hpp"
#include "implicit_octree_nns/detail/utility.hpp"
#include "implicit_octree_nns/point_traits.hpp"
#include "implicit_octree_nns/visualize/visualize_flag.hpp"

namespace implicit_octree_nns::visualize {

struct geometry_drawer {
    mutable std::ostream* os_;
    int dimension_;

    template <typename point_type>
    void draw_point(const point_type& point) const {
        if constexpr (do_visualize) {
            assert(dimension_ == point_traits<point_type>::dimension);
            for (size_t dim = 0; dim < point_traits<point_type>::dimension; dim++) {
                *os_ << point_traits<point_type>::get(point, dim);
                if (dim + 1 < point_traits<point_type>::dimension) {
                    *os_ << " ";
                }
            }
        }
    }

    /**
     * Output Format: #dimensions
     */
    void write_header() const {
        if constexpr (do_visualize) {
            *os_ << dimension_ << '\n';
        }
    }

    template <typename point_type>
    void draw_box(const detail::bounding_box<point_type>& box) const {
        for (size_t dim = 0; dim < point_traits<point_type>::dimension; dim++) {
            *os_ << box.min(dim) << " " << box.max(dim) << " ";
        }
    }

    /**
     * Output Format: octree_cell is_leaf bounds_1 ... bounds_2^dim #points pt_1 ... pt_k
     */
    template <typename point_type>
    void draw_cell(const detail::octree_cell<point_type>& cell, bool is_leaf) const {
        if constexpr (do_visualize) {
            assert(dimension_ == point_traits<point_type>::dimension);
            *os_ << "octree_cell ";
            *os_ << is_leaf << " ";
            draw_box(cell.box);
            *os_ << std::size(cell.points) << " ";
            for (const auto& point : cell.points) {
                draw_point(point);
                *os_ << " ";
            }
            *os_ << '\n';
        }
    }

    template <typename edge_type>
    void draw_voronoi_edges(const std::vector<edge_type>& edges) const {
        if constexpr (do_visualize) {
            *os_ << "voronoi_edges"
                 << " ";
            *os_ << std::size(edges) << " ";
            for (const auto& e : edges) {
                *os_ << e.source() << " " << e.target() << " ";
            }
            *os_ << '\n';
        }
    }

    auto status_to_string(detail::bsearch_result status) const -> std::string {
        if constexpr (do_visualize) {
            if (status == detail::bsearch_result::too_shallow) {
                return "too_shallow";
            } else if (status == detail::bsearch_result::too_deep) {
                return "too_deep";
            } else if (status == detail::bsearch_result::just_right) {
                return "just_right";
            } else {
                assert(false);
                return "";
            }
        }
    }

    template <typename point_type>
    void draw_query(const point_type& pt, int depth, detail::bsearch_result status,
                    const detail::bounding_box<point_type>& box) const {
        if constexpr (do_visualize) {
            *os_ << "query_point"
                 << " ";
            draw_point(pt);
            *os_ << " ";
            *os_ << depth << " ";
            *os_ << status_to_string(status) << " ";
            draw_box(box);
            *os_ << '\n';
            std::flush(*os_);
        }
    }

    void write_end_frame() const {
        if constexpr (do_visualize) {
            *os_ << "end_frame" << '\n';
        }
    }

    void write_end_query() const {
        if constexpr (do_visualize) {
            *os_ << "end_query" << '\n';
        }
    }

    geometry_drawer() = default;
    geometry_drawer(std::ostream& os, int dimension) {
        if constexpr (do_visualize) {
            os_ = &os;
            dimension_ = dimension;
            write_header();
        }
    }
};
}  // namespace implicit_octree_nns::visualize

#endif  // IMPLICIT_OCTREE_NNS_DRAWING_HPP
