#include "tests.hpp"

#include <iostream>

template <typename T>
bool are_equal_nodes(const node_base<T> * lnil, const node_base<T> * lhs,
                     const node_base<T> * rnil, const node_base<T> * rhs) {
    if (lhs == lnil && rhs == rnil) return true;
    if (lhs == lnil || rhs == rnil) return false;
    return lhs->get_value() == rhs->get_value() &&
           lhs->color == rhs->color &&
           are_equal_nodes(lnil, lhs->left, rnil, rhs->left) &&
           are_equal_nodes(lnil, lhs->right, rnil, rhs->right);
}

template <typename T>
bool are_equal_trees(const node_base<T> * lhs, const node_base<T> * rhs) {
    return are_equal_nodes(lhs->parent, lhs, rhs->parent, rhs);
}

template <typename T>
void print_by_level(std::ostream & os, const node_base<T> * root) {
    if (!root) os << "<empty>" << std::endl;
    std::stack<std::pair<const node_base<T> *, unsigned>> nodes;
    auto cur_node = root;
    auto depth = 1u;
    while (true) {
        if (root->parent != cur_node) {
            nodes.push(std::make_pair(cur_node, depth));
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

template <typename T>
void assert_equal(const node_base<T> * expected,
                  const node_base<T> * actual) {
    if (are_equal_trees(expected, actual)) return;
    std::cout << "expected:" << std::endl;
    print_by_level(std::cout, expected);
    std::cout << "but actual: " << std::endl;
    print_by_level(std::cout, actual);
    throw;
}

template <typename T>
void free_nodes(const node_base<T> * node, const node_base<T> * nil = nullptr) {
    while (node != nil) {
        free_nodes(node->right, nil);
        auto left = node->left;
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
        free_nodes(right);
        free_nodes(left);
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
    if (node->right) {
        replace_nullptr_with_nil(nil, node->right);
    } else {
        node->right = nil;
    }
    if (node->left) {
        replace_nullptr_with_nil(nil, node->left);
    } else {
        node->left = nil;
    }
}

template <typename T>
struct root_holder {
    const node_base<T> * const root;

    root_holder(const node_base<T> * root)
            : root(root) { }

    root_holder(const node_base<T> & root_holder) = delete;

    ~root_holder() {
        free_nodes(root, root->parent);
    }
};

template <typename T>
root_holder<T> as_tree(node_base<T> * root) {
    node_base<T> * nil;
    try {
        nil = new node_base<T>();
    } catch (...) {
        free_nodes(root);
        throw;
    }
    root->parent = nil;
    replace_nullptr_with_nil(nil, root);
    return root_holder<T>(root);
}

void insert_1_seq() {
    auto expected =
        as_tree(make_node(1, black));
    rb_tree<int> actual;
    actual.insert(1);
    assert_equal(expected.root, actual.get_root());
}

void insert_2_seq() {
    auto expected =
        as_tree(
            make_node(
                make_node(2, red),
                1, black));
    rb_tree<int> actual;
    actual.insert(1);
    actual.insert(2);
    assert_equal(expected.root, actual.get_root());
}

void insert_3_seq() {
    auto expected =
        as_tree(
            make_node(
                make_node(3, red),
                2, black,
                make_node(1, red)));
    rb_tree<int> actual;
    actual.insert(1);
    actual.insert(2);
    actual.insert(3);
    assert_equal(expected.root, actual.get_root());
}

void insert_4_seq() {
    auto expected =
        as_tree(
            make_node(
                make_node(
                    make_node(4, red),
                    3, black),
                2, black,
                make_node(1, black)));
    rb_tree<int> actual;
    for (int i = 1; i <= 4; ++i) actual.insert(i);
    assert_equal(expected.root, actual.get_root());
}

void insert_5_seq() {
    auto expected =
        as_tree(
            make_node(
                make_node(
                    make_node(5, red),
                    4, black,
                    make_node(3, red)),
                2, black,
                make_node(1, black)));
    rb_tree<int> actual;
    for (int i = 1; i <= 5; ++i) actual.insert(i);
    assert_equal(expected.root, actual.get_root());
}

void insert_6_seq() {
    auto expected =
        as_tree(
            make_node(
                make_node(
                    make_node(
                        make_node(6, red),
                        5, black),
                    4, red,
                    make_node(3, black)),
                2, black,
                make_node(1, black)));
    rb_tree<int> actual;
    for (int i = 1; i <= 6; ++i) actual.insert(i);
    assert_equal(expected.root, actual.get_root());
}

void insert_7_seq() {
    auto expected =
        as_tree(
            make_node(
                make_node(
                    make_node(
                        make_node(7, red),
                        6, black,
                        make_node(5, red)),
                    4, red,
                    make_node(3, black)),
                2, black,
                make_node(1, black)));
    rb_tree<int> actual;
    for (int i = 1; i <= 7; ++i) actual.insert(i);
    assert_equal(expected.root, actual.get_root());
}

void insert_8_seq() {
    auto expected =
        as_tree(
            make_node(
                make_node(
                    make_node(
                        make_node(8, red),
                        7, black),
                    6, red,
                    make_node(5, black)),
                4, black,
                make_node(
                    make_node(3, black),
                    2, red,
                    make_node(1, black))));
    rb_tree<int> actual;
    for (int i = 1; i <= 8; ++i) actual.insert(i);
    assert_equal(expected.root, actual.get_root());
}

void insert_9_seq() {
    auto expected =
        as_tree(
            make_node(
                make_node(
                    make_node(
                        make_node(9, red),
                        8, black,
                        make_node(7, red)),
                    6, red,
                    make_node(5, black)),
                4, black,
                make_node(
                    make_node(3, black),
                    2, red,
                    make_node(1, black))));
    rb_tree<int> actual;
    for (int i = 1; i <= 9; ++i) actual.insert(i);
    assert_equal(expected.root, actual.get_root());
}

void insert_10_seq() {
    auto expected =
        as_tree(
            make_node(
                make_node(
                    make_node(
                        make_node(
                            make_node(10, red),
                            9, black),
                        8, red,
                        make_node(7, black)),
                    6, black,
                    make_node(5, black)),
                4, black,
                make_node(
                    make_node(3, black),
                    2, black,
                    make_node(1, black))));
    rb_tree<int> actual;
    for (int i = 1; i <= 10; ++i) actual.insert(i);
    assert_equal(expected.root, actual.get_root());
}

void insert_11_seq() {
    auto expected =
        as_tree(
            make_node(
                make_node(
                    make_node(
                        make_node(
                            make_node(11, red),
                            10, black,
                            make_node(9, red)),
                        8, red,
                        make_node(7, black)),
                    6, black,
                    make_node(5, black)),
                4, black,
                make_node(
                    make_node(3, black),
                    2, black,
                    make_node(1, black))));
    rb_tree<int> actual;
    for (int i = 1; i <= 11; ++i) actual.insert(i);
    assert_equal(expected.root, actual.get_root());
}

void insert_12_seq() {
    auto expected =
        as_tree(
            make_node(
                make_node(
                    make_node(
                        make_node(
                            make_node(12, red),
                            11, black),
                        10, red,
                        make_node(9, black)),
                    8, black,
                    make_node(
                        make_node(7, black),
                        6, red,
                        make_node(5, black))),
                4, black,
                make_node(
                    make_node(3, black),
                    2, black,
                    make_node(1, black))));
    rb_tree<int> actual;
    for (int i = 1; i <= 12; ++i) actual.insert(i);
    assert_equal(expected.root, actual.get_root());
}

void insert_13_seq() {
    auto expected =
        as_tree(
            make_node(
                make_node(
                    make_node(
                        make_node(
                            make_node(13, red),
                            12, black,
                            make_node(11, red)),
                        10, red,
                        make_node(9, black)),
                    8, black,
                    make_node(
                        make_node(7, black),
                        6, red,
                        make_node(5, black))),
                4, black,
                make_node(
                    make_node(3, black),
                    2, black,
                    make_node(1, black))));
    rb_tree<int> actual;
    for (int i = 1; i <= 13; ++i) actual.insert(i);
    assert_equal(expected.root, actual.get_root());
}

void test() {
    insert_1_seq();
    insert_2_seq();
    insert_3_seq();
    insert_4_seq();
    insert_5_seq();
    insert_6_seq();
    insert_7_seq();
    insert_8_seq();
    insert_9_seq();
    insert_10_seq();
    insert_11_seq();
    insert_12_seq();
    insert_13_seq();
}
