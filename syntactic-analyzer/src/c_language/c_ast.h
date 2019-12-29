#ifndef __AST_H__
#define __AST_H__

#include "c_defs.h"
#include "c_object.h"
#include "c_value.h"
#include "c_symboltype.h"
#include "c_symbol.h"

namespace C_language {

enum ASTTYPE
{
	ASTT_VALUE,
	ASTT_OPERATOR,
};

class Ast : public CObject
{
	ASTTYPE m_type;
	union {
		Value *value;
		C_operator op;
	}m_value;

	Ast *m_left;
	Ast *m_right;

	Ast *m_next; //for argument expression list

	bool m_evaluated; //was this evaluated?
	bool m_valid; //valid operation?
	SymbolType *m_resultingType; //type after evaluated
	Value *m_resultingValue;

	//private constructor
	Ast(ASTTYPE type);

public:
	~Ast();

	static Ast* newOperation(C_operator op, Ast* left, Ast* right);
	static Ast* newValue(Value &value);

	void appendSibling(Ast *ast);

	Value *evaluate();
	SymbolType *getResultingType();

	inline ASTTYPE type() const { return m_type; }
	inline Value *value() const { return m_value.value; }
	inline C_operator op() const { return m_value.op; }
	inline Ast *left() const { return m_left; }
	inline Ast *right() const { return m_right; }
	inline Ast *next() const { return m_next; }
	

	bool isInteger();
	bool isArithmetic();
	bool isReal();
	bool isPointer();
	bool isScalar();
	bool isAggregate();
	bool isModifiable();
	static bool checkOperandsCompatibility(int op, Ast *leftOperand, Ast *rightOperand);

	inline bool isAssignmentExpression() const { return m_type == ASTT_OPERATOR &&
		(m_value.op >= OP_ASSIGN && m_value.op <= OP_RSHIFT_ASSIGN); }
};

}

#endif