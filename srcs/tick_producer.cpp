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
	static inline void push(t_env *env, time_t epoch, double bavv[4])
	{
		(env->lock[2]) = true; //data ready to be pushed
		while (1) //wait for strategy::run to ask for new tick
			if (!(env->lock[2]) || !(env->lock[0])) //data has been pushed
			{
				env->ticks->epoch->push_front(epoch);
				env->ticks->bid->push_front(bavv[0]);
				env->ticks->ask->push_front(bavv[1]);
				env->ticks->volume->push_front(bavv[2] + bavv[3]);
				tick::consumer::flush(env);
				(env->lock[1]) = true; //data pushed
				break;
			}
	}

	int		generate(t_env *env)
	{
		FILE 			*historical_data_file;
		size_t			epoch;
		double			bavv[4]; // bid / ask / vol bid / vol ask
		char			tmp[128];
		char			buf[BUFF_SIZE + 1];
		size_t			i = 0;
		size_t			j = 0;
		size_t			nb_ticks = file::count_lines(env->ticks_file_name);
		//size_t			max_ticks = (TICK_BUFF_SIZE > nb_ticks ? nb_ticks : TICK_BUFF_SIZE);
		uint16_t		chunk = CHUNK_VOLUME_ASK;
		int				l = -1;
		//fopen modes : r/rb/w/wb/a/ab/r+/w+/a+...
		if ((historical_data_file = file::open_r(env->ticks_file_name)))
		{
			env->ticker->size = nb_ticks;
			//we should find a way to skip the header
			while ((fread(buf, 1, BUFF_SIZE, historical_data_file))) //1 = sizeof(char)
			{
				buf[BUFF_SIZE] = '\0';
				i = 0;
				while (buf[i] && (j < nb_ticks)) // is ascii >> not null nor lost
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
							epoch = parser::str::to_time(&tmp[0]);
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
						}
					}
					if (!(env->lock[0]) && j >= TICK_BUFF_SIZE) //data set ready to be pushed
						(env->lock[0]) = true;
				}
			}
			(env->lock[0]) = false;
			fclose(historical_data_file);
			printf("Loaded %zu input rows from the data set\n", env->ticks->epoch->size());
			return (1);
		}
		printf("Failed to open the data file\n");
		return (0);
	}
}
