#ifndef RB_TREE_HPP
#define RB_TREE_HPP

#include <memory>
#include <iostream>
#include <iomanip>
#include <stack>

enum node_color { black, red };

template <typename T>
struct node_base {
    node_base * right;
    node_base * left;
    node_base * parent;
    node_color color;

    node_base()
            : right(this)
            , left(this)
            , parent(this)
            , color(black) { }

    node_base(node_base * right,
              node_base * left,
              node_base * parent,
              node_color color)
            : right(right)
            , left(left)
            , parent(parent)
            , color(color)  {
        if (right) right->parent = this;
        if (left) left->parent = this;
    }

    virtual ~node_base() { }

    virtual const T & get_value() const {
        throw std::logic_error("this node doesn't have a value");
    }

};

template <typename T>
class node_with_value : public node_base<T> {
    T value;

public:

    node_with_value(node_base<T> * right,
                    node_base<T> * left,
                    node_base<T> * parent,
                    node_color color,
                    const T & value)
            : node_base<T>(right, left, parent, color)
            , value(value) { }

    node_with_value(const T & value)
            : node_base<T>()
            , value(value) { }

    const T & get_value() const override {
        return value;
    }

};

template <typename T>
class const_rb_tree_iterator {
    const node_base<T> * node;
    const node_base<T> * nil;

public:

    const_rb_tree_iterator(const node_base<T> * node,
                           const node_base<T> * nil = nullptr)
            : node(node)
            , nil(nil) { }

    const_rb_tree_iterator & operator++() {
        if (node->right == nil) {
            const node_base<T> * parent;
            while ((parent = node->parent)->right == node) {
                node = parent;
            }
            node = parent;
        } else {
            node = node->right;
            while (node->left != nil) {
                node = node->left;
            }
        }
        return *this;
    }

    const_rb_tree_iterator operator++(int) {
        const_rb_tree_iterator it(*this);
        ++(*this);
        return it;
    }

    const_rb_tree_iterator & operator--() {
        if (node == nil) {
            node = node->right;
            return *this;
        }
        if (node->left == nil) {
            const node_base<T> * parent;
            while ((parent = node->parent)->left == node) {
                node = parent;
            }
            node = parent;
        } else {
            node = node->left;
            while (node->right != nil) {
                node = node->right;
            }
        }
        return *this;
    }

    const_rb_tree_iterator operator--(int) {
        const_rb_tree_iterator it(*this);
        --(*this);
        return it;
    }

    const node_base<T> & operator*() {
        return *node;
    }

    const node_base<T> * operator->() {
        return node;
    }

    friend bool operator==(const const_rb_tree_iterator & lhs,
                           const const_rb_tree_iterator & rhs) {
        return lhs.nil == rhs.nil && lhs.node == rhs.node;
    }

    friend bool operator!=(const const_rb_tree_iterator & lhs,
                           const const_rb_tree_iterator & rhs) {
        return lhs.nil != rhs.nil || lhs.node != rhs.node;
    }

};

template <typename T>
class rb_tree {
    node_base<T> * root;

    void free_nodes(const node_base<T> * x) {
        while (x != root->parent) {
            free_nodes(x->left);
            auto y = x->right;
            delete x;
            x = y;
        }
    }

    node_base<T> * copy(const node_base<T> * nil,
                        const node_base<T> * node) const {
        if (nil == node) return root->parent;
        node_base<T> * new_node = new node_with_value<T>(node->value);
        new_node->color = node->color;
        new_node->right = root->parent;
        new_node->left = root->parent;
        try {
            new_node->right = copy(nil, node->right);
            new_node->left = copy(nil, node->left);
        } catch (...) {
            free_nodes(new_node);
            throw;
        }
    }

    void left_rotate(node_base<T> * x) {
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
        x->parent = y;
    }

    void right_rotate(node_base<T> * x) {
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
        x->parent = y;
    }

    void fixup_insert(node_base<T> * z) {
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
        root->color = black;
    }

    bool insert(node_base<T> * z) {
        auto y = root->parent;
        auto x = root;
        while (root->parent != x) {
            y = x;
            if (z->get_value() < x->get_value()) {
                x = x->left;
            } else if (x->get_value() < z->get_value()) {
                x = x->right;
            } else {
                return false;
            }
        }
        z->parent = y;
        if (root->parent == y) {
            root = z;
        } else if (z->get_value() < y->get_value()) {
            y->left = z;
        } else {
            y->right = z;
        }
        z->left = root->parent;
        z->right = root->parent;
        z->color = red;
        fixup_insert(z);
        if (root == z) {
            z->parent->right = z;
            z->parent->left = z;
        } else {
            if (z->get_value() < root->parent->left->get_value()) {
                root->parent->left = z;
            } else if (root->parent->right->get_value()
                       < z->get_value()) {
                root->parent->right = z;
            }
        }
        return true;
    }

    void transplant(node_base<T> * u, node_base<T> * v) {
        if (u->parent == root->parent) {
            root = v;
        } else if (u == u->parent->left) {
            u->parent->left = v;
        } else {
            u->parent->right = v;
        }
        v->parent = u->parent;
    }

    void fixup_erase(node_base<T> * x) {
        while (x != root && x->color == black) {
            if (x == x->parent->left) {
                auto w = x->parent->right;
                if (w->color == red) {
                    w->color = black;
                    x->parent->color = red;
                    left_rotate(x->parent);
                    w = x->parent->right;
                }
                if (w->left->color == black &&
                    w->right->color == black) {
                    w->color = red;
                    x = x->parent;
                } else {
                    if (w->right->color == black) {
                        w->left->color = red;
                        right_rotate(w);
                        w = x->parent->right;
                    }
                    w->color = x->parent->color;
                    x->parent->color = black;
                    w->right->color = black;
                    left_rotate(x->parent);
                    x = root;
                }
            } else {
                auto w = x->parent->left;
                if (w->color == red) {
                    w->color = black;
                    x->parent->color = red;
                    right_rotate(x->parent);
                    w = x->parent->left;
                }
                if (w->right->color == black &&
                    w->left->color == black) {
                    w->color = red;
                    x = x->parent;
                } else {
                    if (w->left->color == black) {
                        w->right->color = red;
                        left_rotate(w);
                        w = x->parent->left;
                    }
                    w->color = x->parent->color;
                    x->parent->color = black;
                    w->left->color = black;
                    right_rotate(x->parent);
                    x = root;
                }
            }
        }
        x->color = black;
    }

    void erase(node_base<T> * z) {
        node_base<T> * y = z;
        node_color y_original_color = y->color;
        node_base<T> * x;
        if (z->left == root->parent) {
            x = z->right;
            transplant(z, z->right);
        } else if (z->right == root->parent) {
            x = z->left;
            transplant(z, z->left);
        } else {
            y = minimum(z->right);
            y_original_color = y->color;
            x = y->right;
            if (y->parent == z) {
                x->parent = y;
            } else {
                transplant(y, y->right);
                y->right = z->right;
                y->right->parent = y;
            }
            transplant(z, y);
            y->left = z->left;
            y->left->parent = y;
            y->color = z->color;
        }
        if (z == root->parent->left) {
            root->parent->left = z->parent;
        }
        if (z == root->parent->right) {
            root->parent->right = z->parent;
        }
        delete z;
        if (y_original_color == black) {
            fixup_erase(x);
        }
    }

public:

    rb_tree()
            : root(new node_base<T>()) { }

    rb_tree(const rb_tree & other)
            : root(new node_base<T>()) {
        auto nil = root;
        try {
            root = copy(other.root->parent, other.root);
        } catch (...) {
            delete root;
            throw;
        }
        root->parent = nil;
        auto leftmost = root;
        while (leftmost->left != nil) { leftmost = leftmost->left; }
        nil->left = leftmost;
        auto rightmost = root;
        while (rightmost->right != nil) { rightmost = rightmost->right; }
        nil->right = rightmost;
    }

    rb_tree(rb_tree && other)
            : root() {
        auto holder = std::unique_ptr<node_base<T>>(other.root);
        other.root = new node_base<T>();
        root = holder.release();
    }

    ~rb_tree() {
        auto nil = root->parent;
        free_nodes(root);
        delete nil;
    }

    const node_base<T> * minimum(const node_base<T> * x) const {
        while (x->left != root->parent) {
            x = x->left;
        }
        return x;
    }

    node_base<T> * minimum(node_base<T> * x) const {
        while (x->left != root->parent) {
            x = x->left;
        }
        return x;
    }

    const node_base<T> * insert(const T & value) {
        auto z = std::make_unique<node_with_value<T>>(value);
        if (insert(z.get())) {
            return z.release();
        } else {
            return nullptr;
        }
    }

    bool contains(const T & value) const {
        const node_base<T> * z = root;
        while (z != root->parent) {
            if (value < z->get_value()) {
                z = z->left;
            } else if (value > z->get_value()) {
                z = z->right;
            } else {
                return true;
            }
        }
        return false;
    }

    void erase(const T & value) {
        erase(find(value));
    }

    void erase(const node_base<T> * node) {
        erase(const_cast<node_base<T> *>(node));
    }

    const node_base<T> * find(const T & value) const {
        auto x = root;
        while (x != root->parent) {
            if (value < x->get_value()) {
                x = x->left;
            } else {
                x = x->right;
            }
        }
        return x == root->parent ? nullptr : x;
    }

    void print_by_level(std::ostream & os) const {
        if (!root) os << "<empty>" << std::endl;
        std::stack<std::pair<node_base<T> *, unsigned>> nodes;
        auto cur_node = root;
        auto depth = 1u;
        while (true) {
            if (root->parent != cur_node) {
                nodes.push({ cur_node, depth });
                cur_node = cur_node->right;
                depth += 6;
            } else if (!nodes.empty()) {
                cur_node = nodes.top().first;
                depth = nodes.top().second;
                nodes.pop();
                os << ">" << std::setw(depth) << ' '
                   << "\033[1;" << (cur_node->color == black ? "34" : "31")
                   << 'm' << cur_node->get_value()
                   << "\033[0m"
                   << std::endl;
                cur_node = cur_node->left;
                depth += 6;
            } else break;
        }
        os << std::endl;
    }

    const node_base<T> * get_root() const {
        return root;
    }

    const_rb_tree_iterator<T> begin() const {
        return const_rb_tree_iterator<T>(root->parent->left,
                                         root->parent);
    }

    const_rb_tree_iterator<T> end() const {
        return const_rb_tree_iterator<T>(root->parent,
                                         root->parent);
    }

};

#endif
