
//Created in: 18/02/2018 03:17
//Last edit: 18/02/2018 04:02

#ifndef __SYNTACTIC_ANALYZER_LANGUAGE_PARSER__
#define __SYNTACTIC_ANALYZER_LANGUAGE_PARSER__

#include <vector>
#include "language/language.h"
#include "language/grammar.h"
#include "grammaranalyzer.h"


namespace syntacticanalyzer {

#define SARG_(x) state->semanticStack[state->semanticArgsIndex + x]
#define SARG_COUNT (state->semanticStack.size() - state->semanticArgsIndex)
#define SARG_TOK(x) (SARG_(x).tok)
#define SARG_TEXT(x) (SARG_(x).text)
#define SARG(x,t) ((t)SARG_(x).value.pval)
#define SARG_CVAL(x) (SARG_(x).value.cval)
#define SARG_SVAL(x) (SARG_(x).value.sval)
#define SARG_LVAL(x) (SARG_(x).value.lval)
#define SARG_UCVAL(x) (SARG_(x).value.ucval)
#define SARG_USVAL(x) (SARG_(x).value.usval)
#define SARG_ULVAL(x) (SARG_(x).value.ulval)
#define SARG_IVAL(x) (SARG_(x).value.ival)
#define SARG_UVAL(x) (SARG_(x).value.uval)
#define SARG_PVAL(x) (SARG_(x).value.pval)
#define SARG_FVAL(x) (SARG_(x).value.fval)
#define SARG_DVAL(x) (SARG_(x).value.dval)
#define SRET(x) ret.value.pval = (void*)x
#define SRET_CVAL(x) (ret.value.cval = x)
#define SRET_SVAL(x) (ret.value.sval = x)
#define SRET_LVAL(x) (ret.value.lval = x)
#define SRET_UCVAL(x) (ret.value.ucval = x)
#define SRET_USVAL(x) (ret.value.usval = x)
#define SRET_ULVAL(x) (ret.value.ulval = x)
#define SRET_IVAL(x) (ret.value.ival = x)
#define SRET_UVAL(x) (ret.value.uval = x)
#define SRET_PVAL(x) (ret.value.pval = x)
#define SRET_FVAL(x) (ret.value.fval = x)
#define SRET_DVAL(x) (ret.value.dval = x)

	typedef std::vector<unsigned int> StateStack;
	typedef std::vector<Token> SemanticStack;

	class LanguageParser;

	struct ParsingState
	{
		LanguageParser &parser;
		int currentState;
		StateStack stateStack;
		SemanticStack semanticStack;
		unsigned int semanticArgsIndex;

		ParsingState(LanguageParser &parser)
			: parser(parser), currentState(NULL) {
		}
	};

	class LanguageParser {
		Language *m_language;
		Lexer *m_lexer;

		ParsingState *m_parsingState;
		Token m_curToken;

		//
		bool _parse(std::ostream *stream);
		int _getNextToken();

	public:
		LanguageParser(Language *language);
		void setTokenizer(Lexer *lexer);
		bool parse(const char *input, std::ostream *stream); //parse an input file

		Language *language() const { return m_language; }
		Lexer *lexer() const { return m_lexer; }
	};

}

#endif