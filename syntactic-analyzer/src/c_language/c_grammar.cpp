
#include <SyntacticAnalyzer.h>

#include "c_actions_decl.h"

namespace C_language {

using namespace syntacticanalyzer;

Grammar *C_grammar_create()
{
	GrammarBuilder* grammarBuilder = createDefaultGrammarBuilder();

#define P(x) { NonterminalSymbol* __X = grammarBuilder->addNonterminal(x);
#define D grammarBuilder->newProduction(__X);
#define T(x) grammarBuilder->addRHS(grammarBuilder->addTerminal(x)->name());
#define N(x) grammarBuilder->addRHS(grammarBuilder->addNonterminal(x)->name());
#define S(x) grammarBuilder->setSemanticAction(0);
#define E grammarBuilder->addProduction(); }
#define O grammarBuilder->addProduction(); grammarBuilder->newProduction(__X);

		/*
#define P(lhs) __X = grammar.addNonTerminal(lhs);
#define D __P = grammar.addProduction(__X);
#define T(x) __P->addRHS(grammar.addTerminal(x));
#define NT(x) __P->addRHS(grammar.addNonTerminal(x));
#define OR D
#define E __P->addRHS(&Grammar::epsilon); //insert empty string
#define S(x) __P->setSemanticAction(x);
*/

	/*
	primary_expression
		: IDENTIFIER
		| CONSTANT
		| STRING_LITERAL
		| '(' expression ')'
		;
	*/
	P("primary_expression")
		D T("IDENTIFIER") 					S(primary_expression1)
		O T("CONSTANT") 					S(primary_expression2)
		O T("STRING_LITERAL")				S(primary_expression3)
		O T("(") N("expression") T(")")	S(primary_expression4)
		E

	/*
	postfix_expression
		: primary_expression
		| postfix_expression '[' expression ']'
		| postfix_expression '(' ')'
		| postfix_expression '(' argument_expression_list ')'
		| postfix_expression '.' IDENTIFIER
		| postfix_expression PTR_OP IDENTIFIER
		| postfix_expression INC_OP
		| postfix_expression DEC_OP
		;
	*/
	P("postfix_expression")
		D N("primary_expression")													S(postfix_expression1)
		O N("postfix_expression") T("[") N("expression") T("]")					S(postfix_expression2)
		O N("postfix_expression") T("(") T(")")									S(postfix_expression3)
		O N("postfix_expression") T("(") N("argument_expression_list") T(")")	S(postfix_expression4)
		O N("postfix_expression") N("struct_member_accessor") T("IDENTIFIER")	S(postfix_expression5)
		O N("postfix_expression") T("INC_OP")										S(postfix_expression6)
		O N("postfix_expression") T("DEC_OP")										S(postfix_expression7)
		E

	/*
	struct_member_accessor *
		: '.'
		| PTR_OP
		;
	*/
	P("struct_member_accessor")
		D T(".")			S(struct_member_accessor1)
		O T("PTR_OP")		S(struct_member_accessor2)
		E

	/*
	argument_expression_list
		: assignment_expression
		| argument_expression_list ',' assignment_expression
		;
	*/
	P("argument_expression_list")
		D N("assignment_expression")											S(argument_expression_list1)
		O N("argument_expression_list") T(",") N("assignment_expression")	S(argument_expression_list2)
		E

	/*
	unary_expression
		: postfix_expression
		| INC_OP unary_expression
		| DEC_OP unary_expression
		| unary_operator cast_expression
		| SIZEOF unary_expression
		| SIZEOF '(' type_name ')'
		;
	*/
	P("unary_expression")
		D N("postfix_expression")							S(unary_expression1)
		O T("INC_OP") N("unary_expression")				S(unary_expression2)
		O T("DEC_OP") N("unary_expression")				S(unary_expression3)
		O N("unary_operator") N("cast_expression")		S(unary_expression4)
		O T("SIZEOF") N("unary_expression")				S(unary_expression5)
		O T("SIZEOF") T("(") N("type_name") T(")")		S(unary_expression6)
		E

	/*
	unary_operator
		: '&'
		| '*'
		| '+'
		| '-'
		| '~'
		| '!'
		;
	*/
	P("unary_operator")
		D T("&")	S(unary_operator1)
		O T("*")	S(unary_operator2)
		O T("+")	S(unary_operator3)
		O T("-")	S(unary_operator4)
		O T("~")	S(unary_operator5)
		O T("!")	S(unary_operator6)
		E

	/*
	cast_expression
		: unary_expression
		| '(' type_name ')' cast_expression
		;
	*/
	P("cast_expression")
		D N("unary_expression")								S(cast_expression1)
		O T("(") N("type_name") T(")") N("cast_expression")	S(cast_expression2)
		E

	/*
	multiplicative_expression
		: cast_expression
		| multiplicative_expression '*' cast_expression
		| multiplicative_expression '/' cast_expression
		| multiplicative_expression '%' cast_expression
		;
	*/
	P("multiplicative_expression")
		D N("cast_expression")												S(multiplicative_expression1)
		O N("multiplicative_expression") T("*") N("cast_expression")			S(multiplicative_expression2)
		O N("multiplicative_expression") T("/") N("cast_expression")			S(multiplicative_expression3)
		O N("multiplicative_expression") T("%") N("cast_expression")			S(multiplicative_expression4)
		E

	/*
	additive_expression
		: multiplicative_expression
		| additive_expression '+' multiplicative_expression
		| additive_expression '-' multiplicative_expression
		;
	*/
	P("additive_expression")
		D N("multiplicative_expression")										S(additive_expression1)
		O N("additive_expression") T("+") N("multiplicative_expression")		S(additive_expression2)
		O N("additive_expression") T("-") N("multiplicative_expression")		S(additive_expression3)
		E

	/*
	shift_expression
		: additive_expression
		| shift_expression LEFT_OP additive_expression
		| shift_expression RIGHT_OP additive_expression
		;
	*/
	P("shift_expression")
		D N("additive_expression")											S(shift_expression1)
		O N("shift_expression") T("LEFT_OP") N("additive_expression")		S(shift_expression2)
		O N("shift_expression") T("RIGHT_OP") N("additive_expression")		S(shift_expression3)
		E

	/*
	relational_expression
		: shift_expression
		| relational_expression '<' shift_expression
		| relational_expression '>' shift_expression
		| relational_expression LE_OP shift_expression
		| relational_expression GE_OP shift_expression
		;
	*/
	P("relational_expression")
		D N("shift_expression")												S(relational_expression1)
		O N("relational_expression") T("<") N("shift_expression")			S(relational_expression2)
		O N("relational_expression") T(">") N("shift_expression")			S(relational_expression3)
		O N("relational_expression") T("LE_OP") N("shift_expression")		S(relational_expression4)
		O N("relational_expression") T("GE_OP") N("shift_expression")		S(relational_expression5)
		E

	/*
	equality_expression
		: relational_expression
		| equality_expression EQ_OP relational_expression
		| equality_expression NE_OP relational_expression
		;
	*/
	P("equality_expression")
		D N("relational_expression")											S(equality_expression1)
		O N("equality_expression") T("EQ_OP") N("relational_expression")		S(equality_expression2)
		O N("equality_expression") T("NE_OP") N("relational_expression")		S(equality_expression3)
		E

	/*
	and_expression
		: equality_expression
		| and_expression '&' equality_expression
		;
	*/
	P("and_expression")
		D N("equality_expression")											S(and_expression1)
		O N("and_expression") T("&") N("equality_expression")				S(and_expression2)
		E

	/*
	exclusive_or_expression
		: and_expression
		| exclusive_or_expression '^' and_expression
		;
	*/
	P("exclusive_or_expression")
		D N("and_expression")													S(exclusive_or_expression1)
		O N("exclusive_or_expression") T("^") N("and_expression")			S(exclusive_or_expression2)
		E

	/*
	inclusive_or_expression
		: exclusive_or_expression
		| inclusive_or_expression '|' exclusive_or_expression
		;
	*/
	P("inclusive_or_expression")
		D N("exclusive_or_expression")										S(inclusive_or_expression1)
		O N("inclusive_or_expression") T("|") N("exclusive_or_expression")	S(inclusive_or_expression2)
		E

	/*
	logical_and_expression
		: inclusive_or_expression
		| logical_and_expression AND_OP inclusive_or_expression
		;
	*/
	P("logical_and_expression")
		D N("inclusive_or_expression")											S(logical_and_expression1)
		O N("logical_and_expression") T("AND_OP") N("inclusive_or_expression")	S(logical_and_expression2)
		E

	/*
	logical_or_expression
		: logical_and_expression
		| logical_or_expression OR_OP logical_and_expression
		;
	*/
	P("logical_or_expression")
		D N("logical_and_expression")											S(logical_or_expression1)
		O N("logical_or_expression") T("OR_OP") N("logical_and_expression")		S(logical_or_expression2)
		E

	/*
	conditional_expression
		: logical_or_expression
		| logical_or_expression '?' expression ':' conditional_expression
		;
	*/
	P("conditional_expression")
		D N("logical_or_expression")															S(conditional_expression1)
		O N("logical_or_expression") T("?") N("expression") T(":") N("conditional_expression")	S(conditional_expression2)
		E

	/*
	assignment_expression
		: conditional_expression
		| unary_expression assignment_operator assignment_expression
		;
	*/
	P("assignment_expression")
		D N("conditional_expression")														S(assignment_expression1)
		O N("unary_expression") N("assignment_operator") N("assignment_expression")		S(assignment_expression2)
		E

	/*
	assignment_operator
		: '='
		| MUL_ASSIGN
		| DIV_ASSIGN
		| MOD_ASSIGN
		| ADD_ASSIGN
		| SUB_ASSIGN
		| LEFT_ASSIGN
		| RIGHT_ASSIGN
		| AND_ASSIGN
		| XOR_ASSIGN
		| OR_ASSIGN
		;
	*/
	P("assignment_operator")
		D T("=")							S(assignment_operator1)
		O T("MUL_ASSIGN")					S(assignment_operator2)
		O T("DIV_ASSIGN")					S(assignment_operator3)
		O T("MOD_ASSIGN")					S(assignment_operator4)
		O T("ADD_ASSIGN")					S(assignment_operator5)
		O T("SUB_ASSIGN")					S(assignment_operator6)
		O T("LEFT_ASSIGN")					S(assignment_operator7)
		O T("RIGHT_ASSIGN")				S(assignment_operator8)
		O T("AND_ASSIGN")					S(assignment_operator9)
		O T("XOR_ASSIGN")					S(assignment_operator10)
		O T("OR_ASSIGN")					S(assignment_operator11)
		E

	/*
	expression
		: assignment_expression
		| expression ',' assignment_expression
		;
	*/
	P("expression")
		D N("assignment_expression")							S(assignment_expression1)
		O N("expression") T(",") N("assignment_expression")	S(assignment_expression2)
		E

	/*
	constant_expression
		: conditional_expression
		;
	*/
	P("constant_expression")
		D N("conditional_expression")		S(constant_expression1)
		E

	/*
	declaration
		: declaration_specifiers ';'
		| declaration_specifiers init_declarator_list ';'
		;
	*/
	P("declaration")
		D N("declaration_specifiers") T(";")								S(declaration1)
		O N("declaration_specifiers") N("init_declarator_list") T(";")	S(declaration2)
		E

	/*
	declaration_specifiers
		: storage_class_specifier
		| storage_class_specifier declaration_specifiers
		| type_specifier
		| type_specifier declaration_specifiers
		| type_qualifier
		| type_qualifier declaration_specifiers
		;
	*/
	P("declaration_specifiers")
		D N("storage_class_specifier")								S(declaration_specifiers1)
		O N("storage_class_specifier") N("declaration_specifiers")	S(declaration_specifiers2)
		O N("type_specifier")											S(declaration_specifiers3)
		O N("type_specifier") N("declaration_specifiers")			S(declaration_specifiers4)
		O N("type_qualifier")											S(declaration_specifiers5)
		O N("type_qualifier") N("declaration_specifiers")			S(declaration_specifiers6)
		E

	/*
	init_declarator_list
		: init_declarator
		| init_declarator_list ',' init_declarator
		;
	*/
	P("init_declarator_list")
		D N("init_declarator")										S(init_declarator_list1)
		O N("init_declarator_list") T(",") N("init_declarator")		S(init_declarator_list2)
		E

	/*
	init_declarator
		: declarator
		| declarator '=' initializer
		;
	*/
	P("init_declarator")
		D N("declarator")										S(init_declarator1)
		O N("declarator") T("=") N("initializer")			S(init_declarator2)
		E

	/*
	storage_class_specifier
		: TYPEDEF
		| EXTERN
		| STATIC
		| AUTO
		| REGISTER
		;
	*/
	P("storage_class_specifier")
		D T("TYPEDEF")		S(storage_class_specifier1)
		O T("EXTERN")		S(storage_class_specifier2)
		O T("STATIC")		S(storage_class_specifier3)
		O T("AUTO")		S(storage_class_specifier4)
		O T("REGISTER")	S(storage_class_specifier5)
		E

	/*
	type_specifier
		: VOID
		| CHAR
		| SHORT
		| INT
		| LONG
		| FLOAT
		| DOUBLE
		| SIGNED
		| UNSIGNED
		| struct_or_union_specifier
		| enum_specifier
		| TYPE_NAME
		;
	*/
	P("type_specifier")
		D T("VOID")								S(type_specifier1)
		O T("CHAR")								S(type_specifier2)
		O T("SHORT")							S(type_specifier3)
		O T("INT")								S(type_specifier4)
		O T("LONG")								S(type_specifier5)
		O T("FLOAT")							S(type_specifier6)
		O T("DOUBLE")							S(type_specifier7)
		O T("SIGNED")							S(type_specifier8)
		O T("UNSIGNED")							S(type_specifier9)
		O N("struct_or_union_specifier")		S(type_specifier10)
		O N("enum_specifier")					S(type_specifier11)
		O T("TYPE_NAME")						S(type_specifier12)
		E

	/*
	struct_or_union_specifier
		: struct_or_union IDENTIFIER '{' struct_declaration_list '}'
		| struct_or_union '{' struct_declaration_list '}'
		| struct_or_union IDENTIFIER
		;
	*/
	P("struct_or_union_specifier")
		D N("struct_or_union") N("struct_tag_opt") N("struct_definition_start") N("struct_declaration_list") N("struct_definition_end")	S(struct_or_union_specifier1)
		O N("struct_or_union") T("IDENTIFIER")																								S(struct_or_union_specifier2)
		E

	/*
	struct_tag_opt *
		: IDENTIFIER
		|
		;
	*/
	P("struct_tag_opt")
		D T("IDENTIFIER")		S(struct_tag_opt1)
		O						S(struct_tag_opt2) 
		E

	/*
	struct_definition_start *
		: '{'
		;
	*/
	P("struct_definition_start")
		D T("{")		S(struct_definition_start1)
		E

	/*
	struct_definition_end *
		: '}'
		;
	*/
	P("struct_definition_end")
		D T("}")		S(struct_definition_end1)
		E

	/*
	struct_or_union
		: STRUCT
		| UNION
		;
	*/
	P("struct_or_union")
		D T("STRUCT")			S(struct_or_union1)
		O T("UNION")			S(struct_or_union2)
		E

	/*
	struct_declaration_list
		: struct_declaration
		| struct_declaration_list struct_declaration
		;
	*/
	P("struct_declaration_list")
		D N("struct_declaration")									S(struct_declaration_list1)
		O N("struct_declaration_list") N("struct_declaration")	S(struct_declaration_list2)
		E

	/*
	struct_declaration
		: specifier_qualifier_list struct_declarator_list ';'
		;
	*/
	P("struct_declaration")
		D N("specifier_qualifier_list") N("struct_declarator_list") T(";")	S(struct_declaration1)
		E

	/*
	specifier_qualifier_list
		: type_specifier specifier_qualifier_list
		| type_specifier
		| type_qualifier specifier_qualifier_list
		| type_qualifier
		;
	*/
	P("specifier_qualifier_list")
		D N("type_specifier") N("specifier_qualifier_list")		S(specifier_qualifier_list1)
		O N("type_specifier")										S(specifier_qualifier_list2)
		O N("type_qualifier") N("specifier_qualifier_list")		S(specifier_qualifier_list3)
		O N("type_qualifier")										S(specifier_qualifier_list4)
		E

	/*
	struct_declarator_list
		: struct_declarator
		| struct_declarator_list ',' struct_declarator
		;
	*/
	P("struct_declarator_list")
		D N("struct_declarator")										S(struct_declarator_list1)
		O N("struct_declarator_list") T(",") N("struct_declarator")	S(struct_declarator_list2)
		E

	/*
	struct_declarator
		: declarator
		| ':' constant_expression
		| declarator ':' constant_expression
		;
	*/
	P("struct_declarator")
		D N("declarator")										S(struct_declarator1)
		O T(":") N("constant_expression")						S(struct_declarator2)
		O N("declarator") T(":") N("constant_expression")	S(struct_declarator3)
		E

	/*
	enum_specifier
		: ENUM '{' enumerator_list '}'
		| ENUM IDENTIFIER '{' enumerator_list '}'
		| ENUM IDENTIFIER
		;
	*/
	P("enum_specifier")
		D T("ENUM") T("{") N("enumerator_list") T("}")					S(enum_specifier1)
		O T("ENUM") T("IDENTIFIER") T("{") N("enumerator_list") T("}")	S(enum_specifier2)
		O T("ENUM") T("IDENTIFIER")										S(enum_specifier3)
		E

	/*
	enumerator_list
		: enumerator
		| enumerator_list ',' enumerator
		;
	*/
	P("enumerator_list")
		D N("enumerator")										S(enumerator_list1)
		O N("enumerator_list") T(",") N("enumerator")		S(enumerator_list2)
		E

	/*
	enumerator
		: IDENTIFIER
		| IDENTIFIER '=' constant_expression
		;
	*/
	P("enumerator");
		D T("IDENTIFIER")										S(enumerator1)
		O T("IDENTIFIER") T("=") N("constant_expression")		S(enumerator2)
		E

	/*
	type_qualifier
		: CONST
		| VOLATILE
		;
	*/
	P("type_qualifier")
		D T("CONST")		S(type_qualifier1)
		O T("VOLATILE")	S(type_qualifier2)
		E

	/*
	declarator
		: pointer direct_declarator
		| direct_declarator
		;
	*/
	P("declarator")
		D N("pointer") N("direct_declarator")	S(declarator1)
		O N("direct_declarator")					S(declarator2)
		E

	/*
	direct_declarator
		: IDENTIFIER
		| '(' declarator ')'
		| direct_declarator '[' constant_expression ']'
		| direct_declarator '[' ']'
		| direct_declarator '(' parameter_type_list ')'
		| direct_declarator '(' identifier_list ')'
		| direct_declarator '(' ')'
		;
	*/
	P("direct_declarator")
		D T("IDENTIFIER")																							S(direct_declarator1)
		O T("(") N("declarator") T(")")																			S(direct_declarator2)
		O N("direct_declarator") T("[") N("constant_expression") T("]")											S(direct_declarator3)
		O N("direct_declarator") T("[") T("]")																	S(direct_declarator4)
		O N("direct_declarator") N("parameter_list_start") N("parameter_type_list") N("parameter_list_end")	S(direct_declarator5)
		O N("direct_declarator") N("parameter_list_start") N("identifier_list") N("parameter_list_end")		S(direct_declarator6)
		O N("direct_declarator") T("(") T(")")																	S(direct_declarator7)
		E

	/*
	parameter_list_start *
		: '('
		;
	*/
	P("parameter_list_start")
		D T("(")		 S(parameter_list_start1)
		E

	/*
	parameter_list_end *
		: ')'
		;
	*/
	P("parameter_list_end")
		D T(")")		S(parameter_list_end1)
		E

	/*
	pointer
		: '*'
		| '*' type_qualifier_list
		| '*' pointer
		| '*' type_qualifier_list pointer
		;
	*/
	P("pointer")
		D T("*")											S(pointer1)
		O T("*") N("type_qualifier_list")					S(pointer2)
		O T("*") N("pointer")								S(pointer3)
		O T("*") N("type_qualifier_list") N("pointer")	S(pointer4)
		E

	/*
	type_qualifier_list
		: type_qualifier
		| type_qualifier_list type_qualifier
		;
	*/
	P("type_qualifier_list")
		D N("type_qualifier")								S(type_qualifier_list1)
		O N("type_qualifier_list") N("type_qualifier")	S(type_qualifier_list2)
		E

	/*
	parameter_type_list
		: parameter_list
		| parameter_list ',' ELLIPSIS
		;
	*/
	P("parameter_type_list")
		D N("parameter_list")								S(parameter_type_list1)
		O N("parameter_list") T(",") T("ELLIPSIS")		S(parameter_type_list2)
		E

	/*
	parameter_list
		: parameter_declaration
		| parameter_list ',' parameter_declaration
		;
	*/
	P("parameter_list")
		D N("parameter_declaration")									S(parameter_list1)
		O N("parameter_list") T(",") N("parameter_declaration")		S(parameter_list2)
		E

	/*
	parameter_declaration
		: declaration_specifiers declarator
		| declaration_specifiers abstract_declarator
		| declaration_specifiers
		;
	*/
	P("parameter_declaration")
		D N("declaration_specifiers") N("declarator")				S(parameter_declaration1)
		O N("declaration_specifiers") N("abstract_declarator")		S(parameter_declaration2)
		O N("declaration_specifiers")									S(parameter_declaration3)
		E

	/*
	identifier_list
		: IDENTIFIER
		| identifier_list ',' IDENTIFIER
		;
	*/
	P("identifier_list")
		D T("IDENTIFIER")												S(identifier_list1)
		O N("identifier_list") T(",") T("IDENTIFIER")					S(identifier_list2)
		E

	/*
	type_name
		: specifier_qualifier_list
		| specifier_qualifier_list abstract_declarator
		;
	*/
	P("type_name")
		D N("specifier_qualifier_list")								S(type_name1)
		O N("specifier_qualifier_list") N("abstract_declarator")		S(type_name2)
		E

	/*
	abstract_declarator
		: pointer
		| direct_abstract_declarator
		| pointer direct_abstract_declarator
		;
	*/
	P("abstract_declarator")
		D N("pointer")												S(abstract_declarator1)
		O N("direct_abstract_declarator")								S(abstract_declarator2)
		O N("pointer") N("direct_abstract_declarator")				S(abstract_declarator3)
		E

	/*
	direct_abstract_declarator
		: '(' abstract_declarator ')'
		| '[' ']'
		| '[' constant_expression ']'
		| direct_abstract_declarator '[' ']'
		| direct_abstract_declarator '[' constant_expression ']'
		| '(' ')'
		| '(' parameter_type_list ')'
		| direct_abstract_declarator '(' ')'
		| direct_abstract_declarator '(' parameter_type_list ')'
		;
	*/
	P("direct_abstract_declarator")
		D T("(") N("abstract_declarator") T(")")																			S(direct_abstract_declarator1)
		O T("[") T("]")																									S(direct_abstract_declarator2)
		O T("[") N("constant_expression") T("]")																			S(direct_abstract_declarator3)
		O N("direct_abstract_declarator") T("[") T("]")																	S(direct_abstract_declarator4)
		O N("direct_abstract_declarator") T("[") N("constant_expression") T("]")											S(direct_abstract_declarator5)
		O T("(") T(")")																									S(direct_abstract_declarator6)
		O N("parameter_list_start") N("parameter_type_list") N("parameter_list_end")									S(direct_abstract_declarator7)
		O N("direct_abstract_declarator") T("(") T(")")																	S(direct_abstract_declarator8)
		O N("direct_abstract_declarator") N("parameter_list_start") N("parameter_type_list") N("parameter_list_end")	S(direct_abstract_declarator9)
		E

	/*
	initializer
		: assignment_expression
		| '{' initializer_list '}'
		| '{' initializer_list ',' '}'
		;
	*/
	P("initializer")
		D N("assignment_expression")						S(initializer1)
		O T("{") N("initializer_list") T("}")				S(initializer2)
		O T("{") N("initializer_list") T(",") T("}")		S(initializer3)
		E

	/*
	initializer_list
		: initializer
		| initializer_list ',' initializer
		;
	*/
	P("initializer_list")
		D N("initializer")								S(initializer_list1)
		O N("initializer_list") T(",") N("initializer")	S(initializer_list2)
		E

	/*
	statement
		: labeled_statement
		| compound_statement
		| expression_statement
		| selection_statement
		| iteration_statement
		| jump_statement
		;
	*/
	P("statement")
		D N("labeled_statement")		S(statement1)
		O N("compound_statement")		S(statement2)
		O N("expression_statement")	S(statement3)
		O N("selection_statement")	S(statement4)
		O N("iteration_statement")	S(statement5)
		O N("jump_statement")			S(statement6)
		E

	/*
	labeled_statement
		: IDENTIFIER ':' statement
		| CASE constant_expression ':' statement
		| DEFAULT ':' statement
		;
	*/
	P("labeled_statement")
		D T("IDENTIFIER") T(":") N("statement")						S(labeled_statement1)
		O T("CASE") N("constant_expression") T(":") N("statement")	S(labeled_statement2)
		O T("DEFAULT") T(":") N("statement")							S(labeled_statement3)
		E

	/*
	compound_statement
		: '{' '}'
		| '{' statement_list '}'
		| '{' declaration_list '}'
		| '{' declaration_list statement_list '}'
		;
	*/
	P("compound_statement")
		D N("block_start") N("block_end")												S(compound_statement1)
		O N("block_start") N("statement_list") N("block_end")							S(compound_statement2)
		O N("block_start") N("declaration_list") N("block_end")							S(compound_statement3)
		O N("block_start") N("declaration_list") N("statement_list") N("block_end")	S(compound_statement4)
		E

	/*
	block_start *
		: '{'
		;
	*/
	P("block_start")
		D T("{")     S(block_start1)
		E

	/*
	block_end *
		: '}'
		;
	*/
		P("block_end")
		D T("}")     S(block_end1)
		E

	/*
	declaration_list
		: declaration
		| declaration_list declaration
		;
	*/
	P("declaration_list")
		D N("declaration")								S(declaration_list1)
		O N("declaration_list") N("declaration")			S(declaration_list2)
		E

	/*
	statement_list
		: statement
		| statement_list statement
		;
	*/
	P("statement_list")
		D N("statement")									S(statement_list1)
		O N("statement_list") N("statement")				S(statement_list2)
		E

	/*
	expression_statement
		: ';'
		| expression ';'
		;
	*/
	P("expression_statement")
		D T(";")											S(expression_statement1)
		O N("expression") T(";")							S(expression_statement2)
		E

	/*
	selection_statement
		: IF '(' expression ')' statement
		| IF '(' expression ')' statement ELSE statement
		| SWITCH '(' expression ')' statement
		;
	*/
	P("selection_statement")
		D T("IF") T("(") N("expression") T(")") N("statement")							S(selection_statement1)
		O T("IF") T("(") N("expression") T(")") N("statement") T("ELSE") N("statement") S(selection_statement2)
		O T("SWITCH") T("(") N("expression") T(")") N("statement")						S(selection_statement3)
		E
	/*
	iteration_statement
		: WHILE '(' expression ')' statement
		| DO statement WHILE '(' expression ')' ';'
		| FOR '(' expression_statement expression_statement ')' statement
		| FOR '(' expression_statement expression_statement expression ')' statement
		;
	*/
	P("iteration_statement")
		D T("WHILE") T("(") N("expression") T(")") N("statement")														S(iteration_statement1)
		O T("DO") N("statement") T("WHILE") T("(") N("expression") T(")") T(";")											S(iteration_statement2)
		O T("FOR") T("(") N("expression_statement") N("expression_statement") T(")") N("statement")						S(iteration_statement3)
		O T("FOR") T("(") N("expression_statement") N("expression_statement") N("expression") T(")") N("statement")	S(iteration_statement4)
		E

	/*
	jump_statement
		: GOTO IDENTIFIER ';'
		| CONTINUE ';'
		| BREAK ';'
		| RETURN ';'
		| RETURN expression ';'
		;
	*/
	P("jump_statement")
		D T("GOTO") T("IDENTIFIER") T(";")			S(jump_statement1)
		O T("CONTINUE") T(";")						S(jump_statement2)
		O T("BREAK") T(";")						S(jump_statement3)
		O T("RETURN") T(";")						S(jump_statement4)
		O T("RETURN") N("expression") T(";")		S(jump_statement5)
		E

	/*
	translation_unit
		: external_declaration
		| translation_unit external_declaration
		;
	*/
	P("translation_unit")
		D N("external_declaration")							S(translation_unit1)
		O N("translation_unit") N("external_declaration")	S(translation_unit2)
		E

	/*
	external_declaration
		: function_definition
		| declaration
		;
	*/
	P("external_declaration")
		D N("function_definition")		S(external_declaration1)
		O N("declaration")				S(external_declaration2)
		E
	/*
	function_definition
		: declaration_specifiers declarator declaration_list compound_statement
		| declaration_specifiers declarator compound_statement
		| declarator declaration_list compound_statement
		| declarator compound_statement
		;
	*/
	P("function_definition")
		D N("declaration_specifiers") N("declarator") N("declaration_list") N("compound_statement")	S(function_definition1)
		O N("declaration_specifiers") N("declarator") N("compound_statement")							S(function_definition2)
		O N("declarator") N("declaration_list") N("compound_statement")									S(function_definition3)
		O N("declarator") N("compound_statement")														S(function_definition4)
		E

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

}