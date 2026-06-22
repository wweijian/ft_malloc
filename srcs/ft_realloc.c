#include "ft_malloc.h"

int	check_extensible(t_zone *zone, t_block *block, size_t size)
{
	t_block	*search;

	search = zone->block;
	size = mem_aligned(size);
	while (search)
	{
		if (search == block)
		{
			if (search->actual_size >= size)
				return 2;
			if (search->next && search->next->free
				&& search->next->actual_size + mem_aligned(sizeof(t_block)) + search->actual_size >= size)
				return 1;
			return 0;
		}
		search = search->next;
	}
	return -1;
}

void	*ft_memcpy(void *dest, const void *src, size_t n)
{
	char		*destbuf;
	const char	*srcbuf;

	if (!dest && !src)
		return (0);
	if (n == 0 || (dest == src))
		return (dest);
	destbuf = (char *)dest;
	srcbuf = (const char *)src;
	while (n != 0)
	{
		if (*destbuf != *srcbuf)
			*destbuf = *srcbuf;
		destbuf++;
		srcbuf++;
		n--;
	}
	return (dest);
}

void	*reallocate_memory(t_block *block, void *ptr, size_t size)
{
	void	*new;

	new = allocate_memory(size);
	size = mem_aligned(size);
	if (!new)
	{
		pthread_mutex_unlock(&g_malloc_mutex);
		return NULL;
	}
	if (size >= block->requested_size)
		size = block->requested_size;
	ft_memcpy(new, ptr, size);
	free_memory(ptr);
	return new;
}

t_block *merge_memory_block(t_zone *zone, t_block *block)
{
	t_block	*search;

	search = zone->block;
	while (search->next)
	{
		if (search == block)
		{
			block->actual_size += block->next->actual_size + mem_aligned(sizeof(t_block));
			block->next = block->next->next;
			break ;
		}
		search = search->next;
	}
	return block;
}

void	*ft_realloc(void *ptr, size_t size)
{
	t_zone	*zone;
	t_block	*block;
	void	*new_ptr;

	if (!ptr)
		return ft_malloc(size);
	pthread_mutex_lock(&g_malloc_mutex);
	zone = find_zone(ptr);
	if (!zone)
	{
		pthread_mutex_unlock(&g_malloc_mutex);
		return NULL;
	}
	block = (t_block *)((char *) ptr - mem_aligned(sizeof(t_block)));
	switch (check_extensible(zone, block, size))
	{
		case 0:
			new_ptr = reallocate_memory(block, ptr, size);
		case 1:
			merge_memory_block(zone, block);
			/* fallthrough */
		case 2:
			split_memory(zone->type, block, size);
			break ;
		default:
			new_ptr = NULL;
	}
	pthread_mutex_unlock(&g_malloc_mutex);
	return new_ptr;
}
