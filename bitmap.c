#include <stdio.h>

#define N  100
#define SIZE_INT 32

static int a[1 + N / SIZE_INT];

int set(int i) {
	a[i / 32] |= 1 << (i % 32);
}

int clear(int i)
{
	a[i / 32] &= ~ 1 << (i % 32);
}

int test(int i)
{
	return (a[i / 32] &= 1 << (i % 32));
}

int main(void)
{
	int i;
	for (i = 0; i < N; i++)
		clear(i);
	scanf("%d", &i);
	printf("%d\n", i);
	set(i);
	for (i = 0; i < N; i++) {
		/*printf("%d\n", i);*/
		if (test(i))
			printf("%d\n", i);
	}

	return 0;
}
