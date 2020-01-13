
//Created in: 29/12/2019 03:19
//Last edit: 12/01/2020 22:47

#ifndef __SYNTACTIC_ANALYZER_PARSER_H__
#define __SYNTACTIC_ANALYZER_PARSER_H__

#include <dependencies/lexer/lexer.h>

using namespace lexer;

namespace syntacticanalyzer {

	class Grammar;
	class SemanticValue;

	class Parser {

	public:
		virtual void setLexer(Lexer* lexer) = 0;
		virtual bool parse(const char* input) = 0;

		virtual Grammar* grammar() const = 0;
		virtual Lexer* lexer() const = 0;
	};

	class ParsingContext
	{
		virtual Parser* parser() = 0;
		virtual SemanticValue* arg(int n) = 0;
		virtual unsigned int argCount() = 0;
	};

}

#endif