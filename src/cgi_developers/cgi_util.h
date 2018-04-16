#ifndef INCLUDE_GUARD__cgi_util_h__
#define INCLUDE_GUARD__cgi_util_h__

#include <stddef.h>

/***
 * �W�����o�͂Ɋւ���o�C�i�����[�h�̕ύX.
 */
void
CGIUtil_Internal_setMode( int no, int is_binary_mode );


/***
 * msec�~���bsleep���܂�.
 */
void
CGIUtil_sleep( size_t msec );


/***
 * @brief
 * �W�����͂���̃f�[�^��stdin_bfr�ւƊi�[����.
 * stdin_bfr�ɂ͍Œ�ł� content_length �o�C�g�ȏ�̃��������m�ۂ���Ă��Ȃ���΂Ȃ�Ȃ�.
 *
 * @return
 * ���ۂɎ擾���ꂽ�o�C�g����Ԃ�.
 * ���Ȃ킿���ꂪcontent_length�Ɠ����ꍇ�͂��ׂĎ擾�ł��Ă���A
 * �����łȂ���΁A�\������EOF�����������\��������.
 */
size_t
CGIUtil_getStdInStr( char* stdin_bfr, size_t content_length );


/***
 * CGI�ň������ϐ�.
 */
typedef struct CGIEVar_tag {
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
} CGIEVar;

CGIEVar*
CGIEVar_create( void );

void
CGIEVar_destroy( CGIEVar* evar );


/***
 * @brief
 * CGI�ň���Query String.
 *
 * Query String ���w�肳��Ă���ꍇ�͂�����擾����.
 * �w�肳��Ă��Ȃ��ꍇ�A���̊֐��͋󕶎���Ԃ�.
 * Query String ���w�肳��Ă��Ȃ��ꍇ�Aevar->query_string_ �� NULL�ƂȂ��Ă���.
 *
 * @return
 * �����Ȃ��1�A���s�Ȃ��0��Ԃ�.
 */
#define CGIUtil_getQueryString( evar ) ( (evar)->query_string_ ? (evar)->query_string_ : "" )

int
CGIUtil_getQueryStringToken( const char* query_string, size_t query_idx,
		char* key, size_t key_size,
		char* val, size_t val_size );


#endif /* INCLUDE_GUARD */
