#include "main.hpp"

// csv format expected:
// (whatever)timestamp | (double)open | (double)high | (double)low | (double)close | (double)volumes
// i.e.:     1566556741,      xxx.xxxx,      xxx.xxxx,     xxx.xxxx,       xxx.xxxx,        xxx.xxxx
// spread and volume breakdown are not yet implemented

#define CHUNK_TIMESTAMP		1
#define CHUNK_OPEN			2
#define CHUNK_HIGH			3
#define CHUNK_LOW			4
#define CHUNK_CLOSE			5
#define CHUNK_VOLUME		1
// #define CHUNK_VOLUME_ASK 1
// #define CHUNK_VOLUME_BID 1
// #define CHUNK_AVG_SPREAD 1

namespace stream::ohlc::producer
{
	static inline void	push(t_env *env, time_t epoch, double ohlcv[5])
	{
		env->lock[2] = true; //data ready to be pushed
		while (1) //wait for strategy::run to ask for new tick
			if (!env->lock[2] || !env->lock[0]) //data has been pushed
			{
				env->ohlc->epoch->push_front(epoch);
				env->ohlc->open->push_front(ohlcv[0]);
				env->ohlc->high->push_front(ohlcv[1]);
				env->ohlc->low->push_front(ohlcv[2]);
				env->ohlc->close->push_front(ohlcv[3]);
				env->ohlc->volume->push_front(ohlcv[4]);
				stream::ohlc::consumer::flush(env);
				env->lock[1] = true; //data pushed
				break;
			}
	}
	int		generate(t_env *env)
	{
		FILE 			*historical_data_file;
		size_t			epoch;
		double			ohlcv[5]; // { o, h, l, c, v }
		char			tmp[128];
		char			buf[BUFF_SIZE + 1];
		size_t			i = 0;
		size_t			j = 0;
		size_t			nb_ticks = file::count_lines(env->ohlc_file_name);
		//size_t			max_ticks = (TICK_BUFF_SIZE > nb_ticks ? nb_ticks : TICK_BUFF_SIZE);
		uint16_t		last_chunk_parsed = CHUNK_VOLUME;
		int				l = -1;
		//fopen modes : r/rb/w/wb/a/ab/r+/w+/a+...
		if ((historical_data_file = file::open_r(env->ohlc_file_name)))
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
					if (last_chunk_parsed == CHUNK_VOLUME)
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
							last_chunk_parsed = 1;
							i++;
						}
					}
					if (last_chunk_parsed == CHUNK_TIMESTAMP)
					{
						while (parser::format::is_numeric(buf[i])) //&& (j + i) < len) // && ((unsigned long)((j * BUFF_SIZE) + i)  < len))
						{
							tmp[++l] = buf[i]; i++;
						}
						if (l >= 0 && buf[i] == ',')
						{
							tmp[++l] = '\0';
							ohlcv[0] = parser::str::to_double(&tmp[0]); //bid price
							l = -1;
							last_chunk_parsed = CHUNK_OPEN;
							i++;
						}
					}
					if (last_chunk_parsed == CHUNK_OPEN)
					{
						while (parser::format::is_numeric(buf[i])) //&& (j + i) < len) // && ((unsigned long)((j * BUFF_SIZE) + i)  < len))
						{
							tmp[++l] = buf[i]; i++;
						}
						if (l >= 0 && buf[i] == ',')
						{
							tmp[++l] = '\0';
							ohlcv[1] = parser::str::to_double(&tmp[0]); //ask price
							l = -1;
							last_chunk_parsed = CHUNK_HIGH;
							i++;
						}
					}
					if (last_chunk_parsed == CHUNK_HIGH)
					{
						while (parser::format::is_numeric(buf[i])) //&& (j + i) < len) // && ((unsigned long)((j * BUFF_SIZE) + i)  < len))
						{
							tmp[++l] = buf[i]; i++;
						}
						if (l >= 0 && buf[i] == ',')
						{
							tmp[++l] = '\0';
							ohlcv[2] = parser::str::to_double(&tmp[0]); //ask price
							l = -1;
							last_chunk_parsed = CHUNK_LOW;
							i++;
						}
					}
					if (last_chunk_parsed == CHUNK_LOW)
					{
						while (parser::format::is_numeric(buf[i])) //&& (j + i) < len) // && ((unsigned long)((j * BUFF_SIZE) + i)  < len))
						{
							tmp[++l] = buf[i]; i++;
						}
						if (l >= 0 && buf[i] == ',')
						{
							tmp[++l] = '\0';
							ohlcv[3] = parser::str::to_double(&tmp[0]); //ask price
							l = -1;
							last_chunk_parsed = CHUNK_CLOSE;
							i++;
						}
					}
					if (last_chunk_parsed == CHUNK_CLOSE)
					{
						while (parser::format::is_numeric(buf[i])) //&& (j + i) < len) // && ((unsigned long)((j * BUFF_SIZE) + i)  < len))
						{
							tmp[++l] = buf[i]; i++;
						}
						if (buf[i] == '\r') i++; // to jump microsoft \r\n eol
						if (l >= 0 && buf[i] == '\n')
						{
							tmp[++l] = '\0';
							ohlcv[4] = parser::str::to_double(&tmp[0]);
							l = -1;
							last_chunk_parsed = CHUNK_VOLUME;
							i++;
							j++; //+1 tick
							//printf("%d\n", j);
							//if (j == 6308)
							//	j = 6308;
							stream::ohlc::producer::push(env, epoch, ohlcv);
						}
					}
					if (!env->lock[0] && j >= TICK_BUFF_SIZE) //data set ready to be pushed
						env->lock[0] = true;
				}
			}
			env->lock[0] = false;
			fclose(historical_data_file);
			printf("Loaded %zu input rows from the data set\n", env->ohlc->epoch->size());
			return (1);
		}
		printf("Failed to open the data file\n");
		return (0);
	}
}
