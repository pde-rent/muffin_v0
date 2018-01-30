#pragma once

#include "stdafx.h"

namespace init
{
	t_tick_data	*tick_data();
	t_ohlc_data	*ohlc_data();
	t_account	*account();
	t_ticker	*ticker();
	//t_order		*order(); //open_order and history_order to differenciate
	t_risk		*risk();
	t_env		*env();
}
namespace tick
{
	int			load_all(string file_name, t_env *env); //on_tick is being called at each tick
	//int		set(string file_name, t_tick_data *data);
	//int		next(t_tick_data *data);
}
namespace fast
{
	size_t		parse_zu(string str);
	double		parse_f(string str);
}
namespace trigger
{
	int			tick_event(t_env *env);
	int			circular(t_tick_data *data);
	namespace trade
	{
		int			open(
			t_env		*env,
			int16_t		order_type,		//-1 = short, +1 = long //need 2 add limit and stop orders
			float		lot_size,		//usually > 0.01 < 500
			int16_t		tp_mode,		//1 = hard price, 2 = % ticker move, 3 = % trade profit to account
			int16_t		sl_mode,		//1 = hard price, 2 = % ticker move, 3 = % trade profit to account
			float		tp,				//hard tp (1), ticker % runup from entry (2), trade % runup from entry (3)
			float		sl,				//hard sl (1), ticker % dd from entry (2), trade % dd from entry (3)
			//int16_t		magic,			//magic number (order unique identifier)
			size_t		ticket,			//ticket number, leave 0 for default
			std::string	comment);
		int			close(
			t_env		*env,
			int16_t		sort,		//1 = FIFO, 2 = LIFO, 3 = random
			int16_t		mode,		//1 = single, 2 = all, 3 = all longs, 4 = all shorts
			size_t		ticket,		//single ticket number //maybe point to first ticket to close? int *?
			std::string	comment);
	}
}
namespace calculate
{
	//P&L...
}
int		on_tick(t_env *env);
int		get_config(string file_name, t_env *env);
