#include "ft_malloc.h"

t_zone *g_zones = NULL;

void *ft_malloc(size_t size) {
	void	*ptr;
	t_type	type;

	if (size <= TINY_MAX_BLOCK_SIZE)
		type = TINY;
	else if (size <= SMALL_MAX_BLOCK_SIZE)
		type = SMALL;
	else
		type = LARGE;
	ptr = allocate_memory(type, size);
	return (ptr);
}
