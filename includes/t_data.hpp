#pragma once

# include <deque>		//C++ queuing/dequeuing lib

//env related defines
// #define ASK(X)				env->data->ask->at(X)
// #define BID(X)				env->data->bid->at(X)
// #define VOL(X)				env->data->volume->at(X)
#define EPOCH(X)				env->epoch->at(X)
#define LAST(X)					env->price->at(X)

//typedef int8_t bool
typedef struct s_tick_data
{
	//vector is slower for reading data FIFO circular but faster to compute (using math libs)
	std::deque<size_t>	*epoch;		//epoch timestamp queue7
	std::deque<double>	*ask;		//tick price queue
	std::deque<double>	*bid;		//tick price queue
	std::deque<double>	*volume;		//volume queue
}				t_tick_data;

// granularity for ohlc data is hardly standardized, some brokers provide
// both volumes up and down, others provide spread..
typedef struct s_ohlc_data
{
	std::deque<size_t>	*epoch;				//epoch timestamp
	std::deque<double>	*open;				//open
	std::deque<double>	*high;				//high
	std::deque<double>	*low;				//low
	std::deque<double>	*close;				//close
	std::deque<double>	*volume;				//volume (up + down)
	// std::deque<double>	*average_spread;	//volume (up + down)
}				t_ohlc_data;
