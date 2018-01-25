#pragma once

typedef char* string;
//typedef int8_t bool
typedef struct s_data
{
	size_t		*epoch;
	float		*px;
	float		*vol;
}				t_data;
typedef struct s_account
{
	uint16_t	margin_call_pct;
	string		holder;
	int			number;
	float		starting_eq;
	float		floating_eq;
	float		balance;
}				t_account;
typedef struct s_ticker
{
	uint16_t	lot_com_pct;
	uint16_t	spread_pct;
	string		iso_base;		//ISO 4217 standard base currency
	string		iso_quote;		//ISO 4217 standard quote currency
	bool		ongoing_long;
	bool		ongoing_short;
}				t_ticker;
typedef struct s_order
{
	size_t		epoch_open;
	size_t		epoch_close;
	uint16_t	lot_size;
	uint16_t	risk_pct_ticker;
	uint16_t	target_pct_ticker;
}				t_order;
typedef struct s_strategy
{
	uint16_t	risk_pct_acc;
	uint16_t	target_pct_acc;
	uint16_t	risk_pct_ticker;
	uint16_t	target_pct_ticker;
}				t_strategy;
