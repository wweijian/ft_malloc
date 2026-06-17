#ifndef FT_MALLOC_H
# define FT_MALLOC_H

# include <stddef.h>
# include <unistd.h>

// MACROS

# define TINY_MAX_BLOCK_SIZE	128
# define SMALL_MAX_BLOCK_SIZE	1024
# define LARGE_MAX_BLOCK_SIZE	2048

# define TINY_ZONE_PAGE_MULT	4
# define SMALL_ZONE_PAGE_MULT	32
# define LARGE_ZONE_PAGE_MULT	128

// STRUCTS
typedef enum e_type
{
	TINY,
	SMALL,
	LARGE,
}	t_type;

typedef struct s_block
{
	size_t			size;
	int				free;
	struct s_block	*next;
}	t_block;

typedef struct s_zone
{
	t_type			type;
	t_block			*block;
	struct s_zone	*next;
}	t_zone;

// GLOBAL VARIABLE
extern t_zone	*g_zones;

// UTILS.C
size_t	get_zone_size(t_type type);
size_t	get_block_max_size(t_type type);

void	*allocate_memory(int type, size_t size);

// USER FUNCTIONS
void	*ft_malloc(size_t size);
void 	*ft_realloc(void *ptr, size_t size);
void 	free(void *ptr);
void	show_alloc_mem();

#endif
