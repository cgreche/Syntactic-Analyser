
//Created in: 18/02/2018 03:17
//Last edit: 12/01/2020 20:57

#ifndef __SYNTACTIC_ANALYZER_PARSER_IMPL_H__
#define __SYNTACTIC_ANALYZER_PARSER_IMPL_H__

#include <vector>
#include <Parser.h>
#include <Grammar.h>
#include "grammaranalyzer.h"

using namespace lexer;

namespace syntacticanalyzer {

	typedef std::vector<unsigned int> StateStack;
	typedef std::vector<SemanticValue*> SemanticStack;

	class ParsingState : public ParsingContext
	{
		friend class ParserImpl;

		Parser &m_parser;
		int m_currentState;
		StateStack m_stateStack;
		SemanticStack m_semanticStack;
		unsigned int m_semanticArgsIndex;

		ParsingState(Parser& parser)
			: m_parser(parser), m_currentState(0), m_semanticArgsIndex(0) {
		}

	public:
		virtual Parser* parser() { return &m_parser; }
		virtual SemanticValue* arg(int n) { return m_semanticStack[m_semanticArgsIndex + n]; }
		virtual unsigned int argCount() { return m_semanticStack.size() - m_semanticArgsIndex; }
	};

	class ParserImpl : public Parser {
		Grammar *m_grammar;
		Lexer *m_lexer;

		GrammarAnalyzer *m_grammarAnalyzer;
		ParsingState m_parsingState;

		//
		bool _parse(std::ostream *stream);
		Token* _getNextToken();

	public:
		ParserImpl();
		~ParserImpl();

		virtual void setLexer(Lexer *lexer);
		virtual void setGrammar(Grammar *grammar);
		virtual bool parse(const char *input);
		bool parse(const char *input, std::ostream *stream); //parse an input file

		virtual Lexer *lexer() const { return m_lexer; }
		virtual Grammar* grammar() const { return m_grammar; }
	};

}

#endif