#ifndef _SDP_H_
#define _SDP_H_

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long long int uint64_t;

typedef signed char int8_t;
typedef signed short int16_t;
typedef signed int int32_t;
typedef signed long long int int64_t;

typedef struct
{
	char *type;
	char *finger_print;
}sdp_finger_print_st;

typedef struct
{
	int key;
	char *value;
}sdp_extmap_st;

typedef struct
{
	char *name;
}sdp_rtcp_fb_st;

typedef struct
{
	int payloadType;
	char *name;
	int rate;
	int extParam;
	sdp_rtcp_fb_st *rtcp_fb;
	char *format_specific_param;
}sdp_payload_st;

struct payloadlist
{
	sdp_payload_st payload;
	struct payloadlist *next;
};

typedef struct payloadlist sdp_payload_list;

typedef struct
{
	int type; // 0 audio  1 video;
	int port;
	char *proto;
	char *net_type;
	char *addr_type;
	char *addr;
	char *a_ice_ufrag;
	char *a_ice_pwd;
	sdp_finger_print_st finger_print;
	uint8_t a_setup_type;  // 0 active 1 passive  2 actpass
	uint8_t a_mid;
	sdp_extmap_st *extmap;
	uint8_t a_dir;  //0 sendonly 1 recvonly  2 sendrecv
	uint8_t rtcp_mux;
	uint8_t rtcp_rsize;
	sdp_payload_list *payloads;
}sdp_media_desc_st;

typedef struct
{
	uint32_t ssrc;
	char *cname;
	char *label;
	char *msid;
	char *msid_track;
	char *mslabel;
}sdp_ssrc_st;

typedef struct
{
	char *ip;
	int port;
	char *type;
}sdp_candidate_st;

typedef struct
{
	int version;
	char *o_name;
	uint64_t o_session_id;
	uint64_t o_session_version;
	char *o_net_type;
	char *o_addr_type;
	char *o_addr;
	char *s_name;
	uint64_t t_start;
	uint64_t t_end;
	sdp_media_desc_st *media_desc;
	sdp_ssrc_st *ssrc;
	sdp_candidate_st *candidate;
}sdp_desc_st;


#endif
