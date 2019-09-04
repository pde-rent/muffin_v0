#pragma once

// TODO: add average / median p&l?
typedef struct s_order
{
	int				type;			//1 = long, -1 = short, 2 = buy limit, -2 = sell limit, 3 = buy stop, -3 = sell stop
	int				status;			//1 = long, -1 = short, 2 = buy limit, -2 = sell limit, 3 = buy stop, -3 = sell stop
	double			entry_price;	//price of theoretical execution
	double			modify_price;	//price at which the order was last modified
	// TODO: add a timeout price?
	double			close_price;	//price at which the order has been closed
	double			volume;			//usually lots between 0.01 and 500 (volume = order.lot_size*ticker.lot_size)
	double			tp_price;		//adjusted take profit [on calculate]
	double			sl_price;		//adjusted stop loss [on calculate]
	double			profit;			//current order profit if running [on calculate]
	double			max_loss;		//max trade/account dd reached since entry [on calculate]
	double			max_profit;		//max trade/account run-up reached since entry [on calculate]
	unsigned int	time_in_loss;	
	unsigned int	time_in_profit;
	unsigned int	epoch_create;	//epoch time of ticket creation [on calculate]
	unsigned int	epoch_open;		//epoch time of market execution (approx = to open for market orders) [on calculate]
	unsigned int	epoch_modify;	//epoch time of last modification [on calculate]
	unsigned int	epoch_timeout;	//expiry for pending orders, force close for market orders [on calculate]
	unsigned int	epoch_close;	//epoch time of ticket archival (order close)
	int				ticket;			//order unique identifier [on calculate]
	int				magic;			//unique identifier (pass number?)
	std::string		description;	//json order description with comment
}				t_order;
