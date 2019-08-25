#include "main.hpp"

int	put_error(std::string str, int err_code)
{
	printf("%s\n", str.c_str());
	// std::cout << str << std::endl; //endl = "\n" + flush
	return (err_code);
}
