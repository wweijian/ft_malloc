#ifndef FT_MALLOC_H
# define FT_MALLOC_H

# include <stddef.h>
# include <unistd.h>

// MACROS

# define TINY_MAX_BLOCK_SIZE	128
# define SMALL_MAX_BLOCK_SIZE	1024

# define TINY_ZONE_PAGE_MULT	4
# define SMALL_ZONE_PAGE_MULT	32

# define ALIGNMENT_MULT			16
# define MIN_ALLOC_SIZE			ALIGNMENT_MULT

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
	size_t			size;
	struct s_zone	*next;
}	t_zone;

// GLOBAL VARIABLE
extern t_zone	*g_zones;

// UTILS.C
size_t	get_zone_size(t_type type, size_t size);
size_t	mem_aligned(size_t size);
size_t	get_block_min_size(t_type type);
t_type	get_zone_type(size_t size);
t_zone	*find_zone(void *ptr);

// ft_malloc.c
t_block	*split_memory(t_type type, t_block *block, size_t size);

// PRINT_NUM.C
void	ft_puthex(unsigned long long hex);
void	ft_putnbr(size_t n);

// USER FUNCTIONS
void	*ft_malloc(size_t size);
void 	*ft_realloc(void *ptr, size_t size);
void 	ft_free(void *ptr);
void	show_alloc_mem();

#endif
