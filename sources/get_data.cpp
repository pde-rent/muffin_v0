#pragma once

#ifdef _WIN32
# include "stdafx.h"
#elif __APPLE__
# include "../includes/stdafx.h"
#endif
	
namespace tick
{
	static	FILE		*histo;

	int		load_all(string file_name, t_env *env)
	{
		string			tmp;
		char			buf[BUFF_SIZE + 1];
		size_t			i = 0;
		size_t			j = 0;
		bool			buffer_filled = false;
		size_t			nb_ticks = file::count_lines(file_name);
		//size_t			max_ticks = (TICK_BUFF_SIZE > nb_ticks ? nb_ticks : TICK_BUFF_SIZE);
		uint16_t		chunk = 3;
		int				l = -1;
		//fopen modes : r/rb/w/wb/a/ab/r+/w+/a+...
		if (((histo = fopen(file_name, "rb")) != NULL))
		{
			env->ticker->size = nb_ticks;
			tmp = (string)malloc(sizeof(char) * 100);
			//if ((fread(buf, 1, line_ln, histo) == line_ln))
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
							env->data->epoch->push_front((size_t)fast::parse_zu(tmp));
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
							env->data->px->push_front((float)fast::parse_f(tmp));
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
							env->data->vol->push_front((float)fast::parse_f(tmp));
							l = -1;
							chunk = 3;
							i++;
							j++;
							if (buffer_filled)
							{
								//the whole tick data is now readable
								trigger::tick_event::next(env);
								trigger::tick_event::circulate(env->data);
							}
						}
					}
					if (!buffer_filled && j == TICK_BUFF_SIZE)
						buffer_filled = true;
				}
			}
			fclose(histo);
			//free(tmp);
			printf("Loaded %zu input rows from the data set\n", env->data->epoch->size());
			return (1);
		}
		printf("Failed to open the data file\n");
		return (0);
	}
}