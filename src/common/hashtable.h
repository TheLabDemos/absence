#ifndef HASHTABLE_H_
#define HASHTABLE_H_

#include <list>
#include <vector>

template <class KeyT, class ValT> 
struct Pair {
	KeyT key; 
	ValT val;
};

template <class KeyType, class ValType>
class HashTable {
private:

	size_t size;
	std::vector<std::list<Pair<KeyType, ValType> > > table;
	//std::list<Pair<KeyType, ValType> > table[100];

	unsigned int (*HashFunc)(const KeyType &key, unsigned long size);
	unsigned int Hash(const KeyType &key) {return (unsigned int)HashFunc(key, (unsigned long)size);}

public:

	HashTable(unsigned long size = 101);
	~HashTable();

	void SetHashFunction(unsigned int (*HashFunc)(const KeyType&, unsigned long));

	void Insert(KeyType key, ValType value);
	void Remove(KeyType key);

	Pair<KeyType, ValType> *Find(KeyType key);
};

//// std hash function
//template <class KeyType>
//unsigned int _HashFunction(const KeyType &key, unsigned long size) {
//	return (size_t)key % size;
//}

// hash table member functions
template <class KeyType, class ValType>
HashTable<KeyType, ValType>::HashTable(unsigned long size) {
	this->size = size;
	//HashFunc = _HashFunction;
	table.resize(size);
}

template <class KeyType, class ValType>
HashTable<KeyType, ValType>::~HashTable() {
	for(unsigned long i=0; i<size; i++) {
		table[i].erase(table[i].begin(), table[i].end());
	}
}

template <class KeyType, class ValType>
void HashTable<KeyType, ValType>::SetHashFunction(unsigned int (*HashFunc)(const KeyType&, unsigned long)) {
	this->HashFunc = HashFunc;
}

template <class KeyType, class ValType>
void HashTable<KeyType, ValType>::Insert(KeyType key, ValType value) {
	Pair<KeyType, ValType> newpair;
	newpair.key = key;
	newpair.val = value;

	table[Hash(key)].push_back(newpair);
}

template <class KeyType, class ValType>
void HashTable<KeyType, ValType>::Remove(KeyType key) {

	unsigned int pos = Hash(key);

	std::list<Pair<KeyType, ValType> >::iterator iter = table[pos].begin();
	while(iter != table[pos].end()) {
		if(iter->key == key) {
			table[pos].erase(iter);
			return;
		}
		iter++;
	}
}

template <class KeyType, class ValType>
Pair<KeyType, ValType> *HashTable<KeyType, ValType>::Find(KeyType key) {

	unsigned int pos = Hash(key);

	std::list<Pair<KeyType, ValType> >::iterator iter = table[pos].begin();
	while(iter != table[pos].end()) {
		if(iter->key == key) {
			return &(*iter);
		}
		iter++;
	}

	return 0;
}

#endif	// HASHTABLE_H_