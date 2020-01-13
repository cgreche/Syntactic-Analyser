
#include <Grammar.h>
#include <lexer/Lexer.h>
//#include <impl/lexer/defaultlexer.h>
#include <Parser.h>
#include <lib/regex-parser/RegexMatcher.h>

namespace C_language {
	using namespace syntacticanalyzer;
	extern Grammar* C_grammar_create();
}


using namespace C_language;


int main() {
	Grammar* grammar = C_grammar_create();
	/*
	Lexer* lexer = createDefaultLexer();
	lexer->addToken("a", grammar->symbol("a")->index(), 0);
	lexer->addToken("b", grammar->symbol("b")->index(), 0);
	lexer->addToken("c", grammar->symbol("c")->index(), 0);
	lexer->addToken("d", grammar->symbol("d")->index(), 0);
	lexer->addToken("e", grammar->symbol("e")->index(), 0);
	lexer->addToken("[ \\t\\n\\r]+", -2, SkipBlankCallback, Ignore);
	Parser* parser = createDefaultParser(grammar, lexer);
	parser->parse("a e c");
	*/
	return 0;
}