// Every pb_ds container CP solutions commonly use, written the usual way.
#include <bits/stdc++.h>
#include <ext/pb_ds/assoc_container.hpp>
#include <ext/pb_ds/tree_policy.hpp>
#include <ext/pb_ds/trie_policy.hpp>
#include <ext/pb_ds/priority_queue.hpp>
using namespace std;
using namespace __gnu_pbds;

template <class T>
using ordered_set = tree<T, null_type, less<T>, rb_tree_tag,
                         tree_order_statistics_node_update>;
template <class T>
using ordered_multiset = tree<T, null_type, less_equal<T>, rb_tree_tag,
                              tree_order_statistics_node_update>;

struct custom_hash {  // the classic anti-hack hash
    static uint64_t splitmix64(uint64_t x) {
        x += 0x9e3779b97f4a7c15ULL;
        x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9ULL;
        x = (x ^ (x >> 27)) * 0x94d049bb133111ebULL;
        return x ^ (x >> 31);
    }
    size_t operator()(uint64_t x) const {
        static const uint64_t FIXED_RANDOM =
            chrono::steady_clock::now().time_since_epoch().count();
        return splitmix64(x + FIXED_RANDOM);
    }
};

#define CHECK(c) do { if (!(c)) { printf("FAIL line %d: %s\n", __LINE__, #c); return 1; } } while (0)

int main() {
    // ordered_set: find_by_order / order_of_key
    ordered_set<int> os;
    for (int x : {5, 1, 9, 3, 7}) os.insert(x);
    CHECK(*os.find_by_order(0) == 1);
    CHECK(*os.find_by_order(2) == 5);
    CHECK(os.order_of_key(6) == 3);
    CHECK(os.find_by_order(5) == os.end());
    os.erase(5);
    CHECK(*os.find_by_order(2) == 7);

    // ordered map with order statistics
    tree<int, string, less<int>, rb_tree_tag, tree_order_statistics_node_update> om;
    om[3] = "c"; om[1] = "a"; om[2] = "b";
    CHECK(om.find_by_order(1)->second == "b");

    // multiset trick with less_equal
    ordered_multiset<int> oms;
    for (int x : {2, 2, 2, 1}) oms.insert(x);
    CHECK(oms.size() == 4);
    CHECK(oms.order_of_key(2) == 1);

    // splay tree flavour
    tree<int, null_type, less<int>, splay_tree_tag, tree_order_statistics_node_update> st;
    st.insert(4); st.insert(2);
    CHECK(*st.find_by_order(0) == 2);

    // split / join
    ordered_set<int> a, b;
    for (int i = 0; i < 10; i++) a.insert(i);
    a.split(4, b);  // a = {0..4}, b = {5..9}
    CHECK(a.size() == 5 && b.size() == 5);
    a.join(b);
    CHECK(a.size() == 10);

    // hash tables
    gp_hash_table<int, int> gp;
    cc_hash_table<int, int> cc;
    gp_hash_table<long long, int, custom_hash> safe;
    for (int i = 0; i < 100000; i++) { gp[i] = i * 2; cc[i] = i * 3; safe[i * 1000003LL] = i; }
    CHECK(gp[777] == 1554 && cc[777] == 2331 && safe[777 * 1000003LL] == 777);
    CHECK(gp.find(-5) == gp.end());
    gp.erase(777);
    CHECK(gp.find(777) == gp.end());

    // trie with prefix search
    typedef trie<string, null_type, trie_string_access_traits<>, pat_trie_tag,
                 trie_prefix_search_node_update> prefix_trie;
    prefix_trie t;
    for (string s : {"apple", "app", "apply", "banana"}) t.insert(s);
    auto range = t.prefix_range("app");
    int cnt = 0;
    for (auto it = range.first; it != range.second; ++it) cnt++;
    CHECK(cnt == 3);

    // meldable priority queues
    __gnu_pbds::priority_queue<int> p1, p2;
    for (int x : {3, 1, 4}) p1.push(x);
    for (int x : {1, 5, 9}) p2.push(x);
    p1.join(p2);
    CHECK(p1.top() == 9 && p1.size() == 6 && p2.empty());
    __gnu_pbds::priority_queue<int, greater<int>, pairing_heap_tag> pq;
    auto h = pq.push(10); pq.push(20);
    pq.modify(h, 30);  // decrease-key style update
    CHECK(pq.top() == 20);
    __gnu_pbds::priority_queue<int, less<int>, binomial_heap_tag> bh; bh.push(1);
    __gnu_pbds::priority_queue<int, less<int>, rc_binomial_heap_tag> rbh; rbh.push(1);
    __gnu_pbds::priority_queue<int, less<int>, thin_heap_tag> th; th.push(1);
    __gnu_pbds::priority_queue<int, less<int>, binary_heap_tag> bnh; bnh.push(1);
    CHECK(bh.top() + rbh.top() + th.top() + bnh.top() == 4);

    puts("pbds: OK");
}
