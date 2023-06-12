#ifndef ADS_SET_H
#define ADS_SET_H

#include <functional>
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <stdexcept>

/**
 * Hash table set implemented with Linear Hashing.
 *
 * Key is the type of the elements.
 * N is the size of each bucket in the hash table. (b in lectures)
 */
template<typename Key, size_t N = 10>
class ADS_set {
public:
    class Iterator;

    class Bucket;

    using value_type = Key;
    using key_type = Key;
    using reference = value_type&;
    using const_reference = const value_type&;
    using size_type = size_t;
    using difference_type = std::ptrdiff_t;
    using const_iterator = Iterator;
    using iterator = const_iterator;
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

    /**
     * Add a key to the hash table.
     *
     * @param key the key to add
     * @return pair of a reference to the Bucket and the index inside the Bucket it was added to. If the key already
     *         existed in the hash table, it will return the values for the already existing key.
     */
    std::pair<Bucket&, size_type> add(key_type key);

public:
    /* Create an empty ADS_set with an initial split_round  */
    ADS_set() : split_round {1}, table_size {1u << split_round}, table {new Bucket[table_size]} {}

    /* Create an ADS_set with a given list of items */
    ADS_set(std::initializer_list<key_type> ilist) : ADS_set {} { insert(ilist); }

    /* Create an ADS_set with a given range of items */
    template<typename InputIt>
    ADS_set(InputIt first, InputIt last): ADS_set {} { insert(first, last); }

    ADS_set(const ADS_set& other) : ADS_set {} {
        for (const auto& item : other) {
            (void) add(item);
        }
    }

    ADS_set(ADS_set&& other) noexcept: ADS_set {} { swap(other); }

    ~ADS_set() { delete[] table; }

    ADS_set& operator=(ADS_set other);

    ADS_set& operator=(std::initializer_list<key_type> ilist);

    /* Return the count of the stored items in the hash table */
    [[nodiscard]] size_type size() const { return table_items_size; };

    /* Return whether the hash table is empty */
    [[nodiscard]] bool empty() const { return table_items_size == 0; };

    /* Insert the given list of items in the hash table */
    void insert(std::initializer_list<key_type> ilist) { insert(ilist.begin(), ilist.end()); }

    std::pair<iterator, bool> insert(const key_type& key);

    /* Insert the given range of items in the hash table */
    template<typename InputIt>
    void insert(InputIt first, InputIt last) { for (auto it {first}; it != last; ++it) (void) add(*it); }

    void clear();

    /**
     * Removes the given key from the hash table.
     *
     * @param key the key to remove
     * @return the amount of removed elements
     */
    size_type erase(const key_type& key);

    /* Count the items in the hash table with key */
    size_type count(const key_type& key) const { return locate(key) != nullptr; }

    /**
     * Finds the given key's item in the hash table.
     *
     * @param key the key to find
     * @return Iterator where the key was found. If the key could not be found the end() Iterator.
     */
    iterator find(const key_type& key) const;

    /**
     * Swap this set with the given other set.
     *
     * @param other the set to be swapped with
     */
    void swap(ADS_set& other);

    /**
     * Get the Iterator to the first item in the hash table.
     *
     * @return Iterator to first item
     */
    const_iterator begin() const;

    /**
     * Get the Iterator after the last item in the hash table.
     *
     * @return Iterator after the last item
     */
    const_iterator end() const;

    void dump(std::ostream& o = std::cerr) const;

     friend bool operator==(const ADS_set& lhs, const ADS_set& rhs) {
         if (lhs.split_round != rhs.split_round) return false;
         if (lhs.table_split_index != rhs.table_split_index) return false;
         if (lhs.table_size != rhs.table_size) return false;
         if (lhs.table_items_size != rhs.table_items_size) return false;

         for (const auto& item : lhs) {
             if (!rhs.count(item)) return false;
         }

         return true;
     }

     friend bool operator!=(const ADS_set& lhs, const ADS_set& rhs) { return !(lhs == rhs); }
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

    Bucket(const Bucket& other) : Bucket {} { swap(other); }

    Bucket(Bucket&& other) noexcept: Bucket {} { swap(other); }

    Bucket& operator=(Bucket other);

    const key_type& operator[](size_type index) const { return values[index]; }

    key_type& operator[](size_type index) { return values[index]; }

    /* Get the index of a stored key in the bucket */
    /**
     * Get the index of a stored key's element in the bucket.
     *
     * @param key the key to find
     * @return Index of the found element. If it wasn't found the size of the bucket.
     */
    size_type index_of(const key_type& key) const;

    /* Locate a stored key in the bucket */
    key_type* locate(const key_type& key) const;

    /**
     * Push a key to the bucket.
     *
     * @param key the key to add
     * @return the index where the key was added at.
     */
    size_type add(key_type key);

    /**
     * Count how many times a key exists in the bucket (0 or 1 times):
     *
     * @param key the key to count for
     * @return how many time the key exists (0 or 1)
     */
    size_type count(const key_type& key) const { return locate(key) != nullptr; }

    /**
     * Remove item with key from the bucket.
     *
     * @param key they key to remove
     * @return how many items were removed (0 or 1)
     */
    size_type erase(const key_type& key);

    [[nodiscard]] size_type size() const { return values_size; }

    [[nodiscard]] size_type full() const { return values_size == values_capacity; }

    void swap(Bucket& other);

    void dump(std::ostream& o = std::cerr) const;
};

template<typename Key, size_t N>
class ADS_set<Key, N>::Iterator {
    using bucket_pointer = typename ADS_set<Key, N>::Bucket*;
    using bucket_size_type = typename ADS_set<Key, N>::size_type;

    bucket_pointer current_bucket {nullptr};
    bucket_pointer end_bucket {nullptr};
    bucket_size_type bucket_index {0};

    void skip_empty_buckets() {
        while (current_bucket != end_bucket && current_bucket->size() == 0) {
            ++current_bucket;
        }
    }

public:
    using value_type = Key;
    using difference_type = std::ptrdiff_t;
    using reference = const value_type&;
    using pointer = const value_type*;
    using iterator_category = std::forward_iterator_tag;

    explicit Iterator() = default;

    explicit Iterator(bucket_pointer current_bucket, bucket_pointer end_bucket, bucket_size_type bucket_index) :
            current_bucket {current_bucket},
            end_bucket {end_bucket},
            bucket_index {bucket_index} {
        // Ensure that new Iterator starts with a valid item
        if (current_bucket->size() == bucket_index) {
            this->bucket_index = 0;
            skip_empty_buckets();
        }
    }

    reference operator*() const { return (*current_bucket)[bucket_index]; }

    pointer operator->() const { return &(*current_bucket)[bucket_index]; }

    Iterator& operator++() {
        if (current_bucket == end_bucket) {
            return *this;
        }

        // Increment the bucket index
        ++bucket_index;

        // Go to next non-empty bucket
        if (current_bucket->size() == bucket_index) {
            bucket_index = 0;
            ++current_bucket;
            skip_empty_buckets();
        }

        return *this;
    }

    Iterator operator++(int) {
        Iterator tmp {*this};
        ++*this;
        return tmp;
    }

    friend bool operator==(const Iterator& lhs, const Iterator& rhs) {
        return lhs.current_bucket == rhs.current_bucket && lhs.end_bucket == rhs.end_bucket &&
               lhs.bucket_index == rhs.bucket_index;
    }

    friend bool operator!=(const Iterator& lhs, const Iterator& rhs) { return !(lhs == rhs); }
};

template<typename Key, size_t N>
typename ADS_set<Key, N>::Bucket& ADS_set<Key, N>::Bucket::operator=(Bucket other) {
    swap(other);

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
    values = std::move(new_values);
    values_capacity = new_values_capacity;
}

template<typename Key, size_t N>
typename ADS_set<Key, N>::size_type ADS_set<Key, N>::Bucket::index_of(const ADS_set::key_type& key) const {
    for (size_type i {0}; i < values_size; ++i) {
        if (key_equal {}(values[i], key)) {
            return i;
        }
    }

    return values_size;
}

template<typename Key, size_t N>
typename ADS_set<Key, N>::key_type* ADS_set<Key, N>::Bucket::locate(const key_type& key) const {
    auto index {index_of(key)};

    if (index == values_size) return nullptr;

    return &values[index];
}

template<typename Key, size_t N>
typename ADS_set<Key, N>::size_type ADS_set<Key, N>::Bucket::add(key_type key) {
    // Expand bucket if it exceeds capacity
    if (values_size >= values_capacity) expand();

    // Store key in slice and increment its size
    values[values_size] = std::move(key);

    return values_size++;
}

template<typename Key, size_t N>
typename ADS_set<Key, N>::size_type ADS_set<Key, N>::Bucket::erase(const ADS_set::key_type& key) {
    size_type index {index_of(key)};

    if (index != values_size) return 0;

    key_type& last_item {values[values_size]};

    values[index] = std::move(last_item);

    return 1;
}

template<typename Key, size_t N>
void ADS_set<Key, N>::Bucket::swap(Bucket& other) {
    using std::swap;

    swap(values_size, other.values_size);
    swap(values_capacity, other.values_capacity);
    swap(values, other.values);
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
    table = std::move(new_table);
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
        (void) add(bucket[i]);
    }
}

template<typename Key, size_t N>
std::pair<typename ADS_set<Key, N>::Bucket&, typename ADS_set<Key, N>::size_type> ADS_set<Key, N>::add(key_type key) {
    Bucket& bucket {table[bucket_at(key)]};

    // Ignore already existing keys
    if (bucket.count(key)) {
        return {bucket, bucket.index_of(key)};
    }

    bool should_split = bucket.full();
    auto bucket_index {bucket.add(std::move(key))};

    if (should_split) split();

    ++table_items_size;

    return {bucket, bucket_index};
}

template<typename Key, size_t N>
ADS_set<Key, N>& ADS_set<Key, N>::operator=(ADS_set other) {
    swap(other);

    return *this;
}

template<typename Key, size_t N>
ADS_set<Key, N>& ADS_set<Key, N>::operator=(std::initializer_list<key_type> ilist) {
    ADS_set tmp {ilist};
    swap(tmp);

    return *this;
}

template<typename Key, size_t N>
std::pair<typename ADS_set<Key, N>::iterator, bool> ADS_set<Key, N>::insert(const ADS_set::key_type& key) {
    // If the key already exists, return the end() iterator
    if (count(key)) {
        return {find(key), false};
    }

    std::pair<Bucket&, size_type> result {add(key)};

    return {Iterator {&(result.first), table + table_size, result.second}, true};
}

template<typename Key, size_t N>
void ADS_set<Key, N>::clear() {
    ADS_set tmp;
    swap(tmp);
}

template<typename Key, size_t N>
typename ADS_set<Key, N>::const_iterator ADS_set<Key, N>::begin() const {
    return Iterator {table, table + table_size, 0};
}

template<typename Key, size_t N>
typename ADS_set<Key, N>::const_iterator ADS_set<Key, N>::end() const {
    auto end {table + table_size};

    return Iterator {end, end, 0};
}

template<typename Key, size_t N>
typename ADS_set<Key, N>::size_type ADS_set<Key, N>::erase(const ADS_set::key_type& key) {
    Bucket& bucket {table[bucket_at(key)]};

    return bucket.erase(key);
}

template<typename Key, size_t N>
typename ADS_set<Key, N>::iterator ADS_set<Key, N>::find(const key_type& key) const {
    Bucket* bucket {&table[bucket_at(key)]};
    size_type bucket_index {bucket->index_of(key)};

    // Return the Iterator of the found item
    if (bucket_index != bucket->size()) {
        return Iterator {bucket, table + table_size, bucket_index};
    }

    // Else return the end Iterator
    return end();
}

template<typename Key, size_t N>
void ADS_set<Key, N>::swap(ADS_set& other) {
    using std::swap;

    swap(split_round, other.split_round);
    swap(table_split_index, other.table_split_index);
    swap(table_size, other.table_size);
    swap(table_items_size, other.table_items_size);
    swap(table, other.table);
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

template<typename Key, size_t N>
void swap(ADS_set<Key, N>& first, ADS_set<Key, N>& second) {
    first.swap(second);
}

template<typename Key, size_t N>
void swap(typename ADS_set<Key, N>::Bucket& first, typename ADS_set<Key, N>::Bucket& second) {
    first.swap(second);
}

#endif // ADS_SET_H
