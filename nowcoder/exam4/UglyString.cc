#include <string>
#include <iostream>
#include <algorithm>
using namespace std;

int main() {
    string s;
    cin >> s;

    int uglyCount = 0;
    int tempCount = 0;

    for (int i = 1; i < s.size(); i++) {
        if (s[i] == s[i - 1]) {
            if (s[i] != '?') { tempCount++; }
        } else {
            if (s[i] == '?') { s[i] = (s[i - 1] == 'A' ? 'B' : 'A'); }
            uglyCount += tempCount;
            tempCount = 0;
        }
    }
    cout << uglyCount + tempCount << endl;
}
