// __gnu_cxx::rope the way CP solutions use it (implicit-treap replacement).
#include <bits/stdc++.h>
#include <ext/rope>
using namespace std;
using namespace __gnu_cxx;

#define CHECK(c) do { if (!(c)) { printf("FAIL line %d: %s\n", __LINE__, #c); return 1; } } while (0)

int main() {
    // rope<int>: cut-and-paste a block of an array
    rope<int> r;
    for (int i = 0; i < 10; i++) r.push_back(i);
    rope<int> mid = r.substr(2, 3);  // {2,3,4}
    r.erase(2, 3);                   // {0,1,5,6,7,8,9}
    r.insert(r.size(), mid);         // {0,1,5,6,7,8,9,2,3,4}
    int expect[] = {0, 1, 5, 6, 7, 8, 9, 2, 3, 4};
    CHECK(r.size() == 10);
    for (int i = 0; i < 10; i++) CHECK(r[i] == expect[i]);
    int sum = 0;
    for (auto it = r.begin(); it != r.end(); ++it) sum += *it;
    CHECK(sum == 45);

    // crope (rope<char>)
    crope s("hello world");
    s.insert(5, ",");
    CHECK(s == crope("hello, world"));
    CHECK(s.substr(7, 5) == crope("world"));
    s.replace(0, 5, "HELLO");
    CHECK(s[0] == 'H' && s.size() == 12);
    string str(s.begin(), s.end());
    CHECK(str == "HELLO, world");
    CHECK(strcmp(s.c_str(), "HELLO, world") == 0);
    CHECK(s.find('w') == 7);
    s += crope(" again");
    CHECK(s.size() == 18);
    CHECK(crope("abc") < crope("abd"));

    // persistence: copies are O(1) and share structure
    rope<int> v0;
    for (int i = 0; i < 1000; i++) v0.push_back(i);
    vector<rope<int>> versions = {v0};
    for (int k = 0; k < 100; k++) {
        rope<int> nxt = versions.back();
        nxt.replace(k, k * 7);
        versions.push_back(nxt);
    }
    CHECK(versions[0][50] == 50);
    CHECK(versions[100][50] == 350);

    // a big cut/paste workload (the typical CF problem shape)
    rope<int> big;
    const int N = 200000;
    for (int i = 0; i < N; i++) big.push_back(i);
    mt19937 rng(1);
    for (int q = 0; q < 20000; q++) {
        int l = rng() % N, len = rng() % (N - l) + 1;
        rope<int> part = big.substr(l, len);
        big.erase(l, len);
        big.insert(0, part);
    }
    CHECK(big.size() == (size_t)N);
    long long total = 0;
    for (auto it = big.begin(); it != big.end(); ++it) total += *it;
    CHECK(total == (long long)N * (N - 1) / 2);

#ifdef _LIBCPP_VERSION
    // cp-mac extra: GCC only has std::tr1::hash<crope>; cp-mac provides
    // std::hash<crope>, so ropes work as unordered_set/map keys.
    unordered_set<crope> seen;
    seen.insert(crope("a")); seen.insert(crope("a")); seen.insert(crope("b"));
    CHECK(seen.size() == 2);
#endif

    puts("rope: OK");
}
