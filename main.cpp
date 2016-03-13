#include "rb_tree.hpp"

rb_tree<int> foo() {
    return rb_tree<int>();
}

int main() {
    rb_tree<int> x0;
    rb_tree<int> x1(x0);
    rb_tree<int> x2(foo());
    x0.insert(42);
    x0.insert(43);
    return 0;
}
