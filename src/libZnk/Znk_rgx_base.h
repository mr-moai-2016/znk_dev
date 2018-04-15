#ifndef INCLUDE_GUARD__Znk_rgx_base_h__
#define INCLUDE_GUARD__Znk_rgx_base_h__

#include <Znk_base.h>

Znk_EXTERN_C_BEGIN

typedef union ZnkUChar64_tag {
	uint64_t u64_; /* �������q��64bit�����ł̎w�肪�ł���悤�ɂ�����P�Ԗڂ̃����o�[�ɂ��� */
	char     s_[ 8 ];
} ZnkUChar64;

#define ZnkRgx_DOT   UINT64_C(0xffffffffffffffff)
/* ZnkRgx_EMPTY�̓ÑJ�ڂ�\�� */
#define ZnkRgx_EMPTY UINT64_C(0xfffffffffffffffe)

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */

