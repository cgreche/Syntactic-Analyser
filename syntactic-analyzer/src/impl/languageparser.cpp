#include <iostream>
#include "languageparser.h"
#include "lexer.h"

#ifdef _DEBUG
	#define DEBUG_PRINT(s,t) (s << t)
#else
	#define DEBUG_PRINT(s,t)
#endif

namespace syntacticanalyzer {

	bool LanguageParser::_parse(std::ostream *stream)
	{
		int curState;

		m_parsingState = new ParsingState(*this);

		if(!stream)
			stream = &std::cout;

		//[Debug]
		DEBUG_PRINT(*stream, "Start parsing.\n");
		//[/Debug]

		m_parsingState->currentState = curState = 0;
		m_parsingState->stateStack.push_back(0);
		int **parsingTable = m_language->parsingTable();

		int tok;
		tok = _getNextToken();
		for(;;) {
			char *text = m_curToken.text;

			DEBUG_PRINT(*stream, "Current State: ");
			DEBUG_PRINT(*stream, curState);
			DEBUG_PRINT(*stream, std::endl);
		
			DEBUG_PRINT(*stream, "Current Token: ");
			DEBUG_PRINT(*stream, (tok == -1 ? "Undefined" : m_language->grammar()->symbolList()[tok]->name()));
			DEBUG_PRINT(*stream, " (\"");
			DEBUG_PRINT(*stream, text);
			DEBUG_PRINT(*stream, "\")");
			DEBUG_PRINT(*stream, std::endl);

			if(tok == -1) {
				//undefined token
				//todo: error
				tok = _getNextToken();
				continue;
			}

			unsigned int action = 0;

	#if 1
			
			int value;
			action = GET_ACTION(parsingTable[curState][tok]);
			value = GET_VALUE(parsingTable[curState][tok]);
			if(action == ACTION_SHIFT) {

				//[DEBUG]
				DEBUG_PRINT(*stream, "Action: shift ");
				DEBUG_PRINT(*stream, value);
				DEBUG_PRINT(*stream, " (");
				DEBUG_PRINT(*stream, m_language->grammar()->symbol(tok)->name());
				DEBUG_PRINT(*stream, ')');
				DEBUG_PRINT(*stream, std::endl);
				//[/DEBUG]

				m_parsingState->currentState = curState = value;
				m_parsingState->stateStack.push_back(value);
				m_parsingState->semanticStack.push_back(m_curToken);

 				tok = _getNextToken();
			}
			else if(action == ACTION_REDUCE) {
				Production *pro = m_language->grammar()->production(value);
				//[Debug]
				DEBUG_PRINT(*stream, "Action: reduce ");
				DEBUG_PRINT(*stream, value);
				DEBUG_PRINT(*stream, " (");
				DEBUG_PRINT(*stream, pro->lhs()->name());
				DEBUG_PRINT(*stream, " -> ");
	#ifdef _DEBUG
				for(u32 nsym = 0; nsym < pro->nrhs(); ++nsym) {
					DEBUG_PRINT(*stream, pro->rhs(nsym)->name());
					if(nsym < pro->nrhs()-1)
						DEBUG_PRINT(*stream, " ");
				}
	#endif
				DEBUG_PRINT(*stream, ')');
				DEBUG_PRINT(*stream, std::endl);
				//[/Debug]

				//do semantic action
				Token retToken;
				m_parsingState->semanticArgsIndex = m_parsingState->semanticStack.size()-pro->nrhs();
				retToken.value = m_parsingState->semanticStack[m_parsingState->semanticArgsIndex].value; // defaults to first semantic arg
				memset(retToken.text,0,sizeof(retToken.text));

				if(pro->semanticAction()) {
					pro->semanticAction()(this->m_parsingState,retToken);
				}

				for(unsigned int j = 0; j < pro->nrhs(); ++j) {
					m_parsingState->stateStack.pop_back();
					m_parsingState->semanticStack.pop_back();
				}
				m_parsingState->currentState = curState = m_parsingState->stateStack.back();
				m_parsingState->semanticStack.push_back(retToken);

				//do the goto
				value = GET_VALUE(parsingTable[curState][pro->lhs()->index()]);
				m_parsingState->currentState = curState = value;
				m_parsingState->stateStack.push_back(value);
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

		if(m_parsingState)
			delete m_parsingState;
		return false;
	}

	int LanguageParser::_getNextToken()
	{
		//reset tok info
		m_curToken.id = -1;
		memset(m_curToken.text,0,sizeof(m_curToken.text));
 		return m_lexer->nextToken(&m_curToken);
	}

	LanguageParser::LanguageParser(Language *language) {
		m_language = language;
	}

	void LanguageParser::setTokenizer(Lexer *lexer) {
		m_lexer = lexer;
	}

	bool LanguageParser::parse(const char *input, std::ostream *stream)
	{
		if(!m_lexer)
			return false;
		m_lexer->setInput(input);
		m_lexer->setPos(0);
		return _parse(stream);
	}

}