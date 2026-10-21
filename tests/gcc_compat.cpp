// GCC behaviour that CP code relies on and that cp-mac once got wrong.
#include <bits/stdc++.h>
#include <ext/pb_ds/assoc_container.hpp>
#include <ext/pb_ds/trie_policy.hpp>
#include <ext/rope>
using namespace std;
using namespace __gnu_pbds;

#define CHECK(c) do { if (!(c)) { printf("FAIL line %d: %s\n", __LINE__, #c); return 1; } } while (0)

typedef long long ll;

// A user's own __gcd next to `using namespace std;` must win, as with GCC.
static ll __gcd(ll a, ll b) { return b ? __gcd(b, a % b) : (a < 0 ? -a : a); }

// GCC made __lg a template in GCC 14; older libstdc++ only has overloads.
#if defined(_LIBCPP_VERSION) || (defined(__GLIBCXX__) && _GLIBCXX_RELEASE >= 14)
# define TEMPLATE_LG 1
#endif

int main() {
    // __gcd / __lg are templates, as in GCC
    CHECK(std::__gcd<ll>(12, 18) == 6);
    vector<ll> v = {12, 18, 30};
    CHECK(accumulate(v.begin(), v.end(), 0LL, std::__gcd<ll>) == 6);
#ifdef TEMPLATE_LG
    CHECK(std::__lg<ll>(1LL << 50) == 50);
#endif
#ifdef _LIBCPP_VERSION   // GCC 15 rejects __int128 here in strict -std=c++NN mode
    CHECK(std::__lg((__int128)1 << 100) == 100);
#endif
    CHECK(std::__lg(-1) == 31);                              // same-width unsigned, like GCC
    static_assert(std::__lg(1024) == 10, "__lg must be constexpr");
    CHECK(__gcd(-4LL, 6LL) == 2);                            // the user's version above

    // rope's fill constructor
    __gnu_cxx::crope s(5, 'a');
    CHECK(s.size() == 5 && s[4] == 'a');
    __gnu_cxx::rope<int> r(100000, 7);
    CHECK(r.size() == 100000 && r[99999] == 7);

    // trie with order statistics (no deprecated allocator rebind)
    typedef trie<string, null_type, trie_string_access_traits<>, pat_trie_tag,
                 trie_order_statistics_node_update> T;
    T t; t.insert("b"); t.insert("a"); t.insert("c");
    CHECK(t.order_of_key("b") == 1);
    CHECK(*t.find_by_order(2) == "c");

    // removed from libc++ in C++17/20, still in GCC
    CHECK(count_if(v.begin(), v.end(), not1(bind2nd(greater<ll>(), 15))) == 1);

    puts("gcc_compat: OK");
}
