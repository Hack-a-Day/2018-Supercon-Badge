#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "tokenizer.h"



#define YYCTYPE		char
#define YYCURSOR	s->_pchCur
#define YYMARKER	s->ptr
#define YYCTXMARKER	s->_pchBackup


typedef struct Scanner
{
	const char* top;			//start of token
	const char* _pchCur;		//cursor; will be just past token when emitting tokens
	const char* ptr;			//used by re2c to store position for backtracking
	const char* pos;			//
	const char* _pchBackup;		//if we need to do backup from a lookahead
	int line;					//line no in source file
	int current_token;
} Scanner;



int scan(Scanner* s)
{
regular:
	if ('\0' == *s->_pchCur)
	{
		return TOKENIZER_ENDOFINPUT;
	}
	s->top = s->_pchCur;


/*!re2c
	re2c:yyfill:enable = 0;	/*buffer contains all text, so 'no'*/

	ALPHANUMS = [a-zA-Z0-9]+;
	whitespace = [ \t\v\f]+;
	dig = [0-9];
	let = [a-zA-Z_];
	hex = [a-fA-F0-9];
	int_des = [uUlL]*;
	any = [\000-\377];
*/

/*!re2c
	dig+			{ return (TOKENIZER_NUMBER); }
	["]([^"]+)["]	{ return (TOKENIZER_STRING); }
	[a-zA-Z]		{ return (TOKENIZER_VARIABLE); }
	"\r\n"|"\n"		{
						s->pos = s->_pchCur;
						s->line++;
						return (TOKENIZER_CR);
					}
	","				{ return (TOKENIZER_COMMA); }
	";"				{ return (TOKENIZER_SEMICOLON); }
	"+"				{ return (TOKENIZER_PLUS); }
	"-"				{ return (TOKENIZER_MINUS); }
	"&"				{ return (TOKENIZER_AND); }
	"|"				{ return (TOKENIZER_OR); }
	"*"				{ return (TOKENIZER_ASTR); }
	"/"				{ return (TOKENIZER_SLASH); }
	"%"				{ return (TOKENIZER_MOD); }
	"("				{ return (TOKENIZER_LEFTPAREN); }
	")"				{ return (TOKENIZER_RIGHTPAREN); }
	"<"				{ return (TOKENIZER_LT); }
	">"				{ return (TOKENIZER_GT); }
	"="				{ return (TOKENIZER_EQ); }

	'let'			{ return (TOKENIZER_LET); }
	'print'			{ return (TOKENIZER_PRINT); }
	'println'		{ return (TOKENIZER_PRINTLN); }
	'if'			{ return (TOKENIZER_IF); }
	'then'			{ return (TOKENIZER_THEN); }
	'else'			{ return (TOKENIZER_ELSE); }
	'for'			{ return (TOKENIZER_FOR); }
	'to'			{ return (TOKENIZER_TO); }
	'next'			{ return (TOKENIZER_NEXT); }
	'goto'			{ return (TOKENIZER_GOTO); }
	'gosub'			{ return (TOKENIZER_GOSUB); }
	'return'		{ return (TOKENIZER_RETURN); }
	'call'			{ return (TOKENIZER_CALL); }
	'end'			{ return (TOKENIZER_END); }
	'out'			{ return (TOKENIZER_OUT); }
	'tune'			{ return (TOKENIZER_TUNE); }
	'termt'			{ return (TOKENIZER_TERMT); }
	'setxy'			{ return (TOKENIZER_SETXY); }
	'clrscr'		{ return (TOKENIZER_CLRSCR); }
	'wait'			{ return (TOKENIZER_WAIT); }
	'led'			{ return (TOKENIZER_LED); }
	'color'			{ return (TOKENIZER_COLOR); }
	'rnd'			{ return (TOKENIZER_RND); }
	'chr'			{ return (TOKENIZER_CHR); }
	'ein'			{ return (TOKENIZER_EIN); }
	'eout'			{ return (TOKENIZER_EOUT); }
	'edr'			{ return (TOKENIZER_EDR); }
	'termup'		{ return (TOKENIZER_TERMUP); }
	'rem'			{ return (TOKENIZER_REM); }
	'uin'			{ return (TOKENIZER_UIN); }
	'uout'			{ return (TOKENIZER_UOUT); }
	'input'			{ return (TOKENIZER_INPUT); }
	'peek'			{ return (TOKENIZER_PEEK); }
	'poke'			{ return (TOKENIZER_POKE); }
	'cursor'		{ return (TOKENIZER_CURSOR); }
	'kin'			{ return (TOKENIZER_KIN); }

	'clr'			{ return (TOKENIZER_CLRSCR); }
	'cls'			{ return (TOKENIZER_CLRSCR); }
	'pnt'			{ return (TOKENIZER_PRINT); }
	'ptl'			{ return (TOKENIZER_PRINTLN); }
	'inp'			{ return (TOKENIZER_INPUT); }
	'sxy'			{ return (TOKENIZER_SETXY); }
	'ret'			{ return (TOKENIZER_RETURN); }
	'cur'			{ return (TOKENIZER_CURSOR); }

	whitespace		{ goto regular; }

	any
	{
		printf("unexpected character: %c\n", *s->_pchCur);
		return (TOKENIZER_ERROR);
	}
*/

}


#if 0
int xxxxTest()
{
	FILE* fp;
	long size;
	char* buff;
	size_t bytes;
	int token = 0;
	Scanner scanner;

	/* Open input file */
	fp = fopen("life8.bas", "rb");
	if(fp == NULL)
	{
		fprintf(stderr, "Can't open test file\n");
		return -1;
	}

	/* Get file size */
	fseek(fp, 0, SEEK_END);
	size = ftell(fp);
	rewind(fp);

	/* Allocate buffer and read */
	buff = (char*) malloc(size * sizeof(char));
	bytes = fread(buff, 1, size, fp);  
	if (bytes != size)
	{
		fprintf(stderr, "Reading error"); 
		return -1;
	}

	/* Start scanning */
	scanner.top = buff;
	scanner._pchCur = buff;
	scanner.pos = buff;
	scanner._pchBackup = buff;
	scanner.line = 1;

	while(token = scan(&scanner))
	{
		switch(token)
		{
			case TOKENIZER_ERROR: printf("TOKENIZER_ERROR\n"); break;
			case TOKENIZER_ENDOFINPUT: printf("TOKENIZER_ENDOFINPUT\n"); break;

			case TOKENIZER_NUMBER:
				printf("#%.*s ", scanner._pchCur - scanner.top, scanner.top);
			break;
			case TOKENIZER_STRING:
				printf("\"%.*s\" ", scanner._pchCur - scanner.top, scanner.top);
			break;
			case TOKENIZER_VARIABLE:
				printf("(\'%.*s\')", scanner._pchCur - scanner.top, scanner.top);
			break;

			case TOKENIZER_LET: printf("TOKENIZER_LET "); break;
			case TOKENIZER_PRINT: printf("TOKENIZER_PRINT "); break;
			case TOKENIZER_IF: printf("TOKENIZER_IF "); break;
			case TOKENIZER_THEN: printf("TOKENIZER_THEN "); break;
			case TOKENIZER_ELSE: printf("TOKENIZER_ELSE "); break;
			case TOKENIZER_FOR: printf("TOKENIZER_FOR "); break;
			case TOKENIZER_TO: printf("TOKENIZER_TO "); break;
			case TOKENIZER_NEXT: printf("TOKENIZER_NEXT "); break;
			case TOKENIZER_GOTO: printf("TOKENIZER_GOTO "); break;
			case TOKENIZER_GOSUB: printf("TOKENIZER_GOSUB "); break;
			case TOKENIZER_RETURN: printf("TOKENIZER_RETURN "); break;
			case TOKENIZER_CALL: printf("TOKENIZER_CALL "); break;
			case TOKENIZER_END: printf("TOKENIZER_END "); break;
			case TOKENIZER_COMMA: printf("TOKENIZER_COMMA "); break;
			case TOKENIZER_SEMICOLON: printf("TOKENIZER_SEMICOLON "); break;
			case TOKENIZER_PLUS: printf("TOKENIZER_PLUS "); break;
			case TOKENIZER_MINUS: printf("TOKENIZER_MINUS "); break;
			case TOKENIZER_AND: printf("TOKENIZER_AND "); break;
			case TOKENIZER_OR: printf("TOKENIZER_OR "); break;
			case TOKENIZER_ASTR: printf("TOKENIZER_ASTR "); break;
			case TOKENIZER_SLASH: printf("TOKENIZER_SLASH "); break;
			case TOKENIZER_MOD: printf("TOKENIZER_MOD "); break;
			case TOKENIZER_LEFTPAREN: printf("TOKENIZER_LEFTPAREN "); break;
			case TOKENIZER_RIGHTPAREN: printf("TOKENIZER_RIGHTPAREN "); break;
			case TOKENIZER_LT: printf("TOKENIZER_LT "); break;
			case TOKENIZER_GT: printf("TOKENIZER_GT "); break;
			case TOKENIZER_EQ: printf("TOKENIZER_EQ "); break;
			case TOKENIZER_CR:
				printf("\n");
			break;
			case TOKENIZER_OUT: printf("TOKENIZER_OUT "); break;
			case TOKENIZER_TUNE: printf("TOKENIZER_TUNE "); break;
			case TOKENIZER_TERMT: printf("TOKENIZER_TERMT "); break;
			case TOKENIZER_SETXY: printf("TOKENIZER_SETXY "); break;
			case TOKENIZER_CLRSCR: printf("TOKENIZER_CLRSCR "); break;
			case TOKENIZER_WAIT: printf("TOKENIZER_WAIT "); break;
			case TOKENIZER_LED: printf("TOKENIZER_LED "); break;
			case TOKENIZER_COLOR: printf("TOKENIZER_COLOR "); break;
			case TOKENIZER_RND: printf("TOKENIZER_RND "); break;
			case TOKENIZER_CHR: printf("TOKENIZER_CHR "); break;
			case TOKENIZER_EIN: printf("TOKENIZER_EIN "); break;
			case TOKENIZER_EOUT: printf("TOKENIZER_EOUT "); break;
			case TOKENIZER_EDR: printf("TOKENIZER_EDR "); break;
			case TOKENIZER_PRINTLN: printf("TOKENIZER_PRINTLN "); break;
			case TOKENIZER_TERMUP: printf("TOKENIZER_TERMUP "); break;
			case TOKENIZER_REM: printf("TOKENIZER_REM "); break;
			case TOKENIZER_UIN: printf("TOKENIZER_UIN "); break;
			case TOKENIZER_UOUT: printf("TOKENIZER_UOUT "); break;
			case TOKENIZER_INPUT: printf("TOKENIZER_INPUT "); break;
			case TOKENIZER_PEEK: printf("TOKENIZER_PEEK "); break;
			case TOKENIZER_POKE: printf("TOKENIZER_POKE "); break;
			case TOKENIZER_CURSOR: printf("TOKENIZER_CURSOR "); break;
			case TOKENIZER_KIN: printf("TOKENIZER_KIN "); break;

			default:
			break;
		}
		if(TOKENIZER_ENDOFINPUT == token)
		{
			break;
		}
	}

	/* Close file and deallocate */
	fclose(fp);
	free(buff);
	return 0;
}
#endif


/*---------------------------------------------------------------------------*/


#define DEBUG 0

#if DEBUG
#define DEBUG_PRINTF(...)  printf(__VA_ARGS__)
#else
#define DEBUG_PRINTF(...)
#endif

Scanner _scanner;


/*---------------------------------------------------------------------------*/
void
tokenizer_init(const char* program)
	{
	/* Start scanning */
	_scanner.top = program;
	_scanner._pchCur = program;
	_scanner.pos = program;
	_scanner._pchBackup = program;
	_scanner.line = 1;

	_scanner.current_token = scan(&_scanner);
	}
/*---------------------------------------------------------------------------*/
int
tokenizer_token(void)
	{
	return _scanner.current_token;
	}
/*---------------------------------------------------------------------------*/
void
tokenizer_next(void)
	{
	if(tokenizer_finished())
		{
		return;
		}
	_scanner.current_token = scan(&_scanner);
	DEBUG_PRINTF("tokenizer_next: '%.*s' %d\n", _scanner._pchCur - _scanner.top, _scanner.top, _scanner.current_token);
	return;
	}
/*---------------------------------------------------------------------------*/
int
tokenizer_num(void)
	{
	return atoi(_scanner.top);
	}
/*---------------------------------------------------------------------------*/
void
tokenizer_string(char *dest, int len)
	{
	int string_len = _scanner._pchCur - _scanner.top;
	//2 because of quote marks we know we have
	string_len -= 2;
	if(len < string_len)
		{
		string_len = len;
		}
	memcpy(dest, _scanner.top+1, string_len);
	dest[string_len] = 0;
	}
/*---------------------------------------------------------------------------*/
void
tokenizer_error_print(void)
	{
	DEBUG_PRINTF("tokenizer_error_print: '%.*s'\n", _scanner._pchCur - _scanner.top, _scanner.top);
	}
/*---------------------------------------------------------------------------*/
int
tokenizer_finished(void)
	{
	return *_scanner.top == 0 || _scanner.current_token == TOKENIZER_ENDOFINPUT;
	}
/*---------------------------------------------------------------------------*/
int
tokenizer_variable_num(void)
	{
	if (*_scanner.top >= 'a' && *_scanner.top <= 'z') return *_scanner.top - 'a';
	else return *_scanner.top - 'A';
	}
/*---------------------------------------------------------------------------*/
const char* __getAt ( void )	//XXX hack for goto cache impl
{
	return _scanner.top;
}
