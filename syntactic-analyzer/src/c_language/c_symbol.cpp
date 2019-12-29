
#include "c_symbol.h"
#include "c_ast.h"

#include <cstring>

namespace C_language {

CSymbol::CSymbol()
: m_type(0),
  m_scopeSeen(NULL),
  m_next(0),
  m_isTypedef(false),
  m_isParameter(false),
  m_paramIndex(0),
  m_useCount(0)
{
	m_identifier[0] = '\0';
}

CSymbol::~CSymbol()
{
	m_type->release();
}

void CSymbol::linkType(SymbolType &symType)
{
	if(!m_type) {
		m_type = &symType;
		symType.addRef();
		return;
	}

	m_type->linkType(symType);
}

void CSymbol::setIdentifier(const char *identifier)
{
	memcpy(m_identifier,identifier,256);
	m_identifier[255] = '\0';
}

void CSymbol::appendSibling(CSymbol *symbol)
{
	if(!symbol)
		return;
	CSymbol *sym = this;
	while(sym->next()) sym = sym->next();
	sym->m_next = symbol;
}

void CSymbol::use()
{
	++m_useCount;
}


/*
bool CSymbol::isTypedef()
{
	SymbolType *type = m_type;
	while(type) {
		if(type->type() == SymbolType::SYMT_TYPESPECIFIER) {
			return type->isTypedef();
		}
		type = type->next();
	}
	return false;
}
*/

unsigned int CSymbol::calculateNecessaryAllocSize() const
{
	return m_type->sizeOf();
}

//static
const char *CSymbol::generateParamName(int paramNumber)
{
	static char name[256];
	sprintf(name,"@unnamedparam%d",paramNumber);
	return name;
}

//static
const char *CSymbol::generateStructTag(int level)
{
	static char name[256];
	static int stCount = 0;
	sprintf(name,"@unnamedsttag%d_%d",stCount++,level);
	return name;
}

}