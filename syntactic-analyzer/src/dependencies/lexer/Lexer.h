//Created in: 26/06/2017 01:50
//Last edit: 05/01/2020 23:49

#pragma once

#ifndef __LEXER_H__
#define __LEXER_H__

#include "Token.h"

namespace lexer {

	class Token;
	class Lexer;

	class TokenizationContext {
	public:
		virtual Lexer* lexer() const = 0;
		virtual unsigned int pos() const = 0;
		virtual unsigned int len() const = 0;
		virtual const char* tokenValue() const = 0;

		virtual void accept(int tokId) = 0;
		virtual void error() = 0;
		virtual void ignore() = 0;
	};

	enum TokenAction {
		Accept,
		Error,
		Ignore
	};

	typedef void(*TokenCallbackFunction)(Token*, TokenizationContext*);
	typedef int(*TokenErrorFunction)(TokenizationContext*);

	class Lexer
	{
	public:
		virtual bool addToken(const char* regex, int tokenId, TokenCallbackFunction callback = 0, TokenAction defaultAction = Accept) = 0;
		virtual void setInput(const char* input) = 0;
		virtual void setPos(int pos) = 0;
		virtual const char* input() = 0;
		virtual int pos() = 0;

		virtual void setDefaultTokenCallback(TokenCallbackFunction tokenFunction) = 0;
		virtual void setTokenErrorFunction(TokenErrorFunction tokenFunction) = 0;

		virtual Token* nextToken() = 0;
	};

	Lexer* createDefaultLexer();
}

#endif
