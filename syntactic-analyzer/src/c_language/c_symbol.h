#ifndef __C_SYMBOL_H__
#define __C_SYMBOL_H__

#include "../lib/nametable.h"
#include <vector>

#include "c_object.h"

namespace C_language {

class Scope;
class Ast;
class CSymbol;
class SymbolType;

typedef std::vector<CSymbol*> CSymbolList;

class CSymbol : public CObject
{
	SymbolType *m_type;
	char m_identifier[256];

	CSymbol *m_next;

	Scope *m_scopeSeen; //scope which this symbol was seen
	Ast *m_initList; //initializer list

	bool m_isTypedef;
	bool m_isParameter;
	int m_paramIndex;
	unsigned int m_useCount;

	~CSymbol();

public:
	CSymbol();
	
	void setIdentifier(const char *name);
	void setScopeSeen(Scope *scope) { m_scopeSeen = scope; }
	void setInitList(Ast *initList) { m_initList = initList; }
	void setTypedefFlag(bool isTypedef) { m_isTypedef = isTypedef; }
	void setParameterFlag(bool isParameter, int paramIndex) { m_isParameter = isParameter; m_paramIndex = paramIndex; }
	void linkType(SymbolType &symType);
	void appendSibling(CSymbol *symbol);

	void use();

	SymbolType *type() const { return m_type; }
	const char *identifier() const { return m_identifier; }
	CSymbol *next() const { return m_next; }
	Scope *scopeSeen() const { return m_scopeSeen; }
	Ast *initList() const { return m_initList; }
	bool isTypedef() const { return m_isTypedef; }
	bool isParameter() const { return m_isParameter; }

	unsigned int calculateNecessaryAllocSize() const;

	//utils
	static const char *generateParamName(int paramNumber);
	static const char *generateStructTag(int level);
};

}

#endif