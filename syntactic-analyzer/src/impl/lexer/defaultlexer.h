
//Created in: 26/06/2017 01:50
//Last edit: 08/01/2020 00:10

#pragma once

#ifndef __DEFAULTLEXER_H__
#define __DEFAULTLEXER_H__

#include <map>
#include <vector>
#include <string>
#include <dependencies/lexer/Lexer.h>
#include <dependencies/regex-parser/RegexMatcher.h>

using namespace lexer;

namespace syntacticanalyzer {

	class Grammar;

	class TokenizationContextImpl : public TokenizationContext {
		Lexer& m_lexer;

		unsigned int m_pos;
		unsigned int m_len;
		std::string m_tokenValue;

		TokenAction m_action;
		int m_acceptedToken;

	public:
		friend class DefaultLexer;
		TokenizationContextImpl(Lexer& lexer)
		: m_lexer(lexer) {
			m_pos = 0;
			m_len = 0;

			m_action = TokenAction::Accept;
			m_acceptedToken = 0;
		}

		virtual Lexer* lexer() const { return &m_lexer; }
		virtual unsigned int pos() const { return m_pos; }
		virtual unsigned int len() const { return m_len; }
		virtual const char* tokenValue() const { return m_tokenValue.c_str(); }

		virtual void accept(int tokId) { m_action = Accept; m_acceptedToken = tokId; }
		virtual void error() { m_action = Error; }
		virtual void ignore() { m_action = Ignore; }

	};

	class DefaultLexer : public Lexer
	{
		struct TokenEntry {
			std::string regex;
			int id;
			RegexMatcher *matcher;
			TokenCallbackFunction callback;
			TokenAction defaultAction;
		};

		const char* m_input;
		int m_pos;
		std::vector<TokenEntry*> m_tokList;

		TokenizationContextImpl *m_context;
		TokenCallbackFunction m_defaultCallbackFunction;
		TokenErrorFunction m_errorFunction;

	public:
		DefaultLexer();
		~DefaultLexer();

		virtual bool addToken(const char* regex, int tokenId, TokenCallbackFunction callback = 0, TokenAction defaultAction = Accept);
		virtual void setInput(const char* input) { m_input = input; }
		virtual void setPos(int pos) { m_pos = pos; }
		virtual const char* input() { return m_input; }
		virtual int pos() { return m_pos; }

		virtual void setDefaultTokenCallback(TokenCallbackFunction callbackFunction) { m_defaultCallbackFunction = callbackFunction; }
		virtual void setTokenErrorFunction(TokenErrorFunction errorFunction) { m_errorFunction = errorFunction; }

		virtual Token* nextToken();
	};

}
#endif
