#include <cassert>
#include <iostream>
#include <iomanip>

#include "custom_vector.hpp"

using namespace std;

template <typename T>
bool operator==(const std::vector<T> & lhs, const Vector<T> & rhs) {
    return lhs.size() == rhs.size() 
        && equal(lhs.begin(), lhs.end(), rhs.begin());
}

template <typename T>
bool operator==(const Vector<T> & lhs, const std::vector<T> & rhs) {
    return rhs == lhs;
}

template <typename T>
bool operator!=(const Vector<T> & lhs, const std::vector<T> & rhs) {
    return !(lhs == rhs);
}

template <typename T>
bool operator!=(const std::vector<T> & lhs, const Vector<T> & rhs) {
    return rhs != lhs;
}

template <typename It>
struct PrintableRange {
    It begin;
    It end;
};

template <typename It>
PrintableRange<It> make_printable(It begin, It end) {
    return { begin, end };
}

template <typename It>
ostream & operator<<(ostream & os, const PrintableRange<It> & pr) {
    if (pr.begin == pr.end) {
        return os << "<empty>";
    }
    It it = pr.begin;
    os << "[" << *it;
    ++it;
    while (it != pr.end) {
        os << ", " << *it;
        ++it;
    }
    return os << "]";
}

// -------------------------------------------------------------------------------

void test_constructor(const Vector<int> & actual, const std::vector<int> & expected) {
    assert(actual.satisfies_invariant());
    assert(actual == expected);
}

void test_constructor() {
    test_constructor({ }, { });
    Vector<int> v = { 1, 2, 3 };   
    test_constructor({ v.begin(), v.end() }, { 1, 2, 3 });
    test_constructor(move(v), { 1, 2, 3 });
    test_constructor(v, { 1, 2, 3});
    test_constructor({ 1 }, { 1 });
    test_constructor({ 1, 2 }, { 1, 2 });
    test_constructor({ 1, 2, 3 }, { 1, 2, 3 });
}

void test_assignment(Vector<int> initial, Vector<int> assigned, 
        const std::vector<int> & expected) {
    initial = assigned;
    assert(initial.satisfies_invariant());
    assert(initial == expected);

    initial = move(assigned);
    assert(initial.satisfies_invariant());
    assert(initial == expected);
}

void test_assignment() {
    test_assignment({ }, { 1, 2, 3 }, { 1, 2, 3 });
    test_assignment({ 1, 2 }, { 1, 2, 3 }, { 1, 2, 3 });
    test_assignment({ 1, 2, 3 }, { 1, 2, 3 }, { 1, 2, 3 });
    test_assignment({ 1, 2, 3, 4, 5, 6, 7, 8, 9 }, { 1, 2, 3 }, { 1, 2, 3 });
}

void test_index_access(const std::vector<int> & expected) {
    Vector<int> v{ expected.begin(), expected.end() };
    for (int i = 0; i < v.size(); ++i) {
        assert(v[i] == expected[i]);
    }
}

void test_index_access() {
    test_index_access({ 1 });
    test_index_access({ 1, 2 });
    test_index_access({ 1, 2, 3 });
    test_index_access({ 1, 2, 3, -42, 4324, 24, 324, 324, 324, 234, 23 });
}

void test_push_back(std::vector<int> dataset) {
    Vector<int> v;
    for (int x : dataset) {
        v.push_back(x);
        assert(v.satisfies_invariant());
    }
    assert(v == dataset);
}

void test_push_back() {
    test_push_back({ 1 });
    test_push_back({ 1, 2 });
    test_push_back({ 1, 2, -42 });
}

void test_pop_back(Vector<int> initial, unsigned int remove, std::vector<int> expected) {
    while (remove--) {
        initial.pop_back();
    }
    assert(initial == expected);
}

void test_pop_back() {
    test_pop_back({ 1 }, 1, { });
    test_pop_back({ 1, 2, 3 }, 3, { });
    test_pop_back({ 1, 2, 3 }, 2, { 1 });
    test_pop_back({ 1, 2, 3 }, 1, { 1, 2 });
    test_pop_back({ 1, 2, 3, 5, 42, 3242, 32 }, 3, { 1, 2, 3, 5 });
}

int main() {
    test_constructor();
    test_assignment();
    test_push_back();
    test_pop_back();
    return 0;
}
