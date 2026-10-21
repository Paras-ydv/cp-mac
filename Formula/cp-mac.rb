# Homebrew formula for cp-mac.
# The published copy lives in https://github.com/Paras-ydv/homebrew-cp,
# so users can run:  brew install paras-ydv/cp/cp-mac
class CpMac < Formula
  desc "GCC-only headers (bits/stdc++.h, pb_ds, rope) for Apple Clang"
  homepage "https://github.com/Paras-ydv/cp-mac"
  url "https://github.com/Paras-ydv/cp-mac/archive/refs/tags/v1.0.0.tar.gz"
  sha256 "REPLACE_WITH_SHA256_OF_RELEASE_TARBALL"
  license all_of: [
    "MIT",
    "GPL-3.0-or-later" => { with: "GCC-exception-3.1" },
  ]
  head "https://github.com/Paras-ydv/cp-mac.git", branch: "main"

  livecheck do
    url :stable
    strategy :github_latest
  end

  depends_on :macos

  def install
    (buildpath/"include/cp-mac/VERSION").write version.to_s
    include.install Dir["include/*"]
    bin.install "bin/cp-mac"
    bin.install_symlink "cp-mac" => "cprun"
    doc.install "NOTICE"
    doc.install "third_party/gcc/README.md" => "GCC-NOTICE.md"
    doc.install "third_party/gcc/COPYING3", "third_party/gcc/COPYING.RUNTIME"
  end

  def caveats
    if HOMEBREW_PREFIX.to_s == "/usr/local"
      <<~EOS
        You're all set: #include <bits/stdc++.h>, pb_ds and rope now work with clang++.
          Check your setup:  cp-mac doctor
          Compile and run:   cprun a.cpp < in.txt
      EOS
    else
      <<~EOS
        One more step on Apple Silicon (once, asks for your password):

          cp-mac link

        Apple Clang looks for headers in /usr/local/include, not in
        #{HOMEBREW_PREFIX}/include. `cp-mac link` adds a few links there so
        clang++, g++ and your editor find <bits/stdc++.h>, pb_ds and rope.
        (cprun works even before that.)

          Check your setup:  cp-mac doctor
          Compile and run:   cprun a.cpp < in.txt
          Remove later:      cp-mac uninstall && brew uninstall cp-mac
      EOS
    end
  end

  test do
    (testpath/"t.cpp").write <<~CPP
      #include <bits/stdc++.h>
      #include <ext/pb_ds/assoc_container.hpp>
      #include <ext/pb_ds/tree_policy.hpp>
      #include <ext/rope>
      using namespace std;
      using namespace __gnu_pbds;
      int f(int n) { volatile char pad[64]; pad[0] = 1; return n == 0 ? pad[0] - 1 : 1 + f(n - 1); }
      int main() {
        tree<int, null_type, less<int>, rb_tree_tag, tree_order_statistics_node_update> s;
        s.insert(2); s.insert(1);
        __gnu_cxx::rope<int> r; r.push_back(3);
        int n; cin >> n;
        cout << *s.find_by_order(1) << r[0] << __gcd(4, 6) << __lg(8) << " " << f(n) << endl;
      }
    CPP
    system ENV.cxx, "-std=c++17", "-I#{include}", "t.cpp", "-o", "t"
    assert_equal "2323 1", pipe_output("./t", "1").strip
    # cprun: symlink dispatch, -I fallback and the 256 MB stack (8 MB would overflow here)
    assert_equal "2323 1000000", pipe_output("#{bin}/cprun t.cpp", "1000000").strip
    assert_match version.to_s, shell_output("#{bin}/cp-mac version")
  end
end
