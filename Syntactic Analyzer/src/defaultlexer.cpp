
//Created in: 26/06/2017 01:50
//Last edit: 26/06/2017 01:50

#include "defaultlexer.h"

namespace syntacticanalyzer {

	DefaultLexer::DefaultLexer() {
	}

	DefaultLexer::~DefaultLexer() {

	}

	bool DefaultLexer::addToken(const char *regex, int id, __tokenCallback callback) {

		for(int i = 0; i < m_tokList.size(); ++i) {
			if(m_tokList[i]->regex == regex) {
				return false;
			}
		}

		TokenEntry *entry = new TokenEntry;
		entry->regex = regex;
		entry->id = id;
		entry->matcher = new RegexMatcher(regex, NULL);
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

		for(int i = 0; i < m_tokList.size(); ++i) {
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

		pToken->tok = tok;
		strncpy(pToken->text, c, biggestMatchLen);
		pToken->text[biggestMatchLen] = '\0';

		m_pos += biggestMatchLen;
		return tok;
	}

}