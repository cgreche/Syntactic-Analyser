#ifndef __C_VALUE_H__
#define __C_VALUE_H__

#include "c_object.h"
#include "c_symbol.h"

namespace C_language {

enum ValueType
{
	LITERAL, //literal values/strings
	VARIABLE, //for symbols
	ABSTRACT
};

class Value : public CObject
{

private:
	ValueType m_valType;
	SymbolType *m_symType;
	union
	{
		long lval;
		CSymbol *sym;
		char *strVal;
	} m_value;

	Value(ValueType type);
	~Value();

public:
	friend class Ast;

	static Value *fromConstant(const char *str);
	static Value *fromString(const char *str);
	static Value *fromChar(const char *str);
	static Value *fromSymbol(CSymbol *sym);
	static Value *fromSymbolType(SymbolType *symType);

	ValueType valueType() const { return m_valType; }
	SymbolType *symbolType() const { return m_symType; }
	long longVal() const { return m_value.lval; }
	CSymbol *symbolVal() const { return m_value.sym; }

	void setLVal(long lval) { m_value.lval = lval; }

	bool isIntegral() const;
	bool isRealFloatingType() const;
	bool isArithmeticType() const;
	bool isScalarType() const;
	bool isAggregateType() const;
};

}

#endif