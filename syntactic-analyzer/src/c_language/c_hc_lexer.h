
//Last Edit: 18/02/2018 04:41

//Hand coded C lexer

#include <SyntacticAnalyzer.h>
#include "c_language.h"

namespace C_language {

using namespace syntacticanalyzer;

class c_hcLexer : public Lexer {

	int getch() {
		if(m_input[m_pos] == '\0')
			return '\0';
		return m_input[m_pos++];
	}

	int ungetch() {
		return m_input[--m_pos];
	}

public:
	c_hcLexer();
	virtual bool addToken(const char *,int,syntacticanalyzer::TokenFunction) { return true; }
	virtual int nextToken(Token *pToken);
};

}
