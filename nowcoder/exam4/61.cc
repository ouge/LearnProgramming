#include <iostream>
#include <algorithm>
#include <vector>
using namespace std;

int main() {
    int n;
    cin >> n;
    vector<int> h(n, 0);
    for (auto& a : h) { cin >> a; }
    sort(h.begin(), h.end());
    int ret = h[1] - h[0];
    for (int i = 2; i < n; i++) { ret = max(h[i] - h[i - 2], ret); }
    cout << max(ret, h[n - 1] - h[n - 2]);
}