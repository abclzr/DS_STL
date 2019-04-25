#ifndef SJTU_DEQUE_HPP
#define SJTU_DEQUE_HPP

#include "exceptions.hpp"

#include <cstddef>

namespace sjtu { 

template<class T>
class deque {
public:

    struct node {
        node *pre, *nxt;
        T v;
        node(const node &other) : v(other.v), pre(nullptr), nxt(nullptr) {}
        node(const T &r) : v(r), pre(nullptr), nxt(nullptr);
    };

    class list<class T> { 
    public:
        node *head, *tail;
        int size;
        
        list() : head(nullptr), tail(nullptr), size(0), {
        }

        list(const list &other) : size(other.size), head(nullptr), tail(nullptr) {
            node *t = other.head;
            while (t != nullptr) {
                if (tail == nullptr) {
                    head = tail = new node(t->v);
                } else {
                    tail->nxt = new node(t->v);
                    tail->nxt->pre = tail;
                    tail = tail->nxt;
                }
                t = t->nxt;
            }
        }

        ~list() {
            node *t = head, t2;
            while (t != nullptr) {
                t2 = t;
                t  = t->nxt;
                delete t2;
            }
        }

        void push_front(const T &a) {
            if (head == nullptr) {
                head = tail = new node(a);
                size = 1;
            } else {
                ++size;
                head->pre = new node(a);
                head->pre->nxt = head;
                head = head->pre;
            }
        }

        void push_back(const T &a) {
            if (tail == nullptr) {
                head = tail = new node(a);
                size = 1;
            } else {
                ++size;
                tail->nxt = new node(a);
                tail->nxt->pre = tail;
                tail = tail->nxt;
            }
        }

        void pop_front() {
            if (size == 0)
                throw container_is_empty();
            --size;
            if (head->nxt = nullptr) {
                delete head;
                head = tail = nullptr;
            } else {
                head = head->nxt;
                delete head->pre;
                head->pre = nullptr;
            }
        }

        void pop_back() {
            if (size == 0)
                throw container_is_empty();
            --size;
            if (tail->pre == nullptr) {
                delete tail;
                head = tail = nullptr;
            } else {
                tail = tail->pre;
                delete tail->nxt;
                tail->nxt = nullptr;
            }
        }

        void add(int k, const T &a) {
            if (k > size)
                throw index_out_of_bound();
            if (k == 0) add_front(a);
            else {
                ++size;
                node *t = head;
                while (--k) t = t->nxt;
                node *r = new node(a);
                r->pre = t;
                r->nxt = t->nxt;
                if (r->pre != nullptr) r->pre->nxt = r;
                if (r->nxt != nullptr) r->nxt->pre = r;
            }
        }

        void del(int k) {
            if (k < 1 || k > size)
                throw index_out_of_bound();
            --size;
            node *t = head;
            while (--k) t = t->nxt;
            node *p = t->pre, *q = t->nxt;
            delete t;
            if (p != nullptr) p->nxt = q;
            if (q != nullptr) q->pre = p;
        }

        void del(node *t) {
            if (t->pre != nullptr) t->pre->nxt = t->nxt;
            if (t->nxt != nullptr) t->nxt->pre = t->pre;
            --size;
            if (head == t) head = t->nxt;
            if (tail == t) tail = t->pre;
            delete t;
        }

        T &operator[](int k) {
            if (k < 1 || k > size)
                throw index_out_of_bound();
            node *t = head;
            while (--k) t = t->nxt;
            return t->v;
        }

        const T &operator[](int k) const {
            if (k < 1 || k > size)
                throw index_out_of_bound();
            node *t = head;
            while (--k) t = t->nxt;
            return t->v;
        }

        T &front() {
            if (size == 0)
                throw container_is_empty();
            return head->v;
        }

        const T &front() const {
            if (size == 0)
                throw container_is_empty();
            return head->v;
        }

        T &back() {
            if (size == 0)
                throw container_is_empty();
            return tail->v;
        }

        const T &back() const {
            if (size == 0)
                throw container_is_empty();
            return tail->v;
        }

        void merge_front(list &other) {
            size += other.size;
            if (head != nullptr) head->pre = other.tail;
            if (other.tail != nullptr) other.tail->nxt = head;
            head = other.head;
        }

        void merge_back(list &other) {
            size += other.size;
            if (tail != nullptr) tail->nxt = other.head;
            if (other.head != nullptr) other.head->pre = tail;
            tail = other.tail;
        }

        void split(list &l1, list &l2) {
            int k = size / 2;
            node *t = head;
            while (--k) t = t->nxt;
            l1.head = head;
            l2.tail = tail;
            l1.tail = t;
            l2.head = t->nxt;
            t->nxt->pre = nullptr;
            t->pre = nullptr;
            l1.size = size / 2;
            l2.size = size - l1.size;
        }
    };


	class const_iterator;
	class iterator {
	private:
        list_node *p;
        node *t;

        int get_pos() const {
            node *it = t;
            list_node *ip = p;
            int ret = 0;
            while (it != nullptr) {
                it = it->pre;
                ++ret;
            }
            ip = ip->pre;
            while (ip != nullptr) {
                ret += ip->l.size;
                ip = ip->pre;
            }
            return ret;
        }

        void move_back(const int &n) {
            if (t == nullptr || p == nullptr)
                throw invalid_iterator();
            int k = n;
            while (k-- && t->nxt != nullptr) t = t->nxt;
            if (k == 0) return;
            if (p->nxt == nullptr)
                throw runtime_error();
            p = p->nxt;
            while (p->size() < k && p->nxt != nullptr)
                k -= p->size(), p = p->nxt;
            if (p->size() >= k) {
               t = p->l.head;
               while (--k) t = t->nxt;
               return;
            } else
                throw runtime_error();
		}
        
        void move_front(const int &n) {
            if (t == nullptr || p == nullptr)
                throw invalid_iterator();
            int k = n;
            while (k-- && t->pre != nullptr) t = t->pre;
            if (k == 0) return;
            if (ip->pre == nullptr)
                throw runtime_error();
            p = p->pre;
            while (p->size() < k && p->pre != nullptr)
                k -= p->size(), p = p->pre;
            if (p->size() >= k) {
                t = p->l.tail;
                while (--k) t = t->pre;
                return;
            } else
                throw runtime_error();
        }

		/**
		 * TODO add data members
		 *   just add whatever you want.
		 */
	public:
		/**
		 * return a new iterator which pointer n-next elements
		 *   even if there are not enough elements, the behaviour is **undefined**.
		 * as well as operator-
		 */
		iterator operator+(const int &n) const {
            iterator it = (*this);
            if (n > 0) it.move_back(n);
            else it.move_front(-n);
            return it;
		}
		iterator operator-(const int &n) const {
            iterator it = (*this);
            if (n > 0) it.move_front(n);
            else it.move_back(-n);
            return it;
		}
		// return th distance between two iterator,
		// if these two iterators points to different vectors, throw invaild_iterator.
		int operator-(const iterator &rhs) const {
            return rhs.get_pos() - get_pos();
		}
		iterator operator+=(const int &n) {
            if (n > 0) move_back(n);
            else move_front(-n);
            return *this;
		}
		iterator operator-=(const int &n) {
            if (n > 0) move_front(n);
            else move_back(-n);
            return *this;
		}
		/**
		 * TODO iter++
		 */
		iterator operator++(int) {
            iterator a = (*this);
            move_back(1);
            return a;
        }
		/**
		 * TODO ++iter
		 */
		iterator& operator++() {
            move_back(1);
            return *this;
        }
		/**
		 * TODO iter--
		 */
		iterator operator--(int) {
            iterator a = (*this);
            move_front(1);
            return a;
        }
		/**
		 * TODO --iter
		 */
		iterator& operator--() {
            move_front(1);
            return *this;
        }
		/**
		 * TODO *it
		 */
		T& operator*() const {
            if (t == nullptr || p == nullptr)
                throw invalid_iterator();
            return t->v;
        }
		/**
		 * TODO it->field
		 */
		T* operator->() const noexcept {
            if (t == nullptr || p == nullptr) return nullptr;
            return &t->v;
        }
		/**
		 * a operator to check whether two iterators are same (pointing to the same memory).
		 */
		bool operator==(const iterator &rhs) const {
            return p == rhs.p && t == rhs.t;
        }
		bool operator==(const const_iterator &rhs) const {
            return p == rhs.p && t == rhs.t;
        }
		/**
		 * some other operator for iterator.
		 */
		bool operator!=(const iterator &rhs) const {
            return p != rhs.p || t != rhs.t;
        }
		bool operator!=(const const_iterator &rhs) const {
            return p != rhs.p || t != rhs.t;
        }
	};
	class const_iterator {
		// it should has similar member method as iterator.
		//  and it should be able to construct from an iterator.
		private:
            const list_node *p;
            const node *t;
			// data members.
		public:
			const_iterator() : p(nullptr), t(nullptr) {}
			const_iterator(const const_iterator &other) : p(other.p), t(other.t) {}
			const_iterator(const iterator &other) : p(other.p), t(other.t) {}
	};

private:

    static const int B = 500;
    static const int b = 250;

    struct list_node {
        list_node *pre, *nxt;
        list l;
        list_node() : l(), pre(nullptr), nxt(nullptr) {}
        list_node(const list_node &other) : l(other.l), pre(nullptr), nxt(nullptr) {}
        ~list_node() {}
        int size() {
            return l.size;
        }
    };

    void balance(list_node *t) {
        if (t->l.size > B) {
            list_node *l1 = new list_node(), *l2 = new list_node();
            t->l.split(l1->l, l2->l);
            l1->pre = t->pre;
            l1->nxt = l2;
            l2->pre = l1;
            l2->nxt = t->nxt;
            if (l1->pre != nullptr) l1->pre->nxt = l1;
            if (l2->nxt != nullptr) l2->nxt->pre = l2;
            if (head == t) head = l1;
            if (tail == t) tail = l2;
            operator delete(t);
        } else if (t->l.size < b) {
           while (t->pre != nullptr && t->pre->size() + t->l.size <= B ) {
               t->l.merge_front(t->pre->l);
               if (head == t->pre) head = t;
               list_node *d = t->pre;
               t->pre = t->pre->pre;
               if (t->pre != nullptr) t->pre->nxt = t;
               operator delete (d);
           }
           while (t->nxt != nullptr && t->nxt->size() + t->l.size <= B) {
               t->l.merge_back(t->nxt->l);
               if (tail == t->nxt) tail = t;
               list_node *d = t->nxt;
               t->nxt = t->nxt->nxt;
               if (t->nxt != nullptr) t->nxt->pre = t;
               operator delete (d);
           }
        }
    }

    list_node *head, *tail;
    int size;

	/**
	 * TODO Constructors
	 */
	deque() : head(nullptr), tail(nullptr), size(0) {}
	deque(const deque &other) : size(other.size), head(nullptr), tail(nullptr) {
        list_node *t = other.head;
        while (t != nullptr) {
            if (head == nullptr) {
                head = tail = new list_node(*t);
            } else {
                tail->nxt = new list_node(*t);
                tail->nxt->pre = tail;
                tail = tail->nxt;
            }
            t = t->nxt;
        }
    }
	/**
	 * TODO Deconstructor
	 */
	~deque() {
        clear();
    }
	/**
	 * TODO assignment operator
	 */
	deque &operator=(const deque &other) {
        clear();
        size = other.size;
        list_node *t = other.head;
        while (t != nullptr) {
            if (head == nullptr) {
                head = tail = new list_node(*t);
            } else {
                tail->nxt = new list_node(*t);
                tail->nxt->pre = tail;
                tail = tail->nxt;
            }
            t = t->nxt;
        }
        return *this;
    }
	/**
	 * access specified element with bounds checking
	 * throw index_out_of_bound if out of bound.
	 */
	T & at(const size_t &pos) {
        if (pos < 0 || pos > size)
            throw index_out_of_bound();
        list_node *t = head;
        int k = pos + 1;
        while (t != nullptr) {
            if (t->size() >= k) return t->l[k];
            else {
                k -= t->size();
                t = t->nxt;
            }
        }
    }
    
	const T & at(const size_t &pos) const {
        if (pos < 0 || pos > size)
            throw index_out_of_bound();
        list_node *t = head;
        int k = pos + 1;
        while (t != nullptr) {
            if (t->size() >= k) return t->l[k];
            else {
                k -= t->size();
                t = t->nxt;
            }
        }
    }

	T & operator[](const size_t &pos) {
        if (pos < 0 || pos > size)
            throw index_out_of_bound();
        list_node *t = head;
        int k = pos + 1;
        while (t != nullptr) {
            if (t->size() >= k) return t->l[k];
            else {
                k -= t->size();
                t = t->nxt;
            }
        }
    }
    
	const T & operator[](const size_t &pos) const {
        if (pos < 0 || pos > size)
            throw index_out_of_bound();
        list_node *t = head;
        int k = pos + 1;
        while (t != nullptr) {
            if (t->size() >= k) return t->l[k];
            else {
                k -= t->size();
                t = t->nxt;
            }
        }
    }
	/**
	 * access the first element
	 * throw container_is_empty when the container is empty.
	 */
	const T & front() const {
        if (size == 0)
            throw container_is_empty();
        return head->l.front();
    }
	/**
	 * access the last element
	 * throw container_is_empty when the container is empty.
	 */
	const T & back() const {
        if (size == 0)
            throw container_is_empty();
        return tail->l.back();
    }
	/**
	 * returns an iterator to the beginning.
	 */
	iterator begin() {}
	const_iterator cbegin() const {}
	/**
	 * returns an iterator to the end.
	 */
	iterator end() {}
	const_iterator cend() const {}
	/**
	 * checks whether the container is empty.
	 */
	bool empty() const {
        return size == 0;
    }
	/**
	 * returns the number of elements
	 */
	size_t size() const {
        return size;
    }
	/**
	 * clears the contents
	 */
	void clear() {
        size = 0;
        list_node *t = head, *t2;
        while (t != nullptr) {
            t2 = t;
            t = t->nxt;
            delete t2;
        }
        head = tail = nullptr;
    }
	/**
	 * inserts elements at the specified locat on in the container.
	 * inserts value before pos
	 * returns an iterator pointing to the inserted value
	 *     throw if the iterator is invalid or it point to a wrong place.
	 */
	iterator insert(iterator pos, const T &value) {}
	/**
	 * removes specified element at pos.
	 * removes the element at pos.
	 * returns an iterator pointing to the following element, if pos pointing to the last element, end() will be returned.
	 * throw if the container is empty, the iterator is invalid or it points to a wrong place.
	 */
	iterator erase(iterator pos) {}
	/**
	 * adds an element to the end
	 */
	void push_back(const T &value) {
        tail->l.push_back(value);
        balance(tail);
        ++size;
    }
	/**
	 * removes the last element
	 *     throw when the container is empty.
	 */
	void pop_back() {
        if (size == 0)
            throw container_is_empty();
        tail->l.pop_back();
        balance(tail);
        --size;
    }
	/**
	 * inserts an element to the beginning.
	 */
	void push_front(const T &value) {
        head->l.push_front(value);
        balance(head);
        ++size;
    }
	/**
	 * removes the first element.
	 *     throw when the container is empty.
	 */
	void pop_front() {
        if (size == 0)
            throw container_is_empty();
        head->l.pop_front();
        balance(head);
        --size;
    }
};

}

#endif
