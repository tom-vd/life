/*	FILE		: game.h
 *	AUTHOR(-S)	: Tom van Dijkhuizen
 *	LAST EDIT	: December 11, 2018
 *	
 *	Compile with:
 *
 *		g++ *.cpp [-Wall] -o life.exe
 *
 *	Written and tested on Windows machines.
 */

#define MAX (1000)

// View dimensions. Keep these values small, large views are prone to bad performance
#define VIEW_WIDTH (120)
#define VIEW_HEIGHT (28)

class game {
	private:
		bool grid[MAX][MAX];
		i32 grid_size;
		i32 live_cells;
		std::ofstream stats_file;

	public:
		f64 perc_live;
		f64 delay;
		char ch_dead;
		char ch_live;
		i32 delta_x;
		i32 delta_y;
		i32 x,y;
		bool make_stats;
	
		game(i32,i32);
		~game(void);
		void print(i32);
		void tick(i32*);
		bool run_file(const std::string);
		void random(void);
		i32 get_live_neighbours(i32,i32);
		void kill_all(void);
		void kill_in_view(void);
		void create_stats_file(const std::string);
		void close_stats_file(void);
}; // game

extern bool rand_bool(f64);
