#include <unistd.h>

void	ft_puthex(unsigned long long hex)
{
	const char	*base = "0123456789ABCDEF";

	if (hex >= 16)
	{
		ft_puthex(hex / 16);
		ft_puthex(hex % 16);
	}
	else
		write(1, &base[hex], 1);

}

void	ft_putnbr(size_t n)
{
	const char	*base = "0123456789";

	if (n >= 10)
	{
		ft_putnbr(n / 10);
		ft_putnbr(n % 10);
	}
	else
		write(1, &base[n], 1);
}
