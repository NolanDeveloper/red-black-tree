#ifndef RB_TREE_HPP
#define RB_TREE_HPP

#include <memory>
#include <iostream>
#include <iomanip>
#include <stack>

template <typename T>
class rb_tree {
protected:
    enum node_color { red, black };

    struct node {
        T                     value;
        node *                parent;
        node_color            color;
        std::unique_ptr<node> left;
        std::unique_ptr<node> right;

        node(const T & x, node * parent) 
                : value(x)
                , parent(parent)
                , color(red)
                , left(nullptr)
                , right(nullptr) { }

        bool is_left() const { return parent->left.get() == this; }
        bool is_right() const { return parent->right.get() == this; }
        bool is_black() const { return black == color; }
        bool is_red() const { return red == color; }
    };

    std::unique_ptr<node> root;

private:
    void right_rotate(node * y) {
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

    void left_rotate(node * x) {
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

    void fixup_insert(node * z) {
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
                        left_rotate(z);
                    }
                    z->parent->color = black;
                    if (z->parent->parent) {
                        z->parent->parent->color = red;
                        right_rotate(z->parent->parent);
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
                        right_rotate(z);
                    }
                    z->parent->color = black;
                    if (z->parent->parent) {
                        z->parent->parent->color = red;
                        left_rotate(z->parent->parent);
                    }
                }
            }
        }
        root->color = black;
    }

public:
    rb_tree()
        : root(nullptr) { }

    bool insert(const T & z) {
        node * x = nullptr;
        node * y = root.get();
        while (y) {
            x = y;
            if (z < x->value)      y = x->left.get();
            else if (z > x->value) y = x->right.get();
            else                   return false;
        } 
        if (!x) {
            root = std::make_unique<node>(z, nullptr);
            root->color = black;
            return true;
        }
        if (z < x->value) {
            x->left = std::make_unique<node>(z, x);
            fixup_insert(x->left.get());
        } else /*if (z > x->val) */{
            x->right = std::make_unique<node>(z, x);
            fixup_insert(x->right.get());
        }
        return true;
    }

    void print_by_level(std::ostream & os) const {
        if (!root) os << "<empty>";
        std::stack<std::pair<node *, unsigned>> nodes;
        auto cur_node = root.get();
        auto depth = 1u;
        while (true) {
            if (cur_node) {
                nodes.push({ cur_node, depth });
                cur_node = cur_node->right.get();
                depth += 4;
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
                depth += 4;
            } else break;
        }
    }
};

#endif
