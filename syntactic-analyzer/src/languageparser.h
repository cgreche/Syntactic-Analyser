
//Created in: 29/12/2019 03:19
//Last edit: 29/12/2019 03:19

#ifndef __SYNTACTIC_ANALYZER_LANGUAGE_PARSER_H__
#define __SYNTACTIC_ANALYZER_LANGUAGE_PARSER_H__

#include <vector>

namespace syntacticanalyzer {

	class Grammar;
	class Token;
	class Lexer;

	class LanguageParser {

	public:
		virtual void setLexer(Lexer* lexer) = 0;
		virtual bool parse(const char* input) = 0;

		virtual Grammar* grammar() const = 0;
		virtual Lexer* lexer() const = 0;
	};

	enum ParsingMethod {
		LALR,
		SLR,
		LR0,
		CLR,
		GLR,
		LL1
	};

	LanguageParser* createLanguageParser(Grammar* grammar, Lexer* lexer, ParsingMethod parsingMethod);

	inline LanguageParser* createDefaultParser(Grammar* grammar, Lexer* lexer) {
		return createLanguageParser(grammar, lexer, LALR);
	}

}

#endif