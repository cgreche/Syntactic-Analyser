#ifndef __DEFS_H__
#define __DEFS_H__

#define NULL 0

namespace C_language {

enum C_operator
{
	OP_UNARY_PLUS,
	OP_UNARY_MINUS,
	OP_ADD,
	OP_SUB,
	OP_MUL,
	OP_DIV,
	OP_MOD,

	OP_INC,
	OP_DEC,

	OP_ASSIGN,
	OP_ADD_ASSIGN,
	OP_SUB_ASSIGN,
	OP_MUL_ASSIGN,
	OP_DIV_ASSIGN,
	OP_MOD_ASSIGN,
	OP_AND_ASSIGN,
	OP_OR_ASSIGN,
	OP_XOR_ASSIGN,
	OP_LSHIFT_ASSIGN,
	OP_RSHIFT_ASSIGN,

	OP_EQ,
	OP_GT,
	OP_LT,
	OP_NE,
	OP_GE,
	OP_LE,

	OP_LOGICAL_AND,
	OP_LOGICAL_OR,
	OP_LOGICAL_NOT,

	OP_BITWISE_AND,
	OP_BITWISE_OR,
	OP_BITWISE_XOR,
	OP_BITWISE_NOT,
	OP_LSHIFT,
	OP_RSHIFT,

	OP_ARRAY_SUBSCRIPT,
	OP_DEREF, //indirection
	OP_REF, //address
	OP_MEMBER_REF,
	OP_MEMBER_POINT,

	OP_CALL,
	OP_COMMA,
	OP_COND,
	OP_COND2,
	OP_SIZEOF,
	OP_Ast,
};

}

#endif