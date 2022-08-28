#include "sdp.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define ALLOC_MEMORY(size) \
	void *mem = malloc(size); \
	assert(mem != NULL); \
	memset(mem, 0, size); \

#define FREE_MEMORY(ptr) \
	if (ptr) \
		free(ptr); \

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
				ALLOC_MEMORY(strlen(q) + 1);
				sdp->o_name = (char*)mem;
				strcpy(sdp->o_name, q);
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
				ALLOC_MEMORY(strlen(q) + 1);
				sdp->o_net_type = (char*)mem;
				strcpy(sdp->o_net_type, q);
			}
			else if (i == 4)
			{
				ALLOC_MEMORY(strlen(q) + 1);
				sdp->o_addr_type = (char*)mem;
				strcpy(sdp->o_addr_type, q);
			}
			i++;
		}
		else
		{
			ALLOC_MEMORY(strlen(q) + 1);
			sdp->o_addr = (char*)mem;
			strcpy(sdp->o_addr, q);
		}
	} while (p != NULL);
	return 0;
}

int sdp_parse_session(sdp_desc_st *sdp, char *value)
{
	ALLOC_MEMORY(strlen(value) + 1);
	sdp->s_name = (char*)mem;
	strcpy(sdp->s_name, value);
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

int sdp_parse_attribute(sdp_desc_st *sdp, char *value)
{
	char *p;
	
	p = strchr(value, ':');
	if (p != NULL)
	{
		if (strncmp(value, "ice-ufrag", strlen("ice-ufrag")) == 0)
		{
			ALLOC_MEMORY(strlen(p + 1) + 1);
			sdp->media_desc->a_ice_ufrag = mem;
			strcpy(sdp->media_desc->a_ice_ufrag, p + 1);
		}
		else if (strncmp(value, "ice-pwd", strlen("ice-pwd")) == 0)
		{
			ALLOC_MEMORY(strlen(p + 1) + 1);
			sdp->media_desc->a_ice_pwd = mem;
			strcpy(sdp->media_desc->a_ice_pwd, p + 1);
		}
		else if (strncmp(value, "fingerprint", strlen("fingerprint")) == 0)
		{
			char *q;
			q = strchr(p + 1, ' ');
			*q = '\0';
			strcpy(sdp->media_desc->finger_print.type, p + 1);
			ALLOC_MEMORY(strlen(q + 1) + 1);
			sdp->media_desc->finger_print.finger_print = mem;
			strcpy(sdp->media_desc->finger_print.finger_print, q + 1);
		}
		else if (strncmp(value, "setup", strlen("setup")) == 0)
		{
			if (strncmp(p + 1, "actpass", strlen("actpass")) == 0)
			{
				sdp->media_desc->a_setup_type = 2;
			}
			else if (strncmp(p + 1, "actpass", strlen("active")) == 0)
			{
				sdp->media_desc->a_setup_type = 0;
			}
			else
			{
				sdp->media_desc->a_setup_type = 1;
			}
		}
		else if (strncmp(value, "mid", strlen("mid")) == 0)
		{
			sdp->media_desc->a_mid = atoi(p + 1);
		}
		else if (strncmp(value, "extmap", strlen("extmap")) == 0)
		{
			char *q;
			q = strchr(p + 1, ' ');
			*q = '\0';
			if (sdp->media_desc->extmaps == NULL)
			{
				sdp_extmap_list *list = (sdp_extmap_list*)malloc(sizeof(sdp_extmap_list));
				memset(list, 0, sizeof(sdp_extmap_list));
				list->extmap.key = atoi(p + 1);
				ALLOC_MEMORY(strlen(q + 1) + 1);
				list->extmap.value = mem;
				strcpy(list->extmap.value, q + 1);
				list->next = NULL;
				sdp->media_desc->extmaps = list;
			}
			else
			{
				sdp_extmap_list *list = sdp->media_desc->extmaps;
				while (list->next)
				{
					list = list->next;
				}
				sdp_extmap_list *newlist = (sdp_extmap_list*)malloc(sizeof(sdp_extmap_list));
				memset(newlist, 0, sizeof(sdp_extmap_list));
				newlist->extmap.key = atoi(p + 1);
				ALLOC_MEMORY(strlen(q + 1) + 1);
				newlist->extmap.value = mem;
				strcpy(newlist->extmap.value, q + 1);
				newlist->next = NULL;
				list->next = newlist;
			}
		}
		else if (strncmp(value, "rtpmap", strlen("rtpmap")) == 0)
		{
			char *q;
			int type;
			q = strchr(p + 1, ' ');
			*q = '\0';
			type = atoi(p + 1);
			sdp_payload_list *list = sdp->media_desc->payloads;
			while (list)
			{
				if (list->payload.payloadType == type)
				{
					ALLOC_MEMORY(strlen(q + 1) + 1);
					list->payload.rtpmap = mem;
					strcpy(list->payload.rtpmap, q + 1);
					break;
				}
				list = list->next;
			}
		}
		else if (strncmp(value, "rtcp-fb", strlen("rtcp-fb")) == 0)
		{
			char *q;
			int type;
			q = strchr(p + 1, ' ');
			*q = '\0';
			type = atoi(p + 1);
			sdp_payload_list *list = sdp->media_desc->payloads;
			while (list)
			{
				if (list->payload.payloadType == type)
				{
					if (list->payload.rtcp_fb == NULL)
					{
						sdp_rtcp_fb_st *fblist = (sdp_rtcp_fb_st*)malloc(sizeof(sdp_rtcp_fb_st));
						memset(fblist, 0, sizeof(sdp_rtcp_fb_st));
						ALLOC_MEMORY(strlen(q + 1) + 1);
						fblist->name = mem;
						strcpy(fblist->name, q + 1);
						fblist->next = NULL;
						list->payload.rtcp_fb = fblist;
					}
					else
					{
						sdp_rtcp_fb_st *fblist = list->payload.rtcp_fb;
						while (fblist->next)
						{
							fblist = fblist->next;
						}
						sdp_rtcp_fb_st *newlist = (sdp_rtcp_fb_st*)malloc(sizeof(sdp_rtcp_fb_st));
						memset(newlist, 0, sizeof(sdp_rtcp_fb_st));
						ALLOC_MEMORY(strlen(q + 1) + 1);
						newlist->name = mem;
						strcpy(newlist->name, q + 1);
						newlist->next = NULL;
						fblist->next = newlist;
					}
					break;
				}
				list = list->next;
			}
		}
		else if (strncmp(value, "fmtp", strlen("fmtp")) == 0)
		{
			char *q;
			int type;
			q = strchr(p + 1, ' ');
			*q = '\0';
			type = atoi(p + 1);
			sdp_payload_list *list = sdp->media_desc->payloads;
			while (list)
			{
				if (list->payload.payloadType == type)
				{
					ALLOC_MEMORY(strlen(q + 1) + 1);
					list->payload.format_specific_param = mem;
					strcpy(list->payload.format_specific_param, q + 1);
					break;
				}
				list = list->next;
			}
		}
		else if (strncmp(value, "ssrc", strlen("ssrc")) == 0)
		{
			char *q;
			uint32_t ssrc;
			char *cname = NULL;
			char *label = NULL;
			uint8_t find = 0;
			q = strchr(p + 1, ' ');
			*q = '\0';
			ssrc = atoi(p + 1);
			if (strncmp(q + 1, "cname:", strlen("cname:")) == 0)
			{
				cname = q + 1 + strlen("cname:");
			}
			else if (strncmp(q + 1, "label:", strlen("label:")) == 0)
			{
				label = q + 1 + strlen("label:");
			}
			else
			{
				return 0;
			}
			if (sdp->media_desc->ssrcs == NULL)
			{
				sdp_ssrc_st *list = (sdp_ssrc_st*)malloc(sizeof(sdp_ssrc_st));
				memset(list, 0, sizeof(sdp_ssrc_st));
				list->ssrc = ssrc;
				if (cname)
				{
					ALLOC_MEMORY(strlen(cname) + 1);
					list->cname = mem;
					strcpy(list->cname, cname);
				}
				if (label)
				{
					ALLOC_MEMORY(strlen(label) + 1);
					list->label = mem;
					strcpy(list->label, label);
				}
				list->next = NULL;
				sdp->media_desc->ssrcs = list;
			}
			else
			{
				sdp_ssrc_st *list = sdp->media_desc->ssrcs;
				while (list->next)
				{
					if (list->ssrc == ssrc)
					{
						if (cname)
						{
							ALLOC_MEMORY(strlen(cname) + 1);
							list->cname = mem;
							strcpy(list->cname, cname);
						}
						if (label)
						{
							ALLOC_MEMORY(strlen(label) + 1);
							list->label = mem;
							strcpy(list->label, label);
						}
						find = 1;
						break;
					}
					list = list->next;
				}
				if (find == 0)
				{
					sdp_ssrc_st *newlist = (sdp_ssrc_st*)malloc(sizeof(sdp_ssrc_st));
					memset(newlist, 0, sizeof(sdp_ssrc_st));
					newlist->ssrc = ssrc;
					if (cname)
					{
						ALLOC_MEMORY(strlen(cname) + 1);
						list->cname = mem;
						strcpy(list->cname, cname);
					}
					if (label)
					{
						ALLOC_MEMORY(strlen(label) + 1);
						list->label = mem;
						strcpy(list->label, label);
					}
					newlist->next = NULL;
					list->next = newlist;
				}
			}
		}
		else if (strncmp(value, "candidate", strlen("candidate")) == 0)
		{
			int i = 0;
			char *q;
			sdp_candidate_st *candidate = (sdp_candidate_st*)malloc(sizeof(sdp_candidate_st));
			assert(candidate != NULL);
			memset(candidate, 0, sizeof(sdp_candidate_st));
			sdp->candidate = candidate;
			do
			{
				q = p;
				p = strstr(q, " ");
				if (p != NULL)
				{
					*p = '\0';
					p++;
					if (i == 4)
					{
						ALLOC_MEMORY(strlen(q) + 1);
						candidate->ip = mem;
						strcpy(candidate->ip, q);
					}
					else if (i == 5)
					{
						candidate->port = atoi(q);
					}
					else if (i == 7)
					{
						ALLOC_MEMORY(strlen(q) + 1);
						candidate->type = mem;
						strcpy(candidate->type, q);
					}
					i++;
				}
			} while (p != NULL);
		}
	}
	else
	{
		if (strncmp(value, "rtcp-mux", strlen("rtcp-mux")) == 0)
		{
			sdp->media_desc->rtcp_mux = 1;
		}
		else if (strncmp(value, "recvonly", strlen("recvonly")) == 0)
		{
			sdp->media_desc->a_dir = 1;
		}
		else if (strncmp(value, "sendonly", strlen("sendonly")) == 0)
		{
			sdp->media_desc->a_dir = 0;
		}
		else if (strncmp(value, "sendrecv", strlen("sendrecv")) == 0)
		{
			sdp->media_desc->a_dir = 2;
		}
		else if (strncmp(value, "rtcp-rsize", strlen("rtcp-rsize")) == 0)
		{
			sdp->media_desc->rtcp_rsize = 1;
		}
	}
	return 0;
}

int sdp_parse_media(sdp_desc_st *sdp, char *value)
{
	char *q = NULL;
	char* p = value;
	int i = 0;

	sdp_media_desc_st *media = (sdp_media_desc_st*)malloc(sizeof(sdp_media_desc_st));
	assert(media != NULL);
	memset(media, 0, sizeof(sdp_media_desc_st));
	sdp->media_desc = (sdp_media_desc_st*)media;
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
				if (strcmp(q, "audio") == 0)
				{
					sdp->media_desc->type = 0;
				}
				else
				{
					sdp->media_desc->type = 1;
				}
			}
			else if (i == 1)
			{
				sdp->media_desc->port = atoi(q);
			}
			else if (i == 2)
			{
				ALLOC_MEMORY(strlen(q) + 1);
				sdp->media_desc->proto = mem;
				strcpy(sdp->media_desc->proto, q);
			}
			else
			{
				if (sdp->media_desc->payloads == NULL)
				{
					sdp_payload_list *list = (sdp_payload_list*)malloc(sizeof(sdp_payload_list));
					memset(list, 0, sizeof(sdp_payload_list));
					list->payload.payloadType = atoi(q);
					list->next = NULL;
					sdp->media_desc->payloads = list;
				}
				else
				{
					sdp_payload_list *list = sdp->media_desc->payloads;
					while (list->next)
					{
						list = list->next;
					}
					sdp_payload_list *newlist = (sdp_payload_list*)malloc(sizeof(sdp_payload_list));
					memset(newlist, 0, sizeof(sdp_payload_list));
					newlist->payload.payloadType = atoi(q);
					newlist->next = NULL;
					list->next = newlist;
				}
			}
			i++;
		}
		else
		{
			if (sdp->media_desc->payloads == NULL)
			{
				sdp_payload_list *list = (sdp_payload_list*)malloc(sizeof(sdp_payload_list));
				memset(list, 0, sizeof(sdp_payload_list));
				list->payload.payloadType = atoi(q);
				list->next = NULL;
				sdp->media_desc->payloads = list;
			}
			else
			{
				sdp_payload_list *list = sdp->media_desc->payloads;
				while (list->next)
				{
					list = list->next;
				}
				sdp_payload_list *newlist = (sdp_payload_list*)malloc(sizeof(sdp_payload_list));
				memset(newlist, 0, sizeof(sdp_payload_list));
				newlist->payload.payloadType = atoi(q);
				newlist->next = NULL;
				list->next = newlist;
			}
		}
	} while (p != NULL);
	return 0;
}

int sdp_parse(char *sdp_str, int len, sdp_desc_st *sdp)
{
	char* delim = "\r\n";
	char parse_media = 0;
	char* p = strtok(sdp_str, delim);
	while (p != NULL) 
	{
		char key = p[0];
		char *value = p + 2;
		//printf("%c=%s\n", key, value);
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
		case 'a':
			if (parse_media == 1)
			{
				sdp_parse_attribute(sdp, value);
			}
			else
			{

			}
			break;
		case 'm':
			sdp_parse_media(sdp, value);
			parse_media = 1;
			break;
		default:
			break;
		}
		p = strtok(NULL, delim);
	}
	return 0;
}

void sdp_free_extmap(sdp_extmap_list *list)
{
	while (list)
	{
		FREE_MEMORY(list->extmap.value);
		sdp_extmap_list *tmp = list;
		list = list->next;
		FREE_MEMORY(tmp);
	}
}

void sdp_free_rtcpfb(sdp_rtcp_fb_st *rtcpfbs)
{
	while (rtcpfbs)
	{
		FREE_MEMORY(rtcpfbs->name);
		sdp_rtcp_fb_st *tmp = rtcpfbs;
		rtcpfbs = rtcpfbs->next;
		FREE_MEMORY(tmp);
	}
}

void sdp_free_payloads(sdp_payload_list *list)
{
	while (list)
	{
		FREE_MEMORY(list->payload.rtpmap);
		FREE_MEMORY(list->payload.format_specific_param);
		sdp_free_rtcpfb(list->payload.rtcp_fb);
		sdp_payload_list *tmp = list;
		list = list->next;
		FREE_MEMORY(tmp);
	}
}

void sdp_free_ssrc(sdp_ssrc_st *ssrcs)
{
	while (ssrcs)
	{
		FREE_MEMORY(ssrcs->cname);
		FREE_MEMORY(ssrcs->label);
		FREE_MEMORY(ssrcs->msid);
		FREE_MEMORY(ssrcs->msid_track);
		FREE_MEMORY(ssrcs->mslabel);
		sdp_ssrc_st *tmp = ssrcs;
		ssrcs = ssrcs->next;
		FREE_MEMORY(tmp);
	}
}

void sdp_free_media(sdp_media_desc_st *media)
{
	if (media)
	{
		FREE_MEMORY(media->proto);
		FREE_MEMORY(media->net_type);
		FREE_MEMORY(media->addr_type);
		FREE_MEMORY(media->addr);
		FREE_MEMORY(media->a_ice_ufrag);
		FREE_MEMORY(media->a_ice_pwd);
		FREE_MEMORY(media->finger_print.finger_print);
		sdp_free_extmap(media->extmaps);
		sdp_free_payloads(media->payloads);
		sdp_free_ssrc(media->ssrcs);
		FREE_MEMORY(media);
	}
}

void sdp_free_candidate(sdp_candidate_st *candidate)
{
	if (candidate)
	{
		FREE_MEMORY(candidate->ip);
		FREE_MEMORY(candidate->type);
		FREE_MEMORY(candidate);
	}
}

void sdp_free(sdp_desc_st *sdp)
{
	if (sdp)
	{
		FREE_MEMORY(sdp->o_name);
		FREE_MEMORY(sdp->o_net_type);
		FREE_MEMORY(sdp->o_addr_type);
		FREE_MEMORY(sdp->o_addr);
		FREE_MEMORY(sdp->s_name);
		sdp_free_media(sdp->media_desc);
		sdp_free_candidate(sdp->candidate);
	}
}

int main()
{
	char *sdp_str = "v=0\r\n\
o=- 1817118955388369990 2 IN IP4 127.0.0.1\r\n\
s=-\r\n\
t=0 0\r\n\
a=group:BUNDLE 0 1\r\n\
a=extmap-allow-mixed\r\n\
a=msid-semantic: WMS\r\n\
m=audio 9 UDP/TLS/RTP/SAVPF 111 0\r\n\
c=IN IP4 0.0.0.0\r\n\
c=IN IP4 0.0.0.0\r\n\
a=rtcp:9 IN IP4 0.0.0.0\r\n\
a=ice-ufrag:rdR+\r\n\
a=ice-pwd:iAfBsNPcWSw71zIOPeLvLRHo\r\n\
a=ice-options:trickle\r\n\
a=fingerprint:sha-256 34:21:EB:2F:49:1F:82:D4:BF:74:7A:47:41:8E:EA:E9:DE:D2:6F:C0:C1:32:72:09:98:F4:D1:A5:F1:70:3B:2C\r\n\
a=setup:actpass\r\n\
a=extmap:1 urn:ietf:params:rtp-hdrext:ssrc-audio-level\r\n\
a=extmap:2 http://www.webrtc.org/experiments/rtp-hdrext/abs-send-time\r\n\
a=recvonly\r\n\
a=rtcp-mux\r\n\
a=rtpmap:111 opus/48000/2\r\n\
a=rtcp-fb:111 transport-cc\r\n\
a=fmtp:111 minptime=10;useinbandfec=1\r\n\
a=rtpmap:0 PCMU/8000\r\n\
a=ssrc:229585655 cname:42f736008211x886\r\n\
a=ssrc:229585655 label:video-c6c84c1i\r\n\
a=candidate:0 1 udp 2130706431 8.135.38.10 8000 typ host generation 0\r\n";
	char sdp_s[1024];
	sdp_desc_st sdp;
	strcpy(sdp_s, sdp_str);
	sdp_parse(sdp_s, strlen(sdp_s), &sdp);
	memset(sdp_s, 0, 1024);
	sdp_free(&sdp);
	return 0;
}