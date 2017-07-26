#ifndef TINYSTL_VECTOR_H
#define TINYSTL_VECTOR_H

#include <algorithm>
#include <memory>
#include <type_traits>

namespace extrastl {

template <class T, class Alloc = std::allocator<T>>
class vector {
  public:
    using value_type      = T;
    using iterator        = T*;
    using const_iterator  = const T*;
    using pointer         = iterator;
    using reference       = T&;
    using const_reference = const T&;
    using size_type       = size_t;
    using difference_type = ptrdiff_t;
    using allocator_type  = Alloc;

  private:
    T*             start_;
    T*             finish_;
    T*             endOfStorage_;
    allocator_type alloc_;

  public:
    // *********************************
    // 构造析构
    // *********************************
    vector() : start_(0), finish_(0), endOfStorage_(0) {}
    explicit vector(const size_type n) { allocateAndFillN(n, value_type()); }
    vector(const size_type n, const value_type& value) {
        allocateAndFillN(n, value);
    }
    template <class InputIterator>
    vector(InputIterator first, InputIterator last) {
        vector_aux(first, last,
                   typename std::is_integral<InputIterator>::type());
    }
    vector(const vector& v) { allocateAndCopy(v.start_, v.finish_); }
    vector(vector&& v) {
        start_        = v.start_;
        finish_       = v.finish_;
        endOfStorage_ = v.endOfStorage_;
        v.start_ = v.finish_ = v.endOfStorage_ = 0;
    }

    ~vector() { destroyAndDeallocateAll(); }

    vector& operator=(const vector& v) {
        if (this != &v) { allocateAndCopy(v.start_, v.finish_); }
        return *this;
    }
    vector& operator=(vector&& v) {
        if (this != &v) {
            destroyAndDeallocateAll();
            start_        = v.start_;
            finish_       = v.finish_;
            endOfStorage_ = v.endOfStorage_;
            v.start_ = v.finish_ = v.endOfStorage_ = 0;
        }
        return *this;
    }

    allocator_type get_allocator() const { return alloc_; }

    // *********************************
    // 迭代器
    // *********************************
    iterator       begin() { return (start_); }
    const_iterator begin() const { return (start_); }
    const_iterator cbegin() const { return (start_); }
    iterator       end() { return (finish_); }
    const_iterator end() const { return (finish_); }
    const_iterator cend() const { return (finish_); }

    // *********************************
    // 容量
    // *********************************
    size_type size() const { return finish_ - start_; }
    size_type capacity() const { return endOfStorage_ - start_; }
    bool      empty() const { return start_ == finish_; }

    // 调整 capacity 大小
    void reserve(size_type n) {
        if (n <= capacity()) return;
        T* newStart  = alloc_.allocate(n);
        T* newFinish = std::uninitialized_copy(begin(), end(), newStart);
        destroyAndDeallocateAll();

        start_        = newStart;
        finish_       = newFinish;
        endOfStorage_ = start_ + n;
    }

    // 将 vector 的 capacity 收缩至 size。
    // 重新分配一块 size 大小的空间，将 0 - size-1 的数据复制过去。
    // 并释放原 capacity 大小的空间。
    void shrink_to_fit() {
        T* t    = static_cast<T*>(alloc_.allocate(size()));
        finish_ = std::uninitialized_copy(start_, finish_, t);
        alloc_.deallocate(start_, capacity());
        start_        = t;
        endOfStorage_ = finish_;
    };

    // *********************************
    // 成员访问
    // *********************************
    reference operator[](const difference_type i) { return *(begin() + i); }
    const_reference operator[](const difference_type i) const {
        return *(cbegin() + i);
    }
    reference front() { return *(begin()); }
    reference back() { return *(end() - 1); }
    pointer   data() { return start_; }

    // *********************************
    // 修改
    // *********************************
    void clear() {
        alloc_.destroy(start_, finish_);
        finish_ = start_;
    }

    void swap(vector& v) {
        if (this != &v) {
            std::swap(start_, v.start_);
            std::swap(finish_, v.finish_);
            std::swap(endOfStorage_, v.endOfStorage_);
        }
    }

    void push_back(const value_type& value) { insert(end(), value); }

    void pop_back() {
        --finish_;
        alloc_.destroy(finish_);
    }

    iterator insert(iterator position, const value_type& val) {
        const auto index = position - begin();
        insert(position, 1, val);
        return begin() + index;
    }

    void insert(iterator position, const size_type& n, const value_type& val) {
        insert_aux(position, n, val,
                   typename std::is_integral<size_type>::type());
    }

    template <class InputIterator>
    void insert(iterator position, InputIterator first, InputIterator last) {
        insert_aux(position, first, last,
                   typename std::is_integral<InputIterator>::type());
    }

    void resize(size_type n, value_type val = value_type()) {
        if (n < size()) {
            alloc_.destroy(start_ + n, finish_);
            finish_ = start_ + n;
        } else if (n > size() && n <= capacity()) {
            auto lengthOfInsert = n - size();
            finish_ = std::uninitialized_fill_n(finish_, lengthOfInsert, val);
        } else if (n > capacity()) {
            auto lengthOfInsert = n - size();
            T*   newStart  = alloc_.allocate(getNewCapacity(lengthOfInsert));
            T*   newFinish = std::uninitialized_copy(begin(), end(), newStart);
            newFinish =
                    std::uninitialized_fill_n(newFinish, lengthOfInsert, val);

            destroyAndDeallocateAll();
            start_        = newStart;
            finish_       = newFinish;
            endOfStorage_ = start_ + n;
        }
    }

    iterator erase(iterator position) { return erase(position, position + 1); }
    iterator erase(iterator first, iterator last) {
        difference_type lenOfTail    = end() - last;
        difference_type lenOfRemoved = last - first;
        finish_                      = finish_ - lenOfRemoved;
        for (; lenOfTail != 0; --lenOfTail) {
            auto temp = (last - lenOfRemoved);
            *temp     = *(last++);
        }
        return (first);
    }

    // *********************************
    // 比较
    // *********************************
    bool operator==(const vector& v) const {
        if (size() != v.size()) {
            return false;
        } else {
            auto ptr1 = start_;
            auto ptr2 = v.start_;
            for (; ptr1 != finish_ && ptr2 != v.finish_; ++ptr1, ++ptr2) {
                if (*ptr1 != *ptr2) return false;
            }
            return true;
        }
    }
    bool operator!=(const vector& v) const { return !(*this == v); }

  private:
    void destroyAndDeallocateAll() {
        if (capacity() != 0) {
            for (auto i = start_; i < finish_; i++) { alloc_.destroy(i); }
            alloc_.deallocate(start_, capacity());
        }
    }

    void allocateAndFillN(const size_type n, const value_type& value) {
        start_ = alloc_.allocate(n);
        std::uninitialized_fill_n(start_, n, value);
        finish_ = endOfStorage_ = start_ + n;
    }

    template <class InputIterator>
    void allocateAndCopy(InputIterator first, InputIterator last) {
        start_        = alloc_.allocate(last - first);
        finish_       = std::uninitialized_copy(first, last, start_);
        endOfStorage_ = finish_;
    }

    template <class InputIterator>
    void vector_aux(InputIterator first, InputIterator last, std::false_type) {
        allocateAndCopy(first, last);
    }

    template <class Integer>
    void vector_aux(Integer n, const value_type& value, std::true_type) {
        allocateAndFillN(n, value);
    }

    template <class InputIterator>
    void insert_aux(iterator position, InputIterator first, InputIterator last,
                    std::false_type) {
        difference_type locationLeft = endOfStorage_ - finish_;
        difference_type locationNeed = distance(first, last);

        if (locationLeft >= locationNeed) {
            if (finish_ - position > locationNeed) {
                std::uninitialized_copy(finish_ - locationNeed, finish_,
                                        finish_);
                std::copy_backward(position, finish_ - locationNeed, finish_);
                std::copy(first, last, position);
            } else {
                iterator temp = std::uninitialized_copy(
                        first + (finish_ - position), last, finish_);
                std::uninitialized_copy(position, finish_, temp);
                std::copy(first, first + (finish_ - position), position);
            }
            finish_ += locationNeed;
        } else {
            reallocateAndCopy(position, first, last);
        }
    }

    template <class Integer>
    void insert_aux(iterator position, Integer n, const value_type& value,
                    std::true_type) {
        assert(n != 0);
        difference_type locationLeft =
                endOfStorage_ - finish_;    // the size of left storage
        difference_type locationNeed = n;

        if (locationLeft >= locationNeed) {
            auto tempPtr = end() - 1;
            for (; tempPtr - position >= 0;
                 --tempPtr) {    //move the [position, finish_) back
                //*(tempPtr + locationNeed) = *tempPtr;//bug
                construct(tempPtr + locationNeed, *tempPtr);
            }
            std::uninitialized_fill_n(position, n, value);
            finish_ += locationNeed;
        } else {
            reallocateAndFillN(position, n, value);
        }
    }

    template <class InputIterator>
    void reallocateAndCopy(iterator position, InputIterator first,
                           InputIterator last) {
        difference_type newCapacity = getNewCapacity(last - first);

        T* newStart        = alloc_.allocate(newCapacity);
        T* newEndOfStorage = newStart + newCapacity;
        T* newFinish = std::uninitialized_copy(begin(), position, newStart);
        newFinish    = std::uninitialized_copy(first, last, newFinish);
        newFinish    = std::uninitialized_copy(position, end(), newFinish);

        destroyAndDeallocateAll();
        start_        = newStart;
        finish_       = newFinish;
        endOfStorage_ = newEndOfStorage;
    }

    void reallocateAndFillN(iterator position, const size_type& n,
                            const value_type& val) {
        difference_type newCapacity = getNewCapacity(n);

        T* newStart        = alloc_.allocate(newCapacity);
        T* newEndOfStorage = newStart + newCapacity;
        T* newFinish = std::uninitialized_copy(begin(), position, newStart);
        newFinish    = std::uninitialized_fill_n(newFinish, n, val);
        newFinish    = std::uninitialized_copy(position, end(), newFinish);

        destroyAndDeallocateAll();
        start_        = newStart;
        finish_       = newFinish;
        endOfStorage_ = newEndOfStorage;
    }

    size_type getNewCapacity(size_type len) const {
        size_type oldCapacity = endOfStorage_ - start_;
        auto      res         = std::max(oldCapacity, len);
        size_type newCapacity = (oldCapacity != 0 ? (oldCapacity + res) : len);
        return newCapacity;
    }

};    // end of class vector
}    // namespace extrastl

template <class T, class Alloc>
bool operator==(const extrastl::vector<T, Alloc>& v1,
                const extrastl::vector<T, Alloc>& v2) {
    //return v1 == v2;
    return v1.operator==(v2);
}
template <class T, class Alloc>
bool operator!=(const extrastl::vector<T, Alloc>& v1,
                const extrastl::vector<T, Alloc>& v2) {
    return !(v1 == v2);
}

#endif