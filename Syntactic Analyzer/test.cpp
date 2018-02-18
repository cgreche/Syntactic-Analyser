
//Last edit: 26/06/2017 02:52

#include <windows.h>
#pragma comment(lib,"Winmm.lib") //for timeGetTime
#include <Mmsystem.h> //for timeGetTime

#include <iostream>
#include <string>
#include <fstream>
#include <limits>
#include <cerrno>
#include <vector>

#include "file.h"
#include "src/grammaranalyzer.h"
#include "src/languageparser.h"

#undef min
#undef max

using namespace syntacticanalyzer;

struct GrammarTest
{
	const char *grammarName;
	Grammar* (*grammarCreate)();
	void (*grammarDestroy)(Grammar *grammar);
	bool (*preParsing)(Grammar *grammar);
	void (*postParsing)(Grammar *grammar, bool result);
};

Grammar *testGrammar_create()
{
	Grammar *g = new Grammar;
	Grammar &grammar = *g;

#define T(x) grammar.addTerminal(x)
#define NT(x) grammar.addNonTerminal(x)
#define P grammar.addProduction
	Symbol *a = grammar.addTerminal("a");
	Symbol *b = grammar.addTerminal("b");
	Symbol *c = grammar.addTerminal("c");
	Symbol *d = grammar.addTerminal("d");
	Symbol *e = grammar.addTerminal("e");

	P(NT("S"),3,a,NT("B"),c);
	P(NT("S"),3,b,NT("C"),c);
	P(NT("S"),3,a,NT("C"),d);
	P(NT("S"),3,b,NT("B"),d);

	P(NT("B"),1,e);
	P(NT("C"),1,e);

	grammar.setStartSymbol((NonterminalSymbol*)grammar.findSymbol("S"));

#undef T
#undef NT
#undef P

	return g;
}

void testGrammar_destroy(Grammar *grammar)
{
	if(grammar)
		delete grammar;
}

GrammarTest testGrammar = {
	"Test Grammar",
	testGrammar_create,
	testGrammar_destroy,
	NULL,
};


Grammar *mathExpr_create()
{
	Grammar *g = new Grammar;
	Grammar &grammar = *g;

#define T(x) grammar.addTerminal(x)
#define NT(x) grammar.addNonTerminal(x)
#define P grammar.addProduction
	Symbol *symPlus = grammar.addTerminal("+");
	Symbol *symMinus = grammar.addTerminal("-");
	Symbol *symTimes = grammar.addTerminal("*");
	Symbol *symDivide = grammar.addTerminal("/");
	Symbol *symDigit = grammar.addTerminal("CONSTANT");

	P(NT("expr"),1,NT("term"));
	P(NT("term"),3,NT("term"),symPlus,NT("factor"));
	P(NT("term"),3,NT("term"),symMinus,NT("factor"));
	P(NT("term"),1,NT("factor"));
	P(NT("term"),1,NT("primary"));

	P(NT("factor"),3,NT("factor"),symTimes,NT("primary"));
	P(NT("factor"),3,NT("factor"),symDivide,NT("primary"));
	P(NT("factor"),1,NT("primary"));

	P(NT("primary"),1,symDigit);
	P(NT("primary"),3,T("("),NT("expr"),T(")"));

	grammar.setStartSymbol((NonterminalSymbol*)grammar.findSymbol("expr"));

#undef T
#undef NT
#undef P

	return g;
}

void mathExpr_destroy(Grammar *grammar)
{
	if(grammar)
		delete grammar;
}

GrammarTest mathExpr = {
	"Mathematical Expression",
	mathExpr_create,
	mathExpr_destroy,
	NULL,
};


#include "c_language/c_language.h"
#include "c_language/c_hc_lexer.h"
namespace C_language {
extern Grammar *C_grammar_init();
extern int c_lex(GrammarAnalyzer& gramAnalyzer, Token *pToken);
};

void C_postParsing(Grammar *grammar, bool result);
GrammarTest c_grammarTest = {
	"C Grammar",
	C_language::C_grammar_init,
	NULL,
	NULL,
	C_postParsing,
};



//////////////////////
// XML
//////////////////////

Grammar *xml_create()
{
	Grammar *g = new Grammar;
	Grammar &grammar = *g;

#define T(x) grammar.addTerminal(x)
#define NT(x) grammar.addNonTerminal(x)
#define P grammar.addProduction

	P(NT("tag_name"), 1, T("IDENTIFIER"));
	P(NT("attr_name"), 1, T("IDENTIFIER"));
	P(NT("target"), 1, T("IDENTIFIER"));

	P(NT("text"), 1, T("IDENTIFIER"));

	P(NT("attr_value"),1,T("IDENTIFIER"));
	P(NT("attr_value"),1,T("SINGLE_QUOTED_STRING"));
	P(NT("attr_value"),1,T("DOUBLE_QUOTED_STRING"));

	P(NT("attr"), 1, NT("attr_name"));
	P(NT("attr"), 3, NT("attr_name"), T("="), NT("attr_value"));

	P(NT("attr_list"),1,NT("attr"));
	P(NT("attr_list"),2,NT("attr_list"),NT("attr"));

	P(NT("start_tag"), 3, T("<"), NT("tag_name"), T(">"));
	P(NT("start_tag"), 4, T("<"), NT("tag_name"), NT("attr_list"), T(">"));

	P(NT("end_tag"), 3, T("</"), NT("tag_name"), T(">"));

	P(NT("self_closing_tag"), 3, T("<"), NT("tag_name"), T("/>"));
	P(NT("self_closing_tag"), 4, T("<"), NT("tag_name"), NT("attr_list"), T("/>"));

	P(NT("element_list"), 1, NT("element"));
	P(NT("element_list"), 2, NT("element_list"), NT("element"));

	P(NT("content_elem"), 1, NT("element"));
	P(NT("content_elem"), 1, NT("processing_instruction"));
	P(NT("content_elem"), 1, NT("comment"));
	P(NT("content_elem"), 1, NT("text"));

	P(NT("content"), 1, NT("content_elem"));
	P(NT("content"), 2, NT("content"), NT("content_elem"));

	P(NT("comment"), 2, T("<!--"), T("-->"));
	P(NT("comment"), 3, T("<!--"), T("STRING"), T("-->"));

	P(NT("processing_instruction"), 3, T("<?"), NT("target"), T("?>"));
	P(NT("processing_instruction"), 4, T("<?"), NT("target"), T("STRING"), T("?>"));

	P(NT("doctype"), 4, T("<!"), NT("DOCTYPE"), NT("STRING"), T(">"));

	P(NT("comment_or_pi"), 1, NT("comment"));
	P(NT("comment_or_pi"), 1, NT("processing_instruction"));

	P(NT("comment_or_pi_list"), 1, NT("comment_or_pi"));
	P(NT("comment_or_pi_list"), 2, NT("comment_or_pi_list"), NT("comment_or_pi"));

	P(NT("element"), 1, NT("self_closing_tag"));
	//P(NT("element"), 1, NT("start_tag"));
	P(NT("element"), 2, NT("start_tag"), NT("end_tag"));
	P(NT("element"), 3, NT("start_tag"), NT("content"), NT("end_tag"));

	P(NT("prolog"), 1, NT("doctype"));
	P(NT("prolog"), 2, NT("comment_or_pi_list"), NT("doctype"));
	P(NT("prolog"), 2, NT("doctype"), NT("comment_or_pi_list"));
	P(NT("prolog"), 3, NT("comment_or_pi_list"), NT("doctype"), NT("comment_or_pi_list"));

	P(NT("document"), 1, NT("prolog"));
	P(NT("document"), 2, NT("prolog"), NT("root_element"));
	P(NT("document"), 3, NT("prolog"), NT("root_element"), NT("comment_or_pi_list") );
	P(NT("document"), 1, NT("element"));
	P(NT("document"), 2, NT("element"), NT("comment_or_pi_list") );
	P(NT("document"), 1, NT("comment_or_pi_list"));

	grammar.setStartSymbol(NT("document"));
#undef T
#undef NT
#undef P

	return g;
}

void xml_destroy(Grammar *grammar)
{
	if(grammar)
		delete grammar;
}


GrammarTest xml = {
	"XML",
	xml_create,
	xml_destroy,
	NULL,
	NULL
};

class GrammarsManager
{
	std::vector<GrammarTest*> m_grammars;
	GrammarTest *m_usingGrammarInfo;
	Grammar *m_usingGrammar;
	GrammarAnalyzer *m_grammarAnalyzer;

	inline float getTime() {
		return (float)timeGetTime();
	}

public:
	GrammarsManager();
	void showGrammarList();
	bool selectGrammar(unsigned int i);
	void parseInput(const char *input);
	Grammar *selectedGrammar() const { return m_usingGrammar; }
	GrammarTest *selectedGrammarInfo() const { return m_usingGrammarInfo; }

	void grammarOptions();
};


GrammarsManager::GrammarsManager()
{
	m_usingGrammar = NULL;
	m_usingGrammarInfo = NULL;
	m_grammarAnalyzer = NULL;
	m_grammars.push_back(&testGrammar);
	m_grammars.push_back(&mathExpr);
	m_grammars.push_back(&c_grammarTest);
	m_grammars.push_back(&xml);
}

void GrammarsManager::showGrammarList()
{
	printf("Grammars:\n");
	for(unsigned int i = 0; i < m_grammars.size(); ++i) {
		printf("[%d] %s\n", i, m_grammars[i]->grammarName);
	}
}

bool GrammarsManager::selectGrammar(unsigned int i)
{
	if(i >= m_grammars.size())
		return false;
	if(m_usingGrammarInfo) {
		if(m_usingGrammarInfo->grammarDestroy)
			m_usingGrammarInfo->grammarDestroy(m_usingGrammar);
		m_usingGrammarInfo = NULL;
		m_usingGrammar = NULL;
	}
	m_usingGrammarInfo = m_grammars[i];

	Grammar *grammar;
	float xTime, xElapsedTime;

 	xTime = getTime();
	grammar = m_grammars[i]->grammarCreate();
	xElapsedTime = (getTime()-xTime)/1000.0f;
	std::cout << m_usingGrammarInfo->grammarName << " " << "initialization time: " << xElapsedTime << std::endl;

	if(m_grammarAnalyzer)
		delete m_grammarAnalyzer;
	m_grammarAnalyzer= new GrammarAnalyzer(*grammar);

	m_grammarAnalyzer->computeFirst();
	m_grammarAnalyzer->computeFollow();
	
	std::ofstream file;
	file.open(".\\grammar.txt");
	if(file.fail())
		std::cout << std::strerror(errno);
	m_grammarAnalyzer->dumpGrammar(file);
	file << std::endl;

	xTime = getTime();
	m_grammarAnalyzer->lr0(&file);
	xElapsedTime = (getTime()-xTime)/1000.0f;
	std::cout << "LR(0) time: " << xElapsedTime << std::endl;
	file << "LR(0) - ";
	m_grammarAnalyzer->dumpStates(file);
	file << std::endl;

	m_grammarAnalyzer->dumpFirstFollowNullable(file);
	file << std::endl;

	xTime = getTime();
	m_grammarAnalyzer->lalr(&file);
	xElapsedTime = (getTime()-xTime)/1000.0f;
	std::cout << "LALR time: " << xElapsedTime << std::endl;
	file << "LALR(1) - ";
	m_grammarAnalyzer->dumpStates(file);
	file << std::endl;
	file.close();

	m_grammarAnalyzer->resolveConflicts();
	m_grammarAnalyzer->generateParsingTable();
	file.open(".\\parsingTable.txt");
	m_grammarAnalyzer->dumpParsingTable(file);
	file.close();
	m_usingGrammar = grammar;
	grammarOptions();

	return true;
}

void GrammarsManager::parseInput(const char *input) {
//	std::ofstream file;
	LanguageParser *parser = new LanguageParser(m_grammarAnalyzer->generateLanguage());

	DefaultLexer *lexer = new DefaultLexer;

	lexer->addToken("</",m_usingGrammar->findSymbol("</")->index());
	lexer->addToken("[_a-zA-Z][_a-zA-Z0-9]*",m_usingGrammar->findSymbol("IDENTIFIER")->index());
	lexer->addToken("/>",m_usingGrammar->findSymbol("/>")->index());
	lexer->addToken("([[:alnum:] ]|(-[[:alnum:] ]))+",m_usingGrammar->findSymbol("STRING")->index());
	lexer->makeDefaultTerminalTokens(m_usingGrammar);
	parser->setTokenizer(lexer);

	bool result = true;
	if(m_usingGrammarInfo->preParsing)
		result = m_usingGrammarInfo->preParsing(m_usingGrammar);
	if(result) {
		result = parser->parse(input, NULL);
		if(result) {
			std::cout << "Valid input." << std::endl;
		}
		else
			std::cout << "Syntax error." << std::endl;
		if(m_usingGrammarInfo->postParsing)
			m_usingGrammarInfo->postParsing(m_usingGrammar, result);
	}

	delete parser->lexer();
	delete parser->language();
}


void GrammarsManager::grammarOptions()
{
	int opt;
	
	if(!m_usingGrammar) {
		printf("No grammar selected.");
		return;
	}
	printf("Using grammar: %s\n", m_usingGrammarInfo->grammarName);
	Grammar *selectedGrammar = m_usingGrammar;

	do {
		printf("Option:\n"
			"[0] <-- Back\n"
			"[1] Add Terminal Symbol\n"
			"[2] Add Nonterminal Symbol\n"
			"[3] Add Production\n"
			"[4] Remove Symbol\n"
			"[5] Remove Production\n"
			"[6] Show Symbols\n"
			"[7] Show Productions\n"
			"[8] Parse input text\n"
			"[9] Parse input file\n"
			);

		std::cin >> opt;
		std::cin.clear();
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

		const char *name = "";

		if(opt == 1) {
			selectedGrammar->addTerminal(name);
		}
		else if(opt == 2) {
			selectedGrammar->addNonTerminal(name);
		}
		else if(opt == 3) {

		}
		else if(opt == 4) {
			selectedGrammar->removeSymbol(name);
		}
		else if(opt == 5) {
			/*
			NonterminalSymbol *lhs;
			Production *proToRemove = NULL;
			const char *rhsName;
			lhs = selectedGrammar->findSymbol(lhs->name());
			ProductionList proList = lhs->proList();
			for(i = 0; i < proList.size() && proToRemove == NULL; ++i) {
				Production *pro = proList[i];
				for(j = 0; j < pro->nrhs(); ++j) {
					if(!pro->rhs(j)->compareByName(rhsName))
						break;
				}
			}

			if(proToRemove) {
				selectedGrammar->removeProduction(proToRemove);
			}
			*/
		}
		else if(opt == 6) {
			SymbolList symbolList = selectedGrammar->symbolList();
			for(unsigned int i = 0; i < symbolList.count(); ++i) {
				Symbol *sym = symbolList[i];
				printf("%d - %-2s - %s\n", sym->index(), sym->isTerminal() ? "T" : "NT", sym->name());
			}
		}
		else if(opt == 7) {
			ProductionList proList = selectedGrammar->productionList();
			for(unsigned int i = 0; i < proList.size(); ++i) {
				Production *pro = proList[i];
				printf("%s ->", pro->lhs()->name());
				for(unsigned int j = 0; j < pro->nrhs(); ++j)
					printf(" %s", pro->rhs(j)->name());
				printf("\n");
			}
		}
		else if(opt == 8) {
			std::string input;
			std::getline(std::cin,input);
			parseInput(input.c_str());
		}
		else if(opt == 9) {
			std::string inputFilePath;
			std::cout << "Input file: ";
			std::getline(std::cin, inputFilePath);
			File *inputFile = File::Open(inputFilePath.c_str(), File::read, NULL);
			if(inputFile) {
				parseInput(inputFile->Buffer());
				inputFile->Close();
			}
		}

		printf("\n");
	}  while(opt != 0);
}

int main(int argc, char *argv[])
{
	--argc;
	++argv;

	GrammarsManager manager;
/*
	if(argc < 1)
	{
		printf("No input file.\n");
		system("PAUSE");
		return 1;
	}

	char *input_file = *argv;
	char output_file[MAX_PATH];
	strncpy(output_file,input_file,(MAX_PATH-1)-5);
	strcat(output_file,".tok");

	tokenize_to_file(input_file,output_file);

	*/

#if 0
	if(grammarAnalysis.parse(g_testGrammar->inputString, &file))
		std::cout << "Valid input." << std::endl;
	else
		std::cout << "Syntax error." << std::endl;

#endif
#if 0
	for(;;) {
//		std::string input;
		std::string inputFilePath;
		std::cout << "Input file: ";
		std::getline(std::cin, inputFilePath);
		File *inputFile = File::Open(inputFilePath.c_str(), File::read, NULL);
		if(inputFile) {
			if(grammarAnalysis.parse(inputFile->Buffer(), &file))
				std::cout << "Valid input." << std::endl;
			else
				std::cout << "Syntax error." << std::endl;
			inputFile->Close();
		}
	}
#endif

	std::string input;
	int opt;
	do {
		printf(
			"Option:\n"
			"[0] Exit\n"
			"[1] Insert Grammar\n"
			"[2] Remove Grammar\n"
			"[3] Select Grammar\n"
			"[4] Show Grammar List\n"
		);
		
		std::cin >> opt;
		std::cin.clear();
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(),'\n');

		if(opt == 1) {
		}
		else if(opt == 2) {

		}
		else if(opt == 3) {
			manager.showGrammarList();
			int selection;
			std::cin >> selection;
			std::cin.clear();
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
			manager.selectGrammar(selection);
		}
		else if(opt == 4) {
			manager.showGrammarList();
		}

	} while(opt != 0);

	std::cout << std::endl;
	system("PAUSE");
	return 1;
}

void printTable()
{
}


void C_processNullStatement(C_language::NullStatement *stmt);
void C_processExpressionStatement(C_language::ExpressionStatement *stmt);
void C_processCompoundStatement(C_language::CompoundStatement *stmt);
void C_processIfStatement(C_language::IfStatement *stmt);
void C_processForStatement(C_language::ForStatement *stmt);
void C_processWhileStatement(C_language::WhileStatement *stmt);
void C_processDoWhileStatement(C_language::DoWhileStatement *stmt);
void C_processReturnStatement(C_language::ReturnStatement *stmt);
void C_processSwitchStatement(C_language::SwitchStatement *stmt);
void C_processReturnStatement(C_language::ReturnStatement *stmt);
void C_processLabeledStatement(C_language::LabeledStatement *stmt);

void C_processNullStatement(C_language::NullStatement *stmt)
{
	//do nothing
	printf("Null Statement;");
}

using namespace C_language;
void C_assembler_assembleExpression(C_language::Ast *exp)
{
	if(exp->type() != C_language::ASTT_OPERATOR)
		return;

	if(exp->left()->type() == C_language::ASTT_OPERATOR) {
		//recursively assemble left()
	}

	if(exp->right()->type() == C_language::ASTT_OPERATOR) {
	}

	C_operator op = exp->op();
	switch(op) {
		case OP_UNARY_PLUS:
		case OP_UNARY_MINUS:
		case OP_ADD:
		case OP_SUB:
		case OP_MUL:
		case OP_DIV:
		case OP_MOD:
			{
				int a = 1;
			}break;

		case OP_INC:
		case OP_DEC:
			break;

		case OP_ASSIGN:
			{
				break;
			}
		case OP_ADD_ASSIGN:
			{
				int a = 1;

				Ast *left = exp->left();
				Ast *right = exp->right();

				break;
			}
	}

/*	OP_ADD_ASSIGN,
	OP_SUB_ASSIGN,
	OP_MUL_ASSIGN,
	OP_DIV_ASSIGN,
	OP_MOD_ASSIGN,
	OP_AND_ASSIGN,
	OP_OR_ASSIGN,
	OP_XOR_ASSIGN,
	OP_LSHIFT_ASSIGN,
	OP_RSHIFT_ASSIGN,
/*
	OP_EQ,
	OP_GT,
	OP_LT,
	OP_NE,
	OP_GE,
	OP_LE,

	OP_LOGICAL_AND,
	OP_LOGICAL_OR,
	OP_LOGICAL_NOT,

	OP_BITWISE_AND,
	OP_BITWISE_OR,
	OP_BITWISE_XOR,
	OP_BITWISE_NOT,
	OP_LSHIFT,
	OP_RSHIFT,

	OP_ARRAY_SUBSCRIPT,
	OP_DEREF, //indirection
	OP_REF, //address
	OP_MEMBER_REF,
	OP_MEMBER_POINT,

	OP_CALL,
	OP_COMMA,
	OP_COND,
	OP_COND2,
	OP_SIZEOF,
	OP_Ast,*/
}

void C_processExpressionStatement(C_language::ExpressionStatement *stmt)
{
	C_language::Ast *exp = stmt->expression();
	do {
		if(exp->isAssignmentExpression()) {
			printf("todo: assemble an expression\n");
			int op = exp->op();
			C_assembler_assembleExpression(exp);
		}
		else {
			printf("todo: useless expression\n");
			//useless expression
			C_language::Ast *nextExp = exp->next();
			exp->release();
			exp = nextExp;
			continue;
		}

		exp = exp->next();

	}while(exp);
}

void C_processCompoundStatement(C_language::CompoundStatement *stmt)
{
	C_language::CSymbol *declarations = stmt->declarationList();
	int allocSize = stmt->calculateNecessaryAllocSize();

	C_language::Statement *statement = stmt->statementList();
	while(statement) {
		C_language::StatementType stmtType = statement->type();
		if(stmtType == C_language::STMT_COMPOUND) {
			C_processCompoundStatement((C_language::CompoundStatement*)statement);
		}
		else if(stmtType == C_language::STMT_RETURN) {
			C_processReturnStatement((C_language::ReturnStatement*)statement);
		}
		else if(stmtType == C_language::STMT_EXPRESSION) {
			C_processExpressionStatement((C_language::ExpressionStatement*)statement);
		}

		statement = statement->next();
	}
}

void C_processIfStatement(C_language::IfStatement *stmt)
{
	printf("todo: assemble an if statement\n");
}

void C_processForStatement(C_language::ForStatement *stmt)
{
	printf("todo: assemble a for statement\n");
}

void C_processWhileStatement(C_language::WhileStatement *stmt)
{
	printf("todo: assemble an while statement\n");
}

void C_processDoWhileStatement(C_language::DoWhileStatement *stmt)
{
	printf("todo: assemble an do-while statement\n");
}

void C_processSwitchStatement(C_language::SwitchStatement *stmt)
{
	printf("todo: assemble an switch statement\n");
}

void C_processReturnStatement(C_language::ReturnStatement *stmt)
{
	int size = 0;
	printf("return %d\n",size);
}

void C_processLabeledStatement(C_language::LabeledStatement *stmt)
{
	printf("Label: %s\n","todo");
}

void C_postParsing(Grammar *grammar, bool result)
{
	//Check for Main Symbol
	C_language::Scope *scope = C_language::C_globalScope();
	C_language::CSymbol *symMain = scope->findSymbol("main");
	C_language::Function *mainType;

	if(!symMain) {
		//no "main" symbol defined
		int error = 1;
		return;
	}

	//Make sure the "main" symbol is a function
	if(symMain->type()->type() != C_language::SymbolType::SYMT_FUNCTION) {
		//main is not a function
		int error = 1;
		return;
	}

	mainType = (C_language::Function*)symMain->type();
	int paramCount = mainType->paramCount();

	//calculate local necessary size
	//process_statements
	C_language::CompoundStatement *body = mainType->functionBody();
	if(!body) {
		//main function declared but not defined
		int error = 1;
		return;
	}

	C_processCompoundStatement(body);
}

