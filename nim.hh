#include <iostream>
#include <vector>

#include "disjointset.h"

class Nim {
public:
  static constexpr int NUM_STATE = 1 << 15;
  static constexpr int NUM_ACTS = 63;
  static constexpr int posToInt(int a) { return 1 << a; };
  static constexpr int posToInt(int a, int b) { return 1 << a | 1 << b; };
  static constexpr int posToInt(int a, int b, int c) {
    return 1 << a | 1 << b | 1 << c;
  };
  static constexpr int posToInt(int a, int b, int c, int d) {
    return 1 << a | 1 << b | 1 << c | 1 << d;
  };
  static constexpr int posToInt(int a, int b, int c, int d, int e) {
    return 1 << a | 1 << b | 1 << c | 1 << d | 1 << e;
  };
  static constexpr int posToInt(int a, int b, int c, int d, int e, int f) {
    return 1 << a | 1 << b | 1 << c | 1 << d | 1 << e | 1 << f;
  };
  std::vector<int> acts;

  Nim() : parent(NUM_STATE) {
    // num one
    for (int i = 0; i < NUM_STATE; ++i) {
      numOne[i] = calNumOne(i);
    }
    // init steps
    for (int i = 0; i < 15; ++i) {
      acts.push_back(posToInt(i));
    }
    auto add2s = [this](int a, int b, int c) {
      acts.push_back(posToInt(a, b));
      acts.push_back(posToInt(b, c));
      acts.push_back(posToInt(a, c));
    };
    add2s(0, 1, 2);
    add2s(1, 3, 4);
    add2s(2, 4, 5);
    add2s(3, 6, 7);
    add2s(4, 7, 8);
    add2s(5, 8, 9);
    add2s(6, 10, 11);
    add2s(7, 11, 12);
    add2s(8, 12, 13);
    add2s(9, 13, 14);
    auto add3s = [this](int a, int b, int c, int d, int e, int f) {
      acts.push_back(posToInt(a, b, d));
      acts.push_back(posToInt(d, e, f));
      acts.push_back(posToInt(a, c, f));
    };
    add3s(0, 1, 2, 3, 4, 5);
    add3s(1, 3, 4, 6, 7, 8);
    add3s(2, 4, 5, 7, 8, 9);
    add3s(3, 6, 7, 10, 11, 12);
    add3s(4, 7, 8, 11, 12, 13);
    add3s(5, 8, 9, 12, 13, 14);

    // isomorphy
    for (int s = 0; s < NUM_STATE; ++s) {
      isomophy(s);
    }

    // judge status
    win[0] = false;
    std::set<int> winSet;
    std::set<std::multiset<int>> winComponentss;
    std::set<std::multiset<int>> allComponentss;
    for (int s = 1; s < NUM_STATE; ++s) {
      bool w = isWin(s, false);
      // if (s < 2000 && w) {
      if (w) {
        winSet.insert(parent.find(s));
        winComponentss.insert(components(s));
      }
      allComponentss.insert(components(s));
    }
    for (std::multiset<int> cs : winComponentss) {
      std::cout << "===========================================" << std::endl;
      for (int c : cs) {
        print(c);
      }
    }
    std::cout << "Num winSet: " << winSet.size() << std::endl;
    std::cout << "Num winComponents: " << winComponentss.size() << std::endl;
    std::cout << "Num allComponents: " << allComponentss.size() << std::endl;
  }

  void isomophy(int s) {
    parent.merge(s, leftup(s));
    parent.merge(s, rightup(s));
    parent.merge(s, flipHorz(s));
    parent.merge(s, rotate120(s));
    parent.merge(s, flipVert(s));
  }

  std::multiset<int> components(int s) {
    DisjointSet ds(15);
    using line = std::pair<int, int>;
    static constexpr line lines[] = {
        {0, 1},  {0, 2},  {1, 2},   //
        {1, 3},  {1, 4},  {3, 4},   //
        {2, 4},  {2, 5},  {4, 5},   //
        {3, 6},  {3, 7},  {6, 7},   //
        {4, 7},  {4, 8},  {7, 8},   //
        {5, 8},  {5, 9},  {8, 9},   //
        {6, 10}, {6, 11}, {10, 11}, //
        {7, 11}, {7, 12}, {11, 12}, //
        {8, 12}, {8, 13}, {12, 13}, //
        {9, 13}, {9, 14}, {13, 14}, //
    };

    for (const line &l : lines) {
      if (has(s, l.first) && has(s, l.second)) {
        ds.merge(l.first, l.second);
      }
    }
    std::map<int, int> root_and_components = ds.bit_components();
    std::multiset<int> components;
    for (const auto &c : root_and_components) {
      if (has(s, c.first)) {
        components.insert(c.second);
      }
    }
    std::multiset<int> result;
    for (int c : components) {
      result.insert(root(c));
    }
    while (true) {
      if (result.count(1) >= 2) {
        result.erase(1);
        result.erase(1);
        continue;
      }
      if (result.count(7) >= 1) {
        result.erase(7);
        continue;
      }
      break;
    }
    return result;
  }

  bool isWin(int s, bool debug) {
    // if left only 1 circle, win
    if (calNumOne(s) == 1) {
      win[s] = true;
      return true;
    }

    // for each action
    // if any act lead to oponent win, we lose
    // if no act lead to oponent win, we win
    for (int a : acts) {
      if (!doable(s, a)) {
        continue;
      }
      int sp = s - a;
      if (debug) {
        std::cout << "Trying action" << std::endl;
        print(a);
        std::cout << "New state is " << win[sp] << std::endl;
        print(sp);
      }
      if (win[sp]) {
        win[s] = false;
        return false;
      }
    }
    win[s] = true;
    return true;
  }

  static bool has(int s, int pos) { return s & (1 << pos); }

  int root(int s) { return parent.find(s); }

  void merge(int a, int b) {
    // std::cout << "Merging ..." << std::endl;
    // print(a);
    // print(b);
    parent.merge(a, b);
    // std::cout << "done" << std::endl;
  }

  static int leftup(int s) {
    if ((s & posToInt(0, 1, 3, 6, 10)) != 0) {
      return s;
    }
    static constexpr int newIdx[] = {2, 4, 5, 7, 8, 9, 11, 12, 13, 14};
    int result = 0;
    for (int i = 0; i < 10; ++i) {
      if (has(s, newIdx[i])) {
        result += (1 << i);
      }
    }
    return result;
  }

  static int rightup(int s) {
    if ((s & posToInt(0, 2, 5, 9, 14)) != 0) {
      return s;
    }
    static constexpr int newIdx[] = {1, 3, 4, 6, 7, 8, 10, 11, 12, 13};
    int result = 0;
    for (int i = 0; i < 10; ++i) {
      if (has(s, newIdx[i])) {
        result += (1 << i);
      }
    }
    return result;
  }

  static int up(int s) {
    while (true) {
      int p = leftup(s);
      if (p == s) {
        break;
      }
      s = p;
    };
    while (true) {
      int p = rightup(s);
      if (p == s) {
        break;
      }
      s = p;
    };
    return s;
  }

  static int flipHorz(int s) {
    static constexpr int newIdx[] = {0, 2, 1,  5,  4,  3,  9, 8,
                                     7, 6, 14, 13, 12, 11, 10};
    int result = 0;
    for (int i = 0; i < 15; ++i) {
      if (has(s, newIdx[i])) {
        result += (1 << i);
      }
    }
    return result;
  }

  static int rotate120(int s) {
    static constexpr int newIdx[] = {10, 11, 6,  12, 7, 3, 13, 8,
                                     4,  1,  14, 9,  5, 2, 0};
    int result = 0;
    for (int i = 0; i < 15; ++i) {
      if (has(s, newIdx[i])) {
        result += (1 << i);
      }
    }
    return result;
  }

  static int flipVert(int s) {
    if ((s & posToInt(6, 10, 11, 9, 13, 14)) != 0) {
      return s;
    }
    static constexpr int newIdx[] = {12, 7, 8,  3,  4, 5,  6, 1,
                                     2,  9, 10, 11, 0, 13, 14};
    int result = 0;
    for (int i = 0; i < 15; ++i) {
      if (has(s, newIdx[i])) {
        result += (1 << i);
      }
    }
    return result;
  }

  static int calNumOne(int n) {
    int cnt = 0;
    while (n > 0) {
      n &= n - 1;
      ++cnt;
    }
    return cnt;
  }

  static bool doable(int s, int act) { return (act & s) == act; }
  static char c(int a, int b) {
    if (a & (1 << b))
      return 'o';
    return '-';
  }
  static void print(int a) {
    std::cout << "    " << c(a, 0) << std::endl;
    std::cout << "   " << c(a, 1) << ' ' << c(a, 2) << std::endl;
    std::cout << "  " << c(a, 3) << ' ' << c(a, 4) << ' ' << c(a, 5)
              << std::endl;
    std::cout << " " << c(a, 6) << ' ' << c(a, 7) << ' ' << c(a, 8) << ' '
              << c(a, 9) << std::endl;
    std::cout << "" << c(a, 10) << ' ' << c(a, 11) << ' ' << c(a, 12) << ' '
              << c(a, 13) << ' ' << c(a, 14) << std::endl;
  }

  std::set<int> roots() { return parent.roots(); }

private:
  bool win[NUM_STATE];
  int numOne[NUM_STATE];
  DisjointSet parent;
};