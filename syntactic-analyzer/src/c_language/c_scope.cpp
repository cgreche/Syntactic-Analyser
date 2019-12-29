#include "c_scope.h"
#include "c_symboltype.h"
#include "c_symbol.h"

namespace C_language {

Scope::Scope(int block, int level)
: m_block(block), m_level(level),
  m_parent(NULL)
{

}



Scope::~Scope()
{
}



CSymbol *Scope::insertSymbol(CSymbol *sym)
{
	return m_symTab.insert(sym->identifier(),sym);
}



StructDef *Scope::insertStruct(StructDef *st)
{
	return m_structTab.insert(st->tag(),st);
}



CSymbol *Scope::findSymbol(const char *name)
{
	Scope *scope = this;
	while(scope) {
		CSymbol *sym = scope->m_symTab.find(name);
		if(sym)
			return sym;
		scope = scope->parent();
	}
	return NULL;
}



CSymbol *Scope::findSymbolWithinMainLevel(const char *name)
{
	return m_symTab.find(name);
}



StructDef *Scope::findStruct(const char *tag)
{
	Scope *scope = this;
	while(scope) {
		StructDef *st = scope->m_structTab.find(tag);
		if(st)
			return st;
		scope = scope->parent();
	}
	return NULL;
}



StructDef *Scope::findStructWithinMainLevel(const char *tag)
{
	return m_structTab.find(tag);
}

}