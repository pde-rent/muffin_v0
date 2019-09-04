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
	// array version:
	// void * memcpy(void *to, const void *from, size_t n_bytes);
	static inline void	push(t_env *env, unsigned int epoch, double ohlcv[5])
	{
		memcpy((&env->ohlc->epoch[1]),
			(&env->ohlc->epoch[0]), sizeof(unsigned int) * (size_t)(OHLC_BUFF_SIZE - 1));
		memcpy((&env->ohlc->open[1]),
			(&env->ohlc->open[0]), sizeof(double) * (size_t)(OHLC_BUFF_SIZE - 1));
		memcpy((&env->ohlc->high[1]),
			(&env->ohlc->high[0]), sizeof(double) * (size_t)(OHLC_BUFF_SIZE - 1));
		memcpy((&env->ohlc->low[1]),
			(&env->ohlc->low[0]), sizeof(double) * (size_t)(OHLC_BUFF_SIZE - 1));
		memcpy((&env->ohlc->close[1]),
			(&env->ohlc->close[0]), sizeof(double) * (size_t)(OHLC_BUFF_SIZE - 1));
		memcpy((&env->ohlc->volume[1]),
			(&env->ohlc->volume[0]), sizeof(double) * (size_t)(OHLC_BUFF_SIZE - 1));
		env->ohlc->epoch[0] = epoch;
		env->ohlc->open[0] = ohlcv[0];
		env->ohlc->high[0] = ohlcv[1];
		env->ohlc->low[0] = ohlcv[2];
		env->ohlc->close[0] = ohlcv[3];
		env->ohlc->volume[0] = ohlcv[4];
		// printf("New ohlc: [%u,%f,%f,%f,%f,%f]\n",
		// 	epoch,ohlcv[0],ohlcv[1],ohlcv[2],ohlcv[3],ohlcv[4]);
	}
	// static inline void 	flush(t_env *env)
	// {
	// 	// in array mode, the flush is push() memcpy
	// }

	// deque version:
	// static inline void	push(t_env *env, unsigned int epoch, double ohlcv[5])
	// {
	// 	env->ohlc->epoch->push_front(epoch);
	// 	env->ohlc->open->push_front(ohlcv[0]);
	// 	env->ohlc->high->push_front(ohlcv[1]);
	// 	env->ohlc->low->push_front(ohlcv[2]);
	// 	env->ohlc->close->push_front(ohlcv[3]);
	// 	env->ohlc->volume->push_front(ohlcv[4]);
	// 	// printf("New ohlc: [%u,%f,%f,%f,%f,%f]\n",
	// 	// 	epoch,ohlcv[0],ohlcv[1],ohlcv[2],ohlcv[3],ohlcv[4]);
	// }
	// static inline void 	flush(t_env *env)
	// {
	// 	env->ohlc->epoch->resize(OHLC_BUFF_SIZE);	//ohlc->epoch->erase(OHLC_BUFF_SIZE);
	// 	env->ohlc->open->resize(OHLC_BUFF_SIZE);		//ohlc->px->erase(OHLC_BUFF_SIZE);
	// 	env->ohlc->high->resize(OHLC_BUFF_SIZE);		//ohlc->px->erase(OHLC_BUFF_SIZE);
	// 	env->ohlc->low->resize(OHLC_BUFF_SIZE);;		//ohlc->vol->erase(OHLC_BUFF_SIZE);
	// 	env->ohlc->close->resize(OHLC_BUFF_SIZE);;		//ohlc->vol->erase(OHLC_BUFF_SIZE);
	// 	env->ohlc->volume->resize(OHLC_BUFF_SIZE);;		//ohlc->vol->erase(OHLC_BUFF_SIZE);
	// }
	int		generate(t_env *env)
	{
		FILE 			*historical_data_file;
		unsigned int	epoch = 0;
		double			ohlcv[5]; // { o, h, l, c, v }
		char			tmp[128];
		char			buf[BUFF_SIZE + 1];
		char 			sep = ',';
		unsigned int	i = 0;
		unsigned int	j = 0;
		unsigned int	nb_data = file::count_lines(env->data_file_path);

		//unsigned int	max_ticks = (OHLC_BUFF_SIZE > nb_data ? nb_data : OHLC_BUFF_SIZE);
		int		last_chunk_parsed = CHUNK_VOLUME;
		int				l = -1;
		//fopen modes : r/rb/w/wb/a/ab/r+/w+/a+...
		if (!(historical_data_file = file::open_r(env->data_file_path)))
			return (put_error(ERR_NO_HISTO_FILE,0));
		printf("Streaming %u ohlcv data points from:\n%s\n",
			nb_data, env->data_file_path.c_str());
		env->ticker.size = nb_data;
		//we should find a way to skip the header
		while ((fread(buf, 1, BUFF_SIZE, historical_data_file))) //1 = sizeof(char)
		{
			printf("read %d bytes\n", BUFF_SIZE);
			buf[BUFF_SIZE] = '\0';
			i = 0;
			while (buf[i] && (j < nb_data)) // is ascii >> not null nor lost
			{
				//if (buf[i] == '\r') i+= 2;
				//else if (buf[i] == '\n') i++;
				if (last_chunk_parsed == CHUNK_VOLUME)
				{
					printf("CHUNK_TIMESTAMP\n");
					while (parser::format::is_timestamp(buf[i])) //&& (j + i) < len) // && ((unsigned long)((j * BUFF_SIZE) + i)  < len))
					{
						tmp[++l] = buf[i]; i++;
					}
					if (l >= 0 && buf[i] == sep)
					{
						tmp[++l] = '\0';
						// epoch = parser::str::to_time(&tmp[0]);
						epoch = (unsigned int)parser::str::to_long(&tmp[0]);
						l = -1;
						last_chunk_parsed = CHUNK_TIMESTAMP;
						i++;
						// printf("epoch: %zu | ", epoch);
					}
				}
				if (last_chunk_parsed == CHUNK_TIMESTAMP)
				{
					printf("CHUNK_OPEN\n");
					while (parser::format::is_numeric(buf[i])) //&& (j + i) < len) // && ((unsigned long)((j * BUFF_SIZE) + i)  < len))
					{
						tmp[++l] = buf[i]; i++;
					}
					if (l >= 0 && buf[i] == sep)
					{
						tmp[++l] = '\0';
						ohlcv[0] = parser::str::to_double(&tmp[0]); //bid price
						l = -1;
						last_chunk_parsed = CHUNK_OPEN;
						i++;
						// printf("open: %f | ", ohlcv[0]);
					}
				}
				if (last_chunk_parsed == CHUNK_OPEN)
				{
					printf("CHUNK_HIGH\n");
					while (parser::format::is_numeric(buf[i])) //&& (j + i) < len) // && ((unsigned long)((j * BUFF_SIZE) + i)  < len))
					{
						tmp[++l] = buf[i]; i++;
					}
					if (l >= 0 && buf[i] == sep)
					{
						tmp[++l] = '\0';
						ohlcv[1] = parser::str::to_double(&tmp[0]); //ask price
						l = -1;
						last_chunk_parsed = CHUNK_HIGH;
						i++;
						// printf("high: %f | ", ohlcv[1]);
					}
				}
				if (last_chunk_parsed == CHUNK_HIGH)
				{
					printf("CHUNK_LOW\n");
					while (parser::format::is_numeric(buf[i])) //&& (j + i) < len) // && ((unsigned long)((j * BUFF_SIZE) + i)  < len))
					{
						tmp[++l] = buf[i]; i++;
					}
					if (l >= 0 && buf[i] == sep)
					{
						tmp[++l] = '\0';
						ohlcv[2] = parser::str::to_double(&tmp[0]); //ask price
						l = -1;
						last_chunk_parsed = CHUNK_LOW;
						i++;
						// printf("low: %f | ", ohlcv[2]);
					}
				}
				if (last_chunk_parsed == CHUNK_LOW)
				{
					while (parser::format::is_numeric(buf[i])) //&& (j + i) < len) // && ((unsigned long)((j * BUFF_SIZE) + i)  < len))
					{
						tmp[++l] = buf[i]; i++;
						printf("CHUNK_CLOSE is_numeric: %c >> %s\n", tmp[l], tmp);
					}
					printf("CHUNK_CLOSE next is: %c\n", buf[i]);
					if (l >= 0 && buf[i] == sep)
					{
						printf("CHUNK_CLOSE >> sep\n");
						tmp[++l] = '\0';
						ohlcv[3] = parser::str::to_double(&tmp[0]); //ask price
						l = -1;
						last_chunk_parsed = CHUNK_CLOSE;
						i++;
						// printf("close: %f | ", ohlcv[3]);
					}
				}
				if (last_chunk_parsed == CHUNK_CLOSE)
				{
					printf("CHUNK_VOLUME\n");
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
						// printf("volume: %f\n", ohlcv[4]);
						printf("producer: parsed: %u buffsize: %ld data points\n",j,(long)OHLC_BUFF_SIZE);
						stream::ohlc::producer::push(env, epoch, ohlcv);
						if (j >= OHLC_BUFF_SIZE) {
							printf(env->consumer_busy
								? "producer: I'll wait for you to be free\n"
								: "producer: You're free, good\n");
							while (env->consumer_busy == true); // wait for consumer to ask for new tick
							env->stream_mutex.lock();
							env->new_data_available = true; // data ready to be consumed
							env->stream_mutex.unlock();
							printf("producer: Data available, you better hurry\n");
							// printf(env->consumer_busy
							// 	? "producer: You're at work, good\n"
							// 	: "producer: I'll wait for you to start working\n");
							// while (env->consumer_busy == false); // wait for consumer to ask for new tick
							// printf("producer: I'm updating the data\n");
							// stream::ohlc::producer::flush(env);
						}
					}
				}
				// if (j >= OHLC_BUFF_SIZE) //data set ready to be pushed
				// {
				// 	printf("producer: unlocking env->lock[0] >> data ready for pull\n");
				// 	env->lock[0] = true;
				// }
			}
		}
		fclose(historical_data_file);
		// printf("Loaded %zu input rows from the data set\n", env->ohlc->epoch->size());
		return (1);
	}
}
