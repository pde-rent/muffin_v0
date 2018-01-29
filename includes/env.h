#pragma once

# define BUFF_SIZE			(1024 * 16)
# define TICK_BUFF_SIZE		50000
# define TICK[X]			data->px->at(X)
# define VOL[X]				data->vol->at(X)
# define EPOCH[X]			data->epoch->at(X)

typedef char* string;
//typedef int8_t bool
typedef struct s_tick_data
{
	std::deque<size_t>	*epoch; //vector is slower for reading data FIFO circular
	std::deque<float>	*px;
	std::deque<float>	*vol;
}				t_tick_data;
typedef struct s_ohlc_data
{
	std::deque<size_t>	*epoch;
	std::deque<float>	*open;
	std::deque<float>	*high;
	std::deque<float>	*low;
	std::deque<float>	*close;
	std::deque<float>	*vol;
}				t_ohlc_data;
typedef struct s_account
{
	uint16_t	margin_call_pct;
	string		holder;
	string		number;
	string		fiat_iso;
	float		starting_eq;
	float		floating_eq;
	float		balance;
}				t_account;
typedef struct s_ticker
{
//	uint16_t	lot_com_pct;
	size_t		nb_rows;
	size_t		current_row;
	uint16_t	spread_pct;
	string		iso_base;		//ISO 4217 standard base currency
	string		iso_quote;		//ISO 4217 standard quote currency
	bool		ongoing_long;
	bool		ongoing_short;
}				t_ticker;
typedef struct s_open_order
{
	uint16_t	order_type;
	uint16_t	lot_size;
	float		tp_price;
	float		sl_price;
	float		order_profit;
	float		order_max_dd;
	float		order_max_runup;
	size_t		epoch_open;	
	size_t		epoch_close;
}				t_open_order;
typedef struct s_closed_order
{
	uint16_t	order_type;
	uint16_t	lot_size;
	float		order_profit;
	size_t		epoch_open;
	size_t		epoch_close;
}				t_closed_order;
typedef struct s_order_book
{
	std::deque<t_open_order> *order;
}				t_order_book;
typedef struct s_order_history
{
	std::deque<t_closed_order> *order;
}				t_order_history;
typedef struct s_risk
{
	uint16_t	max_trades;
	uint16_t	max_short;
	uint16_t	max_long;
	uint16_t	max_dd_pct_acc;
	uint16_t	max_dd_pct_ticker;
	uint16_t	target_pct_acc;
	uint16_t	target_pct_ticker;
	int			max_dd_fiat;
	int			target_fiat;
}				t_risk;
typedef struct s_env
{
	//t_ohlc_data		*data;
	t_risk			*rsk;
	t_ticker		*ticker;
	t_account		*acc;
	t_tick_data		*data;
	t_order_book	*order_book;
	t_order_history	*order_history;
}				t_env;