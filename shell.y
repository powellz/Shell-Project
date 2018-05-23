/*
* CS-252
* shell.y: parser for shell
*
* This parser compiles the following grammar:
*
*	cmd [arg]* [> filename]
*
* you must extend it to understand the complete shell grammar
*
*/

%token	<string_val> WORD

%token 	NOTOKEN GREAT LESS GREATGREAT GREATAMPERSAND NEWLINE PIPE AMPERSAND GREATGREATAMPERSAND

%union	{
    char   *string_val;
}

%{
    //#define yylex yylex
    #include <stdio.h>
    #include <string.h>
    #include <stdlib.h>
    #include <regex.h>
    #include <sys/types.h>
    #include <dirent.h>
    #include <unistd.h>
    #include <signal.h>
    #include <sys/stat.h>
    #include "command.h"
    
    //Debug mode enabled if set to 1. Set to 0 for submission.
    #define DEBUG 0
    #define debug_print(args ...) if (DEBUG) fprintf(stderr, args)
        
    void yyerror(const char * s);
    int yylex();
    void expandWildCardsIfNecessary(char * arg);
    void expandWildcard(char * prefix, char * arg);
    int compstr(const void *file1, const void *file2);
    
    //Entry count used in wildcards.
    int nEntries = 0;
    int maxEntries = 20;
    char **entries = NULL;
    
%}

%%

goal:
command_list1
;

/*commands:
command
| commands command
;

command: simple_command
;

simple_command:
pipe_list io_modifier_list background_optional NEWLINE {
debug_print("   Yacc: Execute command\n");
Command::_currentCommand.execute();
}
| NEWLINE
| error NEWLINE { yyerrok; }
;
*/

command_list1:
command_list2
| command_list1 command_list2
;	/* command loop*/


command_list2: command_line
;

command_line:
pipe_list io_modifier_list background_optional NEWLINE {
    debug_print("   Yacc: Execute command\n");
    Command::_currentCommand.execute();
}
| NEWLINE /*accept empty cmd line*/  {	}
| error NEWLINE{ yyerrok; } /*error recovery*/
;

command_and_args:
command_word argument_list {
    Command::_currentCommand.
    insertSimpleCommand( Command::_currentSimpleCommand );
}
;

argument_list:
argument_list argument
| /* can be empty */
;

argument:
WORD {
    debug_print("   Yacc: insert argument \"%s\"\n", $1);
    
    //Command::_currentSimpleCommand->insertArgument( $1 );
    expandWildCardsIfNecessary($1);
}
;

command_word:
WORD {
    debug_print("   Yacc: insert command \"%s\"\n", $1);
    
    Command::_currentSimpleCommand = new SimpleCommand();
    Command::_currentSimpleCommand->insertArgument( $1 );
}
;

io_modifier_opt:
GREAT WORD {
    debug_print("   Yacc: insert output \"%s\"\n", $2);
    Command::_currentCommand._outFile = $2;
    Command::_currentCommand._outCount++;
}
| GREATGREAT WORD {
    debug_print("   Yacc: insert output \"%s\"\n", $2);
    Command::_currentCommand._outFile = $2;
    Command::_currentCommand._isAppend = 1;
    Command::_currentCommand._outCount++;
}
| GREATAMPERSAND WORD {
    debug_print("   Yacc: insert output \"%s\"\n", $2);
    Command::_currentCommand._outFile = $2;
    Command::_currentCommand._errFile = $2;
    Command::_currentCommand._outCount++;
    Command::_currentCommand._errCount++;
}
| GREATGREATAMPERSAND WORD {
    debug_print("   Yacc: insert output \"%s\"\n", $2);
    Command::_currentCommand._outFile = $2;
    Command::_currentCommand._errFile = $2;
    Command::_currentCommand._isAppend = 1;
    Command::_currentCommand._outCount++;
    Command::_currentCommand._errCount++;
}
| LESS WORD {
    debug_print("   Yacc: insert output \"%s\"\n", $2);
    Command::_currentCommand._inFile = $2;
    Command::_currentCommand._inCount++;
}
;

pipe_list:
pipe_list PIPE command_and_args
| command_and_args
;


io_modifier_list:
io_modifier_list io_modifier_opt
|
;

background_optional:
AMPERSAND
|
;

%%




void expandWildCardsIfNecessary(char * arg)
{
    maxEntries = 20;
    nEntries = 0;
    entries = (char **) malloc (maxEntries * sizeof(char *));
    
    //Return if arg does not contain '*' or '?'
    if (!(strchr(arg, '*') || strchr(arg, '?'))) {
        Command::_currentSimpleCommand->insertArgument(arg);
        return;
    }
    
    expandWildcard(NULL, arg);
    qsort(entries, nEntries, sizeof(char *), compstr);
    for (int i = 0; i < nEntries; i++)
    {
        Command::_currentSimpleCommand->insertArgument(entries[i]);
    }
    return;
}


void expandWildcard(char * prefix, char * arg) {
    
    if (arg[0] == 0) {
        return;
    }
    
    char * argptr = arg;
    char * aclone = (char *) malloc (strlen(arg) + 16*sizeof(char));
    char * acloneptr = aclone;
    
    if (argptr[0] == '/')
    {
        *(aclone) = *(argptr);
        aclone++;
        argptr++;
    }
    
    while (*argptr != '/' && *argptr)
    {
        *(aclone) = *(argptr);
        aclone++;
        argptr++;
    }
    *aclone = '\0';
    
    char * newPrefix = (char *) malloc (128 * sizeof(char));
    if (!(strchr(acloneptr, '*') || strchr(acloneptr, '?'))) {
        if (prefix){
            sprintf(newPrefix, "%s/%s", prefix, acloneptr);
        }
        else {
            newPrefix = strdup(acloneptr);
        }
        
        if (*argptr) {
            argptr++;
            expandWildcard(newPrefix, argptr);
        }
    } else {
        if (prefix == NULL && arg[0] == '/') {
            prefix = strdup("/");
            acloneptr++;
        }
        // Convert “*” -> “.*”
        //         “?” -> “.”
        //         “.” -> “\.”  and others you need
        // Also add ^ at the beginning and $ at the end to match
        // the beginning ant the end of the word.
        // Allocate enough space for regular expression
        char * reg = (char *) malloc (2*strlen(arg) + 10 * sizeof(char));
        char * a = acloneptr;
        char * r = reg;
        
        *(r++) = '^'; // match beginning of line
        while (*a) {
            if (*a == '*') {
                *(r++) = '.';
                *(r++) = '*';
            }
            else if (*a == '?') {
                *(r++) = '.';
            }
            else if (*a == '.') {
                *(r++) = '\\';
                *(r++) = '.';
            }
            else {
                *(r++) = *a;
            }
            a++;
        }
        *(r++) = '$'; *r = '\0'; // match end of line and add null char
        
        // 2. compile regular expression
        regex_t re;
        int expbuf = regcomp( &re, reg, REG_EXTENDED|REG_NOSUB);
        if (expbuf < 0)
        {
            perror("compile");
            return;
        }
        
        // 3. List directory and add as arguments the entries
        // that match the regular expression
        char * dirOpen;
        if (prefix)
            dirOpen = prefix;
        else dirOpen = ".";
            
        DIR * dir = opendir(dirOpen);
        if (dir == NULL) {
            perror("opendir");
            return;
        }
        
        
        struct dirent * ent;
        regmatch_t match;
        
        ent = readdir(dir);
        
        while (ent != NULL) {
            if (regexec(&re, ent->d_name, 1, &match, 0) == 0) {
                if (nEntries == maxEntries)
                {
                    maxEntries *= 2;
                    entries = (char **) realloc (entries, maxEntries * sizeof(char *));
                }
                
                if (*argptr) {
                    //Not implemented
                } else {
                    char * argument = (char *) malloc (128 * sizeof(char));
                    argument[0] = '\0';
                    if (prefix != NULL)
                        sprintf(argument, "%s/%s", prefix, ent->d_name);
                    
                    if (ent->d_name[0] != '.' || arg[0] == '.') {
                        if (argument[0] == '\0')
                            entries[nEntries] = strdup(ent->d_name);
                        else
                            entries[nEntries] = strdup(argument);
                        nEntries++;
                        
                    }
                    
                }
            }
            ent = readdir(dir);
        }
        closedir(dir);
    }
}

int compstr(const void *file1, const void *file2)
{
    return strcmp(*(char *const*)file1, *(char *const*)file2);
}

void
yyerror(const char * s)
{
    fprintf(stderr,"%s", s);
}

#if 0
    main()
{
    yyparse();
}
#endif

