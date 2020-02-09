#ifndef __C_SCOPE_H__
#define __C_SCOPE_H__

#include "../../project/vs2019/CParser/lib/nametable.h"

#include "c_object.h"

namespace C_language {

class CSymbol;
class StructDef;

class Scope : public CObject
{
	Scope(const Scope&); //disable copying
//	Scope& operator=(const Scope&);

private:
	nametable<CSymbol *> m_symTab; // use with (CSymbol*) types
	nametable<StructDef *> m_structTab; //use with (StructDef*) types
	int m_level;
	int m_block;
	Scope *m_parent;

public:
	Scope(int block, int level);
	~Scope();

	void setParent(Scope *parent) { m_parent = parent; }
	CSymbol *insertSymbol(CSymbol *sym); //identifiers, typedef names
	StructDef *insertStruct(StructDef *st); //struct, union, enum tags
	int block() const { return m_block; }
	int level() const { return m_level; }

	CSymbol *findSymbol(const char *name);
	CSymbol *findSymbolWithinMainLevel(const char *name);
	StructDef *findStruct(const char *tag);
	StructDef *findStructWithinMainLevel(const char *tag);

	Scope *parent() const { return m_parent; }

	unsigned int symbolCount() const { return m_symTab.elemCount(); }
	unsigned int structCount() const { return m_structTab.elemCount(); }
};

}

#endif