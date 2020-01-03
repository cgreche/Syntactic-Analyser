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


		Token* curToken = _getNextToken();
		for(;;) {
			int tokId = curToken->id();
			const char* text = curToken->rawValue();

			DEBUG_PRINT(*stream, "Current State: ");
			DEBUG_PRINT(*stream, curState);
			DEBUG_PRINT(*stream, std::endl);
		
			DEBUG_PRINT(*stream, "Current Token: ");
			DEBUG_PRINT(*stream, (tokId == -1 ? "Undefined" : m_grammar->symbol(tokId)->name()));
			DEBUG_PRINT(*stream, " (\"");
			DEBUG_PRINT(*stream, text);
			DEBUG_PRINT(*stream, "\")");
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
				//TODO m_parsingState->semanticStack.push_back(m_curToken);

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

				/*TODO: change TokenImpl to SemanticActionImpl
				//do semantic action
				TokenImpl retToken;
				m_parsingState->semanticArgsIndex = m_parsingState->semanticStack.size()-pro->rhsCount();
				retToken.value = m_parsingState->semanticStack[m_parsingState->semanticArgsIndex].value; // defaults to first semantic arg
				memset(retToken.text,0,sizeof(retToken.text));

				if(pro->semanticAction()) {
					pro->semanticAction()(this->m_parsingState,retToken);
				}
				*/

				for(unsigned int j = 0; j < pro->rhsCount(); ++j) {
					m_parsingState.m_stateStack.pop_back();
					//m_parsingState.semanticStack.pop_back();
				}
				m_parsingState.m_currentState = curState = m_parsingState.m_stateStack.back();


				//m_parsingState->semanticStack.push_back(retToken);

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

}