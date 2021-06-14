#include<stdio.h>
int adder(int n)
{
	int sum = 0, i;
	for (i = 2; i <= n; i += 2)
		sum += i;
	return sum;
}
int main()
{
	int n;
	printf("Enter the limit\n");
	scanf("%d", &n);
	printf("Result is %d\n", adder(n) );
	return 0;
}