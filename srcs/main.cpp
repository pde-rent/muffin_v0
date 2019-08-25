#include "main.hpp"

int main(int ac, char **av)
{
	if (((!compliance::argv(ac, av))))
		return (0);
	t_env			env;
	t_account		acc;
	t_ticker		ticker;
	t_risk			risk;

	env.lock[0] = env.lock[1] = env.lock[2] = false;
	env.risk = &risk;
	env.acc = &acc;
	env.ticker = &ticker;
	env.ohlc_file_name = av[1];
	env.ticks_file_name = av[1];
	env.config_file_name = (av[2] ? av[2] : "./assets/config.yml");
	//init environment & load ticker
	init::env(&env);
	if (!get_config(&env, env.config_file_name)) return (0);
	// start loading data
	init::data(&env);
	printf("Ticker initialized\n");
	//start clockwatch
	auto t1 = std::chrono::high_resolution_clock::now();
	init::split_process(&env);
	//stop clockwatch
	auto t2 = std::chrono::high_resolution_clock::now();
	auto elapsed_ms = /*std::chrono::duration_cast<std::chrono::milliseconds>*/(t2 - t1);

	for (int i = 0; i < env.pass; ++i)
		printf("Pass %d Balance %f\n", i, env.acc->balance[i]);
	printf("Backtest took %d ms\n", (int)elapsed_ms.count());
	
	deinit::env(&env);
	return (0);
}
