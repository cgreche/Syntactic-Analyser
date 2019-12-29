
#include "../grammaranalyzer.h"
#include "../language/grammar.h"

#include "c_actions_decl.h"

namespace C_language {

using namespace syntacticanalyzer;

Grammar CGrammar;

Grammar *C_grammar_init()
{
	NonterminalSymbol *__X;
	Production *__P;
	Grammar &grammar = CGrammar;

#define P(lhs) __X = grammar.addNonTerminal(lhs);
#define D __P = grammar.addProduction(__X);
#define T(x) __P->addRHS(grammar.addTerminal(x));
#define NT(x) __P->addRHS(grammar.addNonTerminal(x));
#define OR D
#define E __P->addRHS(&Grammar::epsilon); //insert empty string
#define S(x) __P->setSemanticAction(x);

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
	OR T("CONSTANT") 					S(primary_expression2)
	OR T("STRING_LITERAL")				S(primary_expression3)
	OR T("(") NT("expression") T(")")	S(primary_expression4)

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
	D  NT("primary_expression")													S(postfix_expression1)
	OR NT("postfix_expression") T("[") NT("expression") T("]")					S(postfix_expression2)
	OR NT("postfix_expression") T("(") T(")")									S(postfix_expression3)
	OR NT("postfix_expression") T("(") NT("argument_expression_list") T(")")	S(postfix_expression4)
	OR NT("postfix_expression") NT("struct_member_accessor") T("IDENTIFIER")	S(postfix_expression5)
	OR NT("postfix_expression") T("INC_OP")										S(postfix_expression6)
	OR NT("postfix_expression") T("DEC_OP")										S(postfix_expression7)

	/*
	struct_member_accessor *
		: '.'
		| PTR_OP
		;
	*/
	P("struct_member_accessor")
	D  T(".")			S(struct_member_accessor1)
	OR T("PTR_OP")		S(struct_member_accessor2)

	/*
	argument_expression_list
		: assignment_expression
		| argument_expression_list ',' assignment_expression
		;
	*/
	P("argument_expression_list")
	D  NT("assignment_expression")											S(argument_expression_list1)
	OR NT("argument_expression_list") T(",") NT("assignment_expression")	S(argument_expression_list2)

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
	D  NT("postfix_expression")							S(unary_expression1)
	OR T("INC_OP") NT("unary_expression")				S(unary_expression2)
	OR T("DEC_OP") NT("unary_expression")				S(unary_expression3)
	OR NT("unary_operator") NT("cast_expression")		S(unary_expression4)
	OR T("SIZEOF") NT("unary_expression")				S(unary_expression5)
	OR T("SIZEOF") T("(") NT("type_name") T(")")		S(unary_expression6)

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
	D  T("&")	S(unary_operator1)
	OR T("*")	S(unary_operator2)
	OR T("+")	S(unary_operator3)
	OR T("-")	S(unary_operator4)
	OR T("~")	S(unary_operator5)
	OR T("!")	S(unary_operator6)

	/*
	cast_expression
		: unary_expression
		| '(' type_name ')' cast_expression
		;
	*/
	P("cast_expression")
	D  NT("unary_expression")								S(cast_expression1)
	OR T("(") NT("type_name") T(")") NT("cast_expression")	S(cast_expression2)

	/*
	multiplicative_expression
		: cast_expression
		| multiplicative_expression '*' cast_expression
		| multiplicative_expression '/' cast_expression
		| multiplicative_expression '%' cast_expression
		;
	*/
	P("multiplicative_expression")
	D  NT("cast_expression")												S(multiplicative_expression1)
	OR NT("multiplicative_expression") T("*") NT("cast_expression")			S(multiplicative_expression2)
	OR NT("multiplicative_expression") T("/") NT("cast_expression")			S(multiplicative_expression3)
	OR NT("multiplicative_expression") T("%") NT("cast_expression")			S(multiplicative_expression4)

	/*
	additive_expression
		: multiplicative_expression
		| additive_expression '+' multiplicative_expression
		| additive_expression '-' multiplicative_expression
		;
	*/
	P("additive_expression")
	D  NT("multiplicative_expression")										S(additive_expression1)
	OR NT("additive_expression") T("+") NT("multiplicative_expression")		S(additive_expression2)
	OR NT("additive_expression") T("-") NT("multiplicative_expression")		S(additive_expression3)

	/*
	shift_expression
		: additive_expression
		| shift_expression LEFT_OP additive_expression
		| shift_expression RIGHT_OP additive_expression
		;
	*/
	P("shift_expression")
	D  NT("additive_expression")											S(shift_expression1)
	OR NT("shift_expression") T("LEFT_OP") NT("additive_expression")		S(shift_expression2)
	OR NT("shift_expression") T("RIGHT_OP") NT("additive_expression")		S(shift_expression3)

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
	D  NT("shift_expression")												S(relational_expression1)
	OR NT("relational_expression") T("<") NT("shift_expression")			S(relational_expression2)
	OR NT("relational_expression") T(">") NT("shift_expression")			S(relational_expression3)
	OR NT("relational_expression") T("LE_OP") NT("shift_expression")		S(relational_expression4)
	OR NT("relational_expression") T("GE_OP") NT("shift_expression")		S(relational_expression5)

	/*
	equality_expression
		: relational_expression
		| equality_expression EQ_OP relational_expression
		| equality_expression NE_OP relational_expression
		;
	*/
	P("equality_expression")
	D  NT("relational_expression")											S(equality_expression1)
	OR NT("equality_expression") T("EQ_OP") NT("relational_expression")		S(equality_expression2)
	OR NT("equality_expression") T("NE_OP") NT("relational_expression")		S(equality_expression3)

	/*
	and_expression
		: equality_expression
		| and_expression '&' equality_expression
		;
	*/
	P("and_expression")
	D  NT("equality_expression")											S(and_expression1)
	OR NT("and_expression") T("&") NT("equality_expression")				S(and_expression2)

	/*
	exclusive_or_expression
		: and_expression
		| exclusive_or_expression '^' and_expression
		;
	*/
	P("exclusive_or_expression")
	D  NT("and_expression")													S(exclusive_or_expression1)
	OR NT("exclusive_or_expression") T("^") NT("and_expression")			S(exclusive_or_expression2)

	/*
	inclusive_or_expression
		: exclusive_or_expression
		| inclusive_or_expression '|' exclusive_or_expression
		;
	*/
	P("inclusive_or_expression")
	D  NT("exclusive_or_expression")										S(inclusive_or_expression1)
	OR NT("inclusive_or_expression") T("|") NT("exclusive_or_expression")	S(inclusive_or_expression2)

	/*
	logical_and_expression
		: inclusive_or_expression
		| logical_and_expression AND_OP inclusive_or_expression
		;
	*/
	P("logical_and_expression")
	D  NT("inclusive_or_expression")											S(logical_and_expression1)
	OR NT("logical_and_expression") T("AND_OP") NT("inclusive_or_expression")	S(logical_and_expression2)

	/*
	logical_or_expression
		: logical_and_expression
		| logical_or_expression OR_OP logical_and_expression
		;
	*/
	P("logical_or_expression")
	D  NT("logical_and_expression")												S(logical_or_expression1)
	OR NT("logical_or_expression") T("OR_OP") NT("logical_and_expression")		S(logical_or_expression2)

	/*
	conditional_expression
		: logical_or_expression
		| logical_or_expression '?' expression ':' conditional_expression
		;
	*/
	P("conditional_expression")
	D  NT("logical_or_expression")																S(conditional_expression1)
	OR NT("logical_or_expression") T("?") NT("expression") T(":") NT("conditional_expression")	S(conditional_expression2)

	/*
	assignment_expression
		: conditional_expression
		| unary_expression assignment_operator assignment_expression
		;
	*/
	P("assignment_expression")
	D  NT("conditional_expression")														S(assignment_expression1)
	OR NT("unary_expression") NT("assignment_operator") NT("assignment_expression")		S(assignment_expression2)

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
	D  T("=")							S(assignment_operator1)
	OR T("MUL_ASSIGN")					S(assignment_operator2)
	OR T("DIV_ASSIGN")					S(assignment_operator3)
	OR T("MOD_ASSIGN")					S(assignment_operator4)
	OR T("ADD_ASSIGN")					S(assignment_operator5)
	OR T("SUB_ASSIGN")					S(assignment_operator6)
	OR T("LEFT_ASSIGN")					S(assignment_operator7)
	OR T("RIGHT_ASSIGN")				S(assignment_operator8)
	OR T("AND_ASSIGN")					S(assignment_operator9)
	OR T("XOR_ASSIGN")					S(assignment_operator10)
	OR T("OR_ASSIGN")					S(assignment_operator11)

	/*
	expression
		: assignment_expression
		| expression ',' assignment_expression
		;
	*/
	P("expression")
	D  NT("assignment_expression")							S(assignment_expression1)
	OR NT("expression") T(",") NT("assignment_expression")	S(assignment_expression2)

	/*
	constant_expression
		: conditional_expression
		;
	*/
	P("constant_expression")
	D  NT("conditional_expression")		S(constant_expression1)


	/*
	declaration
		: declaration_specifiers ';'
		| declaration_specifiers init_declarator_list ';'
		;
	*/
	P("declaration")
	D  NT("declaration_specifiers") T(";")								S(declaration1)
	OR NT("declaration_specifiers") NT("init_declarator_list") T(";")	S(declaration2)

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
	D  NT("storage_class_specifier")								S(declaration_specifiers1)
	OR NT("storage_class_specifier") NT("declaration_specifiers")	S(declaration_specifiers2)
	OR NT("type_specifier")											S(declaration_specifiers3)
	OR NT("type_specifier") NT("declaration_specifiers")			S(declaration_specifiers4)
	OR NT("type_qualifier")											S(declaration_specifiers5)
	OR NT("type_qualifier") NT("declaration_specifiers")			S(declaration_specifiers6)

	/*
	init_declarator_list
		: init_declarator
		| init_declarator_list ',' init_declarator
		;
	*/
	P("init_declarator_list")
	D  NT("init_declarator")										S(init_declarator_list1)
	OR NT("init_declarator_list") T(",") NT("init_declarator")		S(init_declarator_list2)

	/*
	init_declarator
		: declarator
		| declarator '=' initializer
		;
	*/
	P("init_declarator")
	D  NT("declarator")										S(init_declarator1)
	OR NT("declarator") T("=") NT("initializer")			S(init_declarator2)

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
	D  T("TYPEDEF")		S(storage_class_specifier1)
	OR T("EXTERN")		S(storage_class_specifier2)
	OR T("STATIC")		S(storage_class_specifier3)
	OR T("AUTO")		S(storage_class_specifier4)
	OR T("REGISTER")	S(storage_class_specifier5)

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
	D  T("VOID")							S(type_specifier1)
	OR T("CHAR")							S(type_specifier2)
	OR T("SHORT")							S(type_specifier3)
	OR T("INT")								S(type_specifier4)
	OR T("LONG")							S(type_specifier5)
	OR T("FLOAT")							S(type_specifier6)
	OR T("DOUBLE")							S(type_specifier7)
	OR T("SIGNED")							S(type_specifier8)
	OR T("UNSIGNED")						S(type_specifier9)
	OR NT("struct_or_union_specifier")		S(type_specifier10)
	OR NT("enum_specifier")					S(type_specifier11)
	OR T("TYPE_NAME")						S(type_specifier12)

	/*
	struct_or_union_specifier
		: struct_or_union IDENTIFIER '{' struct_declaration_list '}'
		| struct_or_union '{' struct_declaration_list '}'
		| struct_or_union IDENTIFIER
		;
	*/
	P("struct_or_union_specifier")
	D  NT("struct_or_union") NT("struct_tag_opt") NT("struct_definition_start") NT("struct_declaration_list") NT("struct_definition_end")	S(struct_or_union_specifier1)
	OR NT("struct_or_union") T("IDENTIFIER")																								S(struct_or_union_specifier2)

	/*
	struct_tag_opt *
		: IDENTIFIER
		|
		;
	*/
	P("struct_tag_opt")
	D  T("IDENTIFIER")		S(struct_tag_opt1)
	OR						S(struct_tag_opt2) 

	/*
	struct_definition_start *
		: '{'
		;
	*/
	P("struct_definition_start")
	D  T("{")		S(struct_definition_start1)

	/*
	struct_definition_end *
		: '}'
		;
	*/
	P("struct_definition_end")
	D  T("}")		S(struct_definition_end1)

	/*
	struct_or_union
		: STRUCT
		| UNION
		;
	*/
	P("struct_or_union")
	D  T("STRUCT")			S(struct_or_union1)
	OR T("UNION")			S(struct_or_union2)

	/*
	struct_declaration_list
		: struct_declaration
		| struct_declaration_list struct_declaration
		;
	*/
	P("struct_declaration_list")
	D  NT("struct_declaration")									S(struct_declaration_list1)
	OR NT("struct_declaration_list") NT("struct_declaration")	S(struct_declaration_list2)

	/*
	struct_declaration
		: specifier_qualifier_list struct_declarator_list ';'
		;
	*/
	P("struct_declaration")
	D  NT("specifier_qualifier_list") NT("struct_declarator_list") T(";")	S(struct_declaration1)

	/*
	specifier_qualifier_list
		: type_specifier specifier_qualifier_list
		| type_specifier
		| type_qualifier specifier_qualifier_list
		| type_qualifier
		;
	*/
	P("specifier_qualifier_list")
	D  NT("type_specifier") NT("specifier_qualifier_list")		S(specifier_qualifier_list1)
	OR NT("type_specifier")										S(specifier_qualifier_list2)
	OR NT("type_qualifier") NT("specifier_qualifier_list")		S(specifier_qualifier_list3)
	OR NT("type_qualifier")										S(specifier_qualifier_list4)

	/*
	struct_declarator_list
		: struct_declarator
		| struct_declarator_list ',' struct_declarator
		;
	*/
	P("struct_declarator_list")
	D  NT("struct_declarator")										S(struct_declarator_list1)
	OR NT("struct_declarator_list") T(",") NT("struct_declarator")	S(struct_declarator_list2)

	/*
	struct_declarator
		: declarator
		| ':' constant_expression
		| declarator ':' constant_expression
		;
	*/
	P("struct_declarator")
	D  NT("declarator")										S(struct_declarator1)
	OR T(":") NT("constant_expression")						S(struct_declarator2)
	OR NT("declarator") T(":") NT("constant_expression")	S(struct_declarator3)

	/*
	enum_specifier
		: ENUM '{' enumerator_list '}'
		| ENUM IDENTIFIER '{' enumerator_list '}'
		| ENUM IDENTIFIER
		;
	*/
	P("enum_specifier")
	D  T("ENUM") T("{") NT("enumerator_list") T("}")					S(enum_specifier1)
	OR T("ENUM") T("IDENTIFIER") T("{") NT("enumerator_list") T("}")	S(enum_specifier2)
	OR T("ENUM") T("IDENTIFIER")										S(enum_specifier3)

	/*
	enumerator_list
		: enumerator
		| enumerator_list ',' enumerator
		;
	*/
	P("enumerator_list")
	D  NT("enumerator")										S(enumerator_list1)
	OR NT("enumerator_list") T(",") NT("enumerator")		S(enumerator_list2)

	/*
	enumerator
		: IDENTIFIER
		| IDENTIFIER '=' constant_expression
		;
	*/
	P("enumerator");
	D  T("IDENTIFIER")										S(enumerator1)
	OR T("IDENTIFIER") T("=") NT("constant_expression")		S(enumerator2)

	/*
	type_qualifier
		: CONST
		| VOLATILE
		;
	*/
	P("type_qualifier")
	D  T("CONST")		S(type_qualifier1)
	OR T("VOLATILE")	S(type_qualifier2)

	/*
	declarator
		: pointer direct_declarator
		| direct_declarator
		;
	*/
	P("declarator")
	D  NT("pointer") NT("direct_declarator")	S(declarator1)
	OR NT("direct_declarator")					S(declarator2)

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
	D  T("IDENTIFIER")																							S(direct_declarator1)
	OR T("(") NT("declarator") T(")")																			S(direct_declarator2)
	OR NT("direct_declarator") T("[") NT("constant_expression") T("]")											S(direct_declarator3)
	OR NT("direct_declarator") T("[") T("]")																	S(direct_declarator4)
	OR NT("direct_declarator") NT("parameter_list_start") NT("parameter_type_list") NT("parameter_list_end")	S(direct_declarator5)
	OR NT("direct_declarator") NT("parameter_list_start") NT("identifier_list") NT("parameter_list_end")		S(direct_declarator6)
	OR NT("direct_declarator") T("(") T(")")																	S(direct_declarator7)

	/*
	parameter_list_start *
		: '('
		;
	*/
	P("parameter_list_start")
	D  T("(")		 S(parameter_list_start1)

	/*
	parameter_list_end *
		: ')'
		;
	*/
	P("parameter_list_end")
	D T(")")		S(parameter_list_end1)

	/*
	pointer
		: '*'
		| '*' type_qualifier_list
		| '*' pointer
		| '*' type_qualifier_list pointer
		;
	*/
	P("pointer")
	D  T("*")											S(pointer1)
	OR T("*") NT("type_qualifier_list")					S(pointer2)
	OR T("*") NT("pointer")								S(pointer3)
	OR T("*") NT("type_qualifier_list") NT("pointer")	S(pointer4)

	/*
	type_qualifier_list
		: type_qualifier
		| type_qualifier_list type_qualifier
		;
	*/
	P("type_qualifier_list")
	D  NT("type_qualifier")								S(type_qualifier_list1)
	OR NT("type_qualifier_list") NT("type_qualifier")	S(type_qualifier_list2)

	/*
	parameter_type_list
		: parameter_list
		| parameter_list ',' ELLIPSIS
		;
	*/
	P("parameter_type_list")
	D  NT("parameter_list")								S(parameter_type_list1)
	OR NT("parameter_list") T(",") T("ELLIPSIS")		S(parameter_type_list2)

	/*
	parameter_list
		: parameter_declaration
		| parameter_list ',' parameter_declaration
		;
	*/
	P("parameter_list")
	D  NT("parameter_declaration")									S(parameter_list1)
	OR NT("parameter_list") T(",") NT("parameter_declaration")		S(parameter_list2)

	/*
	parameter_declaration
		: declaration_specifiers declarator
		| declaration_specifiers abstract_declarator
		| declaration_specifiers
		;
	*/
	P("parameter_declaration")
	D  NT("declaration_specifiers") NT("declarator")				S(parameter_declaration1)
	OR NT("declaration_specifiers") NT("abstract_declarator")		S(parameter_declaration2)
	OR NT("declaration_specifiers")									S(parameter_declaration3)

	/*
	identifier_list
		: IDENTIFIER
		| identifier_list ',' IDENTIFIER
		;
	*/
	P("identifier_list")
	D  T("IDENTIFIER")												S(identifier_list1)
	OR NT("identifier_list") T(",") T("IDENTIFIER")					S(identifier_list2)

	/*
	type_name
		: specifier_qualifier_list
		| specifier_qualifier_list abstract_declarator
		;
	*/
	P("type_name")
	D  NT("specifier_qualifier_list")								S(type_name1)
	OR NT("specifier_qualifier_list") NT("abstract_declarator")		S(type_name2)

	/*
	abstract_declarator
		: pointer
		| direct_abstract_declarator
		| pointer direct_abstract_declarator
		;
	*/
	P("abstract_declarator")
	D  NT("pointer")												S(abstract_declarator1)
	OR NT("direct_abstract_declarator")								S(abstract_declarator2)
	OR NT("pointer") NT("direct_abstract_declarator")				S(abstract_declarator3)

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
	D  T("(") NT("abstract_declarator") T(")")																			S(direct_abstract_declarator1)
	OR T("[") T("]")																									S(direct_abstract_declarator2)
	OR T("[") NT("constant_expression") T("]")																			S(direct_abstract_declarator3)
	OR NT("direct_abstract_declarator") T("[") T("]")																	S(direct_abstract_declarator4)
	OR NT("direct_abstract_declarator") T("[") NT("constant_expression") T("]")											S(direct_abstract_declarator5)
	OR T("(") T(")")																									S(direct_abstract_declarator6)
	OR NT("parameter_list_start") NT("parameter_type_list") NT("parameter_list_end")									S(direct_abstract_declarator7)
	OR NT("direct_abstract_declarator") T("(") T(")")																	S(direct_abstract_declarator8)
	OR NT("direct_abstract_declarator") NT("parameter_list_start") NT("parameter_type_list") NT("parameter_list_end")	S(direct_abstract_declarator9)

	/*
	initializer
		: assignment_expression
		| '{' initializer_list '}'
		| '{' initializer_list ',' '}'
		;
	*/
	P("initializer")
	D  NT("assignment_expression")						S(initializer1)
	OR T("{") NT("initializer_list") T("}")				S(initializer2)
	OR T("{") NT("initializer_list") T(",") T("}")		S(initializer3)

	/*
	initializer_list
		: initializer
		| initializer_list ',' initializer
		;
	*/
	P("initializer_list")
	D  NT("initializer")								S(initializer_list1)
	OR NT("initializer_list") T(",") NT("initializer")	S(initializer_list2)

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
	D  NT("labeled_statement")		S(statement1)
	OR NT("compound_statement")		S(statement2)
	OR NT("expression_statement")	S(statement3)
	OR NT("selection_statement")	S(statement4)
	OR NT("iteration_statement")	S(statement5)
	OR NT("jump_statement")			S(statement6)

	/*
	labeled_statement
		: IDENTIFIER ':' statement
		| CASE constant_expression ':' statement
		| DEFAULT ':' statement
		;
	*/
	P("labeled_statement")
	D  T("IDENTIFIER") T(":") NT("statement")						S(labeled_statement1)
	OR T("CASE") NT("constant_expression") T(":") NT("statement")	S(labeled_statement2)
	OR T("DEFAULT") T(":") NT("statement")							S(labeled_statement3)

	/*
	compound_statement
		: '{' '}'
		| '{' statement_list '}'
		| '{' declaration_list '}'
		| '{' declaration_list statement_list '}'
		;
	*/
	P("compound_statement")
	D  NT("block_start") NT("block_end")												S(compound_statement1)
	OR NT("block_start") NT("statement_list") NT("block_end")							S(compound_statement2)
	OR NT("block_start") NT("declaration_list") NT("block_end")							S(compound_statement3)
	OR NT("block_start") NT("declaration_list") NT("statement_list") NT("block_end")	S(compound_statement4)

	/*
	block_start *
		: '{'
		;
	*/
	P("block_start")
	D  T("{")     S(block_start1)

	/*
	block_end *
		: '}'
		;
	*/
	P("block_end")
	D  T("}")     S(block_end1)

	/*
	declaration_list
		: declaration
		| declaration_list declaration
		;
	*/
	P("declaration_list")
	D  NT("declaration")								S(declaration_list1)
	OR NT("declaration_list") NT("declaration")			S(declaration_list2)

	/*
	statement_list
		: statement
		| statement_list statement
		;
	*/
	P("statement_list")
	D  NT("statement")									S(statement_list1)
	OR NT("statement_list") NT("statement")				S(statement_list2)

	/*
	expression_statement
		: ';'
		| expression ';'
		;
	*/
	P("expression_statement")
	D  T(";")											S(expression_statement1)
	OR NT("expression") T(";")							S(expression_statement2)

	/*
	selection_statement
		: IF '(' expression ')' statement
		| IF '(' expression ')' statement ELSE statement
		| SWITCH '(' expression ')' statement
		;
	*/
	P("selection_statement")
	D  T("IF") T("(") NT("expression") T(")") NT("statement")							S(selection_statement1)
	OR T("IF") T("(") NT("expression") T(")") NT("statement") T("ELSE") NT("statement") S(selection_statement2)
	OR T("SWITCH") T("(") NT("expression") T(")") NT("statement")						S(selection_statement3)

	/*
	iteration_statement
		: WHILE '(' expression ')' statement
		| DO statement WHILE '(' expression ')' ';'
		| FOR '(' expression_statement expression_statement ')' statement
		| FOR '(' expression_statement expression_statement expression ')' statement
		;
	*/
	P("iteration_statement")
	D  T("WHILE") T("(") NT("expression") T(")") NT("statement")														S(iteration_statement1)
	OR T("DO") NT("statement") T("WHILE") T("(") NT("expression") T(")") T(";")											S(iteration_statement2)
	OR T("FOR") T("(") NT("expression_statement") NT("expression_statement") T(")") NT("statement")						S(iteration_statement3)
	OR T("FOR") T("(") NT("expression_statement") NT("expression_statement") NT("expression") T(")") NT("statement")	S(iteration_statement4)

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
	D  T("GOTO") T("IDENTIFIER") T(";")			S(jump_statement1)
	OR T("CONTINUE") T(";")						S(jump_statement2)
	OR T("BREAK") T(";")						S(jump_statement3)
	OR T("RETURN") T(";")						S(jump_statement4)
	OR T("RETURN") NT("expression") T(";")		S(jump_statement5)

	/*
	translation_unit
		: external_declaration
		| translation_unit external_declaration
		;
	*/
	P("translation_unit")
	D  NT("external_declaration")							S(translation_unit1)
	OR NT("translation_unit") NT("external_declaration")	S(translation_unit2)

	/*
	external_declaration
		: function_definition
		| declaration
		;
	*/
	P("external_declaration")
	D  NT("function_definition")		S(external_declaration1)
	OR NT("declaration")				S(external_declaration2)

	/*
	function_definition
		: declaration_specifiers declarator declaration_list compound_statement
		| declaration_specifiers declarator compound_statement
		| declarator declaration_list compound_statement
		| declarator compound_statement
		;
	*/
	P("function_definition")
	D  NT("declaration_specifiers") NT("declarator") NT("declaration_list") NT("compound_statement")	S(function_definition1)
	OR NT("declaration_specifiers") NT("declarator") NT("compound_statement")							S(function_definition2)
	OR NT("declarator") NT("declaration_list") NT("compound_statement")									S(function_definition3)
	OR NT("declarator") NT("compound_statement")														S(function_definition4)

	CGrammar.setStartSymbol((NonterminalSymbol*)CGrammar.findSymbol("translation_unit"));
	return &CGrammar;
}

}