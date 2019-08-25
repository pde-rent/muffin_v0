#pragma once

namespace file
{
	bool			exists(std::string file_name);
	FILE			*open_r(std::string file_name, bool binary = true);
	FILE			*open_w(std::string file_name, bool erase = false, bool binary = true);
	bool			close(FILE *file);
	bool			write_line(FILE *file, std::string str);
	bool			write(std::string file_name, std::string str, bool erase = false);
	unsigned long	count_lines(std::string file_name);
}
