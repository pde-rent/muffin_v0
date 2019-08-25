#include "main.hpp"

namespace order
{
	static size_t global_ticket = 0;

	int			open(
		t_env		*env,
		int16_t		order_type,		//-1 = short, +1 = long //need 2 add limit and stop orders
		double		volume,			//usually lots > 0.01 < 500
		int16_t		tp_mode,		//0 = no tp, 1 = hard price, 2 = % ticker move, 3 = % trade profit to account
		int16_t		sl_mode,		//0 = no sl, 1 = hard price, 2 = % ticker move, 3 = % trade profit to account
		double		tp,				//either 0 or hard tp (1), ticker % runup from entry (2), trade % runup from entry (3)
		double		sl,				//either 0 hard sl (1), ticker % dd from entry (2), trade % dd from entry (3)
		size_t		ticket,			//ticket number, leave 0 for default
		int			magic,			//magic number (order unique identifier)
		std::string	comment)
	{
		t_order o; //new order

		o.epoch_open = EPOCH(0);
		o.entry_price = LAST(0);//(order_type > 0 ? ASK(0) : BID(0));
		o.volume = volume;
		o.type = order_type;
		o.comment = (comment.empty() ? "[open: " + std::to_string(o.epoch_open) + "]" : comment);
		o.ticket = (ticket ? ticket : ++global_ticket); //default ticket is last in ticket + 1
		o.magic = magic;
		//printf("Opening order ticket %zu for pass %d\n", o.ticket, magic);
		if (tp_mode)
		{
			if (tp_mode == 1)
				o.tp_price = tp;
			else if (tp_mode == 2)
				o.tp_price = o.entry_price * (1 + (order_type * (tp / 100)));
			else if (tp_mode == 3)
				o.tp_price = o.entry_price * 1 +
				(order_type * (env->acc->balance[magic] * (tp / 100)) / ((env->ticker->lot_size * volume)));
		}
		if (sl_mode)
		{
			if (sl_mode == 1)
				o.tp_price = sl;
			else if (sl_mode == 2)
				o.sl_price = o.entry_price * (1 + (-order_type * (sl / 100)));
			else if (sl_mode == 3)
				o.sl_price = o.entry_price * 1 +
				(-order_type * (env->acc->balance[magic] * (sl / 100)) / ((env->ticker->lot_size * volume)));
		}
		env->order_book[magic]->push_front(o);
		if (order_type == 1) NB_LONG(magic)++;
		else if (order_type == -1) NB_SHORT(magic)++;
		return (1);
	}
	int			close(
		t_env		*env,
		// int16_t		sort,			//1 = FIFO, 2 = LIFO, 3 = random
		// int16_t		mode,			//1 = single, 2 = all, 3 = all longs, 4 = all shorts
		size_t		ticket,			//ticket if single close, default is either oldest (FIFO) or newest (LIFO)
		int			magic,
		std::string	comment)		//unique comment to log
	{
		//multiple open/close not yet implemented
		t_order o;
		/*
		for (int i = 0; i < (NB_LONG + NB_SHORT); i++)		//scan order book for matching order ticket
			if (env->order_book->at(i).ticket == ticket)
			{
				o = env->order_book->at(i);
				env->order_book->erase(i);
				break;
			}
		*/
		for (std::deque<t_order>::iterator i = env->order_book[magic]->begin(); i != env->order_book[magic]->end(); ++i)
			if ((*i).ticket == ticket)
			{
				o = *i;
				env->order_book[magic]->erase(i);
				break;
			}
		o.close_price = LAST(0);//(o.type > 0 ? BID(0) : ASK(0));
		o.epoch_close = EPOCH(0);
		o.profit = (o.close_price - o.entry_price)
			* o.type * o.entry_price
			* o.volume * env->ticker->lot_size;
		o.comment += "[close: " + std::to_string(EPOCH(0)) + "]";
		if (!comment.empty()) {
			o.comment += comment;
		}
		env->acc->balance[magic] += o.profit;
		env->order_history[magic]->push_front(o);
		if (o.type == 1) NB_LONG(magic)--;
		else if (o.type == -1) NB_SHORT(magic)--;
		printf("magic: %d balance: %f\n", magic, env->acc->balance[magic]);
		return (1);
	}
	int			modify(
		t_env		*env,			//trade environment
		size_t		ticket,			//order ticket to modify
		int			magic,			//order magic to modify
		double		new_volume,	//modified order volume
		double		new_tp,			//modified order tp_price
		double		new_sl,			//modified order sl_price
		size_t		new_ticket,		//modified order ticket
		int			new_magic,
		std::string	new_comment)	//modified order comment
	{
		//multiple open/close not yet implemented
		t_order o;
		/*
		for (int i = 0; i < (NB_LONG + NB_SHORT); i++)		//scan order book for matching order ticket
			if (env->order_book->at(i).ticket == ticket)
				o = env->order_book->at(i);
		*/
		for (std::deque<t_order>::iterator i = env->order_book[magic]->begin(); i != env->order_book[magic]->end(); ++i)
			if ((*i).ticket == ticket)
			{
				o = *i;
				break;
			}
		o.comment += "[modify: " + std::to_string(EPOCH(0)) + "]";
		if (new_volume) {
			o.volume = new_volume;
			o.comment += "[new lot-size: " + std::to_string(new_volume) + "]";
		}
		if (new_tp) {
			o.tp_price = new_tp;
			o.comment += "[new tp: " + std::to_string(new_tp) + "]";
		}
		if (new_sl) {
			o.sl_price = new_sl;
			o.comment += "[new sl: " + std::to_string(new_sl) + "]";
		}
		if (new_ticket) {
			o.ticket = new_ticket;
			o.comment += "[new ticket: " + std::to_string(new_ticket) + "]";
		}
		if (new_magic) {
			o.magic = new_magic;
			o.comment += "[new magic: " + std::to_string(new_magic) + "]";
		}
		if (!new_comment.empty()) {
			o.comment += new_comment;
		}
		return (1);
	}
}
