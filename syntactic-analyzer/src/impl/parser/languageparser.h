
//Created in: 18/02/2018 03:17
//Last edit: 31/12/2019 20:22

#ifndef __SYNTACTIC_ANALYZER_LANGUAGE_PARSER_IMPL_H__
#define __SYNTACTIC_ANALYZER_LANGUAGE_PARSER_IMPL_H__

#include <vector>
#include "../../LanguageParser.h"
#include "../../ParsingContext.h"
#include "../../lexer/token.h" //TODO: adjust
#include "../language/grammar.h"
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
	typedef std::vector<SemanticValue*> SemanticStack;

	class LanguageParser;

	class ParsingState : public ParsingContext
	{
		friend class LanguageParserImpl;

		LanguageParser &m_parser;
		int m_currentState;
		StateStack m_stateStack;
		SemanticStack m_semanticStack;
		unsigned int m_semanticArgsIndex;

		ParsingState(LanguageParser& parser)
			: m_parser(parser), m_currentState(0), m_semanticArgsIndex(0) {
		}

	public:
		virtual LanguageParser* parser() { return &m_parser; }
		virtual SemanticValue* arg(int n) { return m_semanticStack[m_semanticArgsIndex + n]; }
		virtual unsigned int argCount() { return m_semanticStack.size() - m_semanticArgsIndex; }
	};

	class LanguageParserImpl : public LanguageParser {
		Grammar *m_grammar;
		Lexer *m_lexer;

		GrammarAnalyzer *m_grammarAnalyzer;
		ParsingState m_parsingState;

		//
		bool _parse(std::ostream *stream);
		Token* _getNextToken();

	public:
		LanguageParserImpl();
		~LanguageParserImpl();

		virtual void setLexer(Lexer *lexer);
		virtual void setGrammar(Grammar *grammar);
		virtual bool parse(const char *input);
		bool parse(const char *input, std::ostream *stream); //parse an input file

		virtual Lexer *lexer() const { return m_lexer; }
		virtual Grammar* grammar() const { return m_grammar; }
	};

}

#endif