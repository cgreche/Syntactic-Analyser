//Created in: 26/06/2017 01:50
//Last edit: 18/02/2018 01:09

#pragma once

#ifndef __LEXER_H__
#define __LEXER_H__

namespace syntacticanalyzer {

	struct Token
	{
		int tok;
		char text[256];

		union {
			char cval;
			short sval;
			long lval;
			int ival;
			char *strval;
			unsigned char ucval;
			unsigned short usval;
			unsigned long ulval;
			unsigned int uval;
			unsigned char *bval;
			void *pval;
			float fval;
			double dval;
		}value;

		Token()
			: tok(-1) {
			text[0] = '\0';
		}
	};

	typedef int(*TokenCallback)(Token*);

	class Lexer
	{
	protected:
		const char *m_input;
		int m_pos;
	public:
		Lexer() : m_input(0), m_pos(0) { }
		virtual bool addToken(const char *regex, int tokenValue, TokenCallback callback = 0) = 0;
		virtual void setInput(const char *input) { m_input = input; }
		virtual void setPos(int pos) { m_pos = pos; }
		virtual const char *input() { return m_input; }
		virtual int pos() { return m_pos; }

		virtual int nextToken(Token *pToken) = 0;

	};

}

#endif
