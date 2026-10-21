// Compiled with -D_GLIBCXX_DEBUG: an out-of-bounds access must abort,
// exactly like it does with GCC's debug mode. The test runner expects a
// non-zero exit.
#include <bits/stdc++.h>
using namespace std;

int main(int argc, char**) {
    vector<int> v(3);
    volatile int i = 3 + argc;  // out of bounds, not constant-foldable
    printf("%d\n", v[i]);
    puts("debug_mode: NOT CAUGHT");
    return 0;
}
