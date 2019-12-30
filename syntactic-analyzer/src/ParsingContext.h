#pragma once

//Created in: 29/12/2019 03:19
//Last edit: 29/12/2019 03:19

namespace syntacticanalyzer {

	class LanguageParser;
	class Token;

	class ParsingContext
	{
		virtual LanguageParser* parser() = 0;
		virtual Token* arg(int n) = 0;
		virtual unsigned int argCount(int n) = 0;
	};

}