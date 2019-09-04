#include "main.hpp"

// csv format expected:
// (whatever)timestamp | (double)open | (double)high | (double)low | (double)close | (double)volumes
// i.e.:     1566556741,      xxx.xxxx,      xxx.xxxx,     xxx.xxxx,       xxx.xxxx,        xxx.xxxx
// spread and volume breakdown are not yet implemented

#define CHUNK_TIMESTAMP		1
#define CHUNK_BID			2
#define CHUNK_ASK			3
#define CHUNK_VOLUME_BID	4
#define CHUNK_VOLUME_ASK	5

namespace stream::tick::producer
{
	// array version:
	// void * memcpy(void *to, const void *from, size_t n_bytes);
	static inline void	push(t_env *env, unsigned int epoch, double bavv[4])
	{
		memcpy((&env->ticks->epoch[1]),
			(&env->ticks->epoch[0]), sizeof(unsigned int) * (size_t)(TICK_BUFF_SIZE - 1));
		memcpy((&env->ticks->bid[1]),
			(&env->ticks->bid[0]), sizeof(double) * (size_t)(TICK_BUFF_SIZE - 1));
		memcpy((&env->ticks->ask[1]),
			(&env->ticks->ask[0]), sizeof(double) * (size_t)(TICK_BUFF_SIZE - 1));
		memcpy((&env->ticks->volume[1]),
			(&env->ticks->volume[0]), sizeof(double) * (size_t)(TICK_BUFF_SIZE - 1));
		env->ticks->epoch[0] = epoch;
		env->ticks->bid[0] = bavv[0];
		env->ticks->ask[0] = bavv[1];
		env->ticks->volume[0] = bavv[2] + bavv[3];
		// printf("New ohlc: [%u,%f,%f,%f,%f,%f]\n",
		// 	epoch,ohlcv[0],ohlcv[1],ohlcv[2],ohlcv[3],ohlcv[4]);
	}
	// static inline void 	flush(t_env *env)
	// {
	// 	// in array mode, the flush is push() memcpy
	// }

	// deque version:
	// static inline void push(t_env *env, unsigned int epoch, double bavv[4])
	// {
	// 	while (env->consumer_busy); // wait for consumer to ask for new tick
	// 	env->ticks->epoch->push_front(epoch);
	// 	env->ticks->bid->push_front(bavv[0]);
	// 	env->ticks->ask->push_front(bavv[1]);
	// 	env->ticks->volume->push_front(bavv[2] + bavv[3]);
	// 	// printf("New tick: [%u,%f,%f,%f,%f]\n",
	// 	// 	epoch,bavv[0],bavv[1],bavv[2],bavv[3]);
	// }
	// static inline void flush(t_env *env)
	// {
	// 	env->ticks->epoch->resize(TICK_BUFF_SIZE);	//data->epoch->erase(TICK_BUFF_SIZE);
	// 	env->ticks->bid->resize(TICK_BUFF_SIZE);		//data->px->erase(TICK_BUFF_SIZE);
	// 	env->ticks->ask->resize(TICK_BUFF_SIZE);		//data->px->erase(TICK_BUFF_SIZE);
	// 	env->ticks->volume->resize(TICK_BUFF_SIZE);;		//data->vol->erase(TICK_BUFF_SIZE);
	// }
	int		generate(t_env *env)
	{
		FILE 			*historical_data_file;
		unsigned int	epoch;
		double			bavv[4]; // bid / ask / vol bid / vol ask
		char			tmp[128];
		char			buf[BUFF_SIZE + 1];
		unsigned int	i = 0;
		unsigned int	j = 0;
		unsigned int	nb_data = file::count_lines(env->data_file_path);
		//unsigned int	max_ticks = (TICK_BUFF_SIZE > nb_data ? nb_data : TICK_BUFF_SIZE);
		int		chunk = CHUNK_VOLUME_ASK;
		int				l = -1;
		//fopen modes : r/rb/w/wb/a/ab/r+/w+/a+...
		if (!(historical_data_file = file::open_r(env->data_file_path)))
			return (put_error(ERR_NO_HISTO_FILE,0));
		env->ticker.size = nb_data;
		//we should find a way to skip the header
		while ((fread(buf, 1, BUFF_SIZE, historical_data_file))) //1 = sizeof(char)
		{
			buf[BUFF_SIZE] = '\0';
			i = 0;
			while (buf[i] && (j < nb_data)) // is ascii >> not null nor lost
			{
				//if (buf[i] == '\r') i+= 2;
				//else if (buf[i] == '\n') i++;
				if (chunk == CHUNK_VOLUME_ASK)
				{
					while (parser::format::is_timestamp(buf[i])) //&& (j + i) < len) // && ((unsigned long)((j * BUFF_SIZE) + i)  < len))
					{
						tmp[++l] = buf[i]; i++;
					}
					if (l >= 0 && buf[i] == ',')
					{
						tmp[++l] = '\0';
						// epoch = parser::str::to_time(&tmp[0]);
						epoch = (unsigned int)parser::str::to_long(&tmp[0]);
						l = -1;
						chunk = CHUNK_TIMESTAMP;
						i++;
					}
				}
				if (chunk == CHUNK_TIMESTAMP)
				{
					while (parser::format::is_numeric(buf[i])) //&& (j + i) < len) // && ((unsigned long)((j * BUFF_SIZE) + i)  < len))
					{
						tmp[++l] = buf[i]; i++;
					}
					if (l >= 0 && buf[i] == ',')
					{
						tmp[++l] = '\0';
						bavv[0] = parser::str::to_double(&tmp[0]); // bid price
						l = -1;
						chunk = CHUNK_ASK;
						i++;
					}
				}
				if (chunk == CHUNK_BID)
				{
					while (parser::format::is_numeric(buf[i])) //&& (j + i) < len) // && ((unsigned long)((j * BUFF_SIZE) + i)  < len))
					{
						tmp[++l] = buf[i]; i++;
					}
					if (l >= 0 && buf[i] == ',')
					{
						tmp[++l] = '\0';
						bavv[1] = parser::str::to_double(&tmp[0]); // ask price
						l = -1;
						chunk = CHUNK_ASK;
						i++;
					}
				}
				if (chunk == CHUNK_ASK)
				{
					while (parser::format::is_numeric(buf[i])) //&& (j + i) < len) // && ((unsigned long)((j * BUFF_SIZE) + i)  < len))
					{
						tmp[++l] = buf[i]; i++;
					}
					if (l >= 0 && buf[i] == ',')
					{
						tmp[++l] = '\0';
						bavv[2] = parser::str::to_double(&tmp[0]); // vol bid price
						l = -1;
						chunk = CHUNK_VOLUME_BID;
						i++;
					}
				}
				if (chunk == CHUNK_VOLUME_BID)
				{
					while (parser::format::is_numeric(buf[i])) //&& (j + i) < len) // && ((unsigned long)((j * BUFF_SIZE) + i)  < len))
					{
						tmp[++l] = buf[i]; i++;
					}
					if (l >= 0 && buf[i] == ',')
					{
						tmp[++l] = '\0';
						bavv[3] = parser::str::to_double(&tmp[0]); // vol ask
						l = -1;
						chunk = CHUNK_VOLUME_ASK;
						i++;
						j++; //+1 tick
						//printf("%d\n", j);
						//if (j == 6308)
						//	j = 6308;
						stream::tick::producer::push(env, epoch, bavv);
						if (j >= TICK_BUFF_SIZE) {
							env->new_data_available = true; // data ready to be consumed
						}
						// deque version:
						// stream::tick::producer::flush(env);
					}
				}
			}
		}
		fclose(historical_data_file);
		// printf("Loaded %zu input rows from the data set\n", env->ticks->epoch->size());
		return (1);
	}
}
