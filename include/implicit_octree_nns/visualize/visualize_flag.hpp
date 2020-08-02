#ifndef IMPLICIT_OCTREE_NNS_VISUALIZE_FLAG_HPP
#define IMPLICIT_OCTREE_NNS_VISUALIZE_FLAG_HPP

#ifdef VISUALIZE_ENABLED
constexpr auto do_visualize = true;
#else
constexpr auto do_visualize = false;
#endif

#endif  // IMPLICIT_OCTREE_NNS_VISUALIZE_FLAG_HPP
