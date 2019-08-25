#pragma once

typedef struct s_order
{
	int16_t		type;			//1 = long, -1 = short, 2 = buy limit, -2 = sell limit, 3 = buy stop, -3 = sell stop
	double		entry_price;	//price of theoretical execution
	double		close_price;	//price at which the order has been closed
	double		volume;			//usually lots between 0.01 and 500 (volume = order.lot_size*ticker.lot_size)
	double		tp_price;		//adjusted take profit [on calculate]
	double		sl_price;		//adjusted stop loss [on calculate]
	double		profit;			//current order profit if running [on calculate]
	double		order_max_dd;	//max trade/account dd reached since entry [on calculate]
	double		order_max_runup;//max trade/account run-up reached since entry [on calculate]
	size_t		epoch_create;	//epoch time of ticket creation [on calculate]
	size_t		epoch_open;		//epoch time of market execution (approx = to open for market orders) [on calculate]
	size_t		epoch_timeout;	//expiry for pending orders, force close for market orders [on calculate]
	size_t		epoch_close;	//epoch time of ticket archival (order close)
	size_t		ticket;			//order unique identifier [on calculate]
	int			magic;			//unique identifier (pass number?)
	std::string comment;		//order comment [on calculate?]
}				t_order;
