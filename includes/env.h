#pragma once

# define BUFF_SIZE			(1024 * 16)
# define TICK_BUFF_SIZE		100
# define TICK(X)			env->data->px->at(X)
# define VOL(X)				env->data->vol->at(X)
# define EPOCH(X)			env->data->epoch->at(X)
# define NB_LONG			env->ticker->nb_long
# define NB_SHORT			env->ticker->nb_short

typedef char* string;
//typedef int8_t bool
typedef struct s_tick_data
{
	//vector is slower for reading data FIFO circular but faster to compute (using math libs)
	std::deque<size_t>	*epoch;		//epoch timestamp queue
	std::deque<float>	*px;		//tick price queue
	std::deque<float>	*vol;		//volume queue
}				t_tick_data;
typedef struct s_ohlc_data
{
	std::deque<size_t>	*epoch;		//epoch timestamp queue
	std::deque<float>	*open;		//epoch open queue
	std::deque<float>	*high;		//epoch high queue
	std::deque<float>	*low;		//epoch low queue
	std::deque<float>	*close;		//epoch close queue
	std::deque<float>	*vol;		//epoch volume queue
}				t_ohlc_data;
typedef struct s_account
{
	uint16_t		margin_call_pct;//margin call *100 if applicable [param parsing]
	std::string		holder;			//account holder name [param parsing]
	std::string		number;			//account number (string to contain atenant zeros) [param parsing]
	char			fiat_iso[4];	//account currency (3 chars + NULL terminated) [param parsing]
	float			starting_eq;	//starting equity (usually account deposit) [param parsing]
	float			floating_eq;	//floating account equity (floating p&l) [on calculate]
	float			balance;		//current balance
}				t_account;
typedef struct s_ticker
{
	uint16_t	price_mode;		//1 = tick, 2 = OHLC [param parsing]
	uint16_t	leverage;		//min 1, max 1000 [param parsing]
	uint16_t	spread_mode;	//1 = fixed, 2 = proportionnal, 3 = custom entries (to implement) [param parsing]
	uint16_t	spread;			//one side spread, fixed or % (not to be computed if spread is custom) [param parsing]
	uint16_t	lot_com_pct;	//commission paid per lot to the broker (over spread) [param parsing]
	size_t		lot_size;		//1 to 1M (i.e. BTCUSD = 1, EURUSD = 100000) [param parsing]
	size_t		size;			//nb TICKS or OHLC in csv file (precomputational) [on calculate]
//  size_t		current_row;	//current candle / tick being analysed [on calculate]
	float		min_lot;		//usualy 0.1 or 0.01 (microlots) [param parsing]
	float		max_lot;		//usualy under 500 [param parsing]
	char		iso_base[4];	//ISO 4217 standard base currency (3 chars + NULL) [param parsing]
	char		iso_quote[4];	//ISO 4217 standard quote currency (3 chars + NULL) [param parsing]
	char		denom[7];		//ISO base + ISO quote + NULL [param parsing]
	uint16_t	nb_long;		//true if at least 1 trade is long [on calculate]
	uint16_t	nb_short;		//true if at least 1 trade is short [on calculate]
}				t_ticker;
typedef struct s_open_order
{
	int16_t		type;			//1 = long, -1 = short, 2 = buy limit, -2 = sell limit, 3 = buy stop, -3 = sell stop
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
	std::string comment;		//order comment [on calculate?]
}				t_open_order;
typedef struct s_closed_order
{
	int16_t		type;			//1 = long, -1 = short, 2 = buy limit, -2 = sell limit, 3 = buy stop, -3 = sell stop
	float		lot_size;		//usually between 0.01 and 500 (volume = order.lot_size*ticker.lot_size)
	float		profit;			//profit achieved on the closed trade
	float		order_max_dd;	//max DD reached by the trade
	float		order_max_runup;//max run-up reached by the trade
	size_t		epoch_create;	//epoch time of ticket creation
	size_t		epoch_open;		//epoch time of market execution (approx = to open for market orders)
	size_t		epoch_close;	//epoch time of ticket archival (order close)
	size_t		ticket;			//order unique identifier
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
	int			max_dd_fiat;		//max trade/acc balance DD in fiat [param parsing]
	int			target_fiat;		//max ticker run-up in fiat [param parsing]
}				t_risk;
typedef struct s_env
{
	//t_ohlc_data		*data;
	t_risk						*rsk;			//risk/reward environment
	t_ticker					*ticker;		//broker's quoted instrument environment
	t_account					*acc;			//broker's hosted account environment
	t_tick_data					*data;			//historical data environment
	std::deque<t_open_order>	*order_book;	//the order_book list of orders (here an STL queue is the container)
	std::deque<t_closed_order>	*order_history;	//the order_book list of orders (here an STL queue is the container)
}				t_env;