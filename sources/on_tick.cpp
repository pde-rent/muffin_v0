#pragma once

#ifdef _WIN32
# include "stdafx.h"
#elif __APPLE__
# include "../includes/stdafx.h"
#endif

int		on_tick(t_env *env)
{
	//indicators
	float RSI = on_deque::RSI<float>(env->data->px, 500, 0);
	//float MA2 = on_deque::SMA2<float>(env->data->px, 1000, 0);
	//entry && exit conditions
	printf("PX: %f RSI: %f\n", TICK(0), RSI);
	/*
	if (!NB_LONG && TICK(0) >= MA && TICK(1) < MA)
	{
		trigger::trade::open(env, OP_BUY, 1, NO_TP, NO_SL, 0, 0, 1, "long trade");
		if (NB_SHORT)
			trigger::trade::close(env, 1, 1, 2, "close short");
	}
	else if (!NB_SHORT && TICK(0) <= MA && TICK(1) > MA)
	{
		trigger::trade::open(env, OP_SELL, 1, NO_TP, NO_SL, 0, 0, 2, "short trade");
		if (NB_LONG)
			trigger::trade::close(env, 1, 1, 1, "close long");
	}
	printf("epoch: %zu balance: %f\n", EPOCH(0), env->acc->balance);
	*/
	return (1);
}
