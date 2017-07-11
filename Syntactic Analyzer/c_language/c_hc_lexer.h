
//Last Edit: 26/06/2017 03:26

//Hand coded C lexer

#include "../src/grammaranalyzer.h"

#include "c_language.h"

namespace C_language {

using namespace syntacticanalyzer;

class c_hcLexer : public Lexer {
	GrammarAnalyzer &m_gramAnalyzer;

public:
	c_hcLexer(GrammarAnalyzer& gramAnalyzer);
	virtual int nextToken(Token *pToken);

	int getch() {
		if(m_input[m_pos] == '\0')
			return '\0';
		return m_input[++m_pos];
	}

	int ungetch() {
		return m_input[--m_pos];
	}
};

}
