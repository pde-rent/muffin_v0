#pragma once

typedef struct s_account
{
	uint16_t		margin_call_pct;//margin call *100 if applicable [param parsing]
	std::string		holder;			//account holder name [param parsing]
	std::string		number;			//account number (string to contain atenant zeros) [param parsing]
	std::string		fiat_iso;		//account currency (3 chars + NULL terminated) [param parsing]
	double			starting_eq;	//starting equity (usually account deposit) [param parsing]
	double			floating_eq[MAX_PASS];	//floating account equity (floating p&l) [on calculate]
	double			balance[MAX_PASS];		//current balance
}				t_account;
