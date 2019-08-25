#pragma once

//constants
#include "params.hpp"

//structs
#include "t_ticker.hpp"
#include "t_account.hpp"
#include "t_order.hpp"
#include "t_risk.hpp"
#include "t_data.hpp"

typedef struct s_env
{
	bool						lock[3];					// manual data lock
	t_risk						*risk;						// risk/reward environment
	t_ticker					*ticker;					// broker's quoted instrument environment
	t_account					*acc;						// broker's hosted account environment
	//t_ohlc_data				*data;
	t_tick_data					*ticks;						// tick data store
	t_ohlc_data					*ohlc;						// ohlc data store
	std::deque<double>			*price;						// points to tick->bid or ohlc->close
	std::deque<size_t>			*epoch;						// points to tick->bid or ohlc->close
	std::deque<t_order>			*order_book[MAX_PASS];		// the order_book list of orders (here an STL queue is the container)
	std::deque<t_order>			*order_history[MAX_PASS];	// the order_book list of orders (here an STL queue is the container)
	std::string					parent_folder;
	std::string					ohlc_file_name;
	std::string					ticks_file_name;
	std::string					config_file_name;
	std::string					spread_file_name;
	int							pass;
	int							step[NB_INDICATORS];
}				t_env;
