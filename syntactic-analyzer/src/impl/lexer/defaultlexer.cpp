
//Created in: 26/06/2017 01:50
//Last edit: 29/12/2019 04:24

#include "defaultlexer.h"

#include "language/grammar.h"

namespace syntacticanalyzer {

	DefaultLexer::DefaultLexer()
	: m_input(NULL), m_pos(0) {
	}

	DefaultLexer::~DefaultLexer() {
	}

	bool DefaultLexer::addToken(const char *regex, int id, __tokenCallback callback) {

		for(unsigned int i = 0; i < m_tokList.size(); ++i) {
			if(m_tokList[i]->regex == regex) {
				return false;
			}
		}

		TokenEntry *entry = new TokenEntry;
		entry->regex = regex;
		entry->id = id;
		entry->matcher = createRegexMatcher(regex, NULL);
		entry->callback = callback;
		m_tokList.push_back(entry);
		return true;
	}

	int DefaultLexer::nextToken(Token *pToken) {
		bool res = false;
		int biggestMatchLen = 0;
		int biggestMatchEntryIndex = -1;
		const char *c = &m_input[m_pos];
		int matchLen = 0;
		int curPos = 0;
		int tok = -1;

		if(*c == '\0')
			return 0;

		for(unsigned int i = 0; i < m_tokList.size(); ++i) {
			RegexMatcher *matcher = m_tokList[i]->matcher;
			if(res |= matcher->matchPartially(c, &matchLen)) {
				if(matchLen > biggestMatchLen) {
					biggestMatchLen = matchLen;
					biggestMatchEntryIndex = i;
				}
			}
		}

		if(res) {
			TokenEntry *entry = m_tokList[biggestMatchEntryIndex];
			tok = entry->id;

			if(entry->callback) {
				res = (*entry->callback)(pToken);
			}
		}

		//matching function may change res value
		if(!res) {
			//todo:
			//if(m_defaultErrorFunction)
			//	(*m_defaultErrorFunction)(0, input, curPos);
			biggestMatchLen = 1;
		}

		pToken->id = tok;
		::strncpy(pToken->text, c, biggestMatchLen);
		pToken->text[biggestMatchLen] = '\0';

		m_pos += biggestMatchLen;
		return tok;
	}

	void DefaultLexer::makeDefaultTerminalTokens(Grammar *grammar) {
		/*
		unsigned int termCount = grammar->terminalCount();
		SymbolList terminals = grammar->terminalSymbols();

		TerminalSymbol *eofSym = (TerminalSymbol*)grammar->symbol("$eof");
		for(unsigned int i = 0; i < termCount; ++i) {
			if(terminals[i] != eofSym) {
				std::string name = terminals[i]->name();

				const char *regexOrdinalChars = "^.[$()|*+?{";
				std::size_t found = name.find_first_of(regexOrdinalChars);
				while(found != std::string::npos)
				{
					char c = name[found];
					std::string newChars = "\\";
					newChars += c;
					name.replace(found, 1, newChars);
					found = name.find_first_of(regexOrdinalChars, found + newChars.length());
				}
				addToken(name.c_str(), terminals[i]->index());
			}
		}
		*/
	}
}