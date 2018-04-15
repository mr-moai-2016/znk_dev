#ifndef INCLUDE_GUARD__Znk_missing_libc_h__
#define INCLUDE_GUARD__Znk_missing_libc_h__

#include <Znk_base.h>
#include <Znk_stdc.h>
#include <stdarg.h>

Znk_EXTERN_C_BEGIN

/***
 * memrchr�Ƃ����̂�Posix�ɂ͂Ȃ�GNU�̊g�������A
 * memchr�̑Δ�łقڒN�ł����R�Ɏv�����W���I�֐��Ȃ̂ł����ɐ錾����.
 */
void*
Znk_memrchr( const void* buf, int val, size_t size );

int
Znk_snprintf( char* dest, size_t count, const char* format, ... );
int
Znk_vsnprintf( char* dest, size_t count, const char* format, va_list argptr );

#if defined(_MSC_VER)
#  define snprintf           Znk_snprintf
#  define vsnprintf          Znk_vsnprintf
#endif

/***
 * memmem�Ƃ����̂�Posix�ɂ͂Ȃ�GNU�̊g�������A
 * strstr�̑Δ�łقڒN�ł����R�Ɏv�����W���I�֐��Ȃ̂ł����ɐ錾����.
 */
void*
Znk_memmem( const void* mem, size_t mem_leng, const void* ptn, size_t ptn_leng );

#define Znk_memstr( m, m_size, cstr ) \
	Znk_memmem( (m), (m_size), cstr, Znk_strlen(cstr) )

#define Znk_memstr_literal( m, m_size, literal_str ) \
	Znk_memmem( (m), (m_size), literal_str, Znk_strlen_literal(literal_str) )


Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
