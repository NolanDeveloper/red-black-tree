#include "rb_tree.hpp"

#include <iostream>
#include <vector>
#include <map>
#include <cassert>

using namespace rb_tree;

void assert_equal(const std::unique_ptr<node<int>> & expected, 
                  const std::unique_ptr<node<int>> & actual) {
    if (are_equal(expected.get(), actual.get())) return;
    std::cout << "expected" << std::endl;
    print_by_level(expected.get(), std::cout);
    std::cout << "is not equal to actual" << std::endl;
    print_by_level(actual.get(), std::cout);
    throw;
}

template <typename T>
unsigned check_nested(const node<T> * n) {
    if (!n) return 0;
    // check parents
    if (n->left && n->left->parent != n) {
        std::cout << "left child of " << n->value 
             << " (" << n->left->value << ") "
             << "has wrong parent ";
        if (n->left->parent) {
            std::cout << n->left->parent->value << std::endl;
        } else {
            std::cout << "nullptr" << std::endl;
        }
        throw;
    }
    if (n->right && n->right->parent != n) { 
        std::cout << "right child of " << n->value 
             << " (" << n->right->value << ") "
             << "has wrong parent ";
        if (n->right->parent) {
            std::cout << n->right->parent->value << std::endl;
        } else {
            std::cout << "nullptr" << std::endl;
        }
        throw;
    }
    // check other properties of red-black tree
    if (n->is_red()) {
        if (n->left && n->left->is_red()) {
            std::cout << n->value << " is red, but has red child " 
                      << n->left->value << std::endl;
            throw;
        }
        if (n->right && n->right->is_red()) {
            std::cout << n->value << " is red, but has red child " 
                      << n->right->value << std::endl;
            throw;
        }
    }
    auto left_black_nodes_to_leaf = check_nested(n->left.get());
    auto right_black_nodes_to_leaf = check_nested(n->right.get());
    if (left_black_nodes_to_leaf != right_black_nodes_to_leaf) {
        std::cout << "tree is disbalances below " << n->value
             << " left subtree has " << left_black_nodes_to_leaf
             << " black nodes to leaf, while right has "
             << right_black_nodes_to_leaf << std::endl;
        throw;
    }
    return left_black_nodes_to_leaf + n->is_black();
}

template <typename T>
void check(const std::unique_ptr<node<T>> & root) {
    if (!root) return;
    if (!root->is_black()) {
        std::cout << "root is not black" << std::endl;
        throw;
    }
    check_nested(root.get());
}

template <typename T>
bool insert_and_check(std::unique_ptr<node<T>> & root, const T & x) {
    auto res = insert(root, x);
    check(root);
    return res;
}

template <typename T>
bool insert_and_check_and_print(std::unique_ptr<node<T>> & root, const T & x) {
    auto res = insert(root, x);
    std::cout << "insert " << x << std::endl;
    print_by_level(root, std::cout);
    std::cout << std::endl;
    check(root);
    return res;
}

std::pair<int, node_color> operator"" _b(unsigned long long n) {
    return std::make_pair(n, black);
}

std::pair<int, node_color> operator"" _r(unsigned long long n) {
    return std::make_pair(n, red);
}

std::unique_ptr<node<int>> make_node(
        std::unique_ptr<node<int>> && right,
        std::pair<int, node_color> value,
        std::unique_ptr<node<int>> && left) {
    auto res = std::make_unique<node<int>>(
            std::move(right), 
            value.first, nullptr, value.second, 
            std::move(left));
    res->right->parent = res.get();
    res->left->parent = res.get();
    return res;
}

std::unique_ptr<node<int>> make_node(
        std::pair<int, node_color> value,
        std::unique_ptr<node<int>> && left) {
    auto res = std::make_unique<node<int>>(
            value.first, nullptr, value.second,
            std::move(left));
    res->left->parent = res.get();
    return res;
}

std::unique_ptr<node<int>> make_node(
        std::unique_ptr<node<int>> && right,
        std::pair<int, node_color> value) {
    auto res = std::make_unique<node<int>>(
            std::move(right), 
            value.first, nullptr, value.second);
    res->right->parent = res.get();
    return res;
}

std::unique_ptr<node<int>> make_node(
        std::pair<int, node_color> value) {
    return std::make_unique<node<int>>(value.first, nullptr, value.second);
}

void test_insert_once(int x) {
    std::unique_ptr<node<int>> tree;
    insert(tree, x);
    assert_equal(
        make_node(std::make_pair(x, black)),
        tree);
}

void test_insert_same() {
    std::unique_ptr<node<int>> tree;
    insert(tree, 5);
    assert_equal(
        make_node(5_b),
        tree);
    insert(tree, 5);
    assert_equal(
        make_node(5_b),
        tree);
}

void test_insert_sequential() {
    std::map<int, std::unique_ptr<node<int>>> expected;
    expected[2] = 
        make_node(
            make_node(2_r),
            1_b);

    expected[3] = 
        make_node(
            make_node(3_r),
            2_b,
            make_node(1_r));

    expected[4] = 
        make_node(
            make_node(
                make_node(4_r),
                3_b),
            2_b,
            make_node(1_b));

    expected[5] =
        make_node(
            make_node(
                make_node(5_r),
                4_b,
                make_node(3_r)),
            2_b,
            make_node(1_b));

    expected[6] =
        make_node(
            make_node(
                make_node(
                    make_node(6_r),
                    5_b),
                4_r,
                make_node(3_b)),
            2_b,
            make_node(1_b));

    expected[7] =
        make_node(
            make_node(
                make_node(
                    make_node(7_r),
                    6_b,
                    make_node(5_r)),
                4_r,
                make_node(3_b)),
            2_b,
            make_node(1_b));

    for (auto it = expected.begin(); it != expected.end(); ++it) {
        std::unique_ptr<node<int>> tree;
        for (int i = 1; i <= it->first; ++i) insert(tree, i);
        assert_equal(it->second, tree);
    }
}

void test_left_rotate_root_full() {
    auto tree =
        make_node(
            make_node(
                make_node(5_r),
                4_b,
                make_node(3_r)),
            2_b,
            make_node(1_b));
    left_rotate(tree, tree.get());
    auto expected =
        make_node(
            make_node(5_r),
            4_b,
            make_node(
                make_node(3_r),
                2_b,
                make_node(1_b)));
    assert_equal(expected, tree);
}

void test_right_rotate_root_full() {
    auto tree =
        make_node(
            make_node(5_r),
            4_b,
            make_node(
                make_node(3_r),
                2_b,
                make_node(1_b)));
    right_rotate(tree, tree.get());
    auto expected =
        make_node(
            make_node(
                make_node(5_r),
                4_b,
                make_node(3_r)),
            2_b,
            make_node(1_b));
    assert_equal(expected, tree);
}

void test_left_rotate_root_no_1() {
    auto tree =
        make_node(
            make_node(
                make_node(5_r),
                4_b,
                make_node(3_r)),
            2_b);
    left_rotate(tree, tree.get());
    auto expected =
        make_node(
            make_node(5_r),
            4_b,
            make_node(
                make_node(3_r),
                2_b));
    assert_equal(expected, tree);
}

void test_right_rotate_root_no_1() {
    auto tree =
        make_node(
            5_b,
            make_node(
                make_node(4_r),
                3_b,
                make_node(2_r)));
    right_rotate(tree, tree.get());
    auto expected = 
        make_node(
            make_node(
                5_b,
                make_node(4_r)),
            3_b,
            make_node(2_r));
    assert_equal(expected, tree);
}

void test_erase_one() {
    auto tree = make_node(1_b);
    erase(tree, tree.get());
    auto expected = std::unique_ptr<node<int>>(nullptr);
    assert_equal(expected, tree);
}

int main() {
    test_insert_once(42);
    test_insert_once(100);
    test_insert_once(-100);
    test_insert_once(0);
    test_insert_once(1);
    test_insert_same();
    test_insert_sequential();
    test_left_rotate_root_full();
    test_right_rotate_root_full();
    test_left_rotate_root_no_1();
    test_right_rotate_root_no_1();
    test_erase_one();
    return 0;
}
