#include "ft_malloc.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

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
