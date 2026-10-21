// A typical CP file: the GCC-isms that fail on a stock Mac.
#include <bits/stdc++.h>
using namespace std;
using namespace __gnu_cxx;  // appears in many templates; must not error

#define CHECK(c) do { if (!(c)) { printf("FAIL line %d: %s\n", __LINE__, #c); return 1; } } while (0)

typedef long long ll;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    // __gcd on signed types, results identical to GCC (sign included)
    CHECK(__gcd(12, 18) == 6);
    CHECK(__gcd(12LL, 18LL) == 6);
    CHECK(__gcd(-4, 6) == 2);
    CHECK(__gcd(4, -6) == -2);
    CHECK(__gcd(0, 5) == 5);
    CHECK(std::__gcd(7u, 21u) == 7u);
    CHECK(__gcd((ll)1e18, (ll)6e17) == (ll)2e17);
    __int128 big = (__int128)1 << 100;
    CHECK(__gcd(big, (__int128)1 << 70) == (__int128)1 << 70);

    // __lg for sparse tables
    CHECK(__lg(1) == 0);
    CHECK(__lg(8) == 3);
    CHECK(__lg(9) == 3);
    CHECK(__lg(1LL << 40) == 40);
    CHECK(std::__lg(1000000u) == 19);
    int n = 1000;
    vector<vector<int>> sp(__lg(n) + 1, vector<int>(n));
    CHECK(sp.size() == 10);

    static_assert(__lg(1024) == 10, "__lg should be constexpr");

    // removed-in-C++17 helpers that old templates still use
    vector<int> v(10);
    iota(v.begin(), v.end(), 0);
    random_shuffle(v.begin(), v.end());
    sort(v.begin(), v.end());
    CHECK(v[9] == 9);
    struct Cmp : binary_function<int, int, bool> {
        bool operator()(int a, int b) const { return a > b; }
    };
    sort(v.begin(), v.end(), Cmp());
    CHECK(v[0] == 9);
    CHECK(count_if(v.begin(), v.end(), bind2nd(less<int>(), 5)) == 5);

    // everyday CP library usage
    map<int, int> m; unordered_map<ll, int> um; set<int> s; multiset<int> ms;
    priority_queue<pair<int, int>, vector<pair<int, int>>, greater<>> pq;
    deque<int> dq; bitset<64> bs(255); stack<int> st; queue<int> q;
    m[1]++; um[1LL << 40] = 3; s.insert(2); ms.insert(2); ms.insert(2);
    pq.push({3, 1}); pq.push({1, 2}); dq.push_front(1); st.push(1); q.push(1);
    CHECK(m[1] == 1 && um[1LL << 40] == 3 && ms.count(2) == 2);
    CHECK(pq.top().first == 1 && bs.count() == 8);
    mt19937_64 rng(42);
    CHECK(uniform_int_distribution<int>(1, 6)(rng) >= 1);
    CHECK(__builtin_popcountll(255) == 8 && __builtin_ctz(8) == 3);
    string str = "hello"; reverse(str.begin(), str.end());
    CHECK(str == "olleh");
    CHECK(accumulate(v.begin(), v.end(), 0LL) == 45);
    auto it = lower_bound(v.rbegin(), v.rend(), 5);
    CHECK(*it == 5);

    puts("stdc: OK");
}
