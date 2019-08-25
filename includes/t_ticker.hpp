#pragma once

#define NB_LONG(X)			env->ticker->nb_long[X]
#define NB_SHORT(X)			env->ticker->nb_short[X]
#define TICKER_SPREAD		env->ticker->spread
#define TICKER_LEVEAGE

typedef struct s_ticker
{
	uint16_t	type;				//see param file [param parsing]
	uint16_t	price_mode;			//0 = MODE_OHLC, 1 = MODE_TICK [param parsing]
	uint16_t	leverage;			//min 1, max 1000 [param parsing]
	uint16_t	spread_mode;		//see param file [param parsing]
	uint16_t	spread;				//one side spread, fixed or % (not to be computed if spread is custom) [param parsing]
	uint16_t	lot_com_pct;		//commission paid per lot to the broker (over spread) [param parsing]
	size_t		lot_size;			//1 to 1M (i.e. BTCUSD = 1, EURUSD = 100000) [param parsing]
	size_t		size;				//nb TICKS or OHLC in csv file (precomputational) [on calculate]
//  size_t		current_row;		//current candle / tick being analysed [on calculate]
	double		min_lot;			//usualy 0.1 or 0.01 (microlots) even 0.001 for BTC [param parsing]
	double		max_lot;			//usualy under 500 [param parsing]
	std::string	iso_base;			//ISO 4217 standard base currency (3 chars + NULL) [param parsing]
	std::string	iso_quote;			//ISO 4217 standard quote currency (3 chars + NULL) [param parsing]
	std::string	denom;				//ISO base + ISO quote + NULL [param parsing]
	uint16_t	nb_long[MAX_PASS];	//true if at least 1 trade is long [on calculate]
	uint16_t	nb_short[MAX_PASS];	//true if at least 1 trade is short [on calculate]
}				t_ticker;
