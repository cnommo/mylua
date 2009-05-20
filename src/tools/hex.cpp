#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#include "hex.h"

static const char sEncodeTable[] = "=&%\r\n";
static const char sHexTable[] = "0123456789ABCDEF";


inline static int get_index(char ch)
{
	char *p = strchr(sHexTable, toupper(ch));
	if(p) return p-sHexTable;
	return -1;
}

int String_Encode(const char* s, char** sEncoded)
{
	*sEncoded  = (char *)malloc(strlen(s)*3+4);
	if(*sEncoded == NULL)
		return -1;

	char *q=*sEncoded;
	bzero(*sEncoded, strlen(s)*3+4);
	for(const char *p=s; *p != '\0'; p++)
	{
        if(strchr(sEncodeTable, *p))
        {
            *q++ = '%';
            *q++ = sHexTable[((*p)>>4)&0x0f];
            *q++ = sHexTable[(*p)&0x0f];
        }
        else
            *q++ = *p;
    }
    return 0;
}

int HexString_Encode(const char* s, char** sEncoded)
{
	*sEncoded  = (char *)malloc(strlen(s)*3+4);
	if(*sEncoded == NULL)
		return -1;
	
	char *q=*sEncoded;
	bzero(*sEncoded, strlen(s)*3+4);
	for(const char *p=s; *p != '\0'; p++)
	{
		if (!isprint(*p) || strchr(sEncodeTable, *p))
		{
			*q++ = '%';
			*q++ = sHexTable[((*p)>>4)&0x0f];
			*q++ = sHexTable[(*p)&0x0f];
		}
		else
			*q++ = *p;	
	}
	return 0;
}

int String_Decode(const char *s, char** sDecoded)
{
	*sDecoded  = (char *)malloc(strlen(s)+4);
	if(*sDecoded == NULL)
		return -1;

	int i=0, j=0;
	char *q=*sDecoded;
	bzero(*sDecoded, strlen(s)+4);
	for(const char *p=s; *p != '\0'; p++)
	{
		if(*p  == '%')
		{
			if((i=get_index(*(++p))) < 0 || (j=get_index(*(++p))) <0)
				return -2;

			*q++ = (i<<4)+j;
		}
		else
			*q++ = *p;
	}
	return 0;
}


int HexString_Decode(const char *s, char** sDecoded)
{
	*sDecoded  = (char *)malloc(strlen(s)+4);
	if(*sDecoded == NULL)
		return -1;

	int i=0, j=0;
	char *q=*sDecoded;
	bzero(*sDecoded, strlen(s)+4);
	for(const char *p=s; *p != '\0'; p++)
	{
		if (*p == '%')
		{
			if((i=get_index(*(++p))) <0 || (j=get_index(*(++p))) <0)
				return -2;

			*q++ = (i<<4)+j;
		}
		else
			*q++ = *p;
	}
	return 0;
}
