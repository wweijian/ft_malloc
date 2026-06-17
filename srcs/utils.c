#include "ft_malloc.h"

size_t	get_zone_size(int type)
{
	int	size;

	switch (type)
	{
		case	TINY:
			size = TINY_ZONE_PAGE_MULT;
			break;
		case	SMALL:
			size = SMALL_ZONE_PAGE_MULT;
			break;
		default:
			size = LARGE_ZONE_PAGE_MULT;
	}
	return size * getpagesize();
}

size_t	get_block_max_size(t_type type)
{
	int	size;

	switch (type)
	{
		case	TINY:
			size = TINY_ZONE_MAX_BLOCK_SIZE;
			break;
		case	SMALL:
			size = SMALL_ZONE_MAX_BLOCK_SIZE;
			break;
		default:
			size = LARGE_ZONE_MAX_BLOCK_SIZE;
	}
	return size;
}
