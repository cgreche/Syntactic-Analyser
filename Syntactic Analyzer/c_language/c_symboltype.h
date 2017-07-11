#ifndef __SYMBOLTYPE_H__
#define __SYMBOLTYPE_H__

#include "c_object.h"
#include "c_symbol.h"

namespace C_language {

class Scope;
class Ast;
class CSymbol;
class Statement;
class CompoundStatement;

enum StorageClass
{
	SC_UNSPECIFIED,
	SC_TYPEDEF, //not a real storage class, but good for syntactic/semantic checking
	SC_EXTERN,
	SC_STATIC,
	SC_AUTO,
	SC_REGISTER
};

#define SIGN_SIGNED 1
#define SIGN_UNSIGNED 2

#define STYPE_STRUCT 0
#define STYPE_UNION 1

enum DataType
{
	DT_UNSPECIFIED = 0,
	DT_VOID,
	DT_BOOL,
	DT_CHAR,
    DT_INT,
	DT_SHORTINT,
	DT_LONGINT,
	DT_LONGLONGINT,
    DT_STRUCT,
	DT_FLOAT,
    DT_DOUBLE
};

class TypeSpecifier;
class Pointer;
class Array;
class Function;

//Type categories:
#define TC_VOID						0x00000000
#define TC_BOOL						0x00000001
#define TC_CHAR						0x00000002
#define TC_STD_SIGNED_INT			0x00000004
#define TC_STD_UNSIGNED_INT			(0x00000008|TC_BOOL)
#define TC_STD_INT					(TC_STD_SIGNED_INT|TC_STD_UNSIGNED_INT)
#define TC_REAL_FLOAT				0x00000010
#define TC_COMPLEX					0x00000020
#define TC_FLOAT					(TC_REAL_FLOAT|TC_COMPLEX)
#define TC_BASIC					(TC_CHAR|TC_STD_SIGNED_INT|TC_STD_UNSIGNED_INT|TC_FLOAT)
#define TC_INT						(TC_CHAR|TC_STD_INT)
#define TC_REAL						(TC_INT|TC_REAL_FLOAT)
#define TC_ARITHMETIC				(TC_INT|TC_FLOAT)

#define TC_ARRAY					0x00000040
#define TC_STRUCT					0x00000080
#define TC_UNION					0x00000100
#define TC_FUNCTION					0x00000200
#define TC_POINTER					0x00000400
#define TC_DERIVED					(TC_ARRAY|TC_STRUCT|TC_UNION|TC_FUNCTION|TC_POINTER)

#define TC_SCALAR					(TC_ARITHMETIC|TC_POINTER)
#define TC_AGGREGATE				(TC_ARRAY|TC_STRUCT)

#define TC_DERIVED_DECLARATOR		(TC_ARRAY|TC_FUNCTION|TC_POINTER)

#define TC_EXTENTED					0x80000000

class SymbolType : public CObject
{

public:
	enum Type
	{
		SYMT_TYPESPECIFIER,
		SYMT_POINTER,
		SYMT_ARRAY,
		SYMT_FUNCTION
	};

protected:
	Type m_type;
	unsigned long m_typeCategory;

	//type qualifiers
	bool m_const;
	bool m_volatile;

	StorageClass m_storageClass;
	SymbolType *m_next;

	~SymbolType();

public:
	friend class CSymbol;
	SymbolType(Type type);
//	SymbolType(const SymbolType &symType);

	static TypeSpecifier* createTypeSpecifier();
	static Array* createArray(Ast *expr);
	static Pointer* createPointer();
	static Function* createFunction();

	void setStorageClass(StorageClass storageClass) { m_storageClass = storageClass; }
	void setConst(bool b) { m_const = b; }
	void setVolatile(bool b) { m_volatile = b; }
	void setNext(SymbolType *symType) { m_next = symType; }
	void linkType(SymbolType &symType);
	virtual SymbolType* merge(SymbolType& type);
	
	bool isTypeCompatibleWith(SymbolType *type);

	Type type() const { return m_type; }
	unsigned long typeCategory() const { return m_typeCategory; }
	bool isConst() const { return m_const; }
	bool isVolatile() const { return m_volatile; }
	StorageClass storageClass() const { return m_storageClass; }
	SymbolType *next() const { return m_next; }

	virtual unsigned int sizeOf() const = 0;

	inline bool isInteger() const;
	inline bool isReal() const;
	inline bool isArithmetic() const;
	inline bool isPointer() const;
	inline bool isScalar() const;
	inline bool isAggregate() const;
};

inline bool SymbolType::isInteger() const {	return (m_typeCategory&TC_INT) != 0; }
inline bool SymbolType::isReal() const { return (m_typeCategory&TC_REAL) != 0; }
inline bool SymbolType::isArithmetic() const { return (m_typeCategory&TC_ARITHMETIC) != 0; }
inline bool SymbolType::isPointer() const { return m_typeCategory == TC_POINTER; }
inline bool SymbolType::isScalar() const { return (m_typeCategory&TC_SCALAR) != 0; }
inline bool SymbolType::isAggregate() const { return (m_typeCategory&TC_AGGREGATE) != 0; }

class StructDef : public CObject
{
	char m_tag[256];
	unsigned long m_stType;
	bool m_complete;
	CSymbolList m_symbolList;
	Scope *m_scopeSeen;

	~StructDef();

public:
	StructDef();

	void setTag(const char *tag);
	void setType(unsigned long type) { m_stType = type; }
	void setScopeSeen(Scope *scope) { m_scopeSeen = scope; }
	void insertSymbol(CSymbol* symbol) { m_symbolList.push_back(symbol); }
	void setComplete(bool complete) { m_complete = complete; }
	
	const char *tag() const { return m_tag; }
	unsigned long type() const { return m_stType; }
	bool isComplete() const { return m_complete; }
	Scope* scopeSeen() const { return m_scopeSeen; }
	unsigned int symbolCount() const { return m_symbolList.size(); }

	CSymbol *findMember(const char *member) const;
	unsigned int computeSize() const;
};



class TypeSpecifier : public SymbolType
{
	DataType m_dataType;
	union {
		StructDef *m_structDef;
		int m_sign;
	};

public:
	TypeSpecifier();

	bool isDataTypeCompatibleWith(DataType type);

	//to be used with int/char data types
	void setDataType(DataType type);
	void setSign(int sign);
	DataType dataType() const { return m_dataType; }
	int sign() const { return m_sign; }
	bool isUnsigned() const { return m_sign == SIGN_UNSIGNED; }
	
	//to be used with DT_STRUCT data type
	void setStructDef(StructDef *stdef);
	StructDef *structDef() const { return m_structDef; }

	inline bool isDataTypeInteger() const;
	inline bool isDataTypeCharacter() const;
	inline bool isDataTypeReal() const;

	//SymbolType overrides
	virtual unsigned int sizeOf() const;
};

inline bool TypeSpecifier::isDataTypeInteger() const
{
	DataType type = m_dataType;
	return type == DT_INT || type == DT_SHORTINT || type == DT_LONGINT || type == DT_LONGLONGINT || type == DT_CHAR;
}

inline bool TypeSpecifier::isDataTypeCharacter() const
{
	return m_dataType == DT_CHAR;
}

inline bool TypeSpecifier::isDataTypeReal() const
{
	return m_dataType == DT_FLOAT || m_dataType == DT_DOUBLE;
}


class Pointer : public SymbolType
{
public:
	Pointer();

	//SymbolType overrides
	virtual unsigned int sizeOf() const;
};



class Array : public SymbolType
{
	unsigned int m_elemCount;

public:
	Array();

	//SymbolType overrides
	virtual unsigned int sizeOf() const;

	//
	void setElementCount(unsigned int elemCount) { m_elemCount = elemCount; }
	unsigned int elemCount() const { return m_elemCount; }
};



class Function : public SymbolType
{
	CSymbol *m_paramList; //linked list of parameters
	int m_paramCount; //number of parameters
	bool m_hasVArgs; //has variable arguments?
	CompoundStatement *m_body; //function body

public:
	Function();

	//SymbolType overrides
	virtual unsigned int sizeOf() const;

	//
	void setParameters(CSymbol *paramList, bool hasVArgs);
	void setFunctionBody(CompoundStatement *body) { m_body = body; }
	CSymbol *paramList() const { return m_paramList; }
	CSymbol *param(int paramIndex) const;
	int paramCount() const { return m_paramCount; }
	bool hasVArgs() const { return m_hasVArgs; }
	CompoundStatement *functionBody() const { return m_body; }
};

}

#endif