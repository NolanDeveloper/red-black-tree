#ifndef RB_TREE_HPP
#define RB_TREE_HPP

#include <memory>
#include <iostream>
#include <iomanip>
#include <stack>

namespace rb_tree {

enum node_color { red, black };

template <typename T>
struct node {
    T                     value;
    node *                parent;
    node_color            color;
    std::unique_ptr<node> left;
    std::unique_ptr<node> right;

    node(const T & x, node * parent, node_color color = red) 
            : value(x)
            , parent(parent)
            , color(color)
            , left(nullptr)
            , right(nullptr) { }

    node(std::unique_ptr<node> && right,
         const T & x, 
         node * parent, 
         node_color color,
         std::unique_ptr<node> && left)
            : value(x)
            , parent(parent)
            , color(color)
            , left(std::move(left))
            , right(std::move(right)) { }

    node(std::unique_ptr<node> && right,
         const T & x, 
         node * parent, 
         node_color color)
            : value(x)
            , parent(parent)
            , color(color)
            , left(nullptr)
            , right(std::move(right)) { }

    node(const T & x, 
         node * parent, 
         node_color color,
         std::unique_ptr<node> && left)
            : value(x)
            , parent(parent)
            , color(color)
            , left(std::move(left))
            , right(nullptr) { }

    bool is_left() const { return parent->left.get() == this; }
    bool is_right() const { return parent->right.get() == this; }
    bool is_black() const { return black == color; }
    bool is_red() const { return red == color; }
};

template <typename T>
bool are_equal(const node<T> * lhs, const node<T> * rhs) {
    if (!lhs && !rhs) return true;
    if (!lhs || !rhs) return false;
    return lhs->value == rhs->value && 
           lhs->color == rhs->color &&
           are_equal(lhs->left.get(), rhs->left.get()) && 
           are_equal(lhs->right.get(), rhs->right.get());
}

template <typename T>
void right_rotate(std::unique_ptr<node<T>> & root, node<T> * y) {
    if (root.get() == y) {
        auto x = std::move(y->left);
        x->parent = nullptr;
        y->parent = x.get();
        y->left = std::move(x->right);
        if (y->left) y->left->parent = y;
        x->right = std::move(root);
        root = std::move(x);
    } else {
        auto x = std::move(y->left);
        x->parent = y->parent;
        y->left = std::move(x->right);
        if (y->left) y->left->parent = y;
        if (y->is_left()) {
            x->right = std::move(y->parent->left);
            y->parent->left = std::move(x); 
            y->parent = y->parent->left.get();
        } else {
            x->right = std::move(y->parent->right);
            y->parent->right = std::move(x);
            y->parent = y->parent->right.get();
        }
    }
}

template <typename T>
void left_rotate(std::unique_ptr<node<T>> & root, node<T> * x) {
    if (root.get() == x) {
        auto y = std::move(x->right);
        y->parent = nullptr;
        x->parent = y.get();
        x->right = std::move(y->left);
        if (x->right) x->right->parent = x;
        y->left = std::move(root);
        root = std::move(y);
    } else {
        auto y = std::move(x->right);
        y->parent = x->parent;
        x->right = std::move(y->left);
        if (x->right) x->right->parent = x;
        if (x->is_left()) {
            y->left = std::move(x->parent->left);
            x->parent->left = std::move(y);
            x->parent = x->parent->left.get();
        } else {
            y->left = std::move(x->parent->right);
            x->parent->right = std::move(y);
            x->parent = x->parent->right.get();
        }
    }
}

template <typename T>
void fixup_insert(std::unique_ptr<node<T>> & root, node<T> * z) {
    while (z->parent && z->parent->is_red()) {
        if (z->parent->is_left()) {
            auto y = z->parent->parent->right.get();               
            if (y && y->is_red()) {
                z->parent->color = black;
                z->parent->parent->color = red;
                y->color = black;
                z = z->parent->parent;
            } else {
                if(z->is_right()) {
                    z = z->parent;
                    left_rotate(root, z);
                }
                z->parent->color = black;
                if (z->parent->parent) {
                    z->parent->parent->color = red;
                    right_rotate(root, z->parent->parent);
                }
            }
        } else {
            auto y = z->parent->parent->left.get();               
            if (y && y->is_red()) {
                z->parent->color = black;
                z->parent->parent->color = red;
                y->color = black;
                z = z->parent->parent;
            } else {
                if(z->is_left()) {
                    z = z->parent;
                    right_rotate(root, z);
                }
                z->parent->color = black;
                if (z->parent->parent) {
                    z->parent->parent->color = red;
                    left_rotate(root, z->parent->parent);
                }
            }
        }
    }
    root->color = black;
}

template <typename T>
bool insert(std::unique_ptr<node<T>> & root, const T & z) {
    node<T> * x = nullptr;
    node<T> * y = root.get();
    while (y) {
        x = y;
        if (z < x->value)      y = x->left.get();
        else if (z > x->value) y = x->right.get();
        else                   return false;
    } 
    if (!x) {
        root = std::make_unique<node<T>>(z, nullptr);
        root->color = black;
        return true;
    }
    if (z < x->value) {
        x->left = std::make_unique<node<T>>(z, x);
        fixup_insert(root, x->left.get());
    } else /*if (z > x->val) */{
        x->right = std::make_unique<node<T>>(z, x);
        fixup_insert(root, x->right.get());
    }
    return true;
}

template <typename T>
void transplant(std::unique_ptr<node<T>> & root,
                node<T> * u,
                node<T> * v) {
    if (!v) {
        if (!u->parent) {
            root.reset();
        } else {
            u->get_ref().reset();
        }
    }
    if (!u->parent) {
        root = std::move(v->get_ref());
    } else {
        u->get_ref() = std::move(v->get_ref());
    }
    v->parent = u->parent;
}

template <typename T>
node<T> * minimum(node<T> * z) {
    while (z->left) z = z->left.get();
    return z;
}

template <typename T>
void fixup_erase(std::unique_ptr<node<T>> & root, node<T> * x) {
    while (x != root.get() && x->is_black()) {
        if (x->is_left()) {

        } else {
        }
    }
}

template <typename T>
void erase(std::unique_ptr<node<T>> & root, node<T> * z) {
    auto y_original_color = z->color;
    node<T> * x;
    if (!z->left) {
        x = z->right.get();
        transplant(root, z, z->right.get());
    } else if (!z->right) {
        x = z->left.get();
        transplant(root, z, z->left.get());
    } else {
        auto y = minimum(z->right.get());
        y_original_color = y->color;
        x = y->right.get();
        if (y->parent == z) {
            y->left = std::move(z->left);
            y->parent = z->parent;
            y->color = z->color;
            z->get_ref() = std::move(y->get_ref());
        } else {
            auto t = std::move(y->get_ref());
            t->right->parent = t->parent;
            t->parent->left = std::move(t->right);
            z->left->parent = t.get();
            t->left = std::move(z->left);
            z->right->parent = t.get();
            t->right = std::move(z->right);
            t->color = z->color;
            t->paret = z->parent;
            z->get_ref() = std::move(t);
        }
    }
    if (black == y_original_color) {
        fixup_erase(root, x);
    }
}

template <typename T>
void print_by_level(node<T> * root, std::ostream & os) {
    if (!root) os << "<empty>";
    std::stack<std::pair<node<T> *, unsigned>> nodes;
    auto cur_node = root;
    auto depth = 1u;
    while (true) {
        if (cur_node) {
            nodes.push({ cur_node, depth });
            cur_node = cur_node->right.get();
            depth += 6;
        } else if (!nodes.empty()) {
            cur_node = nodes.top().first;
            depth = nodes.top().second;
            nodes.pop();
            os << ">" << std::setw(depth) << ' ' 
               << "\033[1;" << (cur_node->is_black() ? "34" : "31") << 'm'
               << cur_node->value 
               << "\033[0m"
               << std::endl;
            cur_node = cur_node->left.get();
            depth += 6;
        } else break;
    }
}

};

#endif
