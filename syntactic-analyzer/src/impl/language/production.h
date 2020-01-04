
//Last edit: 03/01/2020 18:33

#ifndef __SYNTACTIC_ANALYZER_PRODUCTION_IMPL_H__
#define __SYNTACTIC_ANALYZER_PRODUCTION_IMPL_H__

#include <vector>
#include "symbol.h"
#include "../../Production.h"

namespace syntacticanalyzer {

	class ParsingState;

	class SemanticValueImpl : public SemanticValue {
		Token* m_token;
		SemanticValue::Value m_value;

	public:
		SemanticValueImpl(Token* token) {
			m_token = token;
			m_value.pval = NULL;
		}

		SemanticValueImpl() {
			m_token = NULL;
			m_value.pval = NULL;
		}

		virtual Token* token() const {
			return m_token;
		}

		virtual SemanticValue::Value value() const {
			return m_value;
		}

		virtual void setValue(SemanticValue::Value value) {
			m_value = value;
		}
	};

	class ProductionImpl : public Production
	{
		unsigned int m_number;
		NonterminalSymbol *m_lhs; //head
		std::vector<Symbol*> m_rhs; //body

		//
		SemanticAction m_semanticAction;

		//only Grammar can create Production
		ProductionImpl(NonterminalSymbol* lhs, std::vector<Symbol*> rhs, SemanticAction action);
		~ProductionImpl();

	public:
		friend class GrammarImpl;
		friend class GrammarBuilderImpl;

		virtual NonterminalSymbol *lhs() const { return m_lhs; }
		virtual Symbol *rhs(unsigned int n) const { if(n >= m_rhs.size()) return NULL; return m_rhs[n]; }
		virtual unsigned int rhsCount() const { return m_rhs.size(); }
		virtual unsigned int number() const { return m_number; }

		virtual SemanticAction semanticAction() const { return m_semanticAction; }
	};

}

#endif