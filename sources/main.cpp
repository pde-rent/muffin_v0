#pragma once

#ifdef _WIN32
# include "stdafx.h"
#elif __APPLE__
# include "../includes/stdafx.h"
#endif

int main(int ac, char **av)
{
	if (((!compliance::argv(ac, av))))
		return (0);
	t_env	*env;
	env = init::env();
	//same here
	env->data_file_name = (av[1]);
	env->config_file_name = (av[2]);
	//start clockwatch
	auto t1 = std::chrono::high_resolution_clock::now();
	if (!get_config(env, env->config_file_name)) return (0);
	printf("Ticker initialized\n");
	init::split_process(env);
	//stop clockwatch
	auto t2 = std::chrono::high_resolution_clock::now();
	auto int_ms = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);

	for (int i = 0; i < env->pass; ++i)
		printf("Pass %d Balance %f\n", i, env->acc->balance[i]);
	printf("Backtest took %d ms\n", (int)int_ms.count());
	
	deinit::env(env);
	return (0);
}
