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
}
namespace tick
{
	int			load_all(string file_name, t_tick_data *data); //on_tick is being called at each tick
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
	int			tick(t_tick_data *data);
	int			circular(t_tick_data *data);
	int			open_trade(
							int		order_type,
							float	lot_size,
							int		tp_mode,
							int		sl_mode,
							float	tp,
							float	sl,
							int		ticket,
							string	comment);
	int			close_trade(
							int		close_order,	//1 = FIFO, 2 = LIFO, 3 = random
							int		close_mode,		//1 = single, 2 = all, 3 = all longs, 4 = all shorts
							int		ticket,			//order ticket number, only useful for single closes
							string	comment);		//order close log comment
}
namespace calculate
{
	//P&L...
}