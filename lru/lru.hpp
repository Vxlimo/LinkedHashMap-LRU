#ifndef SJTU_LRU_HPP
#define SJTU_LRU_HPP

#include "class-integer.hpp"
#include "class-matrix.hpp"
#include "exceptions.hpp"
#include "utility.hpp"
#include <cassert>

class Hash {
public:
    unsigned int operator()(Integer lhs) const
    {
        int val = lhs.val;
        return std::hash<int>()(val);
    }
};
class Equal {
public:
    bool operator()(const Integer& lhs, const Integer& rhs) const
    {
        return lhs.val == rhs.val;
    }
};

namespace sjtu {
template <class T>
class double_list {
public:
    /**
     * the nodes of double_list
     */
    struct Node {
        T* val;
        Node *pre, *nxt;
        Node(T* val, Node* pre = nullptr, Node* nxt = nullptr)
            : val(val)
            , pre(pre)
            , nxt(nxt)
        {
        }
        ~Node()
        {
            if (val != nullptr)
                delete val;
        }
    };
    Node *head, *tail;

    /**
     * constructors and destructors
     * here we denote tail as a node points to nothing
     */
    double_list()
    {
        head = tail = new Node(nullptr);
    }
    double_list(const double_list<T>& other)
    {
        head = tail = new Node(nullptr);
        Node* temp = other.head;
        while (temp != other.tail) {
            insert_tail(*(temp->val));
            temp = temp->nxt;
        }
    }
    ~double_list()
    {
        destroy();
    }
    double_list& operator=(const double_list& other)
    {
        if (this == &other)
            return *this;
        destroy();
        head = tail = new Node(nullptr);
        Node* temp = other.head;
        while (temp != other.tail) {
            insert_tail(*(temp->val));
            temp = temp->nxt;
        }
        return *this;
    }

    /**
     * the iterator of double_list
     * directly point to the Node of double_list
     */
    class iterator {
    public:
        Node* p;
        iterator(Node* p = nullptr)
            : p(p)
        {
        }
        iterator(const iterator& t)
        {
            p = t.p;
        }
        ~iterator()
        {
            p = nullptr;
        }
        Node* at()
        {
            return p;
        }
        /**
         * iter++
         */
        iterator operator++(int)
        {
            if (p == nullptr || p->nxt == nullptr)
                throw invalid_iterator();
            Node* temp = p;
            p = p->nxt;
            return iterator(temp);
        }
        /**
         * ++iter
         */
        iterator& operator++()
        {
            if (p == nullptr || p->nxt == nullptr)
                throw invalid_iterator();
            p = p->nxt;
            return *this;
        }
        /**
         * iter--
         */
        iterator operator--(int)
        {
            if (p == nullptr || p->pre == nullptr)
                throw invalid_iterator();
            Node* temp = p;
            p = p->pre;
            return iterator(temp);
        }
        /**
         * --iter
         */
        iterator& operator--()
        {
            if (p == nullptr || p->pre == nullptr)
                throw invalid_iterator();
            p = p->pre;
            return *this;
        }
        /**
         * if the iter didn't point to a value
         * throw
         */
        T& operator*() const
        {
            if (p == nullptr || p->nxt == nullptr)
                throw invalid_iterator();
            return *(p->val);
        }
        T* operator->() const noexcept
        {
            if (p == nullptr || p->nxt == nullptr)
                throw invalid_iterator();
            return p->val;
        }
        /**
         * other operation
         */
        bool operator==(const iterator& rhs) const
        {
            return p == rhs.p;
        }
        bool operator!=(const iterator& rhs) const
        {
            return p != rhs.p;
        }
    };

    /**
     * return an iterator point at the beginning
     * const or not
     */
    iterator begin()
    {
        return iterator(head);
    }
    iterator begin() const
    {
        return iterator(head);
    }
    /**
     * return an iterator point at the last element
     * maybe not exist
     */
    iterator last()
    {
        if (tail->pre == nullptr)
            throw invalid_iterator();
        return iterator(tail->pre);
    }
    /**
     * return an iterator point at the ending
     * const or not
     */
    iterator end()
    {
        return iterator(tail);
    }
    iterator end() const
    {
        return iterator(tail);
    }

    /**
     * if didn't contain anything, return true,
     * otherwise false
     */
    bool empty() const
    {
        return head != tail;
    }
    /**
     * destroy the whole double_list
     * can't be used after destroy
     */
    void destroy()
    {
        Node* temp = head;
        while (temp != tail) {
            Node* temp2 = temp->nxt;
            delete temp;
            temp = temp2;
        }
        delete tail;
        return;
    }
    /**
     * clear the whole double_list
     * can be used after clear
     */
    void clear()
    {
        destroy();
        head = tail = new Node(nullptr);
        return;
    }

    /**
     * erase the element at iterator pos
     * if the iter didn't point to anything, do nothing
     * otherwise, delete the element pointed by the iter
     * and return the iterator point at the same "index"
     * e.g.
     * 	if the origin iterator point at the 2nd element
     * 	the returned iterator also point at the
     *  2nd element of the list after the operation
     *  or the tail if the list after the operation
     *  don't contain 2nd elememt.
     */
    iterator erase(iterator pos)
    {
        if (pos == iterator(head)) {
            delete_head();
            return iterator(head);
        } else {
            Node* cur = pos.p;
            cur->pre->nxt = cur->nxt;
            cur->nxt->pre = cur->pre;
            pos.p = cur->nxt;
            delete cur;
            return pos;
        }
    }
    /**
     * insert an element at the head of the list
     */
    void insert_head(const T& val)
    {
        Node* cur = new Node(new T(val));
        if (head == tail) {
            cur->nxt = head;
            cur->nxt->pre = cur;
            head = cur;
        } else {
            cur->nxt = head;
            cur->nxt->pre = cur;
            head = cur;
        }
        return;
    }
    /**
     * insert an element at the tail of the list
     */
    void insert_tail(const T& val)
    {
        Node* cur = new Node(new T(val));
        if (head == tail) {
            cur->nxt = tail;
            cur->nxt->pre = cur;
            head = cur;
        } else {
            cur->nxt = tail;
            cur->pre = tail->pre;
            cur->nxt->pre = cur;
            cur->pre->nxt = cur;
        }
        return;
    }
    /**
     * delete the head of the list
     */
    void delete_head()
    {
        if (head == tail)
            throw container_is_empty();
        Node* cur = head;
        if (cur->nxt == tail) {
            cur->nxt->pre = nullptr;
            delete cur;
            head = tail;
        } else {
            Node* tmp = cur->nxt;
            cur->nxt->pre = nullptr;
            delete cur;
            head = tmp;
        }
        return;
    }
    /**
     * delete the tail of the list
     */
    void delete_tail()
    {
        if (head == tail)
            throw container_is_empty();
        Node* cur = tail->pre;
        if (head->nxt == tail) {
            cur->nxt->pre = nullptr;
            delete cur;
            head = tail;
        } else {
            cur->nxt->pre = cur->pre;
            cur->pre->nxt = cur->nxt;
            delete cur;
        }
        return;
    }
};

template <
    class Key,
    class T,
    class Hash = std::hash<Key>,
    class Equal = std::equal_to<Key>>
class hashmap {
public:
    using value_type = pair<const Key, T>;
    using list = double_list<value_type>;
    /**
     * possible size of the hashmap
     */
    static constexpr size_t primes[25] = { 7, 17, 31, 61, 127, 257, 509, 1021, 2053, 4093, 8191, 16381, 32771, 65537, 131071, 262147, 524287, 1048573, 2097143, 4194301, 8388617, 16777213, 33554467, 67108859 };
    /**
     * the current capacity of the hashmap (primes[capacity])
     * the current element number of the hashmap
     */
    size_t capacity, elements;
    /**
     * the hashtable
     */
    list** table;

    /**
     *  constructors and destructors
     */
    hashmap()
    {
        capacity = elements = 0;
        table = new list*[primes[capacity]]();
    }
    hashmap(const hashmap& other)
    {
        capacity = other.capacity;
        elements = other.elements;
        table = new list*[primes[capacity]]();
        for (size_t i = 0; i < primes[capacity]; i++) {
            if (other.table[i] != nullptr)
                table[i] = new list(*other.table[i]);
        }
    }
    ~hashmap()
    {
        destroy();
    }
    hashmap& operator=(const hashmap& other)
    {
        if (this == &other)
            return *this;
        destroy();
        capacity = other.capacity;
        elements = other.elements;
        table = new list*[primes[capacity]]();
        for (size_t i = 0; i < primes[capacity]; i++) {
            if (other.table[i] != nullptr)
                table[i] = new list(*other.table[i]);
        }
        return *this;
    }

    /**
     * the iterator of hashmap
     * point to the Node in every list
     */
    class iterator {
    public:
        using Node = list::Node;
        Node* p;
        iterator(Node* p = nullptr)
            : p(p)
        {
        }
        iterator(const iterator& t)
        {
            p = t.p;
        }
        ~iterator()
        {
            p = nullptr;
        }
        Node* at() const
        {
            return p;
        }
        /**
         * if the iter didn't point to a value
         * throw
         */
        value_type& operator*() const
        {
            if (p == nullptr)
                throw invalid_iterator();
            return *(p->val);
        }
        value_type* operator->() const noexcept
        {
            if (p == nullptr)
                throw invalid_iterator();
            return p->val;
        }
        /**
         * other operation
         */
        bool operator==(const iterator& rhs) const
        {
            return p == rhs.p;
        }
        bool operator!=(const iterator& rhs) const
        {
            return p != rhs.p;
        }
    };

    /**
     * return an iterator point at nothing
     */
    iterator end() const
    {
        return iterator(nullptr);
    }

    /**
     * if didn't contain anything, return true,
     * otherwise false
     */
    bool empty() const
    {
        return elements == 0;
    }
    /**
     * destroy the whole hashmap
     * can't be used after destroy
     */
    void destroy()
    {
        for (size_t i = 0; i < primes[capacity]; i++) {
            if (table[i] != nullptr)
                delete table[i];
        }
        delete[] table;
        capacity = elements = 0;
        return;
    }
    /**
     * clear the whole hashmap
     * can be used after clear
     */
    void clear()
    {
        destroy();
        capacity = elements = 0;
        table = new list*[primes[capacity]]();
        return;
    }
    /**
     * if the capacity is lower than the number of elements, expand it
     */
    void expand()
    {
        if (capacity == 24)
            throw index_out_of_bound();
        list** new_table = new list*[primes[capacity + 1]]();
        for (size_t i = 0; i < primes[capacity]; i++) {
            if (table[i] != nullptr) {
                for (auto [key, val] : *table[i]) {
                    size_t pos = Hash()(key) % primes[capacity + 1];
                    if (new_table[pos] == nullptr)
                        new_table[pos] = new list;
                    new_table[pos]->insert_tail(value_type(key, val));
                }
                delete table[i];
            }
        }
        delete[] table;
        table = new_table;
        capacity++;
        return;
    }

    /**
     * find the key
     * find: return a pointer point to the value
     * not find: return the end (point to nothing)
     */
    iterator find(const Key& key) const
    {
        size_t pos = Hash()(key) % primes[capacity];
        if (table[pos] == nullptr)
            return end();
        for (auto it = table[pos]->begin(); it != table[pos]->end(); it++) {
            if (Equal()(it->first, key))
                return iterator(it.at());
        }
        return end();
    }
    /**
     * insert a new key
     * already have a value_pair with the same key:
     * just update the value, return false
     * not find a value_pair with the same key:
     * insert the value_pair, return true
     */
    sjtu::pair<iterator, bool> insert(const value_type& value_pair)
    {
        auto it = find(value_pair.first);
        if (it == end()) {
            while (elements >= primes[capacity])
                expand();
            size_t pos = Hash()(value_pair.first) % primes[capacity];
            if (table[pos] == nullptr)
                table[pos] = new list;
            table[pos]->insert_head(value_pair);
            elements++;
            return sjtu::pair<iterator, bool>(iterator(table[pos]->tail->pre), true);
        } else {
            it->second = value_pair.second;
            return sjtu::pair<iterator, bool>(it, false);
        }
    }
    /**
     * remove a key
     * the value_pair exists: remove and return true
     * otherwise: return false
     */
    bool remove(const Key& key)
    {
        auto it = find(key);
        if (it == end())
            return false;
        size_t pos = Hash()(key) % primes[capacity];
        table[pos]->erase(typename list::iterator(it.at()));
        elements--;
        return true;
    }
};

template <
    class Key,
    class T,
    class Hash = std::hash<Key>,
    class Equal = std::equal_to<Key>>
class linked_hashmap {
public:
    using value_type = pair<const Key, T>;
    using list_type = double_list<value_type>;
    using list_it = typename double_list<value_type>::iterator;
    using map_type = hashmap<Key, list_it, Hash, Equal>;
    using map_it = typename map_type::iterator;
    /**
     * hashmap: store the key and the iterator to the list
     * list: store the value_pair and the order
     */
    map_type map;
    list_type list;

    /**
     *  constructors and destructors
     */
    linked_hashmap() = default;
    linked_hashmap(const linked_hashmap& other)
    {
        map = other.map;
        list = other.list;
    }
    ~linked_hashmap() = default;
    linked_hashmap& operator=(const linked_hashmap& other)
    {
        if (this == &other)
            return *this;
        map = other.map;
        list = other.list;
        return *this;
    }

    /**
     * the iterator and const_iterator of linked_hashmap
     * point to the Node in the list
     */
    class const_iterator;
    class iterator {
    public:
        list_it p;
        iterator(list_it p = list_it(nullptr))
            : p(p)
        {
        }
        iterator(const iterator& other)
        {
            p = other.p;
        }
        ~iterator()
        {
            p = list_it(nullptr);
        }

        /**
         * iter++
         */
        iterator operator++(int)
        {
            iterator temp = *this;
            p++;
            return temp;
        }
        /**
         * ++iter
         */
        iterator& operator++()
        {
            ++p;
            return *this;
        }
        /**
         * iter--
         */
        iterator operator--(int)
        {
            iterator temp = *this;
            p--;
            return temp;
        }
        /**
         * --iter
         */
        iterator& operator--()
        {
            --p;
            return *this;
        }
        /**
         * if the iter didn't point to a value
         * throw
         */
        value_type& operator*() const
        {
            if (p == list_it(nullptr) || (*(p.p)).val == nullptr)
                throw invalid_iterator();
            return *((p.p)->val);
        }
        value_type* operator->() const noexcept
        {
            if (p == list_it(nullptr) || (*(p.p)).val == nullptr)
                throw invalid_iterator();
            return (p.p)->val;
        }
        /**
         * other operation
         */
        bool operator==(const iterator& rhs) const
        {
            return p == rhs.p;
        }
        bool operator!=(const iterator& rhs) const
        {
            return p != rhs.p;
        }
        bool operator==(const const_iterator& rhs) const
        {
            return p == rhs.p;
        }
        bool operator!=(const const_iterator& rhs) const
        {
            return p != rhs.p;
        }
    };
    class const_iterator {
    public:
        list_it p;
        const_iterator(list_it p = list_it(nullptr))
            : p(p)
        {
        }
        const_iterator(const iterator& other)
        {
            p = other.p;
        }
        ~const_iterator()
        {
            p = list_it(nullptr);
        }
        /**
         * iter++
         */
        const_iterator operator++(int)
        {
            const_iterator temp = *this;
            p++;
            return temp;
        }
        /**
         * ++iter
         */
        const_iterator& operator++()
        {
            ++p;
            return *this;
        }
        /**
         * iter--
         */
        const_iterator operator--(int)
        {
            const_iterator temp = *this;
            p--;
            return temp;
        }
        /**
         * --iter
         */
        const_iterator& operator--()
        {
            --p;
            return *this;
        }
        /**
         * if the iter didn't point to a value
         * throw
         */
        const value_type& operator*() const
        {
            if (p == list_it(nullptr) || (*(p.p)).val == nullptr)
                throw invalid_iterator();
            return *((p.p)->val);
        }
        const value_type* operator->() const noexcept
        {
            if (p == list_it(nullptr) || (*(p.p)).val == nullptr)
                throw invalid_iterator();
            return (p.p)->val;
        }
        /**
         * other operation
         */
        bool operator==(const iterator& rhs) const
        {
            return p == rhs.p;
        }
        bool operator!=(const iterator& rhs) const
        {
            return p != rhs.p;
        }
        bool operator==(const const_iterator& rhs) const
        {
            return p == rhs.p;
        }
        bool operator!=(const const_iterator& rhs) const
        {
            return p != rhs.p;
        }
    };

    /**
     * return the value connected with the key
     * if the key not found, throw
     */
    T& at(const Key& key)
    {
        map_it it = map.find(key);
        if (it == map.end())
            throw index_out_of_bound();
        list_it it2 = it->second;
        return it2->second;
    }
    const T& at(const Key& key) const
    {
        map_it it = map.find(key);
        if (it == map.end())
            throw index_out_of_bound();
        list_it it2 = it->second;
        return it2->second;
    }
    T& operator[](const Key& key)
    {
        return at(key);
    }
    const T& operator[](const Key& key) const
    {
        return at(key);
    }

    /**
     * return an iterator point at the beginning
     * const or not
     */
    iterator begin()
    {
        return iterator(list.begin());
    }
    const_iterator cbegin() const
    {
        return const_iterator(list.begin());
    }
    /**
     * return an iterator point at the ending
     * const or not
     */
    iterator end()
    {
        return iterator(list.end());
    }
    const_iterator cend() const
    {
        return const_iterator(list.end());
    }

    /**
     * if didn't contain anything, return true,
     * otherwise false.
     */
    bool empty() const
    {
        return map.empty();
    }
    /**
     * destroy the whole linked_hashmap
     * can't be used after destroy
     */
    void destroy()
    {
        map.destroy();
        list.destroy();
        return;
    }
    /**
     * clear the whole linked_hashmap
     * can be used after clear
     */
    void clear()
    {
        map.clear();
        list.clear();
        return;
    }
    /**
     * return the number of value_pairs
     */
    size_t size() const
    {
        return map.elements;
    }

    /**
     * find the iterator points at the value_pair
     * which consist of key
     * if not find, return an iterator
     * point at nothing
     */
    iterator find(const Key& key)
    {
        map_it it = map.find(key);
        if (it == map.end())
            return end();
        return iterator(list_it(it->second));
    }
    /**
     * return how many value_pairs consist of the key
     * should only return 0 or 1
     */
    size_t count(const Key& key) const
    {
        map_it it = map.find(key);
        return it != map.end();
    }
    /**
     * insert a new key
     * already have a value_pair with the same key:
     * just update the value, return false
     * and move the value_pair to the end of the list
     * not find a value_pair with the same key:
     * insert the value_pair, return true
     */
    pair<iterator, bool> insert(const value_type& value)
    {
        map_it it = map.find(value.first);
        if (it == map.end()) {
            list.insert_tail(value);
            list_it it2 = list.last();
            map.insert({ value.first, it2 });
            return sjtu::pair<iterator, bool>(iterator(list_it(it2)), true);
        } else {
            list_it it2 = it->second;
            list.erase(it2);
            list.insert_tail(value);
            it2 = list.last();
            map.insert({ value.first, it2 });
            return sjtu::pair<iterator, bool>(iterator(list_it(it2)), false);
        }
    }
    /**
     * erase the element at iterator pos
     * if the iter didn't point to anything, throw
     */
    void remove(iterator pos)
    {
        list_it it = pos.p;
        if (it == list.end())
            throw invalid_iterator();
        map.remove(it->first);
        list.erase(it);
        return;
    }
};

class lru {
    using lmap = sjtu::linked_hashmap<Integer, Matrix<int>, Hash, Equal>;
    using value_type = sjtu::pair<const Integer, Matrix<int>>;

public:
    /**
     * the size of the list
     * pop if the list is full
     */
    size_t size;
    lmap map;
    lru(int size)
        : size(size)
    {
    }
    ~lru() = default;

    /**
     * save the value_pair in the memory
     * delete something in the memory if necessary
     */
    void save(const value_type& v)
    {
        map.insert(v);
        if (map.size() > size) {
            lmap::iterator it = map.begin();
            map.remove(it);
        }
        return;
    }
    /**
     * return a pointer contain the value
     */
    Matrix<int>* get(const Integer& v)
    {
        lmap::iterator it = map.find(v);
        if (it != map.end()) {
            map.insert(value_type(it->first, it->second));
            it = map.find(v);
            return &(it->second);
        }
        return nullptr;
    }

    /**
     * print everything in the memory
     */
    void print()
    {
        for (lmap::iterator it = map.begin(); it != map.end(); it++)
            std::cout << (it->first).val << " " << it->second << std::endl;
        return;
    }
};
}

#endif