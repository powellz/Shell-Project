/*
 *
 * CS-252 Fall 2013
 * shell.l: lexical analyzer for shell
 * You have to extend it.
 *
 */

%{

#include <string.h>
#include <unistd.h>
#include "y.tab.h"

////////////  Start added code ///////////


extern "C" {char * read_line();}
char * shdir = strdup("/proc/self/exe");

int mygetc(FILE * f) {

	static char *p;

	char ch;

	if (!isatty(0)) {

	  // stdin is not a tty. Call real getc

	  return getc(f);

	}

	// stdin is a tty. Call our read_line.

	if (p==NULL || *p == 0) {

	  char * s = read_line();

	  p = s;

	}

	ch = *p;

	p++;

	return ch;

	}

#undef getc

#define getc(f) mygetc(f)

/////////// End added code ///////////

static  void yyunput (int c,char *buf_ptr);

void myunputc(int c) {
    unput(c);
}

%}

%%

\n 	{
		return NEWLINE;
	}

[ \t] 	{
	/* Discard spaces and tabs */
    }	

">" {
		return GREAT;
	}

"<"	{
		return LESS;
	}

">>"	{
		return GREATGREAT;
	}

">&"	{
		return GREATAMPERSAND;
	}

"|"	{
		return PIPE;
	}

"&"	{
		return AMPERSAND;
	}

">>&" 	{
		return GREATGREATAMPERSAND;
	}

\"[^\n\"]*\" {
    char* remquotes = yytext;
    remquotes++;
    yytext = remquotes;
    yytext[strlen(yytext)-1] = '\0';
    yylval.string_val = strdup(yytext);	
    return WORD;
}

[^ \t\n(|)(>)(<)(&)]*(\\[^\s])[^ \t\n]*	{
    char* newyytext = (char*)malloc(strlen(yytext) * 2);
    int a = 0;
    for (int b = 0; b < strlen(yytext); b++) {
        if (yytext[b] == '\\') {
            if (yytext[++b] != '\\') {
                newyytext[a++] = yytext[b];
            } else {
                newyytext[a++] = '\\';
                b++;
            }
        } else {
            newyytext[a++] = yytext[b];
        }
    }
    newyytext[a] = '\0';
    yylval.string_val = newyytext;
    return WORD;
}

[^ \t\n(|)(>)(<)][^ \t\n(|)(>)(<)]*  {
    /* Assume that file names have only alpha chars */
    yylval.string_val = strdup(yytext);
    return WORD;
}

`[^\n`]*` {
    yytext++;
    char * remtick = strdup(yytext);
    remtick[strlen(yytext) - 1] = '\0';
    yytext--;

    int defaultin = dup(0);
    int defaultout = dup(1);

    int fdpipe1[2];
    int fdpipe2[2];
    if (pipe(fdpipe1) < 0)
	{
		perror("pipe1");
		_exit(1);
	}
    if (pipe(fdpipe2) < 0)
	{
		perror("pipe2");
		_exit(1);
	}

    write(fdpipe1[1], remtick, strlen(remtick));
    write(fdpipe1[1], "\nexit\n", 6);
	if (close(fdpipe1[1]) < 0)
	{
		perror("pipe 11");
		_exit(1);
	}

	if (dup2(fdpipe1[0], 0) < 0)
	{
		perror("pipe 100");
		_exit(1);
	}
	if (dup2(fdpipe2[1], 1) < 0)
	{
		perror("pipe 211");
		_exit(1);
	}
	if (close(fdpipe1[0]) < 0)
	{
		perror("pipe 10");
		_exit(1);
	}
	if (close(fdpipe2[1]) < 0)
	{
		perror("pipe 21");
		_exit(1);
	}

	int ret = fork();
	if (ret == 0) {
		execvp(shdir, NULL);
		_exit(1);
	} else if (ret < 0) {
		perror("fork");
		exit(1);
	}

	if (dup2(defaultin, 0) < 0)
	{
		perror("dup defaultin");
		_exit(1);
	}
	if (dup2(defaultout, 1) < 0)
	{
		perror("dup defaultout");
		_exit(1);
	}
	if (close(defaultin) < 0)
	{
		perror("close defaultin");
		_exit(1);
	}
	if (close(defaultout) < 0)
	{
		perror("close defaultout");
		_exit(1);
	}

	char tempchar;
	int a = 0;
	char * buf = (char *) malloc (128 * sizeof(char));
	
	while (read(fdpipe2[0], &tempchar, 1)) {
		if (tempchar == '\n')
		{
			buf[a] = ' ';
		}
		else
		{
			buf[a] = tempchar;
		}
		a++;
	}
	buf[a] = '\0';

	for (int b = 0; b < strlen(buf); b++) {
		unput(buf[strlen(buf) - b - 1]);
	}
}

. {
    /* Invalid character in input */
    return NOTOKEN;
}

%%
