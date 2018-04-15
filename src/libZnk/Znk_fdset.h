#ifndef Znk_fdset_h__INCLUDED__
#define Znk_fdset_h__INCLUDED__

#include <Znk_socket.h>
#include <Znk_socket_ary.h>

Znk_EXTERN_C_BEGIN

struct ZnkTimeval {
	long tv_sec;
	long tv_usec;
};

typedef struct ZnkFdSetImpl* ZnkFdSet;

ZnkFdSet
ZnkFdSet_create( void );
void
ZnkFdSet_destroy( ZnkFdSet fdst );
/**
 * @brief
 *  sock��ZnkFdSet��set����.
 *  �ő�ێ�����FD_SETSIZE�𒴂����ꍇ�ȂǁAset�Ɏ��s����ꍇ������̂ŁA
 *  ���ɓr���V�K�ɐ������� accept�ɂ�� sock �� connect�ɂ�� sock��ǉ�����ꍇ��
 *  �����ꂪ��ʂɔ���������p�r�ł́A�K�����̖߂�l���m�F���A�K�؂ȏ������u���邱��.
 */
bool
ZnkFdSet_set( ZnkFdSet fdst, const ZnkSocket sock );
void
ZnkFdSet_zero( ZnkFdSet fdst );
void
ZnkFdSet_clr( ZnkFdSet fdst, const ZnkSocket sock );
bool
ZnkFdSet_isset( const ZnkFdSet fdst, const ZnkSocket sock );
void
ZnkFdSet_print_e( const ZnkFdSet fdst );
void
ZnkFdSet_copy( ZnkFdSet dst, const ZnkFdSet src );
void
ZnkFdSet_getSocketAry( ZnkFdSet fdst, ZnkSocketAry sock_ary );
ZnkSocket
ZnkFdSet_getMaxOfSocket( const ZnkFdSet fdst );
size_t
ZnkFdSet_FD_SETSIZE( void );

int
ZnkFdSet_select( ZnkSocket sock,
		ZnkFdSet fdst_read, ZnkFdSet fdst_write, ZnkFdSet fdst_except,
		const struct ZnkTimeval* timeout );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
