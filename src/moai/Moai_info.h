#ifndef INCLUDE_GUARD__Moai_info_h__
#define INCLUDE_GUARD__Moai_info_h__

#include <Rano_type.h>
#include <Rano_module_ary.h>

#include <Znk_str.h>
#include <Znk_str_ary.h>
#include <Znk_var.h>
#include <Znk_varp_ary.h>
#include <Znk_htp_hdrs.h>

#include <assert.h>

Znk_EXTERN_C_BEGIN

typedef uint64_t MoaiInfoID;

typedef struct {
	char buf_[ 16+1 ];
} MoaiInfoIDStr;


typedef struct {
	MoaiInfoID            id_;
	struct ZnkHtpHdrs_tag hdrs_;
	ZnkVarpAry            vars_;   /* for POST */
	ZnkBfr                stream_; /* ����M�̍ۂɎ��ۂɎg����X�g���[�� */
	size_t                hdr_size_;
	ZnkHtpReqMethod       req_method_;
	ZnkStr                req_urp_;
	int                   response_state_;
	ZnkStr                proxy_hostname_;
	uint16_t              proxy_port_;
} MoaiInfo;

void
MoaiInfo_initiate( void );

MoaiInfoID
MoaiInfo_regist( const MoaiInfo* info );

MoaiInfo*
MoaiInfo_find( MoaiInfoID query_id );

void
MoaiInfo_erase( MoaiInfoID query_id );
void
MoaiInfo_clear( MoaiInfo* info );

void
MoaiInfoID_getStr( MoaiInfoID id, MoaiInfoIDStr* id_str );
MoaiInfoID
MoaiInfoID_scanIDStr( MoaiInfoIDStr* id_str );


typedef struct {
	bool    is_chunked_;
	bool    is_gzip_;
	bool    is_unlimited_;
	size_t  content_length_;
	RanoTextType txt_type_;
} MoaiBodyInfo;

Znk_INLINE void
MoaiBodyInfo_clear( MoaiBodyInfo* body_info )
{
	MoaiBodyInfo zero = { 0 };
	*body_info = zero;
}

/**
 * @param is_proxy:
 *   ���̃v���O�����̒��p�悪����Ƃ͕ʂ�proxy�T�[�o�ł��邩�ۂ����w��.
 *   true�ł���ꍇ�A���̃v���O�����͒��p������ɕʂ�proxy�Ɉ����p��.
 *   false�ł���ꍇ�A���̃v���O�����͍ŏI�I�Ȓ��p��Ƀ_�C���N�g�ɒ��p����.
 *
 * @param as_local_proxy:
 *   ���̊֐��́A���̃v���O������LocalProxy�Ƃ��Ďg�p����Ă��邩�ۂ���
 *   �������肷��. ���̔��茋�ʂ��i�[�����.
 *
 * @param my_port:
 *   ���̃v���O�������g���Ă���port�ԍ����w�肷��.
 */
void
MoaiInfo_parseHdr( MoaiInfo* info, MoaiBodyInfo* body_info,
		bool* as_local_proxy, uint16_t my_port, bool is_request,
		const char* response_hostname,
		RanoModuleAry mod_ary, const char* server_name );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
