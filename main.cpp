#include "rb_tree.hpp"

#include <iostream>
#include <cassert>

using namespace std;

template <typename T>
class debug_rb_tree : public rb_tree<T> {
public:
    using node = typename rb_tree<T>::node;
    using base = rb_tree<T>;

    static unsigned check(node * n) {
        if (!n) return 0;
        // check parents
        if (n->left && n->left->parent != n) {
            cout << "left child of " << n->value 
                 << " (" << n->left->value << ") "
                 << "has wrong parent ";
            if (n->left->parent) {
                cout << n->left->parent->value << endl;
            } else {
                cout << "nullptr" << endl;
            }
            throw;
        }
        if (n->right && n->right->parent != n) { 
            cout << "right child of " << n->value 
                 << " (" << n->right->value << ") "
                 << "has wrong parent ";
            if (n->right->parent) {
                cout << n->right->parent->value << endl;
            } else {
                cout << "nullptr" << endl;
            }
            throw;
        }
        // check other properties of red-black tree
        if (n->is_red()) {
            if (n->left && n->left->is_red()) {
                cout << n->value << " is red, but has red child " 
                     << n->left->value << endl;
                throw;
            }
            if (n->right && n->right->is_red()) {
                cout << n->value << " is red, but has red child " 
                     << n->right->value << endl;
                throw;
            }
        }
        auto left_black_nodes_to_leaf = check(n->left.get());
        auto right_black_nodes_to_leaf = check(n->right.get());
        if (left_black_nodes_to_leaf != right_black_nodes_to_leaf) {
            cout << "tree is disbalances below " << n->value
                 << " left subtree has " << left_black_nodes_to_leaf
                 << " black nodes to leaf, while right has "
                 << right_black_nodes_to_leaf << endl;
            throw;
        }
        return left_black_nodes_to_leaf + n->is_black();
    }

    bool check() const { 
        if (!base::root) return true;
        if (!base::root->is_black()) {
            cout << "root isn't black" << endl;
            return false;
        }
        return check(base::root.get());
    }

    bool insert(const T & x) {
        auto res = rb_tree<T>::insert(x);
        check();
        return res;
    }

    bool insert_and_print(const T & x) {
        auto res = rb_tree<T>::insert(x);
        cout << "insert " << x << endl;
        base::print_by_level(cout);
        cout << endl;
        check();
        return res;
    }
};

int main() {
    return 0;
}
