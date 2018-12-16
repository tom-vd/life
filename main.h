/*	FILE		: main.h
 *	AUTHOR(-S)	: Tom van Dijkhuizen
 *	LAST EDIT	: December 11, 2018
 *	
 *	Compile with:
 *
 *		g++ *.cpp [-Wall] -o life.exe
 *
 *	Written and tested on Windows machines.
 */

// Constants from various libraries/headers not included.
//#define RAND_MAX (32767)
//#define INT_MAX (32767)

#define __CDATE__ "November 16, 2012"

// Macros used by I/O functions
#define CHAR_TO_INT(c) ((i32) (c - '0'))
#define IS_CHAR_DIGIT(ch) ('0' <= ch && ch <= '9')

// Forward declarations of functions
i32 main(i32,char*[]);
bool cmd_parsefilename(std::string*,i32,char*[]);
bool streq(const char*,std::string);
void read_string(std::istream*,std::string*);
void read_string(string*);
i32 read_int(std::istream*);
i32 read_int(std::istream*,i32);
void run_menu(game*);
void run_params_menu(game*);
void clear_stream(std::ostream*,i32);
void pump_stream(std::istream*);
bool rand_bool(f64);
void run_game(game*);
void wait(f64);
void printinfoblock(void);
