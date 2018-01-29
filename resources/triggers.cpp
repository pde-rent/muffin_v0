#include "stdafx.h"

namespace trigger
{
	int			tick(t_tick_data *data)
	{
		return	(on_tick(data));
	}
	int			open_trade(
		int		order_type,
		float	lot_size,
		int		tp_mode,
		int		sl_mode,
		float	tp,
		float	sl,
		int		ticket,
		string	comment)
	{
		return (1);
	}
	int			close_trade(
		int		close_order,
		int		close_mode,
		int		ticket,
		string	comment)
	{
		return (1);
	}
	int			circular(t_tick_data *data)
	{
		data->epoch->resize(TICK_BUFF_SIZE);	//data->epoch->erase(TICK_BUFF_SIZE);
		data->px->resize(TICK_BUFF_SIZE);		//data->px->erase(TICK_BUFF_SIZE);
		data->vol->resize(TICK_BUFF_SIZE);;		//data->vol->erase(TICK_BUFF_SIZE);
		return (1);
	}
}