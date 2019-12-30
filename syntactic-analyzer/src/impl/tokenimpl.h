
//Last edit: 29/12/2019 13:18

#ifndef __SYNTACTIC_ANALYZER_TOKEN_IMPL_H__
#define __SYNTACTIC_ANALYZER_TOKEN_IMPL_H__

#include "../Token.h"

namespace syntacticanalyzer {

	class TokenImpl : public Token {
		int m_id;
		char m_text[256];

		union {
			char cval;
			short sval;
			long lval;
			int ival;
			char* strval;
			unsigned char ucval;
			unsigned short usval;
			unsigned long ulval;
			unsigned int uival;
			bool bval;
			float fval;
			double dval;
			void* pval;
		} m_value;

	public:
		TokenImpl()
			: m_id(-1) {
			m_text[0] = '\0';
		}

		virtual char asChar() {
			return m_value.cval;
		}

		virtual short asShort() {
			return m_value.sval;
		}

		virtual long asLong() {
			return m_value.lval;
		}

		virtual int asInt() {
			return m_value.ival;
		}

		virtual const char* asString() {
			return m_value.strval;
		}

		virtual unsigned char asUChar() {
			return m_value.ucval;
		}

		virtual unsigned short asUShort() {
			return m_value.usval;
		}

		virtual unsigned long asULong() {
			return m_value.ulval;
		}

		virtual unsigned int asUInt() {
			return m_value.uival;
		}

		virtual bool asBool() {
			return m_value.bval;
		}

		virtual float asFloat() {
			return m_value.fval;
		}

		virtual double asDouble() {
			return m_value.dval;
		}
	};

}

#endif