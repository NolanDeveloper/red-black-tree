#include "rb_tree.hpp"
#include "tests.hpp"

#include <map>
#include <set>
#include <iterator>

rb_tree<int> foo() {
    return rb_tree<int>();
}

//const node_with_value<int> * insert_print_check(rb_tree<int> & tree, int value) {
//    std::cout << "insert " << value << std::endl;
//    auto res = tree.insert(value);
//    tree.print_by_level(std::cout);
//    check<int>(tree.root);
//    return res;
//}
//
//void erase_print_check(rb_tree<int> & tree, const node_with_value<int> * node) {
//    std::cout << "erase " << value_of<int>(node) << std::endl;
//    tree.erase(node);
//    tree.print_by_level(std::cout);
//    check<int>(tree.root);
//}
//
//void make_show() {
//    rb_tree<int> x0;
//    rb_tree<int> x1(x0);
//    rb_tree<int> x2(foo());
//    auto it = insert_print_check(x0, 10);
//    insert_print_check(x0, 20);
//    insert_print_check(x0, 0);
//    insert_print_check(x0, 30);
//    insert_print_check(x0, 15);
//    insert_print_check(x0, -10);
//    insert_print_check(x0, 40);
//    insert_print_check(x0, 50);
//    std::cout << "remove " << it->value << std::endl;
//    x0.erase(it);
//    x0.print_by_level(std::cout);
//    std::cout << "contains 20: " << x0.contains(20) << std::endl;
//    std::cout << "contains 30: " << x0.contains(30) << std::endl;
//    std::cout << "contains -10: " << x0.contains(-10) << std::endl;
//    std::cout << "contains 10: " << x0.contains(10) << std::endl;
//    std::cout << "contains 42: " << x0.contains(42) << std::endl;
//}
//
//struct my_int {
//    static int alive;
//
//    int val;
//
//    my_int()
//            : val() { 
//        ++alive;
//    }
//
//    my_int(int val)
//            : val(val) { 
//        ++alive;
//    }
//    
//    my_int(const my_int & i)
//            : val(i.val) { 
//        ++alive;
//    }
//
//    ~my_int() {
//        --alive;
//    }
//
//    friend bool operator<(const my_int & lhs, const my_int & rhs) {
//        return lhs.val < rhs.val;
//    }
//
//    friend std::ostream & operator<<(std::ostream & os, const my_int & i) {
//        os << i.val;
//        return os;
//    }
//
//};
//
//int my_int::alive = 0;
//
//void do_random(rb_tree<my_int> & tree, 
//               std::set<const node_with_value<my_int> *> & nodes) {
//    if (rand() % 2) {
//        auto node = tree.insert(rand() % 9000 + 1000);
//        if (node) nodes.insert(node);
//    } else {
//        auto it = nodes.begin();
//        for (unsigned i = 0; i < nodes.size(); ++i) {
//            ++it;
//        }
//        tree.erase(*it);
//        nodes.erase(it);
//    }
//}

int main() {
    //make_show();
    //test();

    rb_tree<int> test_tree;
    std::map<int, const node_base<int> *> nodes_to_delete;
    for (int i = 0; i < 100; ++i) {
        auto value = rand() % 9000 + 1000;
        std::cout << "insert " << value << std::endl;
        auto node = test_tree.insert(value);
        test_tree.print_by_level(std::cout);
        check<int>(test_tree.get_root());
        nodes_to_delete.insert(std::make_pair(value, node));
    }
    for (int i = 0; i < 50; ++i) {
        auto it = nodes_to_delete.begin();
        std::advance(it, i);
        std::cout << "erase " << it->second->get_value() << std::endl;
        test_tree.erase(it->second);
        test_tree.print_by_level(std::cout);
        check<int>(test_tree.get_root());
        nodes_to_delete.erase(it->first);
    }
    return 0;
}

