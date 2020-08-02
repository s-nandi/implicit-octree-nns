#ifndef IMPLICIT_OCTREE_NNS_HASH_TABLE_TRAITS_HPP
#define IMPLICIT_OCTREE_NNS_HASH_TABLE_TRAITS_HPP

namespace implicit_octree_nns {
/**
 * A collection of parameters and functions that must be specialized for a
 * hash table before it can be used with this project's data structures
 */
template <typename hash_table_type_>
struct hash_table_traits {
    using hash_table_type = hash_table_type_;
    using key_type = typename hash_table_type::key_type;
    using value_type = typename hash_table_type::value_type;

    /**
     * Inserts a key-value pair of {key, value} into the hash_table without returning anything
     */
    static auto insert(hash_table_type& hash_table, const key_type& key, const value_type& value);

    /**
     * @return An optional containing the value corresponding to the given key in hash_table if the
     * key exists, otherwise returns nullopt
     */
    static auto at(const hash_table_type& hash_table, const key_type& key);
};
}  // namespace implicit_octree_nns

#endif  // IMPLICIT_OCTREE_NNS_HASH_TABLE_TRAITS_HPP
