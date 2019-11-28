
//Last Edit: 17/02/2018

#ifndef __SYNTACTIC_ANALYZER_STATE_H__
#define __SYNTACTIC_ANALYZER_STATE_H__

#include <vector>

#include "grammar.h"

namespace syntacticanalyzer {

	class State;

	class LRItem
	{
		Production *m_production;
		unsigned int m_markIndex;

		SymbolList m_lookAhead;

	public:
		LRItem(Production &pro, unsigned int markIndex)
			: m_production(&pro), m_markIndex(markIndex) {
		}

		Production* production() const { return m_production; }
		unsigned int markIndex() const { return m_markIndex; }

		Symbol *readyToSee() const { return m_production->rhs(m_markIndex); }
		void appendLookAhead(Symbol *sym) { m_lookAhead.append(sym); }
		SymbolList& lookAheadSet() { return m_lookAhead; }
	};

	class Itemset : public std::vector<LRItem>
	{
	public:
		Itemset closure();
		Itemset goTo(Symbol *sym);
	};


	struct ShiftAction
	{
		Symbol *acessingSymbol;
		State *state;

		ShiftAction(Symbol *acessingSymbol, State &state)
			: acessingSymbol(acessingSymbol), state(&state)
		{
		}
	};

	struct ReductionAction
	{
		Production *pro;
		SymbolList lookAhead;

		ReductionAction(Production& pro)
			: pro(&pro)
		{
		}
	};


	class State
	{
		unsigned int m_number;
		Symbol *m_acessingSymbol;
		Itemset m_items;

		std::vector<ShiftAction> shifts;
		std::vector<ReductionAction> reductions;
		std::vector<ShiftAction> gotos;
		void addShiftAction(Symbol *acessingSymbol, State& state);
		void addReduceAction(Production &pro);

		State(unsigned int number, Symbol *acessingSymbol, Itemset &itemset);

	public:
		friend class GrammarAnalyzer;

		unsigned int number() const { return m_number; }
		Itemset& itemset() { return m_items; }
		bool contains(LRItem &lrItem);
	};

}

#endif