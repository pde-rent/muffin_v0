#include "main.hpp"

// this function is the core backtested strategy
// this will be called on ever data point (be it raw trade, tick, or ohlc)
int strategy::run(t_env *env, int pass)
{
	// declare indicators which will be used
	// static double old_MA1 = TICK(0);
	printf("PASS: %d consuming\n", pass);
	static double MA1[MAX_PASS] = {0};
	static double PMA1[MAX_PASS] = {0};		//prev MA
	static double PPMA1[MAX_PASS] = {0};	//prev prev MA
	// indicators time series update
	MA1[pass] = indicators::SMA<double>(env->price, MIN_MA1 + (env->step[0] * pass), 0, pass);
	// double MA2 = on_deque::SMA2<double>(env->price, 1000, 0);

	// entry && exit conditions
	if (MA1[pass] < PMA1[pass] && PMA1[pass] < PPMA1[pass]) {
		if (NB_SHORT[pass] > 0) {
			order::close_all(env, MODE_SELL, pass, "[strategy short close]");
		}
		if (NB_LONG[pass] == 0) {
			order::open(env,
				MODE_BUY, 1.00,					// entry mode / size
				MODE_TP_NONE, MODE_SL_NONE,		// tp mode / sl_mode
				0, 0,							// tp values / sl value
				0, pass,						// ticket / magic
				""								// comment
			);			
		}
	}
	if (MA1[pass] > PMA1[pass] && PMA1[pass] > PPMA1[pass]) {
		if (NB_LONG[pass] > 0) {
			order::close_all(env, MODE_BUY, pass, "[strategy long close]");
		}
		if (NB_SHORT[pass] == 0) {
			order::open(env,
				MODE_SELL, 1.00,
				MODE_TP_NONE, MODE_SL_NONE,
				0, 0,
				0, pass,
				""
			);
		}
	}
	// set MA[1] to PMA and MA[2] to PPMA:
	PPMA1[pass] = PMA1[pass];
	PMA1[pass] = MA1[pass];
	return (1);
}
