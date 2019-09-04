#include "main.hpp"

namespace order
{
	static unsigned int global_ticket = 0;

	static inline std::string get_type(t_order o)
	{
		switch (o.type) {
			case (MODE_BUY): return "market::buy @ask";
			case (MODE_SELL): return "market::sell @bid";
			case (MODE_BUY_LIMIT): return "limit::buy @ask";
			case (MODE_SELL_LIMIT): return "limit::sell @bid";
			case (MODE_BUY_STOP): return "stop::buy @ask";
			case (MODE_SELL_STOP): return "stop::sell @bid";
		}
		return "unknown";
	}
	static inline double get_tp(t_env *env, int tp_mode, double tp, t_order o)
	{
		if (!tp_mode) { return (0.0); }
		double tp_price;

		if (tp_mode == MODE_TP_PRICE) {
			tp_price = tp;
		} else if (tp_mode == MODE_TP_PCT_TICKER) {
			tp_price = o.entry_price * (1 + (o.type * (tp / 100)));
		} else if (tp_mode == MODE_TP_PCT_ACC) {
			tp_price = o.entry_price * 1 +
			(o.type * (env->acc.value.balance[o.magic] * (tp / 100)) / ((env->ticker.lot_size * o.volume)));
		}
		return (tp_price);
	}
	static inline double get_sl(t_env *env, int sl_mode, double sl, t_order o)
	{
		if (!sl_mode) { return (0.0); }
		double sl_price;
	
		if (sl_mode == MODE_SL_PRICE) {
			sl_price = sl;
		} else if (sl_mode == MODE_SL_PCT_TICKER) {
			sl_price = o.entry_price * ((-1) + (o.type * (sl / 100)));
		} else if (sl_mode == MODE_SL_PCT_ACC) {
			sl_price = o.entry_price * (-1) +
			(o.type * (env->acc.value.balance[o.magic] * (sl / 100)) / ((env->ticker.lot_size * o.volume)));
		}
		return (sl_price);
	}
	static inline std::string get_open_description(t_order o, std::string comment)
	{
		return (
		// order infos
		  std::string() +
		  "{type: \"" + order::get_type(o) + "\""
		+ ",magic:" + std::to_string(o.magic)
		+ ",ticket:" + std::to_string(o.ticket)
		// open data
		+ ",\nopen:"
		+ "{epoch:" + std::to_string(o.epoch_open)
		+ ",price:" + std::to_string(o.entry_price)
		+ ",volume:" + std::to_string(o.volume)
		+ ",tp:" + std::to_string(o.tp_price)
		+ ",sl:" + std::to_string(o.sl_price)
		+ ",comment: \"" + comment + "\""
		+ "}");
	}
	static inline std::string get_modify_description(t_order o, std::string comment)
	{
		// comment = comment.empty() ? "" : ",comment: \"" + comment + "\"";
		return (
		// modify data
		  std::string() +
		  "\nmodify:"
		+ "{epoch:" + std::to_string(o.epoch_modify)
		+ ",price:" + std::to_string(o.modify_price)
		+ ",volume:" + std::to_string(o.volume)
		+ ",tp:" + std::to_string(o.tp_price)
		+ ",sl:" + std::to_string(o.sl_price)
		+ ",comment: \"" + comment + "\""
		+ "}");
	}
	static inline std::string get_close_description(t_order o, std::string comment)
	{
		return (
		// close data
		  std::string() +
		  "\nclose:"
		+ "{epoch:" + std::to_string(o.epoch_close)
		+ ",price:" + std::to_string(o.close_price)
		+ ",volume left:" + std::to_string(o.volume)
		+ ",comment: \"" + comment + "\""
		+ "}");
	}
	// array version:
	static inline int ffr(t_order *o) // first free row in the order book
	{
		for (int i = 0; i < ORDER_BOOK_SIZE; i++) {
			if (o[i].status == MODE_EMPTY_SLOT) {
				return (i);
			}
		}
		return (put_error("order_book reached ORDER_BOOK_SIZE", -1));
	}
	static inline int order_book_insert(t_env *env, t_order o)
	{
		env->acc.order_book.mtx.lock();
		switch (o.type) {
			case (MODE_BUY): NB_LONG[o.magic] += 1; break;
			case (MODE_SELL): NB_SHORT[o.magic] -= 1; break;
		}
		// array version:
		env->acc.order_book.order[o.magic][ffr(&env->acc.order_book.order[o.magic][0])] = o;
		// deque version:
		// env->acc.order_book.map[magic]->push_front(o);
		// map version:
		// env->acc.order_book.map[o.magic][o.ticket] = o;
		env->acc.order_book.mtx.unlock();
		printf("%s\n", o.description.c_str());
		return (1);
	}
	static inline int order_book_remove(t_env *env, t_order o, int partial_close)
	{
		env->acc.order_book.mtx.lock();
		switch (o.type) {
			case (MODE_BUY): NB_LONG[o.magic] -= 1; break;
			case (MODE_SELL): NB_SHORT[o.magic] -= 1; break;
		}
		if (!partial_close) {
			env->acc.order_book.order[o.magic][o.ticket].status = MODE_EMPTY_SLOT;
		} else {
			env->acc.order_book.order[o.magic][o.ticket] = o;
		}
		// array version:
		// deque version:
		// env->acc.order_book.map[magic]->push_front(o);
		// map version:
		// env->acc.order_book.map[o.magic][o.ticket] = o;
		env->acc.order_book.mtx.unlock();
		if (!partial_close) printf("%s,\n", o.description.c_str());
		return (1);
	}
	int			open(
		t_env		*env,
		int			order_type,		//-1 = short, +1 = long //need 2 add limit and stop orders
		double		volume,			//usually lots > 0.01 < 500
		int			tp_mode,		//0 = no tp, 1 = hard price, 2 = % ticker move, 3 = % trade profit to account
		int			sl_mode,		//0 = no sl, 1 = hard price, 2 = % ticker move, 3 = % trade profit to account
		double		tp,				//either 0 or hard tp (1), ticker % runup from entry (2), trade % runup from entry (3)
		double		sl,				//either 0 hard sl (1), ticker % dd from entry (2), trade % dd from entry (3)
		int			ticket,			//ticket number, leave 0 for default
		int			magic,			//magic number (order unique identifier)
		std::string	comment)
	{
		t_order o; //new order // function could return a pointer to created object

		o.epoch_open = EPOCH[0];
		o.entry_price = LAST[0];//(order_type > 0 ? ASK(0) : BID(0));
		o.volume = volume;
		o.type = order_type;
		// o.comment = (comment.empty() ? "[order::open::" + order::get_type(o) + "][epoch: " + std::to_string(o.epoch_open) + "]" : comment);
		o.ticket = (ticket ? ticket : ++global_ticket); //default ticket is last in ticket + 1
		o.magic = magic;
		o.tp_price = get_tp(env, tp_mode, tp, o);
		o.sl_price = get_sl(env, sl_mode, sl, o);
		o.status = MODE_OPEN;
		o.description = get_open_description(o, comment);
		order_book_insert(env, o);
		return (1);
	}
	// 
	int 		close(
		t_env		*env,
		t_order		o,
		double		volume, // volume is at max o.volume, allows for partial close
		std::string comment)
	{
		bool partial_close = false;
		o.close_price = LAST[0];//(o.type > 0 ? BID(0) : ASK(0));
		o.epoch_close = EPOCH[0] ? EPOCH[0] : EPOCH[1];
		if (volume == 0.0 || volume > o.volume) {
			volume = o.volume;
		}
		double partial_profit = (o.close_price - o.entry_price)
			* o.type * o.entry_price
			* volume * env->ticker.lot_size;
		o.profit += partial_profit;
		if (volume != o.volume) {
			partial_close = true;
			comment += "[partial close]";
			o.status = MODE_OPEN;
		} else {
			o.status = MODE_CLOSED;
		}
		o.volume -= volume;
		env->acc.value.mtx.lock();
		env->acc.value.balance[o.magic] += partial_profit;
		env->acc.value.mtx.unlock();
		o.description += get_close_description(o, comment);
		if (!partial_close) {
			o.description += "}"; // order.close is the terminating json block
		}
		order_book_remove(env, o, partial_close);
		return (1);
	}
	t_order		*find(
		t_env		*env,
		int			ticket,		//ticket if single close, default is either oldest (FIFO) or newest (LIFO)
		int			magic)		//unique comment to log
	{
		//multiple open/close not yet implemented
		/*
		t_order o;
		for (int i = 0; i < (NB_LONG + NB_SHORT); i++)		//scan order book for matching order ticket
			if (env->acc.order_book.at(i).ticket == ticket)
			{
				o = env->acc.order_book.at(i);
				env->acc.order_book.erase(i);
				break;
			}
		*/
		// array version:
		for (t_order &o: env->acc.order_book.order[magic]) {
			if (o.ticket == ticket) {
				return (&o);
			}
		}
		// deque version:
		// for (std::deque<t_order>::iterator i = env->acc.order_book.map[magic]->begin(); i != env->acc.order_book.map[magic]->end(); ++i)
		// 	if ((*i).ticket == ticket)
		// 	{
		// 		o = *i;
		// 		env->acc.order_book.map[magic]->erase(i);
		// 		break;
		// 	}
		// map version:
		// env->acc.order_book.map[magic][ticket]; // >> unsafe
		// auto it = env->acc.order_book.map[magic].find(ticket);
		// if (!(it == env->acc.order_book.map[magic].end())) {
		// 	return (&(it->second));
		// }
		// key / val >> order == it.second
		return (NULL);
	}
	int			modify(
		t_env		*env,			//trade environment
		t_order		o,
		double		new_volume,		//modified order volume
		double		new_tp,			//modified order tp_price
		double		new_sl,			//modified order sl_price
		int			new_ticket,		//modified order ticket
		int			new_magic,
		std::string	comment)		//modified order comment
	{
		// o.comment += "[modify: " + std::to_string(EPOCH[0]) + "]";
		if (new_ticket) {
			t_order old = o;
			old.status = MODE_TICKET_CHANGED;
			comment += "[ticket end of life: migrating to: " + std::to_string(new_ticket) + "]"; // finishing json object
			old.description += get_modify_description(o, comment) + "}"; // order.modify is the terminating json block
			// printf("%s,\n", old.description.c_str());
			env->acc.order_book.mtx.lock();
			// map version:
			// env->acc.order_book.map[o.magic][o.ticket] = old;
			// array version:
			env->acc.order_book.order[old.magic][old.ticket] = old;
			env->acc.order_book.mtx.unlock();
			o.ticket = new_ticket;
			comment = "[ticket inception: migrated from: " + std::to_string(old.ticket) + "]";
			o.description = get_open_description(o, comment);
		}
		// changing magic would be very dangerous
		// as we currently map order.magic to backtest #pass
		if (new_magic) {
			o.magic = new_magic;
		}
		if (new_volume) {
			o.volume = new_volume;
		}
		if (new_tp) {
			o.tp_price = new_tp;
		}
		if (new_sl) {
			o.sl_price = new_sl;
		}
		o.description += get_modify_description(o, comment);
		env->acc.order_book.mtx.lock();
		// map version:
		// env->acc.order_book.map[o.magic][o.ticket] = o;
		// array version:
		env->acc.order_book.order[o.magic][o.ticket] = o;
		env->acc.order_book.mtx.unlock();
		return (1);
	}
	int 		close_all(t_env *env, int type, int magic, std::string comment)
	{
		// TODO: should take further parameters:
		// MODE_FIFO,				// close order mode
		// int		sort,	//1 = FIFO, 2 = LIFO, 3 = random
		// MODE_CLOSE_SINGLE,		// close quantity mode
		// int		mode,	//1 = single, 2 = all, 3 = all longs, 4 = all shorts
		int sum = 0;
		// TODO: implement deque version
		// array version:
		for (t_order &o: env->acc.order_book.order[magic]) {
			if (o.status == MODE_OPEN
				&& (magic = MODE_DEFAULT || o.magic == magic)
				&& (type == MODE_DEFAULT || o.type == type)) {
				sum += order::close(env, o, o.volume, "[bulk close]" + comment);
			}
		}
		// map version:
		// for (auto const& [ticket, o] : env->acc.order_book.map) {
		// for (auto &x : env->acc.order_book.map[magic]) {
		// 	t_order o = x.second;
		// 	if (o.status == MODE_OPEN
		// 		&& (magic = MODE_DEFAULT || o.magic == magic)
		// 		&& (type == MODE_DEFAULT || o.type == type)) {
		// 		sum += order::close(env, o, o.volume, "[bulk close]");
		// 	}
		// }
		return (sum);
	}
}
