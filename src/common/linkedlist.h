#ifndef _LINKEDLIST_H_
#define _LINKEDLIST_H_

template <class T>
struct ListNode {
	T data;
	ListNode<T> *next, *prev;
};


template <class T>
class LinkedList {
private:
	ListNode<T> *head, *tail;
	int size;

public:

	LinkedList();
	~LinkedList();

	inline ListNode<T> *Begin();
	inline ListNode<T> *End();

	void PushBack(ListNode<T> *node);
	void PushBack(T data);

	void Insert(ListNode<T> *pos, ListNode<T> *node);
	void Insert(ListNode<T> *pos, T data);

	void Remove(ListNode<T> *node);
	ListNode<T> *Erase(ListNode<T> *node);

	ListNode<T> *Find(T key);

	int CountNodes();
	inline int Size() const;

	void operator =(LinkedList &rhs);
};


/////////// implementation //////////
template <class T>
LinkedList<T>::LinkedList() {
	head = tail = 0;
	size = 0;
}


template <class T>
LinkedList<T>::~LinkedList() {

	while(head) {
		Erase(head);
	}
}

template <class T>
ListNode<T> *LinkedList<T>::Begin() {
	return head;
}

template <class T>
ListNode<T> *LinkedList<T>::End() {
	return tail;
}

template <class T>
void LinkedList<T>::PushBack(ListNode<T> *node) {

	if(!head) {		// empty list
		head = tail = node;
		node->next = node->prev = 0;
	} else {
		tail->next = node;
		node->prev = tail;
		tail = node;
		node->next = 0;
	}

	size++;
}

template <class T>
void LinkedList<T>::PushBack(T data) {

	ListNode<T> *node = new ListNode<T>;
	node->data = data;

	if(!head) {		// empty list
		head = tail = node;
		node->next = node->prev = 0;
	} else {
		tail->next = node;
		node->prev = tail;
		tail = node;
		node->next = 0;
	}

	size++;
}

template <class T>
void LinkedList<T>::Insert(ListNode<T> *pos, ListNode<T> *node) {

	if(!head) {
		head = tail = node;
		node->next = node->prev = 0;
	} else {
		node->prev = pos->prev;
		pos->prev = node;
		node->next = pos;
		if(head == pos) head = node; else node->prev->next = node;
	}

	size++;
}

template <class T>
void LinkedList<T>::Insert(ListNode<T> *pos, T data) {

	ListNode<T> *node = new ListNode<T>;
	node->data = data;

	if(!head) {
		head = tail = node;
		node->next = node->prev = 0;
	} else {
		node->prev = pos->prev;
		pos->prev = node;
		node->next = pos;
		if(head == pos) head = node; else node->prev->next = node;
	}

	size++;
}

template <class T>
void LinkedList<T>::Remove(ListNode<T> *node) {

	if(!node) return;	// e.g. Remove(head) on an empty list

	if(node->prev) {
		node->prev->next = node->next;
	} else {
		head = node->next;
	}

	if(node->next) {
		node->next->prev = node->prev;
	} else {
		tail = node->prev;
	}

	size--;
}

template <class T>
ListNode<T> *LinkedList<T>::Erase(ListNode<T> *node) {

	if(!node) return 0;	// e.g. Erase(head) on an empty list

	if(node->prev) {
		node->prev->next = node->next;
	} else {
		head = node->next;
	}

	if(node->next) {
		node->next->prev = node->prev;
	} else {
		tail = node->prev;
	}

	ListNode<T> *destr = node;
	node = node->next;
	
	delete destr;

	size--;

	return node;
}

template <class T>
ListNode<T> *LinkedList<T>::Find(T key) {
	
	ListNode<T> *iter = head;
	while(iter) {
		if(iter->data == key) return iter;
		iter = iter->next;
	}

	return 0;	// null pointer if not found
}

template <class T>
int LinkedList<T>::CountNodes() {

	size = 0;

	ListNode<T> *iter = head;
	while(iter) {
		size++;
		iter = iter->next;
	}

	return size;
}

template <class T>
int LinkedList<T>::Size() const {
	return size;
}


template <class T>
void LinkedList<T>::operator =(LinkedList<T> &rhs) {
	
	ListNode<T> *src = rhs.Begin();
	while(src) {
		PushBack(src->data);
		src = src->next;
	}
}


#endif	// _LINKEDLIST_H_