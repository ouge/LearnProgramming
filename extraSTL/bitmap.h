#ifndef BITMAP_H
#define BITMAP_H

#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>

namespace extrastl {

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

    // Returns the number of bits in the bitset that are set (i.e., that have a value of one)
    size_t count() const {
        uint8_t* ptr = start_;
        size_t   sum = 0;
        for (; ptr != finish_; ++ptr) {
            for (int i = 0; i != 8; ++i) {
                uint8_t t = getMask(*ptr, i);
                if (t) { ++sum; }
            }
        }
        return sum;
    }

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

    // Returns whether any of the bits is set (i.e., whether at least one bit in the bitset is set to one).
    bool any() const {
        uint8_t* ptr = start_;
        for (; ptr != finish_; ++ptr) {
            uint8_t n = *ptr;
            if (n != 0) return true;
        }
        return false;
    }

    // Returns whether none of the bits is set (i.e., whether all bits in the bitset have a value of zero).
    bool none() const { return !any(); }

    //Returns whether all of the bits in the bitset are set (to one).
    bool all() const {
        uint8_t* ptr = start_;
        for (; ptr != finish_; ++ptr) {
            if ((uint8_t)*ptr != (uint8_t)~0) return false;
        }
        return true;
    }

    bitmap& set() {
        uninitialized_fill_n(start_, sizeOfUINT8_, ~0);
        return *this;
    }
    bitmap& set(size_t pos, bool val = true) {
        THROW(pos);
        const auto nth = getNth(pos);
        const auto mth = getMth(pos);
        uint8_t*   ptr = start_ + nth;    //get the nth uint8_t
        setNthInInt8(*ptr, mth, val);
        return *this;
    }

    bitmap& reset() {
        uninitialized_fill_n(start_, sizeOfUINT8_, 0);
        return *this;
    }
    bitmap& reset(size_t pos) {
        set(pos, false);
        return *this;
    }

    bitmap& flip() {
        uint8_t* ptr = start_;
        for (; ptr != finish_; ++ptr) {
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
        uint8_t*    ptr = start_;
        for (; ptr != finish_; ++ptr) {
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
        return ((bytes + EAlign::ALIGN - 1) & ~(EAlign::ALIGN - 1));
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

    uint8_t getMask(uint8_t i, size_t nth) const { return (i & (1 << nth)); }

    // 返回 n 在第几个 Byte
    size_t getNth(size_t n) const { return (n / 8); }

    // 返回 n 在某一字节的偏移
    size_t getMth(size_t n) const { return (n % EAlign::ALIGN); }

    // 使用 std::allocator 分配 n Bytes 空间并用 val 初始化。
    void allocateAndFillN(size_t n, uint8_t val) {
        start_  = dataAllocator::allocate(n);
        finish_ = uninitialized_fill_n(start_, n, val);
    }

    // 如果 n 超出范围抛出异常
    void THROW(size_t n) const {
        if (!(0 <= n && n < size())) throw std::out_of_range("Out Of Range");
    };
};    // end of bitmap
}

template <size_t N>
std::ostream& operator<<(std::ostream& os, const extrastl::bitmap<N>& bm) {
    os << bm.to_string();
    return os;
}

#endif