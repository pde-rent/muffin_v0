#include <thread>

typedef struct s_worker
{
	int 		id; // 0 to env->nb_cores
	std::thread	thread;
}				t_worker;

typedef struct s_backtest_worker
{
	int 		id; // 0 to env->nb_cores
	std::thread thread;
	int			pass_start;
	int			pass_end;
}				t_backtest_worker
