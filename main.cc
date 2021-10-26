#include "nim.hh"
#include <iostream>

static constexpr int A[3] = {2, 3, 4};

int main() {
  std::cout << "start" << std::endl;
  Nim n;
  //   n.isWin(65529, true);
  int s = Nim::posToInt(6_000_0, 10, 11) | //
          Nim::posToInt(4) |         //
          Nim::posToInt(9, 13, 14);
  auto cs = n.components(s);
  std::cout << "Components: " << cs.size() << std::endl;
  for (int c : cs) {
    std::cout << c << std::endl;
    Nim::print(c);
  }
  std::cout << "ok" << std::endl;
}
