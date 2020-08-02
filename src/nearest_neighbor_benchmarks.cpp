#include "implicit_octree_nns/detail/experiment_setup.hpp"

using namespace implicit_octree_nns;
using namespace implicit_octree_nns::detail;

template <typename experiment_type>
void run_experiment(experiment_type experiment) {
    constexpr auto num_queries = 10000000;
    for (int max_points : std::vector{20, 50, 200}) {
        int lo = 10000;
        int hi = 500000;
        int step = 10000;
        for (int num_points = lo; num_points <= hi; num_points += step) {
            // num_queries doesn't matter as long as sufficiently large
            experiment(num_points, num_queries, max_points);
        }
    }
}

template <typename experiment_type>
void run_baseline(experiment_type experiment) {
    constexpr auto num_queries = 10000000;
    int lo = 10000;
    int hi = 500000;
    int step = 10000;
    for (int num_points = lo; num_points <= hi; num_points += step) {
        experiment(num_points, num_queries);
    }
}

int main() {
    run_baseline(baseline_generator(get_normal_distribution(10000.)));
    run_baseline(baseline_generator(get_poisson_distribution<experiment_coord_type>(100000000)));
    run_baseline(baseline_generator(get_uniform_distribution(34641.)));
    run_experiment(benchmark_generator(get_normal_distribution(10000.)));
    run_experiment(benchmark_generator(get_poisson_distribution<experiment_coord_type>(100000000)));
    run_experiment(benchmark_generator(get_uniform_distribution(34641.)));
}