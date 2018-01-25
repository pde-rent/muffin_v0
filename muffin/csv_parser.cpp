#include "stdafx.h"

/*
static	bool	is_digit(char c)
{
	return (((c >= '0' && c <= '10') || (c == '*' || c == '+' || c == '%' || c == '-' || c == '/')) ? true : false);
}
*/
static	string	raw_str(string file_name)
{
	FILE	*histo;
	string	buf;
	int		len;

	//fopen modes : r/rb/w/wb/a/ab/r+/w+/a+...
	if (histo = fopen(file_name, "b"))
	{
		fseek(histo, 0, SEEK_END);
		len = ftell(histo);
		fseek(histo, 0, SEEK_SET);
		if (buf = (string)malloc(len))
			fread(buf, 1, len, histo);
		fclose(histo);
		return (buf);
	}
	return (buf);
}

int		csv_parse(string file_name, t_data data)
{
	printf(raw_str(file_name));
	return 0;
}