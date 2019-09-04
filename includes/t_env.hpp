#pragma once

// constants
#include "params.hpp"

// structs
// #include "t_worker.hpp"
#include <vector>
#include "t_ticker.hpp"
#include "t_account.hpp"
#include "t_risk.hpp"
#include "t_data.hpp"
// classes
#include "ThreadPool.hpp"

typedef struct s_env
{
	// multithreading env:
	// std::mutex stream_mutex // >> global variable
	// thread workers are replaced now by a threadpool (only class)
	// tasks are given to the tread_pool as thread_pool->enqueue(function, args)
	// function can be a lamba, and args can be null
	volatile bool					new_data_available;
	volatile bool					consumer_busy;
	std::mutex 						stream_mutex;
	// t_worker						*stream_worker;
	// t_backtest_worker			*backtest_worker;
	ThreadPool 						*thread_pool;
	std::vector<std::future<int>>	thread_yields;		// the order_book list of orders (here an STL queue is the container)

	int 							nb_cores;			// cpu cores for multithreading >> defaults to 4
	int								nb_pass;			// nb_pass for backtest parameters, depends on min_ and max_
	int								step[NB_INDICATORS];// step between indicator parameters

	// backtest env
	t_risk							risk;				// risk/reward environment
	t_ticker						ticker;				// broker's quoted instrument environment
	t_account						acc;				// broker's hosted account environment

	// data env
	// key pointers // points to tick->bid or ohlc->close
	// dequeue mode:
	// std::deque<double>			*price;				// points to tick->bid or ohlc->close
	// std::deque<unsigned int>	*epoch;
	// array mode:
	double							*price;
	unsigned int					*epoch;

	t_tick_data						*ticks;				// tick data store
	t_ohlc_data						*ohlc;				// ohlc data store

	// paths
	std::string						data_folder;
	std::string						data_file_path;
	std::string						config_file_path;
	std::string						spread_file_path;
}				t_env;
