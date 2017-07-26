// 大家一定玩过“推箱子”这个经典的游戏。具体规则就是在一个N*M的地图上，有1个玩家、1个箱子、1个目的地以及若干障碍，其余是空地。
// 玩家可以往上下左右4个方向移动，但是不能移动出地图或者移动到障碍里去。
// 如果往这个方向移动推到了箱子，箱子也会按这个方向移动一格，当然，箱子也不能被推出地图或推到障碍里。
// 当箱子被推到目的地以后，游戏目标达成。
// 现在告诉你游戏开始是初始的地图布局，请你求出玩家最少需要移动多少步才能够将游戏目标达成。 

// 输入描述:
// 每个测试输入包含1个测试用例
// 第一行输入两个数字N，M表示地图的大小。其中0<N，M<=8。
// 接下来有N行，每行包含M个字符表示该行地图。其中 . 表示空地、X表示玩家、*表示箱子、#表示障碍、@表示目的地。
// 每个地图必定包含1个玩家、1个箱子、1个目的地。

// 输出描述:
// 输出一个数字表示玩家最少需要移动多少步才能将游戏目标达成。当无论如何达成不了的时候，输出-1。

// 输入例子:
// 4 4
// ....
// ..*@
// ....
// .X..
// 6 6
// ...#..
// ......
// #*##..
// ..##.#
// ..X...
// .@#...

// 输出例子:
// 3
// 11

// 思路：BFS

// 过程：
// 1、建立一个四维数组表示到特定人和箱位置需要的最少步数。
// 2、建立一个队列进行BFS。队列元素为人和箱位置状态。
// 3、取队列头，令该位置下人朝四个方向移动，检查合法性。
// 4、如果移动后的人箱状态合法且该状态是第一次到达，添加进队列。

#include <iostream>
#include <queue>
using namespace std;

struct Position {
    int hx_;
    int hy_;
    int bx_;
    int by_;
    Position(int hx, int hy, int bx, int by)
            : hx_(hx), hy_(hy), bx_(bx), by_(by) {}
};

int main() {
    int  n, m;
    char map[8][8]         = {0};
    int  count[8][8][8][8] = {0};

    int             hx, hy, bx, by, ex, ey;
    queue<Position> bfsQueue;
    int             move[4][2] = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};

    cin >> n >> m;

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            cin >> map[i][j];
            if (map[i][j] == 'X') {
                hx = i;
                hy = j;
            }
            if (map[i][j] == '*') {
                bx = i;
                by = j;
            }
            if (map[i][j] == '@') {
                ex = i;
                ey = j;
            }
        }
    }

    count[hx][hy][bx][by] = 1;
    bfsQueue.push(Position(hx, hy, bx, by));

    while (!bfsQueue.empty()) {
        Position currentPos = bfsQueue.front();
        bfsQueue.pop();

        if (currentPos.bx_ == ex && currentPos.by_ == ey) {
            cout << count[currentPos.hx_][currentPos.hy_][currentPos.bx_]
                         [currentPos.by_]
                            - 1
                 << endl;
            return 0;
        }

        for (int i = 0; i < 4; i++) {
            Position movedPos = currentPos;
            movedPos.hx_ += move[i][0];
            movedPos.hy_ += move[i][1];

            if (movedPos.hx_ < 0 || movedPos.hx_ >= n || movedPos.hy_ < 0
                || movedPos.hy_ >= m
                || map[movedPos.hx_][movedPos.hy_] == '#') {
                continue;
            }

            if (movedPos.hx_ == movedPos.bx_ && movedPos.hy_ == movedPos.by_) {
                movedPos.bx_ += move[i][0];
                movedPos.by_ += move[i][1];
                if (movedPos.bx_ < 0 || movedPos.bx_ >= n || movedPos.by_ < 0
                    || movedPos.by_ >= m
                    || map[movedPos.bx_][movedPos.by_] == '#') {
                    continue;
                }
            }

            if (count[movedPos.hx_][movedPos.hy_][movedPos.bx_][movedPos.by_]
                > 0) {
                continue;
            }

            count[movedPos.hx_][movedPos.hy_][movedPos.bx_][movedPos.by_] =
                    count[currentPos.hx_][currentPos.hy_][currentPos.bx_]
                         [currentPos.by_]
                    + 1;
            bfsQueue.push(movedPos);
        }
    }

    cout << -1 << endl;

    return 0;
}