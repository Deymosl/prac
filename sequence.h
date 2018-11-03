#include <type_traits>
#include <cstdint>
#include <memory.h>
#include <new>
#include <iterator>
#include <stdexcept>
#include <algorithm>
#include <cstring>

struct seq_traits {
    using type = char;

    static constexpr bool eq(type a, type b) noexcept {
        return a == b;
    };

    static constexpr bool lt(type a, type b) noexcept {
        return a < b;
    };

    static constexpr void assign(type &r, const type &a) {
        r = a;
    };

    static constexpr std::size_t length(const type *s) {
        return strlen(s);
    };
};

template<class T, class Traits>
struct sequence {
    using size_type = std::size_t;
    using const_iterator = const T *;
    using const_reference = const T &;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;
    static constexpr size_type npos = size_type(-1);

    constexpr sequence() noexcept : st(nullptr), sz(0) {};

    constexpr sequence(const sequence &other) noexcept : st(other.begin()), sz(other.size()) { };

    constexpr sequence(const T *s, size_type count) : st(s),  sz(count) { };

    constexpr sequence(const T *s) : st(s), sz(Traits::length(s)) { };

    constexpr sequence &operator=(const sequence &view) noexcept {
        sz = view.sz;
        st = view.st;
        return *this;
    };

    constexpr const_iterator begin() const noexcept {
        return st;
    };

    constexpr const_iterator end() const noexcept {
        return st + sz;
    };

    constexpr const_reverse_iterator rbegin() const noexcept {
        return std::make_reverse_iterator(st + sz);

    };

    constexpr const_reverse_iterator rend() const noexcept {
        return std::make_reverse_iterator(st);
    };

    constexpr const_reference operator[](size_type pos) const {
        return st[pos];
    };

    constexpr const_reference at(size_type pos) const {
        if (pos >= sz)
            throw std::out_of_range("Out of range");
        return st[pos];
    }; //throws std::out_of_range
    constexpr const_reference front() const {
        return *st;
    };

    constexpr const_reference back() const {
        return st[sz - 1];
    };

    constexpr size_type size() const noexcept {
        return sz;
    };

    constexpr bool empty() const noexcept {
        return sz == 0;
    };

    constexpr void remove_prefix(size_type n) {
        if (n >= sz) {
            sz = 0;
        } else {
            st = st + n;
            sz -= n;
        }
    };

    constexpr void remove_suffix(size_type n) {
        if (n >= sz) {
            sz = 0;
        } else {
            sz -= n;
        }
    };

    constexpr void swap(sequence &v) noexcept {
        const T* temp = v.st;
        v.st = st;
        st = temp;
        size_t tt = v.sz;
        v.sz = sz;
        sz = tt;
    };

    //copies no more than count characters, return number of copyed elements
    size_type copy(T *dest, size_type count, size_type pos = 0) const {
        if (pos >= sz)
            throw std::out_of_range("out of ranggee");
        size_t min = sz < count ? sz : count;
        for (size_t f = 0; f < min; f++) {
            Traits::assign(*(dest + f), *(st + f));
        }
        return min;
    }; //throws std::out_of_range

    constexpr int compare(sequence v) const noexcept {
        auto l1 = begin(), l2 = v.begin();
        for (size_t i = 0 ; i < sz, i < v.size(); i++) {
            if (Traits::eq(*(l1 + i), *(l2 + i)))
                continue;
            if (Traits::lt(*(l1 + i), *(l2+i))) {
                return -1;
            } else if (Traits::lt(*(l2+i), *(l1 + i))) {
                return 1;
            }
        }
        if (sz > v.size()) {
            return 1;
        } else if (sz < v.size()) {
            return -1;
        } else return 0;
    };

    constexpr sequence subseq(size_type pos = 0, size_type count = npos) const {
        if (pos >= sz)
            throw std::out_of_range("sequence subseq :(");
        if (count >= sz)
            count = sz;
        if (pos + count >= sz)
            count = sz - pos;
        return sequence(begin() + pos, count);
    }; //throws std::out_of_range

    constexpr size_type find(sequence v, size_type pos = 0) const noexcept {
        if (v.size() > sz)
            return npos;
        while (pos + v.size() <= sz) {
            if (subseq(pos, v.size()).compare(v) == 0)
                return pos;
            pos++;
        }
        return npos;
    };

    constexpr size_type rfind(sequence v, size_type pos = npos) const noexcept {
        if (v.size() > sz)
            return npos;
        if (pos > sz - v.sz) {
            pos = sz - v.sz;
        }
        while (pos > 0) {
            if (subseq(pos, v.size()).compare(v) == 0)
                return pos;
            pos--;
        }
        if (subseq(pos, v.size()).compare(v) == 0)
            return pos;
        return npos;
    };

    template<class T2, class Traits2>
    friend constexpr bool operator==(sequence<T2, Traits2> lhs, sequence<T2, Traits2> rhs) noexcept;

    template<class T2, class Traits2>
    friend constexpr bool operator!=(sequence<T2, Traits2> lhs, sequence<T2, Traits2> rhs) noexcept;

    template<class T2, class Traits2>
    friend constexpr bool operator<(sequence<T2, Traits2> lhs, sequence<T2, Traits2> rhs) noexcept;

    template<class T2, class Traits2>
    friend constexpr inline bool operator>(sequence<T2, Traits2> lhs, sequence<T2, Traits2> rhs) noexcept;

    template<class T2, class Traits2>
    friend constexpr inline bool operator>=(sequence<T2, Traits2> lhs, sequence<T2, Traits2> rhs) noexcept;

    template<class T2, class Traits2>
    friend constexpr bool operator<=(sequence<T2, Traits2> lhs, sequence<T2, Traits2> rhs) noexcept;
    // ==, !=, <, >, <=, >=

private:
    const_iterator *st;
    size_t sz;

    void copy_construct(T *dest, const T *src, size_t size) const {
        if (size != 0)
            memcpy(dest, src, size * sizeof(T));
    }
};

template<class T2, class Traits2>
constexpr bool operator==(sequence<T2, Traits2> lhs, sequence<T2, Traits2> rhs) noexcept {
    return lhs.compare(rhs) == 0;
}

template<class T2, class Traits2>
constexpr bool operator!=(sequence<T2, Traits2> lhs, sequence<T2, Traits2> rhs) noexcept {
    return lhs.compare(rhs) != 0;
}

template<class T2, class Traits2>
constexpr bool operator<(sequence<T2, Traits2> lhs, sequence<T2, Traits2> rhs) noexcept{
    return lhs.compare(rhs) < 0;
}

template<class T2, class Traits2>
constexpr bool operator>(sequence<T2, Traits2> lhs, sequence<T2, Traits2> rhs) noexcept {
    return lhs.compare(rhs) > 0;
}

template<class T2, class Traits2>
constexpr bool operator>=(sequence<T2, Traits2> lhs, sequence<T2, Traits2> rhs) noexcept {
    return lhs.compare(rhs) >= 0;
}

template<class T2, class Traits2>
constexpr bool operator<=(sequence<T2, Traits2> lhs, sequence<T2, Traits2> rhs) noexcept {
    return lhs.compare(rhs);
}

namespace std {
    template<class T, class Traits>
    struct hash<sequence<T, Traits>> {
        std::size_t operator()(const sequence<T, Traits> &s) const noexcept {
            size_t res = 0;
            for (size_t sq = 0; sq < s.size(); sq++) {
                res += s[sq];
            }
            return res;
        };
    };
}