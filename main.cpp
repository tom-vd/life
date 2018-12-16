/*	FILE		: main.cpp
 *	AUTHOR(-S)	: Tom van Dijkhuizen
 *	LAST EDIT	: December 11, 2018
 *	
 *	Compile with:
 *
 *		g++ *.cpp [-Wall] -o life.exe
 *
 *	Written and tested on Windows machines.
 */

// C/C++ headers
#include <ctime>
#include <iostream>
#include <fstream>
using namespace std;

// Library headers
#include <typedefs>

// Other headers
#include "game.h"
#include "main.h"

/*	Function main(...) -> int
 *
 *	Entry poi32 to the application
*/
i32 main(i32 argc,char* argv[]) {
	(void) argc;
	(void) argv;

	game game(VIEW_WIDTH,VIEW_HEIGHT);
	game.y = game.x = MAX / 2;

	// Prepare the console.
	clear_stream(&cout,game.delta_y);
	printinfoblock();
	cout << "Welcome to Life! ";

	run_menu(&game);
	return 0;
} // main(...)

// Overload of read_int; assumes: max = INT_MAX;
i32 read_int(istream* str) {
	return read_int(str,INT_MAX);
} // read_int(...)

void read_string(istream* istrm,string* ostr) {
	pump_stream(istrm);
	char ch = istrm->get();
	while(ch != '\n') {
		(*ostr) += ch;
		ch = istrm->get();
	} // while
} // read_string(...)

void read_string(string* ostr) {
	read_string(&cin,ostr);
} // read_string(...)

// Overload of read_int; assumes: str = &cin;
i32 read_int(i32 max) {
	return read_int(&cin,max);
} // read_int(...)

// Overload of read_int; assumes: str = &cin; max = INT_MAX
i32 read_int() {
	return read_int(&cin,INT_MAX);
} // read_int(...)

/*	Function read_int(
 *		istream* str: pointer to an istream from which to read the int,
 *		i32 max: the largest number to read
 *	) -> int;
 *
 *	Reads an integer from the stream pointed to by str and returns it. If the
 *	number is larger than the value contained in max, the return value is
 *	devided by ten before it is returned. Also, any non-digit character will be
 *	ignored.
 */
i32 read_int(istream* str,i32 max) {
	i32 ret = 0;
	char ch[] = { 0,0,0,0,0,0 };
	// Clear the stream of any dangling line feeds.
	pump_stream(str);
	for(u32 i = 0; i < sizeof(ch); i++) {
		// If we encounter a NULL character, we break out of the loop.
		if((ch[i] = str->get()) == 0) break;
		if(IS_CHAR_DIGIT(ch[i])) {
			ret *= 10;
			ret += CHAR_TO_INT(ch[i]);
		} else {
			// Stop when we encounter a line feed.
			if(ch[i--] == '\n') return ret;
			// If the character is not a digit, we reset char[i]
			ch[i] = 0;
		} // if
		// If the number is larger than max, we cut off the last digit.
		if(ret > max) return ret / 10;
	} // for
	return ret;
} // read_int(...)

/*	Function run_menu(game* game) -> void;
 *
 *	Runs the main menu.
 */
void run_menu(game* game) {
	char ch = 0;
	string msg,fname;
	while(true) {
		cout << "Main menu\n\n[P]arameters\n[R]un random set-up\n[G]lider gun";
		cout << "\n[S]tatistics\n[O]pen file\n[H]elp\n[Q]uit" << endl;
		if(msg.length() > 0) cout << msg << endl;
		pump_stream(&cin);
		ch = cin.get();
		clear_stream(&cout,game->delta_y);
		switch(ch) {
			case 'p':
			case 'P':
				run_params_menu(game);
				msg = "";
			break;
			case 'G':
			case 'g':
				if(game->run_file("config/glidergun.txt")) {
					run_game(game);
					msg = "";
				} else msg = "Error opening file \"glidergun.txt\"";
			break;
			case 'r':
			case 'R':
				game->random();
				run_game(game);
				msg = "";
			break;
			case 's':
			case 'S':
				cout << "Generate statistics when running?\nCurrent: ";
				cout << (game->make_stats ? "Yes" : "No") << "\n[Y]es/[n]o ";
				pump_stream(&cin);
				ch = cin.get();
				game->make_stats = (ch == 'y' || ch == 'Y');
				if(game->make_stats) game->create_stats_file("stats.txt");
				else game->close_stats_file();
				clear_stream(&cout,VIEW_HEIGHT);
			break;
			case 'o':
			case 'O':
				fname = "";
				clear_stream(&cout,VIEW_HEIGHT);
				cout << "File name to run: ";
				read_string(&cin,&fname);
				if(game->run_file(fname)) {
					run_game(game);
					msg = "";
				} else {
					msg = "Error opening file \"";
					msg.append(fname);
					msg.append("\"");
				} // if
			break;
			case 'h':
			case 'H':
				printinfoblock();
			break;
			case 'q':
			case 'Q':
				cout << "Thank you for using Life!" << endl;
				return;
			break;
			default:
				msg = "Invalid option: ";
				msg.append(1,ch);
			break;
		} // switch
	} // while
	cout << msg << endl;
} // run_menu(...)

/*	Function run_game(game* game) -> void;
 *
 *	Runs the game.
 */
void run_game(game* game) {
	char ch = 0;
	i32 gen_disp = 0;
	i32 gen = 0;
	string fname;
	while(true) {
		game->print(gen_disp);
		cout << "[B]ack | Move: [w/a/s/d] | [K]ill cells | [G]o | [T]ick | [O]pen file ";
		cout << flush;
		pump_stream(&cin);
		ch = cin.get();
		switch(ch) {
			case 'b':
			case 'B':
				clear_stream(&cout,game->delta_y);
				return;
			break;
			case 'a':
			case 'A':
				game->x -= game->delta_x;
			break;
			case 'd':
			case 'D':
				game->x += game->delta_x;
			break;
			case 'w':
			case 'W':
				game->y -= game->delta_y;
			break;
			case 's':
			case 'S':
				game->y += game->delta_y;
			break;
			case 'k':
			case 'K':
				cout << "[V]iew only | [A]ll ";
				pump_stream(&cin);
				ch = cin.get();
				switch(ch) {
					case 'v':
					case 'V':
						game->kill_in_view();
					break;
					case 'a':
					case 'A':
						game->kill_all();
					break;
					default:
						cout << "Invalid option: " << ch;
					break;
				} // switch
			break;
			case 'g':
			case 'G':
				cout << "Number of generations to run: ";
				pump_stream(&cin);
				gen = read_int(&cin);
				while(gen--) {
					game->tick(&gen_disp);
					game->print(gen_disp);
					wait(game->delay);
				} // while;
			break;
			case 'o':
			case 'O':
				fname = "";
				clear_stream(&cout,VIEW_HEIGHT);
				cout << "The contents of the file will be placed at (";
				cout << game->x << "; " << game->y << ")\nFile to run: ";
				read_string(&cin,&fname);
				if(!game->run_file(fname)) {
					cout << "Error opening file \"" << fname << "\"";
				} // if
			break;
			case 't':
			case 'T':
			default:
				game->tick(&gen_disp);
			break;
		} // switch
	} // while
} // run_game(...)

/*	Function run_params_menu(game* game) -> void;
 *
 *	Runs the params menu.
 */
void run_params_menu(game* game) {
	char ch = 0;
	string msg;
	while(ch != 'b' && ch != 'B') {
		cout << "Parameters\n\n[X] speed: " << game->delta_x << "\n[Y] speed: ";
		cout << game->delta_y << "\n[%] of live cells in random mode: ";
		cout << game->perc_live * 100 << "\n[D]ead cell: " << game->ch_dead;
		cout << "\n[L]ive cell: " << game->ch_live << "\n[T]ime delay between";
		cout << " two ticks in run mode: " << game->delay << "\n[B]ack" << endl;
		if(msg.length() > 0) cout << msg << endl;
		pump_stream(&cin);
		ch = cin.get();
		clear_stream(&cout,game->delta_y);
		switch(ch) {
			case 'x':
			case 'X':
				cout << "X speed: ";
				game->delta_x = read_int(&cin);
				msg = "";
			break;
			case 'y':
			case 'Y':
				cout << "Y speed: ";
				game->delta_y = read_int(&cin);
				msg = "";
			break;
			case '%':
				cout << "% of live cells: ";
				game->perc_live = read_int(&cin) / 100.0;
				msg = "";
			break;
			case 'd':
			case 'D':
				cout << "Dead cell: ";
				pump_stream(&cin);
				game->ch_dead = cin.get();
				msg = "";
			break;
			case 'l':
			case 'L':
				cout << "Live cell: ";
				pump_stream(&cin);
				game->ch_live = cin.get();
				msg = "";
			break;
			case 't':
			case 'T':
				cout << "Delay in milliseconds: ";
				game->delay = read_int(&cin) / 1000.0;
			break;
			default:
				msg = "Invalid option: ";
				msg.append(1,ch);
			break;
		} // switch
	} // while
} // run_params_menu(...)

/*	Function clear_stream(ostream* str,i32 i) -> void;
 *
 *	Adds a number of line-feed characters to the stream pointed to by str. The
 *	value held by i is the number of line-feed characters that will be added to
 *	the stream. Also flushes the stream.
 */
void clear_stream(ostream* str,i32 i) {
	while(i--) str->put('\n');
	str->flush();
} // clear_stream(...)

/*	Function pump_stream(istream* str) -> void;
 *
 *	Clears the istream pointed to by str from any dangling line feeds. Of you
 *	ever run into problems with IO operations, try pumping the istream.
 */
void pump_stream(istream* str) {
	char ch = 0;
	while(true) {
		ch = str->get();
		if(ch != '\n') {
			str->putback(ch);
			break;
		} // if
	} // while
} // pump_stream(...)

/*	Function rand_bool(
 *		f64 probability	: probability of the return value being true
 *	) -> bool;
 *
 *	Generates a pseudo-random boolean value.
 */
bool rand_bool(f64 probability) {
	static u32 rnd = time(0) % 42;
	rnd = (221 * rnd + 1) % MAX;
	return (rnd / (f64) MAX) <= probability;
} // rand_bool(...)

// Waits for approximately the value held by delay (time in seconds).
void wait(f64 delay = 1) {
	// Note: we use clock() here, because it's more precise than time()
	auto t = ((f64) clock()) / CLOCKS_PER_SEC;
	// As this program doesn't need high-precision for the waiting operation, we
	// don't really care about whether we should use < or <=
	while((((f64) clock()) / CLOCKS_PER_SEC) - t < delay); // do nothing
} // wait(...)

// Shows the user what they're looking at.
void printinfoblock(void) {
	cout << "Program information (Life, Programmeermethoden 2012)\n" << endl;
	cout << "Author\t: Tom van Dijkhuizen (1216422, Computer science)\nDate\t: " << __CDATE__ << "\r\n\r\n";
	cout << "This program runs Conway\'s game of life (also known as \"Life\"). The main\nmenu, parameters menu and in-game menu all display various options you can\nchoose from. During the";
	cout << " simulation of multiple generations in go mode, you will have no control over the program.\n" << endl;
} // printinfoblock(...)
