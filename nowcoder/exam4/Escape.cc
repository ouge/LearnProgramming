// dp[i][j][s]，代表的含义是从[0,i]中取j个数，使他们的和模n的余数为s，这样的j个数的集合的个数。
// 状态转移方程为：
// dp[i][j][s]=dp[i-1][j][s]+dp[i-1][j-1][(s-i+n)%n]

#include <iostream>
using namespace std;

const int mod = 1e9 + 7;
int       dp[55][1005];    // dp[x][y]表示取x个使他们的和模n的余数为y的所有集合的数目

int main() {
    int n, k;
    cin >> n >> k;
    dp[0][0] = 1;
    for (int i = 0; i < n; i++) {
        // 需要从后往前以保证每次拿的值是上一轮的
        for (int j = k; j >= 1; j--) {
            //
            for (int s = 0; s < n; s++) {
                //  dp[i][j][s]=dp[i-1][j][s]+dp[i-1][j-1][(s-i+n)%n]
                dp[j][s] = (dp[j][s] + dp[j - 1][(n + s - i) % n]) % mod;
            }
        }
    }
    cout << dp[k][0] << endl;
    return 0;
}