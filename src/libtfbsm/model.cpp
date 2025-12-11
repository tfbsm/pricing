#include "libtfbsm/model.hpp"

#include <concepts>
#include <iostream>
#include <vector>

// constexpr int add(int a, int b) { return a + b; }

// static void test(std::invocable<int> auto func, int arg) {
//     func(arg);
// }

void tfbsm::Model::do_something() {
    // static_assert(add(1, 2) == 3);

    std::vector<int> v{1, 2, 3};

    // std::cout << "do something" << std::endl;
    // test([v = std::move(v)](int i) { std::cout << i << " " << v.size(); }, test_field);
}