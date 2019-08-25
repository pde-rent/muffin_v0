#include "main.hpp"

namespace strategy
{
	int		run(t_env *env/*, bool *lock[3]*/)
	{
		//get new tick >> from the lock lock[2]
		//static double old_MA1 = TICK(0);
		static double MA1[MAX_PASS];
		static double PMA1[MAX_PASS] = {0};	//prev MA
		static double PPMA1[MAX_PASS] = {0};	//prev prev MA
		//optimisation loop
		#pragma omp parallel //num_threads(5)
		{
			#pragma omp for
			for (int pass = 0; pass < env->pass; ++pass)
			{
				//indicators
				MA1[pass] = on_deque::SMA<double>(env->price, MIN_MA1 + (env->step[0] * pass), 0, pass);
				//double MA2 = on_deque::SMA2<double>(env->price, 1000, 0);
				//entry && exit conditions
				//printf("ASK: %f MA: %f\n", ASK(0), MA1[pass]);
				if (!NB_LONG(pass) && MA1[pass] < PMA1[pass] && PMA1[pass] < PPMA1[pass])
				{
					order::open(env, MODE_BUY, 1.00, MODE_TP_NONE, MODE_SL_NONE, 0, 0, 1, pass, "");
					if (NB_SHORT(pass) > 0)
						order::close(env,
							/*MODE_FIFO,
							MODE_CLOSE_SINGLE,*/
							2,
							pass,
							"");
				}
				if (!NB_SHORT(pass) && MA1[pass] > PMA1[pass] && PMA1[pass] > PPMA1[pass])
				{
					order::open(env, MODE_SELL, 1.00, MODE_TP_NONE, MODE_SL_NONE, 0, 0, 2, pass, "");
					if (NB_LONG(pass) > 0)
						order::close(env,
							/*MODE_FIFO,
							MODE_CLOSE_SINGLE,*/
							1,
							pass,
							"");
				}
				PPMA1[pass] = PMA1[pass];
				PMA1[pass] = MA1[pass];
				//printf("epoch: %zu balance: %f\n", EPOCH(0), env->acc->balance);
			}
		}
		return (1);
	}
}
