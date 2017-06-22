#include <iostream>
#include <vector>
using namespace std;

int main() {
    int n;
    cin >> n;
    vector<int> nums(n, 0);
    int         count   = 0;
    int         hasSame = 0;
    for (auto &i : nums) { cin >> i; }
    for (int i = 0; i < n - 1; i++) {
        for (int j = i + 1; j < n; j++) {
            if (nums[i] != nums[j]) {
                count++;
            } else {
                hasSame = 1;
            }
        }
    }
    cout << count + hasSame << endl;
}