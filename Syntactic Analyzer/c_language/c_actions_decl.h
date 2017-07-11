

#include "../src/grammar.h"

namespace C_language {

using namespace syntacticanalyzer;

//primary_expression

SEMANTIC_ACTION_C(primary_expression1, identifier);
SEMANTIC_ACTION_C(primary_expression2, constant);
SEMANTIC_ACTION_C(primary_expression3, string_literal);
SEMANTIC_ACTION_C(primary_expression4, '(' expression ')');



//postfix_expression

SEMANTIC_ACTION_C(postfix_expression1, primary_expression);
SEMANTIC_ACTION_C(postfix_expression2, postfix_expression '[' expression ']');
SEMANTIC_ACTION_C(postfix_expression3, postfix_expression '(' ')');
SEMANTIC_ACTION_C(postfix_expression4, postfix_expression '(' argument_expression_list ')');
SEMANTIC_ACTION_C(postfix_expression5, postfix_expression struct_member_accessor IDENTIFIER);
SEMANTIC_ACTION_C(postfix_expression6, postfix_expression INC_OP);
SEMANTIC_ACTION_C(postfix_expression7, postfix_expression DEC_OP);



//struct_member_accessor

SEMANTIC_ACTION_C(struct_member_accessor1, '.');
SEMANTIC_ACTION_C(struct_member_accessor2, PTR_OP);



//argument_expression_list

SEMANTIC_ACTION_C(argument_expression_list1, assignment_expression);
SEMANTIC_ACTION_C(argument_expression_list2, argument_expression_list ',' assignment_expression);



//unary_expression

SEMANTIC_ACTION_C(unary_expression1, postfix_expression);
SEMANTIC_ACTION_C(unary_expression2, INC_OP unary_expression);
SEMANTIC_ACTION_C(unary_expression3, DEC_OP unary_expression);
SEMANTIC_ACTION_C(unary_expression4, unary_operator cast_expression);
SEMANTIC_ACTION_C(unary_expression5, SIZEOF unary_expression);
SEMANTIC_ACTION_C(unary_expression6, SIZEOF '(' type_name ')');



//unary_operator

SEMANTIC_ACTION_C(unary_operator1, '&');
SEMANTIC_ACTION_C(unary_operator2, '*');
SEMANTIC_ACTION_C(unary_operator3, '+');
SEMANTIC_ACTION_C(unary_operator4, '-');
SEMANTIC_ACTION_C(unary_operator5, '~');
SEMANTIC_ACTION_C(unary_operator6, '!');



//cast_expression

SEMANTIC_ACTION_C(cast_expression1, unary_expression);
SEMANTIC_ACTION_C(cast_expression2, '(' type_name ')' cast_expression);



//multiplicative_expression

SEMANTIC_ACTION_C(multiplicative_expression1, cast_expression);
SEMANTIC_ACTION_C(multiplicative_expression2, multiplicative_expression '*' cast_expression);
SEMANTIC_ACTION_C(multiplicative_expression3, multiplicative_expression '/' cast_expression);
SEMANTIC_ACTION_C(multiplicative_expression4, multiplicative_expression '%' cast_expression);



//additive_expression

SEMANTIC_ACTION_C(additive_expression1, multiplicative_expression);
SEMANTIC_ACTION_C(additive_expression2, additive_expression '+' multiplicative_expression);
SEMANTIC_ACTION_C(additive_expression3, additive_expression '-' multiplicative_expression);



//shift_expression

SEMANTIC_ACTION_C(shift_expression1, additive_expression);
SEMANTIC_ACTION_C(shift_expression2, shift_expression LEFT_OP additive_expression);
SEMANTIC_ACTION_C(shift_expression3, shift_expression RIGHT_OP additive_expression);



//relational_expression

SEMANTIC_ACTION_C(relational_expression1, shift_expression);
SEMANTIC_ACTION_C(relational_expression2, relational_expression '<' shift_expression);
SEMANTIC_ACTION_C(relational_expression3, relational_expression '>' shift_expression);
SEMANTIC_ACTION_C(relational_expression4, relational_expression LE_OP shift_expression);
SEMANTIC_ACTION_C(relational_expression5, relational_expression GE_OP shift_expression);



//equality_expression

SEMANTIC_ACTION_C(equality_expression1, relational_expression);
SEMANTIC_ACTION_C(equality_expression2, equality_expression EQ_OP relational_expression);
SEMANTIC_ACTION_C(equality_expression3, equality_expression NE_OP relational_expression);



//and_expression

SEMANTIC_ACTION_C(and_expression1, equality_expression);
SEMANTIC_ACTION_C(and_expression2, and_expression '&' equality_expression);



//exclusive_or_expression

SEMANTIC_ACTION_C(exclusive_or_expression1, and_expression);
SEMANTIC_ACTION_C(exclusive_or_expression2, exclusive_or_expression '^' and_expression);



//inclusive_or_expression

SEMANTIC_ACTION_C(inclusive_or_expression1, exclusive_or_expression);
SEMANTIC_ACTION_C(inclusive_or_expression2, inclusive_or_expression '|' exclusive_or_expression);



//logical_and_expression

SEMANTIC_ACTION_C(logical_and_expression1, inclusive_or_expression);
SEMANTIC_ACTION_C(logical_and_expression2, logical_and_expression AND_OP inclusive_or_expression);



//logical_or_expression

SEMANTIC_ACTION_C(logical_or_expression1, logical_and_expression);
SEMANTIC_ACTION_C(logical_or_expression2, logical_or_expression OR_OP logical_and_expression);



//conditional_expression

SEMANTIC_ACTION_C(conditional_expression1, logical_or_expression);
SEMANTIC_ACTION_C(conditional_expression2, logical_or_expression '?' expression ':' conditional_expression);



//assignment_expression

SEMANTIC_ACTION_C(assignment_expression1, conditional_expression);
SEMANTIC_ACTION_C(assignment_expression2, unary_expression assignment_operator assignment_expression);



//assignment_operator

SEMANTIC_ACTION_C(assignment_operator1, '=');
SEMANTIC_ACTION_C(assignment_operator2, MUL_ASSIGN);
SEMANTIC_ACTION_C(assignment_operator3, DIV_ASSIGN);
SEMANTIC_ACTION_C(assignment_operator4, MOD_ASSIGN);
SEMANTIC_ACTION_C(assignment_operator5, ADD_ASSIGN);
SEMANTIC_ACTION_C(assignment_operator6, SUB_ASSIGN);
SEMANTIC_ACTION_C(assignment_operator7, LEFT_ASSIGN);
SEMANTIC_ACTION_C(assignment_operator8, RIGHT_ASSIGN);
SEMANTIC_ACTION_C(assignment_operator9, AND_ASSIGN);
SEMANTIC_ACTION_C(assignment_operator10, XOR_ASSIGN);
SEMANTIC_ACTION_C(assignment_operator11, OR_ASSIGN);



//expression

SEMANTIC_ACTION_C(expression1, assignment_expression);
SEMANTIC_ACTION_C(expression2, expression ',' assignment_expression);



//constant_expression

SEMANTIC_ACTION_C(constant_expression1, conditional_expression);



//declaration

SEMANTIC_ACTION_C(declaration1, declaration_specifiers ';');
SEMANTIC_ACTION_C(declaration2, declaration_specifiers init_declarator_list ';');



//declaration_specifiers

SEMANTIC_ACTION_C(declaration_specifiers1, storage_class_specifier);
SEMANTIC_ACTION_C(declaration_specifiers2, storage_class_specifier declaration_specifiers);
SEMANTIC_ACTION_C(declaration_specifiers3, type_specifier);
SEMANTIC_ACTION_C(declaration_specifiers4, type_specifier declaration_specifiers);
SEMANTIC_ACTION_C(declaration_specifiers5, type_qualifier);
SEMANTIC_ACTION_C(declaration_specifiers6, type_qualifier declaration_specifiers);



//init_declarator_list

SEMANTIC_ACTION_C(init_declarator_list1, init_declarator);
SEMANTIC_ACTION_C(init_declarator_list2, init_declarator_list ',' init_declarator);



//init_declarator

SEMANTIC_ACTION_C(init_declarator1, declarator);
SEMANTIC_ACTION_C(init_declarator2, declarator '=' initializer);



//storage_class_specifier

SEMANTIC_ACTION_C(storage_class_specifier1, TYPEDEF);
SEMANTIC_ACTION_C(storage_class_specifier2, EXTERN);
SEMANTIC_ACTION_C(storage_class_specifier3, STATIC);
SEMANTIC_ACTION_C(storage_class_specifier4, AUTO);
SEMANTIC_ACTION_C(storage_class_specifier5, REGISTER);



//type_specifier

SEMANTIC_ACTION_C(type_specifier1, VOID);
SEMANTIC_ACTION_C(type_specifier2, CHAR);
SEMANTIC_ACTION_C(type_specifier3, SHORT);
SEMANTIC_ACTION_C(type_specifier4, INT);
SEMANTIC_ACTION_C(type_specifier5, LONG);
SEMANTIC_ACTION_C(type_specifier6, FLOAT);
SEMANTIC_ACTION_C(type_specifier7, DOUBLE);
SEMANTIC_ACTION_C(type_specifier8, SIGNED);
SEMANTIC_ACTION_C(type_specifier9, UNSIGNED);
SEMANTIC_ACTION_C(type_specifier10, struct_or_union_specifier);
SEMANTIC_ACTION_C(type_specifier11, enum_specifier);
SEMANTIC_ACTION_C(type_specifier12, TYPE_NAME);



//struct_or_union_specifier

SEMANTIC_ACTION_C(struct_or_union_specifier1, struct_or_union struct_tag_opt '{' struct_declaration_list '}');
SEMANTIC_ACTION_C(struct_or_union_specifier2, struct_or_union IDENTIFIER);



//struct_tag_opt

SEMANTIC_ACTION_C(struct_tag_opt1, IDENTIFIER);
SEMANTIC_ACTION_C(struct_tag_opt2, E);




//struct_definition_start

SEMANTIC_ACTION_C(struct_definition_start1, '{');



//struct_definition_end

SEMANTIC_ACTION_C(struct_definition_end1, '}');



//struct_or_union

SEMANTIC_ACTION_C(struct_or_union1, STRUCT);
SEMANTIC_ACTION_C(struct_or_union2, UNION);



//struct_declaration_list

SEMANTIC_ACTION_C(struct_declaration_list1, struct_declaration);
SEMANTIC_ACTION_C(struct_declaration_list2, struct_declaration_list struct_declaration);



//struct_declaration

SEMANTIC_ACTION_C(struct_declaration1, specifier_qualifier_list struct_declarator_list ';');



//specifier_qualifier_list

SEMANTIC_ACTION_C(specifier_qualifier_list1, type_specifier specifier_qualifier_list);
SEMANTIC_ACTION_C(specifier_qualifier_list2, type_specifier);
SEMANTIC_ACTION_C(specifier_qualifier_list3, type_qualifier specifier_qualifier_list);
SEMANTIC_ACTION_C(specifier_qualifier_list4, type_qualifier);



//struct_declarator_list

SEMANTIC_ACTION_C(struct_declarator_list1, struct_declarator);
SEMANTIC_ACTION_C(struct_declarator_list2, struct_declarator_list ',' struct_declarator);



//struct_declarator

SEMANTIC_ACTION_C(struct_declarator1, declarator);
SEMANTIC_ACTION_C(struct_declarator2, ':' constant_expression);
SEMANTIC_ACTION_C(struct_declarator3, declarator ':' constant_expression);



//enum_specifier

SEMANTIC_ACTION_C(enum_specifier1, ENUM '{' enumerator_list '}');
SEMANTIC_ACTION_C(enum_specifier2, ENUM IDENTIFIER '{' enumerator_list '}');
SEMANTIC_ACTION_C(enum_specifier3, ENUM IDENTIFIER);



//enumerator_list

SEMANTIC_ACTION_C(enumerator_list1, enumerator);
SEMANTIC_ACTION_C(enumerator_list2, enumerator_list ',' enumerator);



//enumerator

SEMANTIC_ACTION_C(enumerator1, IDENTIFIER);
SEMANTIC_ACTION_C(enumerator2, IDENTIFIER '=' constant_expression);



//type_qualifier

SEMANTIC_ACTION_C(type_qualifier1, CONST);
SEMANTIC_ACTION_C(type_qualifier2, VOLATILE);



//declarator
SEMANTIC_ACTION_C(declarator1, pointer direct_declarator);
SEMANTIC_ACTION_C(declarator2, direct_declarator);



//direct_declarator

SEMANTIC_ACTION_C(direct_declarator1, IDENTIFIER);
SEMANTIC_ACTION_C(direct_declarator2, '(' declarator ')');
SEMANTIC_ACTION_C(direct_declarator3, direct_declarator '[' constant_expression ']');
SEMANTIC_ACTION_C(direct_declarator4, direct_declarator '[' ']');
SEMANTIC_ACTION_C(direct_declarator5, direct_declarator '(' parameter_type_list ')');
SEMANTIC_ACTION_C(direct_declarator6, direct_declarator '(' identifier_list ')');
SEMANTIC_ACTION_C(direct_declarator7, direct_declarator '(' ')');



//parameter_list_start

SEMANTIC_ACTION_C(parameter_list_start1, '(');



//parameter_list_end

SEMANTIC_ACTION_C(parameter_list_end1, ')');



//pointer

SEMANTIC_ACTION_C(pointer1, '*');
SEMANTIC_ACTION_C(pointer2, '*' type_qualifier_list);
SEMANTIC_ACTION_C(pointer3, '*' pointer);
SEMANTIC_ACTION_C(pointer4, '*' type_qualifier_list pointer);



//type_qualifier_list

SEMANTIC_ACTION_C(type_qualifier_list1, type_qualifier);
SEMANTIC_ACTION_C(type_qualifier_list2, type_qualifier_list type_qualifier);



//parameter_type_list

SEMANTIC_ACTION_C(parameter_type_list1, parameter_list);
SEMANTIC_ACTION_C(parameter_type_list2, parameter_list ',' ELLIPSIS);



//parameter_list

SEMANTIC_ACTION_C(parameter_list1, parameter_declaration);
SEMANTIC_ACTION_C(parameter_list2, parameter_list ',' parameter_declaration);



//parameter_declaration

SEMANTIC_ACTION_C(parameter_declaration1, declaration_specifiers declarator);
SEMANTIC_ACTION_C(parameter_declaration2, declaration_specifiers abstract_declarator);
SEMANTIC_ACTION_C(parameter_declaration3, declaration_specifiers);



//identifier_list

SEMANTIC_ACTION_C(identifier_list1, IDENTIFIER);
SEMANTIC_ACTION_C(identifier_list2, identifier_list ',' IDENTIFIER);



//type_name

SEMANTIC_ACTION_C(type_name1, specifier_qualifier_list);
SEMANTIC_ACTION_C(type_name2, specifier_qualifier_list abstract_declarator);



//abstract_declarator

SEMANTIC_ACTION_C(abstract_declarator1, pointer);
SEMANTIC_ACTION_C(abstract_declarator2, direct_abstract_declarator);
SEMANTIC_ACTION_C(abstract_declarator3, pointer direct_abstract_declarator);



//direct_abstract_declarator

SEMANTIC_ACTION_C(direct_abstract_declarator1, '(' abstract_declarator ')');
SEMANTIC_ACTION_C(direct_abstract_declarator2, '[' ']');
SEMANTIC_ACTION_C(direct_abstract_declarator3, '[' constant_expression ']');
SEMANTIC_ACTION_C(direct_abstract_declarator4, direct_abstract_declarator '[' ']');
SEMANTIC_ACTION_C(direct_abstract_declarator5, direct_abstract_declarator '[' constant_expression ']');
SEMANTIC_ACTION_C(direct_abstract_declarator6, '(' ')');
SEMANTIC_ACTION_C(direct_abstract_declarator7, '(' parameter_type_list ')');
SEMANTIC_ACTION_C(direct_abstract_declarator8, direct_abstract_declarator '(' ')');
SEMANTIC_ACTION_C(direct_abstract_declarator9, direct_abstract_declarator '(' parameter_type_list ')');



//initializer

SEMANTIC_ACTION_C(initializer1, assignment_expression);
SEMANTIC_ACTION_C(initializer2, '{' initializer_list '}');
SEMANTIC_ACTION_C(initializer3, '{' initializer_list ',' '}');



//initializer_list

SEMANTIC_ACTION_C(initializer_list1, initializer);
SEMANTIC_ACTION_C(initializer_list2, initializer_list ',' initializer);



//statement

SEMANTIC_ACTION_C(statement1, labeled_statement);
SEMANTIC_ACTION_C(statement2, compound_statement);
SEMANTIC_ACTION_C(statement3, expression_statement);
SEMANTIC_ACTION_C(statement4, selection_statement);
SEMANTIC_ACTION_C(statement5, iteration_statement);
SEMANTIC_ACTION_C(statement6, jump_statement);



//labeled_statement

SEMANTIC_ACTION_C(labeled_statement1, IDENTIFIER ':' statement);
SEMANTIC_ACTION_C(labeled_statement2, CASE constant_expression ':' statement);
SEMANTIC_ACTION_C(labeled_statement3, DEFAULT ':' statement);


//compound_statement

SEMANTIC_ACTION_C(compound_statement1, '{' '}');
SEMANTIC_ACTION_C(compound_statement2, '{' statement_list '}');
SEMANTIC_ACTION_C(compound_statement3, '{' declaration_list '}');
SEMANTIC_ACTION_C(compound_statement4, '{' declaration_list statement_list '}');



//block_start

SEMANTIC_ACTION_C(block_start1, '{');



//block_end

SEMANTIC_ACTION_C(block_end1, '}');



//declaration_list

SEMANTIC_ACTION_C(declaration_list1, declaration);
SEMANTIC_ACTION_C(declaration_list2, declaration_list declaration);



//statement_list

SEMANTIC_ACTION_C(statement_list1, statement);
SEMANTIC_ACTION_C(statement_list2, statement_list statement);



//expression_statement

SEMANTIC_ACTION_C(expression_statement1, ';');
SEMANTIC_ACTION_C(expression_statement2, expression ';');



//selection_statement

SEMANTIC_ACTION_C(selection_statement1, IF '(' expression ')' statement);
SEMANTIC_ACTION_C(selection_statement2, IF '(' expression ')' statement ELSE statement);
SEMANTIC_ACTION_C(selection_statement3, SWITCH '(' expression ')' statement);



//iteration_statement

SEMANTIC_ACTION_C(iteration_statement1, WHILE '(' expression ')' statement);
SEMANTIC_ACTION_C(iteration_statement2, DO statement WHILE '(' expression ')' ';');
SEMANTIC_ACTION_C(iteration_statement3, FOR '(' expression_statement expression_statement ')' statement);
SEMANTIC_ACTION_C(iteration_statement4, FOR '(' expression_statement expression_statement expression ')' statement);



//jump_statement

SEMANTIC_ACTION_C(jump_statement1, GOTO IDENTIFIER ';');
SEMANTIC_ACTION_C(jump_statement2, CONTINUE ';');
SEMANTIC_ACTION_C(jump_statement3, BREAK ';');
SEMANTIC_ACTION_C(jump_statement4, RETURN ';');
SEMANTIC_ACTION_C(jump_statement5, RETURN expression ';');



//translation_unit

SEMANTIC_ACTION_C(translation_unit1, external_declaration);
SEMANTIC_ACTION_C(translation_unit2, translation_unit external_declaration);



//external_declaration

SEMANTIC_ACTION_C(external_declaration1, function_definition);
SEMANTIC_ACTION_C(external_declaration2, declaration);



//function_definition

SEMANTIC_ACTION_C(function_definition1, declaration_specifiers declarator declaration_list compound_statement);
SEMANTIC_ACTION_C(function_definition2, declaration_specifiers declarator compound_statement);
SEMANTIC_ACTION_C(function_definition3, declarator declaration_list compound_statement);
SEMANTIC_ACTION_C(function_definition4, declarator compound_statement);

}