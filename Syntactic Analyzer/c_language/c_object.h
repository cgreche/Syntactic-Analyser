#ifndef __C_OBJECT_H__
#define __C_OBJECT_H__

namespace C_language {

class CObject
{
	int m_refCount;

public:
	CObject();
	virtual ~CObject() { };
	int addRef();
	int release();
	int refCount();
};

}

#endif