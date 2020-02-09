#ifndef __NAMETABLE_H__
#define __NAMETABLE_H__

#include <cstring>
#include "hashtable.h"

template <class T>
class nametable
{
	HashTable m_ht;

	public:
		T insert(const char *name, T elem);
		T find(const char *name);

		unsigned int elemCount() const { return m_ht.entryCount(); }
		void clear() { m_ht.removeAllEntries(); }
};

template <class T>
T nametable<T>::insert(const char *name, T elem)
{
	return reinterpret_cast<T>(m_ht.insert(reinterpret_cast<const unsigned char*>(name),strlen(name),elem));
}

template <class T>
T nametable<T>::find(const char *name)
{
	return reinterpret_cast<T>(m_ht.find(reinterpret_cast<const unsigned char*>(name),strlen(name)));
}

#endif