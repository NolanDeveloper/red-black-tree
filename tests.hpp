#ifndef TESTS_HPP
#define TESTS_HPP

#include "rb_tree.hpp"

void test();

template <typename T>
unsigned check_nested(const node_base<T> * nil, const node_base<T> * n) {
    if (n == nil) return 0;
    // check childes order
    if (n->get_left() != nil && n->get_left()->get_value() > n->get_value()) {
        std::cout << "Tree is unordered: "
                  << n->get_left()->get_value()
                  << "is left child of "
                  << n->get_value() << std::endl;
        throw;
    }
    if (n->get_right() != nil && n->get_right()->get_value() < n->get_value()) {
        std::cout << "Tree is unordered: "
                  << n->get_right()->get_value()
                  << "is right child of "
                  << n->get_value() << std::endl;
        throw;
    }
    // check parents
    if (n->get_left() != nil && n->get_left()->get_parent() != n) {
        std::cout << "left child of " << n->get_value()
             << " (" << n->get_left()->get_value() << ") "
             << "has wrong parent ";
        if (n->get_left()->get_parent() != nil) {
            std::cout << n->get_left()->get_parent()->get_value() << std::endl;
        } else {
            std::cout << "nil" << std::endl;
        }
        throw;
    }
    if (n->get_right() != nil && n->get_right()->get_parent() != n) { 
        std::cout << "right child of " << n->get_value()
             << " (" << n->get_right()->get_value() << ") "
             << "has wrong parent ";
        if (n->get_right()->get_parent() != nil) {
            std::cout << n->get_right()->get_parent()->get_value() << std::endl;
        } else {
            std::cout << "nil" << std::endl;
        }
        throw;
    }
    // check other properties of red-black tree
    if (n->get_color() == red) {
        if (n->get_left() != nil && n->get_left()->get_color() == red) {
            std::cout << n->get_value() << " is red, but has red child " 
                      << n->get_left()->get_value() << std::endl;
            throw;
        }
        if (n->get_right() != nil && n->get_right()->get_color() == red) {
            std::cout << n->get_value() << " is red, but has red child " 
                      << n->get_right()->get_value() << std::endl;
            throw;
        }
    }
    if (n->get_left() != nil && n->get_right() != nil) {
        auto left_black_height = check_nested<T>(nil, n->get_left());
        auto right_black_height = check_nested<T>(nil, n->get_right());
        if (left_black_height != right_black_height) {
            std::cout << "there is disbalance below " << n->get_value()
                 << " left subtree has " << left_black_height
                 << " black nodes to leaf, while right has "
                 << right_black_height << std::endl;
            throw;
        }
    }
    if (n->get_left() != nil) return check_nested<T>(nil, n->get_left()) + (n->get_color() == black);
    if (n->get_right() != nil) return check_nested<T>(nil, n->get_right()) + (n->get_color() == black);
    return n->get_color() == black;
}

template <typename T>
void check(const node_base<T> * root) {
    if (root->get_color() != black) {
        std::cout << "root is not black" << std::endl;
        throw;
    }
    if (root->get_parent()->get_color() != black) {
        std::cout << "nil color is not black" << std::endl;
        throw;
    }
    if (root->get_parent()->get_left()) {// != root->get_parent()) {
        std::cout << "nil->get_left() != nullptr" << std::endl;
        throw;
    }
    if (root->get_parent()->get_right()) {// != root->get_parent()) {
        std::cout << "nil->get_right() != nullptr" << std::endl;
        throw;
    }
    check_nested<T>(root->get_parent(), root);
}

#endif
