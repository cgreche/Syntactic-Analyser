#ifndef __C_STATEMENT_H__
#define __C_STATEMENT_H__

#include "c_language.h"

namespace C_language {

class Ast;
class CSymbol;

enum StatementType
{
	STMT_NULL,
	STMT_EXPRESSION,
	STMT_COMPOUND,
	STMT_IF,
	STMT_FOR,
	STMT_WHILE,
	STMT_DOWHILE,
	STMT_SWITCH,
	STMT_RETURN,
	STMT_LABELED,
	STMT_GOTO,
	STMT_BREAK,
	STMT_CONTINUE
};

class Statement
{
	StatementType m_type;
	Statement *m_next;

public:
	Statement(StatementType type) : m_type(type), m_next(0) { }
	void appendSibling(Statement *statement);
	StatementType type() const { return m_type; }
	Statement *next() const { return m_next; }

	//virtual unsigned int sizeNeeded();
};




class NullStatement : public Statement
{
public:
	NullStatement()
		: Statement(STMT_NULL) { }
};



class ExpressionStatement : public Statement
{
	Ast *m_expr;

public:
	ExpressionStatement(Ast *expression)
		: Statement(STMT_EXPRESSION), m_expr(expression) { }

	Ast *expression() const { return m_expr; }
};



class CompoundStatement : public Statement
{
	Scope &m_scope;
	CSymbol *m_firstDecl;
	Statement *m_firstStmt;

public:
	CompoundStatement(Scope& scope, CSymbol *symList, Statement *stmtList)
		: Statement(STMT_COMPOUND), m_scope(scope), m_firstDecl(symList), m_firstStmt(stmtList) { }

	Scope &scope() const { return m_scope; }
	CSymbol *declarationList() const { return m_firstDecl; }
	Statement *statementList() const { return m_firstStmt; }

	unsigned int calculateNecessaryAllocSize() const;
};



class IfStatement : public Statement
{
	Ast *m_expr;
	Statement *m_ifBody;
	Statement *m_elseBody;

public:
	IfStatement(Ast *expr, Statement *ifBody, Statement *elseBody)
		: Statement(STMT_IF), m_expr(expr), m_ifBody(ifBody), m_elseBody(elseBody) { }

	Ast *expression() const { return m_expr; }
	Statement *ifBody() const { return m_ifBody; }
	Statement *elseBody() const { return m_elseBody; }
};



class ForStatement : public Statement
{
	Statement *m_stmt1;
	Statement *m_stmt2;
	Ast *m_expr;

	Statement *m_forBody;

public:
	ForStatement(Statement *stmt1, Statement *stmt2, Ast *expr, Statement *forBody)
		: Statement(STMT_FOR), m_stmt1(stmt1), m_stmt2(stmt2), m_expr(expr), m_forBody(forBody) { }

	Statement *statement1() const { return m_stmt1; }
	Statement *statement2() const { return m_stmt2; }
	Ast *expression() const { return m_expr; }
	Statement *body() const { return m_forBody; }
};



class WhileStatement : public Statement
{
	Ast *m_expr;
	Statement *m_whileBody;

public:
	WhileStatement(Ast *expr, Statement *whileBody)
		: Statement(STMT_WHILE), m_expr(expr), m_whileBody(whileBody) { }

	Ast *expression() const { return m_expr; }
	Statement *body() const { return m_whileBody; }
};



class DoWhileStatement : public Statement
{
	Statement *m_doBody;
	Ast *m_expr;

public:
	DoWhileStatement(Statement *doBody, Ast *expr)
		: Statement(STMT_DOWHILE), m_doBody(doBody), m_expr(expr) { }

	Statement *body() const { return m_doBody; }
	Ast *expression() const { return m_expr; }
};



class SwitchStatement : public Statement
{
public:
	SwitchStatement()
		: Statement(STMT_SWITCH) { }
};



class ReturnStatement : public Statement
{
	Ast *m_expr;

public:
	ReturnStatement(Ast *expr)
		: Statement(STMT_RETURN), m_expr(expr) { }

	Ast *expression() const { return m_expr; }
};



class LabeledStatement : public Statement
{
	int m_labeledType;
	union
	{
		CSymbol *m_identifier;
		Ast *m_expr;
	};
	Statement *m_statement;

public:
	LabeledStatement(CSymbol *identifier, Statement *statement);
	LabeledStatement(Ast *expression, Statement *statement);
	LabeledStatement(Statement *statement);
};

}

#endif