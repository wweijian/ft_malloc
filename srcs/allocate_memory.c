#include "ft_malloc.h"
#include <sys/mman.h>
#include <unistd.h>
#include <stdio.h>

size_t	mem_aligned(size_t size)
{
	if (size % ALIGNMENT_MULT == 0)
		return size;
	return (ALIGNMENT_MULT - (size % ALIGNMENT_MULT) + size);
}

t_block	*next_available_block(t_block *block, size_t size)
{
	t_block	*search;

	search = block;
	while (search)
	{
		if (search->free && search->size >= size)
			return search;
		search = search->next;
	}
	return NULL;
}

t_zone	*new_zone(t_type type, size_t size)
{
	t_zone	*zone;

	zone = mmap(NULL, get_zone_size(type, size),
				 PROT_READ | PROT_WRITE,
				 MAP_ANON | MAP_PRIVATE,
				 -1, 0);
	if (zone == MAP_FAILED)
		return NULL;
	zone->type = type;
	zone->block = (t_block *) ((char *)zone + mem_aligned(sizeof(t_zone)));
	zone->block->size = get_zone_size(type, size) - mem_aligned(sizeof(t_zone)) - mem_aligned(sizeof(t_block));
	zone->block->free = 1;
	zone->block->next = NULL;
	zone->next = NULL;
	return zone;
}

t_block	*find_block(t_type type, size_t size)
{
	t_zone	*search;
	t_zone	*last_zone;
	t_zone	*zone;
	t_block	*block;

	search = g_zones;
	last_zone = NULL;
	while (type != LARGE && search)
	{
		if (search->type == type)
		{
			block = next_available_block(search->block, size);
			if (block)
				return block;
		}
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

t_block	*split_memory(t_type type, t_block *block, size_t size)
{
	t_block	*new;
	size_t	leftover;

	if (!block)
		return NULL;
	new = NULL;
	block->free = 0;
	if (type == LARGE)
		return block;
	leftover = block->size - size;
	if (leftover >= mem_aligned(sizeof(t_block)) + get_block_min_size(type))
	{
		new = (t_block *)((char *) block + mem_aligned(sizeof(t_block)) + size);
		new->size = leftover - mem_aligned(sizeof(t_block));
		new->free = 1;
		new->next = block->next;
		block->size = size;
		block->next = new;
	}
	return block;
}

void	*allocate_memory(t_type type, size_t size)
{
	t_block	*block;
	
	size = mem_aligned(size);
	block = find_block(type, size);
	block = split_memory(type, block, size);

	if (!block)
		return NULL;
	return (void *)((char *) block + mem_aligned(sizeof(t_block)));
}
