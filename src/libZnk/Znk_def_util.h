#ifndef INCLUDE_GUARD__Znk_def_util_h__
#define INCLUDE_GUARD__Znk_def_util_h__

#include <Znk_base.h>
#include <limits.h>

#define Znk_CLAMP_MIN( val, min ) ( ((val)<(min)) ? (val)=(min) : (val) )
#define Znk_CLAMP_MAX( val, max ) ( ((val)>(max)) ? (val)=(max) : (val) )
#define Znk_CLAMP_MINMAX( val, min, max ) \
	( ((val)>(max)) ? (val)=(max) : Znk_CLAMP_MIN((val),(min)) )
#define Znk_MIN(x,y) ( (x)<(y) ? (x) : (y) )
#define Znk_MAX(x,y) ( (x)>(y) ? (x) : (y) )

/*****************************************************************************/
/***
 * Bit Flag
 */

/**
 * @brief
 * ���݂�@a flag�ɂ����āA�^����ꂽ@a msk���L���ɂȂ��Ă��邩
 * (bit�������Ă��邩)�ǂ����𒲂ׂ�.
 */
#define Znk_IS_FLAG8( flag, msk)  (bool)( ((flag) & (uint8_t )(msk)) != 0 )
#define Znk_IS_FLAG16(flag, msk)  (bool)( ((flag) & (uint16_t)(msk)) != 0 )
#define Znk_IS_FLAG32(flag, msk)  (bool)( ((flag) & (uint32_t)(msk)) != 0 )
#define Znk_IS_FLAG64(flag, msk)  (bool)( ((flag) & (uint64_t)(msk)) != 0 )

#define Znk_SET_FLAG8( flag, msk, v) (v) ? ( (flag) |= (uint8_t) (msk) ) : ( (flag) &= ~((uint8_t) (msk)) )
#define Znk_SET_FLAG16(flag, msk, v) (v) ? ( (flag) |= (uint16_t)(msk) ) : ( (flag) &= ~((uint16_t)(msk)) )
#define Znk_SET_FLAG32(flag, msk, v) (v) ? ( (flag) |= (uint32_t)(msk) ) : ( (flag) &= ~((uint32_t)(msk)) )
#define Znk_SET_FLAG64(flag, msk, v) (v) ? ( (flag) |= (uint64_t)(msk) ) : ( (flag) &= ~((uint64_t)(msk)) )


/***
 * Znk_BIT_<type> �V���[�Y
 * sizeof(<type>)���g�������Ȃ��ꍇ��g���Ȃ��ꍇ�ȂǂɎg�p����.
 */

/* Znk_BIT_int */
#if   INT8_MAX  == INT_MAX /* sizeof(int8_t)  == sizeof(int) */
#  define Znk_BIT_int 8
#elif INT16_MAX == INT_MAX /* sizeof(int16_t) == sizeof(int) */
#  define Znk_BIT_int 16
#elif INT32_MAX == INT_MAX /* sizeof(int32_t) == sizeof(int) */
#  define Znk_BIT_int 32
#elif INT64_MAX == INT_MAX /* sizeof(int64_t) == sizeof(int) */
#  define Znk_BIT_int 64
#else
/* unknown : undefined */
#endif

/* Znk_BIT_size_t */
#if   UINT8_MAX  == SIZE_MAX /* sizeof(uint8_t)  == sizeof(size_t) */
#  define Znk_BIT_size_t 8 
#elif UINT16_MAX == SIZE_MAX /* sizeof(uint16_t) == sizeof(size_t) */
#  define Znk_BIT_size_t 16
#elif UINT32_MAX == SIZE_MAX /* sizeof(uint32_t) == sizeof(size_t) */
#  define Znk_BIT_size_t 32
#elif UINT64_MAX == SIZE_MAX /* sizeof(uint64_t) == sizeof(size_t) */
#  define Znk_BIT_size_t 64
#else
/* unknown : undefined */
#endif

/* Znk_BIT_long */
#if   INT8_MAX  == LONG_MAX /* sizeof(int8_t)  == sizeof(long) */
#  define Znk_BIT_long 8 
#elif INT16_MAX == LONG_MAX /* sizeof(int16_t) == sizeof(long) */
#  define Znk_BIT_long 16
#elif INT32_MAX == LONG_MAX /* sizeof(int32_t) == sizeof(long) */
#  define Znk_BIT_long 32
#elif INT64_MAX == LONG_MAX /* sizeof(int64_t) == sizeof(long) */
#  define Znk_BIT_long 64
#else
/* unknown : undefined */
#endif

/* Znk_BIT_Znk_LongLong */
#if !defined( Znk_BIT_Znk_LongLong )
#  if defined(LLONG_MAX)
/*   long long�����݂�����ɂ����ẮAlimits.h�ɂ����� LLONG_MAX����`����Ă���͂��ł���A
 *   �ȉ��ŋ��߂邱�Ƃ��ł���. */
#    if   INT8_MAX  == LLONG_MAX /* sizeof(int8_t)  == sizeof(Znk_LongLong) */
#      define Znk_BIT_Znk_LongLong 8 
#    elif INT16_MAX == LLONG_MAX /* sizeof(int16_t) == sizeof(Znk_LongLong) */
#      define Znk_BIT_Znk_LongLong 16
#    elif INT32_MAX == LLONG_MAX /* sizeof(int32_t) == sizeof(Znk_LongLong) */
#      define Znk_BIT_Znk_LongLong 32
#    elif INT64_MAX <= LLONG_MAX /* sizeof(int64_t) == sizeof(Znk_LongLong) */
#      define Znk_BIT_Znk_LongLong 64
#    else
/*     unknown : undefined */
#    endif
#  else
/*   long long �����݂��Ȃ����ɂ����ẮAZnk_LongLong�̒�`���ɂ����� 64bit�����Ƃ��Ē�`���Ă���͂��ł���. */
#    define Znk_BIT_Znk_LongLong 64
#  endif
#endif

/*
 * endof Bit Flag
 ***/
/*****************************************************************************/

#endif /* INCLUDE_GUARD */
