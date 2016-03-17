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
}

int main() {
    test();
    //demo();
    return 0;
}

