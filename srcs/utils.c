#include "ft_malloc.h"

size_t	mem_aligned(size_t size)
{
	if (size % ALIGNMENT_MULT == 0)
		return size;
	return (ALIGNMENT_MULT - (size % ALIGNMENT_MULT) + size);
}

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
			size = mem_aligned(TINY_MAX_BLOCK_SIZE + 1);
			break;
	}
	return size;
}

t_type	get_zone_type(size_t size)
{
	size = mem_aligned(size);
	if (size <= TINY_MAX_BLOCK_SIZE)
		return TINY;
	else if (size <= SMALL_MAX_BLOCK_SIZE)
		return SMALL;
	else
		return LARGE;
}

t_zone	*find_zone(void *ptr)
{
	t_zone	*search;
	
	if (ptr == NULL)
		return NULL;
	search = g_zones;
	while (search)
	{
		if ((char *)ptr >= (char *)search + mem_aligned(sizeof(t_zone)) && (char *)ptr < (char *) search + search->size)
			return search;
		search = search->next;
	}
	return NULL;
}
