#include "stdafx.h"

static	long	csv_rows(string file_name)
{
	FILE			*histo;
	char			buf[BUFF_SIZE];
	size_t			neol = 0;
	int				handle;
	unsigned long	len = 0;
	//fopen modes : r/rb/w/wb/a/ab/r+/w+/a+...
	if (((histo = fopen(file_name, "r")) != NULL))
	{
		fseek(histo, 0, SEEK_END);
		len = (ftell(histo));
		fseek(histo, 0, SEEK_SET);
		while (handle = fread(buf, 1, BUFF_SIZE, histo))
		{
			char* ptr = buf;
			while ((ptr = (string)memchr(ptr, '\n', (buf + handle) - ptr)))
			{
				++ptr;
				++neol;
			}
		}
		printf("Characters to parse: %zu\nRows of data: %d\nEstimate loading time: %d ms\n",
				len, neol, neol/290);
		fclose(histo);
		return (neol);
	}
	printf("Failed to open the data file\n");
	return (0);
}

namespace tick
{
	static	FILE		*histo;
/*	static	size_t		timestamp;
	static	float		volume;
	static	float		price;

	int		set(string file_name, t_tick_data *data)
	{
		int		i = -1;

		histo = fopen(file_name, "rb");
		if ((fscanf(histo, "%zu,%f,%f", &timestamp, &price, &volume) == 3))	//skip headers
			while ((fscanf(histo, "%zu,%f,%f", &timestamp, &price, &volume) == 3) && ++i < TICK_BUFF_SIZE)
			{
				data->epoch->push_front(timestamp);	//first epoch
				data->px->push_front(price);		//first price
				data->vol->push_front(volume);		//first volume
				return (1);
			}
		return (0);
	}
	int		next(t_tick_data *data)
	{
		if ((fscanf(histo, "%zu,%f,%f", &timestamp, &price, &volume) == 3))
		{
			data->epoch->push_front(timestamp);
			data->px->push_front(price);
			data->vol->push_front(volume);
			//data->epoch->resize(TICK_BUFF_SIZE);		//data->epoch->erase(TICK_BUFF_SIZE);
			//data->px->resize(TICK_BUFF_SIZE);		//data->px->erase(TICK_BUFF_SIZE);
			//data->vol->resize(TICK_BUFF_SIZE);;		//data->vol->erase(TICK_BUFF_SIZE);
			return (1);
		}
		fclose(histo);
		return (0);
	}*/
	int		load_all(string file_name, t_env *env)
	{
		string			tmp;
		char			buf[BUFF_SIZE + 1];
		size_t			i = 0;
		size_t			j = 0;
		size_t			nb_ticks = csv_rows(file_name);
		int				chunk = 3;
		int				l = -1;
		//fopen modes : r/rb/w/wb/a/ab/r+/w+/a+...
		if (((histo = fopen(file_name, "rb")) != NULL))
		{
			env->ticker->size = nb_ticks;
			tmp = (string)malloc(sizeof(char) * 50);
			//if ((fread(buf, 1, line_ln, histo) == line_ln))
			//	fseek(histo, 0, SEEK_SET); //skip headers (byte size different from data lines)
			while (fread(buf, 1, BUFF_SIZE, histo)) //1 = sizeof(char)
			{
				buf[BUFF_SIZE] = '\0';
				i = 0;
				while (buf[i] && (j < nb_ticks) && (j < TICK_BUFF_SIZE)) // is ascii >> not null nor lost
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
							j++; //the whole tick data is now readable
							trigger::tick(env);
							trigger::circular(env->data);
						}
					}

				}
			}
			fclose(histo);
			free(tmp);
			printf("Loaded %zu input rows from the data set\n", env->data->epoch->size());
			return (1);
		}
		printf("Failed to open the data file\n");
		return (0);
	}
}

/*int		csv_tick_parse(string file_name, t_tick_data *data)
{
	int	rows = csv_rows(file_name);
	return 0;
}

int		csv_ohlc_parse(string file_name, t_ohlc_data *data)
{
	csv_rows(file_name);
	return 0;
}*/