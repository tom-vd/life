/*	FILE		: game.cpp
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
#include <iostream>
#include <fstream>
using namespace std;

// Library headers
#include <typedefs>

// Other headers
#include "game.h"

/*	Constructor game::game(
 *		i32 cwidth	: width of the view,
 *		i32 cheight	: height of the view
 *	);
 *
 *	Sets up the defaults of the game class and makes sure all cells are dead
 *	initially.
 */
game::game(i32 cwidth,i32 cheight) {
	this->perc_live = 0.5;
	this->delay = 0;
	this->live_cells = 0;
	this->delay = 0.1;
	this->kill_all();
	this->delta_x = cwidth;
	this->delta_y = cheight;
	this->ch_dead = '.';
	this->ch_live = 'o';
	this->x = this->y = 0;
	this->make_stats = false;
} // game::game(...)

/*	Destructor game::~game(void);
 *
 *	Closes the stats file if necessary.
 */
game::~game(void) {
	this->close_stats_file();
} // game::~game(...)

/*	Function game::print(
 *		i32 gen	: current generation, displayed below the view
 *	) -> void;
 *
 *	Prints the current generation.
 */
void game::print(i32 gen) {
	char ch_dead = this->ch_dead;
	char ch_live = this->ch_live;
	string out = "";
	for(i32 i = 0; i < VIEW_HEIGHT; i++) {
		i32 y = i + this->y;
		if(!(y < 0 || y >= MAX)) {
			for(i32 j = 0; j < VIEW_WIDTH; j++) {
				i32 x = j + this->x;
				if(x < 0 || x >= MAX) out += 'x';
				else out += (this->grid[y][x] ? ch_live : ch_dead);
			} // for
		} else for(i32 j = 0; j < VIEW_WIDTH; j++) out += 'x';
	} // for
	cout << out << "Generation: " << gen << " | live cells " << this->live_cells;
	cout << " | (" << this->x << "; " << this->y << ")" << endl;
} // game::print(...)

/*	Function game::tick(
 *		i32* gen	: pointer to an i32 that serves as generation counter
 *	) -> void;
 *
 *	Computes the next generation, updates the generation counter pointed to by
 *	gen and counts the number of live cells. Also, if make_stats is true, the
 *	statistics file will be updated.
 */
void game::tick(i32* gen) {
	bool temp[MAX][MAX];
	if(gen) ++(*gen);
	this->live_cells = 0;
	for(i32 i = 0; i < MAX; i++) {
		for(i32 j = 0; j < MAX; j++) {
			i32 neighbours = this->get_live_neighbours(i,j);
			switch(neighbours) {
				case 2:
					temp[i][j] = this->grid[i][j];
				break;
				case 3:
					temp[i][j] = true;
				break;
				default:
					temp[i][j] = false;
				break;
			} // switch
		} // for
	} // for
	for(i32 i = 1; i < MAX - 1; i++) for(i32 j = 1; j < MAX - 1; j++) if((this->grid[i][j] = temp[i][j])) ++this->live_cells;
	if(this->make_stats) this->stats_file << (*gen) << '\t' << this->live_cells << endl;
} // game::tick(...)

/*	Function game::get_live_neighbours(
 *		i32 i	: y coordinate,
 *		i32 j	: x coordinate
 *	) -> i32: the number of live cells around a specified cell.
 *
 *	Computes the number of live cells in the Moore neighbourhood of the cell in
 *	[i][j]. Note that if the cell is at the world's boundary, the cells outside
 *	Are considered dead.
 */
i32 game::get_live_neighbours(i32 i,i32 j) {
	i32 ret = 0;
	// north
	if(i > 0 && this->grid[i - 1][j]) ++ret;
	// west
	if(j > 0 && this->grid[i][j - 1]) ++ret;
	// south
	if(i < MAX && this->grid[i + 1][j]) ++ret;
	// east
	if(j < MAX && this->grid[i][j + 1]) ++ret;
	// north west
	if(i > 0 && j > 0 && this->grid[i - 1][j - 1]) ++ret;
	// north east
	if(i > 0 && j < MAX && this->grid[i - 1][j + 1]) ++ret;
	// south east
	if(i < MAX && j < MAX && this->grid[i + 1][j + 1]) ++ret;
	// south west
	if(i < MAX && j > 0 && this->grid[i + 1][j - 1]) ++ret;
	return ret;
} // game::get_live_neighbours(...)

/*	Function game::run_file(const string fname) -> bool: value indicating if
 *		the file could be opened successfully.
 *
 *	Opens a file and places it in the view. The first character of the file
 *	will be placed in the top-left corner of the view. Note that space
 *	characters are considered dead and non-space characters live. Windows- and
 *	Unix-style newlines are now handled properly. Also updates the live-cell
 *	counter.
 */
bool game::run_file(const string fname) {
	ifstream infile;
	char ch = 0;
	i32 i = this->y;
	i32 j = this->x;
	infile.open(fname.c_str(),ios::in);
	if(!infile.good()) return false;
	this->kill_all();
	ch = infile.get();
	while(!infile.eof()) {
		if(i >= MAX) break;
		if(ch == '\n' || i < 0 || j >= MAX) {
			++i;
			j = this->x;
		} else {
			if(ch != ' ' && ch != '\r') {
				this->grid[i][j] = true;
				++this->live_cells;
			} // if
			++j;
		} // if
		ch = infile.get();
	} // while
	return true;
} // game::run_file(...)

/*	Function game::random(void) -> void;
 *
 *	Fills the view randomly with dead and live cells, according th the
 *	percentage of live cells. Also updates the live-cell counter.
 */
void game::random(void) {
	this->kill_in_view();
	for(i32 i = 0; i < VIEW_HEIGHT; i++) {
		i32 y = i + this->y;
		for(i32 j = 0; j < VIEW_WIDTH; j++) {
			i32 x = j + this->x;
			this->grid[y][x] = rand_bool(this->perc_live);
			if(this->grid[y][x]) ++this->live_cells;
		} // for
	} // for
} // game::random(...)

/*	Function game::kill_all(void) -> void;
 *
 *	Kills all cells in the entire world and resets the live-cell counter.
 */
void game::kill_all(void) {
	for(i32 i = 0; i < MAX; i++) for(i32 j = 0; j < MAX; j++) this->grid[i][j] = false;
	this->live_cells = 0;
} // game::kill_all(...)

/*	Function game::kill_in_view(void) -> void;
 *
 *	Kills all cells in the view and updates the live-cell counter.
 */
void game::kill_in_view(void) {
	for(i32 i = 0; i < VIEW_HEIGHT; i++) {
		i32 y = i + this->y;
		if(y >= 0) {
			for(i32 j = 0; j < VIEW_WIDTH; j++) {
				i32 x = j + this->x;
				if(x >= 0) {
					if(this->grid[y][x]) --this->live_cells;
					this->grid[y][x] = false;
				} else if(x >= MAX) break;
			} // for
		} else if(y >= MAX) break;
	} // for
} // gamt_t::kill_in_view(...)

void game::create_stats_file(const string fname) {
	this->stats_file.open(fname.c_str(),ios::app);
} // game::create_stats_file(...)

void game::close_stats_file(void) {
	if(this->stats_file.is_open()) this->stats_file.close();
} // game::create_stats_file(...)
