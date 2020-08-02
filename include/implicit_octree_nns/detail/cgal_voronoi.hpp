#ifndef IMPLICIT_OCTREE_NNS_CGAL_VORONOI_HPP
#define IMPLICIT_OCTREE_NNS_CGAL_VORONOI_HPP

#include <CGAL/Delaunay_triangulation_2.h>
#include <CGAL/Simple_cartesian.h>

#include "implicit_octree_nns/model_point.hpp"

namespace implicit_octree_nns::detail {
template <typename coordinate_type>
struct voronoi_segments {
    using K = CGAL::Simple_cartesian<coordinate_type>;
    using Point_2 = typename K::Point_2;
    using Iso_rectangle_2 = typename K::Iso_rectangle_2;
    using Segment_2 = typename K::Segment_2;
    using Ray_2 = typename K::Ray_2;
    using Line_2 = typename K::Line_2;
    using Delaunay_triangulation_2 = CGAL::Delaunay_triangulation_2<K>;

    struct Cropped_voronoi_from_delaunay {
        std::vector<Segment_2>
            m_cropped_vd;  // TODO: Replace with vector if it maintains correctness
        Iso_rectangle_2 m_bbox;

        explicit Cropped_voronoi_from_delaunay(const Iso_rectangle_2& bbox) : m_bbox(bbox) {}

        template <class RSL>
        void crop_and_extract_segment(const RSL& rsl) {
            CGAL::Object obj = CGAL::intersection(rsl, m_bbox);
            const Segment_2* s = CGAL::object_cast<Segment_2>(&obj);
            if (s) {
                m_cropped_vd.push_back(*s);
                if (isnan(m_cropped_vd.back().source().x()) ||
                    isnan(m_cropped_vd.back().source().y()) ||
                    isnan(m_cropped_vd.back().target().x()) ||
                    isnan(m_cropped_vd.back().target().y())) {
                    m_cropped_vd.pop_back();
                }
            }
        }

        void operator<<(const Ray_2& ray) { crop_and_extract_segment(ray); }
        void operator<<(const Line_2& line) { crop_and_extract_segment(line); }
        void operator<<(const Segment_2& seg) { crop_and_extract_segment(seg); }
    };

    template <typename ForwardIterator>
    static std::vector<Segment_2> generate(ForwardIterator begin, ForwardIterator end,
                                           coordinate_type max_coordinate) {
        std::vector<Point_2> points(std::distance(begin, end));
        std::transform(begin, end, std::begin(points), [](auto pt) {
            using point_type = decltype(pt);
            const auto& x = point_traits<point_type>::get(pt, 0);
            const auto& y = point_traits<point_type>::get(pt, 1);
            return Point_2(x, y);
        });
        Delaunay_triangulation_2 dt2;
        dt2.insert(std::begin(points), std::end(points));
        constexpr static auto scale = 1;
        Iso_rectangle_2 bbox(-scale * max_coordinate, -scale * max_coordinate,
                             scale * max_coordinate, scale * max_coordinate);
        Cropped_voronoi_from_delaunay vor(bbox);
        dt2.draw_dual(vor);
        return std::vector(vor.m_cropped_vd.begin(), vor.m_cropped_vd.end());
    }
};
}  // namespace implicit_octree_nns::detail

#endif  // IMPLICIT_OCTREE_NNS_CGAL_VORONOI_HPP
