#include "ft_malloc.h"

t_zone *g_zones = NULL;

void *ft_malloc(size_t size) {
	void	*ptr;
	int		zone;

	if (size < TINY_MAX_BLOCK_SIZE)
		zone = TINY;
	else if (size < SMALL_MAX_BLOCK_SIZE)
		zone = SMALL;
	else
		zone = LARGE;
	ptr = allocate_memory(zone, size);
	return (ptr);
}
