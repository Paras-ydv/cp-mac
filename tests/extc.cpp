// <bits/extc++.h> alone must be enough for pb_ds.
#include <bits/extc++.h>
using namespace std;
using namespace __gnu_pbds;

int main() {
    tree<int, null_type, less<int>, rb_tree_tag, tree_order_statistics_node_update> t;
    t.insert(3); t.insert(1);
    gp_hash_table<int, int> h; h[5] = 1;
    if (*t.find_by_order(0) != 1 || h[5] != 1 || __gcd(4, 6) != 2) { puts("FAIL"); return 1; }
    puts("extc: OK");
}
