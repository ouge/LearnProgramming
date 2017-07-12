#include <iostream>
#include <thread>
using namespace std;

void hello() { cout << "Hello Concurrent World\n" << endl; }

int main() {
    thread t(hello);
    t.join();    // 等待线程结束
}