#include "main.hpp"

#define PARAM_CNT			30

int		get_config(t_env *env, std::string file_name)
{
	FILE		*file;
	char		c;
	int 		i;
	std::string	param[PARAM_CNT]; // = { new std::string(); }

	if ((file = file::open_r(file_name)))
	{
		i = -1;
		while ((fread(&c, 1, 1, file)))
			if (c == '[')
			{
				++i;
				// param[i] = new std::string;
				while ((fread(&c, 1, 1, file)) && c != ']')
					param[i] += c;
				param[i] += '\0';
			}
		fclose(file);
		//init account settings
		// if (!param[0].empty())
		env->acc->holder = param[0];
		env->acc->number = param[1];
		env->acc->fiat_iso = param[2];
		env->acc->starting_eq = parser::str::to_double(param[3].c_str());
		//init tickers settings
		env->ticker->type = ((param[4] == "forex") ? 1
						  : (param[4] == "index") ? 2
						  : (param[4] == "commo") ? 3
						  : (param[4] == "stock") ? 4
						  : (param[4] == "btc") ? 5
						  : (param[4] == "eth") ? 6
						  : 0);
		env->ticker->iso_base = param[5];
		env->ticker->iso_quote = param[6];
		env->ticker->denom = (param[5] + param[6]); //PB???
		env->ticker->price_mode = ((param[7] == "tick") ? MODE_TICK
								: (param[7] == "ohlc") ? MODE_OHLC : 0);
		env->ticker->leverage = (uint16_t)parser::str::to_int(param[8].c_str());
		env->ticker->lot_size = parser::str::to_int(param[9].c_str());
		env->ticker->min_lot = parser::str::to_double(param[10].c_str());
		env->ticker->max_lot = parser::str::to_double(param[11].c_str());
		env->ticker->spread_mode = ((param[12] == "fixed_pts") ? 1
								 : (param[12] == "fixed_pct") ? 2
								 : (param[12] == "internal") ? 3
								 : (param[12] == "external") ? 4
								 : 0);
		env->spread_file_name = param[13];
		env->ticker->spread = (uint16_t)parser::str::to_int(param[14].c_str());
		//init tickers settings
		env->risk->max_long = (uint16_t)parser::str::to_int(param[15].c_str());
		env->risk->max_short = (uint16_t)parser::str::to_int(param[16].c_str());
		env->risk->max_trades = (uint16_t)parser::str::to_int(param[17].c_str());
		env->risk->max_dd_pct_acc = parser::str::to_double(param[18].c_str());
		env->risk->max_dd_pct_ticker = parser::str::to_double(param[19].c_str());
		env->risk->max_dd_acc = parser::str::to_double(param[20].c_str());
		env->risk->max_dd_ticker = parser::str::to_double(param[21].c_str());
		env->risk->max_runup_pct_acc = parser::str::to_double(param[22].c_str());
		env->risk->max_runup_pct_ticker = parser::str::to_double(param[23].c_str());
		env->risk->max_runup_acc = parser::str::to_double(param[24].c_str());
		env->risk->max_runup_ticker = parser::str::to_double(param[25].c_str());
		//path parameters
		env->parent_folder = param[26];
		//env->data_file_name = strdup(param[24]);
		//env->config_file_name = strdup(param[25]);
		prepare_bt(env);
		return (1);
	}
	return (0);
}

int		prepare_bt(t_env *env)
{
	env->pass = ((MAX_MA1 - MIN_MA1) > MAX_PASS ? MAX_PASS : (MAX_MA1 - MIN_MA1));
	env->pass = env->pass > 0 ? env->pass : 1;
	env->step[0] = (MAX_MA1 - MIN_MA1) / env->pass;
	for (int pass = 0; pass < env->pass; ++pass)
	{
		env->acc->floating_eq[pass] = env->acc->starting_eq;
		env->acc->balance[pass] = env->acc->starting_eq;
	}
	return (1);
}

/*
int		get_config(string file_name, t_env *env)
{
	FILE		*config_file;
	//std::string	bs;
	std::string	str;
	int			i = -1;
	config_file = fopen(file_name, "rb");
	while (++i < 10)
		fscanf(config_file, "%*[^\n]\n", NULL);
	while (++i < 15)
	{
		fscanf(config_file, "%*[%s]\n", &str);
		printf("%s\n", str);
	}
	return (0);
}
*/