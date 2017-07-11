
//Last edit: 05/06/2017

#include "c_statement.h"

namespace C_language {

void Statement::appendSibling(Statement *statement)
{
	Statement *stmt = this;
	while(stmt->next()) stmt = stmt->next();
	stmt->m_next = statement;
}



unsigned int CompoundStatement::calculateNecessaryAllocSize() const
{
	unsigned int allocSize = 0;

	CSymbol *decl = m_firstDecl;

	while(decl) {
		//todo: check if variable declared is used
		allocSize += decl->calculateNecessaryAllocSize();
		decl = decl->next();
	}

	return allocSize;
}

}