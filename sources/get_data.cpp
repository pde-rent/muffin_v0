#pragma once

#ifdef _WIN32
# include "stdafx.h"
#elif __APPLE__
# include "../includes/stdafx.h"
#endif
	
namespace tick
{
	static	FILE		*histo;

	int		load_BTC(t_env *env, bool *addr[3])
	{
		size_t			epoch;
		float			volume;
		float			price[3];
		string			tmp;
		char			buf[BUFF_SIZE + 1];
		size_t			i = 0;
		size_t			j = 0;
		size_t			k = 0;
		size_t			nb_ticks = file::count_lines(env->data_file_name);
		//size_t			max_ticks = (TICK_BUFF_SIZE > nb_ticks ? nb_ticks : TICK_BUFF_SIZE);
		uint16_t		chunk = 3;
		int				l = -1;
		//fopen modes : r/rb/w/wb/a/ab/r+/w+/a+...
		if (((histo = fopen(env->data_file_name, "rb")) != NULL))
		{
			env->ticker->size = nb_ticks;
			tmp = (string)malloc(sizeof(char) * 100);
			//if ((fread(buf, 1, line_ln, histo) == line_ln)) //jump first lines
			//	fseek(histo, 0, SEEK_SET); //skip headers (byte size different from data lines)
			while ((fread(buf, 1, BUFF_SIZE, histo))) //1 = sizeof(char)
			{
				buf[BUFF_SIZE] = '\0';
				i = 0;
				while (buf[i] && (j < nb_ticks)) // is ascii >> not null nor lost
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
							epoch = (size_t)fast::parse_zu(tmp);
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
							price[0] = (float)fast::parse_f(tmp);
							price[1] = price[0] * 1.0015; //ask price for BTC with 0.15% com
							price[2] = price[0] * 0.9985; //ask price for BTC with 0.15% com / 1.0015?
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
							volume = (float)fast::parse_f(tmp);
							l = -1;
							chunk = 3;
							i++;
							j++; //+1 tick
							*addr[2] = true; //data ready to be pushed
							while (1) //wait for on_tick to ask for new tick
								if (!*addr[2] || !*addr[0]) //data has been pushed
								{
									env->data->epoch->push_front(epoch);
									env->data->ask->push_front(price[1]);
									env->data->bid->push_front(price[2]);
									env->data->vol->push_front(volume);
									trigger::tick_event::circulate(env->data);
									*addr[1] = true; //data pushed
									break;
								}
						}
					}
					if (!*addr[0] && j >= TICK_BUFF_SIZE) //data set ready to be pushed
						*addr[0] = true;
				}
			}
			*addr[0] = false;
			fclose(histo);
			free(tmp);
			printf("Loaded %zu input rows from the data set\n", env->data->epoch->size());
			return (1);
		}
		printf("Failed to open the data file\n");
		return (0);
	}
	int		load_FX(t_env *env, bool *addr[3])
	{
		size_t			epoch;
		float			volume[2];
		float			price[2];
		string			tmp;
		//std::string		tmp2;
		char			buf[BUFF_SIZE + 1];
		size_t			i = 0;
		size_t			j = 0;
		size_t			k = 0;
		size_t			nb_ticks = file::count_lines(env->data_file_name);
		//size_t			max_ticks = (TICK_BUFF_SIZE > nb_ticks ? nb_ticks : TICK_BUFF_SIZE);
		uint16_t		chunk = 5;
		int				l = -1;
		//fopen modes : r/rb/w/wb/a/ab/r+/w+/a+...
		if (((histo = fopen(env->data_file_name, "rb")) != NULL))
		{
			env->ticker->size = nb_ticks;
			tmp = (string)malloc(sizeof(char) * 100);
			//we should find a way to skip the header
			while ((fread(buf, 1, BUFF_SIZE, histo))) //1 = sizeof(char)
			{
				buf[BUFF_SIZE] = '\0';
				i = 0;
				while (buf[i] && (j < nb_ticks)) // is ascii >> not null nor lost
				{
					//if (buf[i] == '\r') i+= 2;
					//else if (buf[i] == '\n') i++;
					if (chunk == 5)
					{
						while ((buf[i] >= '0' && buf[i] <= '9') || buf[i] == 32 || buf[i] == ':') //&& (j + i) < len) // && ((unsigned long)((j * BUFF_SIZE) + i)  < len))
						{
							tmp[++l] = buf[i]; i++;
						}
						if (l >= 0 && buf[i] == ',')
						{
							tmp[++l] = '\0';
							epoch = transform::time::get_epoch(tmp);
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
							price[1] = (float)fast::parse_f(tmp); //bid price
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
						if (l >= 0 && buf[i] == ',')
						{
							tmp[++l] = '\0';
							price[0] = (float)fast::parse_f(tmp); //ask price
							l = -1;
							chunk = 3;
							i++;
						}
					}
					if (chunk == 3)
					{
						while ((buf[i] >= '0' && buf[i] <= '9') || buf[i] == '.') //&& (j + i) < len) // && ((unsigned long)((j * BUFF_SIZE) + i)  < len))
						{
							tmp[++l] = buf[i]; i++;
						}
						if (l >= 0 && buf[i] == ',')
						{
							tmp[++l] = '\0';
							volume[0] = (float)fast::parse_f(tmp); //ask price
							l = -1;
							chunk = 4;
							i++;
						}
					}
					if (chunk == 4)
					{
						while ((buf[i] >= '0' && buf[i] <= '9') || buf[i] == '.') //&& (j + i) < len) // && ((unsigned long)((j * BUFF_SIZE) + i)  < len))
						{
							tmp[++l] = buf[i]; i++;
						}
						if (buf[i] == '\r') i++;
						if (l >= 0 && buf[i] == '\n')
						{
							tmp[++l] = '\0';
							volume[1] = (float)fast::parse_f(tmp);
							l = -1;
							chunk = 5;
							i++;
							j++; //+1 tick
							//printf("%d\n", j);
							//if (j == 6308)
							//	j = 6308;
							*addr[2] = true; //data ready to be pushed
							while (1) //wait for on_tick to ask for new tick
								if (!*addr[2] || !*addr[0]) //data has been pushed
								{
									env->data->epoch->push_front(epoch);
									env->data->ask->push_front(price[0]);
									env->data->bid->push_front(price[1]);
									env->data->vol->push_front(volume[0] + volume[1]);
									trigger::tick_event::circulate(env->data);
									*addr[1] = true; //data pushed
									break;
								}
						}
					}
					if (!*addr[0] && j >= TICK_BUFF_SIZE) //data set ready to be pushed
						*addr[0] = true;
				}
			}
			*addr[0] = false;
			fclose(histo);
			free(tmp);
			printf("Loaded %zu input rows from the data set\n", env->data->epoch->size());
			return (1);
		}
		printf("Failed to open the data file\n");
		return (0);
	}
}