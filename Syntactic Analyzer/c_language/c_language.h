#ifndef __C_LANGUAGE_H__
#define __C_LANGUAGE_H__

#include "c_defs.h"
#include "c_object.h"
#include "c_scope.h"
#include "c_value.h"
#include "c_ast.h"
#include "c_symboltype.h"
#include "c_symbol.h"
#include "c_statement.h"

#undef NULL
#define NULL 0

namespace C_language {

int C_enterScope();
int C_exitScope();
Scope *C_curScope();
Scope *C_globalScope();
void C_insertSymbolChainIntoCurrentScope(CSymbol *symbolChain);

int C_enterBlock();
int C_exitBlock();

}

#endif