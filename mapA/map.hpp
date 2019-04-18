/**
 * implement a container like std::map
 */
#ifndef SJTU_MAP_HPP
#define SJTU_MAP_HPP

// only for std::less<T>
#include <functional>
#include <cstddef>
#include "utility.hpp"
#include "exceptions.hpp"

namespace sjtu {

template<
	class Key,
	class T,
	class Compare = std::less<Key>
> class map {
public:
	/**
	 * the internal type of data.
	 * it should have a default constructor, a copy constructor.
	 * You can use sjtu::map as value_type by typedef.
	 */
	typedef pair<const Key, T> value_type;
	/**
	 * see BidirectionalIterator at CppReference for help.
	 *
	 * if there is anything wrong throw invalid_iterator.
	 *     like it = map.begin(); --it;
	 *       or it = map.end(); ++end();
	 */f
	
private:
	class RB_Tree {
	public:
		struct node *nil;
		struct node {
			node *ch[2], *fa, *pre, *nxt;
			value_type v;
			bool red;
            int size;
			node(const value_type &_v) : v(_v), ch{nil, nil}, fa(nil), pre(nil), nxt(nil), red(false), size(1) {}
            node(const Key &key): v(key, T()), ch{nil, nil}, fa(nil), pre(nil), nxt(nil), red(false), size(1) {}
            node(const node &u) v(u.v), red(u.red), size(u.size){
                ch[0] = ch[1] = fa = pre = nxt = nil;
            }
			void setc(node *r, int c) {
				ch[c] = r;
				if (r != nil) r->fa = this;
			}
			int pl() {
                if (fa != nil)
                    return fa->ch[1] == this;
                else
                    return -1;
			}
            void count() {
                size = ch[0]->size + ch[1]->size + 1;
            }
            node *brother() {
                return fa->ch[pl() ^ 1]
            }
		};
		
		node *root, *head;
		
		int size;

        node *get_head() {
            node *r = root;
            if (r == nil) return nil;
            while (r->ch[0] != nil) r = r->ch[0];
            return r;
        }

        node *get_pre(node *r) {
            if (r->ch[0] != nil) {
                node *t = r->ch[0];
                while (t->ch[1] != nil) t = t->ch[1];
                return t;
            } else {
                while (r != nil && r->pl() != 1) r = r->fa;
                return r;
            }
        }

        node *get_nxt(node *r) {
            if (r->ch[1] != nil) {
                node *t = r->ch[1];
                while (t->ch[0] != nil) t = t->ch[0];
                return t;
            } else {
                while (r != nil && r->pl() != 0) r = r->fa;
                return r;
            }
        }

        void release(node *r) {
            if (r == nil) return;
            release(r->ch[0]);
            release(r->ch[1]);
            delete r;
        }
		
		RB_Tree() {
            nil = (node *) operator new(sizeof(node));
            nil->pre = nil->nxt = nil->ch[0] = nil->ch[1] = nil->fa = nil;
            nil->red = false;
            nil->size = 0;
            head = root = nil;
            size = 0;
        }

        ~RB_Tree() {
            release(root);
            delete nil;
        }

        node *newtree(node *r) {
            if (r->size == 0) return nil;
            node *t = new node(*r);
            t->setc(newtree(r->ch[0]), 0);
            t->setc(newtree(r->ch[1]), 1);
            return t;
        }

        void rotate(node *r) {
            node *f = r->fa;
            int c = r->pl();
            if (f == root) root = r, r->fa = nil;
            else f->fa->setc(r, f->pl());
            f->setc(r->ch[c ^ 1], c);
            r->setc(f, c ^ 1);
            f->count();
            r->count();
        }

        //the initial color of r must be red.
        void insert_fix(node *r) {
            while (r->fa->red) {
                node *uncle = r->fa->brother();
                if (uncle->red) {
                    uncle->red = false;
                    r->fa->red = false;
                    r = r->fa->fa;
                    r->red = true;
                } else {
                    rotate(r);
                    r->red = false;
                    r->fa->red = true;
                    rotate(r);
                }
            }
            root->red = false;
        }

        void delete_fix(node *r) {
            if (r == root) return;
            node *f = r->fa;
            node *b = r->brother();
            int c = r->pl();
            if (!b->red && !b->ch[0]->red && !b->ch[1]->red) {
                if (f->red) {
                    f->red = false;
                    b->red = true;
                } else {
                    b->red = true;
                    delete_fix(f);
                }
            } else {
                if (b->red) {
                    rotate(b);
                    b->red = false;
                    f->red = true;
                    delete_fix(r);
                } else {
                    if (b->ch[c ^ 1]->red) {
                        rotate(b);
                        b->red = f->red;
                        f->red = false;
                        b->ch[c ^ 1]->red = false;
                    } else {
                        node *t = b->ch[c];
                        rotate(t);
                        t->red = false;
                        t->ch[c ^ 1]->red = true;
                        delete_fix(r);
                    }
                }
            }
        }

        void remove(node *r) {
            if (r->ch[0] != nil && r->ch[1] != nil) {
                node *t = r->ch[1];
                while (t->ch[0] != nil) t = t->ch[0];
                r->v = t->v;
                remove(t);
            } else {
                node *p = get_pre(r), *q = get_nxt(r);
                if (p != nil) p->nxt = q;
                if (q != nil) q->pre = p;
                if (r->red) {
                    node *child = r->ch[0];
                    if (child == nil) child = r->ch[1];
                    r->fa->setc(child, r->pl());
                    delete r;
                } else {
                    node *child = r->ch[0];
                    if (child == nil) child = r->ch[1];
                    r->fa->setc(child, r->pl());
                    delete r;
                    if (child.red) child.red = false;
                    else delete_fix(child);
                }
            }
        }

        node *find(const Key &key) const {
            node *r = node;
            auto cmp = Compare();
            while (r != nil) {
                if (!cmp(r->v->first, key) && !cmp(key, r->v->first)) return r;
                else if (cmp(key, r->v->first)) r = r->ch[0];
                else r = r->ch[1];
            }
            if (r == nil) return nullptr;
        }

        pair<node *, bool> insert(const value_type &V) {
            node *r = node;
            auto cmp = Compare();
            while (r != nil) {
                if (!cmp(r->v->first, V.first) && !cmp(V.first, r->v->first)) return ryir<node *, bool>(r, false);
                else if (cmp(V.key, r->v->first)) {
                    if (r->ch[0] != nil)
                        r = r->ch[0];
                    else {
                        r->setc(new node(V), 0)
                        r = r->ch[0];
                        node *p = get_pre(r), *q = get_nxt(r);
                        r->pre = p; r->nxt = q;
                        if (p != nil) p->nxt = r;
                        if (q != nil) q->pre = r;
                        if (p == nil) head = r;
                        insert_fix(r);
                        return pari<node *, bool>(r, true);
                    }
                } else {
                    if (r->ch[1] != nil)
                        r = r->ch[1];
                    else {
                        r->setc(new node(V), 1);
                        r = r->ch[1];
                        node *p = get_pre(r), *q = get_nxt(r);
                        r->pre = p; r->nxt = q;
                        if (p != nil) p->nxt = r;
                        if (q != nil) q->pre = r;
                        if (p == nil) head = r;
                        insert_fix(r);
                        return pari<node *, bool>(r, true);
                    }
                }
            }
        }

        node *find_insert(const Key &key) {
            node *r = node;
            auto cmp = Compare();
            while (r != nil) {
                if (!cmp(r->v->first, key) && !cmp(key, r->v->first)) return r;
                else if (cmp(key, r->v->first)) {
                    if (r->ch[0] != nil)
                        r = r->ch[0];
                    else {
                        r->setc(new node(key), 0)
                        r = r->ch[0];
                        node *p = get_pre(r), *q = get_nxt(r);
                        r->pre = p; r->nxt = q;
                        if (p != nil) p->nxt = r;
                        if (q != nil) q->pre = r;
                        if (p == nil) head = r;
                        insert_fix(r);
                        return r;
                    }
                } else {
                    if (r->ch[1] != nil)
                        r = r->ch[1];
                    else {
                        r->setc(new node(key), 1);
                        r = r->ch[1];
                        node *p = get_pre(r), *q = get_nxt(r);
                        r->pre = p; r->nxt = q;
                        if (p != nil) p->nxt = r;
                        if (q != nil) q->pre = r;
                        if (p == nil) head = r;
                        insert_fix(r);
                        return r;
                    }
                }
            }
        }
	};
	
	
public:
	class const_iterator;
	class iterator {
	private:
		/**
		 * TODO add data members
		 *   just add whatever you want.
		 */

        RB_Tree::node *p;

        friend class const_interator;


	public:
		iterator() : p(nullptr) {}
		iterator(const iterator &other) p(other.p) {}
        iterator(node *r) : p(r) {}
		/**
		 * return a new iterator which pointer n-next elements
		 *   even if there are not enough elements, just return the answer.
		 * as well as operator-
		 */
		/**
		 * TODO iter++
		 */
		iterator operator++(int) {
            iterator a(*this);
            if (p == nullptr) throw index_out_of_bound();
            p = p->nxt;
            if (p->size == 0) p = nullptr;
            return a;
        }
		/**
		 * TODO ++iter
		 */
		iterator & operator++() {
            if (p == nullptr) throw index_out_of_bound();
            p = p->nxt;
            if (p->size == 0) p = nullptr;
            return *this;
        }
		/**
		 * TODO iter--
		 */
		iterator operator--(int) {
            if (p == nullptr) throw index_out_of_bound();
            iterator a(*this);
            p = p->pre;
            if (p->size == 0) {
                p = nullptr;
                throw index_out_of_bound();
            }
            return a;
        }
		/**
		 * TODO --iter
		 */
		iterator & operator--() {
            if (p == nullptr) throw index_out_of_bound();
            p = p->pre;
            if (p->size == 0) {
                p = nullptr;
                throw index_out_of_bound();
            }
            return *this;
        }
		/**
		 * a operator to check whether two iterators are same (pointing to the same memory).
		 */
		value_type & operator*() const {
            return p->v;
        }
		bool operator==(const iterator &rhs) const {
            return p == rhs.p;
        }
		bool operator==(const const_iterator &rhs) const {
            return p == rhs.p;
        }
		/**
		 * some other operator for iterator.
		 */
		bool operator!=(const iterator &rhs) const {
            return p != rhs.p;
        }
		bool operator!=(const const_iterator &rhs) const {
            return p != rhs.p;
        }

		/**
		 * for the support of it->first. 
		 * See <http://kelvinh.github.io/blog/2013/11/20/overloading-of-member-access-operator-dash-greater-than-symbol-in-cpp/> for help.
		 */
		value_type* operator->() const noexcept {}
	};
	class const_iterator {
		// it should has similar member method as iterator.
		//  and it should be able to construct from an iterator.
		private:
			// data members.
            RB_Tree::node *p;

            friend class iterator;

		public:
			const_iterator() : p(nullptr) {}
			const_iterator(const const_iterator &other) : p(other.p) {}
            const_iterator(const iterator &other) : p(other.p) {}
            const iterator(node *r) : p(r) {}
			// And other methods in iterator.
			// And other methods in iterator.
			// And other methods in iterator.
	};
	/**
	 * TODO two constructors
	 */

private:
    RB_Tree TREE;

public:
	map() {
    }
	map(const map &other) {
        TREE.root = RB_Tree::newtree(other.TREE.root);
        TREE.size = other.size();
        TREE.head = RB_Tree::get_head(TREE.root);
    }
	/**
	 * TODO assignment operator
	 */
	map & operator=(const map &other) {
        TREE.release(TREE.root);
        TREE.root = RB_Tree::newtree(other.TREE.root);
        size = other.size();
        TREE.head = RB_Tree::get_head(TREE.root);
    }
	/**
	 * TODO Destructors
	 */
	~map() {}
	/**
	 * TODO
	 * access specified element with bounds checking
	 * Returns a reference to the mapped value of the element with key equivalent to key.
	 * If no such element exists, an exception of type `index_out_of_bound'
	 */
	T & at(const Key &key) {
        auto *r = TREE.find(key);
        if (r == nullptr) throw index_out_of_bound();
        return r->v->second;
    }
	const T & at(const Key &key) const {
        auto *r = TREE.find(key);
        if (r == nullptr) throw index_out_of_bound();
        return r->v->second;
    }
	/**
	 * TODO
	 * access specified element 
	 * Returns a reference to the value that is mapped to a key equivalent to key,
	 *   performing an insertion if such key does not already exist.
	 */
	T & operator[](const Key &key) {
        auto *r = TREE.find_insert(key);
        return r->v->second;
    }
	/**
	 * behave like at() throw index_out_of_bound if such key does not exist.
	 */
	const T & operator[](const Key &key) const {
        auto *r = TREE.find(key);
        if (r == nullptr) throw index_out_of_bound();
        return r->v->second;
    }
	/**
	 * return a iterator to the beginning
	 */
	iterator begin() {
        return iterator(TREE.head);
    }
	const_iterator cbegin() const {
        return const_iterator(TREE.head);
    }
	/**
	 * return a iterator to the end
	 * in fact, it returns past-the-end.
	 */
	iterator end() {
        return iterator(nullptr);
    }
	const_iterator cend() const {
        return const_iterator(nullptr);
    }
	/**
	 * checks whether the container is empty
	 * return true if empty, otherwise false.
	 */
	bool empty() const {
        return TREE.size == 0;
    }
	/**
	 * returns the number of elements.
	 */
	size_t size() const {
        return TREE.size;
    }
	/**
	 * clears the contents
	 */
	void clear() {
        TREE.release(TREE.root);
        TREE.root = TREE.head = TREE.nil;
    }
	/**
	 * insert an element.
	 * return a pair, the first of the pair is
	 *   the iterator to the new element (or the element that prevented the insertion), 
	 *   the second one is true if insert successfully, or false.
	 */
	pair<iterator, bool> insert(const value_type &value) {
        pair<RB_Tree::node *, bool> p = TREE.insert(value);
        return pair<iterator, bool>(iterator(p.first), p.second);
    }
	/**
	 * erase the element at pos.
	 *
	 * throw if pos pointed to a bad element (pos == this->end() || pos points an element out of this)
	 */
	void erase(iterator pos) {
        TREE.remove(pos.p);
    }
	/**
	 * Returns the number of elements with key 
	 *   that compares equivalent to the specified argument,
	 *   which is either 1 or 0 
	 *     since this container does not allow duplicates.
	 * The default method of check the equivalence is !(a < b || b > a)
	 */
	size_t count(const Key &key) const {
        if (TREE.find(key) != nullptr) return 0;
        else return 1;
    }
	/**
	 * Finds an element with key equivalent to key.
	 * key value of the element to search for.
	 * Iterator to an element with key equivalent to key.
	 *   If no such element is found, past-the-end (see end()) iterator is returned.
	 */
	iterator find(const Key &key) {
        RB_Tree::node *p = TREE.find(key);
        return iterator(p);
    }
	const_iterator find(const Key &key) const {
        RB_Tree::node *p = TREE.find(key);
        return const_iterator(p);
    }
};

}

#endif
