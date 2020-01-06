//Created in: 26/06/2017 01:50
//Last edit: 05/01/2020 23:49

#pragma once

#ifndef __LEXER_H__
#define __LEXER_H__

#include "Token.h"

namespace syntacticanalyzer {

	class Token;

	typedef int(*TokenInterceptor)(Token*, Lexer*);
	typedef void(*TokenCallbackFunction)(Token*, Lexer*);
	typedef int(*TokenErrorFunction)(Token*, Lexer*);

	class Lexer
	{
	public:
		virtual bool addToken(const char *regex, int tokenId, TokenCallbackFunction callback = 0) = 0;
		virtual void setInput(const char* input) = 0;
		virtual void setPos(int pos) = 0;
		virtual const char* input() = 0;
		virtual int pos() = 0;

		virtual void setTokenInterceptor(TokenInterceptor interceptor) = 0;
		virtual void setTokenDefaultCallback(TokenCallbackFunction tokenFunction) = 0;
		virtual void setTokenDefaultErrorFunction(TokenErrorFunction tokenFunction) = 0;

		virtual Token* nextToken() = 0;
	};

}

#endif
