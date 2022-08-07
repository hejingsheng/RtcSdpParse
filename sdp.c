#include "sdp.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int sdp_parse_version(sdp_desc_st *sdp, char *value)
{
	sdp->version = atoi(value);
	return 0;
}

int sdp_parse_origin(sdp_desc_st *sdp, char *value)
{
	char *q = NULL;
	char* p = value;
	int i = 0;
	do
	{
		q = p;
		p = strstr(q, " ");
		if (p != NULL)
		{
			*p = '\0';
			p++;
			if (i == 0)
			{
				sdp->o_name = q;
			}
			else if (i == 1)
			{
				sdp->o_session_id = atoll(q);
			}
			else if (i == 2)
			{
				sdp->o_session_version = atol(q);
			}
			else if (i == 3)
			{
				sdp->o_net_type = q;
			}
			else if (i == 4)
			{
				sdp->o_addr_type = q;
				sdp->o_addr = p;
			}
			i++;
		}
	} while (p != NULL);
	return 0;
}

int sdp_parse_session(sdp_desc_st *sdp, char *value)
{
	sdp->s_name = value;
	return 0;
}

int sdp_parse_time(sdp_desc_st *sdp, char *value)
{
	char* p = strstr(value, " ");
	if (p == NULL)
	{
		return -1;
	}
	sdp->t_start = atoi(value);
	sdp->t_end = atoi(p + 1);
	return 0;
}

int sdp_parse(char *sdp_str, int len, sdp_desc_st *sdp)
{
	char* delim = "\r\n";
	char* p = strtok(sdp_str, delim);
	while (p != NULL) 
	{
		char key = p[0];
		char *value = p + 2;
		printf("%c=%s\n", key, value);
		switch (key)
		{
		case 'v':
			sdp_parse_version(sdp, value);
			break;
		case 'o':
			sdp_parse_origin(sdp, value);
			break;
		case 's':
			sdp_parse_session(sdp, value);
			break;
		case 't':
			sdp_parse_time(sdp, value);
			break;
		default:
			break;
		}
		p = strtok(NULL, delim);
	}
	return 0;
}


int main()
{
	char *sdp_str = "v=0\r\n\
o=- 1817118955388369990 2 IN IP4 127.0.0.1\r\n\
s=-\r\n\
t=0 0\r\n\
a=group:BUNDLE 0 1\r\n\
a=extmap-allow-mixed\r\n\
a=msid-semantic: WMS\r\n";
	char sdp_s[1024];
	sdp_desc_st sdp;
	strcpy(sdp_s, sdp_str);
	sdp_parse(sdp_s, strlen(sdp_s), &sdp);
	return 0;
}