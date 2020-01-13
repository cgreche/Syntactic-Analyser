
#include "hashtable.h"

#define HT_INITIAL_CAPACITY 250



Hash defaultHashFunction(const unsigned char *key, unsigned long keyLen)
{
	Hash h = 0;
	const unsigned char *c = key;

	while(keyLen > 0) {
		h = (h<<3) ^ h ^ *c++;
		--keyLen;
	}

	return h;
}



bool defaultKeyComparator(const unsigned char *key1, unsigned long keyLen1, void *param1,
						  const unsigned char *key2, unsigned long keyLen2, void *param2)
{
	if(keyLen1 != keyLen2)
		return false;

	for(unsigned int i = 0; i < keyLen1; ++i) {
		if(key1[i] != key2[i]) {
			return false;
		}
	}

	return true;
}



HashElem *HashTable::findElem(const unsigned char *key, unsigned long keyLen, Bucket **associatedBucket, KeyComparator compareFunction, void *compareParam)
{
	Hash hash = m_hashFunction(key,keyLen) % m_bucketCount;
	Bucket *bucket = &m_buckets[hash];
	if(associatedBucket)
		*associatedBucket = bucket;

	if(!compareFunction)
		compareFunction = m_keyComparator;

	for(HashElem *elem = m_buckets[hash].firstElem; elem; elem = elem->next) {
		if(elem->key == key || compareFunction(key,keyLen,compareParam,elem->key,elem->keyLen,elem->data))
			return elem;
	}

	return NULL;
}



int HashTable::rehash(unsigned int bucketCount)
{
	Bucket *newBuckets = new Bucket[bucketCount];
	if(!newBuckets)
		return -1;

	m_bucketsUsed = 0;
	for(unsigned int i = 0; i < m_bucketCount; ++i) {
		HashElem *elem;
		for(elem = m_buckets[i].firstElem; elem; ) {
			
			Hash h = m_hashFunction(elem->key,elem->keyLen) % bucketCount;

			HashElem *newElem;
			Bucket *bucket;
			bucket = &newBuckets[h];
			newElem = new HashElem(elem->key,elem->keyLen,elem->data);
			if(bucket->firstElem) {
				newElem->next = bucket->firstElem;
				newElem->next->prev = newElem;
				bucket->firstElem = newElem;
			}
			else {
				bucket->firstElem = newElem;
				++m_bucketsUsed;
			}

			++bucket->entryCount;

			HashElem *temp = elem->next;
			delete elem;
			elem = temp;
		}
	}

	delete m_buckets;
	m_buckets = newBuckets;
	m_bucketCount = bucketCount;
	return 1;
}



int HashTable::rehashIfNecessary()
{
	int res = 0;
	if(m_bucketsUsed >= m_bucketCount*80/100)
		res = rehash(m_bucketCount*2);
	else if(m_entryCount > m_bucketCount*2)
		res = rehash(m_bucketCount*2);
	return res;
}



HashTable::HashTable(unsigned int initialCapacity, HashFunction hashFunction, KeyComparator keyComparator)
:m_bucketCount(0), m_entryCount(0), m_bucketsUsed(0),
 m_buckets(NULL),
 m_hashFunction(hashFunction), m_keyComparator(keyComparator)
{
	unsigned int _initialCapacity = initialCapacity != 0 ? initialCapacity : HT_INITIAL_CAPACITY;
	m_hashFunction = hashFunction != 0 ? hashFunction : defaultHashFunction;
	m_keyComparator = keyComparator != 0 ? keyComparator : defaultKeyComparator;
	m_buckets = new Bucket[_initialCapacity];
	if(!m_buckets)
		throw -1;
	m_bucketCount = _initialCapacity;
}



HashTable::~HashTable()
{
	if(m_buckets) {
		delete [] m_buckets;
		m_buckets = NULL;
	}
}



void HashTable::setHashFunction(HashFunction hashFunction)
{
	m_hashFunction = hashFunction;
}



void *HashTable::insert(const unsigned char *key, unsigned long keyLen, void *data, bool unique)
{
	Bucket *bucket = 0;

	HashElem *elem;
	
	if(unique) {
		elem = findElem(key,keyLen,&bucket);
		if(elem) {
			elem->data = data;
			return elem->data;
		}
	}

	int res = rehashIfNecessary();
	if (res == -1) //necessary memory unavailable
		return NULL;

	if(res == 1) //rehashed, find new bucket
		findElem(key,keyLen,&bucket);

	elem = new HashElem(key,keyLen,data);
	if(bucket->firstElem) {
		elem->next = bucket->firstElem;
		elem->next->prev = elem;
		bucket->firstElem = elem;
	}
	else {
		bucket->firstElem = elem;
		++m_bucketsUsed;
	}

	++bucket->entryCount;
	++m_entryCount;

	return NULL;
}



void *HashTable::remove(const unsigned char *key, unsigned long keyLen)
{
	Bucket *bucket;
	HashElem *elem = findElem(key,keyLen,&bucket);
	if(elem) {
		if(bucket->firstElem == elem) {
			bucket->firstElem = elem->next;
		}
		else {
			elem->prev->next = elem->next;
		}
		if(elem->next)
			elem->next->prev = elem->prev;

		void *data = elem->data;
		delete elem;
		--bucket->entryCount;
		--m_entryCount;
		return data;
	}

	return NULL;
}



void *HashTable::find(const unsigned char *key, unsigned long keyLen)
{
	HashElem *elem = findElem(key,keyLen);
	return elem ? elem->data : 0;
}



void *HashTable::findIf(const unsigned char *key, unsigned long keyLen, KeyComparator compareFunction, void *compareParam)
{
	HashElem *elem = findElem(key,keyLen,NULL,compareFunction,compareParam);
	return elem ? elem->data : 0;
}



void HashTable::removeAllEntries()
{
	m_bucketsUsed = 0;
	for(unsigned int i = 0; i < m_bucketCount; ++i) {
		HashElem *elem;
		for(elem = m_buckets[i].firstElem; elem; ) {
			HashElem *temp = elem;
			elem = elem->next;
			delete temp;
		}
		m_buckets[i].firstElem = NULL;
		m_buckets[i].entryCount = 0;
	}

	m_entryCount = 0;
}



unsigned int HashTable::entryCount() const
{
	return m_entryCount;
}