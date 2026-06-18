#include "ft_malloc.h"

size_t	get_zone_size(t_type type, size_t size)
{
	size_t	zone_size;
	size_t	page_size;

	page_size = getpagesize();
	switch (type)
	{
		case	TINY:
			return (TINY_ZONE_PAGE_MULT * page_size);
		case	SMALL:
			return (SMALL_ZONE_PAGE_MULT * page_size);
		default:
			break ;
	}
	zone_size = size + mem_aligned(sizeof(t_zone)) + mem_aligned(sizeof(t_block));
	if (zone_size % page_size != 0)
		zone_size += page_size - (zone_size % page_size);
	return (zone_size);
}

size_t	get_block_min_size(t_type type)
{
	int	size;

	switch (type)
	{
		case TINY:
			size = 0;
			break;
		default:
			size = TINY_MAX_BLOCK_SIZE + 1;
			break;
	}
	return size;
}
