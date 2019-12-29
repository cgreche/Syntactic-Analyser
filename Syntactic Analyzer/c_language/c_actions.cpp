
//Last edit: 18/02/2018 03:26

#include "../src/languageparser.h"
#include "../src/lib/stack.h"
#include "../src/lib/nametable.h"

#include "c_language.h"

namespace C_language {

using namespace syntacticanalyzer;

int g_expectIdentifier = 0;

int g_funcParamCounter = 0;
bool g_thisFuncHasVargs = false;

//primary_expression

SEMANTIC_ACTION_C(primary_expression1, IDENTIFIER)
{
	int a = 1;
	const char *name = state->semanticStack[state->semanticArgsIndex].text;
	CSymbol *sym = C_curScope()->findSymbol(name);
	if(!sym) {
		//undeclared idenifier
		printf("undeclared identifier: %s\n",name);
		int err = 1;
		SRET(NULL);
		return;
	}
	sym->use(); //increment use count

	Value *value = Value::fromSymbol(sym);
	SRET(Ast::newValue(*value));
}

SEMANTIC_ACTION_C(primary_expression2, CONSTANT)
{
	const char *str = SARG_TEXT(0);
	Value *value = Value::fromConstant(str);
	SRET(Ast::newValue(*value));
}

SEMANTIC_ACTION_C(primary_expression3, STRING_LITERAL)
{
	const char *str = SARG_TEXT(0);
	Value *value = Value::fromString(str);
	SRET(Ast::newValue(*value));
}

SEMANTIC_ACTION_C(primary_expression4, '(' expression ')')
{
	SRET(SARG(1,Ast*));
}



//postfix_expression

SEMANTIC_ACTION_C(postfix_expression1, primary_expression)
{
}

SEMANTIC_ACTION_C(postfix_expression2, postfix_expression '[' expression ']')
{
	Ast *expr1 = SARG(0,Ast*);
	Ast *expr2 = SARG(2,Ast*);
	SRET(Ast::newOperation(OP_ARRAY_SUBSCRIPT,expr1,expr2));
}

SEMANTIC_ACTION_C(postfix_expression3, postfix_expression '(' ')')
{
	Ast *expr = SARG(0,Ast*);
	SRET(Ast::newOperation(OP_CALL,expr,NULL));
}

SEMANTIC_ACTION_C(postfix_expression4, postfix_expression '(' argument_expression_list ')')
{
	Ast *expr1 = SARG(0,Ast*);
	Ast *argList = SARG(1,Ast*);
	SRET(Ast::newOperation(OP_CALL,expr1,argList));
}

SEMANTIC_ACTION_C(postfix_expression5, postfix_expression struct_member_accessor IDENTIFIER)
{
	//todo: complete
	Ast *expr = SARG(0,Ast*);
	int op = SARG_IVAL(1);
	char *name = SARG_TEXT(2);

	StructDef *stdef = NULL;
	SymbolType *resultingType = expr->getResultingType();
	if(op == OP_MEMBER_REF) {
		if(resultingType->type() == SymbolType::SYMT_TYPESPECIFIER) {
			TypeSpecifier *specifier = (TypeSpecifier*)resultingType;
			if(specifier->dataType() == DT_STRUCT)
				stdef = specifier->structDef();
		}
	}
	else if(op == OP_MEMBER_POINT) {
		if(resultingType->type() == SymbolType::SYMT_POINTER) {
			SymbolType *nextType = resultingType->next();
			if(nextType && nextType->type() == SymbolType::SYMT_TYPESPECIFIER) {
				TypeSpecifier *specifier = (TypeSpecifier*)resultingType;
				if(specifier->dataType() == DT_STRUCT)
					stdef = specifier->structDef();
			}
		}
	}

	if(!stdef) {
		// '.' or '->' not at right of a struct/union
	}
	else {

		//todo: find another way to identify imcomplete structs
//		Scope *stScope = stdef->scope();
//		if(!stScope) {
			//imcomplete struct
//		}
		CSymbol *member = stdef->findMember(name);
		if(!member) {
			//invalid struct member
		}
	}

	resultingType->release();

	SRET(expr);
}

SEMANTIC_ACTION_C(postfix_expression6, postfix_expression INC_OP)
{
	Ast *expr = SARG(0,Ast*);
	SRET(Ast::newOperation(OP_INC,expr,NULL));
}

SEMANTIC_ACTION_C(postfix_expression7, postfix_expression DEC_OP)
{
	Ast *expr = SARG(0,Ast*);
	SRET(Ast::newOperation(OP_DEC,expr,NULL));
}



//struct_member_accessor

SEMANTIC_ACTION_C(struct_member_accessor1, '.')
{
	g_expectIdentifier++;
	SRET_IVAL(OP_MEMBER_REF);
}

SEMANTIC_ACTION_C(struct_member_accessor2, PTR_OP)
{
	g_expectIdentifier++;
	SRET_IVAL(OP_MEMBER_POINT);
}



//argument_expression_list

SEMANTIC_ACTION_C(argument_expression_list1, assignment_expression)
{
}

SEMANTIC_ACTION_C(argument_expression_list2, argument_expression_list ',' assignment_expression)
{
	Ast *exprList = SARG(0,Ast*);
	Ast *expr = SARG(1,Ast*);
	if(exprList)
		exprList->appendSibling(expr);
	else
		SRET(expr);
}



//unary_expression

SEMANTIC_ACTION_C(unary_expression1, postfix_expression)
{
}

SEMANTIC_ACTION_C(unary_expression2, INC_OP unary_expression)
{
	Ast *expr = SARG(1,Ast*);
	SRET(Ast::newOperation(OP_INC,NULL,expr));
}

SEMANTIC_ACTION_C(unary_expression3, DEC_OP unary_expression)
{
	Ast *expr = SARG(1,Ast*);
	SRET(Ast::newOperation(OP_DEC,NULL,expr));
}

SEMANTIC_ACTION_C(unary_expression4, unary_operator cast_expression)
{
	int op = SARG_IVAL(0);
	Ast *expr = SARG(1,Ast*);
	SRET(Ast::newOperation((C_operator)op,NULL,expr));
}

SEMANTIC_ACTION_C(unary_expression5, SIZEOF unary_expression)
{
	Ast *expr = SARG(1,Ast*);
	SRET(Ast::newOperation(OP_SIZEOF,NULL,expr));
}

SEMANTIC_ACTION_C(unary_expression6, SIZEOF '(' type_name ')')
{
	SymbolType* symType = SARG(2,SymbolType*);
	Ast *astType = Ast::newValue(*Value::fromSymbolType(symType));
	SRET(Ast::newOperation(OP_SIZEOF,NULL,astType));
}



//unary_operator

SEMANTIC_ACTION_C(unary_operator1, '&')
{
	SRET_IVAL(OP_REF);
}

SEMANTIC_ACTION_C(unary_operator2, '*')
{
	SRET_IVAL(OP_DEREF);
}

SEMANTIC_ACTION_C(unary_operator3, '+')
{
	SRET_IVAL(OP_UNARY_PLUS);
}

SEMANTIC_ACTION_C(unary_operator4, '-')
{
	SRET_IVAL(OP_UNARY_MINUS);
}

SEMANTIC_ACTION_C(unary_operator5, '~')
{
	SRET_IVAL(OP_BITWISE_NOT);
}

SEMANTIC_ACTION_C(unary_operator6, '!')
{
	SRET_IVAL(OP_LOGICAL_NOT);
}



//cast_expression

SEMANTIC_ACTION_C(cast_expression1, unary_expression)
{
}

SEMANTIC_ACTION_C(cast_expression2, '(' type_name ')' cast_expression)
{
	SymbolType *symType = SARG(1,SymbolType*);
	Ast *expr1 = Ast::newValue(*Value::fromSymbolType(symType));
	Ast *expr2 = SARG(3,Ast*);
	SRET(Ast::newOperation(OP_Ast,expr1,expr2));
}



//multiplicative_expression

SEMANTIC_ACTION_C(multiplicative_expression1, cast_expression)
{
}

SEMANTIC_ACTION_C(multiplicative_expression2, multiplicative_expression '*' cast_expression)
{
	Ast *expr1 = SARG(0,Ast*);
	Ast *expr2 = SARG(2,Ast*);
	SRET(Ast::newOperation(OP_MUL,expr1,expr2));
}

SEMANTIC_ACTION_C(multiplicative_expression3, multiplicative_expression '/' cast_expression)
{
	Ast *expr1 = SARG(0,Ast*);
	Ast *expr2 = SARG(2,Ast*);
	SRET(Ast::newOperation(OP_DIV,expr1,expr2));
}

SEMANTIC_ACTION_C(multiplicative_expression4, multiplicative_expression '%' cast_expression)
{
	Ast *expr1 = SARG(0,Ast*);
	Ast *expr2 = SARG(2,Ast*);
	SRET(Ast::newOperation(OP_MOD,expr1,expr2));
}



//additive_expression

SEMANTIC_ACTION_C(additive_expression1, multiplicative_expression)
{
}

SEMANTIC_ACTION_C(additive_expression2, additive_expression '+' multiplicative_expression)
{
	Ast *expr1 = SARG(0,Ast*);
	Ast *expr2 = SARG(2,Ast*);
	SRET(Ast::newOperation(OP_ADD,expr1,expr2));
}

SEMANTIC_ACTION_C(additive_expression3, additive_expression '-' multiplicative_expression)
{
	Ast *expr1 = SARG(0,Ast*);
	Ast *expr2 = SARG(2,Ast*);
	SRET(Ast::newOperation(OP_SUB,expr1,expr2));
}



//shift_expression

SEMANTIC_ACTION_C(shift_expression1, additive_expression)
{
}

SEMANTIC_ACTION_C(shift_expression2, shift_expression LEFT_OP additive_expression)
{
	Ast *expr1 = SARG(0,Ast*);
	Ast *expr2 = SARG(2,Ast*);
	SRET(Ast::newOperation(OP_LSHIFT,expr1,expr2));
}

SEMANTIC_ACTION_C(shift_expression3, shift_expression RIGHT_OP additive_expression)
{
	Ast *expr1 = SARG(0,Ast*);
	Ast *expr2 = SARG(2,Ast*);
	SRET(Ast::newOperation(OP_RSHIFT,expr1,expr2));
}



//relational_expression

SEMANTIC_ACTION_C(relational_expression1, shift_expression)
{
}

SEMANTIC_ACTION_C(relational_expression2, relational_expression '<' shift_expression)
{
	Ast *expr1 = SARG(0,Ast*);
	Ast *expr2 = SARG(2,Ast*);
	SRET(Ast::newOperation(OP_LT,expr1,expr2));
}

SEMANTIC_ACTION_C(relational_expression3, relational_expression '>' shift_expression)
{
	Ast *expr1 = SARG(0,Ast*);
	Ast *expr2 = SARG(2,Ast*);
	SRET(Ast::newOperation(OP_GT,expr1,expr2));
}

SEMANTIC_ACTION_C(relational_expression4, relational_expression LE_OP shift_expression)
{
	Ast *expr1 = SARG(0,Ast*);
	Ast *expr2 = SARG(2,Ast*);
	SRET(Ast::newOperation(OP_LE,expr1,expr2));
}

SEMANTIC_ACTION_C(relational_expression5, relational_expression GE_OP shift_expression)
{
	Ast *expr1 = SARG(0,Ast*);
	Ast *expr2 = SARG(2,Ast*);
	SRET(Ast::newOperation(OP_GE,expr1,expr2));
}



//equality_expression

SEMANTIC_ACTION_C(equality_expression1, relational_expression)
{
}

SEMANTIC_ACTION_C(equality_expression2, equality_expression EQ_OP relational_expression)
{
	Ast *expr1 = SARG(0,Ast*);
	Ast *expr2 = SARG(2,Ast*);
	SRET(Ast::newOperation(OP_EQ,expr1,expr2));
}

SEMANTIC_ACTION_C(equality_expression3, equality_expression NE_OP relational_expression)
{
	Ast *expr1 = SARG(0,Ast*);
	Ast *expr2 = SARG(2,Ast*);
	SRET(Ast::newOperation(OP_NE,expr1,expr2));
}



//and_expression

SEMANTIC_ACTION_C(and_expression1, equality_expression)
{
}

SEMANTIC_ACTION_C(and_expression2, and_expression '&' equality_expression)
{
	Ast *expr1 = SARG(0,Ast*);
	Ast *expr2 = SARG(2,Ast*);
	SRET(Ast::newOperation(OP_BITWISE_AND,expr1,expr2));
}



//exclusive_or_expression

SEMANTIC_ACTION_C(exclusive_or_expression1, and_expression)
{
}

SEMANTIC_ACTION_C(exclusive_or_expression2, exclusive_or_expression '^' and_expression)
{
	Ast *expr1 = SARG(0,Ast*);
	Ast *expr2 = SARG(2,Ast*);
	SRET(Ast::newOperation(OP_BITWISE_XOR,expr1,expr2));
}



//inclusive_or_expression

SEMANTIC_ACTION_C(inclusive_or_expression1, exclusive_or_expression)
{
}

SEMANTIC_ACTION_C(inclusive_or_expression2, inclusive_or_expression '|' exclusive_or_expression)
{
	Ast *expr1 = SARG(0,Ast*);
	Ast *expr2 = SARG(2,Ast*);
	SRET(Ast::newOperation(OP_BITWISE_OR,expr1,expr2));
}



//logical_and_expression

SEMANTIC_ACTION_C(logical_and_expression1, inclusive_or_expression)
{
}

SEMANTIC_ACTION_C(logical_and_expression2, logical_and_expression AND_OP inclusive_or_expression)
{
	Ast *expr1 = SARG(0,Ast*);
	Ast *expr2 = SARG(2,Ast*);
	SRET(Ast::newOperation(OP_LOGICAL_AND,expr1,expr2));
}



//logical_or_expression

SEMANTIC_ACTION_C(logical_or_expression1, logical_and_expression)
{
}

SEMANTIC_ACTION_C(logical_or_expression2, logical_or_expression OR_OP logical_and_expression)
{
	Ast *expr1 = SARG(0,Ast*);
	Ast *expr2 = SARG(2,Ast*);
	SRET(Ast::newOperation(OP_LOGICAL_OR,expr1,expr2));
}



//conditional_expression

SEMANTIC_ACTION_C(conditional_expression1, logical_or_expression)
{
}

SEMANTIC_ACTION_C(conditional_expression2, logical_or_expression '?' expression ':' conditional_expression)
{
	Ast *expr1 = SARG(0,Ast*);
	Ast *expr2 = SARG(2,Ast*);
	Ast *expr3 = SARG(4,Ast*);
	Ast *res = Ast::newOperation(OP_COND2,expr2,expr3);
	SRET(Ast::newOperation(OP_COND,expr1,res));
}



//assignment_expression

SEMANTIC_ACTION_C(assignment_expression1, conditional_expression)
{
}

SEMANTIC_ACTION_C(assignment_expression2, unary_expression assignment_operator assignment_expression)
{
	Ast *expr1 = SARG(0,Ast*);
	unsigned int op = SARG_UVAL(1);
	Ast *expr2 = SARG(2,Ast*);
	//todo: check type compability
	SRET(Ast::newOperation((C_operator)op,expr1,expr2));
}



//assignment_operator

SEMANTIC_ACTION_C(assignment_operator1, '=')
{
	SRET_UVAL(OP_ASSIGN);
}

SEMANTIC_ACTION_C(assignment_operator2, MUL_ASSIGN)
{
	SRET_UVAL(OP_MUL_ASSIGN);
}

SEMANTIC_ACTION_C(assignment_operator3, DIV_ASSIGN)
{
	SRET_UVAL(OP_DIV_ASSIGN);
}

SEMANTIC_ACTION_C(assignment_operator4, MOD_ASSIGN)
{
	SRET_UVAL(OP_MOD_ASSIGN);
}

SEMANTIC_ACTION_C(assignment_operator5, ADD_ASSIGN)
{
	SRET_UVAL(OP_ADD_ASSIGN);
}

SEMANTIC_ACTION_C(assignment_operator6, SUB_ASSIGN)
{
	SRET_UVAL(OP_SUB_ASSIGN);
}

SEMANTIC_ACTION_C(assignment_operator7, LEFT_ASSIGN)
{
	SRET_UVAL(OP_LSHIFT_ASSIGN);
}

SEMANTIC_ACTION_C(assignment_operator8, RIGHT_ASSIGN)
{
	SRET_UVAL(OP_RSHIFT_ASSIGN);
}

SEMANTIC_ACTION_C(assignment_operator9, AND_ASSIGN)
{
	SRET_UVAL(OP_AND_ASSIGN);
}

SEMANTIC_ACTION_C(assignment_operator10, XOR_ASSIGN)
{
	SRET_UVAL(OP_XOR_ASSIGN);
}

SEMANTIC_ACTION_C(assignment_operator11, OR_ASSIGN)
{
	SRET_UVAL(OP_OR_ASSIGN);
}



//expression

SEMANTIC_ACTION_C(expression1, assignment_expression)
{
}

SEMANTIC_ACTION_C(expression2, expression ',' assignment_expression)
{
	Ast *expr1 = SARG(0,Ast*);
	Ast *expr2 = SARG(2,Ast*);
	SRET(Ast::newOperation(OP_COMMA,expr1,expr2));
}



//	constant_expression

SEMANTIC_ACTION_C(constant_expression1, conditional_expression)
{
}



//declaration

SEMANTIC_ACTION_C(declaration1, declaration_specifiers ';')
{
	TypeSpecifier *type = SARG(0,TypeSpecifier*);
	if(type->dataType() == DT_STRUCT) {
		
	}
	else {
		//warning: useless declaration
	}
	type->release();
	SRET(NULL);
}

SEMANTIC_ACTION_C(declaration2, declaration_specifiers init_declarator_list ';')
{
	TypeSpecifier *type = SARG(0,TypeSpecifier*);
	CSymbol *symChain = SARG(1,CSymbol*);
	SRET(symChain);

	while(symChain) {
		symChain->linkType(*type);
		symChain = symChain->next();
	}

	type->release();
}



//declaration_specifiers

SEMANTIC_ACTION_C(declaration_specifiers1, storage_class_specifier)
{
}

SEMANTIC_ACTION_C(declaration_specifiers2, storage_class_specifier declaration_specifiers)
{
	TypeSpecifier *type1 = SARG(0,TypeSpecifier*);
	TypeSpecifier *type2 = SARG(1,TypeSpecifier*);
	type1->merge(*type2);
}

SEMANTIC_ACTION_C(declaration_specifiers3, type_specifier)
{
}

SEMANTIC_ACTION_C(declaration_specifiers4, type_specifier declaration_specifiers)
{
	TypeSpecifier *type1 = SARG(0,TypeSpecifier*);
	TypeSpecifier *type2 = SARG(1,TypeSpecifier*);
	type1->merge(*type2);
}

SEMANTIC_ACTION_C(declaration_specifiers5, type_qualifier)
{
}

SEMANTIC_ACTION_C(declaration_specifiers6, type_qualifier declaration_specifiers)
{
	TypeSpecifier *type1 = SARG(0,TypeSpecifier*);
	TypeSpecifier *type2 = SARG(1,TypeSpecifier*);
	type1->merge(*type2);
}



//init_declarator_list

SEMANTIC_ACTION_C(init_declarator_list1, init_declarator)
{

}

SEMANTIC_ACTION_C(init_declarator_list2, init_declarator_list ',' init_declarator)
{
	CSymbol *symbol1 = SARG(0,CSymbol*);
	CSymbol *symbol2 = SARG(2,CSymbol*);
	const char *symbolName1 = symbol1->identifier();
	const char *symbolName2 = symbol2->identifier();
	symbol1->appendSibling(symbol2);
}



//init_declarator

SEMANTIC_ACTION_C(init_declarator1, declarator)
{
	//
}

SEMANTIC_ACTION_C(init_declarator2, declarator '=' initializer)
{
	CSymbol *sym = SARG(0,CSymbol*);
	Ast *init = SARG(2,Ast*);
	sym->setInitList(init);
}



//storage_class_specifier

SEMANTIC_ACTION_C(storage_class_specifier1, TYPEDEF)
{
	TypeSpecifier *res = SymbolType::createTypeSpecifier();
	res->setStorageClass(SC_TYPEDEF);
	SRET(res);
}

SEMANTIC_ACTION_C(storage_class_specifier2, EXTERN)
{
	TypeSpecifier *res = SymbolType::createTypeSpecifier();
	res->setStorageClass(SC_EXTERN);
	SRET(res);
}

SEMANTIC_ACTION_C(storage_class_specifier3, STATIC)
{
	TypeSpecifier *res = SymbolType::createTypeSpecifier();
	res->setStorageClass(SC_STATIC);
	SRET(res);
}

SEMANTIC_ACTION_C(storage_class_specifier4, AUTO)
{
	TypeSpecifier *res = SymbolType::createTypeSpecifier();
	res->setStorageClass(SC_AUTO);
	SRET(res);
}

SEMANTIC_ACTION_C(storage_class_specifier5, REGISTER)
{
	TypeSpecifier *res = SymbolType::createTypeSpecifier();
	res->setStorageClass(SC_REGISTER);
	SRET(res);
}



//type_specifier

SEMANTIC_ACTION_C(type_specifier1, VOID)
{
	TypeSpecifier *type = SymbolType::createTypeSpecifier();
	type->setDataType(DT_VOID);
	SRET(type);
}

SEMANTIC_ACTION_C(type_specifier2, CHAR)
{
	TypeSpecifier *type = SymbolType::createTypeSpecifier();
	type->setDataType(DT_CHAR);
	SRET(type);
}

SEMANTIC_ACTION_C(type_specifier3, SHORT)
{
	TypeSpecifier *type = SymbolType::createTypeSpecifier();
	type->setDataType(DT_SHORTINT);
	SRET(type);
}

SEMANTIC_ACTION_C(type_specifier4, INT)
{
	TypeSpecifier *type = SymbolType::createTypeSpecifier();
	type->setDataType(DT_INT);
	SRET(type);
}

SEMANTIC_ACTION_C(type_specifier5, LONG)
{
	TypeSpecifier *type = SymbolType::createTypeSpecifier();
	type->setDataType(DT_LONGINT);
	SRET(type);
}

SEMANTIC_ACTION_C(type_specifier6, FLOAT)
{
	TypeSpecifier *type = SymbolType::createTypeSpecifier();
	type->setDataType(DT_FLOAT);
	SRET(type);
}

SEMANTIC_ACTION_C(type_specifier7, DOUBLE)
{
	TypeSpecifier *type = SymbolType::createTypeSpecifier();
	type->setDataType(DT_DOUBLE);
	SRET(type);
}

SEMANTIC_ACTION_C(type_specifier8, SIGNED)
{
	TypeSpecifier *type = SymbolType::createTypeSpecifier();
	type->setSign(SIGN_SIGNED);
	SRET(type);
}

SEMANTIC_ACTION_C(type_specifier9, UNSIGNED)
{
	TypeSpecifier *type = SymbolType::createTypeSpecifier();
	type->setSign(SIGN_UNSIGNED);
	SRET(type);
}

SEMANTIC_ACTION_C(type_specifier10, struct_or_union_specifier)
{
	StructDef *stdef = SARG(0,StructDef*);

	TypeSpecifier *type = SymbolType::createTypeSpecifier();
	type->setDataType(DT_STRUCT);
	type->setStructDef(stdef);
	SRET(type);

	g_expectIdentifier = true;
}

SEMANTIC_ACTION_C(type_specifier11, enum_specifier)
{
	g_expectIdentifier = true;
}

SEMANTIC_ACTION_C(type_specifier12, TYPE_NAME)
{
	char *token = SARG_TEXT(0);
	CSymbol *sym = C_curScope()->findSymbol(token);

	SymbolType *type = sym->type();
	type->addRef();
	SRET(type);
	
	g_expectIdentifier = true;
}



//struct_or_union_specifier

SEMANTIC_ACTION_C(struct_or_union_specifier1, struct_or_union struct_tag_opt '{' struct_declaration_list '}')
{
	//todo: complete
	unsigned long type = SARG_ULVAL(0); //struct or union
	StructDef* stdef = SARG(1,StructDef*);
	
	if(stdef->isComplete()) {
		//struct redefinition
		//todo: set an error
//		int a = 1;
	}	

	CSymbol *symChain = SARG(3,CSymbol*);
	while(symChain) {
		if(stdef->findMember(symChain->identifier())) {
			//duplicated member
			int todo = 1;
		}
		stdef->insertSymbol(symChain);
		symChain = symChain->next();
	}

	stdef->setType(type);
//	stdef->setScope(stScope);
	SRET(stdef);
}

SEMANTIC_ACTION_C(struct_or_union_specifier2, struct_or_union IDENTIFIER)
{
	int curLevel = C_curScope()->level();
	unsigned long type = SARG_ULVAL(0);
	char *tag = SARG_TEXT(1);
	StructDef* stdef = C_curScope()->findStruct(tag);
	if(stdef) {
		SRET(stdef);
		return;
	}

	stdef = new StructDef;
	stdef->setTag(tag);
	stdef->setType(type);
	C_curScope()->insertStruct(stdef);
	SRET(stdef);
}



//struct_tag_opt

SEMANTIC_ACTION_C(struct_tag_opt1, IDENTIFIER)
{
	char *tag = SARG_TEXT(0);
	StructDef* stdef = C_curScope()->findStruct(tag);
	if(stdef) {
		SRET(stdef);
		return;
	}

	stdef = new StructDef;
	stdef->setTag(tag);
	stdef->setScopeSeen(C_curScope());
	C_curScope()->insertStruct(stdef);
	SRET(stdef);
}

SEMANTIC_ACTION_C(struct_tag_opt2, E)
{
	const char *tag = CSymbol::generateStructTag(C_curScope()->level());
	StructDef* stdef = new StructDef;
	stdef->setTag(tag);
	stdef->setScopeSeen(C_curScope());
	C_curScope()->insertStruct(stdef);
	SRET(stdef);
}



//struct_definition_start

SEMANTIC_ACTION_C(struct_definition_start1, '{')
{
//	C_enterScope();
//	SRET(C_curScope());
}



//struct_definition_end

SEMANTIC_ACTION_C(struct_definition_end1, '}')
{
//	SRET(C_curScope());
//	C_exitScope();
}



//struct_or_union

SEMANTIC_ACTION_C(struct_or_union1, STRUCT)
{
	SRET_LVAL(STYPE_STRUCT);
	g_expectIdentifier = true;
}

SEMANTIC_ACTION_C(struct_or_union2, UNION)
{
	SRET_LVAL(STYPE_UNION);
	g_expectIdentifier = true;
}



//struct_declaration_list

SEMANTIC_ACTION_C(struct_declaration_list1, struct_declaration)
{
	//Arg 0 == CSymbol
}

SEMANTIC_ACTION_C(struct_declaration_list2, struct_declaration_list struct_declaration)
{
	CSymbol *symbol = SARG(0,CSymbol*);
	CSymbol *symbol2 = SARG(1,CSymbol*);
	const char *name = symbol->identifier();
	const char *name2 = symbol2->identifier();
	symbol->appendSibling(symbol2);
}



//struct_declaration

SEMANTIC_ACTION_C(struct_declaration1, specifier_qualifier_list struct_declarator_list ';')
{
	TypeSpecifier *symType = SARG(0,TypeSpecifier*);
	CSymbol *symChain = SARG(1,CSymbol*);
	CSymbol *sym = symChain;
	while(sym) {
		//todo: check if types are valid
		sym->linkType(*symType);
		sym = sym->next();
	}
	symType->release();

	SRET(symChain);
}



//specifier_qualifier_list

SEMANTIC_ACTION_C(specifier_qualifier_list1, type_specifier specifier_qualifier_list)
{
	TypeSpecifier *type1 = SARG(0,TypeSpecifier*);
	TypeSpecifier *type2 = SARG(1,TypeSpecifier*);
	type1->merge(*type2);
}

SEMANTIC_ACTION_C(specifier_qualifier_list2, type_specifier)
{
}

SEMANTIC_ACTION_C(specifier_qualifier_list3, type_qualifier specifier_qualifier_list)
{
	TypeSpecifier *type1 = SARG(0,TypeSpecifier*);
	TypeSpecifier *type2 = SARG(1,TypeSpecifier*);
	type1->merge(*type2);
}

SEMANTIC_ACTION_C(specifier_qualifier_list4, type_qualifier)
{
}



//struct_declarator_list

SEMANTIC_ACTION_C(struct_declarator_list1, struct_declarator)
{
	CSymbol *symbol = SARG(0,CSymbol*);
}

SEMANTIC_ACTION_C(struct_declarator_list2, struct_declarator_list ',' struct_declarator)
{
	CSymbol *symbol = SARG(0,CSymbol*);
	CSymbol *symbol2 = SARG(2,CSymbol*);
	symbol->appendSibling(symbol2);
}



//struct_declarator

SEMANTIC_ACTION_C(struct_declarator1, declarator)
{
}

SEMANTIC_ACTION_C(struct_declarator2, ':' constant_expression)
{
}

SEMANTIC_ACTION_C(struct_declarator3, declarator ':' constant_expression)
{
}



//enum_specifier

SEMANTIC_ACTION_C(enum_specifier1, ENUM '{' enumerator_list '}')
{
}

SEMANTIC_ACTION_C(enum_specifier2, ENUM IDENTIFIER '{' enumerator_list '}')
{
}

SEMANTIC_ACTION_C(enum_specifier3, ENUM IDENTIFIER)
{
}



//enumerator_list

SEMANTIC_ACTION_C(enumerator_list1, enumerator)
{
}

SEMANTIC_ACTION_C(enumerator_list2, enumerator_list ',' enumerator)
{
}



//enumerator

SEMANTIC_ACTION_C(enumerator1, IDENTIFIER)
{
}

SEMANTIC_ACTION_C(enumerator2, IDENTIFIER '=' constant_expression)
{
}



//type_qualifier

SEMANTIC_ACTION_C(type_qualifier1, CONST)
{
	TypeSpecifier *type = SymbolType::createTypeSpecifier();
	type->setConst(true);
	SRET(type);
}

SEMANTIC_ACTION_C(type_qualifier2, VOLATILE)
{
	TypeSpecifier *type = SymbolType::createTypeSpecifier();
	type->setVolatile(true);
	SRET(type);
}



//declarator

SEMANTIC_ACTION_C(declarator1, pointer direct_declarator)
{
	Pointer *type = SARG(0,Pointer*);
	CSymbol *symbol = SARG(1,CSymbol*);
	symbol->linkType(*type);
	type->release();
	SRET(symbol);
}

SEMANTIC_ACTION_C(declarator2, direct_declarator)
{

}



//direct_declarator

SEMANTIC_ACTION_C(direct_declarator1, IDENTIFIER)
{
	const char *name = SARG_TEXT(0);
	CSymbol *symbol = new CSymbol;
	symbol->setIdentifier(name);
	symbol->setScopeSeen(C_curScope());
	SRET(symbol);
}

SEMANTIC_ACTION_C(direct_declarator2, '(' declarator ')')
{
	SRET(SARG(1,CSymbol*));
}

SEMANTIC_ACTION_C(direct_declarator3, direct_declarator '[' constant_expression ']')
{
	CSymbol *symbol = SARG(0,CSymbol*);
	Ast *expr = SARG(2,Ast*);
	Array *type = SymbolType::createArray(expr);
	symbol->linkType(*type);
	type->release();
	SRET(symbol);
}

SEMANTIC_ACTION_C(direct_declarator4, direct_declarator '[' ']')
{
	CSymbol *symbol = SARG(0,CSymbol*);
	Array *type = SymbolType::createArray(NULL);
	symbol->linkType(*type);
	type->release();
	SRET(symbol);
}

SEMANTIC_ACTION_C(direct_declarator5, direct_declarator '(' parameter_type_list ')')
{
	CSymbol *symbol = SARG(0,CSymbol*);
	CSymbol *paramList = SARG(2,CSymbol*);

	Function *funcType = SymbolType::createFunction();
	funcType->setParameters(paramList,g_thisFuncHasVargs);
	symbol->linkType(*funcType);
	funcType->release();

	g_thisFuncHasVargs = false;
	SRET(symbol);
}

SEMANTIC_ACTION_C(direct_declarator6, direct_declarator '(' identifier_list ')')
{
	Function *type = SymbolType::createFunction();
	CSymbol *symbol = SARG(0,CSymbol*);
	symbol->linkType(*type);
	type->release();
	//todo: identifier list
	SRET(symbol);
}

SEMANTIC_ACTION_C(direct_declarator7, direct_declarator '(' ')')
{
	Function *funcType = SymbolType::createFunction();
	CSymbol *symbol = SARG(0,CSymbol*);
	funcType->setParameters(NULL,false);
	symbol->linkType(*funcType);
	funcType->release();
	SRET(symbol);
}



//parameter_list_start

SEMANTIC_ACTION_C(parameter_list_start1, '(')
{
	g_funcParamCounter = 0;
	//even though the official documentation says there is a function prototype scope
	//we won't be using it here, as we will have the parameter list associated to this function already.
//	C_enterScope();
//	SRET(C_curScope());
}



//parameter_list_end

SEMANTIC_ACTION_C(parameter_list_end1, ')')
{
	//don't exit scope here, we still have to do further computations while in this scope
//	SRET(C_curScope());
//	C_exitScope();
}



//pointer

SEMANTIC_ACTION_C(pointer1, '*')
{
	Pointer *type = SymbolType::createPointer();
	SRET(type);
}

SEMANTIC_ACTION_C(pointer2, '*' type_qualifier_list)
{
	TypeSpecifier *type = SARG(1,TypeSpecifier*);
	Pointer *retType = SymbolType::createPointer();
	retType->merge(*type);
	SRET(retType);
}

SEMANTIC_ACTION_C(pointer3, '*' pointer)
{
	Pointer *type = SARG(1,Pointer*);
	Pointer *retType = SymbolType::createPointer();
	retType->linkType(*type);
	type->release();
	SRET(retType);
}

SEMANTIC_ACTION_C(pointer4, '*' type_qualifier_list pointer)
{
	TypeSpecifier *type1 = SARG(1,TypeSpecifier*);
	Pointer *type2 = SARG(2,Pointer*);
	Pointer *retType = SymbolType::createPointer();
	retType->merge(*type1);
	retType->linkType(*type2);
	type2->release();
	SRET(retType);
}



//type_qualifier_list

SEMANTIC_ACTION_C(type_qualifier_list1, type_qualifier)
{
}

SEMANTIC_ACTION_C(type_qualifier_list2, type_qualifier_list type_qualifier)
{
	TypeSpecifier *type1 = SARG(0,TypeSpecifier*);
	TypeSpecifier *type2 = SARG(1,TypeSpecifier*);
	type1->merge(*type2);
}



//parameter_type_list

SEMANTIC_ACTION_C(parameter_type_list1, parameter_list)
{
}

SEMANTIC_ACTION_C(parameter_type_list2, parameter_list ',' ELLIPSIS)
{
	g_thisFuncHasVargs = true;
}



//parameter_list

SEMANTIC_ACTION_C(parameter_list1, parameter_declaration)
{
	++g_funcParamCounter;
}

SEMANTIC_ACTION_C(parameter_list2, parameter_list ',' parameter_declaration)
{
	++g_funcParamCounter;
	CSymbol *sym1 = SARG(0,CSymbol*);
	CSymbol *sym2 = SARG(2,CSymbol*);
	sym1->appendSibling(sym2);
	SRET(sym1);
}



//parameter_declaration

SEMANTIC_ACTION_C(parameter_declaration1, declaration_specifiers declarator)
{
	TypeSpecifier *symType = SARG(0,TypeSpecifier*);
	CSymbol *symbol = SARG(1,CSymbol*);
	symbol->linkType(*symType);
	symbol->setParameterFlag(true,g_funcParamCounter);
	symType->release();
	SRET(symbol);
}

SEMANTIC_ACTION_C(parameter_declaration2, declaration_specifiers abstract_declarator)
{
	TypeSpecifier *symType1 = SARG(0,TypeSpecifier*);
	SymbolType *symType2 = SARG(1,SymbolType*);
	
	symType2->linkType(*symType1);

	CSymbol *symbol = new CSymbol;
	symbol->setIdentifier(CSymbol::generateParamName(g_funcParamCounter+1));
	symbol->linkType(*symType2);
	symbol->setParameterFlag(true,g_funcParamCounter);

	symType1->release();
	symType2->release();
	SRET(symbol);
}

SEMANTIC_ACTION_C(parameter_declaration3, declaration_specifiers)
{
	TypeSpecifier *symType = SARG(0,TypeSpecifier*);
	CSymbol *symbol = new CSymbol;
	symbol->setIdentifier(CSymbol::generateParamName(g_funcParamCounter+1));
	symbol->linkType(*symType);
	symbol->setParameterFlag(true,g_funcParamCounter);
	symType->release();
	SRET(symbol);
}



//identifier_list

SEMANTIC_ACTION_C(identifier_list1, IDENTIFIER)
{
}

SEMANTIC_ACTION_C(identifier_list2, identifier_list ',' IDENTIFIER)
{
}



//type_name

SEMANTIC_ACTION_C(type_name1, specifier_qualifier_list)
{
}

SEMANTIC_ACTION_C(type_name2, specifier_qualifier_list abstract_declarator)
{
	TypeSpecifier *symType1 = SARG(0,TypeSpecifier*);
	SymbolType *symType2 = SARG(1,SymbolType*);
	symType2->linkType(*symType1);
	symType1->release();
	SRET(symType2);
}



//abstract_declarator

SEMANTIC_ACTION_C(abstract_declarator1, pointer)
{
	//
}

SEMANTIC_ACTION_C(abstract_declarator2, direct_abstract_declarator)
{
	//
}

SEMANTIC_ACTION_C(abstract_declarator3, pointer direct_abstract_declarator)
{
	Pointer *symType1 = SARG(0,Pointer*);
	SymbolType *symType2 = SARG(1,SymbolType*);
	symType2->linkType(*symType1);
	symType1->release();
	SRET(symType2);
}



//direct_abstract_declarator

SEMANTIC_ACTION_C(direct_abstract_declarator1, '(' abstract_declarator ')')
{
	SRET(SARG(1,SymbolType*));
}

SEMANTIC_ACTION_C(direct_abstract_declarator2, '[' ']')
{
	Array *type = SymbolType::createArray(NULL);
	SRET(type);
}

SEMANTIC_ACTION_C(direct_abstract_declarator3, '[' constant_expression ']')
{
	Ast *expr = SARG(1,Ast*);
	Array *type = SymbolType::createArray(expr);
	SRET(type);
}

SEMANTIC_ACTION_C(direct_abstract_declarator4, direct_abstract_declarator '[' ']')
{
	SymbolType *type1 = SARG(0,SymbolType*);
	Array *type2 = SymbolType::createArray(NULL);
	type1->linkType(*type2);
	SRET(type1);
}

SEMANTIC_ACTION_C(direct_abstract_declarator5, direct_abstract_declarator '[' constant_expression ']')
{
	SymbolType *type1 = SARG(0,SymbolType*);
	Ast *expr = SARG(2,Ast*);
	Array *type2 = SymbolType::createArray(expr);
	type1->linkType(*type2);
	SRET(type1);
}

SEMANTIC_ACTION_C(direct_abstract_declarator6, '(' ')')
{
	Function *funcType = SymbolType::createFunction();
	funcType->setParameters(NULL,false);
	SRET(funcType);
}

SEMANTIC_ACTION_C(direct_abstract_declarator7, '(' parameter_type_list ')')
{
	CSymbol *paramList = SARG(1,CSymbol*);
	Function *funcType = SymbolType::createFunction();
	funcType->setParameters(paramList,g_thisFuncHasVargs);
	g_thisFuncHasVargs = false;
	SRET(funcType);
}

SEMANTIC_ACTION_C(direct_abstract_declarator8, direct_abstract_declarator '(' ')')
{
	SymbolType *symType = SARG(0,SymbolType*);
	Function *funcType = SymbolType::createFunction();
	funcType->setParameters(NULL,false);
	symType->linkType(*funcType);
	funcType->release();
	SRET(symType);
}

SEMANTIC_ACTION_C(direct_abstract_declarator9, direct_abstract_declarator '(' parameter_type_list ')')
{
	SymbolType *symType = SARG(0,SymbolType*);
	CSymbol *paramList = SARG(2,CSymbol*);
	Function *funcType = SymbolType::createFunction();
	funcType->setParameters(paramList,g_thisFuncHasVargs);
	symType->linkType(*funcType);
	funcType->release();
	g_thisFuncHasVargs = false;
	SRET(symType);
}



//initializer

SEMANTIC_ACTION_C(initializer1, assignment_expression)
{
	//Ast *ast = SARG(0,Ast*);
}

SEMANTIC_ACTION_C(initializer2, '{' initializer_list '}')
{
	SRET(SARG(1,Ast*));
}

SEMANTIC_ACTION_C(initializer3, '{' initializer_list ',' '}')
{
	SRET(SARG(1,Ast*));
}



//initializer_list

SEMANTIC_ACTION_C(initializer_list1, initializer)
{
	//
}

SEMANTIC_ACTION_C(initializer_list2, initializer_list ',' initializer)
{
	Ast *initList = SARG(0,Ast*);
	Ast *init = SARG(1,Ast*);
	initList->appendSibling(init);
}



//statement

SEMANTIC_ACTION_C(statement1, labeled_statement)
{
}

SEMANTIC_ACTION_C(statement2, compound_statement)
{
}

SEMANTIC_ACTION_C(statement3, expression_statement)
{
}

SEMANTIC_ACTION_C(statement4, selection_statement)
{
}

SEMANTIC_ACTION_C(statement5, iteration_statement)
{
}

SEMANTIC_ACTION_C(statement6, jump_statement)
{
}



//labeled_statement

SEMANTIC_ACTION_C(labeled_statement1, IDENTIFIER ':' statement)
{
	CSymbol *sym = SARG(0,CSymbol*);
	Statement *body = SARG(2,Statement*);
//	LabelStatement *stmt = new LabelStatement(name,body);
	int a = 1;
}

SEMANTIC_ACTION_C(labeled_statement2, CASE constant_expression ':' statement)
{
//	Statement *
}

SEMANTIC_ACTION_C(labeled_statement3, DEFAULT ':' statement)
{
}



//compound_statement

SEMANTIC_ACTION_C(compound_statement1, '{' '}')
{
	Scope *scope = SARG(0,Scope*);
	SRET(new CompoundStatement(*scope,NULL,NULL));
}

SEMANTIC_ACTION_C(compound_statement2, '{' statement_list '}')
{
	Scope *scope = SARG(0,Scope*);
	Statement *stmtList = SARG(1,Statement*);
	CompoundStatement *stmt = new CompoundStatement(*scope,NULL,stmtList);
	SRET(stmt);
}

SEMANTIC_ACTION_C(compound_statement3, '{' declaration_list '}')
{
	Scope *scope = SARG(0,Scope*);
	CSymbol *declList = SARG(1,CSymbol*);
	CompoundStatement *stmt = new CompoundStatement(*scope,declList,NULL);
	SRET(stmt);
}

SEMANTIC_ACTION_C(compound_statement4, '{' declaration_list statement_list '}')
{
	Scope *scope = SARG(0,Scope*);
	CSymbol *declList = SARG(1,CSymbol*);
	Statement *stmtList = SARG(2,Statement*);
	CompoundStatement *stmt = new CompoundStatement(*scope,declList,stmtList);
	SRET(stmt);
}



//block_start

SEMANTIC_ACTION_C(block_start1, '{')
{
	C_enterBlock();
	C_enterScope();
	SRET(C_curScope());
}



//block_end

SEMANTIC_ACTION_C(block_end1, '}')
{
	SRET(C_curScope());
	C_exitScope();
	C_exitBlock();
}



//declaration_list

SEMANTIC_ACTION_C(declaration_list1, declaration)
{
	//todo: symbol chain in one declaration?
	CSymbol *symbol = SARG(0,CSymbol*);
	if(!symbol)
		return;

	//if we are have a typedef storage class, set the symbol as one right now
	SymbolType *type = symbol->type();
	while(type) {
		if(type->type() == SymbolType::SYMT_TYPESPECIFIER) {
			if(type->storageClass() == SC_TYPEDEF) {
				symbol->setTypedefFlag(true);
				type->setStorageClass(SC_UNSPECIFIED); //set storage class back to unespecified as we don't need to know if we are defining a type anymore
				break;
			}
		}

		type = type->next();
	}

	C_curScope()->insertSymbol(symbol);
}

SEMANTIC_ACTION_C(declaration_list2, declaration_list declaration)
{
	//todo: symbol chain in one declaration?
	CSymbol *symbolList = SARG(0,CSymbol*);
	CSymbol *symbol = SARG(1,CSymbol*);
	if(!symbol)
		return;

	//if we are have a typedef storage class, set the symbol as one right now
	SymbolType *type = symbol->type();
	while(type) {
		if(type->type() == SymbolType::SYMT_TYPESPECIFIER) {
			if(type->storageClass() == SC_TYPEDEF) {
				symbol->setTypedefFlag(true);
				type->setStorageClass(SC_UNSPECIFIED); //set storage class back to unespecified as we don't need to know if we are defining a type anymore
				break;
			}
		}

		type = type->next();
	}

	C_curScope()->insertSymbol(symbol);

	if(symbolList)
		symbolList->appendSibling(symbol);
	else {
		SRET(symbol);
	}
}



//statement_list

SEMANTIC_ACTION_C(statement_list1, statement)
{
}

SEMANTIC_ACTION_C(statement_list2, statement_list statement)
{
	Statement *stmt1 = SARG(0,Statement*);
	Statement *stmt2 = SARG(1,Statement*);
	stmt1->appendSibling(stmt2);
}



//expression_statement

SEMANTIC_ACTION_C(expression_statement1, ';')
{
	//null statement
	NullStatement *stmt = new NullStatement;
	SRET(stmt);
}

SEMANTIC_ACTION_C(expression_statement2, expression ';')
{
	Ast *s1 = SARG(0,Ast*);
	ExpressionStatement *stmt = new ExpressionStatement(s1);
	SRET(stmt);
}



//selection_statement

SEMANTIC_ACTION_C(selection_statement1, IF '(' expression ')' statement)
{
	Ast *expr = SARG(2,Ast*);
	Statement *ifBody = SARG(4,Statement*);
	IfStatement *stmt = new IfStatement(expr,ifBody,NULL);
	SRET(stmt);
}

SEMANTIC_ACTION_C(selection_statement2, IF '(' expression ')' statement ELSE statement)
{
	Ast *expr = SARG(2,Ast*);
	Statement *ifBody = SARG(4,Statement*);
	Statement *elseBody = SARG(6,Statement*);
	IfStatement *stmt = new IfStatement(expr,ifBody,elseBody);
	SRET(stmt);
}

SEMANTIC_ACTION_C(selection_statement3, SWITCH '(' expression ')' statement)
{
	Ast *expr = SARG(2,Ast*);
	Statement *switchBody = SARG(4,Statement*);
//	SwitchStatement *stmt = new SwitchStatement(expr,switchBody);
//	SRET(stmt);
}



//iteration_statement

SEMANTIC_ACTION_C(iteration_statement1, WHILE '(' expression ')' statement)
{
	Ast *expr = SARG(2,Ast*);
	Statement *whileBody = SARG(4,Statement*);
	WhileStatement *stmt = new WhileStatement(expr,whileBody);
	SRET(stmt);
}

SEMANTIC_ACTION_C(iteration_statement2, DO statement WHILE '(' expression ')' ';')
{
	Statement *doBody = SARG(1,Statement*);
	Ast *expr = SARG(4,Ast*);
	DoWhileStatement *stmt = new DoWhileStatement(doBody,expr);
	SRET(stmt);
}

SEMANTIC_ACTION_C(iteration_statement3, FOR '(' expression_statement expression_statement ')' statement)
{
	Statement *exprStmt1 = SARG(2,Statement*);
	Statement *exprStmt2 = SARG(3,Statement*);
	Statement *forBody = SARG(5,Statement*);
	ForStatement *stmt = new ForStatement(exprStmt1,exprStmt2,NULL,forBody);
	SRET(stmt);
}

SEMANTIC_ACTION_C(iteration_statement4, FOR '(' expression_statement expression_statement expression ')' statement)
{
	Statement *exprStmt1 = SARG(2,Statement*);
	Statement *exprStmt2 = SARG(3,Statement*);
	Ast *expr = SARG(4,Ast*);
	Statement *forBody = SARG(6,Statement*);
	ForStatement *stmt = new ForStatement(exprStmt1,exprStmt2,expr,forBody);
	SRET(stmt);
}



//jump_statement

SEMANTIC_ACTION_C(jump_statement1, GOTO IDENTIFIER ';')
{
}

SEMANTIC_ACTION_C(jump_statement2, CONTINUE ';')
{
}

SEMANTIC_ACTION_C(jump_statement3, BREAK ';')
{
}

SEMANTIC_ACTION_C(jump_statement4, RETURN ';')
{
	ReturnStatement *stmt = new ReturnStatement(NULL);
	SRET(stmt);
}

SEMANTIC_ACTION_C(jump_statement5, RETURN expression ';')
{
	Ast *expr = SARG(1,Ast*);
	ReturnStatement *stmt = new ReturnStatement(expr);
	SRET(stmt);
}



//translation_unit

SEMANTIC_ACTION_C(translation_unit1, external_declaration)
{
	//"start"
}

SEMANTIC_ACTION_C(translation_unit2, translation_unit external_declaration)
{
	//"start"
}



//external_declaration

void printDeclInfo(CSymbol *symbol)
{
	char decl[256];
	char *c = decl;

	SymbolType *type = symbol->type();

	if(symbol->isTypedef()) {
		c += sprintf(c,"Type definition of");
	}
	else {
		if(type->type() == SymbolType::SYMT_FUNCTION && ((Function*)type)->functionBody()) {
			c += sprintf(c,"Definition of");
		}
		else {
			c += sprintf(c,"Declaration of");
			if(type->type() == SymbolType::SYMT_TYPESPECIFIER && ((TypeSpecifier*)type)->dataType() != DT_STRUCT) {
				c += sprintf(c," ");
				c += sprintf(c,"variable of");
			}
		}
	}

	while(type) {
		c += sprintf(c," ");

		if(type->type() == SymbolType::SYMT_TYPESPECIFIER) {
			c += sprintf(c,"type ");

			DataType dataType = ((TypeSpecifier*)type)->dataType();
			bool isInt = false;
			char *declType = NULL;
			switch(dataType) {
				case DT_VOID:
					declType = "void";
					break;
				case DT_INT:
					declType = "int";
					isInt = true;
					break;
				case DT_CHAR:
					declType = "char";
					isInt = true;
					break;
				case DT_SHORTINT:
					declType = "short int";
					isInt = true;
					break;
				case DT_LONGINT:
					declType = "long int";
					isInt = true;
					break;
				case DT_LONGLONGINT:
					declType = "long long int";
					isInt = true;
					break;
				case DT_FLOAT:
					declType = "float";
					break;
				case DT_DOUBLE:
					declType = "double";
					break;

				case DT_STRUCT:
					declType = "struct";
					break;
			}
			if(isInt) {
				int sign = ((TypeSpecifier*)type)->isUnsigned() ? 2 : 1;
				TypeSpecifier *test = (TypeSpecifier*)type;
				if(sign == SIGN_UNSIGNED)
					c += sprintf(c,"unsigned");
				else if(sign == SIGN_SIGNED)
					c += sprintf(c,"signed");
				c += sprintf(c," ");
			}
			if(declType) {
				c += sprintf(c,"%s",declType);
				if(dataType == DT_STRUCT) {

				}
			}
		}
		else if(type->type() == SymbolType::SYMT_POINTER) {
			c += sprintf(c,"pointer to");
		}
		else if(type->type() == SymbolType::SYMT_ARRAY) {
			c += sprintf(c,"array of");
		}
		else if(type->type() == SymbolType::SYMT_FUNCTION) {
			c += sprintf(c,"function returning value of");
		}

		type = type->next();
	}

	c += sprintf(c,": ");
	c += sprintf(c,symbol->identifier());

	char *cc = decl;
	printf(cc);

	printf("\n");
}

SEMANTIC_ACTION_C(external_declaration1, function_definition)
{
	CSymbol *symbol = SARG(0,CSymbol*);
	if(!symbol)
		return;
	C_insertSymbolChainIntoCurrentScope(symbol);
	printDeclInfo(symbol);
}

SEMANTIC_ACTION_C(external_declaration2, declaration)
{
	CSymbol *firstSymbol = SARG(0,CSymbol*);
	CSymbol *lastSymbol;
	CSymbol *symbol;

	if(!firstSymbol)
		return;
	
	bool isTypedef = false;
	SymbolType *type = firstSymbol->type();
	while(type) {
		if(type->type() == SymbolType::SYMT_TYPESPECIFIER) {
			if(type->storageClass() == SC_TYPEDEF) {
				isTypedef = true;
				type->setStorageClass(SC_UNSPECIFIED); //set storage class back to unespecified as we don't need to know if we are defining a type anymore
				break;
			}
		}

		type = type->next();
	}

	symbol = lastSymbol = firstSymbol;
	while(symbol) {
		Scope *curScope = C_curScope();
		if(curScope->findSymbolWithinMainLevel(symbol->identifier()))
			int alreadyExists = 1;

		symbol->setTypedefFlag(isTypedef);
		curScope->insertSymbol(symbol);
		printDeclInfo(symbol);

		lastSymbol = symbol;
		symbol = symbol->next();
	}

	extern CSymbol *g_firstGlobalSymbol;
	extern CSymbol *g_lastGlobalSymbol;

	//update first and last global symbols
	if(!g_firstGlobalSymbol)
		g_firstGlobalSymbol = firstSymbol;
	else
		g_lastGlobalSymbol->appendSibling(firstSymbol);

	g_lastGlobalSymbol = lastSymbol;
}



//function_definition

SEMANTIC_ACTION_C(function_definition1, declaration_specifiers declarator declaration_list compound_statement)
{
	int a = 1;
}

SEMANTIC_ACTION_C(function_definition2, declaration_specifiers declarator compound_statement)
{
	TypeSpecifier *returningType = SARG(0,TypeSpecifier*);
	CSymbol *symbol = SARG(1,CSymbol*);
	CompoundStatement *funcBody = SARG(2,CompoundStatement*);
	
	if(!symbol->type() || symbol->type()->type() != SymbolType::SYMT_FUNCTION) {
		//error: illegal declarator for a function
		//todo: change printf
		printf("Error: syntax error.\n");
		SRET(NULL);
		return;
	}
	Function *type = (Function*)symbol->type();
	type->setFunctionBody(funcBody);
	symbol->linkType(*returningType);

	returningType->release();
	SRET(symbol);
}

SEMANTIC_ACTION_C(function_definition3, declarator declaration_list compound_statement)
{
	Statement *funcBody = SARG(2,Statement*);
	int a = 1;
}

SEMANTIC_ACTION_C(function_definition4, declarator compound_statement)
{
	CSymbol *symbol = SARG(0,CSymbol*);
	CompoundStatement *funcBody = SARG(1,CompoundStatement*);

	if(!symbol->type() || symbol->type()->type() != SymbolType::SYMT_FUNCTION) {
		//error: illegal declarator for a function
		//todo: change printf
		printf("Error: syntax error.\n");
		SRET(NULL);
		return;
	}

	Function *type = (Function*)symbol->type();

	//declaration_specifiers not specified, default returning type to int
	TypeSpecifier *returningType = SymbolType::createTypeSpecifier();
	returningType->setDataType(DT_INT);

	//todo: remove printf
	printf("Warning: function '%s' returning unespecified type, defaulting to int...\n",symbol->identifier());
	//

	type->setFunctionBody(funcBody);
	symbol->linkType(*returningType);

	returningType->release();
	SRET(symbol);
}

}