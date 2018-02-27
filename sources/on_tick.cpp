#pragma once

#ifdef _WIN32
# include "stdafx.h"
#elif __APPLE__
# include "../includes/stdafx.h"
#endif

int		on_tick(t_env *env, bool *addr[3])
{
	//get new tick >> from the lock addr[2]
	//static float old_MA1 = TICK(0);
	static float MA1[MAX_PASS];
	static float PMA1[MAX_PASS] = {0};	//prev MA
	static float PPMA1[MAX_PASS] = {0};	//prev prev MA
	//optimisation loop
	#pragma omp parallel //num_threads(5)
	{
		#pragma omp for
		for (int pass = 0; pass < env->pass; ++pass)
		{
			//indicators
			MA1[pass] = on_deque::SMA<float>(env->data->ask, MIN_MA1 + (env->step[0] * pass), 0, pass);
			//float MA2 = on_deque::SMA2<float>(env->data->px, 1000, 0);
			//entry && exit conditions
			//printf("ASK: %f MA: %f\n", ASK(0), MA1[pass]);
			if (!NB_LONG(pass) && MA1[pass] < PMA1[pass] && PMA1[pass] < PPMA1[pass])
			{
				OPEN(env, OP_BUY, 1, NO_TP, NO_SL, 0, 0, 1, pass, "long trade for pass " + pass);
				if (NB_SHORT(pass))
					CLOSE(env, 1, 1, 2, pass, "close short for pass " + pass);
			}
			if (!NB_SHORT(pass) && MA1[pass] > PMA1[pass] && PMA1[pass] > PPMA1[pass])
			{
				OPEN(env, OP_SELL, 1, NO_TP, NO_SL, 0, 0, 2, pass, "short trade for pass" + pass);
				if (NB_LONG(pass))
					CLOSE(env, 1, 1, 1, pass, "close long for pass " + pass);
			}
			PPMA1[pass] = PMA1[pass];
			PMA1[pass] = MA1[pass];
			//printf("epoch: %zu balance: %f\n", EPOCH(0), env->acc->balance);
		}
	}
	return (1);
}