#ifndef RB_TREE_HPP
#define RB_TREE_HPP

#include <memory>
#include <iostream>
#include <iomanip>
#include <stack>

enum node_color { black, red };

template <typename T>
class node_base {
    node_base * right;
    node_base * left;
    node_base * parent;
    node_color color;

public:

    node_base()
            : right(nullptr)
            , left(nullptr)
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

    void set_right(node_base * right) {
        this->right = right;
    }

    node_base * get_right() {
        return right; 
    }

    const node_base * get_right() const {
        return right; 
    }

    void set_left(node_base * left) {
        this->left = left;
    }

    node_base * get_left() {
        return left; 
    }

    const node_base * get_left() const {
        return left; 
    }

    void set_parent(node_base * parent) {
        this->parent = parent;
    }

    node_base * get_parent() {
        return parent; 
    }

    const node_base * get_parent() const {
        return parent; 
    }

    void set_color(node_color color) {
        this->color = color;
    }

    node_color get_color() const {
        return color;
    }

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
class rb_tree {
    node_base<T> * root;

    void free_nodes(const node_base<T> * x) {
        while (x != root->get_parent()) {
            free_nodes(x->get_left());
            auto y = x->get_right();
            delete x;
            x = y;
        }
    }

    std::unique_ptr<node_base<T>> copy(const node_base<T> * nil, 
                                       const node_base<T> * node) const {
        if (nil == node) {
            return std::unique_ptr<node_base<T>>(root->get_parent());
        }
        auto left_copy(copy(nil, node->get_left()));
        auto right_copy(copy(nil, node->get_right()));
        auto result = std::make_unique<node_with_value>(
            right_copy.get(), left_copy.get(), nullptr, 
            node->get_color(), node->get_value());
        left_copy.release();
        right_copy.release();
        return result;
    }

    void left_rotate(node_base<T> * x) {
        auto y = x->get_right();
        x->set_right(y->get_left());
        if (y->get_left() != root->get_parent()) {
            y->get_left()->set_parent(x);
        }
        y->set_parent(x->get_parent());
        if (x->get_parent() == root->get_parent()) {
            root = y;
        } else if (x == x->get_parent()->get_left()) {
            x->get_parent()->set_left(y);
        } else {
            x->get_parent()->set_right(y);
        }
        y->set_left(x);
        x->set_parent(y);
    }

    void right_rotate(node_base<T> * x) {
        auto y = x->get_left();
        x->set_left(y->get_right());
        if (y->get_right() != root->get_parent()) {
            y->get_right()->set_parent(x);
        }
        y->set_parent(x->get_parent());
        if (x->get_parent() == root->get_parent()) {
            root = y;
        } else if (x == x->get_parent()->get_right()) {
            x->get_parent()->set_right(y);
        } else {
            x->get_parent()->set_left(y);
        }
        y->set_right(x);
        x->set_parent(y);
    }

    void fixup_insert(node_base<T> * z) {
        while (z->get_parent()->get_color() == red) {
            if (z->get_parent() == z->get_parent()->get_parent()->get_left()) {
                auto y = z->get_parent()->get_parent()->get_right();
                if (y->get_color() == red) {
                    z->get_parent()->set_color(black);
                    y->set_color(black);
                    z->get_parent()->get_parent()->set_color(red);
                    z = z->get_parent()->get_parent();
                } else {
                    if (z == z->get_parent()->get_right()) {
                        z = z->get_parent();
                        left_rotate(z);
                    }
                    z->get_parent()->set_color(black);
                    z->get_parent()->get_parent()->set_color(red);
                    right_rotate(z->get_parent()->get_parent());
                }
            } else {
                auto y = z->get_parent()->get_parent()->get_left();
                if (y->get_color() == red) {
                    z->get_parent()->set_color(black);
                    y->set_color(black);
                    z->get_parent()->get_parent()->set_color(red);
                    z = z->get_parent()->get_parent();
                } else {
                    if (z == z->get_parent()->get_left()) {
                        z = z->get_parent();
                        right_rotate(z);
                    }
                    z->get_parent()->set_color(black);
                    z->get_parent()->get_parent()->set_color(red);
                    left_rotate(z->get_parent()->get_parent());
                }
            }
        }
        root->set_color(black);
    }

    bool insert(node_base<T> * z) {
        auto y = root->get_parent();
        auto x = root;
        while (root->get_parent() != x) {
            y = x;
            if (z->get_value() < x->get_value()) {
                x = x->get_left();
            } else if (x->get_value() < z->get_value()) {
                x = x->get_right();
            } else {
                return false;
            }
        }
        z->set_parent(y);
        if (root->get_parent() == y) {
            root = z;
        } else if (z->get_value() < y->get_value()) {
            y->set_left(z);
        } else {
            y->set_right(z);
        }
        z->set_left(root->get_parent());
        z->set_right(root->get_parent());
        z->set_color(red);
        fixup_insert(z);
        return true;
    }

    void transplant(node_base<T> * u, node_base<T> * v) {
        if (u->get_parent() == root->get_parent()) {
            root = v;
        } else if (u == u->get_parent()->get_left()) {
            u->get_parent()->set_left(v);
        } else {
            u->get_parent()->set_right(v);
        }
        v->set_parent(u->get_parent());
    }

    const node_base<T> * minimum(const node_base<T> * x) const {
        while (x->get_left() != root->get_parent()) {
            x = x->get_left();
        }
        return x;
    }

    node_base<T> * minimum(node_base<T> * x) const {
        while (x->get_left() != root->get_parent()) {
            x = x->get_left();
        }
        return x;
    }

    void fixup_erase(node_base<T> * x) {
        while (x != root && x->get_color() == black) {
            if (x == x->get_parent()->get_left()) {
                auto w = x->get_parent()->get_right();
                if (w->get_color() == red) {
                    w->set_color(black);
                    x->get_parent()->set_color(red);
                    left_rotate(x->get_parent());
                    w = x->get_parent()->get_right();
                }
                if (w->get_left()->get_color() == black && 
                    w->get_right()->get_color() == black) {
                    w->set_color(red);
                    x = x->get_parent();
                } else {
                    if (w->get_right()->get_color() == black) {
                        w->get_left()->set_color(red);
                        right_rotate(w);
                        w = x->get_parent()->get_right();
                    }
                    w->set_color(x->get_parent()->get_color());
                    x->get_parent()->set_color(black);
                    w->get_right()->set_color(black);
                    left_rotate(x->get_parent());
                    x = root;
                }
            } else {
                auto w = x->get_parent()->get_left();
                if (w->get_color() == red) {
                    w->set_color(black);
                    x->get_parent()->set_color(red);
                    right_rotate(x->get_parent());
                    w = x->get_parent()->get_left();
                }
                if (w->get_right()->get_color() == black && 
                    w->get_left()->get_color() == black) {
                    w->set_color(red);
                    x = x->get_parent();
                } else {
                    if (w->get_left()->get_color() == black) {
                        w->get_right()->set_color(red);
                        left_rotate(w);
                        w = x->get_parent()->get_left();
                    }
                    w->set_color(x->get_parent()->get_color());
                    x->get_parent()->set_color(black);
                    w->get_left()->set_color(black);
                    right_rotate(x->get_parent());
                    x = root;
                }
            }
        }
        x->set_color(black);
    }

    void erase(node_base<T> * z) {
        node_base<T> * y = z;
        node_color y_original_color = y->get_color();
        node_base<T> * x;
        if (z->get_left() == root->get_parent()) {
            x = z->get_right();
            transplant(z, z->get_right());
        } else if (z->get_right() == root->get_parent()) {
            x = z->get_left();
            transplant(z, z->get_left());
        } else {
            y = minimum(z->get_right());
            y_original_color = y->get_color();
            x = y->get_right();
            if (y->get_parent() == z) {
                x->set_parent(y);
            } else {
                transplant(y, y->get_right());
                y->set_right(z->get_right());
                y->get_right()->set_parent(y);
            }
            transplant(z, y);
            y->set_left(z->get_left());
            y->get_left()->set_parent(y);
            y->set_color(z->get_color());
        }
        delete z;
        if (y_original_color == black) {
            fixup_erase(x);
        }
    }

public:

    rb_tree()
            : root(new node_base<T>()) {
    }

    rb_tree(const rb_tree & other) 
            : root(new node_base<T>()) {
        auto nil = root;
        root = copy(other.root->get_parent(), other.root).release();
        root->set_parent(nil);
    }

    rb_tree(rb_tree && other)
            : root() {
        auto holder = std::unique_ptr<node_base<T>>(other.root);
        other.root = new node_base<T>();
        root = holder.release();
    }

    ~rb_tree() {
        auto nil = root->get_parent();
        free_nodes(root);
        delete nil;
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
        while (z != root->get_parent()) {
            if (value < z->get_value()) {
                z = z->get_left();
            } else if (value > z->get_value()) {
                z = z->get_right();
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
        while (x != root->get_parent()) {
            if (value < x->get_value()) {
                x = x->get_left();
            } else {
                x = x->get_right();
            }
        }
        return x == root->get_parent() ? nullptr : x;
    }

    void print_by_level(std::ostream & os) const {
        if (!root) os << "<empty>" << std::endl;
        std::stack<std::pair<node_base<T> *, unsigned>> nodes;
        auto cur_node = root;
        auto depth = 1u;
        while (true) {
            if (root->get_parent() != cur_node) {
                nodes.push({ cur_node, depth });
                cur_node = cur_node->get_right();
                depth += 6;
            } else if (!nodes.empty()) {
                cur_node = nodes.top().first;
                depth = nodes.top().second;
                nodes.pop();
                os << ">" << std::setw(depth) << ' ' 
                   << "\033[1;" << (cur_node->get_color() == black ? "34" : "31") 
                   << 'm' << cur_node->get_value()
                   << "\033[0m"
                   << std::endl;
                cur_node = cur_node->get_left();
                depth += 6;
            } else break;
        }
        os << std::endl;
    }

    const node_base<T> * get_root() const {
        return root;
    }

};

#endif
