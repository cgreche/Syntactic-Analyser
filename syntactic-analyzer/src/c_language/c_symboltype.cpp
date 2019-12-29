
//Last edit: 05/06/2017

#include <cassert>

#include "c_defs.h"
#include "c_ast.h"
#include "c_symboltype.h"

namespace C_language {

SymbolType::SymbolType(Type type)
: m_type(type),
  m_typeCategory(0),
  m_const(false), m_volatile(false),
  m_storageClass(SC_UNSPECIFIED),
  m_next(0)
{
}

/*
SymbolType::SymbolType(const SymbolType &symType)
{
	SymbolType *type = (SymbolType*)&symType;
	SymbolType *type2 = this;
	for(;;) {
		*type2 = *type; //make an exact copy
		type = type->next();
		if(!type)
			break;
		type2->m_next = new SymbolType(type->type());
		type2 = type2->next();
	}
}
*/
SymbolType::~SymbolType()
{
	if(m_next)
		m_next->release();
}

SymbolType* SymbolType::merge(SymbolType& type)
{
	if(m_storageClass != SC_UNSPECIFIED && type.m_storageClass != SC_UNSPECIFIED) {
		//error, more than one storage class defined
	}

	if(type.m_storageClass != SC_UNSPECIFIED)
		m_storageClass = type.m_storageClass;

	m_const |= type.m_const;
	m_volatile |= type.m_volatile;

	if(m_type == SYMT_TYPESPECIFIER && type.type() == SYMT_TYPESPECIFIER) {
		TypeSpecifier *t1 = (TypeSpecifier*)this;
		TypeSpecifier *t2 = (TypeSpecifier*)&type;

		if(t1->dataType() != DT_UNSPECIFIED && t2->dataType() != DT_UNSPECIFIED) {
			//error, data type is already defined
		}

		if(t1->sign() != 0 && t2->sign() != 0) {
			//error, type is already signed/unsigned
		}

		if(t2->dataType() != DT_UNSPECIFIED)
			t1->setDataType(t2->dataType());

		if(t2->sign() != 0)
			t1->setSign(t2->sign());
	}

	type.release();
	return this;
}

void SymbolType::linkType(SymbolType &symType)
{
	SymbolType *thisType = this;
	SymbolType *type = &symType;

	while(thisType->next())
		thisType = thisType->next();

	thisType->setNext(&symType);
	symType.addRef();

	/*
	for(;;) {
		thisType->setNext(new SymbolType(*type));
		if(!type->next())
			break;
		type = type->next();
		thisType = thisType->next();
	}
	*/

}

bool SymbolType::isTypeCompatibleWith(SymbolType *type)
{
	//todo: complete
	bool res;

	if(type == NULL)
		return false;

	if(m_type == SYMT_TYPESPECIFIER && type->type() == SYMT_TYPESPECIFIER) {
		res = ((TypeSpecifier*)this)->isDataTypeCompatibleWith(((TypeSpecifier*)type)->dataType());
	}
	else if(m_type == SYMT_POINTER) {
		if(type->type() == SYMT_POINTER) {
			if(type->isConst() && !m_const)
				res = false;
			if(type->isVolatile() && !m_volatile)
				res = false;
		}else if(type->type() == SYMT_ARRAY) {
			res = true;
		}
		//check if our type is a pointer to void, which is compatible which any pointer type
		if(m_next && m_next->type() == SYMT_TYPESPECIFIER) {
			if(!type->next() || type->next()->type() != SYMT_TYPESPECIFIER)
				return false;
			//according to the standard, both operands can have pointer to void type
			if(((TypeSpecifier*)m_next)->dataType() == DT_VOID || ((TypeSpecifier*)type->next())->dataType() == DT_VOID) {
				if(!m_next->next())
					return type->next()->next() == NULL;
				return m_next->next()->isTypeCompatibleWith(type->next()->next());
			}
			// else, fall through
		}
		
		res = true;
	}
	else if(m_type == SYMT_ARRAY) {
	}
	else if(m_type == SYMT_FUNCTION) {
		return false;
	}
	else {
		return false;
	}

	if(!res)
		return false;
	//there is not a next type? If the other type doesn't have a next type also, success
	if(!m_next)
		return type->next() == NULL;
	return m_next->isTypeCompatibleWith(type->next());
}

//static
TypeSpecifier *SymbolType::createTypeSpecifier()
{
	TypeSpecifier *type = new TypeSpecifier;
	return type;
}

//static 
Pointer *SymbolType::createPointer()
{
	Pointer *type = new Pointer;
	return type;
}

//static
Array* SymbolType::createArray(Ast *expr)
{
	//todo: fix for constant expressions
	Array *type = new Array;

	if(!expr)
		return type;

	long elemCount;

	Value *value = expr->evaluate();
	if(!value) {
		//log error: cannot allocate an array of 0 elements
		return type;
	}

	if(value->valueType() != LITERAL) {
		//log error: not a constant value
		return type;
	}

	//constant expression must be an integer constant expression
	if(!value->isIntegral()) {
		//log error: not an integer value
		elemCount = 0;
	}
	else {
		elemCount = value->longVal();
		if(elemCount == 0) {
			//log error: cannot allocate an array of 0 elements
		}
		else {
			//Here, we are assuming value->symbolType IS a TypeSpecifier and compatible with int
			if(!((TypeSpecifier*)value->symbolType())->isUnsigned()) {
				if((int)elemCount < 0) {
					//log error: negative value
					elemCount = 0;
				}
			}
		}
	}

	value->release();

	type->setElementCount(elemCount);
	return type;
}

//static
Function *SymbolType::createFunction()
{
	Function *type = new Function;
	return type;
}



StructDef::StructDef()
: m_stType(0), m_complete(false)
{
	m_tag[0] = '\0';
}

StructDef::~StructDef()
{

}

void StructDef::setTag(const char *tag)
{
	memcpy(m_tag,tag,256);
	m_tag[255] = '\0';
}

CSymbol *StructDef::findMember(const char *member) const
{
	//todo: change to symbol table lookup way?
	int i;
	CSymbol *sym;
	for(i = 0; i < m_symbolList.size(); ++i) {
		sym = m_symbolList[i];
		if(strcmp(member,sym->identifier()) == 0)
			return sym;
	}
	return NULL;
}

unsigned int StructDef::computeSize() const
{
	//todo
	return 0;
}



TypeSpecifier::TypeSpecifier()
: SymbolType(SymbolType::SYMT_TYPESPECIFIER)
, m_dataType(DT_UNSPECIFIED)
{
	m_sign = 0;
}

void TypeSpecifier::setDataType(DataType type)
{
	m_dataType = type;
	switch(type) {
		case DT_BOOL:
			m_typeCategory = TC_BOOL;
			break;

		case DT_CHAR:
			m_typeCategory = TC_CHAR;
			break;

		case DT_INT:
		case DT_SHORTINT:
		case DT_LONGINT:
		case DT_LONGLONGINT:
			m_typeCategory = TC_STD_SIGNED_INT;
			break;

		case DT_FLOAT:
		case DT_DOUBLE:
			m_typeCategory = TC_REAL_FLOAT;
			break;

		case DT_STRUCT:
			m_typeCategory = TC_STRUCT;
			break;
	}
}

void TypeSpecifier::setSign(int sign)
{
	m_sign = sign;
	if(m_typeCategory&TC_STD_INT) {
		m_typeCategory = (sign == SIGN_UNSIGNED) ? TC_STD_UNSIGNED_INT : TC_STD_SIGNED_INT;
	}
}

void TypeSpecifier::setStructDef(StructDef *stdef)
{
	m_structDef = stdef;
	if(stdef) {
		m_typeCategory = (stdef->type() == STYPE_UNION) ? TC_UNION : TC_STRUCT;
	}
}

bool TypeSpecifier::isDataTypeCompatibleWith(DataType type)
{
	DataType myType = m_dataType;
	bool type1isInt = type == DT_INT || type == DT_SHORTINT || type == DT_LONGINT || type == DT_LONGLONGINT || type == DT_CHAR;
	bool type2isInt = myType == DT_INT || myType == DT_SHORTINT || myType == DT_LONGINT || myType == DT_LONGLONGINT || myType == DT_CHAR;
	if(type1isInt && type2isInt)
		return true;

	bool type1isFloat = type == DT_FLOAT || type == DT_DOUBLE;
	bool type2isFloat = myType == DT_FLOAT || myType == DT_DOUBLE;
	if(type1isFloat && type2isFloat)
		return true;

	return false;
}

unsigned int TypeSpecifier::sizeOf() const
{
	switch(m_dataType) {
		case DT_INT:
			return 4;

		case DT_SHORTINT:
			return 2;

		case DT_LONGINT:
			return 4;

		case DT_LONGLONGINT:
			return 8;

		case DT_CHAR:
			return 1; //todo: change?

		case DT_VOID:
			return 0;

		case DT_STRUCT:
			return m_structDef->computeSize();

		case DT_FLOAT:
			return 4;

		case DT_DOUBLE:
			return 8;
	}

	return 0;
}



Pointer::Pointer()
: SymbolType(SymbolType::SYMT_POINTER)
{
	m_typeCategory = TC_POINTER;
}

//virtual
unsigned int Pointer::sizeOf() const
{
	//todo: change?
	return 4;
}



Array::Array()
: SymbolType(SymbolType::SYMT_ARRAY)
, m_elemCount(0)
{
	m_typeCategory = TC_ARRAY;
}

unsigned int Array::sizeOf() const
{
	assert(m_next != NULL);
	unsigned int typeSize = m_next->sizeOf();
	return typeSize*m_elemCount;
}



Function::Function()
: SymbolType(SymbolType::SYMT_FUNCTION),
  m_paramList(NULL),
  m_paramCount(0),
  m_hasVArgs(false),
  m_body(NULL)
{
	m_typeCategory = TC_FUNCTION;
}

void Function::setParameters(CSymbol *paramList, bool hasVArgs)
{
	int paramCount;

	m_paramList = paramList;
	m_hasVArgs = hasVArgs;

	paramCount = 0;
	while(paramList) {
		++paramCount;
		paramList = paramList->next();
	}

	m_paramCount = paramCount;
}

CSymbol *Function::param(int paramIndex) const
{
	CSymbol *param;

	assert(paramIndex < 0);
	assert(paramIndex >= m_paramCount);

	param = m_paramList;
	while(paramIndex > 0) {
		--paramIndex;
		param = param->next();
	}

	return param;
}

//virtual
unsigned int Function::sizeOf() const
{
	//illegal use of sizeof operand
	return 0;
}

}