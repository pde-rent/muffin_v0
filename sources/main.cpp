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
	string data_file = av[1];
	string config_file = av[2];
	//start clockwatch
	auto t1 = std::chrono::high_resolution_clock::now();
	printf("Ticker initialized\n");
	tick::load_all(data_file, env);
	//tick::set(file_name, data);
	//while (tick::next(data));
	//stop clockwatch
	auto t2 = std::chrono::high_resolution_clock::now();
	auto int_ms = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);

	printf("Parsing took %d ms\n", (int)int_ms.count());
	//long a = data->epoch->size();
	//long b = data->px->size();
	//long c = data->vol->size();
	//for (int x = data->epoch->size() - 1; x>=0; x--)
	//	printf("%zu,%f,%f\n", data->epoch->at(x), data->px->at(x), data->vol->at(x));
	
	deinit::env(env);
	return (0);
}
