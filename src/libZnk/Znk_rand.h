#ifndef INCLUDE_GUARD__Znk_rand_h__
#define INCLUDE_GUARD__Znk_rand_h__

/* Generating rand by Mersenne twister method.  */

#include <Znk_base.h>

Znk_EXTERN_C_BEGIN

/**
 * @brief
 *  ������seed��ݒ肵�V���b�t���������s��.
 *  seed_delta�� 0 �ȊO���w�肵���ꍇ�͓����Ő������ꂽ���Ԃɂ��seed�l��
 *  ����Ƀo�C�A�X�l���Z���邱�Ƃ��ł���.
 */
void ZnkRand_shuffle( unsigned long seed_delta );

/**
 * @brief
 *  [ 0, num ) �܂ł̃����_��unsigned long�𓾂�.
 */
unsigned long ZnkRand_getRandUL( unsigned long num );

/**
 * @brief
 *  [ min_real, max_real ) �܂ł̃����_��double�l�𓾂�.
 *  ������ min_real > max_real �̏ꍇ ( max_real, min_real ] �͈̔͂ƂȂ�.
 */
double ZnkRand_getRandR( double min_real, double max_real );

/**
 * @brief
 *  [ min_ival, max_ival ) �܂ł̃����_��int�l�𓾂�.
 *  ������ min_ival > max_ival �̏ꍇ ( max_ival, min_ival ] �͈̔͂ƂȂ�.
 *
 * @note
 *  �����ł͎����Ƃ��ĒP��ZnkRand_getRandR���Ăяo���Ă���.
 *  ����� int��p�̎����ɂ����Ƃ��Ă��A�������č������ł��Ȃ�.
 *  �����Ō��Z�������������邽�߁A�Ⴆ�� INT_MIN, INT_MAX �Ȃǂ��w�肵���ꍇ�ł����Ȃ��������邽�߂ɂ�
 *  ���ǂ����ƍ������x(int64_t��double)�֕ϊ����˂΂Ȃ�Ȃ����߂ł���.
 */
Znk_INLINE int ZnkRand_getRandI( int min_ival, int max_ival ){
	return (int)ZnkRand_getRandR( min_ival, max_ival );
}

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
