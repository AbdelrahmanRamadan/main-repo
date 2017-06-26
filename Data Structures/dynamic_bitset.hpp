#ifndef DYNAMIC_BITSET_HPP
#define DYNAMIC_BITSET_HPP

#include <vector>
#include <string>
#include <cmath>
#include <memory>
#include <iostream>
//#include <iterator>
#include "C++ Helpers/static_log2.hpp"

using namespace std;
template <class T = unsigned long long, class allocator_type = std::allocator<T> >
class dynamic_bitset {
    static_assert(is_integral<T>::value, "Invalid block_type! Please use integral type.");
public:
    class proxy;
    class iterator;
    class const_iterator;

    typedef T block_type;
    typedef typename allocator_traits<allocator_type>::reference block_reference;
    typedef typename allocator_traits<allocator_type>::const_reference block_const_reference;
    typedef typename allocator_traits<allocator_type>::difference_type block_difference_type;
    typedef typename allocator_traits<allocator_type>::size_type block_size_type;

    typedef vector<block_type, allocator_type> container_type;
    typedef typename container_type::iterator block_iterator;
    typedef typename container_type::const_iterator block_const_iterator;
    typedef typename container_type::reverse_iterator block_reverse_iterator;
    typedef typename container_type::const_reverse_iterator block_const_reverse_iterator;

    typedef bool value_type;
    typedef block_difference_type difference_type;
    typedef block_size_type size_type;
    typedef proxy* pointer;
    typedef const proxy* const_pointer;
    typedef proxy reference;
    typedef bool const_reference;

    typedef std::reverse_iterator<iterator> reverse_iterator;
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

    static const size_type block_bits = sizeof (block_type) << 3;
    static const int block_log = static_log2<block_bits>::value;
    static const int npos = -1;

    explicit dynamic_bitset() : bits_size(0) {}
    explicit dynamic_bitset(size_type ubits_size, bool val = false) : bits_size(ubits_size) ,blocks(container_type ((bits_size + block_bits - 1) >> block_log, val)) {
        if(val) fix_last_block();
    }
    explicit dynamic_bitset(const string& digits) {
        // could be binary of hexadecimal digits
        if(digits.size() > 1 && (digits[1] == 'x' || digits[1] == 'X'))
            init_hexa(digits);
        else
            init_binary(digits);
    }

    size_type size();
    void resize(size_type usize, bool val = false) {
        size_type bit_pos;
        if(bits_size < usize) {
            bit_pos = bits_size & (block_bits - 1);
            if(bit_pos) {
                if(val)
                    blocks.back() |= ~(((block_type)1 << bit_pos) - 1);
                else
                    blocks.back() &= (((block_type)1 << bit_pos) - 1);
            }
        }

        bits_size = usize;
        blocks.resize((bits_size + block_bits - 1) >> block_log, val);

        fix_last_block();
    }
    void push_back(bool val) {
        size_type bit_pos = bit_index(bits_size++);
        if(!bit_pos)
            blocks.push_back(val);
        else if(val)
            blocks.back() |= bit_mask(bit_pos);

    }
    void pop_back() {
        size_type bit_pos = bit_index(--bits_size);
        if(!bit_pos)
            blocks.pop_back();
        else
            blocks.back() &= ~bit_mask(bit_pos);
    }

    void clear() {
        bits_size = 0;
        blocks.clear();
    }
    unsigned int to_uint() const {
        return bits_size ? blocks[0] : 0;
    }
    unsigned long long to_ullong() const {
        return bits_size ? blocks[0] : 0;
    }
    string to_binary_string() const {
        string str(bits_size, '0');
        for(size_type i = 0; i < bits_size; ++i)
            str[bits_size - i - 1] = test(i) + '0';
        return str;
    }
    string to_hexa_string() const {
        string str(((bits_size + 3) >> 2) + 2, '0');
        for(size_type i = 0, j = str.size() - 1; i < bits_size; --j, i += 4)
            str[j] = val_hexa(test(i) + (test(i + 1) << 1) + (test(i + 2) << 2) + (test(i + 3) << 3));
        str[0] = '0';
        str[1] = 'x';
        return str;
    }
    string to_string() const {
        return to_binary_string();
    }

    void set() {
        for(block_iterator it = blocks.begin(); it != blocks.end(); ++it)
            *it = all_on;
        fix_last_block();
    }
    void set(size_type pos, bool val = true) {
        if(val)
            blocks[block_index(pos)] |= bit_mask(bit_index(pos));
        else
            blocks[block_index(pos)] &= ~bit_mask(bit_index(pos));
    }
    void reset() {
        for(block_iterator it = blocks.begin(); it != blocks.end(); ++it)
            *it = 0;
    }
    void reset(size_type pos) {
        off(pos);
    }
    void on(size_type pos) {
        blocks[block_index(pos)] |= bit_mask(bit_index(pos));
    }
    void off(size_type pos) {
        blocks[block_index(pos)] &= ~bit_mask(bit_index(pos));
    }
    void flip(size_type pos) {
        blocks[block_index(pos)] ^= bit_mask(bit_index(pos));
    }
    bool test(size_type pos) const {
        return (blocks[block_index(pos)] >> bit_index(pos) ) & 1;
    }

    size_type block_pop_count(const block_type& block) const {
        return (size_type)__builtin_popcountll(block);
    }
    size_type count(bool val = true) const {
        int ret = 0;
        for(size_type i = 0, length = blocks.size(); i < length; ++i)
            ret += block_pop_count(blocks[i]);
        return val ? ret : (bits_size - ret);
    }
    bool any() const {
        bool ret = false;
        for(size_type i = 0, length = blocks.size(); i < length; ++i)
            ret |= blocks[i];
        return ret;
    }
    bool none() const {
        return !any();
    }

    bool operator [] (size_type pos) const {
        return (blocks[block_index(pos)] >> bit_index(pos)) & 1;
    }
    reference operator [] (size_type pos) {
        return reference(*this, pos);
    }

    operator bool() const {
        return any();
    }
    dynamic_bitset operator ~ () const {
        dynamic_bitset ret = *this;
        for(size_type i = 0, length = blocks.size(); i < length; ++i)
            ret.blocks[i] = ~blocks[i];
        ret.fix_last_block();
        return ret;
    }
    dynamic_bitset& operator >>= (int shift_count) {
        int blocks_count = block_index(shift_count);
        int bits_count = bit_index(shift_count);

        for(size_type i = blocks_count, length = blocks.size(); i < length; ++i)
            blocks[i - blocks_count] = blocks[i];
        fill(blocks.begin() + max(0, (int)blocks.size() - blocks_count), blocks.end(), 0);

        if(bits_count && bits_size) {
            blocks[0] >>= bits_count;
            for(size_type i = 1, length = blocks.size(); i < length; ++i) {
                blocks[i - 1] |= blocks[i] << (block_bits - bits_count);
                blocks[i] >>= bits_count;
            }
        }
        return *this;
    }
    dynamic_bitset& operator <<= (int shift_count) {

        int blocks_count = block_index(shift_count);
        int bits_count = bit_index(shift_count);

        for(int i = blocks.size() - 1; i >= blocks_count; --i)
            blocks[i] = blocks[i - blocks_count];
        fill(blocks.begin(), blocks.begin() + min<size_type>(blocks.size(), blocks_count), 0);

        if(bits_count && bits_size) {
            blocks.back() <<= bits_count;
            for(int i = blocks.size() - 2; i >= 0; --i) {
                blocks[i + 1] |= blocks[i] >> (block_bits - bits_count);
                blocks[i] <<= bits_count;
            }
        }
        return *this;
    }
    dynamic_bitset& operator ^= (const dynamic_bitset& other) {
        for(size_type i = 0, length = min(blocks.size(), other.blocks.size()); i < length; ++i)
            blocks[i] ^= other.blocks[i];
        return *this;
    }
    dynamic_bitset& operator &= (const dynamic_bitset& other) {
        for(size_type i = 0, length = min(blocks.size(), other.blocks.size()); i < length; ++i)
            blocks[i] &= other.blocks[i];
        return *this;
    }
    dynamic_bitset& operator |= (const dynamic_bitset& other) {
        for(size_type i = 0, length = min(blocks.size(), other.blocks.size()); i < length; ++i)
            blocks[i] |= other.blocks[i];
        return *this;
    }

    //template <typename enable_if<sizeof(block_type) >= sizeof (U)>::type
    dynamic_bitset& operator ^= (const block_type& other) {
        if(bits_size) {
            block_type mask = other;
            if(bits_size < block_bits)
                mask &= bit_mask(bits_size) - 1;
            blocks[0] ^= mask;
        }
        return *this;
    }
    dynamic_bitset& operator &= (const block_type& other) {
        if(bits_size) blocks[0] &= other;
        return *this;
    }
    dynamic_bitset& operator |= (const block_type& other) {
        if(bits_size) {
            block_type mask = other;
            if(bits_size < block_bits)
                mask &= bit_mask(bits_size) - 1;
            blocks[0] |= mask;
        }
        return *this;
    }

    dynamic_bitset operator >> (int shift_count) const {
        return dynamic_bitset(*this) >>= shift_count;
    }
    dynamic_bitset operator << (int shift_count) const {
        return dynamic_bitset(*this) <<= shift_count;
    }

    template<class U> dynamic_bitset operator ^ (const U& other) const {
        return dynamic_bitset(*this) ^= other;
    }
    template<class U> dynamic_bitset operator & (const U& other) const {
        return dynamic_bitset(*this) &= other;
    }
    template<class U> dynamic_bitset operator | (const U& other) const {
        return dynamic_bitset(*this) |= other;
    }


    class iterator {
    public:
        typedef dynamic_bitset::value_type value_type;
        typedef dynamic_bitset::difference_type difference_type;
        typedef dynamic_bitset::reference reference;
        typedef dynamic_bitset::pointer pointer;
        typedef std::random_access_iterator_tag iterator_category;

        iterator () : data() {}

        bool operator == (const iterator& other) const {
            return data.block_it == other.data.block_it && data.pos == other.data.pos;
        }
        bool operator != (const iterator& other) const {
            return !(*this == other);
        }
        bool operator < (const iterator& other) const {
            return data.block_it < other.data.block_it || (data.block_it == other.data.block_it && data.pos < other.data.pos);
        }
        bool operator > (const iterator& other) const {
            return other < *this;
        }
        bool operator <= (const iterator& other) const {
            return *this < other || *this == other;
        }
        bool operator >= (const iterator& other) const {
            return *this > other || *this == other;
        }

        iterator& operator ++ () {
            data.pos = bit_index(data.pos + 1);
            if(!data.pos)
                ++data.block_it;
            return *this;
        }
        iterator operator ++ (int) {
            iterator ret(*this);
            ++(*this);
            return ret;
        }
        iterator& operator -- () {
            data.pos = bit_index(data.pos - 1);
            if(data.pos == block_bits - 1)
                --data.block_it;
            return *this;
        }
        iterator operator -- (int) {
            iterator ret(*this);
            --(*this);
            return ret;
        }
        iterator& operator += (size_type val) {
            data.block_it += block_index(data.pos + val);
            data.pos = bit_index(data.pos + val);
            return *this;
        }
        iterator operator + (size_type val) const {
            return iterator(*this) += val;
        }
        friend iterator operator + (size_type val, const iterator& other) {
            return other + val;
        }
        iterator& operator -= (size_type val) {
            int diff = val - data.pos - 1;
            if(diff >= 0)
                data.block_it -= block_index(diff) + 1;
            data.pos = bit_index(data.pos - val);
        }
        iterator operator - (size_type val) const {
            return iterator(*this) -= val;
        }
        difference_type operator-(iterator it) const {
            return ((data.block_it - it.data) * block_bits) + (data.pos - it.data.pos);
        }
        proxy& operator*() const {
            return data;
        }
        pointer operator->() const {
            return &data;
        }

    private:
        proxy data;
        explicit iterator (const proxy& ref) : data(ref) {}
    };

    class proxy {
        friend class dynamic_bitset::iterator;
    public:
        proxy(const proxy& other) : block_it(other.block_it), pos(other.pos) {}

        operator bool() const {
            return *block_it & bit_mask(pos);
        }
        proxy& operator = (bool val) {
            if(val)
                *block_it |= bit_mask(pos);
            else
                *block_it &= ~bit_mask(pos);
            return *this;
        }
        proxy& operator = (const proxy& that) {
            *this = (bool)that;
        }
        proxy& operator ^= (bool val) {
            if(val) *block_it ^= bit_mask(pos);
            return *this;
        }
        proxy& operator &= (bool val) {
            if(!val) *block_it &= ~bit_mask(pos);
            return *this;
        }
        proxy& operator |= (bool val) {
            if(val) *block_it |= bit_mask(pos);
            return *this;
        }

    private:
        block_iterator block_it;
        size_type pos;
        bool val;
        explicit proxy () : block_it(block_iterator()), pos(0) {}
        explicit proxy(const dynamic_bitset& ref, size_type _pos) : block_it(ref.blocks.begin() + block_index(_pos)), pos(bit_index(_pos)) {}
        explicit proxy(block_iterator it, size_type _pos) : block_it(it), pos(_pos) {}
    };


private:
    static const block_type all_off = block_type(0);
    static const block_type all_on = ~block_type(0);

    size_type bits_size;
    vector<block_type, allocator_type> blocks;


    char hexa_val(char c) {
        return c <= '9' ? (c - '0') : c >= 'a' ? (c - 'a' + 10) : (c - 'A' + 10);
    }
    char val_hexa(char c) {
        return c < 10 ? (c + '0') : (c - 10 + 'a');
    }
    block_type extract_binary_block(const string& digits, string::size_type& remaining) {
        block_type block = 0;
        size_type limit = min<size_type> (block_bits, remaining);
        for(size_type i = 0; i < limit; ++i)
            block |= bit_mask(digits[--remaining] - '0', i);
        return block;
    }
    block_type extract_hexa_block(const string& digits, string::size_type& remaining) {
        block_type block = 0;
        size_type limit = min<size_type> (block_bits >> 2, remaining - 2);
        for(size_type i = 0; i < limit; ++i)
            block |= bit_mask(hexa_val(digits[--remaining]), i << 2);
        return block;
    }
    void init_hexa(const string& digits) {
        bits_size = (digits.size() - 2) << 2;
        blocks = container_type ((bits_size + block_bits - 1) >> block_log);
        string::size_type remaining = digits.size();
        for(block_iterator it = blocks.begin(); remaining > 2; ++it)
            *it = extract_hexa_block(digits, remaining);
    }
    void init_binary(const string& digits) {
        bits_size = digits.size();
        blocks = container_type ((bits_size + block_bits - 1) >> block_log);
        string::size_type remaining = digits.size();
        for(block_iterator it = blocks.begin(); remaining; ++it)
            *it = extract_binary_block(digits, remaining);
    }

    void fix_last_block() {
        size_type last_block = bits_size & (block_bits - 1);
        if(last_block)
            blocks.back() &= bit_mask(last_block) - 1;
    }


    static block_size_type block_index (const size_type& pos) {
        return pos >> block_log;
    }
    static size_type bit_index (size_type pos) {
        return pos & (block_bits - 1);
    }
    static block_type bit_mask (size_type pos) {
        return block_type(1) << pos;
    }

};

// inline methods



/* needs testing
 * operators ^= |= &= with block_type need to specialized for types > block_type
 * out of range bits which occure in the last block are gurenteed to be 0
 *
 *
 *
 *
 *
 *
 *
 *
 *
 */

#endif // DYNAMIC_BITSET_HPP
