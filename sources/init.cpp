#include "stdafx.h"

namespace init
{
	t_tick_data		*tick_data(void)
	{
		t_tick_data	*data;
		data = (t_tick_data *)malloc(sizeof(t_tick_data));
		data->epoch = new std::deque<size_t>(0);
		data->px = new std::deque<float>(0);
		data->vol = new std::deque<float>(0);
		return (data);
	}
	t_order_book	*order_book(void)
	{
		t_order_book	*order_book;
		order_book = (t_order_book *)malloc(sizeof(t_order_book));
		order_book->order = new std::deque<t_open_order>(0);
		return (order_book);
	}
	t_order_history	*order_history(void)
	{
		t_order_history	*order_history;
		order_history = (t_order_history *)malloc(sizeof(t_order_history));
		order_history->order = new std::deque<t_closed_order>(0);
		return (order_history);
	}
	t_ohlc_data		*ohlc_data(void)
	{
		t_ohlc_data	*data;
		data = (t_ohlc_data *)malloc(sizeof(t_ohlc_data));
		return (data);
	}
	t_account	*account(void)
	{
		t_account	*acc;
		//PARSE THAT PARAMS BRO
		acc = (t_account *)malloc(sizeof(t_account));
		acc->holder = "Paul de Renty";
		acc->starting_eq = 1000.00;
		acc->balance = acc->starting_eq;
		acc->number = "00000001";
		return (acc);
	}
	t_ticker	*ticker(void)
	{
		t_ticker	*ticker;
		ticker = (t_ticker *)malloc(sizeof(t_ticker));
		ticker->nb_long = 0;
		ticker->nb_short = 0;
		return (ticker);
	}
	t_risk		*risk(void)
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
		t_order_book		*order_book;
		t_order_history		*order_history;

		env->rsk =				init::risk();
		env->acc =				init::account();
		env->data =				init::tick_data();
		env->ticker =			init::ticker();
		env->order_book =		init::order_book();
		env->order_history =	init::order_history();
		//param_parse("...");
		//t_tick_data *data = static_cast<t_tick_data*>(histo); //shall result depend on parameters parsing
		return (env);
	}
}