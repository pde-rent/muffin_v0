#pragma once

#ifdef _WIN32
# include "stdafx.h"
#elif __APPLE__
# include "../includes/stdafx.h"
#endif

namespace file
{
	static FILE *file;
	
	FILE			*open_w(string file_name, bool erase)
	{
		if ((file = (erase ? fopen(file_name, "wb") : fopen(file_name, "ab"))))
			return (file);
		return (NULL);
	}
	FILE			*open_r(string file_name)
	{
		return (((file = fopen(file_name, "rb")) ? file : NULL));
	}
	bool			close(FILE *file)
	{
		return (((fclose(file)) ? true : false));
	}
	bool			write_line(FILE *file, string str)
	{
		return ((fputs(str, file)) ? true : false);
	}
	bool			exists(string file_name)
	{
		if ((file = fopen(file_name, "rb")))
		{
			fclose(file);
			return (true);
		}
		return (false);
	}
	bool			write(string file_name, string str, bool erase)
	{
		if ((file = (erase ? fopen(file_name, "wb") : fopen(file_name, "ab"))))
		{
			fputs(str, file);
			fclose(file);
			return (true);
		}
		return (false);
	}
	unsigned long	count_lines(string file_name)
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
			while ((handle = fread(buf, 1, BUFF_SIZE, histo)))
			{
				char* ptr = buf;
				while ((ptr = (string)memchr(ptr, '\n', (buf + handle) - ptr)))
				{
					++ptr;
					++neol;
				}
			}
			printf("Characters to parse: %lu\nRows of data: %zu\nEstimate loading time: %zu ms\n",
				len, neol, neol / 290);
			fclose(histo);
			return (neol);
		}
		printf("Failed to open the data file\n");
		return (0);
	}
}