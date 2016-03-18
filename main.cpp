#include "rb_tree.hpp"
#include "tests.hpp"

#include <map>
#include <set>
#include <iterator>

void demo() {
    rb_tree<int> test_tree;
    std::map<int, const node_base<int> *> nodes_to_delete;
    for (int i = 0; i < 20; ++i) {
        auto value = rand() % 90 + 10;
        std::cout << "insert " << value << std::endl;
        auto node = test_tree.insert(value);
        test_tree.print_by_level(std::cout);
        check<int>(test_tree.get_root());
        nodes_to_delete.insert(std::make_pair(value, node));
    }
    for (int i = 0; i < 10; ++i) {
        auto it = nodes_to_delete.begin();
        std::advance(it, i);
        std::cout << "erase " << it->second->get_value() << std::endl;
        test_tree.erase(it->second);
        test_tree.print_by_level(std::cout);
        check<int>(test_tree.get_root());
        nodes_to_delete.erase(it->first);
    }

    // iterator demo
    auto nil = test_tree.get_root()->get_parent();
    auto first = test_tree.get_root();
    while (first->get_left() != nil) first = first->get_left();
    const_rb_tree_iterator<int> begin(first, nil);
    const_rb_tree_iterator<int> end(nil, nil);
    while (begin != end) {
        std::cout << begin->get_value() << " ";
        ++begin;
    }
    std::cout << std::endl;

    
}

int main() {
    test();
    demo();
    return 0;
}

