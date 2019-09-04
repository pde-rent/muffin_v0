#include "main.hpp"

#define PARAM_CNT			30

namespace config {
	static int	get_induced_parameters(t_env *env)
	{
		env->nb_pass = ((MAX_MA1 - MIN_MA1) > MAX_PASS ? MAX_PASS : (MAX_MA1 - MIN_MA1));
		env->nb_pass = env->nb_pass > 0 ? env->nb_pass : 1;
		env->step[0] = (MAX_MA1 - MIN_MA1) / env->nb_pass;
		for (int pass = 0; pass < env->nb_pass; ++pass)
		{
			env->acc.value.equity[pass] = env->acc.value.starting_equity;
			env->acc.value.balance[pass] = env->acc.value.starting_equity;
		}
		return (1);
	}
	static std::vector<std::string> read_config_brackets(std::string config_file_path/*, std::string *param*/)
	{
		FILE						*file;
		char						c;
		std::vector<std::string>	param;
		std::string 				tmp;

		if (!(file = file::open_r(config_file_path))) {
			put_error(ERR_NO_CONFIG_FILE, 0);
			return (std::vector<std::string>()); // empty list as vector initializer
		}
		while ((fread(&c, 1, 1, file))) {
			if (c == '[') {
				if (!tmp.empty()) {
					param.push_back(tmp);
					tmp = "";
				}
				while ((fread(&c, 1, 1, file)) && c != ']') {
					tmp += c;
				}
				// param[i] += '\0'; // only for C style string
			}		
		}
		fclose(file);
		return (param);
	}
	int		get_parameters(t_env *env, std::string config_file_path)
	{
		// [PARAM_CNT]
		std::vector<std::string> param = read_config_brackets(config_file_path);
		//init account settings
		// if (!param[0].empty())
		env->acc.holder = param[0];
		env->acc.number = param[1];
		env->acc.fiat_iso = param[2];
		env->acc.value.starting_equity = parser::str::to_double(param[3].c_str());
		//init tickers settings
		env->ticker.type = ((param[4] == "forex") ? 1
						  : (param[4] == "index") ? 2
						  : (param[4] == "commo") ? 3
						  : (param[4] == "stock") ? 4
						  : (param[4] == "btc") ? 5
						  : (param[4] == "eth") ? 6
						  : 0);
		env->ticker.iso_base = param[5];
		env->ticker.iso_quote = param[6];
		env->ticker.denom = (param[5] + param[6]); //PB???
		env->ticker.price_mode = ((param[7] == "tick") ? MODE_TICK
								: (param[7] == "ohlc") ? MODE_OHLC : 0);
		env->ticker.leverage = (int)parser::str::to_int(param[8].c_str());
		env->ticker.lot_size = parser::str::to_double(param[9].c_str());
		env->ticker.min_lot = parser::str::to_double(param[10].c_str());
		env->ticker.max_lot = parser::str::to_double(param[11].c_str());
		env->ticker.spread_mode = ((param[12] == "fixed_pts") ? 1
								 : (param[12] == "fixed_pct") ? 2
								 : (param[12] == "internal") ? 3
								 : (param[12] == "external") ? 4
								 : 0);
		env->spread_file_path = param[13];
		env->ticker.spread = (int)parser::str::to_int(param[14].c_str());
		//init tickers settings
		env->risk.max_long = (int)parser::str::to_int(param[15].c_str());
		env->risk.max_short = (int)parser::str::to_int(param[16].c_str());
		env->risk.max_trades = (int)parser::str::to_int(param[17].c_str());
		env->risk.max_dd_pct_acc = parser::str::to_double(param[18].c_str());
		env->risk.max_dd_pct_ticker = parser::str::to_double(param[19].c_str());
		env->risk.max_dd_acc = parser::str::to_double(param[20].c_str());
		env->risk.max_dd_ticker = parser::str::to_double(param[21].c_str());
		env->risk.max_runup_pct_acc = parser::str::to_double(param[22].c_str());
		env->risk.max_runup_pct_ticker = parser::str::to_double(param[23].c_str());
		env->risk.max_runup_acc = parser::str::to_double(param[24].c_str());
		env->risk.max_runup_ticker = parser::str::to_double(param[25].c_str());
		//path parameters
		env->data_folder = param[26];
		std::string data_file_name = param[27];
		env->data_file_path = param[26] + "/" + data_file_name;
		// env->config_file_path = param[28];
		get_induced_parameters(env);
		return (1);
	}
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