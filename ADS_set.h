#ifndef ADS_SET_H
#define ADS_SET_H

#include <functional>
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <stdexcept>

/**
 * Set implemented with Linear hashing scheme.
 *
 * @tparam Key key type
 * @tparam N size of the buckets (b in lectures)
 */
template<typename Key, size_t N = 5>
class ADS_set {
public:
    class Bucket;

    class Iterator;

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
    /** Split round (d in lectures) */
    size_type split_round {0};

    /** Index of next bucket that should be split (nextToSplit in lectures) */
    size_type table_split_index {0};

    /** Number of buckets */
    size_type table_size {0};

    /** Number of total values stored in buckets */
    size_type table_items_size {0};

    /** Table of buckets */
    Bucket* table {nullptr};

    /** Hash instance */
    const hasher hash {};

    /** Hash function for current split round */
    size_type h(const key_type& key) const {
        return hash(key) % (1 << split_round);
    }

    /** Hash function for next split round */
    size_type g(const key_type& key) const {
        return hash(key) % (1 << (split_round + 1));
    }

    /**
     * Get the bucket where the given key's value should be at.
     *
     * @param key the key to probe for
     * @return reference to bucket
     */
    Bucket& bucket_at(const key_type& key) const;

    /**
     * Allocates the given amount of buckets for the hash table.
     * This method will silently ignore smaller new table sizes.
     *
     * @param new_table_size
     */
    void reserve(size_type new_table_size);

    /**
     * Split the next bucket that should be split.
     */
    void split();

public:
    /**
     * Creates an empty set.
     */
    ADS_set();

    /**
     * Delete the set.
     */
    ~ADS_set();

    /**
     * Creates a set with a given range of items.
     *
     * @tparam InputIt type of input iterator
     * @param first first item in range
     * @param last last item in range
     */
    template<typename InputIt>
    ADS_set(InputIt first, InputIt last);

    /**
     * Creates an ADS_set with a given list of keys.
     *
     * @param ilist list of keys to initialize with
     */
    ADS_set(std::initializer_list<key_type> ilist);

    /**
     * Creates a copy of a given set.
     *
     * @param other other set to copy from
     */
    ADS_set(const ADS_set& other);

    /**
     * Creates a set by moving values from other set.
     *
     * @param other other set to move from
     */
    ADS_set(ADS_set&& other) noexcept;

    /**
     * Copies the values of other set to this set by assignment operator.
     *
     * @param other other set to copy from
     * @return reference to this set
     */
    ADS_set& operator=(ADS_set other);

    /**
     * Copies the list of keys to this set by assignment operator.
     *
     * @param ilist list of keys to copy from
     * @return reference to this set
     */
    ADS_set& operator=(std::initializer_list<key_type> ilist);

    /**
     * Insert a given key.
     *
     * @param key the key to insert
     * @return iterator for value and boolean whether it was newly added
     */
    std::pair<iterator, bool> insert(const key_type& key);

    /**
     * Insert a range of given keys.
     *
     * @tparam InputIt type of input iterator
     * @param first first item in range
     * @param last last item in range
     */
    template<typename InputIt>
    void insert(InputIt first, InputIt last);

    /**
     * Insert a given list of keys.
     *
     * @param ilist list of keys to insert
     */
    void insert(std::initializer_list<key_type> ilist);

    /**
     * Clear all values of the set.
     */
    void clear();

    /**
     * Removes the given key from the hash table.
     *
     * @param key the key to remove
     * @return the amount of removed elements
     */
    size_type erase(const key_type& key);

    /**
     * Count how many times a key exists in the set (0 or 1).
     *
     * @param key the key to count for
     * @return how many times the key exists (0 or 1)
     */
    size_type count(const key_type& key) const;

    /**
     * Finds the given key's value in the hash table.
     *
     * @param key the key to find
     * @return iterator of found value; if nothing was found the end iterator
     */
    iterator find(const key_type& key) const;

    /**
     * Swap this set with the given other set.
     *
     * @param other the set to swap with
     */
    void swap(ADS_set& other);

    /**
     * Get the iterator to the first item in the hash table.
     *
     * @return iterator to first item
     */
    const_iterator begin() const;

    /**
     * Get the Iterator after the last item in the hash table.
     *
     * @return Iterator after the last item
     */
    const_iterator end() const;

    /**
     * Get the total amount of stored values.
     *
     * @return total amount of stored values
     */
    [[nodiscard]] size_type size() const { return table_items_size; };

    /**
     * Get whether the set is empty.
     *
     * @return if set is empty
     */
    [[nodiscard]] bool empty() const { return table_items_size == 0; };

    /**
     * Dump the set's content to a given stream.
     *
     * @param o the stream to dump to
     */
    void dump(std::ostream& o = std::cerr) const;

    friend bool operator==(const ADS_set& lhs, const ADS_set& rhs) {
        if (lhs.table_items_size != rhs.table_items_size) return false;

        for (const auto& item: lhs) {
            if (!rhs.count(item)) return false;
        }

        return true;
    }

    friend bool operator!=(const ADS_set& lhs, const ADS_set& rhs) {
        return !(lhs == rhs);
    }
};

template<typename Key, size_t N>
class ADS_set<Key, N>::Bucket {
    /** Amount of stored values */
    size_type values_size {0};

    /** Capacity of bucket */
    size_type values_capacity {0};

    /** Array of values */
    value_type* values {nullptr};

    /**
     * Expand the capacity of Bucket by N values.
     */
    void expand();

public:
    /**
     * Creates an empty bucket.
     */
    Bucket();

    /**
     * Delete this bucket.
     */
    ~Bucket();

    /**
     * Creates a copy of a given bucket.
     *
     * @param other other bucket to copy from
     */
    Bucket(const Bucket& other);

    /**
     * Creates a bucket by moving values from other bucket.
     *
     * @param other other bucket to move from
     */
    Bucket(Bucket&& other) noexcept;

    /**
     * Copies the values of the other bucket to this bucket by assignment operator.
     *
     * @param other other bucket to copy from
     * @return reference to this bucket
     */
    Bucket& operator=(Bucket other);

    /**
     * Get the value at a given index from the bucket.
     *
     * @param index index of value
     * @return reference to value
     */
    reference operator[](size_type index);

    /**
     * Get the constant value at a given index from the bucket.
     *
     * @param index index of value
     * @return constant reference to value
     */
    const_reference operator[](size_type index) const;

    /**
     * Get the index of a stored key's value in the bucket.
     *
     * @param key the key to find
     * @return Index of the found element; if it wasn't found the size of the bucket
     */
    size_type index_of(const key_type& key) const;

    /**
     * Locate the value stored with the given key.
     *
     * @param key the key to locate for
     * @return pointer to found value; if nothing was found nullptr
     */
    value_type* locate(const key_type& key) const;

    /**
     * Push a key to the bucket.
     *
     * @param key the key to insert
     * @return the index where the key was added at.
     */
    std::pair<size_type, bool> insert(key_type key);

    /**
     * Count how many times a key exists in the bucket (0 or 1 times):
     *
     * @param key the key to count for
     * @return how many times the key exists (0 or 1)
     */
    size_type count(const key_type& key) const;

    /**
     * Remove item with key from the bucket.
     *
     * @param key they key to remove
     * @return how many items were removed (0 or 1)
     */
    size_type erase(const key_type& key);

    /**
     * Swap this bucket with the given other bucket.
     *
     * @param other the bucket to swap with
     */
    void swap(Bucket& other);

    /**
     * Get the amount of stored values.
     *
     * @return amount of stored values
     */
    [[nodiscard]] size_type size() const { return values_size; }

    /**
     * Get the amount of available values.
     *
     * @return amount of available values
     */
    [[nodiscard]] size_type capacity() const { return values_capacity; }

    /**
     * Get whether the bucket is full.
     *
     * @return if bucket is full
     */
    [[nodiscard]] size_type full() const { return values_size == values_capacity; }

    /**
     * Dump the bucket's content to a given stream.
     *
     * @param o the stream to dump to
     */
    void dump(std::ostream& o = std::cerr) const;
};

template<typename Key, size_t N>
class ADS_set<Key, N>::Iterator {
public:
    using value_type = Key;
    using difference_type = std::ptrdiff_t;
    using reference = const value_type&;
    using pointer = const value_type*;
    using iterator_category = std::forward_iterator_tag;
private:
    using bucket_pointer = typename ADS_set<Key, N>::Bucket*;
    using bucket_size_type = typename ADS_set<Key, N>::size_type;

    /** Pointer to current bucket */
    bucket_pointer current {nullptr};

    /** Pointer to end bucket */
    bucket_pointer end {nullptr};

    /** Index of current value in current bucket */
    bucket_size_type index {0};

    /**
     * Advance current bucket until bucket has values or is at end bucket.
     */
    void skip_empty_buckets();

public:
    /**
     * Default constructor
     */
    Iterator() = default;

    /**
     * Creates iterator with current and end bucket and index to current value.
     *
     * @param current pointer to current bucket
     * @param end pointer to end bucket
     * @param index index to current value in current bucket
     */
    explicit Iterator(bucket_pointer current, bucket_pointer end, bucket_size_type index);

    reference operator*() const;

    pointer operator->() const;

    Iterator& operator++();

    Iterator operator++(int);

    friend bool operator==(const Iterator& lhs, const Iterator& rhs) {
        return lhs.current == rhs.current && lhs.end == rhs.end &&
               lhs.index == rhs.index;
    }

    friend bool operator!=(const Iterator& lhs, const Iterator& rhs) {
        return !(lhs == rhs);
    }
};

template<typename Key, size_t N>
typename ADS_set<Key, N>::Bucket& ADS_set<Key, N>::bucket_at(const key_type& key) const {
    size_type index {h(key)};

    // Use next split round's hash function for already split buckets
    if (index < table_split_index) {
        index = g(key);
    }

    return table[index];
}


template<typename Key, size_t N>
void ADS_set<Key, N>::reserve(size_type new_table_size) {
    // Ignore calls that request making the table smaller
    if (table_size >= new_table_size) return;

    // Reserve memory for the new_table's buckets
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

    // Remove values from bucket to be split by moving bucket
    Bucket bucket {std::move(table[table_split_index])};

    // Decrement the total items size by what has been removed by the bucket move
    table_items_size -= bucket.size();

    if (table_split_index >= max_table_size) {
        // Advance split round if all buckets have been split
        table_split_index = 0;
        ++split_round;
    } else {
        // Else just advance split index
        ++table_split_index;
    }

    // Add removed values back to set
    for (size_type i {0}; i < bucket.size(); ++i) {
        insert(bucket[i]);
    }
}

template<typename Key, size_t N>
ADS_set<Key, N>::ADS_set() : split_round {1}, table_size {1u << split_round}, table {new Bucket[table_size]} {}

template<typename Key, size_t N>
ADS_set<Key, N>::~ADS_set() {
    delete[] table;
}

template<typename Key, size_t N>
template<typename InputIt>
ADS_set<Key, N>::ADS_set(InputIt first, InputIt last): ADS_set {} {
    insert(first, last);
}

template<typename Key, size_t N>
ADS_set<Key, N>::ADS_set(std::initializer_list<key_type> ilist) : ADS_set {ilist.begin(), ilist.end()} {}

template<typename Key, size_t N>
ADS_set<Key, N>::ADS_set(const ADS_set& other) : ADS_set {other.begin(), other.end()} {}

template<typename Key, size_t N>
ADS_set<Key, N>::ADS_set(ADS_set&& other) noexcept: ADS_set {} {
    swap(other);
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
    // Reference bucket where key should be inserted
    Bucket* bucket {&bucket_at(key)};

    // Split bucket if it's full
    if (bucket->full()) {
        split();

        // Insert bucket might need an update after split
        bucket = &bucket_at(key);
    }

    // Try to insert key in bucket
    auto [index, added] = bucket->insert(key);

    // Increment items size if value was added
    if (added) ++table_items_size;

    Iterator it {bucket, table + table_size, index};

    return {it, added};
}

template<typename Key, size_t N>
template<typename InputIt>
void ADS_set<Key, N>::insert(InputIt first, InputIt last) {
    for (auto it {first}; it != last; ++it) {
        insert(*it);
    }
}

template<typename Key, size_t N>
void ADS_set<Key, N>::insert(std::initializer_list<key_type> ilist) {
    insert(ilist.begin(), ilist.end());
}

template<typename Key, size_t N>
void ADS_set<Key, N>::clear() {
    // Clear all values by creating new empty set and swap them
    ADS_set tmp;
    swap(tmp);
}

template<typename Key, size_t N>
typename ADS_set<Key, N>::size_type ADS_set<Key, N>::erase(const ADS_set::key_type& key) {
    // Reference bucket where key's value should be at
    Bucket& bucket {bucket_at(key)};

    // Try to erase value from bucket
    size_type erased {bucket.erase(key)};

    // Decrement amount of items by how much was erased
    table_items_size -= erased;

    return erased;
}

template<typename Key, size_t N>
typename ADS_set<Key, N>::size_type ADS_set<Key, N>::count(const key_type& key) const {
    // Reference where value should be at
    Bucket& bucket {bucket_at(key)};

    // Check if key could be found in bucket
    return bucket.locate(key) != nullptr;
}

template<typename Key, size_t N>
typename ADS_set<Key, N>::iterator ADS_set<Key, N>::find(const key_type& key) const {
    // Reference bucket where key's value should be at
    Bucket* bucket {&bucket_at(key)};

    // Check if value with key exists in bucket
    size_type index {bucket->index_of(key)};

    // Return iterator to the found item
    if (index < bucket->capacity()) {
        return Iterator(bucket, table + table_size, index);
    }

    // If nothing was found return end iterator
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
typename ADS_set<Key, N>::const_iterator ADS_set<Key, N>::begin() const {
    return Iterator {table, table + table_size, 0};
}

template<typename Key, size_t N>
typename ADS_set<Key, N>::const_iterator ADS_set<Key, N>::end() const {
    auto end {table + table_size};

    return Iterator {end, end, 0};
}

template<typename Key, size_t N>
void ADS_set<Key, N>::dump(std::ostream& o) const {
    o << "split_round = " << split_round;
    o << ", table_split_index = " << table_split_index;
    o << ", table_size = " << table_size;
    o << ", table_items_size = " << table_items_size;
    o << "\n\n";

    for (size_type i {0}; i < table_size; ++i) {
        o << (table_split_index == i ? "-> " : "   ");
        o << std::setfill(' ') << std::setw(4) << i << " | ";
        table[i].dump(o);
        o << "\n";
    }

    o << "\n";
}

template<typename Key, size_t N>
ADS_set<Key, N>::Bucket::Bucket() : values_capacity {N}, values {new value_type[values_capacity]} {}

template<typename Key, size_t N>
ADS_set<Key, N>::Bucket::~Bucket() {
    delete[] values;
}

template<typename Key, size_t N>
ADS_set<Key, N>::Bucket::Bucket(const Bucket& other) : Bucket {} {
    swap(other);
}

template<typename Key, size_t N>
ADS_set<Key, N>::Bucket::Bucket(Bucket&& other) noexcept: Bucket {} {
    swap(other);
}

template<typename Key, size_t N>
typename ADS_set<Key, N>::Bucket& ADS_set<Key, N>::Bucket::operator=(Bucket other) {
    swap(other);

    return *this;
}

template<typename Key, size_t N>
typename ADS_set<Key, N>::reference ADS_set<Key, N>::Bucket::operator[](size_type index) {
    return values[index];
}

template<typename Key, size_t N>
typename ADS_set<Key, N>::const_reference ADS_set<Key, N>::Bucket::operator[](size_type index) const {
    return values[index];
}

template<typename Key, size_t N>
void ADS_set<Key, N>::Bucket::expand() {
    size_type new_values_capacity {values_size + N};
    value_type* new_values {new value_type[new_values_capacity]};

    // Copy/Move values to new_values
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

    return values_capacity;
}

template<typename Key, size_t N>
typename ADS_set<Key, N>::value_type* ADS_set<Key, N>::Bucket::locate(const key_type& key) const {
    size_type index {index_of(key)};

    if (index == values_capacity) return nullptr;

    return &values[index];
}

template<typename Key, size_t N>
std::pair<typename ADS_set<Key, N>::size_type, bool> ADS_set<Key, N>::Bucket::insert(key_type key) {
    size_type index {index_of(key)};

    // Ignore insert if key already exists
    if (index != values_capacity) {
        return {index, false};
    }

    // If size exceeds capacity, expand it
    if (values_size >= values_capacity) expand();

    // Store key and increase bucket's size
    values[index = values_size++] = std::move(key);

    return {index, true};
}

template<typename Key, size_t N>
typename ADS_set<Key, N>::size_type ADS_set<Key, N>::Bucket::count(const key_type& key) const {
    return locate(key) != nullptr;
}

template<typename Key, size_t N>
typename ADS_set<Key, N>::size_type ADS_set<Key, N>::Bucket::erase(const ADS_set::key_type& key) {
    size_type index {index_of(key)};

    // Do not erase anything if value couldn't be found
    if (index == values_capacity) return 0;

    // Replace found value with the last item and decrease bucket's size
    values[index] = std::move(values[--values_size]);

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
void ADS_set<Key, N>::Iterator::skip_empty_buckets() {
    while (current != end && current->size() == 0) {
        ++current;
    }
}

template<typename Key, size_t N>
ADS_set<Key, N>::Iterator::Iterator(bucket_pointer current, bucket_pointer end, bucket_size_type index) :
        current {current}, end {end}, index {index} {
    if (index >= current->size()) {
        this->index = 0;
        skip_empty_buckets();
    }
}

template<typename Key, size_t N>
typename ADS_set<Key, N>::Iterator::reference ADS_set<Key, N>::Iterator::operator*() const {
    return (*current)[index];
}

template<typename Key, size_t N>
typename ADS_set<Key, N>::Iterator::pointer ADS_set<Key, N>::Iterator::operator->() const {
    return &(operator*());
}

template<typename Key, size_t N>
typename ADS_set<Key, N>::Iterator& ADS_set<Key, N>::Iterator::operator++() {
    // Do not advance when we reached the end bucket
    if (current == end) {
        return *this;
    }

    // Increment the bucket index
    ++index;

    // Go to next non-empty bucket
    if (index >= current->size()) {
        index = 0;
        ++current;

        skip_empty_buckets();
    }

    return *this;
}

template<typename Key, size_t N>
typename ADS_set<Key, N>::Iterator ADS_set<Key, N>::Iterator::operator++(int) {
    Iterator tmp {*this};
    ++*this;
    return tmp;
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
