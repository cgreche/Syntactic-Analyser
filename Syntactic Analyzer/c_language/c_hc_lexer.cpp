
//Last Edit: 26/06/2017 03:26

//Hand coded C lexer

#include "../src/grammaranalyzer.h"
#include "../src/lib/nametable.h"

#include "c_language.h"

namespace C_language {

using namespace syntacticanalyzer;

#undef EOF
#define EOF '\0'

#undef UNDEFINED
#define UNDEFINED -1

enum
{
	AUTO = 256,
	BREAK,
	CASE,
	CHAR,
	CONST,
	CONTINUE,
	DEFAULT,
	DO,
	DOUBLE,
	ELSE,
	ENUM,
	EXTERN,
	FLOAT,
	FOR,
	GOTO,
	IF,
	INT,
	LONG,
	REGISTER,
	RETURN,
	SHORT,
	SIGNED,
	SIZEOF,
	STATIC,
	STRUCT,
	SWITCH,
	TYPEDEF,
	UNION,
	UNSIGNED,
	VOID,
	VOLATILE,
	WHILE,

	IDENTIFIER,
	TYPE_NAME,
	CONSTANT,
	STRING_LITERAL,

	ELLIPSIS,
	RIGHT_ASSIGN,
	LEFT_ASSIGN,
	ADD_ASSIGN,
	SUB_ASSIGN,
	MUL_ASSIGN,
	DIV_ASSIGN,
	MOD_ASSIGN,
	AND_ASSIGN,
	XOR_ASSIGN,
	OR_ASSIGN,
	RIGHT_OP,
	LEFT_OP,
	INC_OP,
	DEC_OP,
	PTR_OP,
	AND_OP,
	OR_OP,
	LE_OP,
	GE_OP,
	EQ_OP,
	NE_OP,

	N_C_TOKENS

};

#define TTS(x) \
	do { \
		Symbol *sym = grammar->findSymbol(#x); \
		TOK_TO_SYM[x] = sym ? sym->index() : UNDEFINED; \
	}while(0);

#define GETSYM(x) TOK_TO_SYM[x]

int TOK_TO_SYM[N_C_TOKENS];

void char_to_sym(Grammar *g, char c)
{
	char temp[2];
	temp[0] = c;
	temp[1] = '\0';
	Symbol *sym = g->findSymbol(temp);
	TOK_TO_SYM[c] = sym ? sym->index() : UNDEFINED;
}

extern int g_expectIdentifier;

int c_lexer_onToken_identifier(Token *pToken);

static bool is_lexer_initialized = false;

class c_hcLexer : public Lexer {
	GrammarAnalyzer &m_gramAnalyzer;

public:
	c_hcLexer(GrammarAnalyzer& gramAnalyzer);
	virtual int nextToken(Token *pToken);

	int getch() {
		if(m_input[m_pos] == '\0')
			return '\0';
		return m_input[m_pos++];
	}

	int ungetch() {
		return m_input[--m_pos];
	}
};

c_hcLexer::c_hcLexer(GrammarAnalyzer& gramAnalyzer)
	: m_gramAnalyzer(gramAnalyzer)
{

	Grammar *grammar = gramAnalyzer.grammar();

	int i;
	for(i = 0; i < N_C_TOKENS; ++i)
		TOK_TO_SYM[i] = UNDEFINED;

	TOK_TO_SYM[EOF] = 0; 

	//0 to 255
	char_to_sym(grammar,';');
	char_to_sym(grammar,'{');
	char_to_sym(grammar,'}');
	char_to_sym(grammar,',');
	char_to_sym(grammar,':');
	char_to_sym(grammar,'=');
	char_to_sym(grammar,'(');
	char_to_sym(grammar,')');
	char_to_sym(grammar,'[');
	char_to_sym(grammar,']');
	char_to_sym(grammar,'.');
	char_to_sym(grammar,'&');
	char_to_sym(grammar,'!');
	char_to_sym(grammar,'~');
	char_to_sym(grammar,'-');
	char_to_sym(grammar,'+');
	char_to_sym(grammar,'*');
	char_to_sym(grammar,'/');
	char_to_sym(grammar,'%');
	char_to_sym(grammar,'<');
	char_to_sym(grammar,'>');
	char_to_sym(grammar,'^');
	char_to_sym(grammar,'|');
	char_to_sym(grammar,'?');

	//256+
	TTS(AUTO)
	TTS(BREAK)
	TTS(CASE)
	TTS(CHAR)
	TTS(CONST)
	TTS(CONTINUE)
	TTS(DEFAULT)
	TTS(DO)
	TTS(DOUBLE)
	TTS(ELSE)
	TTS(ENUM)
	TTS(EXTERN)
	TTS(FLOAT)
	TTS(FOR)
	TTS(IF)
	TTS(INT)
	TTS(LONG)
	TTS(REGISTER)
	TTS(RETURN)
	TTS(SHORT)
	TTS(SIGNED)
	TTS(SIZEOF)
	TTS(STATIC)
	TTS(STRUCT)
	TTS(SWITCH)
	TTS(TYPEDEF)
	TTS(UNION)
	TTS(UNSIGNED)
	TTS(VOID)
	TTS(VOLATILE)
	TTS(WHILE)

	TTS(IDENTIFIER)
	TTS(TYPE_NAME)
	TTS(CONSTANT)
	TTS(STRING_LITERAL)

	TTS(ELLIPSIS)
	TTS(RIGHT_ASSIGN)
	TTS(LEFT_ASSIGN)
	TTS(ADD_ASSIGN)
	TTS(SUB_ASSIGN)
	TTS(MUL_ASSIGN)
	TTS(DIV_ASSIGN)
	TTS(MOD_ASSIGN)
	TTS(AND_ASSIGN)
	TTS(XOR_ASSIGN)
	TTS(OR_ASSIGN)
	TTS(RIGHT_OP)
	TTS(LEFT_OP)
	TTS(INC_OP)
	TTS(DEC_OP)
	TTS(PTR_OP)
	TTS(AND_OP)
	TTS(OR_OP)
	TTS(LE_OP)
	TTS(GE_OP)
	TTS(EQ_OP)
	TTS(NE_OP)

	is_lexer_initialized = true;
}


int c_hcLexer::nextToken(Token *pToken)
{
	char __text[256];
	char *t;
	char *text = t = __text;
	char ch;

	if(pToken)
		text = t = pToken->text;

	Grammar *grammar = m_gramAnalyzer.grammar();

#define GETCH (ch = *t++ = this->getch())
#define UNGETCH if(ch != '\0') ch = this->ungetch(); *--t = '\0'

#define IS_DIGIT(x) (x >= '0' && x <= '9')
#define IS_LETTER(x) ((x >= 'a' && x <= 'z') || (x >= 'A' && x <= 'Z') || x == '_')
#define IS_ALNUM(x) (IS_DIGIT(x) || IS_LETTER(x))
#define IS_HEX(x) (IS_DIGIT(x) || (x >= 'a' && x <= 'f') || (x >= 'A' && x <= 'F'))
#define IS_INT_SUFFIX(x) (x == 'u' || x == 'U' || x == 'l' || x == 'L')
#define IS_FLOAT_SUFFIX(x) (x == 'f' || x == 'F' || x == 'l' || x == 'L')
#define IS_WS(x) (x == '\t' || x == '\v' || x == '\n' || x == '\r' || x == '\f' || x == ' ')
#define IS_EOT(x) (IS_WS(x) || x == '\0' || !IS_LETTER(x))

	for(;;) {
		ch = this->getch();
		if(ch == EOF)
			break;

		if(ch == '\t' || ch == '\v' || ch == '\n' || ch == '\r' || ch == '\f' || ch == ' ')
			continue;

		*t++ = ch;

		switch(ch) {
			case '\"':
				goto _string_literal;

			case '0':
				GETCH;
				if(ch == 'x' || ch == 'X') {
					//hex
					GETCH;
					if(IS_HEX(ch)) {
						for(;;) {
							GETCH;
							if(!IS_HEX(ch))
								break;
						}
						if(IS_INT_SUFFIX(ch)) {
							for(;;) {
								GETCH;
								if(!IS_INT_SUFFIX(ch)) {	
									break;
								}
							}
						}
						UNGETCH;
						return GETSYM(CONSTANT);
					}
					//error
					UNGETCH;
					goto _default;
				}
				else if(IS_DIGIT(ch)) {
					//octal
					for(;;) {
						GETCH;
						if(!IS_DIGIT(ch))
							break;
					}
					if(IS_INT_SUFFIX(ch)) {
						for(;;) {
							GETCH;
							if(!IS_INT_SUFFIX(ch)) {
								break;
							}
						}
					}
					UNGETCH;
					return GETSYM(CONSTANT);
				}

				goto _constant_decimal_2;

			case '1': case '2': case '3': case '4': case '5':
			case '6': case '7': case '8': case '9':
				goto _constant_decimal;
			case 'A': case 'B': case 'C': case 'D': case 'E':
			case 'F': case 'G': case 'H': case 'I': case 'J':
			case 'K':
			case 'M': case 'N': case 'O':
			case 'P': case 'Q': case 'R': case 'S': case 'T':
			case 'U': case 'V': case 'W': case 'X': case 'Y':
			case 'Z':
			case '_':
				goto _identifier;

			case 'L':
				GETCH;
				if(ch == '\'') {
					GETCH;
					if(ch == '\'') {
						UNGETCH;
						goto _default;
					}
					else if(ch == '\\') {
						GETCH;
					}
					if(GETCH != '\'') {
						UNGETCH;
						goto _default;
					}
				}else if(ch == '"')
					goto _string_literal;
				goto _identifier2;
			case 'a':
				if(GETCH == 'u' && GETCH == 't' && GETCH == 'o') {
					GETCH;
					if(IS_EOT(ch)) {
						UNGETCH;
						return GETSYM(AUTO);
					}
				}
				goto _identifier2;
			case 'b':
				if(GETCH == 'r' && GETCH == 'e' && GETCH == 'a' && GETCH == 'k') {
					GETCH;
					if(IS_EOT(ch)) {
						UNGETCH;
						return GETSYM(BREAK);
					}
				}
				goto _identifier2;
			case 'c':
				GETCH;
				if(ch == 'a') {
					if(GETCH == 's' && GETCH == 'e') {
						GETCH;
						if(IS_EOT(ch)) {
							UNGETCH;
							return GETSYM(CASE);
						}
					}
				}
				else if(ch == 'h') {
					if(GETCH == 'a' && GETCH == 'r') {
						GETCH;
						if(IS_EOT(ch)) {
							UNGETCH;
							return GETSYM(CHAR);
						}
					}
				}
				else if(ch == 'o') {
					if(GETCH == 'n') {
						GETCH;
						if(ch == 's') {
							if(GETCH == 't') {
								GETCH;
								if(IS_EOT(ch)) {
									UNGETCH;
									return GETSYM(CONST);
								}
							}
						}
						else if(ch == 't') {
							if(GETCH == 'i' && GETCH == 'n' && GETCH == 'u' && GETCH == 'e') {
								GETCH;
								if(IS_EOT(ch)) {
									UNGETCH;
									return GETSYM(CONTINUE);
								}
							}
						}
					}
				}
				goto _identifier2;
			case 'd':
				if(GETCH == 'o') {
					GETCH;
					if(IS_EOT(ch)) {
						UNGETCH;
						return GETSYM(DO);
					}
					if(ch == 'u') {
						if(GETCH == 'b' && GETCH == 'l' && GETCH == 'e') {
							GETCH;
							if(IS_EOT(ch)) {
								UNGETCH;
								return GETSYM(DOUBLE);
							}
						}
					}
				}
				goto _identifier2;
			case 'e':
				GETCH;
				if(ch == 'l') {
					if(GETCH == 's' && GETCH == 'e') {
						GETCH;
						if(IS_EOT(ch)) {
							UNGETCH;
							return GETSYM(ELSE);
						}
					}
				} else if(ch == 'n') {
					if(GETCH == 'u' && GETCH == 'm') {
						GETCH;
						if(IS_EOT(ch)) {
							UNGETCH;
							return GETSYM(ENUM);
						}
					}
				} else if(ch == 'x') {
					if(GETCH == 't' && GETCH == 'e' && GETCH == 'r' && GETCH == 'n') {
						GETCH;
						if(IS_EOT(ch)) {
							UNGETCH;
							return GETSYM(EXTERN);
						}
					}
				}
				goto _identifier2;
			case 'f':
				GETCH;
				if(ch == 'l') {
					if(GETCH == 'o' && GETCH == 'a' && GETCH == 't') {
						GETCH;
						if(IS_EOT(ch)) {
							UNGETCH;
							return GETSYM(FLOAT);
						}
					}
				} else if(ch == 'o') {
					if(GETCH == 'r') {
						GETCH;
						if(IS_EOT(ch)) {
							UNGETCH;
							return GETSYM(FOR);
						}
					}
				}
				goto _identifier2;
			case 'g':
				if(GETCH == 'o' && GETCH == 't' && GETCH == 'o') {
					GETCH;
					if(IS_EOT(ch)) {
						UNGETCH;
						return GETSYM(GOTO);
					}
				}
				goto _identifier2;
			case 'h':
				goto _identifier;
			case 'i':
				GETCH;
				if(ch == 'f') {
					GETCH;
					if(IS_EOT(ch)) {
						UNGETCH;
						return GETSYM(IF);
					}
				} else if(ch == 'n') {
					if(GETCH == 't') {
						GETCH;
						if(IS_EOT(ch)) {
							UNGETCH;
							return GETSYM(INT);
						}
					}
				}				
				goto _identifier2;
			case 'j':
			case 'k':
				goto _identifier;
			case 'l':
				if(GETCH == 'o' && GETCH == 'n' && GETCH == 'g') {
					GETCH;
					if(IS_EOT(ch)) {
						UNGETCH;
						return GETSYM(LONG);
					}
				}
				goto _identifier2;
			case 'm':
			case 'n':
			case 'o':
			case 'p':
			case 'q':
				goto _identifier;
			case 'r':
				if(GETCH == 'e') {
					GETCH;
					if(ch == 'g') {
						if(GETCH == 'i' && GETCH == 's' && GETCH == 't' && GETCH == 'e' && GETCH == 'r') {
							GETCH;
							if(IS_EOT(ch)) {
								UNGETCH;
								return GETSYM(REGISTER);
							}
						}
					} else if(ch == 't') {
						if(GETCH == 'u' && GETCH == 'r' && GETCH == 'n') {
							GETCH;
							if(IS_EOT(ch)) {
								UNGETCH;
								return GETSYM(RETURN);
							}
						}
					}
				}
				goto _identifier2;
			case 's':
				GETCH;
				if(ch == 'h') {
					if(GETCH == 'o' && GETCH == 'r' && GETCH == 't') {
						GETCH;
						if(IS_EOT(ch)) {
							UNGETCH;
							return GETSYM(SHORT);
						}
					}
				}
				else if(ch == 'i') {
					GETCH;
					if(ch == 'g') {
						if(GETCH == 'n' && GETCH == 'e' && GETCH == 'd') {
							GETCH;
							if(IS_EOT(ch)) {
								UNGETCH;
								return GETSYM(SIGNED);
							}
						}
					}
					else if(ch == 'z') {
						if(GETCH == 'e' && GETCH == 'o' && GETCH == 'f') {
							GETCH;
							if(IS_EOT(ch)) {
								UNGETCH;
								return GETSYM(SIZEOF);
							}
						}
					}
				}
				else if(ch == 't') {
					GETCH;
					if(ch == 'a') {
						if(GETCH == 't' && GETCH == 'i' && GETCH == 'c') {
							GETCH;
							if(IS_EOT(ch)) {
								UNGETCH;
								return GETSYM(STATIC);
							}
						}
					}
					else if(ch == 'r') {
						if(GETCH == 'u' && GETCH == 'c' && GETCH == 't') {
							GETCH;
							if(IS_EOT(ch)) {
								UNGETCH;
								return GETSYM(STRUCT);
							}
						}
					}				
				}
				else if(ch == 'w') {
					if(GETCH == 'i' && GETCH == 't' && GETCH == 'c' && GETCH == 'h') {
						GETCH;
						if(IS_EOT(ch)) {
							UNGETCH;
							return GETSYM(SWITCH);
						}
					}
				}
				goto _identifier2;
			case 't':
				if(GETCH == 'y' && GETCH == 'p' && GETCH == 'e' && GETCH == 'd' && GETCH == 'e' && GETCH == 'f') {
					GETCH;
					if(IS_EOT(ch)) {
						UNGETCH;
						return GETSYM(TYPEDEF);
					}
				}
				goto _identifier2;
			case 'u':
				{
					if(GETCH == 'n') {
						GETCH;
						if(ch == 'i') {
							if(GETCH == 'o' && GETCH == 'n') {
								GETCH;
								if(IS_EOT(ch)) {
									UNGETCH;
									return GETSYM(UNION);
								}
							}
						} else if(ch == 's') {
							if(GETCH == 'i' && GETCH == 'g' && GETCH == 'n' && GETCH == 'e' && GETCH == 'd') {
								GETCH;
								if(IS_EOT(ch)) {
									UNGETCH;
									return GETSYM(UNSIGNED);
								}
							}
						}
					}
					goto _identifier2;
				}
			case 'v':
				if(GETCH == 'o') {
					GETCH;
					if(ch == 'i') {
						if(GETCH == 'd') {
							GETCH;
							if(IS_EOT(ch)) {
								UNGETCH;
								return GETSYM(VOID);
							}
						}
					} else if(ch == 'l') {
						if(GETCH == 'a' && GETCH == 't' && GETCH == 'i' && GETCH == 'l' && GETCH == 'e') {
							GETCH;
							if(IS_EOT(ch)) {
								UNGETCH;
								return GETSYM(VOLATILE);
							}
						}
					}
				}
				goto _identifier2;
			case 'w':
				if(GETCH == 'h' && GETCH == 'i' && GETCH == 'l' && GETCH == 'e') {
					GETCH;
					if(IS_EOT(ch)) {
						UNGETCH;
						return GETSYM(WHILE);
					}
				}
				goto _identifier2;
			case 'x':
			case 'y':
			case 'z':
				goto _identifier;

			case '.':
				GETCH;
				if(ch == '.') {
					if(GETCH != '.') {
						UNGETCH;
						goto _default;
					}
					return GETSYM(ELLIPSIS);
				}
				else if(IS_DIGIT(ch))
					goto _constant_float;
				UNGETCH;
				goto _default;
			case '>':
				GETCH;
				if(ch == '>') {
					if(GETCH == '=')
						return GETSYM(RIGHT_ASSIGN);
					UNGETCH;
					return GETSYM(RIGHT_OP);
				}
				else if(ch == '=') {
					return GETSYM(GE_OP);
				}
				UNGETCH;
				goto _default;
			case '<':
				GETCH;
				if(ch == '<') {
					if(GETCH == '=')
						return GETSYM(LEFT_ASSIGN);
					UNGETCH;
					return GETSYM(LEFT_OP);
				}
				else if(ch == '=') {
					return GETSYM(LE_OP);
				}
				UNGETCH;
				goto _default;
			case '+':
				GETCH;
				if(ch == '=') {
					return GETSYM(ADD_ASSIGN);
				}
				else if(ch == '+') {
					return GETSYM(INC_OP);
				}
				UNGETCH;
				goto _default;
			case '-':
				GETCH;
				if(ch == '=') {
					return GETSYM(SUB_ASSIGN);
				}
				else if(ch == '-') {
					return GETSYM(DEC_OP);
				}
				else if(ch == '>') {
					return GETSYM(PTR_OP);
				}
				UNGETCH;
				goto _default;
			case '*':
				if(GETCH == '=') {
					return GETSYM(MUL_ASSIGN);
				}
				UNGETCH;
				goto _default;
			case '/':
				GETCH;
				if(ch == '/') {
					for(;;) {
						GETCH;
						if(ch == '\n' || ch == '\0')
							break;
					}
					break;
				}
				else if(ch == '*')
					goto _comment;
				else if(ch == '=') {
					return GETSYM(DIV_ASSIGN);
				}
				UNGETCH;
				goto _default;
			case '%':
				if(GETCH == '=') {
					return GETSYM(MOD_ASSIGN);
				}
				UNGETCH;
				goto _default;
			case '&':
				GETCH;
				if(ch == '=') {
					return GETSYM(AND_ASSIGN);
				}
				else if(ch == '&') {
					return GETSYM(AND_OP);
				}
				UNGETCH;
				goto _default;
			case '^':
				if(GETCH == '=') {
					return GETSYM(XOR_ASSIGN);
				}
				UNGETCH;
				goto _default;
			case '|':
				GETCH;
				if(ch == '=') {
					return GETSYM(OR_ASSIGN);
				}
				else if(ch == '|') {
					return GETSYM(OR_OP);
				}
				UNGETCH;
				goto _default;
			case '=':
				if(GETCH == '=')
					return GETSYM(EQ_OP);
				UNGETCH;
				goto _default;
			case '!':
				if(GETCH == '=')
					return GETSYM(NE_OP);
				UNGETCH;
				goto _default;
			case ';':
			case '{':
			case ',':
			case ':':
			case '(':
			case ')':
			case '[':
			case ']':
			case '~':
			case '?':
			case ' ':
			case '\t': case '\v': case '\n': case '\f':
			default:
				goto _default;
		}

_comment:
		for(;;) {
			GETCH;
			if(ch == '*') {
				GETCH;
				if(ch == '/')
					continue;
			}

			if(ch == '\0') {
				//error: unterminated comment
				return 0;
			}
		}


		//todo: support '\' chars and '\"' char
		if(ch == '\"')
		{
_string_literal:
			for(;;) {
				GETCH;
				if(ch == '\"' || ch == '\0') {
					break;
				}
			}
			if(ch == '\"') {
//				m_curToken.value.strval,(m_curToken.text+1),strlen(m_curToken.text)-2);
				return GETSYM(STRING_LITERAL);
			}
			UNGETCH;
			goto _default;
		}
		else if(IS_ALNUM(ch)) {
_identifier:
			for(;;) {
				GETCH;
_identifier2:
				if(!IS_ALNUM(ch)) {
					UNGETCH;
					break;
				}
			}
			
			return c_lexer_onToken_identifier(pToken);
		}

		else if(IS_DIGIT(ch))
		{
_constant_decimal:
			for(;;) {
				GETCH;
_constant_decimal_2:
				if(!IS_DIGIT(ch))
					break;
			}
			if(ch == '.')
				goto _constant_float;

			if(IS_INT_SUFFIX(ch)) {
				for(;;) {
					GETCH;
					if(!IS_INT_SUFFIX(ch)) {
						break;
					}
				}
			}
			UNGETCH;

//			aToken.value.lval = atoi(m_curToken.text);
			return GETSYM(CONSTANT);

_constant_float:
			for(;;) {
				GETCH;
				if(!IS_DIGIT(ch))
					break;
			}
			if(ch == 'e' || ch == 'E') {
				GETCH;
				if(ch == '+' || ch == '-') {
					GETCH;
				}
				if(!IS_DIGIT(ch))
					goto _default;
				for(;;) {
					GETCH;
					if(!IS_DIGIT(ch))
						break;
				}
			}
			if(IS_FLOAT_SUFFIX(ch)) {
				for(;;) {
					GETCH;
					if(!IS_FLOAT_SUFFIX(ch)) {
						break;
					}
				}
			}
			UNGETCH;

			return GETSYM(CONSTANT);
		}
		else
		{
_default:
			if(*text == '\0')
				return 0;

			if(strlen(text) == 1)
				return GETSYM(*text);

			Symbol *sym = grammar->findSymbol(text);
			int tok = sym ? sym->index() : -1;
			return tok;
		}


	} //

	return 0;
}

int c_lexer_onToken_identifier(Token *pToken) {
	pToken->tok = GETSYM(IDENTIFIER);
	if(g_expectIdentifier > 0) {
		--g_expectIdentifier;
	}
	else {
		char *text = pToken->text;
		CSymbol *sym = C_curScope()->findSymbol(text);
		pToken->value.pval = sym;
		if(g_expectIdentifier == 0 && sym && sym->isTypedef()) {
			return GETSYM(TYPE_NAME);
		}
	}
	return pToken->tok;
}

}
