#include <cassert>

#include "implicit_octree_nns/detail/experiment_setup.hpp"

using namespace implicit_octree_nns::detail;

template <typename visualize_type>
void run_visualize(visualize_type visualize) {
    std::vector num_points_list{50, 1000};
    std::vector max_points_list{5, 50};
    assert(std::size(max_points_list) == std::size(num_points_list));
    for (size_t i = 0; i < std::size(max_points_list); i++) {
        visualize(num_points_list[i], max_points_list[i]);
    }
}

int main() {
    constexpr auto num_queries = 5;
    run_visualize(visualize_generator(get_normal_distribution(10.), num_queries));
    run_visualize(
        visualize_generator(get_poisson_distribution<experiment_coord_type>(100), num_queries));
    run_visualize(visualize_generator(get_uniform_distribution(17.), num_queries));
}