#pragma once

#include "dependencies/lexer/Lexer.h"
#include "GrammarBuilder.h"
#include "Parser.h"

namespace syntacticanalyzer {
	
	GrammarBuilder* createDefaultGrammarBuilder();
	Parser* createDefaultParser(Grammar* grammar, Lexer* lexer);

}