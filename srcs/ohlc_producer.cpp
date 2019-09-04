#include "main.hpp"

// csv format expected:
// (whatever)timestamp | (double)open | (double)high | (double)low | (double)close | (double)volumes
// i.e.:     1566556741,      xxx.xxxx,      xxx.xxxx,     xxx.xxxx,       xxx.xxxx,        xxx.xxxx
// spread and volume breakdown are not yet implemented

namespace stream::ohlc::producer
{
	// array version:
	// void * memcpy(void *to, const void *from, size_t n_bytes);
	static inline void	push(t_env *env, unsigned int epoch, double ohlcv[5])
	{
		// offset all data stored to erase the last data point with the second last
		// insert the newest data point to [0] to be used by indicators
		// memmove makes sure memory does not get mixed up like memcpy()
		memmove((&env->ohlc->epoch[1]),
			(&env->ohlc->epoch[0]), sizeof(unsigned int) * (size_t)(OHLC_BUFF_SIZE - 1));
		memmove((&env->ohlc->open[1]),
			(&env->ohlc->open[0]), sizeof(double) * (size_t)(OHLC_BUFF_SIZE - 1));
		memmove((&env->ohlc->high[1]),
			(&env->ohlc->high[0]), sizeof(double) * (size_t)(OHLC_BUFF_SIZE - 1));
		memmove((&env->ohlc->low[1]),
			(&env->ohlc->low[0]), sizeof(double) * (size_t)(OHLC_BUFF_SIZE - 1));
		memmove((&env->ohlc->close[1]),
			(&env->ohlc->close[0]), sizeof(double) * (size_t)(OHLC_BUFF_SIZE - 1));
		memmove((&env->ohlc->volume[1]),
			(&env->ohlc->volume[0]), sizeof(double) * (size_t)(OHLC_BUFF_SIZE - 1));
		env->ohlc->epoch[0] = epoch;
		env->ohlc->open[0] = ohlcv[0];
		env->ohlc->high[0] = ohlcv[1];
		env->ohlc->low[0] = ohlcv[2];
		env->ohlc->close[0] = ohlcv[3];
		env->ohlc->volume[0] = ohlcv[4];
		printf("New ohlc: [%u,%f,%f,%f,%f,%f]\n",
		 	epoch,ohlcv[0],ohlcv[1],ohlcv[2],ohlcv[3],ohlcv[4]);
	}
	static inline char 	**csv_parse_row(char *row_start, char *row_end,
							char **split, int depth, int max_size, char sep)
	{
		// split is already alocated and has a fixed size, enough to store the csv data
		char *p = row_start;
		int i = 0;
		int j = -1;
		
		if (!split) {
			printf("csv_parse_row() error: missing **split\n");
			return NULL;
		}
		while (p < row_end && *p != '\n' && i < depth)
		{
			if (parser::format::is_numeric(*p) && (j < (max_size - 1))) {
				split[i][++j] = *p;
			} else if (*p == sep) {
				(j < (max_size - 1)) ? (split[i][++j] = '\0') : (split[i][j] = '\0');
				i++; j = -1;
			}
			++p;
		}
		// return null if row has different length than expected size >> error parsing
		// (i < (depth - 1)) ? printf("csv_parse_row() error: expected more data cells\n") : 0;
		return ((i < (depth - 1)) ? NULL : split);
	}
	static void terminate(t_env *env)
	{
		// tell consumers we're done with the csv file >> end signal
		// epoch 0 ohlcv {0,0,0,0,0}
		double end_ohlc[] = { 0.0, 0.0, 0.0, 0.0, 0.0 };
		// save data to the rolling ohlc / tick arrays or deque
		stream::ohlc::producer::push(env, 0, end_ohlc);
	}
	int		generate(t_env *env)
	{
		// unsigned int	epoch = 0;
		// double			ohlcv[5]; // { o, h, l, c, v }

		FILE			*f; // historical_data_file
		char			buf[BUFF_SIZE];
		unsigned int	neol = 0;
		int				handle;
		// char			split[6][128]; // epoch, o, h, l, c, v
		char			**split; // epoch, o, h, l, c, v
		char			**tmp; // epoch, o, h, l, c, v
		char 			sep = ',';
		// unsigned int	nb_data = file::count_lines(env->data_file_path);

		// unsigned long	len = 0;
		//fopen modes : r/rb/w/wb/a/ab/r+/w+/a+...
		printf("env->data_file_path: %s\n",env->data_file_path.c_str());
		if (!(f = file::open_r(env->data_file_path))) {
			return (put_error(ERR_NO_HISTO_FILE, 0));
		} else {
			// printf("Stream%s\n", file_name.c_str());
		}
		split = new char *[6];
		for (int i = 0; i < 6; i++) {
			split[i] = new char[128];
			memset(split[i], '\0', 128);
		}
		// fseek(f, 0, SEEK_END);	// move file pointer to EOF
		// len = (ftell(file));	// read EOF location
		// printf("Characters to parse: %lu\n", ftell(f));
		fseek(f, 0, SEEK_SET);	// move file pointer back to start
		// read data chunk from the historical data file
		while ((handle = fread(buf, 1, BUFF_SIZE, f)))
		{
			char *ptr = &buf[0];
			char *pptr = ptr;
			// while *ptr sets to the next end of line, keep on
			// parsing the rows one per one and make it available
			while ((ptr = (char *)memchr(ptr, '\n', (&buf[0] + handle) - ptr)))
			{
				++ptr; // jump after eol
				// deal with line
				// char **csv_parse_row(char *row_start, char *row_end,
				//						char **split, size_t size, char sep)
				if (!(tmp = csv_parse_row(pptr, (ptr-2), split, 6, 128, sep))) {
					pptr = ptr;
					// broken line >> incomplete
					continue;
				}
				++neol;
				double ohlcv[] = {
					parser::str::to_double(split[1]),			// open
					parser::str::to_double(split[2]),			// high
					parser::str::to_double(split[3]),			// low
					parser::str::to_double(split[4]),			// close
					parser::str::to_double(split[5])			// volume
				};
				// save data to the rolling ohlc / tick arrays or deque
				// printf("env->stream_mutex.lock()\n");
				while (env->consumer_busy == true); // wait for consumer to ask for new tick
				env->stream_mutex.lock();
				stream::ohlc::producer::push(
					env,
					(unsigned int)parser::str::to_long(split[0]),	// epoch
					ohlcv
				);
				// if we have enough available data points to start working, tell consumers
				if (neol >= OHLC_BUFF_SIZE) {
					// printf("neol: %d, buff_size: %d\n", neol, OHLC_BUFF_SIZE);
					env->new_data_available = true; // data ready to be consumed
					// while (env->consumer_busy == false); // wait for consumer to ask for new tick
				}
				env->stream_mutex.unlock();
				// printf("env->stream_mutex.unlock()\n");
				// update previous with current pointer
				pptr = ptr;
			}
		}
		printf("stream::ohlc::producer CLOSING HANDLE\n");
		fclose(f);
		// free char * buffer
		for (int i = 0; i < 6; i++) {
			delete [] split[i];
		}
		delete [] split;
		// send finishing signal to consumers
		printf("stream::ohlc::producer::terminate()\n");		
		stream::ohlc::producer::terminate(env);
		exit(1);
		// printf("Generated %u data points for %d parameter pass\n", neol, env->nb_pass);
		return (neol);
	}
}
