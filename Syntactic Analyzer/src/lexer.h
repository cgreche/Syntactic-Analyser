//Created in: 26/06/2017 01:50
//Last edit: 26/06/2017 01:50

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
			: tok(-1)
		{
			text[0] = '\0';
		}
	};

	class Lexer
	{
	protected:
		const char *m_input;
		int m_pos;
	public:
		Lexer() : m_input(0), m_pos(0) { }
		virtual void setInput(const char *input) { m_input = input; }
		virtual void setPos(int pos) { m_pos = pos; }
		virtual int nextToken(Token *pToken) = 0;
		virtual const char *getInput() { return m_input; }
		virtual int getPos() { return m_pos; }
	};

}

#endif
