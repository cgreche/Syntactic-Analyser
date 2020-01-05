//Last Edit: 02/01/2020 21:52
#include "state.h"

namespace syntacticanalyzer {

	Itemset Itemset::closure()
	{
		Itemset &itemset = *this;
		Itemset newItemset = itemset;

		unsigned int i, j;
		for(i = 0; i < newItemset.size(); ++i) {
			LRItem &item = newItemset[i];
			Symbol *sym = item.production()->rhs(item.markIndex());

			if(sym && sym->isNonterminal()) {
				NonterminalSymbol *lhs = (NonterminalSymbol*)sym;
				Production** pros = lhs->associatedProductions();
				unsigned int proCount = lhs->associatedProductionCount();
				for (unsigned int i = 0; i < proCount; ++i) {
					bool isAdded = false;

					for(j = 0; j < newItemset.size(); ++j) {
						if(newItemset[j].production() == pros[i] && newItemset[j].markIndex() == 0) {
							isAdded = true;
							break;
						}
					}

					if(!isAdded) {
						newItemset.add(LRItem(*pros[i], 0));
					}
				}
			}
		}

		return newItemset;
	}

	Itemset Itemset::goTo(Symbol *sym)
	{
		Itemset &itemset = *this;
		Itemset newItemset;

		unsigned int i;
		for(i = 0; i < itemset.size(); ++i) {
			LRItem& item = itemset[i];
			Symbol *_sym = item.production()->rhs(item.markIndex());
			if(sym == _sym) {
				newItemset.add(LRItem(*item.production(), item.markIndex() + 1));
			}
		}

		return newItemset.closure();
	}

	State::State(unsigned int number, Symbol *acessingSymbol, Itemset &itemset)
		: m_number(number)
	{
		m_acessingSymbol = acessingSymbol;
		m_items = itemset;
	}

	bool State::contains(LRItem &lrItem)
	{
		for(unsigned int i = 0; i < m_items.size(); ++i) {
			if(m_items[i].production() == lrItem.production()
				&& m_items[i].markIndex() == lrItem.markIndex())
				return true;
		}
		return false;
	}

	void State::addShiftAction(Symbol *acessingSymbol, State &state)
	{
		if(acessingSymbol->isTerminal())
			shifts.push_back(ShiftAction(acessingSymbol, state));
		else
			gotos.push_back(ShiftAction(acessingSymbol, state));
	}

	void State::addReduceAction(Production &pro)
	{
		reductions.push_back(ReductionAction(pro));
	}

}