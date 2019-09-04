#pragma once

#define OPEN				order::open
#define CLOSE				order::close

namespace order
{
	int			open(
		t_env		*env,				//trade environment
		int			order_type,			//-1 = short, +1 = long //need 2 add limit and stop orders
		double		volume,				//usually lots > 0.01 < 500
		int			tp_mode = 0,		//0 = no tp, 1 = hard price, 2 = % ticker move, 3 = % trade profit to account
		int			sl_mode = 0,		//0 = no sl, 1 = hard price, 2 = % ticker move, 3 = % trade profit to account
		double		tp = 0,				//either 0 or hard tp (1), ticker % runup from entry (2), trade % runup from entry (3)
		double		sl = 0,				//either 0 hard sl (1), ticker % dd from entry (2), trade % dd from entry (3)
		int			ticket = 0,			//ticket number, leave 0 for default
		int			magic = 0,			//unique identifier (pass number?)
		std::string	comment = "");
	// find order from ticket (TODO: magic optional ?)
	t_order		*find(
		t_env		*env,				//trade environment
		int			ticket,				//order ticket to close, default 0 for newest, 1 for oldest
		int			magic);				//unique comment to log
	int			close(
		t_env		*env,				//trade environment
		t_order		order,
		double		volume = 0.0,		// volume for partial close, 0 is default (full order close)
		std::string	comment = "");		//unique comment to log
	int			modify(
		t_env		*env,				//trade environment
		t_order		order,
		double		new_volume,			//modified order volume
		double		new_tp,				//modified order tp_price
		double		new_sl,				//modified order sl_price
		int			new_ticket,			//modified order ticket
		int			new_magic,
		std::string	comment = "");		//modified order comment
	int 		close_all(
		t_env 		*env,
		// int	sort = 1,			//1 = FIFO, 2 = LIFO, 3 = random
		// int	mode = 1,			//1 = single, 2 = all, 3 = all longs, 4 = all shorts
		int 		type,
		int 		magic,
		std::string comment = "");
}
