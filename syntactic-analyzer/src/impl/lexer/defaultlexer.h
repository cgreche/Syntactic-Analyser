
//Created in: 26/06/2017 01:50
//Last edit: 31/12/2019 16:27

#pragma once

#ifndef __DEFAULTLEXER_H__
#define __DEFAULTLEXER_H__

#include <vector>
#include <string>
#include <regex-parser/RegexMatcher.h>

#include "../../lexer/Lexer.h" //TODO: adjust

namespace syntacticanalyzer {

	class Grammar;

	class DefaultLexer : public Lexer
	{
		struct TokenEntry {
			std::string regex;
			int id;
			RegexMatcher *matcher;
			TokenCallbackFunction callback;
		};

		const char* m_input;
		int m_pos;
		std::vector<TokenEntry*> m_tokList;

		TokenInterceptor m_tokenInterceptor;
		TokenCallbackFunction m_defaultCallbackFunction;
		TokenErrorFunction m_defaultErrorFunction;

	public:
		DefaultLexer();
		~DefaultLexer();

		virtual bool addToken(const char* regex, int id, TokenCallbackFunction callback = 0);

		virtual void setInput(const char* input) { m_input = input; }
		virtual void setPos(int pos) { m_pos = pos; }
		virtual const char* input() { return m_input; }
		virtual int pos() { return m_pos; }

		virtual void setTokenInterceptor(TokenInterceptor tokenInterceptor) { m_tokenInterceptor = tokenInterceptor; }
		virtual void setTokenDefaultCallback(TokenCallbackFunction callbackFunction) { m_defaultCallbackFunction = callbackFunction; }
		virtual void setTokenDefaultErrorFunction(TokenErrorFunction errorFunction) { m_defaultErrorFunction = errorFunction; }

		virtual Token* nextToken();

		void makeDefaultTerminalTokens(Grammar *grammar);
	};

	Lexer *createDefaultLexer();

}
#endif
