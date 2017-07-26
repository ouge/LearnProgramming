// 小易拥有一个拥有魔力的手环上面有n个数字(构成一个环),当这个魔力手环每次使用魔力的时候就会发生一种奇特的变化：
// 每个数字会变成自己跟后面一个数字的和(最后一个数字的后面一个数字是第一个)。
// 一旦某个位置的数字大于等于100就马上对100取模(比如某个位置变为103,就会自动变为3)。
// 现在给出这个魔力手环的构成，请你计算出使用k次魔力之后魔力手环的状态。

// 输入描述:
// 输入数据包括两行：
// 第一行为两个整数n(2 ≤ n ≤ 50)和k(1 ≤ k ≤ 2000000000),以空格分隔
// 第二行为魔力手环初始的n个数，以空格分隔。范围都在0至99.

// 输出描述:
// 输出魔力手环使用k次之后的状态，以空格分隔，行末无空格。

// 输入例子:
// 3 2
// 1 2 3

// 输出例子:
// 8 9 7

// 思路：矩阵快速幂

#include <iostream>
#include <vector>
using namespace std;

void matrixMultiply(vector<vector<int>> &a, vector<vector<int>> &b) {
    vector<vector<int>> c(a.size(), vector<int>(b[0].size(), 0));
    for (int i = 0; i < c.size(); i++) {
        for (int j = 0; j < c[0].size(); j++) {
            for (int k = 0; k < b.size(); k++) { c[i][j] += a[i][k] * b[k][j]; }
            c[i][j] %= 100;
        }
    }
    swap(a, c);
}

int main() {
    int n, k;
    cin >> n >> k;
    vector<vector<int>> nums{vector<int>(n, 0)};
    vector<vector<int>> marix(n, vector<int>(n, 0));

    for (auto &a : nums[0]) { cin >> a; }

    for (int i = 0; i < n - 1; i++) {
        marix[i][i]     = 1;
        marix[i + 1][i] = 1;
    }
    marix[n - 1][n - 1] = 1;
    marix[0][n - 1]     = 1;

    while (k) {
        if (k & 1) {
            matrixMultiply(nums, marix);
            k--;
        } else {
            k >>= 1;
            matrixMultiply(marix, marix);
        }
    }

    bool printflag = false;
    for (auto &a : nums[0]) {
        if (printflag) {
            cout << ' ' << a;
        } else {
            cout << a;
            printflag = true;
        }
    }

    cout << endl;
}
