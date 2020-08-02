#ifndef IMPLICIT_OCTREE_NNS_MODEL_HASH_TABLE_HPP
#define IMPLICIT_OCTREE_NNS_MODEL_HASH_TABLE_HPP

#include <optional>
#include <unordered_map>

#include "implicit_octree_nns/hash_table_traits.hpp"
#include "implicit_octree_nns/model_point.hpp"

namespace implicit_octree_nns::model {

template <typename point_type>
struct hash_table {
    using key_type = std::pair<point_type, int>;
    using value_type = int;
    using hasher = typename point_traits<point_type>::hasher;
    std::unordered_map<key_type, value_type, hasher> table;
};
}  // namespace implicit_octree_nns::model

/**
 * The specialization of the model hash table on the required traits
 */
//! [Specializing the model hash table]
template <typename point_type>
struct implicit_octree_nns::hash_table_traits<implicit_octree_nns::model::hash_table<point_type>> {
    using hash_table_type = model::hash_table<point_type>;
    using key_type = typename hash_table_type::key_type;
    using value_type = typename hash_table_type::value_type;
    using hasher = typename hash_table_type::hasher;

    static auto insert(hash_table_type& hash_table, const key_type& key, const value_type& value) {
        hash_table.table.insert({key, value});
    }

    static auto at(const hash_table_type& hash_table, const key_type& key)
        -> std::optional<value_type> {
        if (contains(hash_table, key)) {
            return {hash_table.table.at(key)};
        } else {
            return {};
        }
    }

   private:
    static auto contains(const hash_table_type& hash_table, const key_type& key) {
        return hash_table.table.count(key) > 0;
    }
};
//! [Specializing the model hash table]

#endif  // IMPLICIT_OCTREE_NNS_MODEL_HASH_TABLE_HPP
