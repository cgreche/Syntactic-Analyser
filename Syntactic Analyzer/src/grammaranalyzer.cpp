
//Last edit: 26/06/2017 19:23

#include "grammaranalyzer.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <set>
#include <cstdlib> //abs

namespace syntacticanalyzer {
	//
	// Grammar Analyzer
	//

	GrammarAnalyzer::GrammarAnalyzer(Grammar &grammar)
	{
		m_grammar = &grammar;
		m_parsingTable = NULL;
		m_lexer = NULL;
	}

	GrammarAnalyzer::~GrammarAnalyzer()
	{
		if(m_parsingTable) {
			for(unsigned int i = 0; i < m_states.size(); ++i) {
				delete [] m_parsingTable[i];
			}
			delete [] m_parsingTable;
		}
	}

	inline bool GrammarAnalyzer::_isnullable(Symbol *sym) {
		if(sym->isTerminal())
			return sym == NULL;
		if(m_nullable[sym->index()])
			return true;
		if(m_beingProcessed[sym->index()])
			return true;
		bool nullable = true;
		m_beingProcessed[sym->index()] = true;
		for(unsigned int i = 0; i < ((NonterminalSymbol*)sym)->proList().size(); ++i) {
			Production *pro = ((NonterminalSymbol*)sym)->proList()[i];
			for(unsigned int j = 0; j < pro->nrhs(); ++j) {
				if(!_isnullable(pro->rhs(j))) {
					nullable = false;
					break;
				}
			}
		}
		m_beingProcessed[sym->index()] = false;
		m_nullable[sym->index()] = nullable;
		return nullable;
	}

	inline void GrammarAnalyzer::_fillFirst(Symbol *sym)
	{
		if(sym->isTerminal()) {
			return;
		}
		if(m_beingProcessed[sym->index()])
			return;
		m_beingProcessed[sym->index()] = true;
		for(unsigned int i = 0; i < ((NonterminalSymbol*)sym)->proList().size(); ++i) {
			Production *pro = ((NonterminalSymbol*)sym)->proList()[i];
			for(unsigned int j = 0; j < pro->nrhs(); ++j) {
				Symbol *_rhs = pro->rhs(j);
				_fillFirst(_rhs);
				m_firstSets[sym->index()].mergeUnique(m_firstSets[_rhs->index()]);
				if(!m_nullable[_rhs->index()]) {
					break;
				}
			}
		}
		m_beingProcessed[sym->index()] = false;
	}

	void GrammarAnalyzer::computeNullable()
	{
		if(!m_grammar)
			return;

		m_nullable.resize(m_grammar->symbolList().count());
		m_beingProcessed.resize(m_grammar->symbolList().count());

		SymbolList &symList = m_grammar->symbolList();
		m_nullable.resize(symList.count());
		for(unsigned int i = 0; i < symList.count(); ++i) {
			m_nullable[i] = _isnullable(symList[i]);
		}
	}

	void GrammarAnalyzer::computeFirst()
	{
		if(!m_grammar)
			return;

		SymbolList &symList = m_grammar->symbolList();
		m_firstSets.resize(symList.count());
		m_nullable.resize(m_grammar->symbolList().count());

		computeNullable();
	
		for(unsigned int i = 0; i < m_grammar->m_termSymList.count(); ++i) {
			Symbol *sym = m_grammar->m_termSymList[i];
			m_firstSets[sym->index()].append(sym);
		}

		for(unsigned int i = 0; i < m_grammar->m_nontermSymList.count(); ++i) {
			_fillFirst(m_grammar->m_nontermSymList[i]);
		}

	#if 0
		//loop through all symbols until there are no changes
		u32 changes;

		do {
			changes = 0;

			u32 i;
			for(i = 0; i < symList.count(); ++i) {
				Symbol *sym = symList[i];
				//if sym is TERMINAL, FIRST(sym) = sym
				if(sym->symClass() == TERMINAL) {
					if(m_firstSets[sym->index()].appendUnique(sym))
						++changes;
				}
				else if(sym->symClass() == NONTERMINAL) {

					u32 j;
					for(j = 0; j < sym->proList().size(); ++j) {
						Production *pro = sym->proList()[j];
						u32 k;
						bool nullable = true;
						for(k = 0; k < pro->nrhs(); ++k) {
							if(m_firstSets[sym->index()].mergeUnique(m_firstSets[pro->rhs(k)->index()]) > 0)
								++changes;
							if(!m_nullable[pro->rhs(k)->index()]) {
								nullable = false;
								break;
							}
						}
						if(nullable) {
							m_nullable[sym->index()] = true;
	//						if(m_firstSets[sym->index()].appendUnique(&Grammar::epsilon))
						//		++changes;
						}
					}

				}

			}

		}while(changes > 0);
	#endif
	}

	void GrammarAnalyzer::computeFollow()
	{
		if(!m_grammar)
			return;

		//firstSet must be calculated
		if(m_firstSets.size() == 0)
			return;

		SymbolList &symList = m_grammar->symbolList();
		m_followSets.resize(symList.count());
		m_followSets[m_grammar->startSymbol()->index()].append(m_grammar->findSymbol("$eof"));

		int changes;
		do {
			changes = 0;

			unsigned int i;
			for(i = 0; i < m_grammar->m_nontermSymList.count(); ++i) {
				NonterminalSymbol *lhs = (NonterminalSymbol*)m_grammar->m_nontermSymList[i];

				unsigned int j;
				for(j = 0; j < lhs->proList().size(); ++j) {
					Production *pro = lhs->proList()[j];
					int k;
					if(pro->nrhs() > 0) {
						for(k = 0; k < (int)(pro->nrhs() - 1); ++k) {
							Symbol *rhs = pro->rhs(k);
							Symbol *next = pro->rhs(k + 1);
							if(!rhs->isNonTerminal())
								continue;

							if(m_followSets[rhs->index()].mergeUnique(m_firstSets[next->index()]) > 0)
								++changes;

							if(k+1 == (pro->nrhs()-1)) {
								if(m_nullable[next->index()]) {
									if(m_followSets[rhs->index()].mergeUnique(m_followSets[lhs->index()]) > 0)
										++changes;
								}
							}
						}

						if(pro->rhs(k)->isNonTerminal()) {
							if(m_followSets[pro->rhs(k)->index()].mergeUnique(m_followSets[lhs->index()]) > 0)
								++changes;
						}
					}
				}

			}

		}while(changes != 0);
	}

	void GrammarAnalyzer::_clean()
	{
		if(!m_grammar)
			return;
		if(!m_firstSets.empty()) m_firstSets.clear();
		if(!m_followSets.empty()) m_followSets.clear();
		if(!m_nullable.empty()) m_nullable.clear();

		if(!m_states.empty()) {
			if(m_parsingTable) {
				for(unsigned int i = 0; i < m_states.size(); ++i)
					delete [] m_parsingTable[i];
				delete[] m_parsingTable;
			}

			for(unsigned int i = 0; i < m_states.size(); ++i) {
				delete m_states[i];
			}
			m_states.clear();
		}
	}

	#ifdef _DEBUG
	#define DEBUG_PRINT(s,t) (s << t)
	#else
	#define DEBUG_PRINT(s,t)
	#endif

	void GrammarAnalyzer::lr0(std::ostream *stream)
	{
		Itemset itemset, newItemset;
		unsigned int i;

		if(!stream)
			stream = &std::cout;

		DEBUG_PRINT(*stream, "Generating states...\n");

		NonterminalSymbol *startSym = (NonterminalSymbol*)m_grammar->findSymbol("$start");
		if(!startSym)
			return;

		itemset.push_back(LRItem(*startSym->proList()[0],0));
		newItemset = itemset.closure();
		State *initialState = addState(NULL,newItemset);

	#ifdef _DEBUG
		dumpStateInfo(*initialState,*stream);
	#endif

		unsigned int st;
		for(st = 0; st < stateCount(); ++st) {
			Itemset &itemset = state(st)->itemset();

			DEBUG_PRINT(*stream, "Processing state: ");
			DEBUG_PRINT(*stream, st);

			//Save reductions
			for(unsigned int k = 0 ; k < itemset.size(); ++k) {
				LRItem &item = itemset[k];
				if(item.production()->nrhs() == item.markIndex()) {
					DEBUG_PRINT(*stream, "State: ");
					DEBUG_PRINT(*stream, st);
					DEBUG_PRINT(*stream, ", ");
					DEBUG_PRINT(*stream, "Saving reduction to rule");
					DEBUG_PRINT(*stream, item.production()->number());
					DEBUG_PRINT(*stream, std::endl);
					state(st)->addReduction(*item.production());
				}
			}

			DEBUG_PRINT(*stream, std::endl);

	#ifdef _DEBUG
			DEBUG_PRINT(*stream, "Creating itemsets from state: ");
			DEBUG_PRINT(*stream, st);
			DEBUG_PRINT(*stream, std::endl);
	//		getchar();
	#endif

			//
			//Make new item sets
			//

			//get the symbols that will be used in the GoTo operations (the symbols will be unique)
			std::set<Symbol*> uniqueSymbols;
			for(i = 0; i < itemset.size(); ++i) {
				LRItem &item = itemset[i];

				Symbol *sym = item.readyToSee();
				if(sym)
					uniqueSymbols.insert(sym);
			}

			//Do the GoTo operations (move the mark to the right) on all unique symbols in the itemset
			for(std::set<Symbol*>::iterator it = uniqueSymbols.begin(); it != uniqueSymbols.end(); ++it)
			{
				Symbol *sym = *it;
				newItemset = itemset.goTo(sym);
				if(newItemset.size() > 0) {
	#ifdef _DEBUG
				//check if state (itemset) already exists
					unsigned int n_states = stateCount();
					State *state = addState(sym, newItemset);
					this->state(st)->addTransition(sym, *state);
				//	printf("%s action: %s %d\n",sym->symClass() == TERMINAL ? "SHIFT" : "GOTO",sym->name(),state->number());
					if(stateCount() == n_states) {
						(*stream) << "State " << state->number() << " already created" << std::endl;
						continue;
					}

					dumpStateInfo(*state, *stream);
	#else
					state(st)->addTransition(sym, *addState(sym, newItemset));
	#endif
				}
			}

			DEBUG_PRINT(*stream, std::endl);
		}
	}

	State *GrammarAnalyzer::addState(Symbol *acessingSymbol, Itemset &itemset)
	{
		State *s = findState(itemset);
		if(!s) {
			s = new State(stateCount(),acessingSymbol,itemset);
			m_states.push_back(s);
		}
		return s;
	}

	State *GrammarAnalyzer::findState(Itemset &itemset)
	{
		State *state = NULL;
		unsigned int i,j;

		for(i = 0; i < m_states.size(); ++i) {
			State &tempState = *m_states[i];
			Itemset &tempItemset = tempState.itemset();
			if(tempItemset.size() != itemset.size()) {
				continue;
			}

			state = &tempState;
			for(j = 0; j < itemset.size(); ++j) {
				if(!tempState.contains(itemset[j])) {
					state = NULL;
					break;
				}
			}

			if(state)
				break;
		}

		return state;
	}

	void GrammarAnalyzer::lalr(std::ostream *stream)
	{
		u32 i, j, k;
		State *s;
		Itemset *itemset;

		s = state(0);
		itemset = &s->itemset();

		(*itemset)[0].appendLookAhead(m_grammar->findSymbol("$eof"));

		u32 changes;
		SymbolList firstSet;
		do {
			changes = 0;
			for(i = 0; i < stateCount(); ++i) {
				s = state(i);
				itemset = &s->itemset();

				//Propagate lookaheads to other items in this state
				for(j = 0; j < itemset->size(); ++j) {
					LRItem &item = (*itemset)[j];

					//check if item is kernel
		//			if(item.markIndex() > 0 || item.production() == m_initialProduction)
		//				continue;

					Production *pro = item.production();
					Symbol *sym = pro->rhs(item.markIndex());

					if(!sym || sym->isTerminal()) {
						continue;
					}

					firstSet.clear();

					bool nullable = true;
					Symbol *nextSym = pro->rhs(item.markIndex()+1);
					if(nextSym) {
						firstSet.mergeUnique(m_firstSets[nextSym->index()]);
						nullable = m_nullable[nextSym->index()];
					}
					if(nullable)
						firstSet.mergeUnique(item.lookAheadSet());

					//
					//copy lookahead to all productions starting with rhs sym in this itemset
					for(k = 0; k < itemset->size(); ++k) {
					//	if(k == j)
					//		continue;
						LRItem &item2 = (*itemset)[k];
						Production *pro2 = item2.production();
						Symbol *sym2 = pro2->lhs();
						if(sym != sym2)
							continue;
						changes += item2.lookAheadSet().mergeUnique(firstSet);
					}
				}


				//Propagate lookahead to other states
				//copy the same lookaheads to kernel itens starting with the shift symbol
				//same with gotos
				//shifts
				for(j = 0; j < s->shifts.size(); ++j) {
					Symbol *acessingSym = s->shifts[j].acessingSymbol;
					State *s2 = s->shifts[j].state;
					for(k = 0; k < itemset->size(); ++k) {
						LRItem &item = (*itemset)[k];
						Production *pro = item.production();
						Symbol *sym = pro->rhs(item.markIndex());
						if(!sym)
							continue;
						if(sym == acessingSym) {
							for(u32 l = 0; l < s2->itemset().size(); ++l) {
								LRItem& item2 = s2->itemset()[l];
								//check if item is kernel
								if(item2.markIndex() > 0 || item2.production() == m_grammar->initialProduction()) {
									//check for same core
									if(pro == item2.production()) {
										changes += item2.lookAheadSet().mergeUnique(item.lookAheadSet());
									}
								}
							}
						}
					}
				}

				//gotos
				for(j = 0; j < s->gotos.size(); ++j) {
					Symbol *acessingSym = s->gotos[j].acessingSymbol;
					State *s2 = s->gotos[j].state;
					for(k = 0; k < itemset->size(); ++k) {
						LRItem &item = (*itemset)[k];
						Production *pro = item.production();
						Symbol *sym = pro->rhs(item.markIndex());
						if(!sym)
							continue;
						if(sym == acessingSym) {
							for(u32 l = 0; l < s2->itemset().size(); ++l) {
								LRItem& item2 = s2->itemset()[l];
								//check if item is kernel
								if(item2.markIndex() > 0 || item2.production() == m_grammar->initialProduction()) {
									//check for same core
									if(pro == item2.production()) {
										changes += item2.lookAheadSet().mergeUnique(item.lookAheadSet());
									}
								}
							}
						}
					}
				}

		/*
				//Now set lookaheads to reductions
				for(j = 0; j < itemset->size(); ++j) {
					LRItem &item = (*itemset)[j];
					Production *pro = item.production();
					if(item.markIndex() == pro->nrhs()) {
						for(u32 k = 0; k < s->reductions.size(); ++k) {
							Symbol *redlhs = s->reductions[k].pro->lhs();
							if(redlhs && redlhs == pro->lhs())
								s->reductions[k].lookAhead.mergeUnique(item.lookAheadSet());
						}
					}
				}*/

			}

		}while(changes != 0);
	
		for(i = 0; i < stateCount(); ++i) {
			s = state(i);
			itemset = &s->itemset();
			//Now set lookaheads to reductions
			for(j = 0; j < itemset->size(); ++j) {
				LRItem &item = (*itemset)[j];
				Production *pro = item.production();
				if(item.markIndex() == pro->nrhs()) {
					for(u32 k = 0; k < s->reductions.size(); ++k) {
						Symbol *redlhs = s->reductions[k].pro->lhs();
						if(redlhs && redlhs == pro->lhs())
							s->reductions[k].lookAhead.mergeUnique(item.lookAheadSet());
					}
				}
			}
		}


		for(i = 0; i < stateCount(); ++i) {
			dumpStateInfo(*state(i), stream ? *stream : std::cout);
		}

	}


	//void GrammarAnalyzer::resolveShitReductConflict(State &state, )
	//{
	//}

	void GrammarAnalyzer::resolveConflicts()
	{
		u32 i, j, k, l;
		for(i = 0; i < m_states.size(); ++i) {
			State* s = m_states[i];
			uint nshifts = s->shifts.size();
			uint nreds = s->reductions.size();

			if(nreds > 1)
				int a = 1;

			if(nshifts > 0 && nreds > 0)
				int a = 2;

			//lalr shift-reduce conflict
			for(j = 0; j < nreds; ++j) {
				for(k = 0; k < s->reductions[j].lookAhead.count(); ++k) {
					Symbol *laSym = s->reductions[j].lookAhead[k];

					for(l = 0; l < nshifts; ++l) {
						Symbol *acSym = s->shifts[l].acessingSymbol;
						if(acSym == laSym) {
							int state = s->number();
							int conflict = 1;
						}
					}
				}
			}
		}
	}


	void GrammarAnalyzer::generateParsingTable()
	{
		unsigned int rows, cols;
		int **table;
		unsigned int i, j, k;

		rows = stateCount();
		cols = grammar()->symbolCount();
		table = new int*[rows];
		for(i = 0; i < rows; ++i) {
			table[i] = new int[cols];

			for(j = 0; j < cols; ++j)
				table[i][j] = ACTION_ERROR;

			State *state = this->state(i);

			for(j = 0; j < state->shifts.size(); ++j) {
				int symIndex = state->shifts[j].acessingSymbol->index();
				int targetStateIndex = state->shifts[j].state->number();
				table[i][symIndex] = MAKE_SHIFT_ACTION(targetStateIndex);
			}
			for(j = 0; j < state->gotos.size(); ++j) {
				int symIndex = state->gotos[j].acessingSymbol->index();
				int targetStateIndex = state->gotos[j].state->number();
				table[i][symIndex] = MAKE_SHIFT_ACTION(targetStateIndex);
			}
			for(j = 0; j < state->reductions.size(); ++j) {
				for(k = 0; k < state->reductions[j].lookAhead.count(); ++k) {
					int symIndex = state->reductions[j].lookAhead[k]->index();
					int targetRuleIndex = state->reductions[j].pro->number();

					if(table[i][symIndex] != 0)
						int conflict = 1;
					else {
						if(targetRuleIndex == grammar()->initialProduction()->number())
							table[i][symIndex] = MAKE_ACCEPT_ACTION(targetRuleIndex);
						else
							table[i][symIndex] = MAKE_REDUCE_ACTION(targetRuleIndex);
					}
				}
			}
		}

		m_parsingTable = table;
	}

	inline int center(int a, int b) {
		float _a = a;
		float _b = b;
		return (int)(fabs(a - b) / 2);
	}

	inline void printCenter(int colWidth, const char *str, std::ostream &stream) {
		int len = ::strlen(str);
		int marginLeft, marginRight;
		int diff = abs(colWidth - len);
		marginLeft = marginRight = diff/2;
		if((diff & 1) == 1)
			marginLeft += 1;
		while(marginLeft-- > 0)
			stream << " ";
		stream << std::setw(len+marginRight) << str;
	}

	void GrammarAnalyzer::dumpParsingTable(std::ostream &stream)
	{
		unsigned int rows;
		int *colCaptionWidth;
		int **table;
		unsigned int i, j;

		if(!m_parsingTable)
			return;
		table = m_parsingTable;

		rows = stateCount();
		colCaptionWidth = new int[m_grammar->symbolCount()];

		//rows is the number of states,
		//we must know the number of chars for the last state number
		int maxStateCharCount = 0;
		int n = rows;
		while(n > 0) {
			n /= 10;
			++maxStateCharCount;
		}

		const char *colCaptionState = "STATE";
		const char *colCaptionAction = "ACTION";
		const char *colCaptionGoto = "GOTO";
		const char *colSeparator = " | ";

		int len = ::strlen(colCaptionState);
		int firstColWidth =  len > maxStateCharCount ? len : maxStateCharCount;
		int actionWidth = 1 + maxStateCharCount;

		int sumActionCol = 0;
		int sumGoToCol = 0;
		for(j = 0; j < m_grammar->m_termSymList.count(); ++j) {
			const char *symName = m_grammar->m_termSymList[j]->name();
			int len = ::strlen(symName);
			int colWidth = len;
			if(colWidth < actionWidth) //action identifier + state number
				colWidth = actionWidth;
			colCaptionWidth[m_grammar->m_termSymList[j]->index()] = colWidth;
			sumActionCol += colWidth;
		}
		for(j = 0; j < m_grammar->m_nontermSymList.count(); ++j) {
			const char *symName = m_grammar->m_nontermSymList[j]->name();
			int len = ::strlen(symName);
			int colWidth = len;
			if(colWidth < actionWidth) //action identifier + state number
				colWidth = actionWidth;
			colCaptionWidth[m_grammar->m_nontermSymList[j]->index()] = colWidth;
			sumGoToCol += colWidth;
		}

		sumActionCol += (m_grammar->m_termSymList.count()-1)*::strlen(colSeparator);
		sumGoToCol += (m_grammar->m_nontermSymList.count()-1)*::strlen(colSeparator);

		printCenter(firstColWidth,colCaptionState,stream);
		stream << colSeparator;
		printCenter(sumActionCol,colCaptionAction,stream);
		stream << colSeparator;
		printCenter(sumGoToCol,colCaptionGoto,stream);
		stream << "\n";
		printCenter(firstColWidth,"",stream);
		for(j = 0; j < m_grammar->m_termSymList.count(); ++j) {
			Symbol *sym = m_grammar->m_termSymList[j];
			stream << colSeparator;
			printCenter(colCaptionWidth[sym->index()], sym->name(), stream);
		}
		for(j = 0; j < m_grammar->m_nontermSymList.count(); ++j) {
			Symbol *sym = m_grammar->m_nontermSymList[j];
			stream << colSeparator;
			printCenter(colCaptionWidth[sym->index()], sym->name(), stream);
		}

		stream << "\n";

		for(i = 0; i < rows; ++i) {
			std::ostringstream ostr;
			ostr << i;
			printCenter(firstColWidth, ostr.str().c_str(), stream);
			for(int j = 0; j < m_grammar->m_termSymList.count(); ++j) {
				int symIndex = m_grammar->m_termSymList[j]->index();
				int action = table[i][symIndex] >> 0x10;
				int value = table[i][symIndex] & 0xffff;
				std::ostringstream actionStr;
				stream << colSeparator;
				if(action == ACTION_SHIFT) {
					actionStr << 's' << value;
				}
				else if(action == ACTION_REDUCE) {
					actionStr << 'r' << value;
				}
				else if(action == ACTION_ACCEPT) {
					actionStr << "acc";
				}
				else {
					actionStr << "";
				}
				printCenter(colCaptionWidth[symIndex],actionStr.str().c_str(),stream);
			}

			//Non-terminals are always "go to"
			for(unsigned int j = 0; j < m_grammar->m_nontermSymList.count(); ++j) {
				int symIndex = m_grammar->m_nontermSymList[j]->index();
				int action = table[i][symIndex] >> 0x10;
				int value = table[i][symIndex] & 0xffff;
				stream << colSeparator;
				std::ostringstream ostr;
				ostr << value;
				if(action == ACTION_SHIFT)
					printCenter(colCaptionWidth[symIndex],ostr.str().c_str(),stream);
				else
					printCenter(colCaptionWidth[symIndex],"",stream);
			}
			stream << "\n";
		}

		delete[] colCaptionWidth;
	}

	void GrammarAnalyzer::setTokenizer(Lexer *lexer) {
		m_lexer = lexer;
	}

	u32 GrammarAnalyzer::addToken(const char *regExp, TerminalSymbol *symbol) {
		//todo: search for duplicate
		m_defaultLexer.addToken(regExp,symbol->index());
		return 1;
	}

	void GrammarAnalyzer::makeDefaultTerminalTokens() {
		uint termCount = m_grammar->terminalCount();
		SymbolList terminals = m_grammar->terminalSymbols();

		TerminalSymbol *eofSym = (TerminalSymbol*)m_grammar->findSymbol("$eof");
		for(int i = 0; i < termCount; ++i) {
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
				addToken(name.c_str(), (TerminalSymbol*)terminals[i]);
			}
		}
	}

	bool GrammarAnalyzer::parse(const char *input, std::ostream *stream)
	{
		if(m_lexer == NULL)
			m_lexer = &m_defaultLexer;
		m_lexer->setInput(input);
		m_lexer->setPos(0);
		return parse(stream);
	}

	bool GrammarAnalyzer::parse(unsigned int *input, std::ostream *stream)
	{
		if(!input)
			return false;

		return parse(stream);
	}

	bool GrammarAnalyzer::parse(std::ostream *stream)
	{
		State* curState;
		unsigned int i;
	//	unsigned int *input = m_parsingState.parsingInput;

		if(!m_parsingTable) {
			dumpParsingTable(*stream);
		}

		if(!stream)
			stream = &std::cout;

		//[Debug]
		DEBUG_PRINT(*stream, "Start parsing.\n");
		//[/Debug]

		m_parsingState.currentState = curState = state(0);
		if(!curState)
			return false;

		m_parsingState.stack.push_back(0);

		int tok;
		tok = getNextToken();
		for(;;) {
			char *text = m_curToken.text;

			DEBUG_PRINT(*stream, "Current State: ");
			DEBUG_PRINT(*stream, curState->number());
			DEBUG_PRINT(*stream, std::endl);
		
			DEBUG_PRINT(*stream, "Current Token: ");
			DEBUG_PRINT(*stream, (tok == -1 ? "Undefined" : m_grammar->symbolList()[tok]->name()));
			DEBUG_PRINT(*stream, " (\"");
			DEBUG_PRINT(*stream, text);
			DEBUG_PRINT(*stream, "\")");
			DEBUG_PRINT(*stream, std::endl);

			if(tok == -1) {
				//undefined token
				//todo: error
				tok = getNextToken();
				continue;
			}

			unsigned int action = 0;

	#if 1
			int **table = m_parsingTable;
			int value;
			action = GET_ACTION(table[curState->number()][tok]);
			value = GET_VALUE(table[curState->number()][tok]);
			if(action == ACTION_SHIFT) {

				//[DEBUG]
				DEBUG_PRINT(*stream, "Action: shift ");
				DEBUG_PRINT(*stream, value);
				DEBUG_PRINT(*stream, " (");
				DEBUG_PRINT(*stream, m_grammar->symbol(tok)->name());
				DEBUG_PRINT(*stream, ')');
				DEBUG_PRINT(*stream, std::endl);
				//[/DEBUG]

				m_parsingState.currentState = curState = state(value);
				m_parsingState.stack.push_back(value);
				m_parsingState.semanticStack.push_back(m_curToken);

 				tok = getNextToken();
			}
			else if(action == ACTION_REDUCE) {
				Production *pro = m_grammar->production(value);
				//[Debug]
				DEBUG_PRINT(*stream, "Action: reduce ");
				DEBUG_PRINT(*stream, value);
				DEBUG_PRINT(*stream, " (");
				DEBUG_PRINT(*stream, pro->lhs()->name());
				DEBUG_PRINT(*stream, " -> ");
	#ifdef _DEBUG
				for(u32 nsym = 0; nsym < pro->nrhs(); ++nsym) {
					DEBUG_PRINT(*stream, pro->rhs(nsym)->name());
					if(nsym < pro->nrhs()-1)
						DEBUG_PRINT(*stream, " ");
				}
	#endif
				DEBUG_PRINT(*stream, ')');
				DEBUG_PRINT(*stream, std::endl);
				//[/Debug]

				//do semantic action
				Token retToken;
				m_parsingState.semanticArgsIndex = m_parsingState.semanticStack.size()-pro->nrhs();
				retToken.value = this->sArg(0)->value; // defaults to first semantic arg
				memset(retToken.text,0,sizeof(retToken.text));

				if(pro->semanticAction()) {
					pro->semanticAction()(*this,retToken);
				}

				for(unsigned int j = 0; j < pro->nrhs(); ++j) {
					m_parsingState.stack.pop_back();
					m_parsingState.semanticStack.pop_back();
				}
				m_parsingState.currentState = curState = state(m_parsingState.stack.back());
				m_parsingState.semanticStack.push_back(retToken);

				//do the goto
				value = GET_VALUE(table[curState->number()][pro->lhs()->index()]);
				m_parsingState.currentState = curState = state(value);
				m_parsingState.stack.push_back(value);
			}
			else if(action == ACTION_ACCEPT) {
				//[DEBUG]
				DEBUG_PRINT(*stream, "Action: accept");
				DEBUG_PRINT(*stream, std::endl);
				//[/DEBUG]
				int accept = 1;
				return true;
			}
			else {
				int a = 1;
				//error
				break;
			}

	#endif
	#if 0
			//[Debug]
	//		i = m_parsingState.inputPos;
	//		(*stream) << "Input: ";
	//		for(;;) {
	//			unsigned int t = input[i++];
	//			(*stream) << t;
	//			if(t == 0)
	//				break;
	//			(*stream) << ", ";
	//		}
	//		(*stream) << std::endl;

	#ifdef _DEBUG
			(*stream) << "Stack: [";
			for(i = 0; i < m_parsingState.stack.size(); ++i) {
				(*stream) << m_parsingState.stack[i];
				if((i+1) != m_parsingState.stack.size())
					(*stream) << ", ";
			}
			(*stream) << "]" << std::endl;
	#endif
			//[/Debug]

			if(action == 0 && curState->shifts.size() > 0) {

				if(!preShiftProcessed && m_preShiftAction) {
					tok = m_preShiftAction(*this,tok,&m_curSemantic);
				}

				preShiftProcessed = true;

				for(i = 0; i < curState->shifts.size(); ++i) {
					Symbol *sym = curState->shifts[i].acessingSymbol;
					if(sym->index() == tok) {

						//[DEBUG]
						DEBUG_PRINT(*stream, "Action: shift ");
						DEBUG_PRINT(*stream, curState->shifts[i].state->number());
						DEBUG_PRINT(*stream, " (");
						DEBUG_PRINT(*stream, curState->shifts[i].acessingSymbol->name());
						DEBUG_PRINT(*stream, ')');
						DEBUG_PRINT(*stream, std::endl);
						//[/DEBUG]

						m_parsingState.currentState = curState = curState->shifts[i].state;
						m_parsingState.stack.push_back(curState->number());
	//					if(tok != 0)
	//						m_parsingState.inputPos++;
						action = 1;
						m_parsingState.semanticStack.push_back(m_curSemantic);

 						tok = getNextToken();
						preShiftProcessed = false;
						break;
					}
				}
			}

			if(action == 0 && curState->reductions.size() > 0) {
				for(i = 0; i < curState->reductions.size(); ++i) {
					if(curState->reductions[i].lookAhead.contains(m_grammar->symbol(tok))) {

						Production *pro = curState->reductions[i].pro;

						if(pro == m_grammar->findSymbol("$start")->proList()[0]) {
							//[DEBUG]
							DEBUG_PRINT(*stream, "Action: accept");
							DEBUG_PRINT(*stream, std::endl);
							//[/DEBUG]
							int accept = 1;
							return true;
						}

						//[Debug]
						DEBUG_PRINT(*stream, "Action: reduce ");
						DEBUG_PRINT(*stream, pro->number());
						DEBUG_PRINT(*stream, " (");
						DEBUG_PRINT(*stream, pro->lhs()->name());
						DEBUG_PRINT(*stream, " -> ");
	#ifdef _DEBUG
						for(u32 nsym = 0; nsym < pro->nrhs(); ++nsym) {
							DEBUG_PRINT(*stream, pro->rhs(nsym)->name());
							if(nsym < pro->nrhs()-1)
								DEBUG_PRINT(*stream, " ");
						}
	#endif
						DEBUG_PRINT(*stream, ')');
						DEBUG_PRINT(*stream, std::endl);
						//[/Debug]

						//do semantic action
						SemanticValue retSemantic;
						m_parsingState.semanticArgsIndex = m_parsingState.semanticStack.size()-pro->nrhs();
						retSemantic.value = this->sArg(0)->value; // defaults to first semantic arg
						memset(retSemantic.token,0,sizeof(retSemantic.token));

						if(pro->semanticAction()) {
							pro->semanticAction()(*this,retSemantic);
						}

						int test = m_parsingState.semanticStack.size();
						int test2 = pro->nrhs();
						int a = 1;

						for(unsigned int j = 0; j < pro->nrhs(); ++j) {
							m_parsingState.stack.pop_back();
							m_parsingState.semanticStack.pop_back();
						}
						m_parsingState.currentState = curState = state(m_parsingState.stack.back());
						m_parsingState.semanticStack.push_back(retSemantic);

						//do the goto
						for(i = 0; i < curState->gotos.size(); ++i) {
							Symbol *sym = curState->gotos[i].acessingSymbol;
							if(sym == pro->lhs()) {
								m_parsingState.currentState = curState = curState->gotos[i].state;
								m_parsingState.stack.push_back(curState->number());
								action = 2;
								break;
							}
						}

						if(action != 0)
							break;
					}
				}
			}

			if(action == 0) {
				DEBUG_PRINT(*stream, "Action: error");
				DEBUG_PRINT(*stream, " ");
				DEBUG_PRINT(*stream, "(");
				DEBUG_PRINT(*stream, (tok == -1 ? "Undefined" : m_grammar->symbolList()[tok]->name()));
				DEBUG_PRINT(*stream, ")\n");
				break;
			}
	#endif
			DEBUG_PRINT(*stream, std::endl);
		}
		return false;
	}

	int GrammarAnalyzer::getNextToken()
	{
		//reset tok info
		m_curToken.tok = -1;
		memset(m_curToken.text,0,sizeof(m_curToken.text));
 		return m_lexer->nextToken(&m_curToken);
	}









	void GrammarAnalyzer::dumpGrammar(std::ostream &stream)
	{
		stream << "Dumping Grammar:" << std::endl;
		dumpSymbols(stream);
		stream << std::endl;
		dumpPros(stream);
	}

	void GrammarAnalyzer::dumpSymbols(std::ostream &stream)
	{
		u32 i;

		stream << "Grammar Symbols:" << std::endl;
		Grammar *grammar = m_grammar;
		SymbolList &symList = grammar->symbolList();
		for(i = 0; i < symList.count(); ++i)
		{
			Symbol *sym = symList[i];
			stream << sym->index() << ". " << sym->name() << " - " << (sym->symClass() == TERMINAL ? "Terminal" : "Nonterminal") << std::endl;
		}
	}

	void GrammarAnalyzer::dumpPros(std::ostream &stream)
	{
		Grammar *grammar = m_grammar;

		stream << "Grammar Productions rules:" << std::endl;

		ProductionList &proList = grammar->productionList();
		for(ProductionList::iterator it = proList.begin(); it != proList.end();	++it)
		{
			Production &pro = **it;
			stream << pro.number() << ". " << pro.lhs()->name() << " -> ";
			for(unsigned int j = 0; j < pro.nrhs(); ++j)
				stream << pro.rhs(j)->name() << ' ';
			stream << std::endl;
		}
	}

	void GrammarAnalyzer::dumpStates(std::ostream &stream)
	{
		u32 i;
		stream << "State list:" << std::endl;
		for(i = 0; i < stateCount(); ++i) {
			State *state = this->state(i);
			dumpStateInfo(*state,stream);
		}
	}

	void GrammarAnalyzer::dumpStateInfo(State &state, std::ostream &stream)
	{
		u32 i, j;
		Grammar *grammar = m_grammar;

		stream << "State " << state.number() << ':' << std::endl;
		Itemset &itemset = state.itemset();
		for(i = 0; i < itemset.size(); ++i) {
			LRItem &item = itemset[i];
			Production *pro = item.production();
			Symbol *sym = pro->rhs(item.markIndex());

			stream.width(2);
			stream << std::left;
			stream << (pro == grammar->initialProduction() || (item.markIndex() != 0)?"K":"NK");
			stream.width(0);
			stream << " - ";
			stream << '[' << pro->lhs()->name() << " -> ";
			for(j = 0; j < pro->nrhs(); ++j) {
				if(j == item.markIndex())
					stream << '.';
				stream << pro->rhs(j)->name();
				if(j < pro->nrhs()-1)
					stream << ' ';
			}

			if(j == item.markIndex())
				stream << ".";

			if(item.lookAheadSet().count() > 0) {
				stream << ", ";
				for(u32 j = 0; j < item.lookAheadSet().count(); ++j) {
					stream << item.lookAheadSet()[j]->name();
					if(j < item.lookAheadSet().count()-1)
						stream << '/';
				}
			}
			stream << ']';
			stream << std::endl;
		}
	}

	void GrammarAnalyzer::dumpFirstFollowNullable(std::ostream &stream)
	{
		Grammar *grammar = m_grammar;
		SymbolList& symList = grammar->symbolList();

		u32 i;

		stream << "Nullable sets:" << std::endl;
		if(m_nullable.size() > 0) {
			for(i = 0; i < symList.count(); ++i) {
				if(symList[i]->symClass() != NONTERMINAL)
					continue;
				stream << "nullable" << '(' << symList[i]->name() << ')' << " = ";
				stream << (isNullable(symList[i]->index()) ? "true" : "false") << std::endl;
			}
		}

		stream << std::endl;

		stream << "FIRST sets:" << std::endl;
		if(m_firstSets.size() > 0) {
			for(i = 0; i < symList.count(); ++i) {
				stream << "FIRST" << '(' << symList[i]->name() << ')' << " = {";
				for(u32 j = 0; j < first(i).count(); ++j) {
					stream << first(i)[j]->name();
					if(j < first(i).count()-1)
						stream << ",";
				}
				stream << "}" << std::endl;
			}
		}

		stream << std::endl;

		stream << "FOLLOW sets:" << std::endl;
		if(m_followSets.size() > 0) {
			for(i = 0; i < symList.count(); ++i) {
				if(symList[i]->symClass() != NONTERMINAL)
					continue;
				stream << "FOLLOW" << '(' << symList[i]->name() << ')' << " = {";
				for(u32 j = 0; j < follow(i).count(); ++j) {
					stream << follow(i)[j]->name();
					if(j < follow(i).count() - 1)
						stream << ",";
				}
				stream << "}" << std::endl;
			}
		}

		stream << std::endl;
	}

}