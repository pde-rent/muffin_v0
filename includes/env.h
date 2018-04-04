#pragma once

# define ASK(X)				env->data->ask->at(X)
# define BID(X)				env->data->bid->at(X)
# define VOL(X)				env->data->vol->at(X)
# define EPOCH(X)			env->data->epoch->at(X)
# define NB_LONG(X)			env->ticker->nb_long[X]
# define NB_SHORT(X)		env->ticker->nb_short[X]
# define OPEN				trigger::trade::open
# define CLOSE				trigger::trade::close
# define SPREAD				env->ticker->spread

typedef char* string;
//typedef int8_t bool
typedef struct s_tick_data
{
	//vector is slower for reading data FIFO circular but faster to compute (using math libs)
	std::deque<size_t>	*epoch;		//epoch timestamp queue7
	std::deque<float>	*ask;		//tick price queue
	std::deque<float>	*bid;		//tick price queue
	std::deque<float>	*vol;		//volume queue
}				t_tick_data;
typedef struct s_ohlc_data
{
	std::deque<size_t>	*epoch;		//epoch timestamp queue
	std::deque<float>	*open;		//open queue
	std::deque<float>	*high;		//high queue
	std::deque<float>	*low;		//low queue
	std::deque<float>	*close;		//close queue
	std::deque<float>	*vol;		//volume queue
}				t_ohlc_data;
typedef struct s_account
{
	uint16_t		margin_call_pct;//margin call *100 if applicable [param parsing]
	string			holder;			//account holder name [param parsing]
	string			number;			//account number (string to contain atenant zeros) [param parsing]
	string			fiat_iso;		//account currency (3 chars + NULL terminated) [param parsing]
	float			starting_eq;	//starting equity (usually account deposit) [param parsing]
	float			floating_eq[MAX_PASS];	//floating account equity (floating p&l) [on calculate]
	float			balance[MAX_PASS];		//current balance
}				t_account;
typedef struct s_ticker
{
	uint16_t	type;				//see param file [param parsing]
	uint16_t	price_mode;			//1 = tick, 2 = OHLC [param parsing]
	uint16_t	leverage;			//min 1, max 1000 [param parsing]
	uint16_t	spread_mode;		//see param file [param parsing]
	uint16_t	spread;				//one side spread, fixed or % (not to be computed if spread is custom) [param parsing]
	uint16_t	lot_com_pct;		//commission paid per lot to the broker (over spread) [param parsing]
	size_t		lot_size;			//1 to 1M (i.e. BTCUSD = 1, EURUSD = 100000) [param parsing]
	size_t		size;				//nb TICKS or OHLC in csv file (precomputational) [on calculate]
//  size_t		current_row;		//current candle / tick being analysed [on calculate]
	float		min_lot;			//usualy 0.1 or 0.01 (microlots) even 0.001 for BTC [param parsing]
	float		max_lot;			//usualy under 500 [param parsing]
	string		iso_base;			//ISO 4217 standard base currency (3 chars + NULL) [param parsing]
	string		iso_quote;			//ISO 4217 standard quote currency (3 chars + NULL) [param parsing]
	string		denom;				//ISO base + ISO quote + NULL [param parsing]
	uint16_t	nb_long[MAX_PASS];			//true if at least 1 trade is long [on calculate]
	uint16_t	nb_short[MAX_PASS];			//true if at least 1 trade is short [on calculate]
}				t_ticker;
typedef struct s_open_order
{
	int16_t		type;			//1 = long, -1 = short, 2 = buy limit, -2 = sell limit, 3 = buy stop, -3 = sell stop
	float		entry_price;	//price of theoretical execution
	float		lot_size;		//usually between 0.01 and 500 (volume = order.lot_size*ticker.lot_size)
	float		tp_price;		//adjusted take profit [on calculate]
	float		sl_price;		//adjusted stop loss [on calculate]
	float		profit;			//current order profit if running [on calculate]
	float		order_max_dd;	//max trade/account dd reached since entry [on calculate]
	float		order_max_runup;//max trade/account run-up reached since entry [on calculate]
	size_t		epoch_create;	//epoch time of ticket creation [on calculate]
	size_t		epoch_open;		//epoch time of market execution (approx = to open for market orders) [on calculate]
	size_t		epoch_timeout;	//expiry for pending orders, force close for market orders [on calculate]
	size_t		ticket;			//order unique identifier [on calculate]
	int			magic;			//unique identifier (pass number?)
	std::string comment;		//order comment [on calculate?]
}				t_open_order;
typedef struct s_closed_order
{
	int16_t		type;			//1 = long, -1 = short, 2 = buy limit, -2 = sell limit, 3 = buy stop, -3 = sell stop
	float		entry_price;	//price of theoretical execution
	float		close_price;	//price at which the order has been closed
	float		lot_size;		//usually between 0.01 and 500 (volume = order.lot_size*ticker.lot_size)
	float		profit;			//profit achieved on the closed trade
	float		order_max_dd;	//max DD reached by the trade
	float		order_max_runup;//max run-up reached by the trade
	size_t		epoch_create;	//epoch time of ticket creation
	size_t		epoch_open;		//epoch time of market execution (approx = to open for market orders)
	size_t		epoch_close;	//epoch time of ticket archival (order close)
	size_t		ticket;			//order unique identifier
	int			magic;			//unique identifier (pass number?)
	std::string	comment;		//order comment
}				t_closed_order;
typedef struct s_risk
{
	uint16_t	max_trades;			//max overall trades running [param parsing]
	uint16_t	max_short;			//max short tardes running [param parsing]
	uint16_t	max_long;			//max long trades running [param parsing]
	uint16_t	max_dd_pct_acc;		//max relative trade/acc balance DD in % [param parsing]
	uint16_t	max_dd_pct_ticker;	//max relative ticker DD in % since entry [param parsing]
	uint16_t	target_pct_acc;		//max relative trade/acc balance run-up in % [param parsing]
	uint16_t	target_pct_ticker;	//max relative ticker run-up in % since entry [param parsing]
	float		max_dd_fiat;		//max trade/acc balance DD in fiat [param parsing]
	float		max_dd_ticker;		//max ticker dd in fiat [param parsing]
}				t_risk;
typedef struct s_env
{
	//t_ohlc_data		*data;
	t_risk						*rsk;						//risk/reward environment
	t_ticker					*ticker;					//broker's quoted instrument environment
	t_account					*acc;						//broker's hosted account environment
	t_tick_data					*data;						//historical data environment
	std::deque<t_open_order>	*order_book[MAX_PASS];		//the order_book list of orders (here an STL queue is the container)
	std::deque<t_closed_order>	*order_history[MAX_PASS];	//the order_book list of orders (here an STL queue is the container)
	string						parent_folder;
	string						data_file_name;
	string						config_file_name;
	string						spread_file_name;
	int							pass;
	int							step[NB_INDICATORS];
}				t_env;
