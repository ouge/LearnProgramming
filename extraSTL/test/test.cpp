#include "../vector.h"
#include "../bitmap.h"
#include "../list.h"
#include <iostream>
using namespace std;

int main() {
    extrastl::vector<int>  vec;
    extrastl::bitmap<1000> bm;
    extrastl::list<int>    l;
    l.push_back(11);
    l.push_back(11);
    l.push_back(11);
    cout << l.size() << l.back() << endl;

    return 0;
}