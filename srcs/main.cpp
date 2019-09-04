#include "main.hpp"

// mutex printf_lock;
// #define printf_locked(x) { scoped_lock<mutex> lock(printf_lock); printf(x); }

int main(int ac, char **av)
{
	// check arguments and parse flags
	if (!compliance::argv(ac, av)) { return (0); }
	// init environment structure (passed across the program but non global)
	t_env *env = init::env();
	// run backtest optimizer
	init::backtest(env); // returns time >> could be used
	// display backtest optimizer results
	print::result::ending_equity(env);
	// free env remaining allocated memory
	deinit::env(env);
	return (0);
}
