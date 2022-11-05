#include "crc.h"
#include <iostream>

int main() {
  {
    crc::cooler_shared_ptr<int> x = 42;
    std::cout << x.get() << std::endl;
    crc::cooler_shared_ptr<int> y = 123;
    x = y;
    std::cout << x.get() << std::endl;
    // Old value of x should be freed here.
    crc::collect();
    std::cout << x.get() << std::endl;
    x = crc::cooler_shared_ptr<int>(1337);
    y = crc::cooler_shared_ptr<int>(420);
  }
  // Dead roots should get un-logged.
  crc::collect();
  return 0;
}
