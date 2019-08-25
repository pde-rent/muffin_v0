#include "main.hpp"

namespace file
{
	static FILE *file;
	
	FILE			*open_w(std::string file_name, bool erase, bool binary)
	{
		std::string mode = erase ? (binary ? "wb" : "w") : (binary ? "ab" : "a");
		return ((file = fopen(file_name.c_str(), mode.c_str())) ? file : NULL);
	}
	FILE			*open_r(std::string file_name, bool binary)
	{
		return ((file = fopen(file_name.c_str(), binary ? "rb" : "r")) ? file : NULL);
	}
	bool			close(FILE *file)
	{
		return (((fclose(file)) ? true : false));
	}
	bool			write_line(FILE *file, std::string str)
	{
		return ((fputs(str.c_str(), file)) ? true : false);
	}
	bool			exists(std::string file_name)
	{
		if ((file = open_r(file_name)))
		{
			fclose(file);
			return (true);
		}
		return (false);
	}
	bool			write(std::string file_name, std::string str, bool erase)
	{
		if ((file = open_w(file_name, erase)))
		{
			fputs(str.c_str(), file);
			fclose(file);
			return (true);
		}
		return (false);
	}
	unsigned long	count_lines(std::string file_name)
	{
		FILE			*histo;
		char			buf[BUFF_SIZE];
		size_t			neol = 0;
		int				handle;
		unsigned long	len = 0;
		//fopen modes : r/rb/w/wb/a/ab/r+/w+/a+...
		if ((histo = open_r(file_name, false)))
		{
			fseek(histo, 0, SEEK_END);
			len = (ftell(histo));
			fseek(histo, 0, SEEK_SET);
			while ((handle = fread(buf, 1, BUFF_SIZE, histo)))
			{
				char *ptr = &buf[0];
				// while *ptr sets to the next end of line, keep on searching
				while ((ptr = (char *)memchr(ptr, '\n', (buf + handle) - ptr)))
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