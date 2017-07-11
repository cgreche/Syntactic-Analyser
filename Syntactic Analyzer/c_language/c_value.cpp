
#include "../src/lib/string.h"
#include "c_value.h"
#include "c_symboltype.h"

#include <cstdlib>
#include <cassert>

//
/*
There are five standard signed integer types, designated as signed char, short
int, int, long int, and long long int. (These and other types may be
designated in several additional ways, as described in 6.7.2.) There may also be
implementation-defined extended signed integer types.28) The standard and extended
signed integer types are collectively called signed integer types.29)

The type
_Bool and the unsigned integer types that correspond to the standard signed integer
types are the standard unsigned integer types

The standard signed integer types and standard unsigned integer types are collectively
called the standard integer types

There are three real floating types, designated as float, double, and long
double.

The real floating and complex types
are collectively called the floating types.

The type char, the signed and unsigned integer types, and the floating types are
collectively called the basic types

The three types char, signed char, and unsigned char are collectively called
the character types

The type char, the signed and unsigned integer types, and the enumerated types are
collectively called integer types. The integer and real floating types are collectively called
real types.

Integer and floating types are collectively called arithmetic types. Each arithmetic type
belongs to one type domain: the real type domain comprises the real types, the complex
type domain comprises the complex types.

The void type comprises an empty set of values; it is an incomplete type that cannot be
completed.

Arithmetic types and pointer types are collectively called scalar types. Array and
structure types are collectively called aggregate types.37)
*/

namespace C_language {

Value::Value(ValueType type)
: m_valType(type),
  m_symType(NULL)
{
}

Value::~Value()
{
	//todo: revise and complete
	if(m_valType == LITERAL) {
		if(m_symType->type() == SymbolType::SYMT_ARRAY) {
			SymbolType *typespecifier = m_symType->next();
			while(typespecifier->type() != SymbolType::SYMT_TYPESPECIFIER) typespecifier = typespecifier->next();
			if(((TypeSpecifier*)typespecifier)->dataType() == DT_CHAR)
				delete [] m_value.strVal;
		}
		
	}

	m_symType->release();
}

//static
Value *Value::fromConstant(const char *str)
{
	Value *ret = new Value(LITERAL);
	TypeSpecifier *type = SymbolType::createTypeSpecifier();
	type->setDataType(DT_INT);
	type->setConst(true);
	ret->m_symType = type;
	
	char *c = (char*)str;
	if(*c == '0') {
		++c;
		if(*c == 'x') {
			//is hex
		}
		else if(*c >= '0' && *c <= '7') {
			//is octal
		}
	}
	else {
		//is decimal
	}

	ret->m_value.lval = atol(str);

	return ret;
}

//static
Value *Value::fromString(const char *str)
{
	Value *ret = new Value(LITERAL);
	Array *arrayType = SymbolType::createArray(NULL);;
	TypeSpecifier *charType = SymbolType::createTypeSpecifier();
	charType->setDataType(DT_CHAR);
	charType->setConst(true);
	arrayType->setNext(charType);

	string resStr;

	char *c = (char*)str;
	++c; //ignore first ' " '
	while(*c) {
		if(*c == '\"')
			break;
		if(*c == '\\') {
				++c;
				switch(*c) {
					case 'a': //alert
					case 'b': //backspace
					case 'f': //form feed
					case 'n': //new line
					case 'r': //carriage return
					case 't': //horizontal tab
					case 'v': //vertical tab
						resStr += *c;
						break;

					case '\'':
					case '\"':
					case '\?':
					case '\\':
						resStr += *c;
						break;

					case 'x':
						break;

					default:
						{
							//todo: complete
							if(*c >= '0' && *c <= '7') {
								//is octal
							}
							else {
								//invalid character escape sequence
							}

							break;
						}
				}
				break;
		}
		else {
			//char
			resStr += *c;
		}
		++c;
	}

	if(resStr.data()[resStr.length()] != '\0')
		resStr += '\0';

	int strSize = resStr.size();

	arrayType->setElementCount(strSize);
	ret->m_symType = arrayType;
	ret->m_value.strVal = new char[strSize];
	string_op::copy(ret->m_value.strVal,resStr.data(),strSize);
	return ret;
}

//static
Value *Value::fromChar(const char *str)
{
	Value *ret = new Value(LITERAL);
	return ret;
}

//static
Value *Value::fromSymbol(CSymbol *sym)
{
	Value *ret = new Value(VARIABLE);
	ret->m_value.sym = sym;
	if(sym->type()) {
		ret->m_symType = sym->type();
		ret->m_symType->addRef();
	}
	return ret;
}

//static
Value *Value::fromSymbolType(SymbolType *symType)
{
	Value *ret = new Value(VARIABLE);
	ret->m_symType = symType;
	ret->m_value.sym = NULL;
	symType->addRef();
	return ret;
}

bool Value::isIntegral() const
{
	assert(m_symType != NULL);
	return (m_symType->typeCategory()&TC_INT) != 0;
}

bool Value::isRealFloatingType() const
{
	assert(m_symType != NULL);
	return m_symType->typeCategory() == TC_REAL_FLOAT;
}

bool Value::isArithmeticType() const
{
	assert(m_symType != NULL);
	return m_symType->typeCategory() == TC_POINTER || (m_symType->typeCategory()&TC_INT);
}

bool Value::isScalarType() const
{
	assert(m_symType != NULL);
	return m_symType->typeCategory() == TC_SCALAR;
}

bool Value::isAggregateType() const
{
	assert(m_symType != NULL);
	return m_symType->typeCategory() == TC_AGGREGATE;
}

}