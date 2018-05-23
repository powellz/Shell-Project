/*
* CS354: Operating Systems.
* Purdue University
* Example that shows how to read one line with simple editing
* using raw terminal.
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAX_BUFFER_LINE 2048

// Buffer where line is stored
int line_length;
int cursor_pos;
char line_buffer[MAX_BUFFER_LINE];

// Simple history array
// This history does not change.
// Yours have to be updated.
char ** history;
int history_index = 0;
int history_length = 0;
int history_totalsize = 8;

void read_line_print_usage()
{
    char * usage = "\n"
    " ctrl-?       Print usage\n"
    " Backspace    Deletes last character\n"
    " up arrow     See last command in the history\n";
    
    write(1, usage, strlen(usage));
}

/*
* Input a line with some basic editing.
*/
char * read_line() {
    
    // Set terminal in raw mode
    tty_raw_mode();
    
    line_length = 0;
    cursor_pos = 0;
    
    if(history == NULL)
    history = (char**) malloc(history_totalsize * sizeof(char*));
    
    // Read one line until enter is typed
    while (1) {
        
        // Read one character in raw mode.
        char ch;
        read(0, &ch, 1);
        
        if (ch>=32 && ch!=127) {
            // It is a printable character.
            
            if(cursor_pos == line_length)
            {
                // Do echo
                write(1,&ch,1);
                
                // If max number of character reached return.
                if (line_length==MAX_BUFFER_LINE-2) break;
                
                // add char to buffer.
                line_buffer[line_length]=ch;
            }
            else
            {
                char chtemp = ' ';
                
                int a = cursor_pos;
                while(a < line_length)
                {
                    write(1, &chtemp,1);
                    a++;
                }
                
                chtemp = 8;
                a = cursor_pos;
                while(a < line_length)
                {
                    write(1,&chtemp,1);
                    a++;
                }
                
                write(1,&ch,1);
                
                a = cursor_pos;
                while(a < line_length)
                {
                    write(1,&line_buffer[a],1);
                    a++;
                }
                
                a = cursor_pos;
                while(a < line_length)
                {
                    write(1,&chtemp,1);
                    a++;
                }
                
                a = line_length;
                while(a > cursor_pos)
                {
                    line_buffer[a] = line_buffer[a-1];
                    a--;
                }
                
                line_buffer[cursor_pos] = ch;
            }
            line_length++;
            cursor_pos++;
        }
        else if (ch==10) {
            if(history_length == history_totalsize)
            {
                history_totalsize *= 2;
                history = (char**) realloc(history, history_totalsize*sizeof(char*));
                
            }
            
            // <Enter> was typed. Return line
            line_buffer[line_length] = 0;
            if(line_buffer[0] != 0)
            {
                history[history_length] = strdup(line_buffer);
            }
            history_length++;
            
            // Print newline
            write(1,&ch,1);
            
            break;
        }
        else if (ch == 31) {
            // ctrl-?
            read_line_print_usage();
            line_buffer[0]=0;
            break;
        }
        else if (ch == 8 || ch == 127) {
            // <backspace> was typed. Remove previous character read.
            
            // If at front of line
            if (line_length == 0 || cursor_pos == 0)
                continue;
            
            int a = cursor_pos-1;
            while(a < line_length-1)
            {
                line_buffer[a] = line_buffer[a+1];
                a++;
            }
            
            a = 0;
            // Go back one character
            ch = 8;
            while(a < cursor_pos)
            {
                write(1,&ch,1);
                a++;
            }
            
            // Write a space to erase the last character read
            ch = ' ';
            a = 0;
            while(a < line_length)
            {
                write(1,&ch,1);
                a++;
            }
            
            // Go back one character
            ch = 8;
            
            a = 0;
            while(a < line_length)
            {
                write(1,&ch,1);
                a++;
            }
            
            a = 0;
            while(a < line_length -1)
            {
                write(1, &line_buffer[a], 1);
                a++;
            }
            
            a = 0;
            while(a < line_length-cursor_pos)
            {
                write(1, &ch,1);
                a++;
            }
            
            // Remove one character from buffer
            line_length--;
            cursor_pos--;
        }  
        else if (ch==27) {
            // Escape sequence. Read two chars more
            //
            // HINT: Use the program "keyboard-example" to
            // see the ascii code for the different chars typed.
            //
            char ch1;
            char ch2;
            read(0, &ch1, 1);
            read(0, &ch2, 1);
            if (ch1==91 && ch2==65 && history_length > 0) {
                // Up arrow. Print next line in history.
                
                // Erase old line
                // Print backspaces
                int i = 0;
                for (i =0; i < line_length; i++) {
                    ch = 8;
                    write(1,&ch,1);
                }
                
                // Print spaces on top
                for (i =0; i < line_length; i++) {
                    ch = ' ';
                    write(1,&ch,1);
                }
                
                // Print backspaces
                for (i =0; i < line_length; i++) {
                    ch = 8;
                    write(1,&ch,1);
                }
                
                // Copy line from history
                strcpy(line_buffer, history[history_index]);
                line_length = strlen(line_buffer);
                history_index=(history_index+1)%history_length;
                cursor_pos = line_length;
                
                // echo line
                write(1, line_buffer, line_length);
            } else if (ch1==91 && ch2==66 && history_length > 0) {
                // Down arrow. Print previous line in history.
                
                // Erase old line
                // Print backspaces
                int i = 0;
                for (i =0; i < line_length; i++) {
                    ch = 8;
                    write(1,&ch,1);
                }
                
                // Print spaces on top
                for (i =0; i < line_length; i++) {
                    ch = ' ';
                    write(1,&ch,1);
                }
                
                // Print backspaces
                for (i =0; i < line_length; i++) {
                    ch = 8;
                    write(1,&ch,1);
                }
                
                // Copy line from history
                if (history_index >= 1)
                {
                    strcpy(line_buffer, history[--history_index]);
                }
                else
                {
                    history_index = history_length;
                    strcpy(line_buffer, history[--history_index]);
                }
                
                line_length = strlen(line_buffer);
                cursor_pos = line_length;
                
                // echo line
                write(1, line_buffer, line_length);
            }
            // Right arrow
            else if(ch1==91 && ch2==67 && cursor_pos < line_length)
            {
                ch = line_buffer[cursor_pos];
                write(1, &ch, 1);
                cursor_pos++;
            }
            // Left arrow
            else if(ch1==91 && ch2==68 && cursor_pos > 0)
            {
                ch = 8;
                write(1, &ch, 1);
                cursor_pos--;
            }
            //End
            else if (ch1==79 && ch2==70)
            {
                while(cursor_pos < line_length)
                {
                    ch = line_buffer[cursor_pos++];
                    write(1, &ch, 1);
                }
            }
            //Home
            else if(ch1==79 && ch2==72)
            {
                while(cursor_pos > 0)
                {
                    ch = 8;
                    write(1, &ch, 1);
                    cursor_pos--;
                }
            }
            //Delete
            else if (ch1==91 && ch2==51 && cursor_pos < line_length) {
                
	    }
            
        }
        
    }
    
    // Add eol and null char at the end of string
    line_buffer[line_length]=10;
    line_length++;
    line_buffer[line_length]=0;
    
    return line_buffer;
}

