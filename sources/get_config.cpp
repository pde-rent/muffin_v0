#pragma once

#ifdef _WIN32
# include "stdafx.h"
#elif __APPLE__
# include "../includes/stdafx.h"
#endif

# define PARAM_CNT			30

int		get_config(t_env *env, string file_name)
{
	FILE		*file;
	string		*param;
	char		c;
	int			i = -1;
	int			j = -1;

	if (((file = fopen(file_name, "rb")) != NULL))
	{
		param = (string *)malloc(sizeof(string) * PARAM_CNT);
		while (++i < PARAM_CNT)
			param[i] = (string)malloc(sizeof(char) * 100);
		i = -1;
		while ((fread(&c, 1, 1, file)))
			if (c == '[')
			{
				++i; j = -1;
				while ((fread(&c, 1, 1, file)) && c != ']')
					param[i][++j] = c;
				param[i][++j] = '\0';
			}
		fclose(file);
		//init account settings
		env->acc->holder = _strdup(param[0]);
		env->acc->number = _strdup(param[1]);
		env->acc->fiat_iso = _strdup(param[2]);
		env->acc->starting_eq = fast::parse_f(param[3]);
		//init tickers settings
		env->ticker->type = (!strcmp(param[4], "forex") ? 1
						  : (!strcmp(param[4], "index") ? 2
						  : (!strcmp(param[4], "commo") ? 3
						  : (!strcmp(param[4], "stock") ? 4
						  : (!strcmp(param[4], "btc") ? 5
						  : (!strcmp(param[4], "eth") ? 6
						  : 0))))));
		env->ticker->iso_base = _strdup(param[5]);
		env->ticker->iso_base = _strdup(param[6]);
		env->ticker->denom = _strdup(strcat(param[5], param[6])); //PB???
		env->ticker->price_mode = (!strcmp(param[7], "tick") ? 1
								: (!strcmp(param[7], "ohlc") ? 2 : 0));
		env->ticker->leverage = (uint16_t)fast::parse_zu(param[8]);
		env->ticker->lot_size = fast::parse_zu(param[9]);
		env->ticker->min_lot = fast::parse_f(param[10]);
		env->ticker->max_lot = fast::parse_f(param[11]);
		env->ticker->spread_mode = (!strcmp(param[12], "fixed_pts") ? 1
								 : (!strcmp(param[12], "fixed_pct") ? 2
								 : (!strcmp(param[12], "internal") ? 3
								 : (!strcmp(param[12], "external") ? 4
								 : 0))));
		env->spread_file_name = _strdup(param[13]);
		env->ticker->spread = (uint16_t)fast::parse_zu(param[14]);
		//init tickers settings
		env->rsk->max_long = (uint16_t)fast::parse_zu(param[15]);
		env->rsk->max_short = (uint16_t)fast::parse_zu(param[16]);
		env->rsk->max_trades = (uint16_t)fast::parse_zu(param[17]);
		env->rsk->max_dd_pct_acc = (uint16_t)fast::parse_zu(param[18]);
		env->rsk->max_dd_pct_ticker = (uint16_t)fast::parse_zu(param[19]);
		env->rsk->max_dd_fiat = fast::parse_f(param[20]);
		env->rsk->max_dd_ticker = fast::parse_f(param[21]);
		//... add target parameters
		//path parameters
		env->parent_folder = _strdup(param[25]);
		//env->data_file_name = _strdup(param[24]);
		//env->config_file_name = _strdup(param[25]);
		for (i = 0; i < PARAM_CNT; i++)
			free(param[i]);
		free(param);
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