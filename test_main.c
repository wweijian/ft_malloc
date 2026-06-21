#include "ft_malloc.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

typedef enum e_test_result
{
	TEST_PASS,
	TEST_FAIL,
}	t_test_result;

typedef t_test_result	(*t_test_fn)(void);

typedef struct s_test
{
	const char	*name;
	t_test_fn	fn;
	int			line;
}	t_test;

#define TEST_CASE(fn) \
	enum { fn##_line = __LINE__ }; \
	static t_test_result	fn(void)

#define EXPECT(expr) \
	do { \
		if (!(expr)) { \
			printf("    %s:%d: expected %s\n", __FILE__, __LINE__, #expr); \
			return TEST_FAIL; \
		} \
	} while (0)

static int	is_aligned(void *ptr)
{
	return ((uintptr_t)ptr % ALIGNMENT_MULT == 0);
}

static int	capture_show_alloc_mem(char *buffer, size_t buffer_size)
{
	int		pipe_fd[2];
	int		stdout_copy;
	ssize_t	bytes_read;

	if (pipe(pipe_fd) == -1)
		return -1;
	stdout_copy = dup(STDOUT_FILENO);
	if (stdout_copy == -1)
	{
		close(pipe_fd[0]);
		close(pipe_fd[1]);
		return -1;
	}
	fflush(stdout);
	if (dup2(pipe_fd[1], STDOUT_FILENO) == -1)
	{
		close(pipe_fd[0]);
		close(pipe_fd[1]);
		close(stdout_copy);
		return -1;
	}
	close(pipe_fd[1]);
	show_alloc_mem();
	fflush(stdout);
	if (dup2(stdout_copy, STDOUT_FILENO) == -1)
	{
		close(pipe_fd[0]);
		close(stdout_copy);
		return -1;
	}
	close(stdout_copy);
	bytes_read = read(pipe_fd[0], buffer, buffer_size - 1);
	close(pipe_fd[0]);
	if (bytes_read < 0)
		return -1;
	buffer[bytes_read] = '\0';
	return 0;
}

TEST_CASE(test_show_alloc_mem_prints_allocations)
{
	char	buffer[4096];

	EXPECT(ft_malloc(TINY_MAX_BLOCK_SIZE + ALIGNMENT_MULT) != NULL);
	EXPECT(ft_malloc(ALIGNMENT_MULT * 4) != NULL);
	EXPECT(capture_show_alloc_mem(buffer, sizeof(buffer)) == 0);
	EXPECT(strstr(buffer, "SMALL : ") != NULL);
	EXPECT(strstr(buffer, "TINY : ") != NULL);
	EXPECT(strstr(buffer, "144 byte") != NULL);
	EXPECT(strstr(buffer, "64 byte") != NULL);
	EXPECT(strstr(buffer, "Total : ") != NULL);
	return TEST_PASS;
}

TEST_CASE(test_free_marks_block_free)
{
	void	*first;
	void	*second;
	t_block	*first_block;
	t_block	*second_block;

	first = ft_malloc(32);
	second = ft_malloc(32);
	EXPECT(first != NULL);
	EXPECT(second != NULL);
	first_block = (t_block *)((char *)first - mem_aligned(sizeof(t_block)));
	second_block = (t_block *)((char *)second - mem_aligned(sizeof(t_block)));
	EXPECT(first_block->free == 0);
	EXPECT(second_block->free == 0);
	ft_free(first);
	EXPECT(first_block->free == 1);
	EXPECT(second_block->free == 0);
	ft_free(NULL);
	ft_free(first);
	EXPECT(first_block->free == 1);
	return TEST_PASS;
}

TEST_CASE(test_realloc_preserves_data_when_growing)
{
	unsigned char	*ptr;
	unsigned char	*new_ptr;
	size_t			i;

	ptr = ft_malloc(32);
	EXPECT(ptr != NULL);
	i = 0;
	while (i < 32)
	{
		ptr[i] = (unsigned char)(0xa0 + i);
		i++;
	}
	new_ptr = ft_realloc(ptr, 512);
	EXPECT(new_ptr != NULL);
	EXPECT(is_aligned(new_ptr));
	i = 0;
	while (i < 32)
	{
		EXPECT(new_ptr[i] == (unsigned char)(0xa0 + i));
		i++;
	}
	return TEST_PASS;
}

TEST_CASE(test_realloc_null_behaves_like_malloc)
{
	unsigned char	*ptr;
	size_t			i;

	ptr = ft_realloc(NULL, 48);
	EXPECT(ptr != NULL);
	EXPECT(is_aligned(ptr));
	i = 0;
	while (i < 48)
	{
		ptr[i] = (unsigned char)i;
		EXPECT(ptr[i] == (unsigned char)i);
		i++;
	}
	return TEST_PASS;
}

TEST_CASE(test_malloc_returns_aligned_writable_memory)
{
	unsigned char	*ptr;
	size_t			i;

	ptr = ft_malloc(64);
	EXPECT(ptr != NULL);
	EXPECT(is_aligned(ptr));
	i = 0;
	while (i < 64)
	{
		ptr[i] = (unsigned char)(i + 1);
		i++;
	}
	i = 0;
	while (i < 64)
	{
		EXPECT(ptr[i] == (unsigned char)(i + 1));
		i++;
	}
	return TEST_PASS;
}

TEST_CASE(test_many_small_allocations_stay_aligned)
{
	void	*ptr;
	size_t	size;

	size = 1;
	while (size <= TINY_MAX_BLOCK_SIZE)
	{
		ptr = ft_malloc(size);
		EXPECT(ptr != NULL);
		EXPECT(is_aligned(ptr));
		memset(ptr, (int)(size & 0xff), size);
		size++;
	}
	return TEST_PASS;
}

TEST_CASE(test_allocations_are_distinct_and_do_not_overlap)
{
	unsigned char	*a;
	unsigned char	*b;
	size_t			i;

	a = ft_malloc(32);
	b = ft_malloc(32);
	EXPECT(a != NULL);
	EXPECT(b != NULL);
	EXPECT(a != b);
	memset(a, 0x11, 32);
	memset(b, 0x22, 32);
	i = 0;
	while (i < 32)
	{
		EXPECT(a[i] == 0x11);
		EXPECT(b[i] == 0x22);
		i++;
	}
	return TEST_PASS;
}

TEST_CASE(test_boundary_sizes_are_allocatable)
{
	void	*tiny_last;
	void	*small_first;
	void	*small_last;
	void	*large_first;

	large_first = ft_malloc(SMALL_MAX_BLOCK_SIZE + 1);
	small_last = ft_malloc(SMALL_MAX_BLOCK_SIZE);
	small_first = ft_malloc(TINY_MAX_BLOCK_SIZE + 1);
	tiny_last = ft_malloc(TINY_MAX_BLOCK_SIZE);
	EXPECT(tiny_last != NULL);
	EXPECT(small_first != NULL);
	EXPECT(small_last != NULL);
	EXPECT(large_first != NULL);
	EXPECT(is_aligned(tiny_last));
	EXPECT(is_aligned(small_first));
	EXPECT(is_aligned(small_last));
	EXPECT(is_aligned(large_first));
	memset(tiny_last, 0xa1, TINY_MAX_BLOCK_SIZE);
	memset(small_first, 0xb2, TINY_MAX_BLOCK_SIZE + 1);
	memset(small_last, 0xc3, SMALL_MAX_BLOCK_SIZE);
	memset(large_first, 0xd4, SMALL_MAX_BLOCK_SIZE + 1);
	return TEST_PASS;
}

TEST_CASE(test_malloc_zero_does_not_crash)
{
	void	*ptr;

	ptr = ft_malloc(0);
	if (ptr != NULL)
		EXPECT(is_aligned(ptr));
	return TEST_PASS;
}

static int	run_test(t_test test, int *passed, int *failed)
{
	t_test_result	result;

	printf("[RUN]  test_main.c:%d %s\n", test.line, test.name);
	result = test.fn();
	if (result == TEST_PASS)
	{
		(*passed)++;
		printf("[PASS] test_main.c:%d %s\n", test.line, test.name);
		return 0;
	}
	(*failed)++;
	printf("[FAIL] test_main.c:%d %s\n", test.line, test.name);
	return 1;
}

static void	print_test_index(const t_test *tests, size_t count)
{
	size_t	i;

	printf("Test cases:\n");
	i = 0;
	while (i < count)
	{
		printf("  %2zu. test_main.c:%d %s\n",
			i + 1, tests[i].line, tests[i].name);
		i++;
	}
	printf("\n");
}

int	main(void)
{
	static const t_test	tests[] = {
		{"boundary sizes are allocatable",
			test_boundary_sizes_are_allocatable,
			test_boundary_sizes_are_allocatable_line},
		{"show_alloc_mem prints allocations",
			test_show_alloc_mem_prints_allocations,
			test_show_alloc_mem_prints_allocations_line},
		{"free marks block free", test_free_marks_block_free,
			test_free_marks_block_free_line},
		{"realloc preserves data when growing",
			test_realloc_preserves_data_when_growing,
			test_realloc_preserves_data_when_growing_line},
		{"realloc null behaves like malloc",
			test_realloc_null_behaves_like_malloc,
			test_realloc_null_behaves_like_malloc_line},
		{"malloc returns aligned writable memory",
			test_malloc_returns_aligned_writable_memory,
			test_malloc_returns_aligned_writable_memory_line},
		{"many small allocations stay aligned",
			test_many_small_allocations_stay_aligned,
			test_many_small_allocations_stay_aligned_line},
		{"allocations are distinct and do not overlap",
			test_allocations_are_distinct_and_do_not_overlap,
			test_allocations_are_distinct_and_do_not_overlap_line},
		{"malloc zero does not crash", test_malloc_zero_does_not_crash,
			test_malloc_zero_does_not_crash_line},
	};
	int					passed;
	int					failed;
	size_t				i;

	passed = 0;
	failed = 0;
	print_test_index(tests, sizeof(tests) / sizeof(tests[0]));
	i = 0;
	while (i < sizeof(tests) / sizeof(tests[0]))
	{
		run_test(tests[i], &passed, &failed);
		i++;
	}
	printf("\n%d passed, %d failed\n", passed, failed);
	if (failed != 0)
		return 1;
	return 0;
}
