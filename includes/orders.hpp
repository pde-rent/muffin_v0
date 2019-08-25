#pragma once

#define OPEN				order::open
#define CLOSE				order::close

namespace order
{
	int			open(
		t_env		*env,				//trade environment
		int16_t		order_type,			//-1 = short, +1 = long //need 2 add limit and stop orders
		double		volume,				//usually lots > 0.01 < 500
		int16_t		tp_mode = 0,		//0 = no tp, 1 = hard price, 2 = % ticker move, 3 = % trade profit to account
		int16_t		sl_mode = 0,		//0 = no sl, 1 = hard price, 2 = % ticker move, 3 = % trade profit to account
		double		tp = 0,				//either 0 or hard tp (1), ticker % runup from entry (2), trade % runup from entry (3)
		double		sl = 0,				//either 0 hard sl (1), ticker % dd from entry (2), trade % dd from entry (3)
		size_t		ticket = 0,			//ticket number, leave 0 for default
		int			magic = 0,				//unique identifier (pass number?)
		std::string	comment = "");
	int			close(
		t_env		*env,				//trade environment
		// int16_t		sort = 1,			//1 = FIFO, 2 = LIFO, 3 = random
		// int16_t		mode = 1,			//1 = single, 2 = all, 3 = all longs, 4 = all shorts
		size_t		ticket = 0,			//order ticket to close, default 0 for newest, 1 for oldest
		int			magic = 0,				//unique identifier (pass number?)
		std::string	comment = "\0");	//unique comment to log
	int			modify(
		t_env		*env,			//trade environment
		size_t		ticket,			//order ticket to modify
		int			magic,			//order magic to modify
		double		new_volume,		//modified order volume
		double		new_tp,			//modified order tp_price
		double		new_sl,			//modified order sl_price
		size_t		new_ticket,		//modified order ticket
		int			new_magic,
		std::string	new_comment);	//modified order comment
}
