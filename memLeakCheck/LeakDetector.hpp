#ifndef __LEAK_DETECTOR__
#define __LEAK_DETECTOR__

#include <iostream>
#include <cstdint>
#include <cstring>

void* operator new(size_t _size, char* _file, unsigned int _line);
void* operator new[](size_t _size, char* _file, unsigned int _line);

class _leak_detector {
  public:
    // callCount 保证了 LeakDector() 只调用了一次
    static unsigned int callCount;
    _leak_detector() noexcept { ++callCount; }
    ~_leak_detector() noexcept {
        if (--callCount == 0) { LeakDetector(); }
    }

  private:
    static unsigned int LeakDetector() noexcept;
};

unsigned int _leak_detector::callCount = 0;

static _leak_detector _exit_counter;

struct _MemoryList {
    _MemoryList *next, *prev;
    size_t       size;       // 申请内存大小
    bool         isArray;    // 是否申请了数组
    char*        file;       // 存储所在文件
    unsigned int line;       // 存储所在行
};

// 保存未释放的内存大小
static unsigned long _memory_allocated = 0;

// 链表结点
static _MemoryList _root{
        &_root, &_root,    // 第一个元素的前向后相指针均指向自己
        0,                 // 内存大小为0
        false,             // 不是数组
        NULL,              // 文件指针为空
        0                  // 行号为0
};

// 头插
void* AllocateMemory(size_t _size, bool _array, char* _file, unsigned _line) {
    size_t       newSize = sizeof(_MemoryList) + _size;
    _MemoryList* newElem = (_MemoryList*)malloc(newSize);

    newElem->next    = _root.next;
    newElem->prev    = &_root;
    newElem->size    = _size;
    newElem->isArray = _array;
    newElem->file    = NULL;

    if (_file) {
        newElem->file = (char*)malloc(strlen(_file) + 1);
        strcpy(newElem->file, _file);
    }

    newElem->line = _line;

    _root.next->prev = newElem;
    _root.next       = newElem;

    _memory_allocated += _size;

    return (char*)newElem + sizeof(_MemoryList);
}

void DeleteMemory(void* _ptr, bool _array) {
    // 找到所在内存块起始位置
    _MemoryList* currentElem =
            (_MemoryList*)((char*)_ptr - sizeof(_MemoryList));

    if (currentElem->isArray != _array) return;

    currentElem->prev->next = currentElem->next;
    currentElem->next->prev = currentElem->prev;

    //
    _memory_allocated -= currentElem->size;

    if (currentElem->file) free(currentElem->file);
    free(currentElem);
}

// 重载 new 运算符
void* operator new(size_t _size) {
    return AllocateMemory(_size, false, NULL, 0);
}
void* operator new[](size_t _size) {
    return AllocateMemory(_size, true, NULL, 0);
}
void* operator new(size_t _size, char* _file, unsigned int _line) {
    return AllocateMemory(_size, false, _file, _line);
}
void* operator new[](size_t _size, char* _file, unsigned int _line) {
    return AllocateMemory(_size, true, _file, _line);
}

// 重载 delete 运算符
void operator delete(void* _ptr) noexcept { DeleteMemory(_ptr, false); }
void operator delete[](void* _ptr) noexcept { DeleteMemory(_ptr, true); }

//
unsigned int _leak_detector::LeakDetector(void) noexcept {
    unsigned int count = 0;
    // 遍历整个列表, 如果有内存泄露，那么 _LeakRoot.next 总不是指向自己的
    _MemoryList* ptr = _root.next;
    while (ptr && ptr != &_root) {
        // 输出存在内存泄露的相关信息, 如泄露大小, 产生泄露的位置
        if (ptr->isArray)
            std::cout << "leak[] ";
        else
            std::cout << "leak   ";
        std::cout << ptr << " size " << ptr->size;
        if (ptr->file)
            std::cout << " (locate in " << ptr->file << " line " << ptr->line
                      << ")";
        else
            std::cout << " (Cannot find position)";
        std::cout << std::endl;

        ++count;
        ptr = ptr->next;
    }

    if (count)
        std::cout << "Total " << count << " leaks, size " << _memory_allocated
                  << " byte." << std::endl;
    return count;
}

#endif    // __LEAK_DETECTOR__