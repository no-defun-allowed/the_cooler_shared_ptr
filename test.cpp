#include "crc.h"
#include <iostream>
#include <optional>
#include <vector>

using namespace crc;

void scalar_test() {
  cooler_shared_ptr<int> x = 42;
  std::cout << x.get() << std::endl;
  cooler_shared_ptr<int> y = 123;
  x = y;
  std::cout << x.get() << std::endl;
  // Old value of x should be freed here.
  collect();
  std::cout << x.get() << std::endl;
  x = cooler_shared_ptr<int>(1337);
  y = cooler_shared_ptr<int>(420);
}

void vector_test() {
  std::vector<cooler_shared_ptr<int> > v;
  for (int i = 0; i < 20; i++) v.push_back(i);
}

struct tree {
public:
  tree(std::optional<cooler_shared_ptr<tree>> left,
       std::optional<cooler_shared_ptr<tree>> right)
    : left(left), right(right) { }
  std::optional<cooler_shared_ptr<tree>> left;
  std::optional<cooler_shared_ptr<tree>> right;
};

std::optional<cooler_shared_ptr<tree>> make_tree(unsigned int n) {
  if (n == 0)
    return {};
  return tree(make_tree(n - 1), make_tree(n - 1));
}

unsigned int tree_depth(std::optional<cooler_shared_ptr<tree>> t) {
  if (!t) return 0;
  unsigned int l = tree_depth(t.value()->left), r = tree_depth(t.value()->right);
  return l > r ? l + 1 : r + 1;
}

void tree_test() {
  std::cerr << "tree has depth " << tree_depth(make_tree(14)) << std::endl;
}

int main() {
  scalar_test();
  vector_test();
  tree_test();
  collect();
  std::cerr << collections << " collections performed" << std::endl;
}
