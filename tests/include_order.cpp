// Standard headers before <bits/stdc++.h> (common in older templates).
// Used to be a hard error at -std=c++23 with libc++.
#include <iostream>
#include <map>
#include <algorithm>
#include <bits/stdc++.h>
#include <ext/pb_ds/assoc_container.hpp>
using namespace std;

int main() {
    map<int, int> m; m[1] = 2;
    __gnu_pbds::gp_hash_table<int, int> h; h[3] = 4;
    cout << (m[1] + h[3] == 6 && __gcd(4, 6) == 2 ? "include_order: OK" : "include_order: FAIL") << "\n";
}
