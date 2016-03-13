#ifndef RB_TREE_HPP
#define RB_TREE_HPP

#include <memory>

enum node_color { black, red };

struct node_base {
    node_base * right;
    node_base * left;
    node_base * parent;
    node_color color;
    
    node_base(node_base * right, 
              node_base * left,
              node_base * parent,
              node_color color)
            : right(right)
            , left(left)
            , parent(parent)
            , color(color)  { 
        right->parent = this;
        left->parent = this;
    }

    node_base(node_base * parent,
              node_color color)
            : right(nullptr)
            , left(nullptr)
            , parent(parent)
            , color(color) { }
};

template <typename T>
struct node_with_value : node_base {
    T value;

    node_with_value(node_base * right, 
                    node_base * left,
                    node_base * parent,
                    node_color color,
                    const T & value)
            : node_base(right, left, parent, color)
            , value(value) { }
    
    node_with_value(node_base * parent,
                    node_color color,
                    const T & value)
            : node_base(parent, color)
            , value(value) { }
};

template <typename T>
struct rb_tree {
    node_base * root;

    static node_with_value<T> * right(node_base * node) {
        return static_cast<node_with_value<T> *>(node->right);
    }

    static node_with_value<T> * left(node_base * node) {
        return static_cast<node_with_value<T> *>(node->left);
    }

    static const T & value_of(node_base * node) {
        return static_cast<node_with_value<T> *>(node)->value;
    }

    void free_node(node_with_value<T> * x) {
        while (x != root->parent) {
            free_node(left(x));
            auto y = right(x);
            delete x;
            x = y;
        }
    }

    node_with_value<T> * make_node(
            const T & value, node_color color) {
        return new node_with_value<T>(
            root->parent, root->parent, nullptr, color, value);
    }

    std::unique_ptr<node_base> copy(const node_base * nil, 
                                    const node_base * node) {
        if (nil == node) return std::unique_ptr<node_base>(root->parent);
        std::unique_ptr<node_base> left_copy(copy(nil, node->left));
        std::unique_ptr<node_base> right_copy(copy(nil, node->right));
        std::unique_ptr<node_base> result(new node_with_value<T>(
            right_copy.get(), left_copy.get(), nullptr, node->color,
            static_cast<const node_with_value<T> *>(node)->value));
        left_copy.release();
        right_copy.release();
        return result;
    }

    void left_rotate(node_base * x) {
        auto y = x->right;
        x->right = y->left;
        if (y->left != root->parent) {
            y->left->parent = x;
        }
        y->parent = x->parent;
        if (x->parent == root->parent) {
            root = y;
        } else if (x == x->parent->left) {
            x->parent->left = y;
        } else {
            x->parent->right = y;
        }
        y->left = x;
    }

    void right_rotate(node_base * x) {
        auto y = x->left;
        x->left = y->right;
        if (y->right != root->parent) {
            y->right->parent = x;
        }
        y->parent = x->parent;
        if (x->parent == root->parent) {
            root = y;
        } else if (x == x->parent->right) {
            x->parent->right = y;
        } else {
            x->parent->left = y;
        }
        y->right = x;
    }

    void fixup_insert(node_base * z) {
        while (z->parent->color == red) {
            if (z->parent == z->parent->parent->left) {
                auto y = z->parent->parent->right;
                if (y->color == red) {
                    z->parent->color = black;
                    y->color = black;
                    z->parent->parent->color = red;
                    z = z->parent->parent;
                } else {
                    if (z == z->parent->right) {
                        z = z->parent;
                        left_rotate(z);
                    }
                    z->parent->color = black;
                    z->parent->parent->color = red;
                    right_rotate(z->parent->parent);
                }
            } else {
                auto y = z->parent->parent->left;
                if (y->color == red) {
                    z->parent->color = black;
                    y->color = black;
                    z->parent->parent->color = red;
                    z = z->parent->parent;
                } else {
                    if (z == z->parent->left) {
                        z = z->parent;
                        right_rotate(z);
                    }
                    z->parent->color = black;
                    z->parent->parent->color = red;
                    left_rotate(z->parent->parent);
                }
            }
        }
    }

    void insert(node_with_value<T> * z) {
        auto y = root->parent;
        auto x = root;
        while (root->parent != x) {
            y = x;
            if (value_of(z) < value_of(x)) {
                x = x->left;
            } else {
                x = x->right;
            }
        }
        z->parent = y;
        if (root->parent == y) {
            root = z;
        } else if (value_of(z) < value_of(y)) {
            y->left = z;
        } else {
            y->right = z;
        }
        z->left = root->parent;
        z->right = root->parent;
        z->color = red;
        fixup_insert(z);
    }

public:
    rb_tree()
            : root(new node_base(nullptr, black)) {
        root->parent = root;
        root->left = root;
        root->right = root;
    }

    rb_tree(const rb_tree & other) 
            : root(new node_base(nullptr, black)) {
        root->parent = root;
        root->left = root;
        root->right = root;
        auto nil = root;
        root = copy(other.root->parent, other.root).release();
        root->parent = nil;
    }

    rb_tree(rb_tree && other)
            : root(other.root) {
        other.root = new node_base(nullptr, black);
        other.root->parent = other.root;
        other.root->left = other.root;
        other.root->right = other.root;
    }

    void insert(const T & value) {
        std::unique_ptr<node_with_value<T>> z(
                new node_with_value<T>(nullptr, black, value));
        insert(z.get());
        z.release();
    }

    ~rb_tree() {
        if (root->parent == root) {
            delete root;
        } else {
            auto nil = root->parent;
            free_node(static_cast<node_with_value<T> *>(root));
            delete nil;
        }
    }


};

#endif
