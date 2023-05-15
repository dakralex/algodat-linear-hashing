#ifndef ADS_SET_H
#define ADS_SET_H

#include <functional>
#include <algorithm>
#include <iostream>
#include <stdexcept>

template <typename Key, size_t N = 1>
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
  /* Node struct for the records in a Bucket implemented as a linked list */
  struct Node {
    key_type key;
    Node *next {nullptr};
    Node(const key_type &k) : key{k} {}
  };

  /* Bucket struct for records in table */
  struct Bucket {
    Node *items {nullptr};
  };

  /* Number of Bucket elements in table */
  size_type table_size {0};

  /* Number of Node elements in table */
  size_type table_items {0};

  /* Index for next bucket to split from table */
  size_type table_split_index {0};

  /* Table with stored buckets */
  Bucket *table {nullptr};

  size_type h(const key_type &key) const {
    return hasher{}(key) % table_size;
  }
public:
  /* Create an ADS_set with an intial size of 2^N buckets */
  ADS_set(): table_size{1 << N}, table{new Bucket[table_size]} {}
  ADS_set(std::initializer_list<key_type> ilist);
  template<typename InputIt> ADS_set(InputIt first, InputIt last);
  // ADS_set(const ADS_set &other);

  // ~ADS_set();

  // ADS_set &operator=(const ADS_set &other);
  // ADS_set &operator=(std::initializer_list<key_type> ilist);

  size_type size() const { return table_items; };
  bool empty() const { return table_items == 0; };

  void insert(std::initializer_list<key_type> ilist);
  // std::pair<iterator,bool> insert(const key_type &key);
  template<typename InputIt> void insert(InputIt first, InputIt last);

  // void clear();
  // size_type erase(const key_type &key);

  size_type count(const key_type &key) const;
  // iterator find(const key_type &key) const;

  // void swap(ADS_set &other);

  // const_iterator begin() const;
  // const_iterator end() const;

  // void dump(std::ostream &o = std::cerr) const;

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

#endif // ADS_SET_H
