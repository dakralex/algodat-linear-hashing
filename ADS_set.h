#ifndef ADS_SET_H
#define ADS_SET_H

#include <functional>
#include <algorithm>
#include <utility>
#include <iostream>
#include <iomanip>
#include <stdexcept>

/**
 * Hash table set implemented with Linear Hashing.
 *
 * Key is the type of the elements.
 * N is the size of each bucket in the hash table. (b in lectures)
 */
template<typename Key, size_t N = 5>
class ADS_set {
public:
    // class Iterator;
    class Bucket;

    using value_type = Key;
    using key_type = Key;
    using reference = value_type&;
    using const_reference = const value_type&;
    using size_type = size_t;
    using difference_type = std::ptrdiff_t;
    // using const_iterator = Iterator;
    // using iterator = const_iterator;
    using key_equal = std::equal_to<key_type>;
    using hasher = std::hash<key_type>;
private:
    /* Current round of splitting (d in lectures) */
    size_type split_round {0};

    /* Index for next bucket to split from hash table (nextToSplit in lectures) */
    size_type table_split_index {0};

    /* Number of buckets in hash table */
    size_type table_size {0};

    /* Number of items in hash table */
    size_type table_items_size {0};

    /* Hash table with bucket lists */
    Bucket* table {nullptr};

    /* Hash instance for hash table */
    const hasher hash {};

    /* Hash function for current split round */
    size_type h(const key_type& key) const {
        return hash(key) % (1 << split_round);
    }

    /* Hash function for next split round */
    size_type g(const key_type& key) const {
        return hash(key) % (1 << (split_round + 1));
    }

    /* Get the bucket index for a given key in the hash table */
    size_type bucket_at(const key_type& key) const;

    /* Locate a stored key in the hash table */
    key_type* locate(const key_type& key) const;

    /* Allocate buckets for the current splitting round */
    void reserve(size_type new_table_size);

    /* Split the next to be split bucket in the hash table */
    void split();

    /* Add a key to the hash table */
    void add(const key_type& key);

    void add(key_type&& key);

public:
    /* Create an empty ADS_set with an initial split_round  */
    ADS_set() : split_round {1}, table_size {1u << split_round}, table {new Bucket[table_size]} {}

    /* Create an ADS_set with a given list of items */
    ADS_set(std::initializer_list<key_type> ilist) : ADS_set {} { insert(ilist); }

    /* Create an ADS_set with a given range of items */
    template<typename InputIt>
    ADS_set(InputIt first, InputIt last): ADS_set {} { insert(first, last); }

    ADS_set(const ADS_set& other) { swap(other); }

    ~ADS_set() { delete[] table; }

    // ADS_set &operator=(const ADS_set& other);
    // ADS_set &operator=(std::initializer_list<key_type> ilist);

    /* Return the count of the stored items in the hash table */
    [[nodiscard]] size_type size() const { return table_items_size; };

    /* Return whether the hash table is empty */
    [[nodiscard]] bool empty() const { return table_items_size == 0; };

    /* Insert the given list of items in the hash table */
    void insert(std::initializer_list<key_type> ilist) { insert(ilist.begin(), ilist.end()); }

    // std::pair<iterator,bool> insert(const key_type& key);

    /* Insert the given range of items in the hash table */
    template<typename InputIt>
    void insert(InputIt first, InputIt last) { for (auto it {first}; it != last; ++it) add(*it); }

    void clear();

    // size_type erase(const key_type& key);

    /* Count the items in the hash table with key */
    size_type count(const key_type& key) const { return locate(key) != nullptr; }

    // iterator find(const key_type& key) const;

    void swap(ADS_set& other);

    // const_iterator begin() const;
    // const_iterator end() const;

    void dump(std::ostream& o = std::cerr) const;

    // friend bool operator==(const ADS_set& lhs, const ADS_set& rhs);
    // friend bool operator!=(const ADS_set& lhs, const ADS_set& rhs);
};

template<typename Key, size_t N>
class ADS_set<Key, N>::Bucket {
private:
    size_type values_size {0};
    size_type values_capacity {0};
    key_type* values {nullptr};
private:
    /* Expand values capacity by N */
    void expand();

public:
    Bucket() : values_capacity {N}, values {new key_type[values_capacity]} {}

    ~Bucket() { delete[] values; }

    Bucket(const Bucket& other) {
        values_size = other.values_size;
        values_capacity = other.values_capacity;
        values = new key_type[values_capacity];
        std::copy(other.values, other.values + values_size, values);
    }

    Bucket(Bucket&& other) noexcept {
        values_size = std::exchange(other.values_size, 0);
        values_capacity = std::exchange(other.values_capacity, N);
        values = std::exchange(other.values, new key_type[values_capacity]);
    }

    Bucket& operator=(const Bucket& other);

    Bucket& operator=(Bucket&& other) noexcept;

    const key_type& operator[](size_type index) const { return values[index]; }

    key_type& operator[](size_type index) { return values[index]; }

    /* Locate a stored key in the bucket */
    key_type* locate(const key_type& key) const;

    /* Push a key to the bucket */
    void add(const key_type& key);

    void add(key_type&& key);

    size_type count(const key_type& key) const { return locate(key) != nullptr; }

    [[nodiscard]] size_type size() const { return values_size; }

    [[nodiscard]] size_type full() const { return values_size == values_capacity; }

    void dump(std::ostream& o = std::cerr) const;
};

// template <typename Key, size_t N>
// class ADS_set<Key, N>::Iterator {
//   value_type* value;
// public:
//   using value_type = Key;
//   using difference_type = std::ptrdiff_t;
//   using reference = const value_type &;
//   using pointer = const value_type *;
//   using iterator_category = std::forward_iterator_tag;
//
//   explicit Iterator(/* implementation-dependent */);
//   reference operator*() const;
//   pointer operator->() const;
//   Iterator &operator++();
//   Iterator operator++(int);
//   friend bool operator==(const Iterator &lhs, const Iterator &rhs);
//   friend bool operator!=(const Iterator &lhs, const Iterator &rhs);
// };
//
// template <typename Key, size_t N>
// void swap(ADS_set<Key,N> &lhs, ADS_set<Key,N> &rhs) { lhs.swap(rhs); }

template<typename Key, size_t N>
typename ADS_set<Key, N>::Bucket& ADS_set<Key, N>::Bucket::operator=(const Bucket& other) {
    if (this != &other) {
        values_size = other.values_size;
        values_capacity = other.values_capacity;
        std::copy(other.values, other.values + other.values_size, values);
    }

    return *this;
}

template<typename Key, size_t N>
typename ADS_set<Key, N>::Bucket& ADS_set<Key, N>::Bucket::operator=(Bucket&& other) noexcept {
    if (this != &other) {
        values_size = std::exchange(other.values_size, 0);
        values_capacity = std::exchange(other.values_capacity, N);
        values = std::exchange(other.values, new key_type[values_capacity]);
    }

    return *this;
}


template<typename Key, size_t N>
void ADS_set<Key, N>::Bucket::expand() {
    size_type new_values_capacity {values_size + N};
    key_type* new_values {new key_type[new_values_capacity]};

    // Copy values to new_values
    for (size_type i {0}; i < values_size; ++i) {
        new_values[i] = std::move(values[i]);
    }

    // Free memory
    delete[] values;

    // Update values and capacity
    values = new_values;
    values_capacity = new_values_capacity;
}

template<typename Key, size_t N>
typename ADS_set<Key, N>::key_type* ADS_set<Key, N>::Bucket::locate(const key_type& key) const {
    for (size_type i {0}; i < values_size; ++i) {
        if (key_equal {}(values[i], key)) {
            return &values[i];
        }
    }

    return nullptr;
}

template<typename Key, size_t N>
void ADS_set<Key, N>::Bucket::add(const key_type& key) {
    // Expand bucket if it exceeds capacity
    if (values_size >= values_capacity) expand();

    // Store key in slice and increment its size
    values[values_size++] = key;
}

template<typename Key, size_t N>
void ADS_set<Key, N>::Bucket::add(key_type&& key) {
    if (values_size >= values_capacity) expand();

    values[values_size++] = std::move(key);
}

template<typename Key, size_t N>
void ADS_set<Key, N>::Bucket::dump(std::ostream& o) const {
    o << "(size: " << std::setfill(' ') << std::setw(2) << values_size << ", ";
    o << "capacity: " << std::setfill(' ') << std::setw(2) << values_capacity << ") | ";

    for (size_type i {0}; i < values_size; ++i) {
        if (i > 0 && i % N == 0) o << " -> | ";
        o << values[i] << " ";
    }
}

template<typename Key, size_t N>
typename ADS_set<Key, N>::size_type ADS_set<Key, N>::bucket_at(const key_type& key) const {
    size_type index {h(key)};

    // Use next split round's hash function for already split buckets
    if (index < table_split_index) {
        index = g(key);
    }

    return index;
}

template<typename Key, size_t N>
typename ADS_set<Key, N>::key_type* ADS_set<Key, N>::locate(const key_type& key) const {
    return table[bucket_at(key)].locate(key);
}

template<typename Key, size_t N>
void ADS_set<Key, N>::reserve(size_type new_table_size) {
    Bucket* new_table {new Bucket[new_table_size]};

    // Copy current table content to new_table
    for (size_type i {0}; i < table_size; ++i) {
        new_table[i] = std::move(table[i]);
    }

    delete[] table;

    // Update table to new_table
    table = new_table;
    table_size = new_table_size;
}

template<typename Key, size_t N>
void ADS_set<Key, N>::split() {
    // Calculate maximum table_size for this split round
    const size_type max_table_size {1u << split_round};

    // Double the table size
    if (table_size == max_table_size) {
        reserve(table_size << 1);
    }

    Bucket bucket {std::move(table[table_split_index])};

    table_items_size -= bucket.size();

    if (table_split_index >= max_table_size) {
        // Advance split round if all buckets have been split
        table_split_index = 0;
        ++split_round;
    } else {
        // Else just advance split index
        ++table_split_index;
    }

    for (size_type i {0}; i < bucket.size(); ++i) {
        add(bucket[i]);
    }
}

template<typename Key, size_t N>
void ADS_set<Key, N>::add(const key_type& key) {
    Bucket& bucket {table[bucket_at(key)]};

    // Ignore already existing keys
    if (bucket.count(key)) return;

    bool should_split = bucket.full();
    bucket.add(key);

    if (should_split) split();

    ++table_items_size;
}

template<typename Key, size_t N>
void ADS_set<Key, N>::add(key_type&& key) {
    Bucket& bucket {table[bucket_at(key)]};

    if (bucket.count(key)) return;

    bool should_split = bucket.full();
    bucket.add(std::move(key));

    if (should_split) split();

    ++table_items_size;
}

template<typename Key, size_t N>
void ADS_set<Key, N>::clear() {
    ADS_set tmp;
    swap(tmp);
}

template<typename Key, size_t N>
void ADS_set<Key, N>::swap(ADS_set& other) {
    std::swap(table, other.table);
    std::swap(split_round, other.split_round);
    std::swap(table_split_index, other.table_split_index);
    std::swap(table_size, other.table_size);
    std::swap(table_items_size, other.table_items_size);
    std::swap(hash, other.hash);
}

template<typename Key, size_t N>
void ADS_set<Key, N>::dump(std::ostream& o) const {
    o << "table_size = " << table_size << ", table_items_size = " << table_items_size << ", table_split_index = "
      << table_split_index << "\n\n";
    o << "=== HASH TABLE ===\n\n";

    for (size_type i {0}; i < table_size; ++i) {
        o << (table_split_index == i ? "-> " : "   ");
        o << std::setfill(' ') << std::setw(4) << i << " | ";
        table[i].dump(o);
        o << "\n";
    }

    o << "\n";
}

#endif // ADS_SET_H
