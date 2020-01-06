#include <iostream>
#include "languageparser.h"

#ifdef _DEBUG
	#define DEBUG_PRINT(s,t) (s << t)
#else
	#define DEBUG_PRINT(s,t)
#endif

namespace syntacticanalyzer {

	bool LanguageParserImpl::_parse(std::ostream *stream)
	{
		int curState;

		if(!stream)
			stream = &std::cout;

		//[Debug]
		DEBUG_PRINT(*stream, "Start parsing.\n");
		//[/Debug]

		m_parsingState.m_currentState = curState = 0;
		m_parsingState.m_stateStack.push_back(0);
		int** parsingTable = m_grammarAnalyzer->parsingTable();

		int tokId;


		Token* curToken = _getNextToken();
		for(;;) {
			DEBUG_PRINT(*stream, "Current State: ");
			DEBUG_PRINT(*stream, curState);
			DEBUG_PRINT(*stream, std::endl);

			int tokId;
			const char* text;
			if (curToken) {
				tokId = curToken->id();
				text = curToken->rawValue();
			}
			else {
				tokId = m_grammar->symbol("$eof")->index();
				text = NULL;
			}

			DEBUG_PRINT(*stream, "Current Token: ");
			DEBUG_PRINT(*stream, (tokId == -1 ? "Undefined" : m_grammar->symbol(tokId)->name()));
			if (text) {
				DEBUG_PRINT(*stream, " (\"");
				DEBUG_PRINT(*stream, text);
				DEBUG_PRINT(*stream, "\")");
			}
			DEBUG_PRINT(*stream, std::endl);

			if(tokId == -1) {
				//undefined token
				//todo: error
				curToken = _getNextToken();
				continue;
			}

			unsigned int action = 0;

	#if 1
			
			int value;
			action = GET_ACTION(parsingTable[curState][tokId]);
			value = GET_VALUE(parsingTable[curState][tokId]);
			if(action == ACTION_SHIFT) {

				//[DEBUG]
				DEBUG_PRINT(*stream, "Action: shift ");
				DEBUG_PRINT(*stream, value);
				DEBUG_PRINT(*stream, " (");
				DEBUG_PRINT(*stream, m_grammar->symbol(tokId)->name());
				DEBUG_PRINT(*stream, ')');
				DEBUG_PRINT(*stream, std::endl);
				//[/DEBUG]

				m_parsingState.m_currentState = curState = value;
				m_parsingState.m_stateStack.push_back(value);
				m_parsingState.m_semanticStack.push_back(new SemanticValueImpl(curToken));

 				curToken = _getNextToken();
			}
			else if(action == ACTION_REDUCE) {
				Production *pro = m_grammar->production(value);
				//[Debug]
				DEBUG_PRINT(*stream, "Action: reduce ");
				DEBUG_PRINT(*stream, value);
				DEBUG_PRINT(*stream, " (");
				DEBUG_PRINT(*stream, pro->lhs()->name());
				DEBUG_PRINT(*stream, " -> ");
	#ifdef _DEBUG
				for(unsigned int nsym = 0; nsym < pro->rhsCount(); ++nsym) {
					DEBUG_PRINT(*stream, pro->rhs(nsym)->name());
					if(nsym < pro->rhsCount()-1)
						DEBUG_PRINT(*stream, " ");
				}
	#endif
				DEBUG_PRINT(*stream, ')');
				DEBUG_PRINT(*stream, std::endl);
				//[/Debug]

				//do semantic action
				SemanticValueImpl *retValue = new SemanticValueImpl;
				m_parsingState.m_semanticArgsIndex = m_parsingState.m_semanticStack.size() - pro->rhsCount();
				retValue->setValue(m_parsingState.m_semanticStack[m_parsingState.m_semanticArgsIndex]->value()); // defaults to first semantic arg

				if(pro->semanticAction()) {
					pro->semanticAction()(m_parsingState,*retValue);
				}

				for(unsigned int j = 0; j < pro->rhsCount(); ++j) {
					m_parsingState.m_stateStack.pop_back();
					m_parsingState.m_semanticStack.pop_back();
				}
				m_parsingState.m_currentState = curState = m_parsingState.m_stateStack.back();

				m_parsingState.m_semanticStack.push_back(retValue);

				//do the goto
				value = GET_VALUE(parsingTable[curState][pro->lhs()->index()]);
				m_parsingState.m_currentState = curState = value;
				m_parsingState.m_stateStack.push_back(value);
			}
			else if(action == ACTION_ACCEPT) {
				//[DEBUG]
				DEBUG_PRINT(*stream, "Action: accept");
				DEBUG_PRINT(*stream, std::endl);
				//[/DEBUG]
				int accept = 1;
				return true;
			}
			else {
				int a = 1;
				//error
				break;
			}

	#endif
			DEBUG_PRINT(*stream, std::endl);
		}

		return false;
	}

	Token* LanguageParserImpl::_getNextToken()
	{
 		return m_lexer->nextToken();
	}

	LanguageParserImpl::LanguageParserImpl()
		: m_parsingState(*this), m_lexer(NULL), m_grammar(NULL), m_grammarAnalyzer(NULL) {
	}

	LanguageParserImpl::~LanguageParserImpl() {
		if (m_grammarAnalyzer)
			delete m_grammarAnalyzer;
	}

	void LanguageParserImpl::setLexer(Lexer* lexer) {
		m_lexer = lexer;
	}

	void LanguageParserImpl::setGrammar(Grammar* grammar) {
		m_grammar = grammar;
		m_grammarAnalyzer = new GrammarAnalyzer(*grammar);
		m_grammarAnalyzer->lr0(NULL);
		m_grammarAnalyzer->computeNullable();
		m_grammarAnalyzer->computeFirst();
		m_grammarAnalyzer->computeFollow();
		m_grammarAnalyzer->lalr(NULL);
		m_grammarAnalyzer->generateParsingTable();
	}

	bool LanguageParserImpl::parse(const char* input, std::ostream* stream)
	{
		if (!m_lexer)
			return false;
		m_lexer->setInput(input);
		m_lexer->setPos(0);
		return _parse(stream);
	}

	bool LanguageParserImpl::parse(const char* input)
	{
		return parse(input, NULL);
	}

	LanguageParser* createDefaultParser(Grammar* grammar, Lexer* lexer) {
		LanguageParserImpl* parser = new LanguageParserImpl;
		parser->setGrammar(grammar);
		parser->setLexer(lexer);
		return parser;
	}

}