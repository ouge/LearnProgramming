// 牛牛和羊羊在玩一个有趣的猜数游戏。在这个游戏中,牛牛玩家选择一个正整数,羊羊根据已给的提示猜这个数字。第i个提示是"Y"或者"N",表示牛牛选择的数是否是i的倍数。
// 例如,如果提示是"YYNYY",它表示这个数使1,2,4,5的倍数,但不是3的倍数。
// 注意到一些提示会出现错误。例如: 提示"NYYY"是错误的,因为所有的整数都是1的倍数,所以起始元素肯定不会是"N"。此外,例如"YNNY"的提示也是错误的,因为结果不可能是4的倍数但不是2的倍数。
// 现在给出一个整数n,表示已给的提示的长度。请计算出长度为n的合法的提示的个数。
// 例如 n = 5:
// 合法的提示有:
// YNNNN YNNNY YNYNN YNYNY YYNNN YYNNY
// YYNYN YYNYY YYYNN YYYNY YYYYN YYYYY
// 所以输出12
//
// 输入描述:
// 输入包括一个整数n(1 ≤ n ≤ 10^6),表示已给提示的长度。
//
// 输出描述:
// 输出一个整数,表示合法的提示个数。因为答案可能会很大,所以输出对于1000000007的模
//
// 输入例子1:
// 5
//
// 输出例子1:
// 12

#include <iostream>
#include <vector>
using namespace std;

int main() {
    int n;
    cin >> n;
    long long   ans = 1;
    vector<int> vis(n + 1);
    const int   mod = 1e9 + 7;

    for (int i = 2; i <= n; i++) {
        // 如果vis[i] == 1，说明不是质数
        if (vis[i]) continue;
        // 将质数 i 的所有幂倍数置1。
        for (int j = i + i; j <= n; j += i) { vis[j] = 1; }
        int      tmp = n;
        int      cnt = 0;

        // n 以内 i 的最大幂 cnt
        while (tmp >= i) {
            tmp /= i;
            cnt++;
        }

        // 不同的质数组互斥（一个质数组指的是质数和它的幂倍数）
        // 因此，因此分别统计每个质数组的方案数（cnt + 1），然后相乘就行。
        // 而混合数（含有不同的质数组）在其中质数组方案确定的情况下是唯一的，不需要统计。
        ans = ans * (cnt + 1) % mod;
    }

    cout << ans << endl;
    return 0;
}