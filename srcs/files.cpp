#include "main.hpp"

namespace file
{
	FILE			*open_w(std::string file_name, bool erase, bool binary)
	{
		std::string mode = erase ? (binary ? "wb" : "w") : (binary ? "ab" : "a");
		return (fopen(file_name.c_str(), mode.c_str()));
	}
	FILE			*open_r(std::string file_name, bool binary)
	{
		return (fopen(file_name.c_str(), binary ? "rb" : "r"));
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
		FILE *file;

		if ((file = open_r(file_name)))
		{
			fclose(file);
			return (true);
		}
		return (false);
	}
	bool			write(std::string file_name, std::string str, bool erase)
	{
		FILE *file;
	
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
		FILE			*f;
		char			buf[BUFF_SIZE];
		size_t			neol = 0;
		int				handle;
		// unsigned long	len = 0;
		//fopen modes : r/rb/w/wb/a/ab/r+/w+/a+...
		if (!(f = open_r(file_name))) {
			return (put_error(ERR_NO_HISTO_FILE, 0));
		} else {
			// printf("Stream%s\n", file_name.c_str());
		}
		// fseek(f, 0, SEEK_END);	// move file pointer to EOF
		// len = (ftell(file));	// read EOF location
		// printf("Characters to parse: %lu\n", ftell(f));
		fseek(f, 0, SEEK_SET);	// move file pointer back to start
		while ((handle = fread(buf, 1, BUFF_SIZE, f)))
		{
			char *ptr = &buf[0];
			// while *ptr sets to the next end of line, keep on searching
			while ((ptr = (char *)memchr(ptr, '\n', (&buf[0] + handle) - ptr)))
			{
				++ptr;
				++neol;
			}
		}
		fclose(f);
		return (neol);
	}
}
