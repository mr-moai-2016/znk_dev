#ifndef INCLUDE_GUARD__Moai_connection_h__
#define INCLUDE_GUARD__Moai_connection_h__

#include <Znk_socket.h>
#include <Znk_htp_hdrs.h>
#include <Znk_fdset.h>
#include "Moai_info.h"
#include "Moai_fdset.h"

Znk_EXTERN_C_BEGIN

/***
 * DNS��:
 * DNS�֘A��RFC�ł���RFC1035�ɂ��΁A�h���C�����͍ő�ł�255�I�N�e�b�g�ȉ��Ƃ���.
 *
 * IP:
 * IPV6���܂߂��64������Ώ\��.
 */

typedef enum {
	 MoaiSockType_e_None=0 /* ���ڑ���ԂȂ� */
	,MoaiSockType_e_Listen /* ���X�j���O�\�P�b�g */
	,MoaiSockType_e_Outer  /* �O���T�C�g�ւ̐ڑ��\�P�b�g */
	,MoaiSockType_e_Inner  /* ���[�J���z�X�g���̃u���E�U�����Accept�\�P�b�g */
} MoaiSockType;

typedef struct MoaiConnection_tag* MoaiConnection;
typedef bool (*MoaiConnectedCallback)( MoaiConnection mcn, MoaiFdSet mfds, MoaiInfoID );

struct MoaiConnection_tag {
	ZnkStr          hostname_;        /* �S�[���ƂȂ�z�X�g��(Proxy�ł͂Ȃ�) */
	uint16_t        port_;            /* �S�[���ƂȂ�z�X�g�Ƃ̒ʐM�ɗp����port(Proxy�ł͂Ȃ�) */
	ZnkSocket       I_sock_;          /* I��sock */
	bool            I_is_keep_alive_; /* I��sock��keep-alive�ł��邩�ۂ� */
	ZnkSocket       O_sock_;          /* O��sock */
	/***
	 * Pipeline�ɑΉ����邽�߂ɂ́AInner������Outer�����̒ʐM�p��
	 * content_length_remain_�����ꂼ��Ɨ����ėp�ӂ���K�v������.
	 */
	size_t          req_content_length_remain_;
	size_t          res_content_length_remain_;
	uint64_t        exile_time_;
	bool            is_connect_inprogress_;
	uint64_t        connect_begin_time_;
	MoaiConnectedCallback cb_func_;

	/***
	 * HTTP Pipeline���ɑΉ�.
	 */
	ZnkBfr          info_id_list_;
	ZnkBfr          invoked_info_id_list_;
	size_t          response_idx_;
};


void
MoaiConnection_initiate( void );

void
MoaiConnection_setListeningSock( ZnkSocket L_sock );
bool
MoaiConnection_isListeningSock( ZnkSocket query_sock );
void
MoaiConnection_setBlockingMode( bool blocking_mode );
bool
MoaiConnection_isBlockingMode( void );

MoaiConnection
MoaiConnection_regist( const char* hostname, uint16_t port,
		ZnkSocket I_sock, ZnkSocket O_sock, MoaiFdSet mfds );

MoaiConnection
MoaiConnection_connectO( const char* goal_hostname, uint16_t goal_port,
		const char* cnct_hostname, uint16_t cnct_port,
		ZnkSocket I_sock, MoaiFdSet mfds );
bool
MoaiConnection_connectFromISock( const char* hostname, uint16_t port, ZnkSocket I_sock, const char* label,
		MoaiInfoID info_id, MoaiFdSet mfds, bool is_proxy_use, MoaiConnectedCallback cb_func );

const char*
MoaiConnection_hostname( const MoaiConnection mcn );
uint16_t
MoaiConnection_port( const MoaiConnection mcn );
ZnkSocket
MoaiConnection_I_sock( const MoaiConnection mcn );
ZnkSocket
MoaiConnection_O_sock( const MoaiConnection mcn );
void
MoaiConnection_pushConnectedEvent( MoaiConnection mcn, MoaiConnectedCallback cb_func, MoaiInfoID info_id );
bool
MoaiConnection_invokeCallback( MoaiConnection mcn, MoaiFdSet mfds );

bool
MoaiConnection_isConnectInprogress( const MoaiConnection mcn );
void
MoaiConnection_setConnectInprogress( MoaiConnection mcn, bool is_connect_inprogress );

void
MoaiConnection_clear( MoaiConnection mcn, MoaiFdSet mfds );

MoaiConnection
MoaiConnection_find_byISock( const ZnkSocket query_sock );
MoaiConnection
MoaiConnection_find_byOSock( const ZnkSocket query_sock );

Znk_INLINE void
MoaiConnection_clear_byISock( ZnkSocket I_sock, MoaiFdSet mfds ){
	MoaiConnection mcn = MoaiConnection_find_byISock( I_sock );
	MoaiConnection_clear( mcn, mfds );
}
Znk_INLINE void
MoaiConnection_clear_byOSock( ZnkSocket O_sock, MoaiFdSet mfds ){
	MoaiConnection mcn = MoaiConnection_find_byOSock( O_sock );
	MoaiConnection_clear( mcn, mfds );
}

void
MoaiConnection_clearAll( MoaiFdSet mfds );

void
MoaiConnection_erase( MoaiConnection mcn, MoaiFdSet mfds );

MoaiInfo*
MoaiConnection_getInvokedInfo( const MoaiConnection mcn, size_t idx );
size_t
MoaiConnection_countResponseIdx( MoaiConnection mcn );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
