// bits/extc++.h — drop-in replacement for GCC's <bits/extc++.h>:
// everything in <bits/stdc++.h> plus the policy-based data structures
// and rope.
// Part of cp-mac: https://github.com/Paras-ydv/cp-mac (MIT License)
#if __INCLUDE_LEVEL__ > 0
#pragma GCC system_header
#endif

#ifndef CPMAC_BITS_EXTCXX_H
#define CPMAC_BITS_EXTCXX_H

// Real GCC: use its own (see <bits/stdc++.h>).
#if defined(__GNUC__) && !defined(__clang__)
#include_next <bits/extc++.h>
#else

#include <bits/stdc++.h>

#include <ext/pb_ds/assoc_container.hpp>
#include <ext/pb_ds/exception.hpp>
#include <ext/pb_ds/hash_policy.hpp>
#include <ext/pb_ds/list_update_policy.hpp>
#include <ext/pb_ds/priority_queue.hpp>
#include <ext/pb_ds/tag_and_trait.hpp>
#include <ext/pb_ds/tree_policy.hpp>
#include <ext/pb_ds/trie_policy.hpp>
#include <ext/rope>

#endif // real GCC
#endif // CPMAC_BITS_EXTCXX_H
