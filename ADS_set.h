#ifndef ADS_SET_H
#define ADS_SET_H

#include <functional>
#include <algorithm>
#include <iostream>
#include <stdexcept>

/**
 * Hash table set implemented with Linear Hashing.
 *
 * Key is the type of the elements.
 * N is the size of each bucket in the hash table. (b in lectures)
 */
template <typename Key, size_t N = 5>
class ADS_set {
public:
  // class /* iterator type (implementation-defined) */;
  using value_type = Key;
  using key_type = Key;
  using reference = value_type &;
  using const_reference = const value_type &;
  using size_type = size_t;
  using difference_type = std::ptrdiff_t;
  // using const_iterator = /* iterator type */;
  // using iterator = const_iterator;
  using key_equal = std::equal_to<key_type>;
  using hasher = std::hash<key_type>;

private:
  /* A bucket's item count threshold until next bucket should be split */
  static const size_type split_bucket_size {N};

  struct Bucket {
    // Make the values array contain the primary and overflow bucket
    key_type values[2u * N];
    size_type size {0};

    /* Add a key to the bucket */
    void add(const key_type &key);

    /* Locate a stored key in the bucket */
    key_type *locate(const key_type &key);

    void dump(std::ostream &o = std::cerr) const;
  };

  /* Current round of splitting (d in lectures) */
  size_type split_round {0};

  /* Index for next bucket to split from hash table (nextToSplit in lectures) */
  size_type table_split_index {0};

  /* Number of buckets in hash table */
  size_type table_size {0};

  /* Number of items in hash table */
  size_type table_items_size {0};

  /* Hash table with buckets */
  Bucket *table {nullptr};

  /* Get the bucket index for a given key in the hash table */
  size_type bucket_at(const key_type &key) const;

  /* Hash function for current split round */
  size_type h(const key_type &key) const {
    return hasher{}(key) % table_size;
  }

  /* Hash function for next split round */
  size_type g(const key_type &key) const {
    return hasher{}(key) % (table_size << 1);
  }

  /* Add a key to the hash table */
  void add(const key_type &key);

  /* Locate a stored key in the hash table */
  key_type *locate(const key_type &key) const;
public:
  /* Create an empty ADS_set with an initial split_round  */
  ADS_set(): split_round {1}, table_size {1u << split_round}, table{new Bucket[table_size]} {}

  /* Create an ADS_set with a given list of items */
  ADS_set(std::initializer_list<key_type> ilist) { insert(ilist); }

  /* Create an ADS_set with a given range of items */
  template<typename InputIt> ADS_set(InputIt first, InputIt last) { insert(first, last); }
  // ADS_set(const ADS_set &other);

  ~ADS_set() { delete[] table; }

  // ADS_set &operator=(const ADS_set &other);
  // ADS_set &operator=(std::initializer_list<key_type> ilist);

  /* Return the count of the stored items in the hash table */
  size_type size() const { return table_items_size; };

  /* Return whether the hash table is empty */
  bool empty() const { return table_items_size == 0; };

  /* Insert the given list of items in the hash table */
  void insert(std::initializer_list<key_type> ilist) { insert(ilist.begin(), ilist.end()); }

  // std::pair<iterator,bool> insert(const key_type &key);

  /* Insert the given range of items in the hash table */
  template<typename InputIt> void insert(InputIt first, InputIt last);

  // void clear();
  // size_type erase(const key_type &key);

  /* Count the items in the hash table with key */
  size_type count(const key_type &key) const { return locate(key) != nullptr; }

  // iterator find(const key_type &key) const;

  // void swap(ADS_set &other);

  // const_iterator begin() const;
  // const_iterator end() const;

  void dump(std::ostream &o = std::cerr) const;

  // friend bool operator==(const ADS_set &lhs, const ADS_set &rhs);
  // friend bool operator!=(const ADS_set &lhs, const ADS_set &rhs);
};

// template <typename Key, size_t N>
// class ADS_set<Key,N>::/* iterator type */ {
// public:
//   using value_type = Key;
//   using difference_type = std::ptrdiff_t;
//   using reference = const value_type &;
//   using pointer = const value_type *;
//   using iterator_category = std::forward_iterator_tag;
//
//   explicit /* iterator type */(/* implementation-dependent */);
//   reference operator*() const;
//   pointer operator->() const;
//   /* iterator type */ &operator++();
//   /* iterator type */ operator++(int);
//   friend bool operator==(const /* iterator type */ &lhs, const /* iterator type */ &rhs);
//   friend bool operator!=(const /* iterator type */ &lhs, const /* iterator type */ &rhs);
// };

// template <typename Key, size_t N>
// void swap(ADS_set<Key,N> &lhs, ADS_set<Key,N> &rhs) { lhs.swap(rhs); }

template<typename Key, size_t N>
typename ADS_set<Key, N>::size_type ADS_set<Key, N>::bucket_at(const key_type &key) const {
  size_type index {h(key)};

  //
  if (index < table_split_index) index = g(key);

  return index;
}

template<typename Key, size_t N>
typename ADS_set<Key, N>::key_type *ADS_set<Key, N>::Bucket::locate(const key_type &key) {
  // Search for an equivalent key in the bucket
  for (size_type i {0}; i < size; ++i) {
    if (key_equal{}(values[i], key)) {
      return &values[i];
    }
  }

  return nullptr;
}

template <typename Key, size_t N>
typename ADS_set<Key, N>::key_type *ADS_set<Key,N>::locate(const key_type &key) const {
  size_type index {bucket_at(key)};

  return table[index].locate(key);
}

template<typename Key, size_t N>
void ADS_set<Key, N>::Bucket::add(const key_type &key) {
  // Store key in bucket and increment the bucket's size
  values[size++] = key;

  // Split if the bucket reached its split threshold
  if (size > split_bucket_size - 1) {
    // TODO Split method here
  }
}

template<typename Key, size_t N>
void ADS_set<Key, N>::add(const key_type &key) {
  // Silently ignore pre-existing keys
  if (count(key)) return;

  size_type index {bucket_at(key)};

  table[index].add(key);

  ++table_items_size;
}

template<typename Key, size_t N>
template<typename InputIt> void ADS_set<Key, N>::insert(InputIt first, InputIt last) {
  for (auto it{first}; it != last; ++it) {
    add(*it);
  }
}

template<typename Key, size_t N>
void ADS_set<Key, N>::Bucket::dump(std::ostream &o) const {
  o << "(size: " << std::setfill(' ') << std::setw(5) << size << ") | ";

  for (size_type i {}; i < size; ++i) {
    if (i == N) o << " -> | ";
    o << values[i] << " ";
  }
}

template<typename Key, size_t N>
void ADS_set<Key, N>::dump(std::ostream &o) const {
  o << "table_size = " << table_size << ", table_items_size = " << table_items_size << ", table_split_index = " << table_split_index << "\n\n";
  o << "=== HASH TABLE ===\n\n";

  for (size_type i {0}; i < table_size; ++i) {
    o << i << " | ";
    table[i].dump(o);
    o << "\n";
  }

  o << "\n";
}

#endif // ADS_SET_H
