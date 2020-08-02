#include "implicit_octree_nns/detail/experiment_setup.hpp"
using namespace implicit_octree_nns::detail;

template <typename experiment_type>
void run_experiment(experiment_type experiment) {
    for (int max_points : std::vector{20, 50, 200}) {
        int lo = 10000;
        int hi = 500000;
        int step = 10000;
        for (int num_points = lo; num_points <= hi; num_points += step) {
            experiment(num_points, max_points);
        }
    }
}

int main() {
    run_experiment(experiment_generator(get_normal_distribution(10000.)));
    run_experiment(
        experiment_generator(get_poisson_distribution<experiment_coord_type>(100000000)));
    run_experiment(experiment_generator(get_uniform_distribution(34641.)));
}