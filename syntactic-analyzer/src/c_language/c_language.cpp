
#include "c_language.h"
#include "../../project/vs2019/CParser/lib/stack.h"

namespace C_language {

Scope g_globalScope(0,0);
Scope *g_curScope = &g_globalScope;
CSymbol *g_firstGlobalSymbol = NULL;
CSymbol *g_lastGlobalSymbol = NULL;

stack<int> g_blockNumberStack;

int g_blockNumber = 0;
int g_curBlockNumber = 0;
int g_curNestLevel = 0;

int C_curNestLevel()
{
	return g_curNestLevel;
}

int C_curBlockNumber()
{
	return g_curBlockNumber;
}

int C_enterBlock()
{
	g_blockNumberStack.push(g_curBlockNumber);
	return (g_curBlockNumber = ++g_blockNumber);
}

int C_exitBlock()
{
	return (g_curBlockNumber = g_blockNumberStack.pop());
}

int C_enterScope()
{
	int curLevel = ++g_curNestLevel;
	if(g_curScope->level() < curLevel) {
		Scope *newScope = new Scope(g_curBlockNumber,curLevel);
		newScope->setParent(g_curScope);
		g_curScope = newScope;
	}

	return curLevel;
}

int C_exitScope()
{
	int curLevel = g_curNestLevel-1;
	//if nest level is below 0, give an error (if it is below GLOBAL level)
	if(curLevel < 0) {
		//todo: change printf
		printf("Fatal Error: nesting level below GLOBAL level\n");
	}
	else {
		--g_curNestLevel;
		if(g_curScope->level() > curLevel) {
			g_curScope = g_curScope->parent();
		}
	}

	return curLevel;
}

void C_insertSymbolChainIntoCurrentScope(CSymbol *symbolChain)
{
	CSymbol *firstSymbol, *lastSymbol;
	firstSymbol = lastSymbol = symbolChain;
	while(symbolChain) {
		Scope *curScope = C_curScope();
		if(curScope->findSymbolWithinMainLevel(symbolChain->identifier())) {
			int alreadyExists = 1;
		}

		curScope->insertSymbol(symbolChain);
		lastSymbol = symbolChain;
		symbolChain = symbolChain->next();
	}

	//update first and last global symbols
	if(!g_firstGlobalSymbol)
		g_firstGlobalSymbol = firstSymbol;
	else
		g_lastGlobalSymbol->appendSibling(firstSymbol);

	g_lastGlobalSymbol = lastSymbol;
}

Scope *C_curScope()
{
	return g_curScope;
}

Scope *C_globalScope()
{
	return &g_globalScope;
}

}