#pragma once

#include <algorithm>
#include <utility>
#include <iterator>
#include <cstring>
#include <iostream> 
#include <iterator>

template <typename ValType, typename Allocator = std::allocator<ValType>>
class Vector {
    using value_type = ValType;
    using reference = ValType &;
    using const_reference  = const ValType &;
    using iterator = ValType *;
    using const_iterator = const ValType *;
    using difference_type = typename std::iterator_traits<const_iterator>::difference_type;
    using size_type = typename Allocator::size_type;

    using ValPtr = ValType *;
    using ValCPtr = const ValType *;
    using ValRef = ValType &;
    using ValCRef = const ValType &;

    Allocator _allocator;
    size_type _size;
    size_type _capacity;
    ValPtr _data;

    void destroy(ValPtr begin, ValPtr end) {
        while (begin != end) {
            begin->~ValType();
            ++begin;
        }
    }

    void realloc(size_type new_size) {
        using namespace std;

        auto new_array = _allocator.allocate(new_size);
        try {
            uninitialized_copy(_data, _data + min(_size, new_size), new_array);
        } catch (...) {
            _allocator.deallocate(new_array, new_size);
            throw;
        }
        destroy(_data, _data + _size);
        _allocator.deallocate(_data, _capacity);
        _data = new_array;
    }

    size_type calculate_new_capacity(size_type new_size) {
        size_type new_capacity = _capacity ? _capacity : 1;
        while (new_capacity < new_size) new_capacity *= 2;
        return new_capacity;
    }

public:
    Vector()
            : _size(0)
            , _capacity(0) 
            , _data(nullptr) { }

    Vector(const Vector & v)
            : _size(v._size)
            , _capacity(v._capacity) 
            , _data(v._data == nullptr ? nullptr : _allocator.allocate(v._capacity)) { 
        if (!_data) return;
        try {
            std::uninitialized_copy(v.begin(), v.end(), _data);
        } catch (...) {
            _allocator.deallocate(_data, _capacity);
        }
    }

    Vector(Vector && v)
            : _size(v._size)
            , _capacity(v._capacity) 
            , _data(v._data) {
        v._data = nullptr;
    }

    template <typename It>
    Vector(It begin, It end)
            : _size(0)
            , _capacity(0) 
            , _data(nullptr) {
        using std::uninitialized_copy;
        using std::distance;

        resize(distance(begin, end));
        try {
            uninitialized_copy(begin, end, _data);
        } catch (...) {
            _allocator.deallocate(_data, _capacity);
        }
    }

    Vector(std::initializer_list<ValType> initializer) 
            : _size(initializer.size())
            , _capacity(calculate_new_capacity(initializer.size()))
            , _data(_allocator.allocate(_capacity)) {
        using std::uninitialized_copy;

        try {
            uninitialized_copy(initializer.begin(), initializer.end(), _data);
        } catch (...) {
            _allocator.deallocate(_data, _capacity);
        }
    }

    ~Vector() {
        if (!_data) return;
        destroy(_data, _data + _size);
        _allocator.deallocate(_data, _capacity);
    }

    Vector & operator=(Vector v) {
        using std::swap;

        swap(_data, v._data);
        swap(_size, v._size);
        swap(_capacity, v._capacity);
        return *this;
    }

    ValCRef operator[](size_type i) const { return _data[i]; }
    ValRef operator[](size_type i) { 
        return const_cast<ValRef>(static_cast<const Vector &>(*this)[i]); 
    }

private:
    template <typename K>
    void push_back_impl(K && x) {
        if (_size == _capacity) {
            reserve(calculate_new_capacity(_size + 1));
        }
        new (_data + _size) ValType(std::forward<K>(x));
        ++_size;
    }

public:
    void push_back(ValType && x) { push_back_impl(x); }
    void push_back(const ValType & x) { push_back_impl(x); }

    void pop_back() { 
        --_size; 
        (_data + _size)->~ValType();
    }

    ValCRef back() const { return _data[_size - 1]; }
    ValRef back() { 
        return const_cast<ValRef>(static_cast<const Vector &>(*this).back()); 
    }

    ValCRef front() const { return _data[0]; }
    ValRef front() { 
        return const_cast<ValRef>(static_cast<const Vector &>(*this).front()); 
    }

    bool empty() const { return !_size; }

    ValCPtr begin() const { return _data; }
    ValPtr begin() {
        return const_cast<ValPtr>(static_cast<const Vector &>(*this).begin()); 
    }
    
    ValCPtr end() const { return _data + _size; }
    ValPtr end() { 
        return const_cast<ValPtr>(static_cast<const Vector &>(*this).end()); 
    }

    void resize(size_type n) {
        if (n > _size) {
            reserve(n);
            auto it = _data + _size;
            auto end = _data + n;
            while (it != end) {
                new (it++) ValType();
            }
        } 
        _size = n;
    }

    void reserve(size_type n) {
        if (n <= _capacity) return;
        realloc(n);
        _capacity = n;
    }

    size_type size() const { return _size; }
    size_type capacity() const { return _capacity; }

private:
    template <typename K>
    ValPtr insert_impl(ValPtr where, K && x) {
        using std::forward;
        using std::uninitialized_copy;

        ValPtr result;
        if (_size == _capacity) {
            auto new_capacity = calculate_new_capacity(_size + 1);
            ValPtr new_data = _allocator.allocate(new_capacity);
            auto it = uninitialized_copy(_data, where, new_data);
            new (it++) ValType(forward<K>(x));
            uninitialized_copy(where, _data + _size, it);
            _allocator.deallocate(_data, _size);
            _data = new_data;
            ++_size;
            _capacity = new_capacity;
            return it;
        } else {
            auto it = _data + _size;
            new (it) ValType(forward<K>(x));
            ++_size;
            return it;
        }
    }

public:
    ValPtr insert(ValPtr where, ValType && x) { return insert_impl(where, x); }
    ValPtr insert(ValPtr where, const ValType & x) { return insert_impl(where, x); }

    bool satisfies_invariant() const {
        if (_data) {
            return _capacity > 0 && _size > 0 && _size <= _capacity;
        } else {
            return !_capacity && !_size;
        }
    }
};

