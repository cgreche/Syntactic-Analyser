//Created in: 26/06/2017 01:50
//Last edit: 29/12/2019 13:13

#pragma once

#ifndef __SYNTACTIC_ANALYZER_LEXER_H__
#define __SYNTACTIC_ANALYZER_LEXER_H__

#include "Token.h"

namespace syntacticanalyzer {

	class Token;

	class Lexer
	{
	protected:
		const char *m_input;
		int m_pos;
	public:
		Lexer() : m_input(0), m_pos(0) { }
		virtual bool addToken(const char *regex, int tokenValue, TokenFunction callback = 0) = 0;
		virtual void setInput(const char *input) { m_input = input; }
		virtual void setPos(int pos) { m_pos = pos; }
		virtual const char *input() { return m_input; }
		virtual int pos() { return m_pos; }

		virtual int nextToken(Token *pToken) = 0;

	};

}

#endif
