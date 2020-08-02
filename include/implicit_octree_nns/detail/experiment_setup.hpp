#ifndef IMPLICIT_OCTREE_NNS_EXPERIMENT_SETUP_HPP
#define IMPLICIT_OCTREE_NNS_EXPERIMENT_SETUP_HPP

#include <chrono>
#include <fstream>
#include <iostream>
#include <random>
#include <string>
#include <tuple>

#include "implicit_octree_nns/detail/kd_tree.hpp"
#include "implicit_octree_nns/detail/utility.hpp"
#include "implicit_octree_nns/nearest_neighbor.hpp"

namespace implicit_octree_nns::detail {
using namespace std::string_literals;

using experiment_coord_type = double;
constexpr static auto experiment_dimension = 2;
using experiment_time_scale = std::chrono::nanoseconds;

std::string makename(const std::string& logname, int dimension, int num_points, int num_queries,
                     experiment_coord_type max_coord, int max_points, unsigned generator_seed) {
    return logname + "_" + std::to_string(dimension) + "_" + std::to_string(num_points) + "_" +
           std::to_string(num_queries) + "_" + std::to_string(static_cast<int>(max_coord)) + "_" +
           std::to_string(max_points) + "_" + std::to_string(generator_seed);
}

std::string makename(const std::string& logname, int dimension, int num_points, int num_queries,
                     experiment_coord_type max_coord, const std::string& label,
                     unsigned generator_seed) {
    return logname + "_" + std::to_string(dimension) + "_" + std::to_string(num_points) + "_" +
           std::to_string(num_queries) + "_" + std::to_string(static_cast<int>(max_coord)) + "_" +
           label + "_" + std::to_string(generator_seed);
}

template <typename T>
auto get_normal_distribution(T stddev = 66.) {
    auto normal_distribution = std::normal_distribution<T>(T{0}, stddev);
    auto normal_distribution_bound = stddev * stddev;
    return std::tuple{normal_distribution, normal_distribution_bound,
                      "normal"s + std::to_string(static_cast<long long>(stddev))};
}

template <typename T>
auto get_uniform_distribution(T max_bound = 5.) {
    auto uniform_distribution = std::uniform_real_distribution<T>(-max_bound, max_bound);
    auto uniform_distribution_bound = max_bound;
    return std::tuple{uniform_distribution, uniform_distribution_bound,
                      "uniform"s + std::to_string(static_cast<long long>(max_bound))};
}

template <typename T>
auto get_poisson_distribution(int mean = 10) {
    auto poisson_distribution = [mean](auto& generator) {
        auto got = std::poisson_distribution<long long>(mean)(generator);
        return static_cast<T>(got - mean);
    };
    auto poisson_distribution_bound = mean * mean;
    return std::tuple{poisson_distribution, poisson_distribution_bound,
                      "poisson"s + std::to_string(mean)};
}

template <typename distribution_type>
void log_size_data(const std::string& logname, const distribution_type& distribution,
                   int num_points, int max_points, unsigned generator_seed) {
    constexpr auto dimension = experiment_dimension;
    using coordinate_type = experiment_coord_type;

    auto generator = std::mt19937{generator_seed};
    auto point_set = generate_random_points<dimension>(num_points, generator, distribution);
    auto max_coord = get_max_magnitude(std::begin(point_set), std::end(point_set));

    std::string name = makename(logname, dimension, num_points, 0, static_cast<int>(max_coord),
                                max_points, generator_seed);
    std::string data_name = name + ".log";
    std::ofstream ofile(data_name);

    using point_type = std::remove_reference_t<decltype(*std::begin(point_set))>;
    auto splitter = splitting_condition{};
    splitter.max_points = max_points;

    auto locator = nearest_neighbor<point_type>{std::begin(point_set), std::end(point_set),
                                                max_coord, std::cout, splitter};
    ofile << "num_cells: " << locator.size() << '\n';
    ofile << "max_depth: " << locator.depth() << '\n';
}

template <typename distribution_type>
void visualization_data(const std::string& logname, distribution_type distribution, int num_points,
                        int num_queries, int max_points, unsigned generator_seed) {
    constexpr auto dimension = experiment_dimension;
    using coordinate_type = experiment_coord_type;

    auto generator = std::mt19937{generator_seed};  // NOLINT
    auto point_set = generate_random_points<dimension>(num_points, generator, distribution);
    auto max_coord = get_max_magnitude(std::begin(point_set), std::end(point_set));

    std::string name = makename(logname, dimension, num_points, num_queries,
                                static_cast<int>(max_coord), max_points, generator_seed);
    std::string vis_name = name + ".octvis";
    auto visualize_output = std::ofstream(vis_name);

    using point_type = std::remove_reference_t<decltype(*std::begin(point_set))>;
    auto splitter = splitting_condition{};
    splitter.max_points = max_points;

    auto locator = nearest_neighbor<point_type>{std::begin(point_set), std::end(point_set),
                                                max_coord, visualize_output, splitter};

    auto queries = generate_random_points<dimension>(
        num_queries, generator,
        std::uniform_real_distribution<coordinate_type>(-max_coord, max_coord));
    for (const auto& query_point : queries) {
        locator.find_nearest_neighbor(query_point);
    }
}

template <typename distribution_type>
void benchmark_data(const std::string& logname, distribution_type distribution, int num_points,
                    int num_queries, int max_points, unsigned generator_seed) {
    constexpr auto dimension = experiment_dimension;
    using coordinate_type = experiment_coord_type;

    auto generator = std::mt19937{generator_seed};  // NOLINT
    auto point_set = generate_random_points<dimension>(num_points, generator, distribution);
    auto max_coord = get_max_magnitude(std::begin(point_set), std::end(point_set));

    using point_type = std::remove_reference_t<decltype(*std::begin(point_set))>;
    auto splitter = splitting_condition{};
    splitter.max_points = max_points;

    auto queries = generate_random_points<dimension>(
        num_queries, generator,
        std::uniform_real_distribution<coordinate_type>(-max_coord, max_coord));

    auto start_time_build = std::chrono::high_resolution_clock::now();
    auto locator = nearest_neighbor<point_type>{std::begin(point_set), std::end(point_set),
                                                max_coord, std::cout, splitter};
    auto end_time_build = std::chrono::high_resolution_clock::now();
    auto time_build_ms =
        std::chrono::duration_cast<experiment_time_scale>((end_time_build - start_time_build))
            .count();
    std::cout << "Got " << locator.size() << " cells" << std::endl;

    auto start_time = std::chrono::high_resolution_clock::now();
    for (const auto& query_point : queries) {
        locator.find_nearest_neighbor(query_point);
    }
    auto end_time = std::chrono::high_resolution_clock::now();
    auto time_ms =
        std::chrono::duration_cast<experiment_time_scale>((end_time - start_time) / num_queries)
            .count();

    const std::string build_prefix{"benchmark.construct."};
    const std::string query_prefix{"benchmark.query."};
    std::string name = makename(logname, dimension, num_points, num_queries,
                                static_cast<int>(max_coord), max_points, generator_seed);
    std::ofstream build_file(build_prefix + name + ".log");
    std::ofstream query_file(query_prefix + name + ".log");
    build_file << time_build_ms << '\n';
    query_file << time_ms << '\n';
}

template <typename distribution_type>
void baseline_data(const std::string& logname, distribution_type distribution, int num_points,
                   int num_queries, unsigned generator_seed) {
    constexpr auto dimension = experiment_dimension;
    using coordinate_type = experiment_coord_type;

    auto generator = std::mt19937{generator_seed};  // NOLINT
    auto point_set = generate_random_points<dimension>(num_points, generator, distribution);
    using point_type = std::remove_reference_t<decltype(*std::begin(point_set))>;
    auto max_coord = get_max_magnitude(std::begin(point_set), std::end(point_set));
    auto queries = generate_random_points<dimension>(
        num_queries, generator,
        std::uniform_real_distribution<coordinate_type>(-max_coord, max_coord));

    auto start_time_build = std::chrono::high_resolution_clock::now();
    auto locator = kd_tree<point_type>{point_set, generator_seed};
    auto end_time_build = std::chrono::high_resolution_clock::now();
    auto time_build_ms =
        std::chrono::duration_cast<experiment_time_scale>((end_time_build - start_time_build))
            .count();
    std::cout << "Got " << locator.size() << " cells" << std::endl;

    auto start_time = std::chrono::high_resolution_clock::now();
    for (const auto& query_point : queries) {
        locator.find_nearest_neighbor(query_point);
    }
    auto end_time = std::chrono::high_resolution_clock::now();
    auto time_ms =
        std::chrono::duration_cast<experiment_time_scale>((end_time - start_time) / num_queries)
            .count();

    const std::string build_prefix{"benchmark.construct."};
    const std::string query_prefix{"benchmark.query."};
    std::string name = makename(logname, dimension, num_points, num_queries,
                                static_cast<int>(max_coord), "kdtree", generator_seed);
    std::ofstream build_file(build_prefix + name + ".log");
    std::ofstream query_file(query_prefix + name + ".log");
    build_file << time_build_ms << '\n';
    query_file << time_ms << '\n';
}

template <typename distribution_type, typename bound_type>
auto experiment_generator(
    std::tuple<distribution_type, bound_type, std::string> distribution_bounds_name,
    unsigned seed_ = 5) {
    auto [distribution_, bounds_, name_] = distribution_bounds_name;
    auto experiment = [distribution = distribution_, bounds = bounds_, name = name_, seed = seed_](
                          int num_points, int max_points) {
        std::cout << "Running experiment " << name << " with " << num_points << " points "
                  << "and a " << bounds << " bound"
                  << " with M = " << max_points << " and seed " << seed << std::endl;
        log_size_data(name, distribution, num_points, max_points, seed);
    };
    return experiment;
}

template <typename distribution_type, typename bound_type>
auto visualize_generator(
    std::tuple<distribution_type, bound_type, std::string> distribution_bounds_name,
    int num_queries_, unsigned seed_ = 5) {
    const static std::string name_prefix = "visualize_";
    auto [distribution_, bounds_, name_] = distribution_bounds_name;
    auto visualize = [distribution = distribution_, name = name_prefix + name_,
                      num_queries = num_queries_, seed = seed_](int num_points, int max_points) {
        std::cout << "Running visualization " << name << " with " << num_points << " points"
                  << " with M = " << max_points << " and seed " << seed << std::endl;
        visualization_data(name, distribution, num_points, num_queries, max_points, seed);
    };
    return visualize;
}

template <typename distribution_type, typename bound_type>
auto benchmark_generator(
    std::tuple<distribution_type, bound_type, std::string> distribution_bounds_name,
    unsigned seed_ = 5) {
    auto [distribution_, bounds_, name_] = distribution_bounds_name;
    auto benchmark = [distribution = distribution_, name = name_, seed = seed_](
                         int num_points, int num_queries, int max_points) {
        std::cout << "Running benchmark " << name << " with " << num_points << " points "
                  << num_queries << " queries"
                  << " with M = " << max_points << " and seed " << seed << std::endl;
        benchmark_data(name, distribution, num_points, num_queries, max_points, seed);
    };
    return benchmark;
}

template <typename distribution_type, typename bound_type>
auto baseline_generator(
    std::tuple<distribution_type, bound_type, std::string> distribution_bounds_name,
    unsigned seed_ = 5) {
    auto [distribution_, bounds_, name_] = distribution_bounds_name;
    auto baseline = [distribution = distribution_, name = name_, seed = seed_](int num_points,
                                                                               int num_queries) {
        std::cout << "Running baseline " << name << " with " << num_points << " points "
                  << num_queries << " queries"
                  << " and seed " << seed << std::endl;
        baseline_data(name, distribution, num_points, num_queries, seed);
    };
    return baseline;
}
}  // namespace implicit_octree_nns::detail

#endif  // IMPLICIT_OCTREE_NNS_EXPERIMENT_SETUP_HPP
