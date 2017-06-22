#include <iostream>
#include <algorithm>
#include <string>

using namespace std;

int main() {
    string s;
    cin >> s;
    int count = s.size();
    for (int i = 1; i < s.size(); i++) {
        if (s[i] == s[i - 1]) { count--; }
    }
    cout << count << endl;
}