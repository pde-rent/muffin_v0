#include "main.hpp"

namespace print::result
{
	void ending_equity(t_env *env) {
		for (int i = 0; i < env->nb_pass; ++i) {
			printf("Pass %d Balance %f\n", i, env->acc.value.balance[i]);
		}
	}
}
