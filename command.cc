/*
* CS252: Shell project
*
* Template file.
* You will need to add more code here to execute the command table.
*
* NOTE: You are responsible for fixing any bugs this code may have!
*
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <pwd.h>
#include "command.h"
int debug = 0;

SimpleCommand::SimpleCommand()
{
    // Create available space for 5 arguments
    _numOfAvailableArguments = 5;
    _numOfArguments = 0;
    _arguments = (char **) malloc( _numOfAvailableArguments * sizeof( char * ) );
}

void
SimpleCommand::insertArgument( char * argument )
{
    if ( _numOfAvailableArguments == _numOfArguments  + 1 ) {
        // Double the available space
        _numOfAvailableArguments *= 2;
        _arguments = (char **) realloc( _arguments,
        _numOfAvailableArguments * sizeof( char * ) );
    }
    
    char * etExp = envTildeExpansion(argument);
    
    if (etExp) {
        argument = strdup(etExp);
    }
    
    _arguments[ _numOfArguments ] = argument;
    
    // Add NULL argument at the end
    _arguments[ _numOfArguments + 1] = NULL;
    
    _numOfArguments++;
}

char * SimpleCommand::envTildeExpansion(char * argument) {
    char * arg = strdup(argument);
    char * replace = (char *) malloc (sizeof(argument) + 1000);
    char * dollarloc = strchr(arg, '$');
    if (dollarloc)
    {
        int b = 0;
        for(int a = 0; arg[a] != '\0'; a++)
        {
            b = 0;
            if (arg[a] != '$')
            {
                char * temp = (char*)malloc(500);
                while(arg[a] != '$')
                {
                    if (arg[a] == '\0')
                        break;
                    temp[b++] = arg[a++];
                }
                temp[b] = '\0';
                strcat(replace, temp);
                a--;
            }
            else
            {
                char * temp = (char*)malloc(500);
                a++;
                a++;
                while(arg[a] != '}')
                {
                    if (arg[a] == '\0')
                        break;
                    temp[b++] = arg[a++];
                }
                temp[b] = '\0';
                strcat(replace, getenv(temp));
            }
        }
        arg = strdup(replace);
    }
    
    //Tilde
    if (arg[0] == '~')
    {
        if (arg[1] == '\0')
        {
            arg = strdup(getenv("HOME"));
        }
        else
        {
            arg = strdup(getpwnam(arg+1)->pw_dir);
        }
    }
    return arg;
}

Command::Command()
{
    // Create available space for one simple command
    _numOfAvailableSimpleCommands = 1;
    _simpleCommands = (SimpleCommand **)
    malloc( _numOfSimpleCommands * sizeof( SimpleCommand * ) );
    
    _numOfSimpleCommands = 0;
    _outFile = 0;
    _inFile = 0;
    _errFile = 0;
    _background = 0;
    _isAppend = 0;
    _inCount = 0;
    _outCount = 0;
    _errCount = 0;
}

void
Command::insertSimpleCommand( SimpleCommand * simpleCommand )
{
    if ( _numOfAvailableSimpleCommands == _numOfSimpleCommands ) {
        _numOfAvailableSimpleCommands *= 2;
        _simpleCommands = (SimpleCommand **) realloc( _simpleCommands,
        _numOfAvailableSimpleCommands * sizeof( SimpleCommand * ) );
    }
    
    _simpleCommands[ _numOfSimpleCommands ] = simpleCommand;
    _numOfSimpleCommands++;
}

void
Command:: clear()
{
    for ( int i = 0; i < _numOfSimpleCommands; i++ ) {
        for ( int j = 0; j < _simpleCommands[ i ]->_numOfArguments; j ++ ) {
            free ( _simpleCommands[ i ]->_arguments[ j ] );
        }
        
        free ( _simpleCommands[ i ]->_arguments );
        free ( _simpleCommands[ i ] );
    }
    
    if ( _outFile ) {
        free( _outFile );
    }
    
    if ( _inFile ) {
        free( _inFile );
    }
    
    if ( _errFile ) {
        free( _errFile );
    }
    
    _numOfSimpleCommands = 0;
    _outFile = 0;
    _inFile = 0;
    _errFile = 0;
    _background = 0;
    _isAppend = 0;
    _inCount = 0;
    _outCount = 0;
    _errCount = 0;
}

void
Command::print()
{
    if (!debug)
        return;
    printf("\n\n");
    printf("              COMMAND TABLE                \n");
    printf("\n");
    printf("  #   Simple Commands\n");
    printf("  --- ----------------------------------------------------------\n");
    
    for ( int i = 0; i < _numOfSimpleCommands; i++ ) {
        printf("  %-3d ", i );
        for ( int j = 0; j < _simpleCommands[i]->_numOfArguments; j++ ) {
            printf("\"%s\" \t", _simpleCommands[i]->_arguments[ j ] );
        }
    }
    
    printf( "\n\n" );
    printf( "  Output       Input        Error        Background\n" );
    printf( "  ------------ ------------ ------------ ------------\n" );
    printf( "  %-12s %-12s %-12s %-12s\n", _outFile?_outFile:"default",
    _inFile?_inFile:"default", _errFile?_errFile:"default",
    _background?"YES":"NO");
    printf( "\n\n" );
    
}

void
Command::execute() {
    // Don't do anything if there are no simple commands
    if (_numOfSimpleCommands == 0) {
        prompt();
        return;
    }
    
    if (_inCount > 1)
    {
        printf("Ambiguous input redirect.");
        clear();
        prompt();
        return;
    }
    
    if (_outCount > 1)
    {
        printf("Ambiguous output redirect.");
        clear();
        prompt();
        return;
    }
    
    if (_errCount > 1)
    {
        printf("Ambiguous error redirect.");
        clear();
        prompt();
        return;
    }
    
    // If the exit command is called, exit here.
    if (!strcmp(_simpleCommands[0]->_arguments[0], "exit")) {
        printf("Good bye!!\n");
        exit(1);
    }
    
    // Print contents of Command data structure
    // Disabled if debug is off
    print();
    
    // Add execution here
    // For every simple command fork a new process
    // Setup i/o redirection
    // and call exec
    
    int ret;
    int defaultin = dup(0);
    int defaultout = dup(1);
    int defaulterr = dup(2);
    
    int infd;
    int errfd;
    int outfd;
    
    if (_inFile == 0)
        infd = dup(defaultin);
    else
        infd = open(_inFile, O_RDONLY);
    
    if (_errFile == 0)
        errfd = dup(defaulterr);
    else{
        if (_isAppend)
            errfd = open(_errFile, O_WRONLY | O_CREAT | O_APPEND, 0600);
        else
            errfd = open(_errFile, O_WRONLY | O_CREAT | O_TRUNC, 0600);
    }
    
    dup2(errfd, 2);
    close(errfd);
    
    for (int a = 0; a < _numOfSimpleCommands; a++) {
        dup2(infd, 0);
        close(infd);
        
        if (a == _numOfSimpleCommands - 1) {
            if (_outFile){
                if (_isAppend)
                    outfd = open(_outFile, O_WRONLY | O_CREAT | O_APPEND, 0600);
                else
                    outfd = open(_outFile, O_WRONLY | O_CREAT | O_TRUNC, 0600);
            }else
                outfd = dup(defaultout);
        } else {
            int pipefd[2];
            if (pipe(pipefd) == -1)
            {
                perror("pipe");
                exit(1);
            }
            
            infd = pipefd[0];
            outfd = pipefd[1];
        }
        
        dup2(outfd, 1);
        close(outfd);
        
        if(!strcmp(_simpleCommands[a]->_arguments[0], "setenv"))
        {
            int seerror;
            if(getenv(_simpleCommands[a]->_arguments[1]))
            seerror = setenv(_simpleCommands[a]->_arguments[1], _simpleCommands[a]->_arguments[2], 1);
            else
                seerror = setenv(_simpleCommands[a]->_arguments[1], _simpleCommands[a]->_arguments[2], 0);
            
            if (seerror)
                perror("setenv");
            
            clear();
            prompt();
            return;
        }
        else if(!strcmp(_simpleCommands[a]->_arguments[0], "unsetenv"))
        {
            int useerror;
            useerror = unsetenv(_simpleCommands[a]->_arguments[1]);
            
            if (useerror)
                perror("unsetenv");
            
            clear();
            prompt();
            return;
        }
        else if(!strcmp(_simpleCommands[a]->_arguments[0], "cd"))
        {
            int cderror;
            if(_simpleCommands[a]->_arguments[1])
            {
                cderror = chdir(_simpleCommands[a]->_arguments[1]);
            }
            else
            {
                cderror = chdir(getenv("HOME"));
            }
            
            if (cderror < 0) {
                perror("cd");
            }
            
            clear();
            prompt();
            return;
        }
        
        
        ret = fork();
        if (ret == 0) {
            //child
            
            
            //check for printenv
            if (!strcmp(_simpleCommands[a]->_arguments[0], "printenv")) {
                char** tempEnv = environ;
                while (*tempEnv != NULL) {
                    printf("%s\n", *tempEnv);
                    tempEnv = tempEnv + 1;
                }
                exit(0);
            }
            
            execvp(_simpleCommands[a]->_arguments[0], _simpleCommands[a]->_arguments);
            perror("execvp");
            _exit(1);
        } else if (ret < 0) {
            perror("fork");
            return;
        }
    }
    
    
    dup2(defaultin, 0);
    dup2(defaultout, 1);
    dup2(defaulterr, 2);

    close(defaultin);
    close(defaultout);
    close(defaulterr);
    
    if (!_background) {
        waitpid(ret, NULL, 0);
    }
    
    
    // Clear to prepare for next command
    clear();
    
    // Print new prompt
    prompt();
}


// Shell implementation

void
Command::prompt()
{
    if (isatty(0)) {
        printf("myshell>");
    }
}

void zombie(int sig)
{
    int pid = wait3(0, 0, NULL);
    while(waitpid(-1, NULL, WNOHANG) > 0);
}

void ctrlc(int sig)
{
    Command::_currentCommand.clear();
    printf("\n");
    Command::_currentCommand.prompt();
}

Command Command::_currentCommand;
SimpleCommand * Command::_currentSimpleCommand;

int yyparse(void);

main()
{
    struct sigaction sigact;
    sigact.sa_handler = ctrlc;
    sigemptyset(&sigact.sa_mask);
    sigact.sa_flags = SA_RESTART;
    
    int error = sigaction(SIGINT, &sigact, NULL);
    if (error)
    {
        perror("sigaction ctrlc");
        exit(-1);
    }
    
    sigact.sa_handler = zombie;
    sigemptyset(&sigact.sa_mask);
    error = sigaction(SIGCHLD, &sigact, NULL);
    if (error)
    {
        perror("sigaction zombie");
        exit(-1);
    }
    
    Command::_currentCommand.prompt();
    yyparse();
}
