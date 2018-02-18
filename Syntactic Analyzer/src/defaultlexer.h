
//Created in: 26/06/2017 01:50
//Last edit: 26/06/2017 01:50

#pragma once

#ifndef __DEFAULTLEXER_H__
#define __DEFAULTLEXER_H__

#include "lexer.h"

#include <vector>
#include <string>
#include <RegexMatcher.h>

namespace syntacticanalyzer {

	typedef int(*__tokenCallback)(Token*);
	class Grammar;

	class DefaultLexer : public Lexer
	{
		struct TokenEntry {
			std::string regex;
			int id;
			RegexMatcher *matcher;
			__tokenCallback callback;
		};

		std::vector<TokenEntry*> m_tokList;

	public:
		DefaultLexer();
		~DefaultLexer();
		virtual int nextToken(Token *pToken);
		bool addToken(const char *regex, int id, __tokenCallback callback = 0);

		void makeDefaultTerminalTokens(Grammar *grammar);
	};

}
#endif
