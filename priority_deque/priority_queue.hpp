#ifndef SJTU_PRIORITY_QUEUE_HPP
#define SJTU_PRIORITY_QUEUE_HPP

#include <cstddef>
#include <functional>
#include "exceptions.hpp"

namespace sjtu {

template<class T>
void swap(T &a, T &b) {
    T c = a;
    a = b;
    b = c;
}

/**
 * a container like std::priority_queue which is a heap internal.
 * it should be based on the vector written by yourself.
 */
template<typename T, class Compare = std::less<T>>
class priority_queue {
private:
    class node {
    public:
        T v;
        int h;
        node *ch[2];
        node(const T &c) : v(c), ch{nullptr, nullptr}, h(0) {}
        node(const node &t) : v(t.v), ch{nullptr, nullptr}, h(0) {}
    };
    
    node *root;
    size_t sz;
public:
	/**
	 * TODO constructors
	 */
	priority_queue() : root(nullptr), sz(0) {}

    void clean(node *r) {
        if (r == nullptr) return;
        clean(r->ch[0]);
        clean(r->ch[1]);
        delete r;
    }

    node *newtree(node *t) {
        if (t == nullptr) return nullptr;
        T tt = t->v;
        node *r = new node(*t);
        r->ch[0] = newtree(t->ch[0]);
        r->ch[1] = newtree(t->ch[1]);
        r->h = t->h;
        return r;
    }

	priority_queue(const priority_queue &other) {
        sz = other.size();
        root = newtree(other.root);
    }
	/**
	 * TODO deconstructor
	 */
	~priority_queue() {
        clean(root);
    }
	/**
	 * TODO Assignment operator
	 */
	priority_queue &operator=(const priority_queue &other) {
        if (&other == this) return *this;
        sz = other.size();
        clean(root);
        root = newtree(other.root);
        return *this;
    }
	/**
	 * get the top of the queue.
	 * @return a reference of the top element.
	 * throw container_is_empty if empty() returns true;
	 */
	const T & top() const {
        if (sz == 0)
            throw container_is_empty();
        return root->v;
	}
	/**
	 * TODO
	 * push new element to the priority queue.
	 */
	void push(const T &e) {
        node *rt = new node(e);
        root = merge(root, rt);
        ++sz;
	}
	/**
	 * TODO
	 * delete the top element.
	 * throw container_is_empty if empty() returns true;
	 */
	void pop() {
        if (sz == 0)
            throw container_is_empty();

        node *t = root;
        root = merge(root->ch[0], root->ch[1]);
        --sz;
        delete t;
	}
	/**
	 * return the number of the elements.
	 */
	size_t size() const {
        return sz;
	}
	/**
	 * check if the container has at least an element.
	 * @return true if it is empty, false if it has at least an element.
	 */
	bool empty() const {
        return sz == 0;
	}
	/**
	 * return a merged priority_queue with at least O(logn) complexity.
	 */

    node *merge(node *r1, node *r2) {
        if (r1 == nullptr) return r2;
        if (r2 == nullptr) return r1;
        if (Compare()(r1->v, r2->v)) swap(r1, r2);
        r1->ch[1] = merge(r1->ch[1], r2);

        if (r1->ch[0] && r1->ch[1]) {
            if (r1->ch[0]->h < r1->ch[1]->h)
                swap(r1->ch[0], r1->ch[1]);
        } else {
            if (r1->ch[0] == nullptr) {
                r1->ch[0] = r1->ch[1];
                r1->ch[1] = nullptr;
            }
        }
        if (r1->ch[1]) r1->h = r1->ch[1]->h + 1;
        else r1->h = 0;

        return r1;
    }

	void merge(priority_queue &other) {
        sz += other.size();
        root = merge(root, other.root);
        ++sz;

        other.sz = 0;
        other.root = nullptr;
	}
};

}

#endif
