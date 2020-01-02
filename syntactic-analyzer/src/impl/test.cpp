
//Last edit: 18/02/2018 04:46

#include <iostream>
#include <fstream>
#include <vector>

#include "file.h"
#include "parser/languageparser.h"

#include "../GrammarBuilder.h"

#undef min
#undef max

#ifdef WIN32
#include <windows.h>
#pragma comment(lib,"Winmm.lib") //for timeGetTime
#include <Mmsystem.h> //for timeGetTime

unsigned int getTime() {
	return timeGetTime();
}

#else
unsigned int getTime() {
	return 0;
}
#endif

using namespace syntacticanalyzer;


#if 0

struct GrammarTest
{
	const char *grammarName;
	Grammar* (*grammarCreate)();
	void (*grammarDestroy)(Grammar *grammar);
	bool (*preParsing)(LanguageParser *parser);
	void (*postParsing)(LanguageParser *parser, bool result);
};

Grammar *testGrammar_create()
{
	GrammarBuilder* grammarBuilder = createDefaultGrammarBuilder();

#define P(x) { NonterminalSymbol* __X = grammarBuilder->addNonTerminal(x);
#define D grammarBuilder->newProduction(__X);
#define T(x) grammarBuilder->addRHS(grammarBuilder->addTerminal(x)->index());
#define N(x) grammarBuilder->addRHS(grammarBuilder->addNonTerminal(x)->index());
#define S(x) grammarBuilder->setSemanticAction(x);
#define E grammarBuilder->addProduction(); }
#define O grammarBuilder->addProduction(); grammarBuilder->newProduction(__X);

	Symbol *a = grammarBuilder->addTerminal("a");
	Symbol *b = grammarBuilder->addTerminal("b");
	Symbol *c = grammarBuilder->addTerminal("c");
	Symbol *d = grammarBuilder->addTerminal("d");
	Symbol *e = grammarBuilder->addTerminal("e");

	P("S")
	D T("a") N("B") T("c")
	O T("b") N("C") T("c")
	O T("a") N("C") T("d")
	O T("b") N("B") T("d")
	E

	P("B") D T("e") E
	P("C") D T("e") E

	grammarBuilder->setStartSymbol((NonterminalSymbol*)grammarBuilder->symbol("S"));

#undef P
#undef D
#undef T
#undef N
#undef S
#undef E
#undef O

	return grammarBuilder->build();
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
	GrammarBuilder* grammarBuilder = createDefaultGrammarBuilder();

#define P(x) { NonterminalSymbol* __X = grammarBuilder->addNonTerminal(x);
#define D grammarBuilder->newProduction(__X);
#define T(x) grammarBuilder->addRHS(grammarBuilder->addTerminal(x)->index());
#define N(x) grammarBuilder->addRHS(grammarBuilder->addNonTerminal(x)->index());
#define S(x) grammarBuilder->addRHS(x);
#define E grammarBuilder->addProduction(); }
#define O grammarBuilder->addProduction(); grammarBuilder->newProduction(__X);

	Symbol *symPlus = grammarBuilder->addTerminal("+");
	Symbol *symMinus = grammarBuilder->addTerminal("-");
	Symbol *symTimes = grammarBuilder->addTerminal("*");
	Symbol *symDivide = grammarBuilder->addTerminal("/");
	Symbol *symDigit = grammarBuilder->addTerminal("CONSTANT");

	P("expr") D N("term") E

	P("term")
		D N("term") T("+") N("factor")
		O N("term") T("-") N("factor")
		O N("factor")
		O N("primary")
		E

	P("factor")
		D N("factor") T("*") N("primary")
		O N("factor") T("/") N("primary")
		O N("primary")
		E

	P("primary")
		D T("CONSTANT")
		O T("(") N("expr") T(")")
		E

	grammarBuilder->setStartSymbol((NonterminalSymbol*)grammarBuilder->symbol("expr"));

#undef P
#undef D
#undef T
#undef N
#undef S
#undef E
#undef O

	return grammarBuilder->build();
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


#include "../c_language/c_language.h"
#include "../c_language/c_hc_lexer.h"
namespace C_language {
extern Grammar *C_grammar_init();
};

bool C_preParsing(LanguageParser *parser);
void C_postParsing(LanguageParser *parser, bool result);
GrammarTest c_grammarTest = {
	"C Grammar",
	C_language::C_grammar_init,
	NULL,
	C_preParsing,
	C_postParsing,
};



//////////////////////
// XML
//////////////////////

Grammar *xml_create()
{
	GrammarBuilder* grammarBuilder = createDefaultGrammarBuilder();

#define P(x) { NonterminalSymbol* __X = grammarBuilder->addNonTerminal(x);
#define D grammarBuilder->newProduction(__X);
#define T(x) grammarBuilder->addRHS(grammarBuilder->addTerminal(x)->index());
#define N(x) grammarBuilder->addRHS(grammarBuilder->addNonTerminal(x)->index());
#define S(x) grammarBuilder->addRHS(x);
#define E grammarBuilder->addProduction(); }
#define O grammarBuilder->addProduction(); grammarBuilder->newProduction(__X);

	P("tag_name") D T("IDENTIFIER") E
	P("attr_name") D T("IDENTIFIER") E
	P("target") D T("IDENTIFIER") E

	P("text") D T("IDENTIFIER") E

	P("attr_value")
		D T("IDENTIFIER")
		O T("SINGLE_QUOTED_STRING")
		O T("DOUBLE_QUOTED_STRING")
		E

	P("attr")
		D N("attr_name")
		O N("attr_name") T("=") N("attr_value")
		E

	P("attr_list")
		D N("attr")
		O N("attr_list") N("attr")
		E

	P("start_tag")
		D T("<") N("tag_name") T(">")
		O T("<") N("tag_name") N("attr_list") T(">")
		E

	P("end_tag") D T("</") N("tag_name") T(">") E

	P("self_closing_tag")
		D T("<") N("tag_name") T("/>")
		O T("<") N("tag_name") N("attr_list") T("/>")
		E

	P("element_list")
		D N("element")
		O N("element_list") N("element")
		E

	P("content_elem")
		D N("element")
		O N("processing_instruction")
		O N("comment")
		O N("text")
		E

	P("content")
		D N("content_elem")
		O N("content") N("content_elem")
		E

	P("comment")
		D T("<!--") T("-->")
		O T("<!--") T("STRING") T("-->")
		E

	P("processing_instruction")
		D T("<?") N("target") T("?>")
		O T("<?") N("target") T("STRING") T("?>")
		E

	P("doctype") D T("<!") N("DOCTYPE") N("STRING") T(">") E

	P("comment_or_pi")
		D N("comment")
		O N("processing_instruction")
		E

	P("comment_or_pi_list")
		D N("comment_or_pi")
		O N("comment_or_pi_list") N("comment_or_pi")
		E

	P("element")
		D N("self_closing_tag")
		//O N("start_tag")
		O N("start_tag") N("end_tag")
		O N("start_tag") N("content") N("end_tag")
		E

	P("prolog")
		D N("doctype")
		O N("comment_or_pi_list") N("doctype")
		O N("doctype") N("comment_or_pi_list")
		O N("comment_or_pi_list") N("doctype") N("comment_or_pi_list")
		E

	P("document")
		D N("prolog")
		O N("prolog") N("root_element")
		O N("prolog") N("root_element") N("comment_or_pi_list")
		O N("element")
		O N("element") N("comment_or_pi_list")
		O N("comment_or_pi_list")
		E

	grammarBuilder->setStartSymbol((NonterminalSymbol*)grammarBuilder->symbol("document"));

#undef P
#undef D
#undef T
#undef N
#undef S
#undef E
#undef O

	return grammarBuilder->build();
}

void xml_destroy(Grammar *grammar)
{
	if(grammar)
		delete grammar;
}

bool xml_preParsing(LanguageParser *parser) {
	DefaultLexer *lexer = new DefaultLexer;
	Grammar *g = parser->grammar();
	lexer->addToken("</",g->symbol("</")->index());
	lexer->addToken("[_a-zA-Z][_a-zA-Z0-9]*",g->symbol("IDENTIFIER")->index());
	lexer->addToken("/>",g->symbol("/>")->index());
	lexer->addToken("([[:alnum:] ]|(-[[:alnum:] ]))+",g->symbol("STRING")->index());
	lexer->makeDefaultTerminalTokens(g);
	parser->setTokenizer(lexer);
	return true;
}

void xml_postparsing(LanguageParser *parser, bool result) {
	if(parser->lexer())
		delete parser->lexer();
}


GrammarTest xml = {
	"XML",
	xml_create,
	xml_destroy,
	xml_preParsing,
	xml_postparsing
};

class GrammarsManager
{
	std::vector<GrammarTest*> m_grammars;
	GrammarTest *m_usingGrammarInfo;
	Grammar *m_usingGrammar;
	GrammarAnalyzer *m_grammarAnalyzer;

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
	unsigned int xTime, xElapsedTime;

 	xTime = getTime();
	grammar = m_grammars[i]->grammarCreate();
	xElapsedTime = getTime() - xTime;
	std::cout << m_usingGrammarInfo->grammarName << " " << "initialization time (ms): " << xElapsedTime << std::endl;

	if(m_grammarAnalyzer)
		delete m_grammarAnalyzer;
	m_grammarAnalyzer= new GrammarAnalyzer(*grammar);

	m_grammarAnalyzer->computeFirst();
	m_grammarAnalyzer->computeFollow();
	
	std::ofstream file;
	file.open(".\\grammar.txt");
	/*
	if(file.fail())
		std::cout << std::strerror(errno);
		*/
	m_grammarAnalyzer->dumpGrammar(file);
	file << std::endl;

	xTime = getTime();
	m_grammarAnalyzer->lr0(&file);
	xElapsedTime = getTime()-xTime;
	std::cout << "LR(0) time (ms): " << xElapsedTime << std::endl;
	file << "LR(0) - ";
	m_grammarAnalyzer->dumpStates(file);
	file << std::endl;

	m_grammarAnalyzer->dumpFirstFollowNullable(file);
	file << std::endl;

	xTime = getTime();
	m_grammarAnalyzer->lalr(&file);
	xElapsedTime = getTime()-xTime;
	std::cout << "LALR time (ms): " << xElapsedTime << std::endl;
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
	LanguageParser *parser = new LanguageParser(new Language(m_grammarAnalyzer->grammar(),m_grammarAnalyzer->parsingTable()));

	bool result = true;
	if(m_usingGrammarInfo->preParsing)
		result = m_usingGrammarInfo->preParsing(parser);
	if(result) {
		result = parser->parse(input, NULL);
		if(result) {
			std::cout << "Valid input." << std::endl;
		}
		else
			std::cout << "Syntax error." << std::endl;
		if(m_usingGrammarInfo->postParsing)
			m_usingGrammarInfo->postParsing(parser, result);
	}

	if(parser->language())
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
		}
		else if(opt == 2) {
		}
		else if(opt == 3) {

		}
		else if(opt == 4) {
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
			/*
			SymbolList symbolList = selectedGrammar->symbolList();
			for(unsigned int i = 0; i < symbolList.count(); ++i) {
				Symbol *sym = symbolList[i];
				printf("%d - %-2s - %s\n", sym->index(), sym->isTerminal() ? "T" : "NT", sym->name());
			}
			*/
		}
		else if(opt == 7) {
			/*
			ProductionList proList = selectedGrammar->productionList();
			for(unsigned int i = 0; i < proList.size(); ++i) {
				Production *pro = proList[i];
				printf("%s ->", pro->lhs()->name());
				for(unsigned int j = 0; j < pro->rhsCount(); ++j)
					printf(" %s", pro->rhs(j)->name());
				printf("\n");
			}
			*/
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
			"[1] Select Lexer\n"
			//"[2] Insert Grammar\n"
			//"[3] Remove Grammar\n"
			"[2] Select Grammar\n"
			"[3] Show Grammar List\n"
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

bool C_preParsing(LanguageParser *parser) {
	parser->setTokenizer(new C_language::c_hcLexer(*parser));
	return true;
}

void C_postParsing(LanguageParser *parser, bool result)
{
	delete parser->lexer();
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

#endif





class ParsingManager {
	LanguageParser* currentParser;

	Lexer* m_selectedLexer;
	Grammar* m_selectedGrammar;

	void setLexer();
	void setGrammar();

	void parseInputText(const char* input) {
		currentParser->parse(input);
	}

	void parseInputFile(const char* filePath) {
		File* inputFile = File::Open(filePath, File::read, NULL);
		if (inputFile) {
			parseInputText(inputFile->Buffer());
			inputFile->Close();
		}

	}

	void promptLexerOptions() {
	}

	void promptGrammarOptions() {
	}

	void promptInputToParse() {
		std::string input;
		std::cout << "Input: ";
		std::getline(std::cin, input);
		parseInputText(input.c_str());
	}

	void promptFileToParse() {
		std::string inputFilePath;
		std::cout << "Input file: ";
		std::getline(std::cin, inputFilePath);
		parseInputFile(inputFilePath.c_str());
	}

public:
	ParsingManager() {

	}
	~ParsingManager() {

	}

	void promptOptions() {
		int opt;
		for (;;) {
			printf("Option:\n"
				"[0] <-- Back\n"
				"[1] Select Lexer\n"
				"[2] Select Grammar\n"
				"[3] Parse input text\n"
				"[4] Parse input file\n"
			);

			std::cin >> opt;
			std::cin.clear();
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

			switch (opt) {
			case 0:
				return;
			case 1:
				promptLexerOptions();
				break;
			case 2:
				promptGrammarOptions();
				break;
			case 3:
				promptInputToParse();
				break;
			case 4:
				promptFileToParse();
				break;
			}
		}
	}

};

ParsingManager parsingManager;


int main(int argc, char *argv) {
	parsingManager.promptOptions();
	return 0;
}

