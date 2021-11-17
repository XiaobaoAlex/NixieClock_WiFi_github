#include <stdio.h>
 
bool isCoincide(char *a, char *p)
{
	char *ptemp = p;
	while (*a != '\0')
	{
		if (*a == *p)
		{
			a++;
			p++;
		}
		else
		{
			a++;
			p = ptemp;
		}
		if (*p == '\0')
		{
			return 1;
		}
	}
	return 0;
}
 
int main()
{
	bool i;
	char *a = "\rAT+CWMODE=1\r\n000";
	char *b = "CMMODE";
 
	i = isCoincide(a, b);
 
	printf("%d\n", i);
 
	getchar();
 
	return 0;
}
