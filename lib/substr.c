/* implementation of string operations */
char *substr(char *str, char *sub, int index, int len);

char *substr(char *str, char *sub, int i, int l)
{
	int j = 0;
	char *p = str;

	while (i-- > 0) p++;
	for (j = 0; j < l && p[j] != '\0'; j++)
		sub[j] = p[j];
	if (j <= l) sub[j] = '\0'; // encounter a null terminator

	return sub;
}

