
#include <cassert>
#include "c_ast.h"

namespace C_language {

Ast::Ast(ASTTYPE type)
: m_type(type),
  m_left(NULL), m_right(NULL),
  m_next(NULL),
  m_resultingType(NULL)
{

}

Ast::~Ast()
{
	if(m_left)
		m_left->release();
	if(m_right)
		m_right->release();
	if(m_resultingType)
		m_resultingType->release();
}

Ast *Ast::newOperation(C_operator op, Ast *left, Ast *right)
{
	checkOperandsCompatibility(op,left,right);

	Ast *ret = new Ast(ASTT_OPERATOR);
	if(!ret) {
		//do something
	}
	ret->m_value.op = op;
	ret->m_left = left;
	ret->m_right = right;
	return ret;
}

Ast *Ast::newValue(Value &value)
{
	Ast *ret = new Ast(ASTT_VALUE);
	if(!ret) {
		//do something
	}
	ret->m_value.value = &value;
	return ret;
}

void Ast::appendSibling(Ast *ast)
{
	Ast *cur = this;
	while(cur->next())
		cur = cur->next();
	cur->m_next = ast;
}

Value *Ast::evaluate()
{
	Value *ret = NULL;
	if(m_type == ASTT_VALUE) {
		Value *val = m_value.value;
		ValueType valType = val->valueType();
		SymbolType *symType = val->symbolType();

		if(valType == LITERAL) {
			symType->addRef();
			val->addRef();
			m_resultingType = symType;
			return val; //return self
		}
		else if(valType == VARIABLE) {
			CSymbol *sym = val->symbolVal();
			Ast *initList = sym->initList();

			ret = initList->evaluate();
			return ret;

		}
		else if(valType == ABSTRACT) {
		}
	}
	else if(m_type == ASTT_OPERATOR) {
		//todo
	}
	else {
		return NULL;
	}

	return ret;
}



SymbolType *Ast::getResultingType()
{
	if(type() == ASTT_VALUE) {
		//return direct type
		Value *val = value();
		SymbolType *type = val->symbolType();
		if(!type)
			return NULL;
		
		SymbolType *retType = type;
		type->addRef();
		return retType;
	}

	if(type() == ASTT_OPERATOR) {
		SymbolType *op1type = m_left ? m_left->getResultingType() : NULL;
		SymbolType *op2type = m_right ? m_right->getResultingType() : NULL;

		switch(op()) {
			case OP_UNARY_PLUS:
			case OP_UNARY_MINUS:
				return NULL;

			case OP_ADD:
			case OP_SUB:
				{
					assert(op1type && op2type);

					if(op1type->isArithmetic() || op2type->isArithmetic()) {

					}
					
					if(op1type->isInteger() && op2type->isInteger()) {
					}

					if(op1type->isPointer() && op2type->isInteger()) {
						return op1type;
					}

					if(op() == OP_SUB && op1type->isPointer() && op2type->isPointer()) {
						//todo: check pointers  compatibility
						return op1type;
					}


					return NULL;
				}

			case OP_MUL:
			case OP_DIV:
			case OP_MOD:
				{
					return NULL;
				}

			case OP_ARRAY_SUBSCRIPT:
				break;
		}
	}
	return NULL;
}



bool Ast::isInteger()
{
	SymbolType *type = getResultingType();
	if(!type)
		return false;
	return type->isInteger();
}

bool Ast::isArithmetic()
{
	SymbolType *type = getResultingType();
	if(!type)
		return false;
	return type->isArithmetic();
}

bool Ast::isReal()
{
	SymbolType *type = getResultingType();
	if(!type)
		return false;
	return type->isReal();
}

bool Ast::isPointer()
{
	SymbolType *type = getResultingType();
	if(!type)
		return false;
	return type->isPointer();
}

bool Ast::isScalar()
{
	SymbolType *type = getResultingType();
	if(!type)
		return false;
	return type->isScalar();
}

bool Ast::isAggregate()
{
	SymbolType *type = getResultingType();
	if(!type)
		return false;
	return type->isAggregate();
}

bool Ast::isModifiable()
{
	//to-do
	return false;
}


//static
bool Ast::checkOperandsCompatibility(int op, Ast *leftOperand, Ast *rightOperand)
{
	switch(op) {
		case OP_UNARY_PLUS:
		case OP_UNARY_MINUS:
			{
				assert(rightOperand);
				return rightOperand->isArithmetic();
			}

		case OP_ADD:
		case OP_SUB:
			{
				assert(leftOperand && rightOperand);
				SymbolType *op1type = leftOperand->getResultingType();
				SymbolType *op2type = rightOperand->getResultingType();

				if(op1type->isArithmetic() && op2type->isArithmetic())
					return true;

				if(op == OP_ADD) {
					if(op1type->isPointer() && op2type->isInteger())
						return true;

					if(op1type->isInteger() && op2type->isPointer())
						return true;

					return false;
				}

				//op == OP_SUB (to-do: complete, check the Standards file for reference)
				if(op1type->isPointer() && op2type->isPointer() && op1type->isTypeCompatibleWith(op2type))
					return true;

				if(op1type->isPointer() && op2type->isInteger())
					return true;

				return false;
			}

		case OP_MUL:
		case OP_DIV:
		case OP_MOD:
			{
				assert(leftOperand && rightOperand);

				if(op == OP_MOD)
					return leftOperand->isInteger() && rightOperand->isInteger();

				return leftOperand->isArithmetic() && rightOperand->isArithmetic();
			}

		case OP_INC:
		case OP_DEC:
			{
				assert(rightOperand);
				return (rightOperand->isReal() || rightOperand->isPointer())
					&& rightOperand->isModifiable();
			}


		case OP_ASSIGN:
		case OP_ADD_ASSIGN:
		case OP_SUB_ASSIGN:
		case OP_MUL_ASSIGN:
		case OP_DIV_ASSIGN:
		case OP_MOD_ASSIGN:
		case OP_AND_ASSIGN:
		case OP_OR_ASSIGN:
		case OP_XOR_ASSIGN:
		case OP_LSHIFT_ASSIGN:
		case OP_RSHIFT_ASSIGN:
			break;

		case OP_EQ:
		case OP_NE:
			{
				assert(leftOperand && rightOperand);
				SymbolType *op1Type = leftOperand->getResultingType();
				SymbolType *op2Type = rightOperand->getResultingType();

				if(op1Type->isArithmetic() && op2Type->isArithmetic())
					return true;
				
				//— both operands are pointers to qualified or unqualified versions of compatible types;
				//— one operand is a pointer to an object or incomplete type and the other is a pointer to a
				//qualified or unqualified version of void; or
				//— one operand is a pointer and the other is a null pointer constant.
				break;
			}

		case OP_GT:
		case OP_LT:
		case OP_GE:
		case OP_LE:
			{
				assert(leftOperand && rightOperand);
				SymbolType *op1Type = leftOperand->getResultingType();
				SymbolType *op2Type = rightOperand->getResultingType();

				if(op1Type->isReal() && op2Type->isReal())
					return true;

				//to-do:
				//both operands are pointers to qualified or unqualified versions of compatible object
				//types; or

				//both operands are pointers to qualified or unqualified versions of compatible
				//incomplete types.
				break;
			}

		case OP_LOGICAL_AND:
		case OP_LOGICAL_OR:
			{
				assert(leftOperand && rightOperand);
				return leftOperand->isScalar() && rightOperand->isScalar();
			}

		case OP_LOGICAL_NOT:
			{
				assert(rightOperand);
				return rightOperand->isScalar();
			}

		case OP_BITWISE_AND:
		case OP_BITWISE_OR:
		case OP_BITWISE_XOR:
			{
				assert(rightOperand);
				return rightOperand->isInteger();
			}

		case OP_BITWISE_NOT:
			{
				assert(rightOperand);
				return rightOperand->isInteger();
			}

		case OP_LSHIFT:
		case OP_RSHIFT:
			{
				assert(leftOperand && rightOperand);
				return leftOperand->isInteger() && rightOperand->isInteger();
			}

		case OP_ARRAY_SUBSCRIPT:
			break;

		case OP_DEREF: //indirection
			{
				assert(rightOperand);
				return rightOperand->isPointer();
			}

		case OP_REF: //address
			{
				break;
			}
	}

	return false;
}

}
