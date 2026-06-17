#include "ft_malloc.h"
#include <sys/mman.h>
#include <unistd.h>
#include <stdio.h>

t_block	*next_available_block(t_block *block, size_t size)
{
	t_block	*search;

	search = block;
	while (search)
	{
		if (search->free && search->size > sizeof(t_block) + size)
			return search;
		search = search->next;
	}
	return NULL;
}

t_zone	*new_zone(int type, size_t size)
{
	t_zone	*zone;

	zone = mmap(NULL, get_zone_size(type),
				 PROT_READ | PROT_WRITE,
				 MAP_ANON | MAP_PRIVATE,
				 -1, 0);
	if (zone == MAP_FAILED)
		return NULL;
	zone->type = type;
	zone->block = (t_block *) ((char *)zone + sizeof(t_zone));
	zone->block->size = get_zone_size(type) - sizeof(t_zone) - sizeof(t_block);
	zone->block->free = 1;
	zone->block->next = NULL;
	zone->next = NULL;
	return zone;
}

t_block	*find_block(int type, size_t size)
{
	t_zone	*search;
	t_zone	*last_zone;
	t_zone	*zone;
	t_block	*block;

	search = g_zones;
	while(search)
	{
		if (search->type == type)
		{
			block = next_available_block(search->block, size);
			if (block)
				return block;
		}
		if (!search->next)
			last_zone = search;
		search = search->next;
	}
	zone = new_zone(type, size);
	if (!zone)
		return NULL;
	if (g_zones == NULL)
		g_zones = zone;
	else
		last_zone->next = zone;
	return zone->block;
}

void	*allocate_memory(int type, size_t size)
{
	t_block	*block;
	size_t	old_block_size;
	void	*return_address;
	
	block = find_block(type, size);
	return_address = (char *)block + sizeof(t_block);
	old_block_size = block->size;
	block->size = size;
	block->free = 0;
	block->next = (char *)return_address + size;
	block->next->size = old_block_size - size - sizeof(t_block);
	block->next->free = 1;
	block->next->next = NULL;

	return return_address;
}
