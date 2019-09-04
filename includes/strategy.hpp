#pragma once

#include "t_env.hpp"

namespace strategy
{
	// int split_to_thread_pool(t_env *env);
	int			run(t_env *env, int pass);
	inline int	split_to_thread_pool(t_env *env)
	{
		//get new tick >> from the lock lock[2]
		//optimisation loop
		// #pragma omp parallel //num_threads(5)
		// {
		// 	#pragma omp for
		for (int pass = 0; pass < env->nb_pass; ++pass) {
			env->thread_yields.emplace_back(
				env->thread_pool->enqueue(strategy::run, env, pass)
			);
		}
		return (1);
	}
	inline int wait_for_thread_pool(t_env *env)
	{
		int sum = 0;
		for (auto &&yield: env->thread_yields) { // not sure why we use &&std::future<int> forwarding reference
			/*printf("thread_pool task yielded: %d\n", */
			if (yield.valid()) {
				yield.get();
				sum++;
			}
		}
		return (sum);
	}
}
