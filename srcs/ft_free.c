#include "ft_malloc.h"
#include <sys/mman.h>

int single_non_large_zone(t_type type)
{
	t_zone	*search;
	int		count;

	if (type == LARGE)
		return 0;
	search = g_zones;
	count = 0;
	while (search)
	{
		if (search->type == type)
		{
			count++;
			if (count > 1)
				return 0;
		}
		search = search->next;
	}
	return 1;
}

void unmap_zone(t_zone *zone)
{
	t_zone	*search;
	
	if (single_non_large_zone(zone->type) || !(zone->block->next == NULL && zone->block->free == 1))
		return ;
	if (zone == g_zones)
	{
		g_zones = zone->next;
		return ((void) munmap(zone, zone->size));
	}
	search = g_zones;
	while (search->next)
	{
		if (search->next == zone)
		{
			search->next = search->next->next;
			return ((void) munmap(zone, zone->size));
		}
		search = search->next;
	}
}

int	free_block(t_zone *zone, void *ptr)
{
	t_block	*search;
	t_block	*block;

	search = zone->block;
	block = (t_block *)((char *) ptr - mem_aligned(sizeof(t_block)));
	while (search)
	{
		if (block == search)
		{
			if (search->free == 1)
				return 0;
			search->free = 1;
			search->requested_size = 0;
			return 1;
		}
		search = search->next;
	}
	return 0;
}

void	coalesce_block(t_zone *zone)
{
	t_block	*search;

	search = zone->block;
	while (search)
	{
		if (search->free == 1 && search->next && search->next->free ==1)
		{
			search->actual_size += search->next->actual_size + mem_aligned(sizeof(t_block));
			search->next = search->next->next;
		}
		else
			search = search->next;
	}
}

void	ft_free(void *ptr)
{
	t_zone	*search;
	
	search = find_zone(ptr);
	if (search && free_block(search, ptr))
	{
		coalesce_block(search);
		unmap_zone(search);
	}
}
