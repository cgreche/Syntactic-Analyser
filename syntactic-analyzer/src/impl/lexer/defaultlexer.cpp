
//Created in: 26/06/2017 01:50
//Last edit: 08/01/2020 00:09

#include "defaultlexer.h"
#include "token.h"

namespace syntacticanalyzer {

	DefaultLexer::DefaultLexer()
	: m_input(NULL), m_pos(0) {
		m_context = new TokenizationContextImpl(*this);
		m_defaultCallbackFunction = NULL;
		m_errorFunction = NULL;
	}

	DefaultLexer::~DefaultLexer() {
		if (m_context)
			delete m_context;
	}

	bool DefaultLexer::addToken(const char *regex, int id, TokenCallbackFunction callback, TokenAction action) {
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
		entry->defaultAction = action;
		m_tokList.push_back(entry);
		return true;
	}

	Token* DefaultLexer::nextToken() {
		bool res = false;
		int biggestMatchLen = 0;
		int biggestMatchEntryIndex = -1;
		const char *c = &m_input[m_pos];
		int matchLen = 0;
		TokenAction defaultAction = Error;

		if(*c == '\0')
			return NULL;

		for(unsigned int i = 0; i < m_tokList.size(); ++i) {
			RegexMatcher *matcher = m_tokList[i]->matcher;
			if(res |= matcher->matchPartially(c, &matchLen)) {
				if(matchLen > biggestMatchLen) {
					biggestMatchLen = matchLen;
					biggestMatchEntryIndex = i;
				}
			}
		}

		std::string text;
		text.assign(c, biggestMatchLen);

		m_context->m_pos = m_pos;
		m_context->m_len = biggestMatchLen;
		m_context->m_tokenValue = text;
		m_context->m_action = defaultAction;

		m_pos += biggestMatchLen;

		TokenImpl* token = NULL;

		if(res) {
			TokenEntry *entry = m_tokList[biggestMatchEntryIndex];
			token = new TokenImpl(entry->id, text.c_str());
			m_context->m_acceptedToken = entry->id;
			m_context->m_action = entry->defaultAction;
			if (entry->callback) {
				(*entry->callback)(token, m_context);
			}
			else {
				if (m_defaultCallbackFunction)
					m_defaultCallbackFunction(token, m_context);
			}
		}

		//Can't be in else, matching function may change res value
		if(m_context->m_action == Error) {
			if (m_errorFunction)
				m_errorFunction(m_context);
		}

		if (m_context->m_action == Ignore)
			return this->nextToken();

		if (m_context->m_action == Accept && (token == NULL || m_context->m_acceptedToken != token->id()))
			return new TokenImpl(m_context->m_acceptedToken, m_context->m_tokenValue.c_str());
		return token;
	}

}

namespace lexer {
	Lexer* createDefaultLexer() {
		return new syntacticanalyzer::DefaultLexer;
	}
}
