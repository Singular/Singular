/*
 * writable_set - A std::multiset wrapper that allows in-place modification of elements
 *
 * Created by Claude Sonnet 4.5 (2024-10-22)
 *
 * OVERVIEW:
 * writable_set provides a container similar to std::multiset, but allows modification
 * of elements through iterators. Normally, std::set and std::multiset don't allow
 * modification because changing an element could violate the sorted invariant.
 * writable_set solves this by storing pointers to elements internally while presenting
 * a clean interface that hides the pointer mechanics from the user.
 *
 * INTERNAL DESIGN:
 * - Uses std::multiset<T*> to store pointers to dynamically allocated T objects
 * - Implements a ptr_compare functor that stores a pointer to the Compare object
 *   and dereferences element pointers when comparing
 * - Provides an iterator_wrapper that automatically dereferences pointers, so users
 *   work directly with T& references in range-based for loops and iterator operations
 *
 * MEMORY MANAGEMENT:
 * - Objects are allocated with new on insert/emplace
 * - Objects are deleted on erase/clear/destruction
 * - RAII principle: the container owns the lifetime of all objects
 *
 * COMPARATOR WITH STATE:
 * - The Compare template parameter can have instance variables
 * - Stores a Compare comp_ member that users can access and modify via key_comp()
 * - The ptr_compare stores a pointer to comp_, so modifications immediately affect
 *   the multiset's comparison behavior (though existing elements won't re-sort
 *   without explicit removal and re-insertion)
 * - Call reorder() after modifying the comparator to rebuild the tree structure
 *   with the new comparison function without reallocating objects
 *
 * USAGE CAVEAT:
 * When modifying elements through iterators, users must ensure the modification
 * doesn't change the sort order. Breaking this invariant will corrupt the set.
 * Example of CORRECT usage:
 *   for (auto& obj : set) { obj.non_sort_field = value; }
 * Example of INCORRECT usage:
 *   for (auto& obj : set) { obj.sort_key = new_value; }
 */

#ifndef WRITABLE_SET_H
#define WRITABLE_SET_H

#include <set>
#include <memory>
#include <iterator>
#include <utility>

template<typename T, typename Compare = std::less<T>>
class writable_set {
private:
    struct ptr_compare {
        Compare* comp;
        ptr_compare(Compare* c) : comp(c) {}
        bool operator()(const T* a, const T* b) const {
            return (*comp)(*a, *b);
        }
    };

    using set_type = std::multiset<T*, ptr_compare>;
    Compare comp_;  // Must be declared before data_ for initialization order
    set_type data_;

public:
    using value_type = T;
    using size_type = typename set_type::size_type;
    using difference_type = typename set_type::difference_type;

    // Iterator that dereferences pointers automatically
    template<typename BaseIterator>
    class iterator_wrapper {
        BaseIterator it_;
        friend class writable_set;

    public:
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = T*;
        using reference = T&;

        iterator_wrapper() = default;
        explicit iterator_wrapper(BaseIterator it) : it_(it) {}

        reference operator*() const { return **it_; }
        pointer operator->() const { return *it_; }

        iterator_wrapper& operator++() { ++it_; return *this; }
        iterator_wrapper operator++(int) { iterator_wrapper tmp = *this; ++it_; return tmp; }
        iterator_wrapper& operator--() { --it_; return *this; }
        iterator_wrapper operator--(int) { iterator_wrapper tmp = *this; --it_; return tmp; }

        bool operator==(const iterator_wrapper& other) const { return it_ == other.it_; }
        bool operator!=(const iterator_wrapper& other) const { return it_ != other.it_; }

        iterator_wrapper operator+(difference_type n) const {
            iterator_wrapper result = *this;
            if (n > 0) {
                for (difference_type i = 0; i < n; ++i) ++result;
            } else {
                for (difference_type i = 0; i > n; --i) --result;
            }
            return result;
        }

        iterator_wrapper operator-(difference_type n) const {
            return *this + (-n);
        }

        iterator_wrapper& operator+=(difference_type n) {
            if (n > 0) {
                for (difference_type i = 0; i < n; ++i) ++(*this);
            } else {
                for (difference_type i = 0; i > n; --i) --(*this);
            }
            return *this;
        }

        iterator_wrapper& operator-=(difference_type n) {
            return *this += (-n);
        }
    };

    using iterator = iterator_wrapper<typename set_type::iterator>;
    using const_iterator = iterator_wrapper<typename set_type::const_iterator>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    // Constructors
    writable_set() : comp_(), data_(ptr_compare(&comp_)) {}
    writable_set(const Compare& comp) : comp_(comp), data_(ptr_compare(&comp_)) {}

    template<typename InputIt>
    writable_set(InputIt first, InputIt last, const Compare& comp = Compare())
        : comp_(comp), data_(ptr_compare(&comp_)) {
        for (; first != last; ++first) {
            insert(*first);
        }
    }

    writable_set(std::initializer_list<T> init, const Compare& comp = Compare())
        : comp_(comp), data_(ptr_compare(&comp_)) {
        for (const auto& item : init) {
            insert(item);
        }
    }

    // Copy constructor
    writable_set(const writable_set& other) : comp_(other.comp_), data_(ptr_compare(&comp_)) {
        for (const auto& item : other) {
            insert(item);
        }
    }

    // Move constructor
    writable_set(writable_set&& other) noexcept
        : comp_(std::move(other.comp_)), data_(ptr_compare(&comp_)) {
        // Move pointers from other without copying objects
        for (auto ptr : other.data_) {
            data_.insert(ptr);
        }
        other.data_.clear();
    }

    // Assignment operators
    writable_set& operator=(const writable_set& other) {
        if (this != &other) {
            clear();
            for (const auto& item : other) {
                insert(item);
            }
        }
        return *this;
    }

    writable_set& operator=(writable_set&& other) noexcept {
        if (this != &other) {
            clear();
            data_ = std::move(other.data_);
        }
        return *this;
    }

    // Destructor
    ~writable_set() {
        clear();
    }

    // Iterators
    iterator begin() { return iterator(data_.begin()); }
    iterator end() { return iterator(data_.end()); }
    const_iterator begin() const { return const_iterator(data_.begin()); }
    const_iterator end() const { return const_iterator(data_.end()); }
    const_iterator cbegin() const { return const_iterator(data_.begin()); }
    const_iterator cend() const { return const_iterator(data_.end()); }

    reverse_iterator rbegin() { return reverse_iterator(end()); }
    reverse_iterator rend() { return reverse_iterator(begin()); }
    const_reverse_iterator rbegin() const { return const_reverse_iterator(end()); }
    const_reverse_iterator rend() const { return const_reverse_iterator(begin()); }
    const_reverse_iterator crbegin() const { return const_reverse_iterator(cend()); }
    const_reverse_iterator crend() const { return const_reverse_iterator(cbegin()); }

    // Capacity
    bool empty() const { return data_.empty(); }
    size_type size() const { return data_.size(); }
    size_type max_size() const { return data_.max_size(); }

    // Modifiers
    void clear() {
        for (auto ptr : data_) {
            delete ptr;
        }
        data_.clear();
    }

    iterator insert(const T& value) {
        T* new_obj = new T(value);
        auto result = data_.insert(new_obj);
        return iterator(result);
    }

    iterator insert(T&& value) {
        T* new_obj = new T(std::move(value));
        auto result = data_.insert(new_obj);
        return iterator(result);
    }

    template<typename... Args>
    iterator emplace(Args&&... args) {
        T* new_obj = new T(std::forward<Args>(args)...);
        auto result = data_.insert(new_obj);
        return iterator(result);
    }

    iterator erase(const_iterator pos) {
        delete *pos.it_;
        return iterator(data_.erase(pos.it_));
    }

    iterator erase(const_iterator first, const_iterator last) {
        for (auto it = first; it != last; ++it) {
            delete *it.it_;
        }
        return iterator(data_.erase(first.it_, last.it_));
    }

    size_type erase(const T& value) {
        auto it = find(value);
        if (it != end()) {
            erase(it);
            return 1;
        }
        return 0;
    }

    // Lookup
    iterator find(const T& value) {
        T temp = value;
        T* temp_ptr = &temp;
        auto it = data_.find(temp_ptr);
        return iterator(it);
    }

    const_iterator find(const T& value) const {
        T temp = value;
        T* temp_ptr = &temp;
        auto it = data_.find(temp_ptr);
        return const_iterator(it);
    }

    size_type count(const T& value) const {
        T temp = value;
        T* temp_ptr = &temp;
        return data_.count(temp_ptr);
    }

    bool contains(const T& value) const {
        return find(value) != end();
    }

    // Access to comparator
    Compare key_comp() const { return comp_; }
    Compare value_comp() const { return comp_; }

    // Non-const access to comparator (allows modification)
    Compare& key_comp() { return comp_; }
    Compare& value_comp() { return comp_; }

    // Reorder - rebuilds the tree with current comparison function
    // without copying or deleting the objects
    void reorder() {
        // Swap the old tree to a local variable
        set_type old_data{ptr_compare(&comp_)};
        old_data.swap(data_);

        // Now data_ is empty, old_data has all our pointers
        // Re-insert all pointers from old tree into new tree
        for (auto ptr : old_data) {
            data_.insert(ptr);
        }

        // old_data destructor just clears pointers, doesn't delete objects
    }
};

#endif // WRITABLE_SET_H
