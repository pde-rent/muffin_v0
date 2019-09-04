#pragma once

typedef struct s_risk
{
	int	max_trades;				//max overall trades running [param parsing]
	int	max_short;				//max short tardes running [param parsing]
	int	max_long;				//max long trades running [param parsing]
	double		max_dd_pct_acc;			//max relative trade/acc balance DD in % [param parsing]
	double		max_dd_pct_ticker;		//max relative ticker DD in % since entry [param parsing]
	double		max_dd_acc;				//max trade/acc balance DD in fiat [param parsing]
	double		max_dd_ticker;			//max ticker dd in fiat [param parsing]
	double		max_runup_acc;			//max absolute run-up of account in base currency since entry [param parsing]
	double		max_runup_ticker;		//max absolute run-up of ticker in base currency since entry[param parsing]
	double		max_runup_pct_acc;		//max relative trade/acc balance run-up in % [param parsing]
	double		max_runup_pct_ticker;	//max relative ticker run-up in % since entry [param parsing]
}				t_risk;
