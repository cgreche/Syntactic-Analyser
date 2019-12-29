
#include "c_object.h"

namespace C_language {

CObject::CObject()
{
	m_refCount = 1;
}

int CObject::addRef()
{
	return ++m_refCount;
}

int CObject::release()
{
	int refCount = --m_refCount;
	if(refCount == 0)
		delete this;
	return refCount;
}

}