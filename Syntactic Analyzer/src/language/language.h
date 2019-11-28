
//Created in: 18/02/2018 03:17
//Last edit: 18/02/2018 03:17

#ifndef __SYNTACTIC_ANALYZER_LANGUAGE_H__
#define __SYNTACTIC_ANALYZER_LANGUAGE_H__

namespace syntacticanalyzer {

	class Grammar;

	class Language {
		Grammar *m_grammar;
		int **m_parsingTable;

		public:
			Language(Grammar *grammar, int **parsingTable) {
				m_grammar = grammar;
				m_parsingTable = parsingTable;
			}

			Grammar *grammar() const { return m_grammar; }
			int **parsingTable() const { return m_parsingTable; }
	};

}

#endif