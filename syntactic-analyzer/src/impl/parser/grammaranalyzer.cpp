
//Last edit: 02/01/2020 18:16

#include <iostream>
#include <iomanip>
#include <sstream>
#include <set>
#include <cstdlib> //abs

#include "grammaranalyzer.h"

namespace syntacticanalyzer {

	static bool symListContains(std::vector<Symbol*>& symList, Symbol *sym) {
		for (unsigned int i = 0; i < symList.size(); ++i) {
			if (symList[i] == sym)
				return true;
		}
		return false;
	}

	static int symListMerge(std::vector<Symbol*>& symList1, std::vector<Symbol*>& symList2) {
		unsigned int i;
		unsigned int appendCount = 0;
		for (i = 0; i < symList2.size(); ++i) {
			if (!symListContains(symList1,symList2[i])) {
				symList1.push_back(symList2[i]);
				++appendCount;
			}
		}

		return appendCount;
	}

	//
	// Grammar Analyzer
	//

	GrammarAnalyzer::GrammarAnalyzer(Grammar &grammar)
	{
		m_grammar = &grammar;
		m_symbols = grammar.symbols();
		m_terminals = grammar.terminals();
		m_nonterminals = grammar.nonterminals();
		m_productions = grammar.productions();
		m_symbolCount = grammar.symbolCount();
		m_terminalCount = grammar.terminalCount();
		m_nonterminalCount = grammar.nonterminalCount();
		m_productionCount = grammar.productionCount();

		m_parsingTable = NULL;
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
			return false; //todo: check if sym is epsilon (the empty string)
		if(m_nullable[sym->index()])
			return true;
		if(m_beingProcessed[sym->index()])
			return m_nullable[sym->index()];
		bool nullable = true;
		m_beingProcessed[sym->index()] = true;
		for(unsigned int i = 0; i < ((NonterminalSymbol*)sym)->associatedProductionCount(); ++i) {
			Production *pro = ((NonterminalSymbol*)sym)->associatedProductions()[i];
			for(unsigned int j = 0; j < pro->rhsCount(); ++j) {
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
		for(unsigned int i = 0; i < ((NonterminalSymbol*)sym)->associatedProductionCount(); ++i) {
			Production *pro = ((NonterminalSymbol*)sym)->associatedProductions()[i];
			for(unsigned int j = 0; j < pro->rhsCount(); ++j) {
				Symbol *_rhs = pro->rhs(j);
				_fillFirst(_rhs);
				symListMerge(m_firstSets[sym->index()], m_firstSets[_rhs->index()]);
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

		m_nullable.resize(m_grammar->symbolCount());
		m_beingProcessed.resize(m_grammar->symbolCount());

		Symbol** symbols = m_grammar->symbols();
		m_nullable.resize(m_grammar->symbolCount());
		for(unsigned int i = 0; i < m_grammar->symbolCount(); ++i) {
			m_nullable[i] = _isnullable(symbols[i]);
		}
	}

	void GrammarAnalyzer::computeFirst()
	{
		if(!m_grammar)
			return;

		m_firstSets.resize(m_grammar->symbolCount());
		m_nullable.resize(m_grammar->symbolCount());

		computeNullable();
	
		//if sym is TERMINAL, FIRST(sym) = sym
		for(unsigned int i = 0; i < m_grammar->terminalCount(); ++i) {
			Symbol *sym = m_terminals[i];
			m_firstSets[sym->index()].push_back(sym);
		}

		for(unsigned int i = 0; i < m_grammar->nonterminalCount(); ++i) {
			_fillFirst(m_nonterminals[i]);
		}

	#if 0
		//loop through all symbols until there are no changes
		u32 changes;

		do {
			changes = 0;

			u32 i;
			for(i = 0; i < m_grammar->nonTerminalCount(); ++i) {
				NonterminalSymbol *sym = (NonterminalSymbol*)m_grammar->m_nontermSymList[i];
				
				u32 j;
				for(j = 0; j < sym->proList().size(); ++j) {
					Production *pro = sym->proList()[j];
					bool nullable = true;
					for(unsigned int k = 0; k < pro->nrhs(); ++k) {
						if(m_firstSets[sym->index()].merge(m_firstSets[pro->rhs(k)->index()]) > 0)
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

		Symbol** symbols = m_grammar->symbols();
		m_followSets.resize(m_grammar->symbolCount());
		m_followSets[m_grammar->startSymbol()->index()].push_back(m_grammar->symbol("$eof"));

		int changes;
		do {
			changes = 0;

			unsigned int i;
			for(i = 0; i < m_grammar->nonterminalCount(); ++i) {
				NonterminalSymbol *lhs = (NonterminalSymbol*)m_nonterminals[i];

				unsigned int j;
				for(j = 0; j < lhs->associatedProductionCount(); ++j) {
					Production *pro = lhs->associatedProductions()[j];
					int k;
					if(pro->rhsCount() > 0) {
						for(k = 0; k < (int)(pro->rhsCount() - 1); ++k) {
							Symbol *rhs = pro->rhs(k);
							Symbol *next = pro->rhs(k + 1);
							if(!rhs->isNonterminal())
								continue;

							if(symListMerge(m_followSets[rhs->index()],m_firstSets[next->index()]) > 0)
								++changes;

							if(k+1 == (pro->rhsCount()-1)) {
								if(m_nullable[next->index()]) {
									if(symListMerge(m_followSets[rhs->index()],m_followSets[lhs->index()]) > 0)
										++changes;
								}
							}
						}

						if(pro->rhs(k)->isNonterminal()) {
							if(symListMerge(m_followSets[pro->rhs(k)->index()],m_followSets[lhs->index()]) > 0)
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

		NonterminalSymbol *startSym = (NonterminalSymbol*)m_grammar->symbol("$start");
		if(!startSym)
			return; 

		itemset.add(LRItem(*startSym->associatedProductions()[0],0));
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
				if(item.production()->rhsCount() == item.markIndex()) {
					DEBUG_PRINT(*stream, "State: ");
					DEBUG_PRINT(*stream, st);
					DEBUG_PRINT(*stream, ", ");
					DEBUG_PRINT(*stream, "Saving reduction to rule");
					DEBUG_PRINT(*stream, item.production()->number());
					DEBUG_PRINT(*stream, std::endl);
					state(st)->addReduceAction(*item.production());
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
					this->state(st)->addShiftAction(sym, *state);
				//	printf("%s action: %s %d\n",sym->symClass() == TERMINAL ? "SHIFT" : "GOTO",sym->name(),state->number());
					if(stateCount() == n_states) {
						(*stream) << "State " << state->number() << " already created" << std::endl;
						continue;
					}

					dumpStateInfo(*state, *stream);
	#else
					state(st)->addShiftAction(sym, *addState(sym, newItemset));
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
		unsigned int i, j, k;
		State *s;
		Itemset *itemset;

		s = state(0);
		itemset = &s->itemset();

		(*itemset)[0].appendLookAhead(m_grammar->symbol("$eof"));

		int changes;
		std::vector<Symbol*> firstSet;
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
						symListMerge(firstSet, m_firstSets[nextSym->index()]);
						nullable = m_nullable[nextSym->index()];
					}
					if(nullable)
						symListMerge(firstSet,item.lookAheadSet());

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
						changes += symListMerge(item2.lookAheadSet(),firstSet);
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
							for(unsigned int l = 0; l < s2->itemset().size(); ++l) {
								LRItem& item2 = s2->itemset()[l];
								//check if item is kernel
								if(item2.markIndex() > 0 || item2.production() == m_grammar->initialProduction()) {
									//check for same core
									if(pro == item2.production()) {
										changes += symListMerge(item2.lookAheadSet(),item.lookAheadSet());
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
							for(unsigned int l = 0; l < s2->itemset().size(); ++l) {
								LRItem& item2 = s2->itemset()[l];
								//check if item is kernel
								if(item2.markIndex() > 0 || item2.production() == m_grammar->initialProduction()) {
									//check for same core
									if(pro == item2.production()) {
										changes += symListMerge(item2.lookAheadSet(), item.lookAheadSet());
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
				if(item.markIndex() == pro->rhsCount()) {
					for(unsigned int k = 0; k < s->reductions.size(); ++k) {
						Symbol *redlhs = s->reductions[k].pro->lhs();
						if (redlhs && redlhs == pro->lhs())
							symListMerge(s->reductions[k].lookAhead, item.lookAheadSet());
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
		unsigned int i, j, k, l;
		for(i = 0; i < m_states.size(); ++i) {
			State* s = m_states[i];
			unsigned int nshifts = s->shifts.size();
			unsigned int nreds = s->reductions.size();

			if(nreds > 1)
				int a = 1;

			if(nshifts > 0 && nreds > 0)
				int a = 2;

			//lalr shift-reduce conflict
			for(j = 0; j < nreds; ++j) {
				for(k = 0; k < s->reductions[j].lookAhead.size(); ++k) {
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
				for(k = 0; k < state->reductions[j].lookAhead.size(); ++k) {
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
		return (int)fabs((a - (float)b)/2);
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
		for(j = 0; j < m_grammar->terminalCount(); ++j) {
			const char *symName = m_terminals[j]->name();
			int len = ::strlen(symName);
			int colWidth = len;
			if(colWidth < actionWidth) //action identifier + state number
				colWidth = actionWidth;
			colCaptionWidth[m_terminals[j]->index()] = colWidth;
			sumActionCol += colWidth;
		}
		for(j = 0; j < m_grammar->nonterminalCount(); ++j) {
			const char *symName = m_nonterminals[j]->name();
			int len = ::strlen(symName);
			int colWidth = len;
			if(colWidth < actionWidth) //action identifier + state number
				colWidth = actionWidth;
			colCaptionWidth[m_nonterminals[j]->index()] = colWidth;
			sumGoToCol += colWidth;
		}

		sumActionCol += (m_grammar->terminalCount()-1)*::strlen(colSeparator);
		sumGoToCol += (m_grammar->nonterminalCount()-1)*::strlen(colSeparator);

		printCenter(firstColWidth,colCaptionState,stream);
		stream << colSeparator;
		printCenter(sumActionCol,colCaptionAction,stream);
		stream << colSeparator;
		printCenter(sumGoToCol,colCaptionGoto,stream);
		stream << "\n";
		printCenter(firstColWidth,"",stream);
		for(j = 0; j < m_grammar->terminalCount(); ++j) {
			Symbol *sym = m_terminals[j];
			stream << colSeparator;
			printCenter(colCaptionWidth[sym->index()], sym->name(), stream);
		}
		for(j = 0; j < m_grammar->nonterminalCount(); ++j) {
			Symbol *sym = m_nonterminals[j];
			stream << colSeparator;
			printCenter(colCaptionWidth[sym->index()], sym->name(), stream);
		}

		stream << "\n";

		for(i = 0; i < rows; ++i) {
			std::ostringstream ostr;
			ostr << i;
			printCenter(firstColWidth, ostr.str().c_str(), stream);
			for(j = 0; j < m_grammar->terminalCount(); ++j) {
				int symIndex = m_terminals[j]->index();
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
			for(unsigned int j = 0; j < m_grammar->nonterminalCount(); ++j) {
				int symIndex = m_nonterminals[j]->index();
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


	void GrammarAnalyzer::dumpGrammar(std::ostream &stream)
	{
		stream << "Dumping Grammar:" << std::endl;
		dumpSymbols(stream);
		stream << std::endl;
		dumpPros(stream);
	}

	void GrammarAnalyzer::dumpSymbols(std::ostream &stream)
	{
		stream << "Grammar Symbols:" << std::endl;
		Grammar *grammar = m_grammar;
		for(unsigned int i = 0; i < grammar->symbolCount(); ++i)
		{
			Symbol* sym = grammar->symbol(i);
			stream << sym->index() << ". " << sym->name() << " - " << (sym->isTerminal() ? "Terminal" : "Nonterminal") << std::endl;
		}
	}

	void GrammarAnalyzer::dumpPros(std::ostream &stream)
	{
		stream << "Grammar Productions rules:" << std::endl;

		Production** pros = m_productions;
		for(unsigned int i = 0; i < m_productionCount; ++i)
		{
			stream << pros[i]->number() << ". " << pros[i]->lhs()->name() << " -> ";
			for(unsigned int j = 0; j < pros[i]->rhsCount(); ++j)
				stream << pros[i]->rhs(j)->name() << ' ';
			stream << std::endl;
		}
	}

	void GrammarAnalyzer::dumpStates(std::ostream &stream)
	{
		stream << "State list:" << std::endl;
		for(unsigned int i = 0; i < stateCount(); ++i) {
			State *state = this->state(i);
			dumpStateInfo(*state,stream);
		}
	}

	void GrammarAnalyzer::dumpStateInfo(State &state, std::ostream &stream)
	{
		unsigned int i, j;
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
			for(j = 0; j < pro->rhsCount(); ++j) {
				if(j == item.markIndex())
					stream << '.';
				stream << pro->rhs(j)->name();
				if(j < pro->rhsCount()-1)
					stream << ' ';
			}

			if(j == item.markIndex())
				stream << ".";

			if(item.lookAheadSet().size() > 0) {
				stream << ", ";
				for(unsigned int j = 0; j < item.lookAheadSet().size(); ++j) {
					stream << item.lookAheadSet()[j]->name();
					if(j < item.lookAheadSet().size()-1)
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

		unsigned int symbolCount = grammar->symbolCount();

		unsigned int i;

		stream << "Nullable sets:" << std::endl;
		if(m_nullable.size() > 0) {
			for(i = 0; i < symbolCount; ++i) {
				Symbol* sym = grammar->symbol(i);
				if (sym->isTerminal())
					continue;
				stream << "nullable" << '(' << sym->name() << ')' << " = ";
				stream << (isNullable(sym->index()) ? "true" : "false") << std::endl;
			}
		}

		stream << std::endl;

		stream << "FIRST sets:" << std::endl;
		if(m_firstSets.size() > 0) {
			for(i = 0; i < symbolCount; ++i) {
				Symbol* sym = grammar->symbol(i);
				stream << "FIRST" << '(' << sym->name() << ')' << " = {";
				for(unsigned int j = 0; j < first(i).size(); ++j) {
					stream << first(i)[j]->name();
					if(j < first(i).size()-1)
						stream << ",";
				}
				stream << "}" << std::endl;
			}
		}

		stream << std::endl;

		stream << "FOLLOW sets:" << std::endl;
		if(m_followSets.size() > 0) {
			for(i = 0; i < symbolCount; ++i) {
				Symbol* sym = grammar->symbol(i);
				if (sym->isTerminal())
					continue;
				stream << "FOLLOW" << '(' << sym->name() << ')' << " = {";
				for(unsigned int j = 0; j < follow(i).size(); ++j) {
					stream << follow(i)[j]->name();
					if(j < follow(i).size() - 1)
						stream << ",";
				}
				stream << "}" << std::endl;
			}
		}

		stream << std::endl;
	}

}