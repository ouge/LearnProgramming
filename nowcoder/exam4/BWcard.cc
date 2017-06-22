#include <iostream>
#include <algorithm>
#include <string>
using namespace std;

int main() {
    string s;
    cin >> s;
    int count1 = 0;
    int count2 = 0;
    if (s.empty() || s.size() == 1) { return 0; }
    for (int i = 0; i < s.size(); i++) {
        if (i & 1) {
            if (s[i] == 'B') { count1++; }
        } else {
            if (s[i] == 'W') { count1++; }
        }
    }
    for (int i = 0; i < s.size(); i++) {
        if (i & 1) {
            if (s[i] == 'W') { count2++; }
        } else {
            if (s[i] == 'B') { count2++; }
        }
    }
    cout << min(count1, count2) << endl;
}