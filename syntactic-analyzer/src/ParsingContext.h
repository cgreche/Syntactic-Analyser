#pragma once

//Created in: 29/12/2019 03:19
//Last edit: 04/01/2020 13:01

namespace syntacticanalyzer {

	class LanguageParser;
	class SemanticValue;

	class ParsingContext
	{
		virtual LanguageParser* parser() = 0;
		virtual SemanticValue* arg(int n) = 0;
		virtual unsigned int argCount() = 0;
	};

}