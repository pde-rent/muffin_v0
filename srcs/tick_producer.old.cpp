#include "main.hpp"

namespace stream::tick::producer
{
	static	FILE		*histo;

	int		generate(t_env *env)
	{
		unsigned int	epoch;
		double			volume;
		double			price[2];
		char			tmp[128];
		char			buf[BUFF_SIZE + 1];
		unsigned int	i = 0;
		unsigned int	j = 0;
		unsigned int	nb_data = file::count_lines(env->data_file_path);
		//unsigned int			max_ticks = (TICK_BUFF_SIZE > nb_data ? nb_data : TICK_BUFF_SIZE);
		int		chunk = 3; // given timestamp / bid / volume
		int				l = -1;
		//fopen modes : r/rb/w/wb/a/ab/r+/w+/a+...
		if (((histo = file::open_r(env->data_file_path))))
		{
			env->ticker.size = nb_data;
			//if ((fread(buf, 1, line_ln, histo) == line_ln)) //jump first lines
			//	fseek(histo, 0, SEEK_SET); //skip headers (byte size different from data lines)
			while ((fread(buf, 1, BUFF_SIZE, histo))) //1 = sizeof(char)
			{
				buf[BUFF_SIZE] = '\0';
				i = 0;
				while (buf[i] && (j < nb_data)) // is ascii >> not null nor lost
				{
					if (chunk == 3)
					{
						while ((buf[i] >= '0' && buf[i] <= '9')) //&& (j + i) < len) // && ((unsigned long)((j * BUFF_SIZE) + i)  < len))
						{
							tmp[++l] = buf[i]; i++;
						}
						if (l >= 0 && buf[i] == ',')
						{
							tmp[++l] = '\0';
							epoch = (unsigned int)str_parser::to_int(&tmp[0]);
							l = -1;
							chunk = 1;
							i++;
						}
					}
					if (chunk == 1)
					{
						while ((buf[i] >= '0' && buf[i] <= '9') || buf[i] == '.') //&& (j + i) < len) // && ((unsigned long)((j * BUFF_SIZE) + i)  < len))
						{
							tmp[++l] = buf[i]; i++;
						}
						if (l >= 0 && buf[i] == ',')
						{
							tmp[++l] = '\0';
							double mid = str_parser::to_double(&tmp[0]);
							price[0] = mid * 0.9985; //bid price for BTC with 0.15% com / 1.0015?
							price[1] = mid * 1.0015; //ask price for BTC with 0.15% com
							l = -1;
							chunk = 2;
							i++;
						}
					}
					if (chunk == 2)
					{
						while ((buf[i] >= '0' && buf[i] <= '9') || buf[i] == '.') //&& (j + i) < len) // && ((unsigned long)((j * BUFF_SIZE) + i)  < len))
						{
							tmp[++l] = buf[i]; i++;
						}
						if (l >= 0 && buf[i] == '\n')
						{
							tmp[++l] = '\0';
							volume = str_parser::to_double(&tmp[0]);
							l = -1;
							chunk = 3;
							i++;
							j++; //+1 tick
							env->lock[2] = true; //data ready to be pushed
							while (1) //wait for strategy::run to ask for new tick
								if (!env->lock[2] || !env->lock[0]) //data has been pushed
								{
									env->data->epoch->push_front(epoch);
									env->data->bid->push_front(price[0]);
									env->data->ask->push_front(price[1]);
									env->data->volume->push_front(volume);
									tick::consumer::flush(env->data);
									env->lock[1] = true; //data pushed
									break;
								}
						}
					}
					if (!env->lock[0] && j >= TICK_BUFF_SIZE) //data set ready to be pushed
						env->lock[0] = true;
				}
			}
			env->lock[0] = false;
			fclose(histo);
			printf("Loaded %zu input rows from the data set\n", env->data->epoch->size());
			return (1);
		}
		printf("Failed to open the data file\n");
		return (0);
	}
}
