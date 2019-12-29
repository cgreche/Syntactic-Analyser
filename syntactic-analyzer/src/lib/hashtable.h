#ifndef __HASHTABLE_H__
#define __HASHTABLE_H__

#undef NULL
#define NULL 0

typedef unsigned long Hash;
typedef Hash (*HashFunction)(const unsigned char *key, unsigned long keyLen);
typedef bool (*KeyComparator)(const unsigned char *key1, unsigned long keyLen1, void *param1, const unsigned char *key2, unsigned long keyLen2, void *param2);

struct HashElem
{
	HashElem *next, *prev;
	const unsigned char *key;
	unsigned long keyLen;
	void *data;

	HashElem()
	: next(NULL), prev(NULL),
	  key(NULL), keyLen(0),
	  data(NULL)
	{
	}

	HashElem(const unsigned char *key, unsigned long keyLen, void *data)
		: next(NULL), prev(NULL),
		  key(key), keyLen(keyLen),
		  data(data)
	{
	}
};



class HashTable
{
	unsigned int m_bucketCount;
	unsigned int m_entryCount; //total number of HashElems
	unsigned int m_bucketsUsed;

	struct Bucket
	{
		unsigned int entryCount;
		HashElem *firstElem;

		Bucket()
			: entryCount(0), firstElem(NULL)
		{
		}

	} *m_buckets;

	HashFunction m_hashFunction;
	KeyComparator m_keyComparator;

	int rehash(unsigned int bucketCount);
	int rehashIfNecessary();


	HashElem *findElem(const unsigned char *key, unsigned long keyLen, Bucket **bucket = NULL, KeyComparator compareFunction = NULL, void *compareParam = NULL);

	public:
		HashTable(unsigned int initialCapacity = 0, HashFunction hashFunction = 0, KeyComparator keyComparator = 0);
		~HashTable();

		void setHashFunction(HashFunction hashFunction);

		void *insert(const unsigned char *key, unsigned long keyLen, void *data, bool unique = true);
		void *remove(const unsigned char *key, unsigned long keyLen);
		void *find(const unsigned char *key, unsigned long keyLen);
		void *findIf(const unsigned char *key, unsigned long keyLen, KeyComparator compareFunction, void *compareParam);

		void removeAllEntries();
		unsigned int entryCount() const;
};

#endif