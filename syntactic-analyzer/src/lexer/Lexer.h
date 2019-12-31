//Created in: 26/06/2017 01:50
//Last edit: 31/12/2019 16:25

#pragma once

#ifndef __LEXER_H__
#define __LEXER_H__

#include "Token.h"

namespace syntacticanalyzer {

	class Token;

	class Lexer
	{
	public:
		virtual bool addToken(const char *regex, int tokenId, TokenFunction callback = 0) = 0;
		virtual void setInput(const char* input) = 0;
		virtual void setPos(int pos) = 0;
		virtual const char* input() = 0;
		virtual int pos() = 0;

		virtual Token* nextToken() = 0;
	};

}

#endif
