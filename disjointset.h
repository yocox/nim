#include <iostream>
#include <map>
#include <set>
#include <vector>
class DisjointSet {
public:
  using IntSet = std::set<int>;
  DisjointSet(int num) : num(num), parent(num), rank(num) {
    for (int i = 0; i < num; ++i) {
      parent[i] = i;
    }
  }

  int find(int a) {
    while (parent[a] != a) {
      parent[a] = parent[parent[a]];
      a = parent[a];
    }
    return a;
  }

  void merge(int a, int b) {
    int na = calNumOne(a);
    int nb = calNumOne(b);

    // if (na != nb) {
    //   std::cout << a << " <=> " << b << std::endl;
    //   std::cout << na << ' ' << nb << std::endl;
    //   assert(false);
    // }

    a = find(a);
    b = find(b);

    if (a == b) {
      return;
    }

    if (rank[a] < rank[b]) {
      std::swap(a, b);
    }

    parent[b] = a;
    if (rank[a] == rank[b]) {
      ++rank[a];
    }
  }

  IntSet roots() {
    IntSet result;
    for (int s = 0; s < num; ++s) {
      result.insert(find(s));
    }
    return result;
  }

  std::map<int, IntSet> components() {
    std::map<int, IntSet> result;
    auto rs = roots();
    for (int i = 0; i < num; ++i) {
      result[find(i)].insert(i);
    }
    return result;
  }

  std::map<int, int> bit_components() {
    std::map<int, int> result;
    auto rs = roots();
    for (int i = 0; i < num; ++i) {
      result[find(i)] += 1 << i;
    }
    // for (auto c : result) {
    //   std::cout << c.first << ": " << c.second << std::endl;
    // }
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

private:
  int num;
  std::vector<int> parent;
  std::vector<int> rank;
};