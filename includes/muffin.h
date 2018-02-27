#pragma once

#include "stdafx.h"

namespace init //constructors
{
	//t_tick_data	*tick_data();
	//t_ohlc_data	*ohlc_data();
	//t_account		*account();
	//t_ticker		*ticker();
	//t_risk		*risk();
	t_env		*env();
	int			split_process(t_env *env);
}
namespace deinit //destructors
{
	int		env(t_env *env);
}
namespace tick
{
	int			load_BTC(t_env *env, bool *addr[3]); //on_tick is being called at each tick
	int			load_FX(t_env *env, bool *addr[3]); //on_tick is being called at each tick
	//int		set(string file_name, t_tick_data *data);
	//int		next(t_tick_data *data);
}
namespace file
{
	bool			exists(string file_name);
	FILE			*open_r(string file_name);
	FILE			*open_w(string file_name, bool erase = false);
	bool			close(FILE *file);
	bool			write_line(FILE *file, string str);
	bool			exists(string file_name);
	bool			write(string file_name, string str, bool erase = false);
	unsigned long	count_lines(string file_name);
}
namespace fast
{
	size_t		parse_zu(string str);
	float		parse_f(string str);
}
namespace trigger
{
	namespace tick_event
	{
		int		next(t_env *env, bool *addr[3]);
		int		circulate(t_tick_data *data);
	}
	namespace trade
	{
		int			open(
			t_env		*env,				//trade environment
			int16_t		order_type,			//-1 = short, +1 = long //need 2 add limit and stop orders
			float		lot_size,			//usually > 0.01 < 500
			int16_t		tp_mode = 0,		//0 = no tp, 1 = hard price, 2 = % ticker move, 3 = % trade profit to account
			int16_t		sl_mode = 0,		//0 = no sl, 1 = hard price, 2 = % ticker move, 3 = % trade profit to account
			float		tp = 0,				//either 0 or hard tp (1), ticker % runup from entry (2), trade % runup from entry (3)
			float		sl = 0,				//either 0 hard sl (1), ticker % dd from entry (2), trade % dd from entry (3)
			//int16_t	magic = 0,				//magic = 0 number (order unique identifier)
			size_t		ticket = 0,			//ticket number, leave 0 for default
			int			magic = 0,				//unique identifier (pass number?)
			std::string	comment = "\0");
		int			close(
			t_env		*env,				//trade environment
			int16_t		sort = 1,			//1 = FIFO, 2 = LIFO, 3 = random
			int16_t		mode = 1,			//1 = single, 2 = all, 3 = all longs, 4 = all shorts
			size_t		ticket = 0,			//order ticket to close, default 0 for newest, 1 for oldest
			int			magic = 0,				//unique identifier (pass number?)
			std::string	comment = "\0");	//unique comment to log
		int			modify(
			t_env		*env,			//trade environment
			size_t		ticket,			//order ticket to modify
			int			magic,			//order magic to modify
			float		new_lot_size,	//modified order lot_size
			float		new_tp,			//modified order tp_price
			float		new_sl,			//modified order sl_price
			size_t		new_ticket,		//modified order ticket
			int			new_magic,
			std::string	new_comment);	//modified order comment
	}
}
namespace calculate
{
	//P&L...
}
namespace compliance
{
	int		argv(int ac, char **av);
}
namespace transform
{
	namespace time
	{
		std::string get_timestamp(time_t epochTime, const char* format = "%Y%m%d %H:%M:%S");
		time_t get_epoch(const char* theTime, const char* format = "%Y%m%d %H:%M:%S"); //+%f microseconds?
	}
}
int		put_error(string str, int err_code);
int		on_tick(t_env *env, bool *addr[3]);
int		get_config(t_env *env, string file_name);
int		prepare_bt(t_env *env);