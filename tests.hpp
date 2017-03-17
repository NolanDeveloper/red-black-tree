#ifndef TESTS_HPP
#define TESTS_HPP

#include "rb_tree.hpp"

void test();

template <typename T>
unsigned check_nested(const node_base<T> * nil, const node_base<T> * n) {
    if (n == nil) return 0;
    // check childes order
    if (n->left != nil && n->left->get_value() > n->get_value()) {
        std::cout << "Tree is unordered: "
                  << n->left->get_value()
                  << "is left child of "
                  << n->get_value() << std::endl;
        throw;
    }
    if (n->right != nil && n->right->get_value() < n->get_value()) {
        std::cout << "Tree is unordered: "
                  << n->right->get_value()
                  << "is right child of "
                  << n->get_value() << std::endl;
        throw;
    }
    // check parents
    if (n->left != nil && n->left->parent != n) {
        std::cout << "left child of " << n->get_value()
             << " (" << n->left->get_value() << ") "
             << "has wrong parent ";
        if (n->left->parent != nil) {
            std::cout << n->left->parent->get_value() << std::endl;
        } else {
            std::cout << "nil" << std::endl;
        }
        throw;
    }
    if (n->right != nil && n->right->parent != n) {
        std::cout << "right child of " << n->get_value()
             << " (" << n->right->get_value() << ") "
             << "has wrong parent ";
        if (n->right->parent != nil) {
            std::cout << n->right->parent->get_value() << std::endl;
        } else {
            std::cout << "nil" << std::endl;
        }
        throw;
    }
    // check other properties of red-black tree
    if (n->color == red) {
        if (n->left != nil && n->left->color == red) {
            std::cout << n->get_value() << " is red, but has red child "
                      << n->left->get_value() << std::endl;
            throw;
        }
        if (n->right != nil && n->right->color == red) {
            std::cout << n->get_value() << " is red, but has red child "
                      << n->right->get_value() << std::endl;
            throw;
        }
    }
    if (n->left != nil && n->right != nil) {
        auto left_black_height = check_nested<T>(nil, n->left);
        auto right_black_height = check_nested<T>(nil, n->right);
        if (left_black_height != right_black_height) {
            std::cout << "there is disbalance below " << n->get_value()
                 << " left subtree has " << left_black_height
                 << " black nodes to leaf, while right has "
                 << right_black_height << std::endl;
            throw;
        }
    }
    if (n->left != nil) return check_nested<T>(nil, n->left) + (n->color == black);
    if (n->right != nil) return check_nested<T>(nil, n->right) + (n->color == black);
    return n->color == black;
}

template <typename T>
void check(const node_base<T> * root) {
    if (root->color != black) {
        std::cout << "root is not black" << std::endl;
        throw;
    }
    if (root->parent->color != black) {
        std::cout << "nil color is not black" << std::endl;
        throw;
    }
    check_nested<T>(root->parent, root);
}

#endif
