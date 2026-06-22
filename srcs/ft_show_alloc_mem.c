#include "ft_malloc.h"

void	print_address(void *ptr)
{
	write(1, "0x", 2);
	if (ptr == 0)
		return ((void) write(1, "000000", 6));
	ft_puthex((unsigned long long)ptr);
}

size_t	print_blocks(t_block *block)
{
	t_block	*search;
	size_t	size;

	search = block;
	size = 0;
	while (search)
	{
		if (!search->free)
		{
			print_address((char *) search + mem_aligned(sizeof(t_block)));
			write(1, " - ", 3);
			print_address((char *) search + mem_aligned(sizeof(t_block)) + search->requested_size);
			write(1, " : ", 3);
			ft_putnbr(search->requested_size);
			write(1, " bytes\n", 7);
			size += search->requested_size;
		}
		search = search->next;
	}
	return size;
}

void show_alloc_mem()
{
	t_zone	*search;
	size_t	total_size;

	search = g_zones;
	total_size = 0;
	while (search)
	{
		write(1, "\n", 1);
		switch (search->type)
		{
			case TINY:
				write(1, "TINY : ", 7);
				break;
			case SMALL:
				write(1, "SMALL : ", 8);
				break;
			default:
				write(1, "LARGE : ", 8);
		}
		print_address(search);
		write(1, "\n", 1);
		total_size += print_blocks(search->block);
		search = search->next;
	}
	write(1, "Total : ", 8);
	ft_putnbr(total_size);
	write(1, " bytes\n", 7);
}
