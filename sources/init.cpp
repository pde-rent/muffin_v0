#pragma once

#ifdef _WIN32
# include "stdafx.h"
#elif __APPLE__
# include "../includes/stdafx.h"
#endif

namespace init
{
	static t_tick_data		*tick_data(void)
	{
		t_tick_data	*data;
		data = (t_tick_data *)malloc(sizeof(t_tick_data));
		data->epoch = new std::deque<size_t>(0);
		data->bid = new std::deque<float>(0);
		data->ask = new std::deque<float>(0);
		data->vol = new std::deque<float>(0);
		return (data);
	}
	static t_ohlc_data		*ohlc_data(void)
	{
		t_ohlc_data	*data;
		data = (t_ohlc_data *)malloc(sizeof(t_ohlc_data));
		return (data);
	}
	static t_account	*account(void)
	{
		t_account	*acc;
		//PARSE THAT PARAMS BRO
		acc = (t_account *)malloc(sizeof(t_account));
		return (acc);
	}
	static t_ticker	*ticker(void)
	{
		t_ticker	*ticker;
		ticker = (t_ticker *)malloc(sizeof(t_ticker));
		return (ticker);
	}
	static t_risk		*risk(void)
	{
		t_risk	*rsk;
		rsk = (t_risk *)malloc(sizeof(t_risk));
		return (rsk);
	}
	t_env		*env(void)
	{
		t_env	*env;
		env = (t_env *)malloc(sizeof(t_env));
		t_risk				*rsk;
		t_ticker			*ticker;
		t_account			*acc;
		t_tick_data			*data;

		env->rsk =				init::risk();
		env->acc =				init::account();
		env->data =				init::tick_data();
		env->ticker =			init::ticker();
		for (int pass = 0; pass < MAX_PASS; pass++)
		{
			env->order_book[pass] = new std::deque<t_open_order>(0);
			env->order_history[pass] = new std::deque<t_closed_order>(0);
			NB_LONG(pass) = 0;
			NB_SHORT(pass) = 0;
		}
		//param_parse("...");
		//t_tick_data *data = static_cast<t_tick_data*>(histo); //shall result depend on parameters parsing
		return (env);
	}
	int	split_process(t_env *env)
	{
		bool lock[3] = { false, false, false };
		bool *addr[3] = { &lock[0], &lock[1], &lock[2] };
		#pragma omp parallel sections
		{
			#pragma omp section		//tick loading thread
			{
				if (env->ticker->type > 0 && env->ticker->type < 5)
					tick::load_FX(env, addr);
				else
					tick::load_BTC(env, addr);
			}
			#pragma omp section		//on_tick backtester thread
			{
				trigger::tick_event::next(env, addr);
			}
		}
	}
}