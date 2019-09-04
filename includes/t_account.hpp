#pragma once

// #include <deque>
// #include <unordered_map>
#include <mutex>

#include "t_order.hpp"

#define NB_LONG				env->acc.order_book.nb_long // [X]
#define NB_SHORT			env->acc.order_book.nb_short // [X]
#define STARTING_EQ 		env->acc.value.starting_equity
#define FLOATING_EQ			env->acc.value.equity // [X]
#define BALANCE				env->acc.value.balance // [X]

typedef struct s_order_book
{
	std::mutex				mtx;		// to lock access to order_book->active
	// unordered map allows for fast lookup and insertion by order ticket whatever the amount of trades running
	// array version:
	t_order 				order[MAX_PASS][ORDER_BOOK_SIZE];
	// map version:
	// std::unordered_map<int, t_order> map[MAX_PASS];
	// queue version:
	// std::deque<t_order>		active[MAX_PASS];	// running trades
	// std::deque<t_order>		history[MAX_PASS];	// passed trades
	int						nb_long[MAX_PASS];	//true if at least 1 trade is long [on calculate]
	int						nb_short[MAX_PASS];	//true if at least 1 trade is short [on calculate]
}				t_order_book;

typedef struct s_value
{
	std::mutex 				mtx;
	double					starting_equity;			// starting equity for all pass(usually account deposit) [param parsing]
	double					equity[MAX_PASS];			// floating account value (floating p&l) [on calculate]
	double					balance[MAX_PASS];			// current balance
}				t_value;

typedef struct s_account
{
	// account id
	std::string				holder;						// account holder name [param parsing]
	std::string				number;						// account number (string to contain atenant zeros) [param parsing]
	std::string				fiat_iso;					// account currency (3 chars + NULL terminated) [param parsing]
	// account numerical details
	int				margin_call_pct;			// margin call *100 if applicable [param parsing]	
	t_order_book			order_book;
	t_value					value;
}				t_account;
