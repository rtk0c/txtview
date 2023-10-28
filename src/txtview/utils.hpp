#pragma once

// https://stackoverflow.com/a/2595226
inline void HashCombine(size_t& seed, size_t hash) {
    seed ^= hash + 0x9e3779b9 + (seed<<6) + (seed>>2);
}

template <typename T>
inline void HashCombineObj(size_t& seed, const T& v) {
    HashCombine(seed, std::hash<T>{}(v));
}

template <typename T>
struct MemberHash {
    size_t operator()(const T& t) { return t.HashCode(); }
};

// TODO consider switching to boost::unordered_flat_map? or ankerl::unordered_dense::?
template <typename Key, typename Value>
using HashMap = std::unordered_map<Key, Value, MemberHash<Key>>;
