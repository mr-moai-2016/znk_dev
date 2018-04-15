#include "Znk_s_base.h"
#include "Znk_mem_find.h"
#include "Znk_stdc.h"
#include "Znk_def_util.h"
#include "Znk_s_posix.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#define DECIDE_STRLENG( str_leng, str ) Znk_setStrLen_ifNPos( &str_leng, str )

char*
ZnkS_concatEx( char* buf, size_t buf_size, const char* cstr, size_t cstr_leng )
{
	size_t dst_len;
	if( buf_size == 0 || buf == NULL ){ return NULL; }
	dst_len = ZnkS_strnlen( buf, buf_size );
	if( dst_len == buf_size ){ return NULL; }

	if( cstr_leng < buf_size - dst_len ){
		memmove( buf + dst_len, cstr, cstr_leng );
		buf += ( dst_len + cstr_leng );
		*buf = '\0';
		return buf;
	}
	/* trancate over parts */
	memmove( buf + dst_len, cstr, buf_size - 1 - dst_len );
	buf += buf_size - 1;
	*buf = '\0';
	return buf;
}
char*
ZnkS_concat( char* buf, size_t buf_size, const char* cstr )
{
	return ZnkS_concatEx( buf, buf_size, cstr, Znk_strlen( cstr ) );
}

void
ZnkS_copy( char* buf, size_t buf_size, const char* cstr, size_t cstr_leng )
{
	if( buf_size ){
		const size_t cpy_size = ( cstr_leng == Znk_NPOS ) ?
			Znk_MIN( Znk_strlen(cstr), buf_size-1 ) :
			Znk_MIN( cstr_leng, buf_size-1 ) ;
		memmove( buf, cstr, cpy_size );
		buf[ cpy_size ] = '\0';
	}
}

/***
 * va_start <=> va_end �Ԃɂ����镡����Ăяo���ɑΉ�.
 */
int
ZnkS_vsnprintf_sys__( char* buf, size_t buf_size, const char* fmt, va_list ap )
{
	int str_len;
	va_list ap_copy;

#ifdef _MSC_VER
	if( buf_size == 0 ){
		if( Znk_strchr( fmt, '%' ) ){
			return Znk_strlen( fmt ) + 512;
		}
		return Znk_strlen( fmt );
	}
#endif

	/**
	 * ap_copy���g�����Ƃɂ����, 
	 * �O������ZnkS_vsnprintf_sys�𕡐���Ăяo�����Ƃ��\�ɂ���.
	 */
	va_copy( ap_copy, ap );

	/**
	 * ��VC �� _vsnprintf �̏ꍇ:
	 * buf_size == 0 �̏ꍇ
	 *   -1 ���Ԃ�.
	 *   buf �Ɉ�ؐG�ꂸ�Ȃɂ����Ȃ�.
	 * str_len < buf_size �̏ꍇ
	 *   str_len ���Ԃ�.
	 *   buf[ str_len ] �̈ʒu��NULL��������������.
	 * str_len == buf_size �̏ꍇ
	 *   str_len ���Ԃ�(-1�ł͂Ȃ�).
	 *   ���̏ꍇNULL�I�[���s��Ȃ�.
	 * str_len > buf_size �̏ꍇ
	 *   -1���Ԃ�.
	 *   ���̏ꍇNULL�I�[���s��Ȃ�.
	 *
	 *
	 * ��GCC(or BSD) �� vsnprintf �̏ꍇ:
	 * C99�ƌ����Ɉ�v����.
	 * buf_size == 0 �̏ꍇ
	 *   str_len ���Ԃ�.
	 *   buf �Ɉ�ؐG�ꂸ�Ȃɂ����Ȃ�.
	 * str_len < buf_size �̏ꍇ
	 *   str_len ���Ԃ�.
	 *   buf[ str_len ] �̈ʒu��NULL��������������.
	 * str_len >= buf_size �̏ꍇ
	 *   str_len���Ԃ�.
	 *   buf[ buf_size-1 ] �̈ʒu��NULL��������������.
	 * �����G���[�����������ꍇ�A-1 ��Ԃ����Ƃ����蓾��.
	 *
	 */
#ifdef _MSC_VER
	str_len = _vsnprintf(buf, buf_size, fmt, ap_copy);
#else
	str_len = vsnprintf(buf, buf_size, fmt, ap_copy);
#endif

	va_end( ap_copy );

#ifdef _MSC_VER
	if( str_len < 0 || (size_t)str_len == buf_size ){
		/**
		 * VC��_vsnprintf �ɔ�����NULL�I�[������.
		 */
		buf[ buf_size-1 ] = '\0';
		str_len = Znk_strlen( fmt ) + 512;
		if( buf_size*2 > (size_t)str_len ){
			str_len = buf_size*2;
		}
	}
#endif
	return str_len;
}

int
ZnkS_snprintf_sys__( char* buf, size_t buf_size, const char* fmt, ... )
{
	int str_len;
	va_list ap;
	va_start(ap, fmt);
	str_len = ZnkS_vsnprintf_sys__( buf, buf_size, fmt, ap );
	va_end(ap);
	return str_len;
}

/***
 * vscanf�͑����̏����n�Œ񋟂���Ă�����̂́A�W���ł͂Ȃ�.
 * ������C99����͕W���ƂȂ���.
 * ����ɑ�������֐���񋟂��邩�ǂ����͌��ݖ���.
 */
bool
ZnkS_getIntD( int* ans, const char* str )
{
	return (bool)( sscanf( str, "%d", ans ) == 1 );
}
bool
ZnkS_getIntX( int* ans, const char* str )
{
	/***
	 * 16�i��
	 * printf�ł�%x�ƈقȂ�Aunsigned int* �ł͂Ȃ�int*�ł��邱�Ƃɒ��ӂ���.
	 * (K&R 308P Table B-2)
	 */
	return (bool)( sscanf( str, "%x", ans ) == 1 );
}
bool
ZnkS_getIntO( int* ans, const char* str )
{
	/***
	 * 8�i��
	 * printf�ł�%o�ƈقȂ�Aunsigned int* �ł͂Ȃ�int*�ł��邱�Ƃɒ��ӂ���.
	 * (K&R 308P Table B-2)
	 */
	return (bool)( sscanf( str, "%o", ans ) == 1 );
}

bool
ZnkS_getI16D( int16_t* ans, const char* str )
{
	return (bool)( sscanf( str, "%hd", ans ) == 1 );
}
bool
ZnkS_getI32D( int32_t* ans, const char* str )
{
	return (bool)( sscanf( str, "%d", ans ) == 1 );
}
bool
ZnkS_getI64D( int64_t* ans, const char* str )
{
#if defined(__MINGW32__)
	/***
	 * MinGW�ł�scanf�͕W�����C�u������static-link�����ꍇ��Linux�n������ ll
	 * msvcrt.dll�𓮓I�����N�����ꍇ��Windows������ I64 �̎w��ƂȂ�
	 * Znk_PFMD_64����g�������ł���. ����Ă��̏ꍇ��strtoll/strtoull�n���g��.
	 * (...�Ƃ������S���ł���ł��悢�C�����邪�Ƃ肠����)
	 */
	char* end = NULL;
	*ans = strtoll( str, &end, 10 );
	return (bool)( str != end );
#elif defined(__GNUC__)
	/* for -Wformat warning */
	Znk_LongLong lld;
	if( sscanf( str, "%" Znk_PFMD_64 "d", &lld ) == 1 ){
		*ans = lld;
		return true;
	}
	return false;
#else
	return (bool)( sscanf( str, "%" Znk_PFMD_64 "d", ans ) == 1 );
#endif
}

bool
ZnkS_getU16U( uint16_t* ans, const char* str )
{
	return (bool)( sscanf( str, "%hu", ans ) == 1 );
}
bool
ZnkS_getU32U( uint32_t* ans, const char* str )
{
	return (bool)( sscanf( str, "%u", ans ) == 1 );
}
bool
ZnkS_getU64U( uint64_t* ans, const char* str )
{
#if defined(__MINGW32__)
	/***
	 * MinGW�ł�scanf�͕W�����C�u������static-link�����ꍇ��Linux�n������ ll
	 * msvcrt.dll�𓮓I�����N�����ꍇ��Windows������ I64 �̎w��ƂȂ�
	 * Znk_PFMD_64����g�������ł���. ����Ă��̏ꍇ��strtoll/strtoull�n���g��.
	 * (...�Ƃ������S���ł���ł��悢�C�����邪�Ƃ肠����)
	 */
	char* end = NULL;
	*ans = strtoull( str, &end, 10 );
	return (bool)( str != end );
#elif defined(__GNUC__)
	/* for -Wformat warning */
	Znk_LongLong llu;
	if( sscanf( str, "%" Znk_PFMD_64 "u", &llu ) == 1 ){
		*ans = llu;
		return true;
	}
	return false;
#else
	return (bool)( sscanf( str, "%" Znk_PFMD_64 "u", ans ) == 1 );
#endif
}

bool
ZnkS_getU16X( uint16_t* ans, const char* str )
{
	return (bool)( sscanf( str, "%hx", ans ) == 1 );
}
bool
ZnkS_getU32X( uint32_t* ans, const char* str )
{
	return (bool)( sscanf( str, "%x", ans ) == 1 );
}
bool
ZnkS_getU64X( uint64_t* ans, const char* str )
{
#if defined(__MINGW32__)
	/***
	 * MinGW�ł�scanf�͕W�����C�u������static-link�����ꍇ��Linux�n������ ll
	 * msvcrt.dll�𓮓I�����N�����ꍇ��Windows������ I64 �̎w��ƂȂ�
	 * Znk_PFMD_64����g�������ł���. ����Ă��̏ꍇ��strtoll/strtoull�n���g��.
	 * (...�Ƃ������S���ł���ł��悢�C�����邪�Ƃ肠����)
	 */
	char* end = NULL;
	*ans = strtoull( str, &end, 16 );
	return (bool)( str != end );
#elif defined(__GNUC__)
	/* for -Wformat warning */
	Znk_LongLong llx;
	if( sscanf( str, "%" Znk_PFMD_64 "x", &llx ) == 1 ){
		*ans = llx;
		return true;
	}
	return false;
#else
	return (bool)( sscanf( str, "%" Znk_PFMD_64 "x", ans ) == 1 );
#endif
}
bool
ZnkS_getSzU( size_t* ans, const char* str )
{
	bool result = false;

#if   Znk_CPU_BIT == 64
	uint64_t tmp = 0;
	result = ZnkS_getU64U( &tmp, str );
#elif Znk_CPU_BIT == 32
	uint32_t tmp = 0;
	result = ZnkS_getU32U( &tmp, str );
#else
	/* not support */
	size_t tmp = 0;
#endif
	*ans = (size_t)tmp;
	return result;
}
bool
ZnkS_getSzX( size_t* ans, const char* str )
{
	bool result = false;

#if   Znk_CPU_BIT == 64
	uint64_t tmp = 0;
	result = ZnkS_getU64X( &tmp, str );
#elif Znk_CPU_BIT == 32
	uint32_t tmp = 0;
	result = ZnkS_getU32X( &tmp, str );
#else
	/* not support */
	size_t tmp = 0;
#endif
	*ans = (size_t)tmp;
	return result;
}
bool
ZnkS_getReal( double* ans, const char* str )
{
	return (bool)( sscanf( str, "%lf", ans ) == 1 );
}
bool
ZnkS_getRealF( float* ans, const char* str )
{
	return (bool)( sscanf( str, "%f", ans ) == 1 );
}
bool
ZnkS_getBool( const char* str )
{
	if( !ZnkS_empty(str) ){
		const size_t size = Znk_strlen(str);
		switch( size ){
		case 1: return (bool)( str[0] == 'T' || str[0] == '1' );
		case 4: return (bool)( strcmp( str, "true" ) == 0 );
		default: break;
		}
	}
	return false;
}
bool
ZnkS_getPtrX( void** ans, const char* str )
{
	/***
	 * %p�͏����n�ɂ���ĕ\�L���F�X�ƕς��\��������A
	 * ���ꂪ�o�͂���`���̓|�[�^�u���ȃp�b�L���O������Ƃ��Ă͎g���Ȃ����ʂ�����.
	 * �܂��Ă�sscanf��%p�ƂȂ�Ƃ��Ȃ���������̂ɂȂ��Ă���\��������A
	 * �����ł͑ւ��ɂ��m����%x��p�����������̗p����.
	 */
	bool result = false;
#if   Znk_CPU_BIT == 64
	uint64_t tmp = 0;
	result = ZnkS_getU64X( &tmp, str );
#elif Znk_CPU_BIT == 32
	uint32_t tmp = 0;
	result = ZnkS_getU32X( &tmp, str );
#else
	/* not support */
	void* tmp = NULL;
#endif
	*ans = (void*)(uintptr_t)tmp;
	return result;
}



static int
StrCompare_StdFuncAdapter( const char* s1, const char* s2, size_t n, void* arg )
{
	typedef int (*FuncT_strncmp)( const char*, const char*, size_t );
	FuncT_strncmp strncmp_func = (FuncT_strncmp) arg;
	return (*strncmp_func)( s1, s2, n );
}
void ZnkS_makeStrCompare_fromStdFunc(
		ZnkS_FuncArg_StrCompare* strcompare_funcarg,
		int (*strncmp_func)( const char*, const char*, size_t ) )
{
	strcompare_funcarg->func_ = StrCompare_StdFuncAdapter;
	strcompare_funcarg->arg_  = (void*)strncmp_func;
}
bool
ZnkS_compareBegin(
		const char* str, size_t str_leng,
		const char* ptn, size_t ptn_leng, const ZnkS_FuncArg_StrCompare* strcompare_funcarg )
{
	if( ptn_leng == Znk_NPOS ){ ptn_leng = Znk_strlen( ptn ); }
	if( str_leng != Znk_NPOS ){
		/***
		 * ���̏ꍇ�Astr_leng �̒l��NULL�I�[�܂ł̃T�C�Y�ƈ�v����Ƃ͌���Ȃ�.
		 *
		 * 1. str_leng < ptn_leng �̂Ƃ�
		 *   ���炩��false �ł���.
		 *
		 *   ���̂Ƃ� strncmp�n�̔�r�֐��ł́Astr��NULL�I�[����܂ł𒲂ׂ邪�A
		 *   str_leng < Znk_strlen(str) �����藧�ꍇ�A��r�ΏۂƂ��ė~�����Ȃ�
		 *   [ str_leng, Znk_strlen(str) ) �͈̔͂ɂ���str�̓��e����r�ΏۂƂȂ��Ă��܂�.
		 *   ���̂��߁A���̏ꍇ���܂����O���Ȃ���΂Ȃ�Ȃ�.
		 *
		 * 2. str_leng >= ptn_leng �̂Ƃ�
		 *   ���̂Ƃ� str�͍ő�ł�ptn_leng���܂ł�����r����Ȃ�.
		 *   str_leng < Znk_strlen(str) �����藧�ꍇ�ł��Aptn_leng < Znk_strlen(str)�ł��邽��
		 *   �u��r�ΏۂƂ��ė~�����Ȃ��v��Ԃ͔��������Ȃ�.
		 *   �]���āAstrncmp�n�֐��ɒ��ړn���Ă����܂�Ȃ�.
		 */
		if( str_leng < ptn_leng ){ return false; }
	}
	if( strcompare_funcarg == NULL || strcompare_funcarg->func_ == NULL ){
		return (bool)( strncmp( str, ptn, ptn_leng ) == 0 );
	}
	return (bool)( (*strcompare_funcarg->func_)( str, ptn, ptn_leng,
				strcompare_funcarg->arg_ ) == 0 );
}
bool
ZnkS_compareEnd(
		const char* str, size_t str_leng,
		const char* ptn, size_t ptn_leng, const ZnkS_FuncArg_StrCompare* strcompare_funcarg )
{
	if( str_leng == Znk_NPOS ){ str_leng = Znk_strlen( str ); }
	if( ptn_leng == Znk_NPOS ){ ptn_leng = Znk_strlen( ptn ); }
	/***
	 * ���̔���͕K�{�ł���.
	 * �ȉ��̏����ɂ����� &(str[0]) �����O�̈ʒu�������悤�Ȃ��Ƃ͔����˂΂Ȃ�Ȃ�.
	 */
	if( str_leng < ptn_leng ){ return false; }
	/***
	 * ptn_leng == 0�̂Ƃ��́A���true�Ƃ���.
	 * str_leng == 0�̂Ƃ��Aptn_leng == 0�̏ꍇ�݂̂����ɗ���\���͂��邪�A
	 * ���̏ꍇ��true�ƂȂ�A�������Ȃ�.
	 */
	if( ptn_leng == 0 ){ return true; }

	if( strcompare_funcarg == NULL || strcompare_funcarg->func_ == NULL ){
		return (bool)( strncmp( str+str_leng-ptn_leng, ptn, ptn_leng ) == 0 );
	}
	return (bool)( (*strcompare_funcarg->func_)( str+str_leng-ptn_leng, ptn, ptn_leng,
				strcompare_funcarg->arg_ ) == 0 );
}
bool
ZnkS_compareContain(
		const char* str, size_t str_leng, size_t pos,
		const char* ptn, size_t ptn_leng, const ZnkS_FuncArg_StrCompare* strcompare_funcarg )
{
	DECIDE_STRLENG( str_leng, str );
	DECIDE_STRLENG( ptn_leng, ptn );
	/***
	 * ���̔���͕K�{�ł���.
	 * �ȉ��̏����ɂ����� &(str[0]) �����O�̈ʒu�������悤�Ȃ��Ƃ͔����˂΂Ȃ�Ȃ�.
	 */
	if( str_leng < ptn_leng ){
		/* ���炩�Ƀ}�b�`���Ȃ� */
		return false;
	}
	/***
	 * ptn_leng == 0�̂Ƃ��́A���true�Ƃ���.
	 * str_leng == 0�̂Ƃ��Aptn_leng == 0�̏ꍇ�݂̂����ɗ���\���͂��邪�A
	 * ���̏ꍇ��true�ƂȂ�A�������Ȃ�.
	 */
	if( ptn_leng == 0 ){ return true; }

	if( pos == Znk_NPOS ){
		/* ���̏ꍇ�A���ʂɍŌ��ptn�ŏI����Ă��邩�ǂ����𒲂ׂ���̂Ƃ��� */
		pos = str_leng - ptn_leng;
	} else if( pos > str_leng - ptn_leng ){
		/***
		 * pos �͍Ō�� ptn_leng ��������������w���Ă���.
		 * ���̎��_�� ptn_leng != 0 �ł��邩��A�����͊m���Ƀ}�b�`���� false �ƂȂ�. */
		return false;
	} else {
		/* none */
	}

	if( strcompare_funcarg == NULL || strcompare_funcarg->func_ == NULL ){
		return (bool)( memcmp( str+pos, ptn, ptn_leng ) == 0 );
	}
	return (bool)( (*strcompare_funcarg->func_)( str+pos, ptn, ptn_leng,
				strcompare_funcarg->arg_ ) == 0 );
}

bool
ZnkS_isBegin( const char* str, const char* ptn )
{
	return (bool)( strncmp( str, ptn, Znk_strlen(ptn) ) == 0 );
}
bool
ZnkS_isEnd( const char* str, const char* ptn )
{
	ZnkS_FuncArg_StrCompare cmp_funcarg; ZnkS_makeStrCompare_fromStdFunc( &cmp_funcarg, strncmp );
	return ZnkS_compareEnd( str, Znk_NPOS, ptn, Znk_NPOS, &cmp_funcarg );
}
bool
ZnkS_isBeginEx( const char* str, const size_t str_leng, const char* ptn, size_t ptn_leng )
{
	ZnkS_FuncArg_StrCompare cmp_funcarg; ZnkS_makeStrCompare_fromStdFunc( &cmp_funcarg, strncmp );
	return ZnkS_compareBegin( str, str_leng, ptn, ptn_leng, &cmp_funcarg );
}
bool
ZnkS_isEndEx( const char* str, const size_t str_leng, const char* ptn, size_t ptn_leng )
{
	ZnkS_FuncArg_StrCompare cmp_funcarg; ZnkS_makeStrCompare_fromStdFunc( &cmp_funcarg, strncmp );
	return ZnkS_compareEnd( str, str_leng, ptn, ptn_leng, &cmp_funcarg );
}

bool
ZnkS_isCaseBegin( const char* str, const char* ptn )
{
	return (bool)( ZnkS_strncasecmp( str, ptn, Znk_strlen(ptn) ) == 0 );
}
bool
ZnkS_isCaseEnd( const char* str, const char* ptn )
{
	ZnkS_FuncArg_StrCompare cmp_funcarg; ZnkS_makeStrCompare_fromStdFunc( &cmp_funcarg, ZnkS_strncasecmp );
	return ZnkS_compareEnd( str, Znk_NPOS, ptn, Znk_NPOS, &cmp_funcarg );
}
bool
ZnkS_isCaseBeginEx( const char* str, size_t str_leng, const char* ptn, size_t ptn_leng )
{
	ZnkS_FuncArg_StrCompare cmp_funcarg; ZnkS_makeStrCompare_fromStdFunc( &cmp_funcarg, ZnkS_strncasecmp );
	return ZnkS_compareBegin( str, str_leng, ptn, ptn_leng, &cmp_funcarg );
}
bool
ZnkS_isCaseEndEx( const char* str, size_t str_leng, const char* ptn, size_t ptn_leng )
{
	ZnkS_FuncArg_StrCompare cmp_funcarg; ZnkS_makeStrCompare_fromStdFunc( &cmp_funcarg, ZnkS_strncasecmp );
	return ZnkS_compareEnd( str, str_leng, ptn, ptn_leng, &cmp_funcarg );
}


bool
ZnkS_isMatchBeginEx(
		const char* str, size_t str_leng,
		const char* ptn, size_t ptn_leng,
		const ZnkS_FuncArg_IsKeyChar*  iskeychar_funcarg,
		const ZnkS_FuncArg_StrCompare* strcompare_funcarg )
{
	if( iskeychar_funcarg != NULL && iskeychar_funcarg->func_ != NULL ){
		if( ptn_leng == Znk_NPOS ){ ptn_leng = Znk_strlen( ptn ); }

		/* ����strncmp ����������΁A���R Znk_strlen(str) >= ptn_leng �ł��� */
		if( !ZnkS_compareBegin( str, str_leng, ptn, ptn_leng, strcompare_funcarg ) ){ return false; }
		if( str[ ptn_leng ] == '\0' ){ return true; }

		return (bool)( !(*iskeychar_funcarg->func_)( str[ ptn_leng ], iskeychar_funcarg->arg_ ) );
	}
	return ZnkS_compareBegin( str, str_leng, ptn, ptn_leng, strcompare_funcarg );
}
static bool
IsKeyChar_isMatchBegin( uint32_t ch, void* arg )
{
	const char* keychars = (const char*) arg;
	return (bool)( strchr( keychars, (int)ch ) != NULL );
}
bool
ZnkS_isMatchBegin( const char* str, const char* ptn, const char* keychars )
{
	ZnkS_FuncArg_IsKeyChar  iskeychar_funcarg  = { IsKeyChar_isMatchBegin, NULL };
	iskeychar_funcarg.arg_ = (void*)keychars;
	return ZnkS_isMatchBeginEx( str, Znk_NPOS, ptn, Znk_NPOS,
			&iskeychar_funcarg,
			NULL );
}
bool
ZnkS_isMatchSWC( const char* ptn, size_t ptn_leng,
		const char* query, size_t query_leng )
{
	Znk_setStrLen_ifNPos( &ptn_leng, ptn );
	Znk_setStrLen_ifNPos( &query_leng, query );

	if( ptn_leng == 1 ){
		if( ptn[ 0 ] == '*' ){
			return true;
		}
		if( query_leng == 1 ){
			return (bool)( ptn[ 0 ] == query[ 0 ] );
		} 
		return false;
	}

	if( ptn[ 0 ] == '*' ){
		/* *ABC : ABC�ŏI�邩�ǂ��� */
		if( ZnkS_isEndEx( query, query_leng, ptn+1, ptn_leng-1 ) ){
			/* found */
			return true;
		}
	} else if( ptn[ ptn_leng-1 ] == '*' ){
		/* ABC* : ABC�Ŏn�܂邩�ǂ��� */
		if( ZnkS_isBeginEx( query, query_leng, ptn, ptn_leng-1 ) ){
			/* found */
			return true;
		}
	} else {
		const char* p = (char*)Znk_memchr( ptn, '*', ptn_leng );
		if( p ){
			/* AB*CD : AB�Ŏn�܂�CD�ŏI�邩�ǂ��� */
			const size_t prev_leng = p - ptn;
			if(  ZnkS_isBeginEx( query, query_leng, ptn, prev_leng )
			  && ZnkS_isEndEx( query+prev_leng, query_leng-prev_leng, ptn+prev_leng+1, ptn_leng-prev_leng-1 ) )
			{
				/* found */
				return true;
			}
		} else if( query_leng == ptn_leng && ZnkS_eqEx( query, ptn, query_leng ) ){
			/* found */
			return true;
		}
	}
	/* not found */
	return false;
}



uint32_t
ZnkS_get_id32( const char* s, size_t leng )
{
	union { uint32_t u32; uint8_t u[4]; } U;
	const uint8_t* us = (const uint8_t*)s;
	U.u32 = 0;
	if( leng == Znk_NPOS ){
		/***
		 * strncpy�͍Ō�ɗ]�v��zero-fill���s���̂ł��̕����ʂł��邽�߁A
		 * (���ł�0-clear����Ă���̂�)�����ł͗p���Ȃ�.
		 */
		leng = 0;
		while( *us && leng != 4 ){ U.u[ leng++ ] = *(us++); }
	} else {
		Znk_CLAMP_MAX( leng, 4 );
		Znk_memcpy( U.u, us, leng );
	}
	return U.u32;
}
uint64_t
ZnkS_get_id64( const char* s, size_t leng )
{
	union { uint64_t u64; uint8_t u[8]; } U;
	const uint8_t* us = (const uint8_t*)s;
	U.u64 = 0;
	if( leng == Znk_NPOS ){
		/***
		 * strncpy�͍Ō�ɗ]�v��zero-fill���s���̂ł��̕����ʂł��邽�߁A
		 * (���ł�0-clear����Ă���̂�)�����ł͗p���Ȃ�.
		 */
		leng = 0;
		while( *us && leng != 8 ){ U.u[ leng++ ] = *(us++); }
	} else {
		Znk_CLAMP_MAX( leng, 8 );
		Znk_memcpy( U.u, us, leng );
	}
	return U.u64;
}

const char*
ZnkS_get_extension( const char* str, char dot_ch )
{
	/***
	 * strrchr�̑�2������0���w�肷�邱�Ƃ��ł��A���̏ꍇ��strrchr�͍Ō��NULL�I�[������
	 * �ʒu��Ԃ�. �������Ȃ���A�g���q���擾�������Ƃ������̗p�r�̏ꍇ�A���̌��ʂ�
	 * �Ӗ�������Ƃ͎v���Ȃ�. ���������Ă��̏ꍇ�͖�������""��Ԃ��Ƃ����d�l�Ƃ���.
	 */
	if( dot_ch != '\0' ){
		const char* ans = strrchr( str, dot_ch );
		if( ans == NULL ){
			/***
			 * �Z�p���[�^�ƂȂ�dot_ch���܂܂�Ă��Ȃ������ꍇ�Astrrchr��NULL��Ԃ�.
			 */
			return "";
		}
		return ans+1; /* dot_ch�̂���ʒu�̎��̈ʒu��Ԃ� */
	}
	return "";
}


size_t
ZnkS_lfind_one_of( const char* str, size_t begin, size_t end, const char* chset, size_t chset_size )
{
	if( begin == Znk_NPOS ){ return Znk_NPOS; }
	DECIDE_STRLENG( end, str );
	if( end == 0 ){     return Znk_NPOS; }
	if( begin >= end ){ return Znk_NPOS; }

	DECIDE_STRLENG( chset_size, chset );
	if( chset_size == 0 ){ return begin; }

	while( true ){
		if( ZnkMem_lfind_8( (uint8_t*)chset, chset_size, (uint8_t)str[begin], 1 ) != Znk_NPOS ){
			/***
			 * str[begin] �� chset �𖞂��� begin ����������.
			 */
			break;
		}
		++begin;
		if( begin >= end ){
			/***
			 * str�̏I�[�����܂ŃX�L�����������A���ǂ��ׂ� chset ���ɂȂ�����������.
			 */
			begin = Znk_NPOS;
			break;
		}
	}
	return begin;
}

size_t
ZnkS_rfind_one_of( const char* str, size_t begin, size_t end, const char* chset, size_t chset_size )
{
	if( begin == Znk_NPOS ){ return Znk_NPOS; }
	DECIDE_STRLENG( end, str );
	if( end == 0 ){     return Znk_NPOS; }
	if( begin >= end ){ return Znk_NPOS; }
	--end;

	DECIDE_STRLENG( chset_size, chset );
	if( chset_size == 0 ){ return end; }

	while( true ){
		if( ZnkMem_lfind_8( (uint8_t*)chset, chset_size, (uint8_t)str[end], 1 ) != Znk_NPOS ){
			/***
			 * str �� �ʒupos ����t���Ɍ��āA���߂� chset �ɂ��镶�������ꂽ.
			 */
			break;
		}

		if( end == 0 ){
			/***
			 * str�̈�ԏ��߂̕����܂ŃX�L�����������A���ǂ��ׂ� chset ���ɂȂ�����������.
			 */
			end = Znk_NPOS;
			break;
		}
		--end;
	}
	return end;
}


size_t
ZnkS_lfind_one_not_of( const char* str, size_t begin, size_t end, const char* chset, size_t chset_size )
{
	if( begin == Znk_NPOS ){ return Znk_NPOS; }
	DECIDE_STRLENG( end, str );
	if( end == 0 ){     return Znk_NPOS; }
	if( begin >= end ){ return Znk_NPOS; }

	DECIDE_STRLENG( chset_size, chset );
	if( chset_size == 0 ){ return begin; }

	while( true ){
		if( ZnkMem_lfind_8( (uint8_t*)chset, chset_size, (uint8_t)str[begin], 1 ) == Znk_NPOS ){
			/***
			 * str �� �ʒubegin ���珇�Ɍ��āA���߂� chset �ɂȂ����������ꂽ.
			 */
			break;
		}
		++begin;
		if( begin >= end ){
			/***
			 * str�̏I�[�����܂ŃX�L�����������A���ǂ��ׂ� chset ���ɂ��镶��������.
			 */
			begin = Znk_NPOS;
			break;
		}
	}
	return begin;
}
size_t
ZnkS_rfind_one_not_of( const char* str, size_t begin, size_t end, const char* chset, size_t chset_size )
{
	if( begin == Znk_NPOS ){ return Znk_NPOS; }
	DECIDE_STRLENG( end, str );
	if( end == 0 ){     return Znk_NPOS; }
	if( begin >= end ){ return Znk_NPOS; }
	--end;

	DECIDE_STRLENG( chset_size, chset );
	if( chset_size == 0 ){ return end; }

	while( true ){
		if( ZnkMem_lfind_8( (uint8_t*)chset, chset_size, (uint8_t)str[end], 1 ) == Znk_NPOS ){
			/***
			 * str �� �ʒuend ����t���Ɍ��āA���߂� chset �ɂȂ����������ꂽ.
			 */
			break;
		}

		if( end == 0 ){
			/***
			 * str�̈�ԏ��߂̕����܂ŃX�L�����������A���ǂ��ׂ� chset ���ɂ��镶��������.
			 */
			end = Znk_NPOS;
			break;
		}
		--end;
	}
	return end;
}

size_t
ZnkS_find_key_and_val( const char* str, size_t begin, size_t end,
		size_t* key_begin, size_t* key_end,
		size_t* val_begin, size_t* val_end,
		const char* delimiter,  size_t delimiter_leng,
		const char* skip_chset, size_t skip_chset_leng )
{
	size_t delimiter_pos;
	DECIDE_STRLENG( delimiter_leng, delimiter );

	/***
	 * �܂� delimiter �̊J�n�ʒu�ł��� delimiter_pos ��find.
	 */
	delimiter_pos = ZnkMem_lfind_data_BF( (uint8_t*)str + begin, end - begin,
			(uint8_t*)delimiter, delimiter_leng, 1 );
	if( delimiter_pos == Znk_NPOS ){
		/***
		 * str[begin,end) ����delimiter�����݂��Ȃ��ꍇ�A
		 * val_begin, val_end �ɂ� Znk_NPOS ���Z�b�g����.
		 * �܂��Akey_begin, key_end �ɂ��Ă̂݁A�����ŒP�Ƃ�find����.
		 */
		ZnkS_find_side_skip( str, begin, end,
				key_begin, key_end, skip_chset, skip_chset_leng );
		*val_begin = Znk_NPOS; *val_end = Znk_NPOS;
		return Znk_NPOS;
	}
	delimiter_pos += begin; /* ���Έʒu�����Έʒu�֒��� */

	/***
	 * key_begin, key_end ���m��.
	 */
	ZnkS_find_side_skip( str, begin, delimiter_pos,
			key_begin, key_end, skip_chset, skip_chset_leng );

	/***
	 * val_begin, val_end ���m��.
	 */
	ZnkS_find_side_skip( str, delimiter_pos+delimiter_leng, end,
			val_begin, val_end, skip_chset, skip_chset_leng );

	return delimiter_pos;

}

size_t
ZnkS_lfind_arg( const char* str, size_t begin, size_t end,
		size_t arg_idx, size_t* arg_leng,
		const char* skip_chset, size_t skip_chset_leng )
{
	size_t arg_end = Znk_NPOS;

	while( true ){
		/***
		 * ���݂�begin����� skip_chset ����Ȃ镔����skip���Aarg�̊J�n�ʒu�����߂�.
		 */
		begin = ZnkS_lfind_one_not_of( str, begin, end, skip_chset, skip_chset_leng );
		if( begin == Znk_NPOS ){
			/***
			 * arg �͂��͂⑶�݂����A�ړI��arg_idx�Ԗڂ�arg�����݂��Ȃ�.
			 */
			*arg_leng = 0;
			return Znk_NPOS;
		}
		/***
		 * skip_chset ���n�܂�ʒu(arg�̏I���ʒu)�����߂�.
		 */
		arg_end = ZnkS_lfind_one_of( str, begin, end, skip_chset, skip_chset_leng );
		if( arg_end == Znk_NPOS ){
			/***
			 * arg �� end �܂ő���������ł������ꍇ.
			 */
			if( arg_idx == 0 ){
				/***
				 * OK. �Y����arg���擾����.
				 */
				*arg_leng = end - begin;
				return begin;
			} else {
				/***
				 * arg �͂��͂⑶�݂����A�ړI��arg_idx�Ԗڂ�arg�����݂��Ȃ�.
				 */
				*arg_leng = 0;
				return Znk_NPOS;
			}
		}

		if( arg_idx == 0 ){
			/***
			 * OK. �Y����arg���擾����.
			 */
			*arg_leng = arg_end - begin;
			return begin;
		} else {
			/***
			 * ���� arg �擾�Ɉڂ�.
			 */
			--arg_idx;
			begin = arg_end;
		}
	}
}
size_t
ZnkS_rfind_arg( const char* str, size_t begin, size_t end,
		size_t arg_idx, size_t* arg_leng,
		const char* skip_chset, size_t skip_chset_leng )
{
	size_t arg_begin = Znk_NPOS;

	while( true ){
		/***
		 * ���݂�end����� skip_chset ����Ȃ镔����skip���Aarg�̏I���ʒu�����߂�.
		 */
		end = ZnkS_rfind_one_not_of( str, begin, end, skip_chset, skip_chset_leng );
		if( end == Znk_NPOS ){
			/***
			 * arg �͂��͂⑶�݂����A�ړI��arg_idx�Ԗڂ�arg�����݂��Ȃ�.
			 */
			*arg_leng = 0;
			return Znk_NPOS;
		}
		/***
		 * end �� ���� arg �̈�ԍŌ�̕������̂��̂̈ʒu���w���Ă���.
		 * ���� arg �� arg_leng(������)�� 1 ���傫���ꍇ�͂�������̏�����
		 * �o���l�Ƃ��Ă��悳�����Ɍ����邪�Aarg_leng �擾�̌v�Z���� + 1 ���Ȃ����
		 * �Ȃ�Ȃ��Ƃ������ӎ�������������. �܂� arg_leng �� 1 �ɓ������ꍇ�A
		 * begin �� end ���������Ȃ�\����������. ZnkS_rfind_one_not_of �ł�
		 * begin �� end �̒l���������ꍇ��Znk_NPOS��Ԃ����A����͂����Ŋ��҂���
		 * ���ʂł͂Ȃ�. �����ł͂��̂悤�ȏꍇ���l�����āAend ���m���� begin�����傫��
		 * �{���̈Ӗ��ł�end�l�ɂ��Ă����K�v������.
		 */
		++end;

		/***
		 * skip_chset ���n�܂�ʒu(arg�̊J�n�ʒu�̈�O�̈ʒu)�����߂�.
		 */
		arg_begin = ZnkS_rfind_one_of( str, begin, end, skip_chset, skip_chset_leng );
		if( arg_begin == Znk_NPOS ){
			/***
			 * arg �� begin �܂ő���������ł������ꍇ.
			 */
			if( arg_idx == 0 ){
				/***
				 * OK. �Y����arg���擾����.
				 */
				*arg_leng = end - begin;
				return begin;
			} else {
				/***
				 * arg �͂��͂⑶�݂����A�ړI��arg_idx�Ԗڂ�arg�����݂��Ȃ�.
				 */
				*arg_leng = 0;
				return Znk_NPOS;
			}
		}
		/***
		 * arg_begin �� Znk_NPOS �ȊO�ł������ꍇ�A���̎��_�� arg �̈�ԍŏ��̕�����
		 * ��O�̈ʒu���w���Ă���. ���̂��߁A������ 1 �C���N�������g���˂΂Ȃ�Ȃ�.
		 */
		++arg_begin;

		if( arg_idx == 0 ){
			/***
			 * OK. �Y����arg���擾����.
			 */
			*arg_leng = end - arg_begin;
			return arg_begin;
		} else {
			/***
			 * ���� arg �擾�Ɉڂ�.
			 */
			--arg_idx;
			end = arg_begin;
		}
	}
}


