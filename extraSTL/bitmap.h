#ifndef EXTRASTL_BITMAP_H
#define EXTRASTL_BITMAP_H

#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>

namespace extrastl {

// TODO: 拓展成2bit位图
template <size_t N>
class bitmap {
  public:
    using dataAllocator = std::allocator<uint8_t>;

  private:
    uint8_t*     start_;
    uint8_t*     finish_;
    const size_t size_;
    const size_t sizeOfUINT8_;
    enum EAlign { ALIGN = 8 };

  public:
    bitmap() : size_(roundUp8(N)), sizeOfUINT8_(roundUp8(N) / 8) {
        allocateAndFillN(sizeOfUINT8_, 0);
    }

    // 统计位图中 1 的个数
    size_t count() const {
        size_t sum = 0;
        for (uint8_t* ptr = start_; ptr != finish_; ++ptr) {
            // 检查每一个Byte中1的个数
            for (int i = 0; i != 8; ++i) {
                uint8_t t = getMask(*ptr, i);
                if (t) { ++sum; }
            }
        }
        return sum;
    }

    // 返回位图总位数
    size_t size() const { return size_; }

    // 检查第pos位是否为1。
    bool test(size_t pos) const {
        THROW(pos);
        const auto nth  = getNth(pos);
        const auto mth  = getMth(pos);
        uint8_t*   ptr  = start_ + nth;
        uint8_t    temp = getMask(*ptr, mth);
        if (temp) return true;
        return false;
    }

    bool any() const {
        for (uint8_t* ptr = start_; ptr != finish_; ++ptr) {
            uint8_t n = *ptr;
            if (n != 0) return true;
        }
        return false;
    }

    bool none() const { return !any(); }

    bool all() const {
        for (uint8_t* ptr = start_; ptr != finish_; ++ptr) {
            if ((uint8_t)*ptr != (uint8_t)~0) return false;
        }
        return true;
    }

    bitmap& set() {
        std::uninitialized_fill_n(start_, sizeOfUINT8_, ~0);
        return *this;
    }
    bitmap& set(size_t pos, bool val = true) {
        THROW(pos);
        const auto nth = getNth(pos);
        const auto mth = getMth(pos);
        uint8_t*   ptr = start_ + nth;
        setNthInInt8(*ptr, mth, val);
        return *this;
    }

    bitmap& reset() {
        std::uninitialized_fill_n(start_, sizeOfUINT8_, 0);
        return *this;
    }
    bitmap& reset(size_t pos) {
        set(pos, false);
        return *this;
    }

    bitmap& flip() {
        for (uint8_t* ptr = start_; ptr != finish_; ++ptr) {
            uint8_t n = *ptr;
            *ptr      = ~n;
        }
        return *this;
    };
    bitmap& flip(size_t pos) {
        THROW(pos);
        const auto nth  = getNth(pos);
        const auto mth  = getMth(pos);
        uint8_t*   ptr  = start_ + nth;
        uint8_t    temp = getMask(*ptr, mth);
        if (temp)
            setNthInInt8(*ptr, mth, false);
        else
            setNthInInt8(*ptr, mth, true);
        return *this;
    }

    std::string to_string() const {
        std::string str;
        for (uint8_t* ptr = start_; ptr != finish_; ++ptr) {
            uint8_t n = *ptr;
            for (int i = 0; i != 8; ++i) {
                uint8_t t = getMask(n, i);
                if (t)
                    str += "1";
                else
                    str += "0";
            }
        }
        return str;
    }

  private:
    // 将分配的空间大小上调至 8bit 的倍数
    size_t roundUp8(size_t bits) {
        return ((bits + EAlign::ALIGN - 1) & ~(EAlign::ALIGN - 1));
    }

    // 将第 i * 8 + nth 位设为 newVal
    void setNthInInt8(uint8_t& i, size_t nth, bool newVal) {
        uint8_t temp = getMask(i, nth);
        if ((bool)temp == newVal) {
            return;
        } else {
            if (temp) {
                temp = ~temp;
                i    = i & temp;
            } else {
                i = i | (1 << nth);
            }
        }
    }

    // 获得8bit i
    uint8_t getMask(uint8_t i, size_t nth) const { return (i & (1 << nth)); }

    // 返回 n 在第几个 Byte
    size_t getNth(size_t n) const { return (n / 8); }

    // 返回 n 在某一字节的偏移
    size_t getMth(size_t n) const { return (n % EAlign::ALIGN); }

    // 使用 std::allocator 分配 n Bytes 空间并用 val 初始化。
    void allocateAndFillN(size_t n, uint8_t val) {
        dataAllocator alloc;
        start_  = alloc.allocate(n);
        finish_ = std::uninitialized_fill_n(start_, n, val);
    }

    // 如果 n 超出范围抛出异常
    void THROW(size_t n) const {
        if (!(0 <= n && n < size())) throw std::out_of_range("Out Of Range");
    };
};
}

template <size_t N>
std::ostream& operator<<(std::ostream& os, const extrastl::bitmap<N>& bm) {
    os << bm.to_string();
    return os;
}

#endif