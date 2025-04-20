#include <stdio.h>


int binSearch(int a[100], int l, int r, int k)
{
	int c = 0;
	while(r - l > 1)
	{
		c = (l + r)/2;
		if(a[c] >= k)
		{
			r = c;
		}
		else
		{
			l = c;
		}
	}

	return r;
}

int main()
{
	int n, search;
	
	scanf("%d %d", &n, &search);

	int a[100];
	
	a[0] = 1;

	for(int i = 0; i < n; i++)
	{
		scanf("%d", &a[i + 1]);
	}

    printf("%d", binSearch(a, 0, n, search));

	return 0;
}
