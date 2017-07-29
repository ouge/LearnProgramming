#ifndef EXTRASTL_LIST_H_
#define EXTRASTL_LIST_H_

#include <memory>
#include <iterator>
#include <list>
namespace extrastl {
namespace detail {
template <class T>
struct node;
template <class T>
struct listIterator;
}

//the class of list
template <class T>
class list {
    friend struct detail::listIterator<T>;

  private:
    using nodeAllocator = std::allocator<detail::node<T>>;
    using nodePtr       = detail::node<T>*;

  public:
    using value_type     = T;
    using iterator       = detail::listIterator<T>;
    using const_iterator = detail::listIterator<const T>;
    using reference      = T&;
    using size_type      = size_t;

  private:
    iterator head;
    iterator tail;

  public:
    // **************************************************************
    // ************************构造函数*******************************
    // **************************************************************
    list() {
        head.p = newNode();
        tail.p = head.p;
    }
    explicit list(size_type n, const value_type& val = value_type()) {
        ctorAux(n, val, std::is_integral<value_type>());
    }
    template <class InputIterator>
    list(InputIterator first, InputIterator last) {
        ctorAux(first, last, std::is_integral<InputIterator>());
    }
    list(const list& l) {
        head.p = newNode();
        tail.p = head.p;
        for (auto node = l.head.p; node != l.tail.p; node = node->next)
            push_back(node->data);
    }
    list& operator=(const list& l) {
        if (this != &l) { list(l).swap(*this); }
        return *this;
    }
    ~list() {
        nodeAllocator alloc;
        for (; head != tail;) {
            auto          temp = head++;
            nodeAllocator alloc;
            alloc.destroy(temp.p);
            alloc.deallocate(temp.p, 1);
        }
        alloc.deallocate(tail.p, 1);
    }

    // **************************************************************
    // ************************元素访问*******************************
    // **************************************************************
    reference front() { return (head.p->data); }
    reference back() { return (tail.p->prev->data); }

    // **************************************************************
    // ***************************容量********************************
    // **************************************************************
    bool      empty() const { return head == tail; }
    size_type size() const {
        size_type length = 0;
        for (auto h = head; h != tail; ++h) ++length;
        return length;
    };

    // **************************************************************
    // ***************************修改********************************
    // **************************************************************

    void push_front(const value_type& val) {
        auto node    = newNode(val);
        head.p->prev = node;
        node->next   = head.p;
        head.p       = node;
    }

    void popfront() {
        auto oldNode = head.p;
        head.p       = oldNode->next;
        head.p->prev = nullptr;
        deleteNode(oldNode);
    }

    void push_back(const value_type& val) {
        auto node      = newNode();
        (tail.p)->data = val;
        (tail.p)->next = node;
        node->prev     = tail.p;
        tail.p         = node;
    }

    void popback() {
        auto newTail  = tail.p->prev;
        newTail->next = nullptr;
        deleteNode(tail.p);
        tail.p = newTail;
    }
    iterator insert(iterator position, const value_type& val) {
        if (position == begin()) {
            push_front(val);
            return begin();
        } else if (position == end()) {
            auto ret = position;
            push_back(val);
            return ret;
        }
        auto node        = newNode(val);
        auto prev        = position.p->prev;
        node->next       = position.p;
        node->prev       = prev;
        prev->next       = node;
        position.p->prev = node;
        return iterator(node);
    }

    void insert(iterator position, size_type n, const value_type& val) {
        insert_aux(position, n, val,
                   typename std::is_integral<size_type>::type());
    }

    template <class InputIterator>
    void insert(iterator position, InputIterator first, InputIterator last) {
        insert_aux(position, first, last,
                   typename std::is_integral<InputIterator>::type());
    }
    iterator erase(iterator position) {
        if (position == head) {
            popfront();
            return head;
        } else {
            auto prev              = position.p->prev;
            prev->next             = position.p->next;
            position.p->next->prev = prev;
            deleteNode(position.p);
            return iterator(prev->next);
        }
    }
    iterator erase(iterator first, iterator last) {
        typename list<T>::iterator res;
        for (; first != last;) {
            auto temp = first++;
            res       = erase(temp);
        }
        return res;
    }

    void clear() { erase(begin(), end()); }

    void swap(list& x) {
        std::swap(head.p, x.head.p);
        std::swap(tail.p, x.tail.p);
    }

    // **************************************************************
    // ***************************迭代器********************************
    // **************************************************************
    iterator       begin();
    iterator       end();
    const_iterator begin() const;
    const_iterator end() const;

    // **************************************************************
    // ***************************操作********************************
    // **************************************************************

    void splice(iterator position, list& x) {
        this->insert(position, x.begin(), x.end());
        x.head.p = x.tail.p;
    }

    void splice(iterator position, list& x, iterator first, iterator last) {
        if (first.p == last.p) return;
        auto tailNode = last.p->prev;
        if (x.head.p == first.p) {
            x.head.p       = last.p;
            x.head.p->prev = nullptr;
        } else {
            first.p->prev->next = last.p;
            last.p->prev        = first.p->prev;
        }
        if (position.p == head.p) {
            first.p->prev  = nullptr;
            tailNode->next = head.p;
            head.p->prev   = tailNode;
            head.p         = first.p;
        } else {
            position.p->prev->next = first.p;
            first.p->prev          = position.p->prev;
            tailNode->next         = position.p;
            position.p->prev       = tailNode;
        }
    }

    void splice(iterator position, list& x, iterator i) {
        auto next = i;
        this->splice(position, x, i, ++next);
    }

    void merge(list& x) {
        auto it1 = begin(), it2 = x.begin();
        while (it1 != end() && it2 != x.end()) {
            if (*it1 <= *it2)
                ++it1;
            else {
                auto temp = it2++;
                this->splice(it1, x, temp);
            }
        }
        if (it1 == end()) { this->splice(it1, x, it2, x.end()); }
    }

    template <class Compare>
    void merge(list& x, Compare comp) {
        auto it1 = begin(), it2 = x.begin();
        while (it1 != end() && it2 != x.end()) {
            if (comp(*it2, *it1)) {
                auto temp = it2++;
                this->splice(it1, x, temp);
            } else
                ++it1;
        }
        if (it1 == end()) { this->splice(it1, x, it2, x.end()); }
    }

    void sort() { sort(std::less<T>()); }

    template <class Compare>
    void sort(Compare comp) {
        if (empty() || head.p->next == tail.p) return;
        list carry;
        list counter[64];
        int  fill = 0;
        while (!empty()) {
            carry.splice(carry.begin(), *this, begin());
            int i = 0;
            while (i < fill && !counter[i].empty()) {
                counter[i].merge(carry, comp);
                carry.swap(counter[i++]);
            }
            carry.swap(counter[i]);
            if (i == fill) ++fill;
        }
        for (int i = 0; i != fill; ++i) {
            counter[i].merge(counter[i - 1], comp);
        }
        swap(counter[fill - 1]);
    }

  private:
    void ctorAux(size_type n, const value_type& val, std::true_type) {
        head.p = newNode();    //add a dummy node
        tail.p = head.p;
        while (n--) push_back(val);
    }
    template <class InputIterator>
    void ctorAux(InputIterator first, InputIterator last, std::false_type) {
        head.p = newNode();    //add a dummy node
        tail.p = head.p;
        for (; first != last; ++first) push_back(*first);
    }
    nodePtr newNode(const T& val = T()) {
        nodeAllocator alloc;
        nodePtr       res = alloc.allocate(1);
        alloc.construct(res, detail::node<T>(val, nullptr, nullptr, this));
        return res;
    }
    void deleteNode(nodePtr p) {
        p->prev = p->next = nullptr;
        nodeAllocator::destroy(p);
        nodeAllocator::deallocate(p);
    }

    void insert_aux(iterator position, size_type n, const T& val,
                    std::true_type) {
        for (auto i = n; i != 0; --i) { position = insert(position, val); }
    }
    template <class InputIterator>
    void insert_aux(iterator position, InputIterator first, InputIterator last,
                    std::false_type) {
        for (--last; first != last; --last) {
            position = insert(position, *last);
        }
        insert(position, *last);
    }

    const_iterator changeIteratorToConstIterator(iterator& it) const;

  public:
    friend void swap(list<T>& x, list<T>& y);
    friend bool operator==(const list<T>& lhs, const list<T>& rhs);
    friend bool operator!=(const list<T>& lhs, const list<T>& rhs);
};

template <class T>
bool operator==(const list<T>& lhs, const list<T>& rhs) {
    auto node1 = lhs.head.p, node2 = rhs.head.p;
    for (; node1 != lhs.tail.p && node2 != rhs.tail.p;
         node1 = node1->next, node2 = node2->next) {
        if (node1->data != node2->data) break;
    }
    if (node1 == lhs.tail.p && node2 == rhs.tail.p) return true;
    return false;
}
template <class T>
bool operator!=(const list<T>& lhs, const list<T>& rhs) {
    return !(lhs == rhs);
}

namespace detail {

// 链表节点结构
template <class T>
struct node {
    T        data;
    node*    prev;
    node*    next;
    list<T>* container;
    node(const T& d, node* p, node* n, list<T>* c)
            : data(d), prev(p), next(n), container(c) {}
    bool operator==(const node& n) {
        return data == n.data && prev == n.prev && next == n.next
               && container == n.container;
    }
};

// list迭代器
template <class T>
struct listIterator : public std::iterator<std::bidirectional_iterator_tag, T> {
    using nodePtr = node<T>*;

    nodePtr p;

    explicit listIterator(nodePtr ptr = nullptr) : p(ptr) {}

    listIterator& operator++() {
        p = p->next;
        return *this;
    };
    listIterator operator++(int) {
        auto res = *this;
        ++*this;
        return res;
    }
    listIterator& operator--() {
        p = p->prev;
        return *this;
    }
    listIterator operator--(int) {
        auto res = *this;
        --*this;
        return res;
    }
    bool operator==(const listIterator<T>& other) { return p == other.p; }
    bool operator!=(const listIterator<T>& other) { return !(*this == other); }

    T& operator*() { return p->data; }
    T* operator->() { return &(operator*()); }
};
}    // namespace detail
}    // namespace extrastl

template <class T>
bool operator==(const extrastl::detail::listIterator<T>& lhs,
                const extrastl::detail::listIterator<T>& rhs) {
    return lhs.operator==(rhs);
}

template <class T>
bool operator!=(const extrastl::detail::listIterator<T>& lhs,
                const extrastl::detail::listIterator<T>& rhs) {
    return !(lhs == rhs);
}

#endif