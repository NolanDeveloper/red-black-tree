#include "tests.hpp"

#include <iostream>

template <typename T>
bool are_equal_nodes(const node_base<T> * lnil, const node_base<T> * lhs,
                     const node_base<T> * rnil, const node_base<T> * rhs) {
    if (lhs == lnil && rhs == rnil) return true;
    if (lhs == lnil || rhs == rnil) return false;
    return lhs->get_value() == rhs->get_value() &&
           lhs->get_color() == rhs->get_color() &&
           are_equal_nodes(lnil, lhs->get_left(), rnil, rhs->get_left()) &&
           are_equal_nodes(lnil, lhs->get_right(), rnil, rhs->get_right());
}

template <typename T>
bool are_equal_trees(const node_base<T> * lhs, const node_base<T> * rhs) {
    return are_equal_nodes(lhs->get_parent(), lhs, rhs->get_parent(), rhs);
}

template <typename T>
void assert_equal(const node_base<T> * expected,
                  const node_base<T> * actual) {
    if (are_equal_trees(expected, actual)) return;
    std::cout << "trees are not equal" << std::endl;
    throw;
}

template <typename T>
void free_node(const node_base<T> * node) {
    while (node) {
        free_node(node->get_right());
        auto left = node->get_left();
        delete node;
        node = left;
    }
}

template <typename T>
node_base<T> * make_node(node_base<T> * right,
                         const T & value, node_color color,
                         node_base<T> * left) {
    try {
        return new node_with_value<T>(right, left, nullptr, color, value);
    } catch (...) {
        free_node(right);
        free_node(left);
        throw;
    }
}

template <typename T>
node_base<T> * make_node(const T & value, node_color color,
                         node_base<T> * left) {
    return make_node<T>(nullptr, value, color, left);
}

template <typename T>
node_base<T> * make_node(node_base<T> * right,
                         const T & value, node_color color) {
    return make_node<T>(right, value, color, nullptr);
}

template <typename T>
node_base<T> * make_node(const T & value, node_color color) {
    return make_node<T>(nullptr, value, color, nullptr);
}

template <typename T>
void replace_nullptr_with_nil(node_base<T> * nil,
                              node_base<T> * node) {
    if (node->get_right()) {
        replace_nullptr_with_nil(nil, node->get_right());
    } else {
        node->set_right(nil); 
    }
    if (node->get_left()) {
        replace_nullptr_with_nil(nil, node->get_left());
    } else {
        node->set_left(nil);
    }
}

template <typename T>
node_base<T> * as_tree(node_base<T> * root) {
    node_base<T> * nil;
    try {
       nil = new node_base<T>();
    } catch (...) {
        free_node(root);
        throw;
    }
    root->set_parent(nil);
    replace_nullptr_with_nil(nil, root);
    return root;
}

void insert_1() {
    const node_base<double> * expected = 
        as_tree(make_node<double>(1, black));
    rb_tree<double> actual;
    actual.insert(1);
    assert_equal(expected, actual.get_root());
}

void insert_1_2_3() {
    const node_base<double> * expected = 
        as_tree(
            make_node(
                make_node(3., red),
                2., black,
                make_node(1., red)));
    rb_tree<double> actual;
    actual.insert(1);
    actual.insert(2);
    actual.insert(3);
    assert_equal(expected, actual.get_root());
}

void test() {
    insert_1();
    insert_1_2_3();
}
