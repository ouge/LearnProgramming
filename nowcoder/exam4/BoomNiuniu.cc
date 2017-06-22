#include <iostream>
#include <vector>
using namespace std;

int main() {
    int n, a;
    cin >> n >> a;
    int t;
    while (n--) {
        cin >> t;
        if (t == a) { a <<= 1; }
    }
    cout << a << endl;
}