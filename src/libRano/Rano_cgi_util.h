#ifndef INCLUDE_GUARD__Rano_cgi_util_h__
#define INCLUDE_GUARD__Rano_cgi_util_h__

#include <Rano_type.h>
#include <Rano_module.h>
#include <Znk_bfr.h>
#include <Znk_str.h>
#include <Znk_varp_ary.h>
#include <Znk_htp_hdrs.h>
#include <Znk_bird.h>

Znk_EXTERN_C_BEGIN

typedef struct RanoCGIEVar_tag {
	char* server_name_;
	char* server_port_;
	char* content_type_;
	char* content_length_;
	char* remote_addr_;
	char* remote_host_;
	char* remote_port_;
	char* request_method_;
	char* query_string_;
	char* http_cookie_;
	char* http_user_agent_;
	char* http_accept_;
} RanoCGIEVar;

typedef struct {
	bool (*func_)( ZnkVarpAry post_vars, void* arg, const char* content_type, bool* is_updated );
	void*  arg_;
} RanoCGIUtilFnca_procPostVars;

typedef int (*RanoCGIUtilFuncT_procStr)( ZnkStr str, void* arg );

void
RanoCGIUtil_addHdrVarsStr( ZnkStr str, const ZnkVarpAry vars );
void
RanoCGIUtil_addPostVarsStr( ZnkStr str, const ZnkVarpAry hpvs );

void
RanoCGIUtil_getStdInStr( ZnkBfr stdin_bfr, size_t content_length );

RanoCGIEVar*
RanoCGIEVar_create( void );

void
RanoCGIEVar_destroy( RanoCGIEVar* evar );

/**
 * @brief
 * URL�ɂ����� Query String ���w�肳��Ă���ꍇ�͂�����擾����.
 * �w�肳��Ă��Ȃ��ꍇ�A���̊֐��͋󕶎���Ԃ�.
 */
Znk_INLINE const char*
RanoCGIUtil_getQueryString( RanoCGIEVar* evar )
{
	/* Query String ���w�肳��Ă��Ȃ��ꍇ�Aevar->query_string_ �� NULL�ƂȂ��Ă���. */
	return evar->query_string_ ? evar->query_string_ : "";
}

void
RanoCGIUtil_splitQueryString( ZnkVarpAry post_vars, const char* query_string, const size_t query_string_leng, bool is_unescape_val );


bool
RanoCGIUtil_getPostVars_fromInBfr( ZnkVarpAry post_vars, RanoModule mod, ZnkHtpHdrs htp_hdrs,
		ZnkBfr in_bfr, size_t content_length,
		const char* content_type, const char* src_boundary, const char* dst_boundary, 
		ZnkStr pst_str, const char* varname_of_urlenc_body, bool is_unescape_val,
		RanoCGIUtilFnca_procPostVars* fnca_proc_post_vars );

/**
 * @brief
 *   POST���ɂ����� multipart/form-data ����� application/x-www-form-urlencoded �`������͂��邱�Ƃɂ��
 *   post_vars���擾����.
 *
 * @param evar:
 *   ����CGI�ɂ�������ϐ����i�[���ꂽRanoCGIEVar�^���w�肷��.
 *   ���̎w��͕K�{�ł���.
 *
 * @param post_vars:
 *   ���ʂ��i�[����Post�ϐ��p�̔z��.
 *   ���̎w��͕K�{�ł���.
 *
 * @param mod:
 *   �t�B���^�p�̃��W���[��.
 *   RanoModule�ɂ��t�B���^�����O���s�������ꍇ�Ɏw�肷��.
 *   ���ꂪ�s�v�Ȃ�NULL���w�肷��.
 *
 * @param htp_hdrs:
 *   �t�B���^�ΏۂƂȂ�HTTP�w�b�_.
 *   RanoModule�ɂ��t�B���^�����O���s�������ꍇ�Ɏw�肷��.
 *   ���ꂪ�s�v�Ȃ�NULL���w�肷��.
 *
 * @param pst_str:
 *   Post�ϐ��z��̓��e�����|�[�g���镶������擾�������ꍇ�Ɏw�肷��.
 *   ���ꂪ�s�v�Ȃ�NULL���w�肷��.
 *
 * @param varname_of_urlenc_body:
 *   ����� application/x-www-form-urlencoded �`���̏ꍇ�݈̂Ӗ�������.
 *   ������w�肷��Ƃ��̂悤�ȕϐ���������Post�ϐ��̈�Ƃ��Ēǉ����邱�Ƃ��ł��A
 *   ���̒l�̓��e��HTTP�w�b�_�ȍ~�ɑ��� application/x-www-form-urlencoded �`���̕����񂻂̂��̂ƂȂ�.
 *   ���ꂪ�s�v�Ȃ�NULL���w�肷��.
 */
bool
RanoCGIUtil_getPostedFormData( RanoCGIEVar* evar, ZnkVarpAry post_vars, RanoModule mod, ZnkHtpHdrs htp_hdrs,
		ZnkStr pst_str, const char* varname_of_urlenc_body, bool is_unescape_val );

void
RanoCGIMsg_initiate( bool with_html_start, const char* base_href );

void
RanoCGIMsg_finalize( void );

void
RanoCGIMsg_begin( void );

void
RanoCGIMsg_end( void );

bool
RanoCGIUtil_displayBinary( const char* filename, const char* content_type );

bool
RanoCGIUtil_displayText( const char* filename, const char* content_type, RanoCGIUtilFuncT_procStr proc_str, void* proc_str_arg );

void
RanoCGIUtil_replaceNLtoHtmlBR( ZnkStr str );

bool
RanoCGIUtil_printTemplateHTML( RanoCGIEVar* evar, ZnkBird bird, const char* template_html_file );

const char*
RanoCGIUtil_skipProtocolPrefix( const char* src );

RanoTextType
RanoCGIUtil_getTextType( const char* content_type );

size_t
RanoCGIUtil_initLog( const char* log_filebasename, const char* count_filename,
		size_t pitch_of_count, size_t pitch_of_logfiles, bool keep_open );
size_t
RanoCGIUtil_initMultiDirLog( const char* logdir_basename, const char* count_filename,
		size_t pitch_of_count, size_t pitch_of_logfiles, bool keep_open );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
