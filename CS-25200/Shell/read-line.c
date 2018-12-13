/*
 * CS252: Systems Programming
 * Purdue University
 * Example that shows how to read one line with simple editing
 * using raw terminal.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define MAX_BUFFER_LINE 2048

extern void tty_raw_mode(void);

// Buffer where line is stored
int line_length;
char line_buffer[MAX_BUFFER_LINE];

// Simple history array
// This history does not change.
// Yours have to be updated.
int history_index = -1;
int hisotryPointer = -1;
char ** history;
//int history_length = sizeof(history)/sizeof(char *);
int history_length = 128;

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

  // check if histor of commands is null
  if(!history){
    history = (char**) malloc(sizeof(char*) * history_length);
  }

  //create a cursor
  int mouse  = 0;

  // Set terminal in raw mode
  tty_raw_mode();

  line_length = 0;

  // Read one line until enter is typed
  while (1) {

    // Read one character in raw mode.
    char character;
    read(0, &character, 1);

    if (character>=32 && character <= 126) {
      // It is a printable character.

      // Do echo
      write(1,&character,1);

      // If max number of character reached return.
      if (line_length==MAX_BUFFER_LINE-2) break;

      // add char to buffer.
      mouse++;
      line_buffer[line_length]=character;
      line_length++;
    }
    else if (character==10) {
      // <Enter> was typed. Return line
      if(history_index < history_length){
        history_index++;
        hisotryPointer = history_index;
        history[history_index] = strdup(line_buffer, line_length);
      }

      // Print newline
      write(1,&character,1);
      mouse = 0;
      break;
    }
    else if (character == 31) {
      // ctrl-?
      read_line_print_usage();
      line_buffer[0]=0;
      break;
    }
    else if (character == 8 || character == 127) {
      // <backspace> was typed. Remove previous character read.
      if (mouse > 0){
        // Go back one character
        character = 8;
        write(1,&character,1);

        // Write a space to erase the last character read
        character = ' ';
        write(1,&character,1);

        // Go back one character
        character = 8;
        write(1,&character,1);

        mouse--;
        // Remove one character from buffer
        line_length--;
      }
    }
    //CONTROL E WAS PRESSED
    else if (character == 5){
      while(mouse < line_length){
        char temp = 27;
        write(1,&temp,1);
        temp = 91;
        write(1,&temp,1);
        temp = 67;
        write(1,&temp,1);
        mouse++;
      }
    }
    //CONTROL A WAS PRESSED
    else if (character == 1){
      while(mouse > 0){
        char temp = 27;
        write(1,&temp,1);
        temp = 91;
        write(1,&temp,1);
        temp = 68;
        write(1,&temp,1);
        mouse--;
      }
    }
    else if (character==27) {
      // Escape sequence. Read two chars more
      //
      // HINT: Use the program "keyboard-example" to
      // see the ascii code for the different chars typed.
      //
      char character1;
      char character2;
      read(0, &character1, 1);
      read(0, &character2, 1);
      if (character1==91 && character2==65) {
    	// Up arrow. Print next line in history.

    	// Erase old line
    	// Print backspaces
    	for (int x  =0; x < line_length; x++) {
    	  character = 8;
    	  write(1,&character,1);
    	}

    	// Print spaces on top
    	for (int x =0; x < line_length; x++) {
    	  character = ' ';
    	  write(1,&character,1);
    	}

    	// Print backspaces
    	for (int x =0; x < line_length; x++) {
    	  character = 8;
    	  write(1,&character,1);
    	}

    	// Copy line from history
      if(hisotryPointer >= 0){
      	strcpy(line_buffer, history[hisotryPointer]);
      	line_length = strlen(line_buffer);
        hisotryPointer--;
        mouse = strlen(line_buffer);
      }
    	// echo line
    	write(1, line_buffer, line_length);
  }

  else if (character1 == 91 && character2 == 67){
    if (mouse < line_length){
      char temp = 27;
      write(1, &temp,1);
      temp = 91;
      write(1, &temp,1);
      temp = 67;
      write(1, &temp,1);
      mouse++;
    }
  }
  else if (character1 == 91 && character2 == 66){
    for (int x = 0; x < line_length; x++){
      character = 8;
      write (1, &character,1);
    }

    for (int x = 0; x < line_length; x++){
      character = ' ';
      write (1, &character,1);
    }

    for (int x = 0; x < line_length; x++){
      character = 8;
      write (1, &character,1);
    }

    if (hisotryPointer < history_index){
      hisotryPointer++;
      strcpy(line_buffer, history[hisotryPointer]);
      line_length = strlen(line_buffer);
      mouse = strlen(line_buffer);
    }

    write(1, line_buffer,line_length);
  }
  else if (character1 == 91 && character2 == 68){
    if (mouse > 0){
      char temp = 27;
      write(1, &temp,1);
      temp = 91;
      write(1, &temp,1);
      temp = 68;
      write(1, &temp,1);
      mouse--;
    }
  }
  }
}

  // Add eol and null char at the end of string
  line_buffer[line_length]=10;
  line_length++;
  line_buffer[line_length]=0;

  return line_buffer;
}
