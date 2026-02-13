#include <stdio.h>

#define ALPHABET_SIZE 256

unsigned char templ[] = "hello";

int uc_strlen(unsigned char* s)
{
	int len = 0;
	while (s[len] != 0)
		len++;
	return len;
}

int boyer_moore_search(unsigned char* text)
{
	int n = uc_strlen(text);
	int m = uc_strlen(templ);

	int badchar[ALPHABET_SIZE];

	for (int i = 0; i < ALPHABET_SIZE; i++)
		badchar[i] = -1;

	for (int i = 0; i < m; i++)
		badchar[(unsigned char)templ[i]] = i;

	int shift = 0;

	while (shift <= n - m)
	{
		int j = m - 1;

		while (j >= 0 && templ[j] == text[shift + j])
			j--;

		if (j < 0)
			return shift;

		int bcIndex = badchar[(unsigned char)text[shift + j]];
		int move = j - bcIndex;
		if (move < 1)
			move = 1;

		shift += move;
	}

	return -1;
}

int main()
{
	unsigned char text[] = "hello world";

	int pos = boyer_moore_search(text);
	printf("pos = %d\n", pos);

	return 0;
}
