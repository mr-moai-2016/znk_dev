#include "Znk_tostr_double.h"
#include <Znk_def_util.h>
#include <string.h>


#if defined(__BORLANDC__)
#  pragma warn -8071
#endif

enum {
	 Znk_TOSTR_ENOUGH_BUFSIZE_FOR_DOUBLE_F = 512
	,Znk_TOSTR_ENOUGH_BUFSIZE_FOR_DOUBLE_G = 512
	,Znk_TOSTR_ENOUGH_BUFSIZE_FOR_DOUBLE_E = 32
	,Znk_TOSTR_ENOUGH_BUFSIZE_FOR_DOUBLE_X = 32
	,Znk_TOSTR_ENOUGH_BUFSIZE_FOR_DOUBLE   = 512
};


/***
 * Private global function : Prototype declare.
 */
char*
ZnkToStr_Private_UInt32ToStr( char* p, uint32_t uval, bool term_nul,
		size_t width, char space_ch, size_t radix, ZnkToStrFlags flags );


/*****************************************************************************/
/***
 * DoubleInfoBase
 */

/**
 * NanCallBackFunc 
 *   dval �� NaN, Inf, -Inf �ł������ꍇ�ɂǂ̂悤�ȕ������ݒ肷�邩���L�q����
 *   �R�[���o�b�N�֐��̌`�����ȉ��̂悤�ɒ�`����.
 *
 *   typedef char* (*NanCallBackFunc)( int, char*, bool );
 *
 *   ��1���� int type 
 *     dval ����L�R��̂����̂�����ł��邩�𔻕ʂ��邽�߂� type �l��
 *     �ݒ肳��Ă���. type �̒l�����̂Ƃ� dval �� Inf�Atype �̒l�����̂Ƃ�
 *     dval �� -Inf�Atype �̒l�� 0 �̂Ƃ� dval �� Nan �ł��邱�Ƃ�����.
 *
 *   ��2���� char* p
 *     ��������i�[����o�b�t�@�ւ̃|�C���^.
 *     ���̊֐��ŗ^����ꂽ���̂Ɠ�����.
 *
 *   ��3���� char* p
 *     �������'\0'�I�[���邷�邩�ۂ�������.
 *     ���̊֐��ŗ^����ꂽ���̂Ɠ�����.
 *
 * NanCallBackFunc �^�̈������Ƃ�֐��ɂ́A�ȉ��̂Q�̈������Ƃ���̂�����.
 *
 * nan_callback_func
 *   NanCallBackFunc �^�R�[���o�b�N�֐��ւ̃|�C���^��^����.
 *   nan_callback_func �� NULL ��^�����ꍇ�A
 *   �f�t�H���g�̐ݒ�֐�(setDefaultNanCStr)���Ăяo�����.
 *   setDefaultNanCStr�֐��́A���ꂼ�� nan, inf, -inf �Ƃ����������ݒ肷��.
 *
 * is_check_nan
 *   dval �� Nan, Inf, -Inf �`�F�b�N���s�����ۂ�������.
 *   true �̂Ƃ�
 *     ���̃`�F�b�N���s���Anan_callback_func �̎w��ɂ��A
 *     �K�؂ȕ�����̐ݒ肪�s����.
 *   false �̂Ƃ�
 *     dval �� Nan, Inf, -Inf �ł͂Ȃ��Ƃ������Ƃ��\�߂킩���Ă���A
 *     �`�F�b�N���s�v�̏ꍇ�Ɍ���Afalse ���w�肷�邱�Ƃ��ł���.
 *     �܂��A���̂Ƃ��Anan_callback_func ���R�[������邱�Ƃ͂Ȃ�.
 */
typedef char* (*NanCallBackFunc)( int, char*, bool );

/***
 * �w��N��z��C���f�b�N�X�Ƃ��ė^����� 32bit 10^N ��������.
 * N�� 0�ȏ�9�ȉ��łȂ���΂Ȃ�Ȃ�.
 */
static const uint32_t st_pow10_table32[] = {
	1,
	10,
	100,
	1000,
	10000,
	100000,
	1000000,
	10000000,
	100000000,
	1000000000, //32bit�̏ꍇ�A�����܂�.
};


/***
 * IEEE754 double�^�ɂ�����
 * 2��idx��l�̎w�����ɑ�������bit��͈ȉ��̂悤�ɂȂ�.
 * (�ŏ��bit�͌����ɂ͕�����(���Ȃ��1�������A���Ȃ��0�ɂȂ�)�ł���A
 * �w�����ł͂Ȃ�)
 *
 * (-1023)
 * 0x000, 0x001, 0x002, 0x003, 0x004, 0x005, 0x006, 0x007, 0x008, 0x009, 0x00a, ...
 * ...
 * ... 0x3f8, 0x3f9, 0x3fa, 0x3fb, 0x3fc, 0x3fd, 0x3fe, 0x3ff, //3fe 2^(-1), 3ff 2^0
 * //2^1  2^2    2^3
 * 0x400, 0x401, 0x402, 0x403, 0x404, 0x405, 0x406, 0x407, 0x408, 0x409, 0x40a, ...
 * ...
 * ...  0x7f8, 0x7f9, 0x7fa, 0x7fb, 0x7fc, 0x7fd, 0x7fe, 0x7ff,//(1024)
 *
 * idx �̒l���炱�������߂�ɂ́A0x3ff(=1023)�����Z���A52�r�b�g���V�t�g����΂悢.
 * idx �� -1023�ȏ�1024�ȉ��łȂ���΂Ȃ�Ȃ�.
 */
Znk_INLINE uint64_t getExp2Bits( int idx )
{
	return (uint64_t)(idx + 0x3ff) << 52;
}


#if 0
/**
 * 2��idx���double�l�ō����擾����.
 *
 * Inline_getExp2(0,-1023) ��0( �w�����A�������Ƃ��ɂ��ׂ�0 ).
 * �܂� log10( Inline_getExp2(0,1024) ) �� ���́�
 *
 * Inline_getExp2(N,-1023) (N>0) �͂��ׂĐ���̐��l(�ɂ߂�0�ɋ߂�).
 * �܂� log10( Inline_getExp2(N,1024) ) �� ���ׂĐ���̕��l.
 *
 * Inline_getExp2(0,1024) �͐��́�.
 * �܂� log10( Inline_getExp2(0,1024) ) �� ���́�
 *
 * Inline_getExp2(N,1024) (N>0) �͂��ׂ�NaN.
 * �܂� log10( Inline_getExp2(N,1024) ) �� ���ׂ�NaN.
 *
 * Inline_getExp2(N,1023) (N�͔C��) �͂��ׂĐ���̐��l.
 * �܂��Alog10( Inline_getExp2(N,1023) ) �� ���ׂĐ���̐��l.
 */
Znk_INLINE double Inline_getExp2( int idx ) // make* �Ƃ������O�̕����K�؂�...
{
	uint64_t t = getExp2Bits(idx);
	return (double)( *(double*)( &t ) );
}
#endif


/**
 * �ufraction * 2��idx��v ��double�l�ō����擾����.
 */
Znk_INLINE double Inline_getFracBitExp2( uint64_t fraction_bits, int idx )
{
	uint64_t bits = getExp2Bits(idx) | fraction_bits;
	return *(double*)(&bits);
}
Znk_INLINE double Inline_getFracExp2( double fraction, int idx )
{
    uint64_t fraction_bits = *(uint64_t*)&fraction;
    fraction_bits &= UINT64_C(0x000fffffffffffff);
	return Inline_getFracBitExp2( fraction_bits, idx );
}



/**
 * Inf�Ɋւ���VC/GCC�Ƃ��Ɉȉ���������.
 *
 * Inf==Inf( x, y �� �Ƃ���  Inf �ł���Ȃ�� x == y ����ɐ��藧��. )
 * -Inf==-Inf( x, y �� �Ƃ��� -Inf �ł���Ȃ�� x == y ����ɐ��藧��. )
 * 1.0/0.0 == std::numeric_limits<double>::infinity().
 * Inf+1 == Inf
 * Inf-1 == Inf
 * Inf*2 == Inf
 * Inf/2 == Inf
 * ��� 1 < Inf
 * 1 > Inf  �͕s����.
 * 1 == Inf �͕s����.
 * Inf > -Inf
 * Inf > 0
 * -Inf < 0
 * Inf > DBL_MAX
 * -Inf < DBL_MIN
 */ 


/**
 * +Inf ���Ӑ}�I�ɓ���.
 */
static double
getPositiveInf( void )
{
	static double inf = 0.0;
	if (inf == 0.0) {
		/**
		 * �ŏ��̈�񂾂����������s�����.
		 * �R���p�C���̌x����h�����߁A��C�����C���֐����� 0.0 ���擾���Ă���.
		 * �{���Ȃ�� numeric_limits ����擾����̂���Ԃ悢�̂��낤���A
		 * Boost�ɂ��΁ASTL limits �̎������s���S�ȃR���p�C�������������݂���Ƃ̂���.
		 * ����͎��ۂɊm�F���Ă݂Ȃ���΂Ȃ�Ƃ������Ȃ����A
		 * �Ƃ肠�����A�����ł�Inline_getFracExp2 ��p������@(bit���x���Őݒ�)���g��.
		 */
		inf = Inline_getFracExp2( 1.0, 1024 );
	}
	return inf;
}

/**
 * -Inf ���Ӑ}�I�ɓ���.
 */
static double
getNegativeInf( void )
{
	static double inf = 0.0;
	if (inf == 0.0) {
		/**
		 * �ŏ��̈�񂾂����������s�����.
		 * �R���p�C���̌x����h�����߁A��C�����C���֐����� 0.0 ���擾���Ă���.
		 */
		inf = -getPositiveInf();
	}
	return inf;
}

Znk_INLINE bool
Inline_isPositiveInf( double dval )
{
	return (bool)( dval == getPositiveInf() );
}


Znk_INLINE bool
Inline_isNegativeInf( double dval )
{
	return (bool)( dval == getNegativeInf() );
}


/**
 * ��or���̖�����Ȃ�� true.
 * �����Ȃ���� false
 *
 * �T�O�I�ɂ͈ȉ��̔���Ƃ�������.
 * (!Double_isFinite(n) && !Double_isNaN(n));
 */
Znk_INLINE bool Inline_isInf( double dval ) {
	return (bool)( Inline_isPositiveInf( dval ) || Inline_isNegativeInf( dval ) );
}


/**
 * NaN ���Ӑ}�I�ɓ���.
 * ���� fraction == *.0(����������0) �Ǝw�肵���ꍇ�A����Inf��������.
 */
Znk_INLINE double Inline_getNaN( double fraction ){
	return Inline_getFracExp2( fraction, 1024 );
}

/**
 * �R���p�C���ɂ��œK����}�~���邽�߂Ɏg�p�����C�����C���֐��Q
 */
static bool double_ne( double d1, double d2 ) { return (bool)(d1 != d2); }

/**
 * NaN �Ȃ�� true.
 * �����Ȃ���� false
 * VC�ł� _isnan�Ɠ���.
 *
 * VC/GCC�Ƃ���
 * x �� NaN �ł���Ȃ�� x != x ����ɐ��藧��.
 *
 * ������̒���:
 * �R���p�C���ɍœK�������Ȃ����߁A
 * �����Ĕ�C�����C���֐��Ŕ�r������.
 */
Znk_INLINE bool Inline_isNaN( double dval ) { return double_ne(dval, dval); }

/**
 * �L����(NaN�ł�Inf�ł��Ȃ�)�Ȃ�� true.
 * �����Ȃ���� false
 * VC�ł� _finite�Ɠ���.
 */
Znk_INLINE bool Inline_isFinite( double n ) { return (bool)( !Inline_isNaN(n) && !Inline_isInf(n) ); }


double ZnkDouble_getPositiveInf( void )      { return getPositiveInf(); }
double ZnkDouble_getNegativeInf( void )      { return getNegativeInf(); }

bool   ZnkDouble_isPositiveInf( double dval ){ return Inline_isPositiveInf( dval ); }
bool   ZnkDouble_isNegativeInf( double dval ){ return Inline_isPositiveInf( dval ); }
bool   ZnkDouble_isInf   ( double dval )     { return Inline_isInf   ( dval ); }

double ZnkDouble_getNaN  ( double fraction ) { return Inline_getNaN  ( fraction ); }
bool   ZnkDouble_isNaN   ( double dval )     { return Inline_isNaN   ( dval ); }
bool   ZnkDouble_isFinite( double dval )     { return Inline_isFinite( dval ); }

/*
 * endof DoubleInfoBase
 ***/
/*****************************************************************************/




Znk_INLINE char*
writeBytes( char* p, const void* src_bytes, size_t copy_size, bool term_nul )
{
	memcpy( p, src_bytes, copy_size );
	p += copy_size;
	if( term_nul ){ *p = '\0'; }
	return p;
}


/**
 * @brief
 *   Inline_decompToBase10 �Ŏ擾�����`���� fraction10 ��^���A
 *   p�Ŏ������buf�ʒu�� fraction10 �����������Ȃ������̕�������쐬����.
 *
 * @param
 * p: 
 *   �o�b�t�@�̊J�n�ʒu�������|�C���^.
 *   ���̊J�n�ʒu���猋�ʂ��o�b�t�@�֏㏑�������.
 *   �K�{�o�b�t�@�T�C�Y�� 32.
 *   (�����̎����I�ȍő吸�x��15���ł��邽�߁A32byte����Ώ\���ƍl������)
 *
 * term_nul:
 *   �Ō��'\0'�I�[���邩�ۂ�.
 *
 * fraction10:
 *   Inline_decompToBase10 �Ŏ擾�����`���� fraction10
 *
 * term_nul:
 *   �Ō��'\0'�I�[���邩�ۂ�.
 *
 * decimal_width:
 *   �������̏����_�ȉ��̌������w��.
 *   ������16�ȏ���w�肵�Ă������I�ȈӖ��͂Ȃ�.
 *
 * @return
 *   �쐬���ꂽ������̏I�[�ʒu('\0'���i�[�����ׂ��ʒu)��Ԃ�.
 */
Znk_INLINE char*
Fraction10ToStr_asDecFix( char* p, double fraction10, bool term_nul, size_t decimal_width )
{
	/***
	 * uint64_t i = static_cast<uint64_t>(decimal);
	 * �́Adecimal > UINT32_MAX �̂Ƃ��A���Ғʂ�̕ϊ��ɂȂ�Ȃ�.
	 * ������ɂ��� 10�i���̏ꍇ�Adouble�̐��x��15�����炢�����Ȃ����߁A
	 * 9byte�Â擾���Ă����Ȃ�.
	 */
	size_t radix = 10;

	uint32_t i = (uint32_t)fraction10;
	double decimal = fraction10 - i;
	size_t width = decimal_width;

	p = ZnkToStr_Private_UInt32ToStr( p, i,
			false, 0, '0', radix, 0 );
	*p++ = '.';

	if( decimal_width > 9 ){
		width = 9;
		decimal_width -= 9;
	}
	do {
		decimal *= st_pow10_table32[ width ];
		i = (uint32_t)decimal;
		p = ZnkToStr_Private_UInt32ToStr( p, i,
				false, width, '0', radix, ZnkToStr_e_IS_SUPPLEMENT | ZnkToStr_e_IS_TRUNCATE );
		decimal = decimal - i;

		if( decimal_width >= width ){
			decimal_width -= width;
		} else {
			break;
		}
	} while( decimal_width );

	if( decimal_width > 0 ){
		decimal *= st_pow10_table32[ decimal_width ];
		i = (uint32_t)(decimal);
		p = ZnkToStr_Private_UInt32ToStr( p, i,
				false, decimal_width, '0', radix, ZnkToStr_e_IS_SUPPLEMENT | ZnkToStr_e_IS_TRUNCATE );
	}
	if( term_nul ){ *p = '\0'; }
	return p;
}



/*****************************************************************************/
static const double st_table_plus[] = {
	1.0e00, 1.0e01, 1.0e02, 1.0e03, 1.0e04, 1.0e05, 1.0e06, 1.0e07, 1.0e08, 1.0e09,
	1.0e10, 1.0e11, 1.0e12, 1.0e13, 1.0e14, 1.0e15, 1.0e16, 1.0e17, 1.0e18, 1.0e19,
	1.0e20, 1.0e21, 1.0e22, 1.0e23, 1.0e24, 1.0e25, 1.0e26, 1.0e27, 1.0e28, 1.0e29,
	1.0e30, 1.0e31, 1.0e32, 1.0e33, 1.0e34, 1.0e35, 1.0e36, 1.0e37, 1.0e38, 1.0e39,
	1.0e40, 1.0e41, 1.0e42, 1.0e43, 1.0e44, 1.0e45, 1.0e46, 1.0e47, 1.0e48, 1.0e49,
	1.0e50, 1.0e51, 1.0e52, 1.0e53, 1.0e54, 1.0e55, 1.0e56, 1.0e57, 1.0e58, 1.0e59,
	1.0e60, 1.0e61, 1.0e62, 1.0e63, 1.0e64, 1.0e65, 1.0e66, 1.0e67, 1.0e68, 1.0e69,
	1.0e70, 1.0e71, 1.0e72, 1.0e73, 1.0e74, 1.0e75, 1.0e76, 1.0e77, 1.0e78, 1.0e79,
	1.0e80, 1.0e81, 1.0e82, 1.0e83, 1.0e84, 1.0e85, 1.0e86, 1.0e87, 1.0e88, 1.0e89,
	1.0e90, 1.0e91, 1.0e92, 1.0e93, 1.0e94, 1.0e95, 1.0e96, 1.0e97, 1.0e98, 1.0e99,
};
static const double st_table_minus[] = {
	1.0e-00, 1.0e-01, 1.0e-02, 1.0e-03, 1.0e-04, 1.0e-05, 1.0e-06, 1.0e-07, 1.0e-08, 1.0e-09,
	1.0e-10, 1.0e-11, 1.0e-12, 1.0e-13, 1.0e-14, 1.0e-15, 1.0e-16, 1.0e-17, 1.0e-18, 1.0e-19,
	1.0e-20, 1.0e-21, 1.0e-22, 1.0e-23, 1.0e-24, 1.0e-25, 1.0e-26, 1.0e-27, 1.0e-28, 1.0e-29,
	1.0e-30, 1.0e-31, 1.0e-32, 1.0e-33, 1.0e-34, 1.0e-35, 1.0e-36, 1.0e-37, 1.0e-38, 1.0e-39,
	1.0e-40, 1.0e-41, 1.0e-42, 1.0e-43, 1.0e-44, 1.0e-45, 1.0e-46, 1.0e-47, 1.0e-48, 1.0e-49,
	1.0e-50, 1.0e-51, 1.0e-52, 1.0e-53, 1.0e-54, 1.0e-55, 1.0e-56, 1.0e-57, 1.0e-58, 1.0e-59,
	1.0e-60, 1.0e-61, 1.0e-62, 1.0e-63, 1.0e-64, 1.0e-65, 1.0e-66, 1.0e-67, 1.0e-68, 1.0e-69,
	1.0e-70, 1.0e-71, 1.0e-72, 1.0e-73, 1.0e-74, 1.0e-75, 1.0e-76, 1.0e-77, 1.0e-78, 1.0e-79,
	1.0e-80, 1.0e-81, 1.0e-82, 1.0e-83, 1.0e-84, 1.0e-85, 1.0e-86, 1.0e-87, 1.0e-88, 1.0e-89,
	1.0e-90, 1.0e-91, 1.0e-92, 1.0e-93, 1.0e-94, 1.0e-95, 1.0e-96, 1.0e-97, 1.0e-98, 1.0e-99,
};
Znk_INLINE double getPow10D( int exp )
{
	/***
	 * d = 10^x = 2^p
	 * log2 d = x log2 10 = p
	 * a * 2^p (  0.5 < a < 2 )
	 * 0.5 * 2^p < a * 2^p < 2 * 2^p
	 * -1 + p < log2 a + p < 1 + p
	 */
	if( exp >= 0 ){
		if( exp < 100 ){
			return st_table_plus[ exp ];
		} else if( exp < 200 ){
			return st_table_plus[ exp-100 ] * 1.0e+100;
		} else if( exp < 300 ){
			return st_table_plus[ exp-200 ] * 1.0e+200;
		} else {
		}
		return st_table_plus[ exp-300 ] * 1.0e+300;
	}
	if( exp > -100 ){
		return st_table_minus[ -exp ];
	} else if( exp > -200 ){
		return st_table_minus[ -exp-100 ] * 1.0e-100;
	} else if( exp > -300 ){
		return st_table_minus[ -exp-200 ] * 1.0e-200;
	} else {
	}
	return st_table_minus[ -exp-300 ] * 1.0e-300;
}
#define M_LOG2_10 3.3219280948873626
Znk_INLINE double getAboutLog10( double dval ){
    uint64_t bits = *(uint64_t*)&dval;
    int      exp  = (int)( (bits >> 52) & UINT64_C(0x7ff) ) - (int)(0x3ff);
	return exp * ( 1.0 / M_LOG2_10 );
}
/*****************************************************************************/




Znk_INLINE double getExp10AndFractionFast( double positive_dval, int* exp_10 )
{
	/**
	 * positive_dval = 0�̏ꍇ�Alog10( positive_dval )��-���ɂȂ�.
	 */
	double L = getAboutLog10( positive_dval );

	/**
	 * rL = L - exp_10 = L - (int)(L)���
	 * -1.0 < rL < 1.0 �����藧��.
	 */
	*exp_10 = (int)(L);

	/**
	 * rL < 1.0  ���A10^rL < 10.0( =10^(1.0) )
	 * rL > -1.0 ���A10^rL > 0.1 ( =10^(-1.0))
	 *
	 * ratio = 10^rL�Ƃ�����
	 * 0.1 < ratio <10.0 �����藧��.
	 * ���̂Ƃ��Aratio �� 1��菬�����ꍇ�͏�����1�ʂ͕K����zero �ł���.
	 *
	 * rL�͈�ʂɏ������܂ނ��߁Apow�֐���p�����ɍ�����10^rL�����߂�͓̂��.
	 */
	return positive_dval / getPow10D( *exp_10 );
}


/***
 * @brief
 *   ����double�l(0�ł����Ă͂Ȃ�Ȃ�)��10�i���`���̉����E�w���ɕ�������.
 *
 * @param
 * positive_dval: 
 *   �ϊ��Ώۂ�double�l.
 *   positive_dval > 0 �łȂ���΂Ȃ�Ȃ�.
 *
 * fraction10: 
 *   dval��10�i�w���\���ɒ������ꍇ�̉����ɑ������镔�����i�[�����.
 *   fraction10�̐��������͏�Ɉꌅ�ŁA�ȉ��̂悤�ɂP�̈ʂƏ����_�Ə����_�ȉ�����Ȃ�. 
 *   A.BCDEFGHIJ...
 *   �P�̈ʂɓ�����A �� 0 ���傫�� 9 �ȉ��̐����ɂȂ�.
 *
 * exp_10: 
 *   dval��10�i�w���\���ɒ������ꍇ�̎w���ɑ������镔�����i�[�����.
 *   -308����308�܂ł̒l���Ƃ�ƍl���Ă悢.
 *
 * @return
 *   �Ȃ�.
 */
Znk_INLINE void
getBase10Fmt( double positive_dval, double* fraction10, int* exp_10 )
{
	/* Znk_ASSERT( positive_dval > 0.0 ); */
	/**
	 * 1.0 <= dval < 2.0,
	 * 0.1 < ratio < 10.0 ���A
	 * 0.1 < dval*ratio < 20.0
	 *
	 * fraction10 = dval*ratio �Ƃ�����
	 * 0.1 < fraction10 < 20.0
	 */
	*fraction10 = getExp10AndFractionFast( positive_dval, exp_10 );

	/**
	 * �����ŁA10.0 <= fraction10 < 20.0 �̏ꍇ�� fraction10 �� 10�Ŋ���A���̑���exp_10�� 1 ���₷.
	 * �܂��A  0.1  <  fraction10 < 1.0 �̏ꍇ��  fraction10 �� 10�{���A  ���̑���exp_10�� 1 ���炷.
	 * �������邱�ƂŁA��� 1.0 <= fraction10 < 10.0 �����藧��.
	 * ���̂Ƃ��Afraction10 ��1�̈ʂ͕K����zero �ɂȂ�.
	 */
	/* fraction10 += DBL_EPSILON; */
	if( *fraction10 >= 10.0 ){
		*fraction10 /= 10.0;
		++(*exp_10);
	} else if( *fraction10 < 1.0 ){
		*fraction10 *= 10.0;
		--(*exp_10);
	} else {
		// �������Ȃ�.
	}
}


/***
 * @brief
 *   double�l��10�i���`���̉����E�w���ɕ�������.
 *
 * @param
 * dval: 
 *   �ϊ��Ώۂ�double�l.
 *
 * sign_bit: 
 *   dval �� ���̂Ƃ� true
 *   ��(0 or ��)�̂Ƃ� false���i�[�����.
 *
 * fraction10: 
 *   dval��10�i�w���\���ɒ������ꍇ�̉����ɑ������镔�����i�[�����.
 *   fraction10�̐��������͏�Ɉꌅ�ŁA�ȉ��̂悤�ɂP�̈ʂƏ����_�Ə����_�ȉ�����Ȃ�. 
 *   A.BCDEFGHIJ...
 *   �P�̈ʂɓ�����A �� dval ����[���ł���ꍇ�� 0 ���傫�� 9 �ȉ��̐����ɂȂ�.
 *   dval �� 0.0 �̂Ƃ��AA �� 0 �ɂȂ�.
 *
 * exp_10: 
 *   dval��10�i�w���\���ɒ������ꍇ�̎w���ɑ������镔�����i�[�����.
 *   -308����308�܂ł̒l���Ƃ�ƍl���Ă悢.
 *
 * @return
 *   ���������������ꍇ true ��Ԃ�.
 *   �����Ȃ���� false ��Ԃ�.
 */
Znk_INLINE bool
Inline_decompToBase10( double dval, bool* sign_bit, double* fraction10, int* exp_10 )
{
    uint64_t bits = *(uint64_t*)&dval;
    *sign_bit = (bool)( (bits >> 63) == UINT64_C(0x1) );
	
	if( dval != 0.0 ){
		if( *sign_bit ){
			getBase10Fmt( -dval, fraction10, exp_10 );
		} else {
			getBase10Fmt( dval, fraction10, exp_10 );
		}
	} else {
		*fraction10 = 0.0;
		*exp_10 = 0;
	}
	return true;
}




#define addSignCh( p, sign_ch ) if( (sign_ch) != '\0' ){ *(p)++ = (sign_ch); }

Znk_INLINE char getSignCh( bool is_negative, bool is_zero, ZnkToStrFlags flags )
{
	bool is_none_sign  = Znk_IS_FLAG32(flags, ZnkToStr_e_IS_NONE_SIGN);
	if( is_none_sign ){ return '\0'; }

	if( is_negative ){ return '-'; }

	if( is_zero ){
		bool is_zero_sign_white = Znk_IS_FLAG32(flags, ZnkToStr_e_IS_ZERO_SIGN_WHITE);
		bool is_zero_sign_plus  = Znk_IS_FLAG32(flags, ZnkToStr_e_IS_ZERO_SIGN_PLUS);
		bool is_zero_sign_minus = Znk_IS_FLAG32(flags, ZnkToStr_e_IS_ZERO_SIGN_MINUS);

		if( is_zero_sign_minus ){
			return '-';
		}else if( is_zero_sign_plus ){
			return '+';
		}else if( is_zero_sign_white ){
			return ' ';
		}else{
			/* none */
		}
		return '\0';
	}
	if( Znk_IS_FLAG32(flags, ZnkToStr_e_IS_PLUS) ){ return '+'; }

	return '\0';
}

Znk_INLINE char getExpSignCh( bool is_negative, bool is_zero, ZnkToStrFlags flags )
{
	bool is_none_sign  = Znk_IS_FLAG32(flags, ZnkToStr_e_IS_EXP_NONE_SIGN);
	if( is_none_sign ){ return '\0'; }

	if( is_negative ){ return '-'; }

	if( is_zero ){
		bool is_zero_sign_plus  = Znk_IS_FLAG32(flags, ZnkToStr_e_IS_EXP_ZERO_SIGN_PLUS);
		bool is_zero_sign_minus = Znk_IS_FLAG32(flags, ZnkToStr_e_IS_EXP_ZERO_SIGN_MINUS);

		if( is_zero_sign_minus ){
			return '-';
		}else if( is_zero_sign_plus ){
			return '+';
		}else{
			/* none */
		}
		return '\0';
	}

	if( Znk_IS_FLAG32(flags, ZnkToStr_e_IS_EXP_PLUS) ){  return '+'; }

	return '\0';
}


#if 0
Znk_INLINE char* addSign( char* p, bool sign_bit, char plus_ch ) {
	*p++ = sign_bit ? '-' : plus_ch;
	return p;
}

/**
 * plus_ch �̒l�ɉ����āA�������ʒup�ɕt������.
 *
 * ��plus_ch �� '-' �̂Ƃ�
 *   sign_bit �� true �̂Ƃ��A'-' ��t������.
 *   sign_bit �� false �̂Ƃ��A�����t�����Ȃ�.
 *
 * ��plus_ch �� '-' �ȊO�̂Ƃ�
 *   sign_bit �� true �̂Ƃ��A'-' ��t������.
 *   sign_bit �� false �̂Ƃ��Aplus_ch ��t������.
 *
 * ���炩�̕������t�����ꂽ�ꍇ�� true �� �����Ȃ���� false ��Ԃ�.
 */
Znk_INLINE bool autoaddSign( char*& p, bool sign_bit, char plus_ch )
{
	bool is_added = false;
	switch( plus_ch ){
	case '-':
		if( sign_bit ){
			*p++ = '-';
			is_added = true;
		}
		break;
	default:
		p = addSign( p, sign_bit, plus_ch );
		is_added = true;
		break;
	}
	return is_added;
}
#endif

#if 0
/**
 * ���炩�̕������ǉ������ꍇ���ۂ��𔻒肷��.
 */
Znk_INLINE bool isAddSign( bool sign_bit, char plus_ch )
{
	bool is_added = false;
	switch( plus_ch ){
	case '-':
		if( sign_bit ){
			is_added = true;
		}
		break;
	default:
		is_added = true;
		break;
	}
	return is_added;
}
#endif

/**
 * p ����� size�� ch ��ǉ�.
 * �ŏI�ǉ��ʒu�̎��̃A�h���X��Ԃ�.
 */
Znk_INLINE char* fillChar( char* p, size_t size, char ch )
{
	memset( p, ch, size );
	p += size;
	return p;
}


/**
 * begin ���� size ���������폜����.
 * �������A�폜���begin�ʒu�͕s�ς� end �ʒu�݂̂��ς��.
 * �V�����X�V���ꂽ end �ʒu���Ԃ�.
 */
Znk_INLINE char* removeFrontByte_withBeginFix( char* begin, char* end, size_t size )
{
	if( size ){
		/**
		 * �ŏ���size�����J�b�g����.
		 * ���̂��߂ɁAsize byte�ȍ~�̕�����begin�ʒu�Ɉړ����Ă���.
		 */
		char* new_end = end-size;
		memmove( begin, begin+size, new_end-begin );
		return new_end;
	}
	return end;
}

/**
 * ���������Œ�ɂ�����]��X�y�[�X��space_ch����.
 *
 * int_width < 2 �̂Ƃ��A�������Ȃ�.
 * int_width >= 2 �̂Ƃ��Aint_width �� exp_10 �̒l�ɂ���� �ŏ��� '0' ����������. 
 */
Znk_INLINE char* addFrontZeros( char* p, size_t int_width, int exp_10, char space_ch, bool is_supplement )
{
	if( int_width < 2 || !is_supplement ){
		return p;
	} else {
		size_t remain_width = int_width - 1;
		if( exp_10 >= (int)(remain_width) ){
			return p;
		}
		if( exp_10 > 0 ){
			remain_width -= exp_10;
		}
	
		p = fillChar( p, remain_width, space_ch );
	}
	return p;
}

/**
 * �����_�����Ɉړ����邽�߂� begin ���ǂꂾ���E�ֈړ����Ă����΂悢���Ƃ���
 * �ʒu�����l�𓾂�.
 */
Znk_INLINE void modifyOffset_forFix( size_t* offset, int exp_10, size_t int_width )
{
	if( exp_10 < 0 ){
		*offset = (int_width == 0) ? -exp_10-1 : -exp_10;
	}
}
Znk_INLINE void modifyOffset_forFlex( size_t* offset, int exp_10 )
{
	if( exp_10 < 0 ){
		*offset = -exp_10;
	}
}


/**
 * is_sign_top == true �̂Ƃ�:
 *   is_compact== true �̂Ƃ�:
 *     ���������Ȃ��Ƃ��͂ł��邾��int_width����-�����߂�悤�ɂ���.
 *     �� : int_width == 5 �̂Ƃ�
 *       @@@12.3456 => -@@12.3456
 *       23456.0000 => -23456.0000
 *   is_compact == false �̂Ƃ�:
 *     ���������Ȃ��ꍇ�ł� int_width + 1 �����ŌŒ肷��.
 *     �� : int_width == 5 �̂Ƃ�
 *       @@@12.3456 => -@@@12.3456
 *       23456.0000 => -23456.0000
 *
 * is_sign_top == false �̂Ƃ�:
 *   ���is_compact == true�̂悤�ɐU����.
 *   ���Ȃ킿�A��Ɍ��������Ȃ��Ƃ��͂ł��邾��int_width����-�����߂�悤�ɂ���.
 *   �� : int_width == 5 �̂Ƃ�
 *     @@@12.3456
 *     @@-12.3456
 *     23456.0000
 *     -23456.0000
 *
 *  compact �ł���Ƃ́A��ԍ��̕�����space_ch�Ȃ�΂���𕄍��Œu�����邱�Ƃ�����.
 *  compact �łȂ��Ƃ́A��ԍ��̕������Ȃ�ł��낤�Ƃ��̍����ɕ�����ǉ����邱�Ƃ�����.
 */
/**
 * �������₷�����邽�߁A�ȉ��ɓT�^�I�ȃp�^�[����p���čl����.
 *
 *   exp_10 == 3 �̂Ƃ�
 *     1.2345e2 => 1234.5 �Ƃ����ϊ�
 *   exp_10 == 2 �̂Ƃ�
 *     1.2345e2 => 123.45 �Ƃ����ϊ�
 *   exp_10 == 1 �̂Ƃ�
 *     1.2345e1 => 12.345 �Ƃ����ϊ�
 *   exp_10 == 0 �̂Ƃ�
 *     1.2345e0 => 1.2345 �Ƃ����ϊ�
 *   exp_10 == -1 �̂Ƃ�
 *     1.2345e-1 => 0.12345 �Ƃ����ϊ�
 *   exp_10 == -2 �̂Ƃ�
 *     1.2345e-1 => 0.012345 �Ƃ����ϊ�
 *   exp_10 == -3 �̂Ƃ�
 *     1.2345e-1 => 0.0012345 �Ƃ����ϊ�
 *
 * ���̂��Ƃ��A����������.
 * exp_10 >= 0 �̂Ƃ��A���������̌����� exp_10+1
 * exp_10 <  0 �̂Ƃ��A���������̌����͏�� 1
 * int_width == 0, int_width == 1 �̂Ƃ��́Aspace_ch���߂̗]�n�͂Ȃ�.
 * int_width >= 2 �ł���A�ȉ��̏����𖞂����Ƃ��Aspace_ch���߂̉\������������.
 * static_cast<int>(int_width) > exp_10+1
 */
Znk_INLINE char*
compaction( char* begin, bool is_supplement, bool is_compact,
		size_t int_width, int exp_10,
		char space_ch, char sign_ch,
		bool is_sign_top )
{
	if( is_sign_top ){
		if( is_compact && int_width >= 2 && (int)int_width > exp_10+1 ){
			/*
			 * int_width �ɔ�ׂ� exp_10�ɂ��J�グ���������Ȃ��ꍇ.
			 * space_ch �ɂ�閄�߂���������.
			 **/
			if( sign_ch != '\0' ){
				/**
				 * ���������݂���ꍇ�́A�������������炷.
				 * (���������Ȃ����ߐ؂�̂Ă͔������Ȃ�)
				 */
				addSignCh( begin, sign_ch );
				begin = addFrontZeros( begin, int_width-1, exp_10, space_ch, is_supplement );
			} else {
				/**
				 * ������t���Ȃ��ꍇ.
				 */
				begin = addFrontZeros( begin, int_width, exp_10, space_ch, is_supplement );
			}
		} else {
			/**
			 * ������P�ɍ��ɒǉ�����ꍇ.
			 */
			addSignCh( begin, sign_ch );
			begin = addFrontZeros( begin, int_width, exp_10, space_ch, is_supplement );
		}
	} else {
		if( int_width >= 2 && (int)int_width > exp_10+1 ){
			/*
			 * int_width �ɔ�ׂ� exp_10�ɂ��J�グ���������Ȃ��ꍇ.
			 **/
			if( sign_ch != '\0' ){
				/**
				 * ���������݂���ꍇ�́A�������������炷.
				 * (���������Ȃ����ߐ؂�̂Ă͔������Ȃ�)
				 */
				begin = addFrontZeros( begin, int_width-1, exp_10, space_ch, is_supplement );
				addSignCh( begin, sign_ch );
			} else {
				/**
				 * ������t���Ȃ��ꍇ.
				 */
				begin = addFrontZeros( begin, int_width, exp_10, space_ch, is_supplement );
			}
		} else {
			/**
			 * ������P�ɍ��ɒǉ�����ꍇ.
			 */
			begin = addFrontZeros( begin, int_width, exp_10, space_ch, is_supplement );
			addSignCh( begin, sign_ch );
		}
	}
	return begin;
}


/**
 * sign_policy �̒l�ɉ����� begin �� int_width �̒l�𒲐���
 * �K�v�ɉ����� ���������� space_ch �Ŗ��߂�.
 */
Znk_INLINE char* adjust_bySignPolicy( char* begin, size_t* int_width,
		int exp_10, char space_ch,
		char sign_ch, char sign_policy,
		ZnkToStrFlags flags,
		bool is_supplement, bool is_truncate )
{
	size_t offset = 0;
	bool is_sign_top = Znk_IS_FLAG32(flags, ZnkToStr_e_IS_SIGN_TOP);
	bool is_compact  = Znk_IS_FLAG32(flags, ZnkToStr_e_IS_COMPACT);

	if( is_truncate ){
		switch( sign_policy ){
		case 'U': // �K�v�Ȃ�Ε������J�b�g����int_width�Ɏ��߂�.
		{
			//fprintf(stderr, "adjust_bySignPolicy:%c: int_width=[%u] exp_10=[%d]\n", sign_policy, int_width, exp_10 );
	
			if( *int_width >= 2 && (int)*int_width > exp_10+1 && sign_ch != '\0' ){
				/**
				 * space_ch���߂̂Ƃ��͏��compact��
				 */
				if( is_sign_top ){
					addSignCh( begin, sign_ch );
					begin = addFrontZeros( begin, *int_width-1, exp_10, space_ch, is_supplement );
				} else {
					begin = addFrontZeros( begin, *int_width-1, exp_10, space_ch, is_supplement );
					addSignCh( begin, sign_ch );
				}
			} else {
				/**
				 * space_ch���߂��Ȃ��Ƃ��́A���������Ȃ�.
				 */
				begin = addFrontZeros( begin, *int_width, exp_10, space_ch, is_supplement );
			}
			break;
		}
		case 'S': // �����̕��������l�\�������炵��int_width�Ɏ��߂�.
		{
			//fprintf(stderr, "adjust_bySignPolicy:%c: int_width=[%u] exp_10=[%d] is_sign_top=[%d]\n", sign_policy, int_width, exp_10, is_sign_top );
	
			if( *int_width > 0 && sign_ch != '\0' ){
				--*int_width;
				if( is_sign_top ){
					addSignCh( begin, sign_ch );
					begin = addFrontZeros( begin, *int_width, exp_10, space_ch, is_supplement );
				} else {
					begin = addFrontZeros( begin, *int_width, exp_10, space_ch, is_supplement );
					addSignCh( begin, sign_ch );
				}
			} else {
				/**
				 * space_ch���߂��Ȃ��Ƃ��́A���������Ȃ�.
				 */
				begin = addFrontZeros( begin, *int_width, exp_10, space_ch, is_supplement );
			}
			break;
		}
		case 'E':
		default:
			/* �����̕�����int_width���z���Ċg��. */
			//fprintf(stderr, "adjust_bySignPolicy:%c: int_width=[%u] exp_10=[%d]\n", sign_policy, int_width, exp_10 );
	
			/**
			 * ����������int_width�𒴂���ꍇ�ɏ�ʂ��ł��؂���.
			 * ����������int_width�ɖ����Ȃ��ꍇ��
			 *   is_supplement == true  �̂Ƃ��� space_ch ���߂��s��.
			 *   is_supplement == false �̂Ƃ��� space_ch ���߂��s�Ȃ킢.
			 */
			begin = compaction( begin, is_supplement, is_compact,
					*int_width, exp_10,
					space_ch, sign_ch,
					is_sign_top );
			break;
		}
		modifyOffset_forFix( &offset, exp_10, *int_width );

	} else {
		/**
		 * ����������int_width�𒴂���ꍇ�ł���ʂ��ł��؂��Ȃ�.
		 * ����������int_width�ɖ����Ȃ��ꍇ��
		 *   is_supplement == true  �̂Ƃ��� space_ch ���߂��s��.
		 *   is_supplement == false �̂Ƃ��� space_ch ���߂��s�Ȃ킢.
		 */
		begin = compaction( begin, is_supplement, is_compact,
				*int_width, exp_10,
				space_ch, sign_ch,
				is_sign_top );

		modifyOffset_forFlex( &offset, exp_10 );
	}

	/**
	 * �����_�����Ɉړ����邽�߂� begin �ʒu����.
	 *
	 * exp_10 >= 0 �̂Ƃ�:
	 *   ���Ɉړ����邱�Ƃ͂��蓾�Ȃ�(�E�ɂ����ړ������Ȃ�)���߁A
	 *   offset=0 �łȂ���΂Ȃ�Ȃ�.
	 * exp_10 < 0 �̂Ƃ�:
	 *   offset = -exp_10 �Ŋ�{�I�ɂ悢.
	 *   �������Aint_width == 0 ���� �Œ蕝�̂Ƃ��Aoffset �� 1 ���₵�Ă����Ȃ���΂Ȃ�Ȃ�.
	 *   ��������Ȃ��ƍ����ɏ����_��2���Ă��܂�.
	 */
	begin += offset;

	return begin;
}


/**
 * �����_�̈ړ��Ɋւ����{����
 */
Znk_INLINE char* swapFloatingPoint( char* point )
{
	Znk_SWAP( char, *point, *(point-1) ); --point;
	return point;
}
Znk_INLINE char* setZero_onLeftOfPoint( char* point )
{
	*(point-1) = '0';
	return point;
}
Znk_INLINE char* moveFloatingPointToRight( char* point, size_t num )
{
	while( num > 0 ){
		Znk_SWAP( char, *point, *(point+1) ); ++point;
		--num;
	}
	return point;
}


/**
 * ���Ƃ�double�l�͔�[���łȂ���΂Ȃ�Ȃ�.
 * sign_policy != 'A' �łȂ���΂Ȃ�Ȃ�.
 */
Znk_INLINE void getShortage(
		int* num_shortage, bool* sign_shortage,
		char sign_ch, int exp_10, size_t int_width, char sign_policy )
{

	*num_shortage = exp_10 - (int)(int_width) + 1;

	*sign_shortage = false;
	if( sign_ch != '\0' ){
		switch( sign_policy ){
		case 'U':
			if( int_width == 0 || int_width == 1 || *num_shortage >= 0 ){
				*sign_shortage = true;
			}
			break;
		case 'S':
			if( int_width == 0 ){
				*sign_shortage = true;
			} else if( *num_shortage >= 0 ){
				++*num_shortage;
			} else {
			}
			break;
		default:
			break;
		}
	}
}


/*****************************************************************************/


static char*
setDefaultNanCStr( int type, char* p, bool term_nul )
{
	if( type > 0 ){
		return writeBytes( p, "inf", 3, term_nul );
	} else if( type < 0 ){
		return writeBytes( p, "-inf", 4, term_nul );
	} else {
		/* none */
	}
	return writeBytes( p, "nan", 3, term_nul );
}


Znk_INLINE bool
checkNanAndInf( char** p, double dval, bool term_nul, NanCallBackFunc nan_callback_func )
{
	if( nan_callback_func == NULL ){ nan_callback_func = setDefaultNanCStr; }
	if( Inline_isNaN( dval ) ){
		*p = (*nan_callback_func)( 0, *p, term_nul );
		return true;
	}
	if( Inline_isInf( dval ) ){
		if( dval < 0.0 ){
			*p = (*nan_callback_func)( -1, *p, term_nul );
			return true;
		}
		*p = (*nan_callback_func)( 1, *p, term_nul );
		return true;
	}
	return false;
}



/**
 * @brief
 *   C99 printf�� %a %A ���̕ϊ��������s��.
 *   Linux �� glibc printf�̏o�͂Ƀ}�b�`����悤�Ȏd�l�ɂ��Ă���A
 *   �������� dval==0.0�̂Ƃ�0�A����ȊO�̂Ƃ��͕K�� 1 �ƂȂ�.
 *
 * @param
 *
 * term_nul:
 *   �Ō��'\0'�I�[���邩�ۂ�.
 *
 * decimal_width:
 *   IS_FLEX_DECIMAL �� true �̏ꍇ
 *     ���������.
 *   IS_FLEX_DECIMAL �� false �̏ꍇ
 *     ���������Œ蕝�ɂ������ꍇ�ł���A���̌������w�肷��.
 *     truncate�ł���Asupplement�ł�����悤�ȏ������s��.
 *     0���w�肵���ꍇ�͐������݂̂ɂȂ�.
 *
 * exp_width:
 *   �w�����̌���.
 *
 * flags:
 *   �ȉ��̑��������߂���.
 *   IS_LOWER
 *   IS_EXP_LOWER
 *   IS_FLEX_DECIMAL
 *   IS_PLUS
 *   IS_NONE_SIGN
 *   IS_ZERO_SIGN_WHITE
 *   IS_ZERO_SIGN_PLUS
 *   IS_ZERO_SIGN_MINUS
 *   IS_NONE_EXP
 *   IS_EXP_PLUS
 *   IS_EXP_NONE_SIGN
 *   IS_EXP_ZERO_SIGN_PLUS
 *   IS_EXP_ZERO_SIGN_MINUS
 *
 * nan_callback_func
 *   NanCallBackFunc �錾�ɂ���R�����g���Q��.
 *
 * is_check_nan
 *   NanCallBackFunc �錾�ɂ���R�����g���Q��.
 */
static char*
DoubleToStr_asHexE( char* p, double dval, bool term_nul,
		size_t decimal_width, size_t exp_width, ZnkToStrFlags flags,
		NanCallBackFunc nan_callback_func, bool is_check_nan )
{
	if( is_check_nan ){
		if( checkNanAndInf( &p, dval, term_nul, nan_callback_func ) ){ return p; }
	}

	{
		bool is_lower        = Znk_IS_FLAG32(flags, ZnkToStr_e_IS_LOWER);
		bool is_exp_lower    = Znk_IS_FLAG32(flags, ZnkToStr_e_IS_EXP_LOWER);
		bool is_flex_decimal = Znk_IS_FLAG32(flags, ZnkToStr_e_IS_FLEX_DECIMAL);
	
	    uint64_t bits = *(uint64_t*)&dval;
	
	    bool     sign_bit = (bool)( (bits >> 63) == UINT64_C(0x1) );
	    int      exp      = (int)( (bits >> 52) & UINT64_C(0x7ff) ) - (int)0x3ff;
	    uint64_t fraction_bits = bits & UINT64_C(0x000fffffffffffff);
	
		const char* xdigit = is_lower ?
			"0123456789abcdef" :
			"0123456789ABCDEF";

	
		char sign_ch = getSignCh( sign_bit, (bool)(dval == 0), flags );
		if( sign_ch != '\0' ){ *p++ = sign_ch; }
	
		*p++ = '0';
		*p++ = is_lower ? 'x' : 'X';
	
		if( dval == 0.0 ){
			*p++ = '0';
	
			if( !is_flex_decimal ){
				/**
				 * �Œ蕝���[�h�̂Ƃ��́A��������0��t������.
				 */
				if( decimal_width > 0 ){
					*p++ = '.';
					while( decimal_width ){
						*p++ = '0';
						--decimal_width;
					}
				}
			}
			*p++ = is_exp_lower ? 'p' : 'P';
			*p++ = '+';
			*p++ = '0';
			if( term_nul ){ *p++ = '\0'; }
			return p;
		} else {
			/**
			 * �����̈�ԉE���̌�(13��)���珬���_(1��)�̕����֏��Ɍ����X�L�������鏈���ɂ�����
			 * ���߂Ĕ�[���̌��������ꍇ��true�ƂȂ鐧��t���O.
			 */
			bool   in_body = false;

			/**
			 * �����̈�ԉE���̌�(13��)���珬���_(1��)�̕����֏��Ɍ����X�L�������鏈���ɂ�����
			 * ���߂Ĕ�[���̌��������ʒu���珬���_�ɒB����܂łɉ������邩�������l.
			 */
			size_t decimal_width_enable=13;

			size_t idx;

			*p++ = '1';
			*p++ = '.';
		
			for(idx=0; idx<13; ++idx){
				p[ 12-idx ] = xdigit[ (uint8_t)(fraction_bits & UINT64_C(0xF)) ];
				fraction_bits >>= 4;
		
				if( !in_body ){
					if( p[ 12-idx ] == '0' ){
						--decimal_width_enable;
					} else {
						/**
						 * ���߂Ĕ�[���̌�������.
						 */
						in_body = true;
					}
				}
			}
		
			if( is_flex_decimal || decimal_width_enable == decimal_width ){
				/**
				 * ���ɉ��H�Ȃ��ɕK�v�\���Ȍ������񋟂����.
				 */
				if( decimal_width_enable > 0 ){
					p += decimal_width_enable;
				} else {
					/**
					 * ���������Ȃ����ߏ����_�̕\�����K�v�Ȃ�.
					 * �����_�̕��A�ꕶ���߂�.
					 */
					--p;
				}
		
			} else {
		
				if(decimal_width_enable > decimal_width ){
					if( decimal_width > 0 ){
						/**
						 * GCC�ł͂����ł�������̌����Q�Ƃ��A�l�̌ܓ������Ă���.
						 * �Ƃ肠���������ł́A�P�Ɍ��̌���؂�̂ĂĂ���.
						 */
						p += decimal_width;
					} else {
						/**
						 * ���������Ȃ����ߏ����_�̕\�����K�v�Ȃ�.
						 * �����_�̕��A�ꕶ���߂�.
						 */
						--p;
					}
				} else {
					size_t num;
					p += decimal_width_enable;
					/**
					 * �����𖞂������߁A�c��̌���0�Ŗ��߂�.
					 */
					num = decimal_width - decimal_width_enable;
					while( num ){
						*p++ = '0';
						--num;
					}
				}
		
			}
		
			if( ! Znk_IS_FLAG32(flags, ZnkToStr_e_IS_NONE_EXP) ){
				char exp_sign_ch;
				*p++ = is_exp_lower ? 'p' : 'P';
		
				exp_sign_ch = getExpSignCh( (bool)(exp < 0), (bool)(exp == 0), flags );
				if( exp_sign_ch != '\0' ){ *p++ = exp_sign_ch; }
		
				if( exp < 0 ){ exp = -exp; }
		
				flags |= ZnkToStr_e_IS_SUPPLEMENT;
				p = ZnkToStr_Private_UInt32ToStr( p, exp,
						false, exp_width, '0', 10, flags );
			}
		
			if( term_nul ){ *p++ = '\0'; }
		}
	}
	return p;
}



/***
 * @brief
 *   double�l�������E�w���`����10�i��������ɕϊ�����.
 *
 * @param
 * p: 
 *   �o�b�t�@�̊J�n�ʒu�������|�C���^.
 *   ���̊J�n�ʒu���猋�ʂ��o�b�t�@�֏㏑�������.
 *   �K�{�o�b�t�@�T�C�Y�� 32.
 *   (�����̎����I�ȍő吸�x��15���A�����_�Ɗe�필���A�w�������l�����Ă�32byte����Ώ\���ƍl������)
 *
 * dval: 
 *   �ϊ��Ώۂ�double�l.
 *
 * term_nul:
 *   �Ō��'\0'�I�[���邩�ۂ�.
 *
 * decimal_width:
 *   �������̏����_�ȉ��̌������w��.
 *   ������16�ȏ���w�肵�Ă������I�ȈӖ��͂Ȃ�.
 *
 * exp_width:
 *   �w�����̌������w��.
 *   double �l �� 10�i���ł�exp_10�͈̔͂� -308����308�܂łł��邽�߁A
 *   3 ���ł��ׂĂ̏ꍇ���܂��Ȃ��邽�߁A������傫�Ȓl���w�肵�Ă�
 *   ���܂�Ӗ��͂Ȃ�.
 *
 *   IS_EXP_TRUNCATE ���w�肳��Ă���ꍇ�A
 *     ���ۂ̌�����exp_width �̎w�蕝�𒴂��Ă���ꍇ�͏�ʂ�ł��؂�.
 *     �����Ȃ���Ή������Ȃ�.
 *   IS_EXP_SUPPLEMENT ���w�肳��Ă���ꍇ�A
 *     ���ۂ̌�����exp_width �̎w�蕝�ɖ����Ȃ��ꍇ�͕s������ '0' �Ŗ��߂�.
 *     �����Ȃ���Ή������Ȃ�.
 *
 *   �� GCC �̕W��printf �̃f�t�H���g�̓���� exp_width == 2,
 *      IS_EXP_SUPPLEMENT == true, IS_EXP_TRUNCATE == false �ł���.
 *
 *   �� VC �̕W��printf �̃f�t�H���g�̓���� exp_width == 3,
 *      IS_EXP_SUPPLEMENT == true, IS_EXP_TRUNCATE == false �ł���.
 *      VC8.0�ȍ~�Ɍ���΁A_set_output_format�Œ�������΁A
 *      GCC �Ɠ�������( exp_width == 2) �Ƃ��邱�Ƃ��ł���Ƃ̂���.
 *
 * flags
 *   �ȉ��̑��������߂���.
 *   IS_EXP_LOWER
 *   IS_PLUS
 *   IS_NONE_SIGN
 *   IS_ZERO_SIGN_WHITE
 *   IS_ZERO_SIGN_PLUS
 *   IS_ZERO_SIGN_MINUS
 *   IS_NONE_EXP
 *   IS_EXP_PLUS
 *   IS_EXP_NONE_SIGN
 *   IS_EXP_ZERO_SIGN_PLUS
 *   IS_EXP_ZERO_SIGN_MINUS
 *   IS_EXP_TRUNCATE
 *   IS_EXP_SUPPLEMENT
 *
 * nan_callback_func
 *   NanCallBackFunc �錾�ɂ���R�����g���Q��.
 *
 * is_check_nan
 *   NanCallBackFunc �錾�ɂ���R�����g���Q��.
 */
static char*
DoubleToStr_asDecE( char* p, double dval, bool term_nul,
		size_t decimal_width, size_t exp_width, ZnkToStrFlags flags,
		NanCallBackFunc nan_callback_func, bool is_check_nan )
{
	if( is_check_nan ){
		if( checkNanAndInf( &p, dval, term_nul, nan_callback_func ) ){ return p; }
	}

	{
		//bool is_truncate_exp = true; // ����� false �ɂ��邱�Ƃɂ���āAexp_width == 4 �Ƃ����������s�v�ɂȂ邩������Ȃ�.
		/**
		 * dval ��艺�L�R�l�𓾂�.
		 * (������Inline_decompToBase10�ɂ��擾���ꂽ��͊�{�I�ɕω����Ȃ�.)
		 */
		bool   sign_bit   = false;
		double fraction10 = 0.0;
		int    exp_10     = 0;
		char   sign_ch;
		Inline_decompToBase10( dval, &sign_bit, &fraction10, &exp_10 );
	
		sign_ch = getSignCh( sign_bit, (bool)(dval == 0), flags );
		if( sign_ch != '\0' ){ *p++ = sign_ch; }
	
		p = Fraction10ToStr_asDecFix( p, fraction10, term_nul, decimal_width );
	
		if( ! Znk_IS_FLAG32(flags, ZnkToStr_e_IS_NONE_SIGN) ){
			char exp_sign_ch;
			ZnkToStrFlags exp_flags = 0;
			bool is_exp_lower = Znk_IS_FLAG32(flags,ZnkToStr_e_IS_EXP_LOWER);

			*p++ = is_exp_lower ? 'e' : 'E';
			exp_sign_ch = getExpSignCh( (bool)(exp_10 < 0), (bool)(exp_10 == 0), flags );
			if( exp_sign_ch != '\0' ){ *p++ = exp_sign_ch; }
	
			if( exp_10 < 0 ){ exp_10 = -exp_10; }

			if( Znk_IS_FLAG32(flags, ZnkToStr_e_IS_EXP_TRUNCATE) )  { exp_flags |= ZnkToStr_e_IS_TRUNCATE; }
			if( Znk_IS_FLAG32(flags, ZnkToStr_e_IS_EXP_SUPPLEMENT) ){ exp_flags |= ZnkToStr_e_IS_SUPPLEMENT; }
			p = ZnkToStr_Private_UInt32ToStr( p, exp_10, false, exp_width, '0', 10, exp_flags );
		}
		if( term_nul ){ *p = '\0'; }
	}
	return p;
}



/**
 * @brief
 *   double�l�������E�w���`���ł͂Ȃ��ʏ��10�i��������ɕϊ�����.
 *   �Œ蕝 or �L�k�̗������T�|�[�g����.
 *
 * @param
 * p: 
 *   �o�b�t�@�̊J�n�ʒu�������|�C���^.
 *   ���̊J�n�ʒu���猋�ʂ��o�b�t�@�֏㏑�������.
 *   �K�{�o�b�t�@�T�C�Y�� 512.
 *
 * dval: 
 *   �ϊ��Ώۂ�double�l
 *
 * term_nul:
 *   �Ō��'\0'�I�[���邩�ۂ�.
 *
 * int_width:
 *   ���������̊�l��^����.
 *   IS_TRUNCATE �� true �̂Ƃ�
 *     �w���\������W�J���ꂽ���ʁA��������������𒴂���ꍇ�͏�ʂ̌���؂�̂Ă�.
 *     �����Ȃ���Ή������Ȃ�.
 *   IS_SUPPLEMENT �� true �̂Ƃ�
 *     �w���\������W�J���ꂽ���ʁA��������������ɖ����Ȃ��ꍇ�́A�s������space_ch�Ŗ��߂�.
 *     �����Ȃ���Ή������Ȃ�.
 *   int_width == 0, IS_TRUNCATE == true �̏ꍇ��sign_policy�̎w��ɂ���ẮA
 *   -.123 �� -00123.56 �Ƃ����悤�ȕ\�L�ɂȂ邱�Ƃ����蓾��.
 *
 * decimal_width:
 *   ���������̌������w��.
 * 
 * space_ch:
 *   int_width �ɖ����Ȃ��������𖄂߂�̂Ɏg�p���镶�����w��.
 *   �ł��T�^�I�ȗ�́Aspace_ch = '0' �� space_ch = ' ' �ł���.
 *
 * flags:
 *   �ȉ��̑��������߂���.
 *   IS_TRUNCATE
 *   IS_SUPPLEMENT
 *   IS_SIGN_TOP
 *   IS_COMPACT
 *   IS_NONE_SIGN
 *   IS_PLUS
 *   IS_ZERO_SIGN_WHITE
 *   IS_ZERO_SIGN_PLUS
 *   IS_ZERO_SIGN_MINUS
 *
 * sign_policy:
 *   IS_TRUNCATE == true �������������w�茅�𒴂���ꍇ�A
 *   �����̈����Ɋւ��Ĉȉ��̂ǂ��D�悵�ď������邩���w�肷��.
 *
 *       ����  �����\��  �w�蕝
 *     U ��    ��        ��
 *     S ��    ��        ��(int_width=0�̏ꍇ�͕������ȗ������)
 *     E ��    ��        ��(�����͕K���\�������. �����ꕶ��������int_width���I�[�o�[���邱�Ƃ�����)
 *
 *   �ڍא���:
 *   U : �����_������ʂ̕����񐔂��m����int_width�ɂȂ邱�Ƃ�ۏ؂���.
 *       ���̑���A���������̌�����int_width�𒴂��A���������t�������ꍇ�A
 *       �����𖳎�����(���������͎w�茅���ɂȂ�).
 *       IS_COMPACT �̎w��ɂ�����炸�A�K��compact�ƂȂ�.
 *
 *   S : �����_������ʂ̕����񐔂��m����int_width�ɂȂ邱�Ƃ�ۏ؂���.
 *       ���̑���A���������̌�����int_width�𒴂��A���������t�������ꍇ�A
 *       �����͕K���܂߁A���̕������ꌅ�]���ɂ���.
 *       int_width=1�̏ꍇ�A�������������݂̂Ƃ������P�[�X�����蓾��.
 *       int_width=0�̏ꍇ�A�������ȗ������.
 *       IS_COMPACT �̎w��ɂ�����炸�A�K��compact�ƂȂ�.
 *
 *   E : �����_������ʂ̕����񐔂��m����int_width��int_width+1�ɂȂ邱�Ƃ�ۏ؂���.
 *       �������t������Ȃ��ꍇ�A���������̌�����int_width�ɂȂ邱�Ƃ��ۏ؂����. 
 *       ���������̌�����int_width�𒴂��A���������t�������ꍇ�A
 *       �����ꕶ�����̃T�C�Y�����g������A�g�[�^���ł�int_width����I�[�o�[����.
 *
 *       is_sign_top �� true �ŕ��������݂���ꍇ�́A
 *         IS_COMPACT == true �̂Ƃ�:
 *           ���������Ȃ��Ƃ��͂ł��邾��int_width����-�����߂�悤�ɂ���.
 *           �� : space_ch=@, int_width == 5 �̂Ƃ�
 *             @@@12.3456
 *             -@@12.3456
 *             23456.0000
 *             -23456.0000
 *         IS_COMPACT == false �̂Ƃ�:
 *           ���������Ȃ��ꍇ�ł� int_width + 1 �����ŌŒ肷��.
 *           �� : space_ch=@, int_width == 5 �̂Ƃ�
 *             @@@12.3456
 *             -@@@12.3456
 *             23456.0000
 *             -23456.0000
 *
 *       int_width=0�̏ꍇ���A�������t�������.
 *
 * ans_num_shortage:
 *   IS_TRUNCATE �w��̂Ƃ��A�����̏�ʌ��������ǂꂾ������ꂽ���������l���i�[�����.
 *   �����Ȃ�΂��̐����������Ă���A0 �ȉ��Ȃ�Α������܂������Ȃ����Ƃ�����.
 *   ans_num_shortage or ans_sign_shortage �̂����ꂩ�� NULL ���w�肳�ꂽ�ꍇ�A
 *   ���̒l���擾���鏈���͍s���Ȃ�.
 *
 * ans_sign_shortage:
 *   IS_TRUNCATE �w��̂Ƃ��Adval �����̐��ɂ�������炸�������t������Ȃ������ꍇ�� true
 *   �����Ȃ���� false ���i�[�����.
 *   ans_num_shortage or ans_sign_shortage �̂����ꂩ�� NULL ���w�肳�ꂽ�ꍇ�A
 *   ���̒l���擾���鏈���͍s���Ȃ�.
 *
 * nan_callback_func
 *   NanCallBackFunc �錾�ɂ���R�����g���Q��.
 *
 * is_check_nan
 *   NanCallBackFunc �錾�ɂ���R�����g���Q��.
 *
 */
static char*
DoubleToStr_asDecF( char* p, double dval, bool term_nul,
		size_t int_width, size_t decimal_width,
		char space_ch, ZnkToStrFlags flags, char sign_policy,
		int* ans_num_shortage, bool* ans_sign_shortage,
		NanCallBackFunc nan_callback_func, bool is_check_nan )
{
	char* end;
	if( is_check_nan ){
		if( checkNanAndInf( &p, dval, term_nul, nan_callback_func ) ){ return p; }
	}

	{
		bool is_truncate   = Znk_IS_FLAG32(flags, ZnkToStr_e_IS_TRUNCATE);
		bool is_supplement = Znk_IS_FLAG32(flags, ZnkToStr_e_IS_SUPPLEMENT);
		/**
		 * dval ��艺�L�R�l�𓾂�.
		 * (������Inline_decompToBase10�ɂ��擾���ꂽ��͊�{�I�ɕω����Ȃ�.)
		 */
		bool   sign_bit   = false;
		double fraction10 = 0.0;
		int    exp_10     = 0;
		char   sign_ch;
		char*  begin;
		char*  point;
		Inline_decompToBase10( dval, &sign_bit, &fraction10, &exp_10 );
	
		sign_ch = getSignCh( sign_bit, (bool)(dval == 0), flags );
	
		/**
		 * Shortage ���v�Z�擾.
		 */
		if( ans_num_shortage != NULL && ans_sign_shortage != NULL ) {
			//if( sign_policy == 'A' || dval == 0.0 ){
			if( dval == 0.0 ){
				*ans_num_shortage  = 0;
				*ans_sign_shortage = false;
			} else {
				getShortage( ans_num_shortage, ans_sign_shortage,
						sign_ch, exp_10, int_width, sign_policy );
			}
		}
	
		/**
		 * sign_policy �ɂ��Abegin �� int_width �𒲐�.
		 * �܂��A���������Œ胂�[�h�̏ꍇ�A��ʂ�K�؂� space_ch �Ŗ��߂�.
		 * begin �͐��������̊J�n�ʒu(�����̎��̈ʒu)��������
		 * ��̏����ŕω�����\��������.
		 */
		begin = p;
		begin = adjust_bySignPolicy( begin, &int_width,
				exp_10, space_ch,
				sign_ch, sign_policy,
				flags,
				is_supplement, is_truncate );
	
		/**
		 * fraction10 ��^���A���݂�begin�ʒu�� �����Ȃ������̕�������쐬����.
		 * end �͕�����̏I�[�ʒu('\0'���i�[�����ׂ��ʒu)��������
		 * ��̏����ŕω�����\��������.
		 */
		{
			size_t taken_size = decimal_width;
			if( exp_10 > 0 ){
				taken_size += exp_10;
			}
			end = Fraction10ToStr_asDecFix( begin, fraction10, term_nul, taken_size );
		}
	
		/**
		 * �����_�̌��݈ʒu.
		 * ��̏����ŕω�����\��������.
		 */
		point = begin+1;
	
		/**
		 * �����Œ胂�[�h���ۂ�
		 */
		//bool is_fix_mode = (sign_policy != 'A');
	
		if( exp_10 >= 0 ){
			/**
			 * exp_10 �����̏ꍇ
			 * point���E�ւƈړ�����.
			 */
	
			/**
			 * �Ō���� exp_10�� '0' ��ǉ�.
			 */
			//end = fillChar( end, exp_10, '0' );
	
			/**
			 * ���������_���E��exp_10�����J��
			 */
			point = moveFloatingPointToRight( point, exp_10 );
	
			if( is_truncate ){
				/**
				 * �w�茅���̍ŏI����.
				 * point - begin �����������̌��݂̌���������.
				 * ���N�G�X�g���ꂽ int_width ����J�b�g���ׂ��T�C�Y������o��.
				 *
				 * cut_size �����̏ꍇ:
				 *   int_width=5�Ƃ��āA
				 *   1234567.89...�̂悤�ɁA���������� int_width ���z���Ă���ꍇ�ɋN����.
				 *
				 * cut_size ��0�̏ꍇ:
				 *   int_width=5�Ƃ��āA
				 *   12345.678...�̂悤�ɁA���������� int_width �ƒ��x�������ꍇ�ɋN����.
				 *   ���̏ꍇ�͍l�����Ȃ��Ă悢.
				 *
				 * cut_size �����̏ꍇ:
				 *   int_width=5�Ƃ��āA
				 *   00123.456...�̂悤��0������Ă���ꍇ�ɋN����.
				 *   ���̏ꍇ�͍l�����Ȃ��Ă悢.
				 */
				int cut_size = (int)( point - begin ) - (int)( int_width );
				if( cut_size > 0 ){
					end = removeFrontByte_withBeginFix( begin, end, cut_size );
				}
			}
		} else {
			/**
			 * exp_10 �����̏ꍇ
			 * �擪�� 0 ���t������Ȃ���Apoint�����ւƈړ�����.
			 */
	
			/**
			 * ���������_������ -exp_10-1 �����J��
			 * �����ɂ���������'0'�ɒu�����Ă���.
			 * �Ⴆ�΁A1.2345E-3 => 0.0012345 �ƕϊ������ꍇ�A
			 * ���ۂ̕��������_�̈ړ��ʂ�3�ɂȂ�̂����A
			 * �ȉ��̂悤�ɍŏI�񂾂��͓��ʏ��������邽�߁A
			 * �܂�3-1=2�񂾂����[�v�����Ă���`�ɂȂ�.
			 *
			 * ??1.2345
			 * ??.12345
			 * ?0.12345
			 *
			 * ?0.12345
			 * ?.012345
			 * 0.012345
			 *
			 * 0.012345
			 * .0012345
			 * �����ŁA�ŏI�I�ɐ���������t���Ȃ��ꍇ�͂��̂܂܏I������.
			 * ����������t����ꍇ�́Apoint�̍�����0�ɒu�����Ȃ���΂Ȃ�Ȃ�.
			 */
			size_t num = -exp_10-1;
			begin -= num; 
			while( num > 0 ){
				point = swapFloatingPoint( point );
				point = setZero_onLeftOfPoint( point );
				--num;
			}
	
			/**
			 * �ŏI��
			 * ���������_������ 1 �����J�ڂ����邾��.
			 * �Ƃ肠����������0�ɒu�����Ȃ�.
			 */
			point = swapFloatingPoint( point );
	
			if( !is_truncate || int_width > 0 ) {
				/**
				 * point�̍�����0�ɒu������.
				 */
				point = setZero_onLeftOfPoint( point );
			}
	
			end = point + decimal_width + 1;
		}
	
	
		if( term_nul ){ *end = '\0'; }
	}
	return end;
}


static char*
DoubleToStr_asDecG( char* p, double dval, bool term_nul,
		size_t int_width, size_t decimal_width, size_t exp_width,
		char space_ch, ZnkToStrFlags flags, char sign_policy,
		char* selected_method,
		NanCallBackFunc nan_callback_func, bool is_check_nan )
{
	if( is_check_nan ){
		if( checkNanAndInf( &p, dval, term_nul, nan_callback_func ) ){ return p; }
	}

	{
		/**
		 * dval ��艺�L�R�l�𓾂�.
		 * (������Inline_decompToBase10�ɂ��擾���ꂽ��͊�{�I�ɕω����Ȃ�.)
		 */
		bool   sign_bit   = false;
		double fraction10 = 0.0;
		int    exp_10     = 0;
		Inline_decompToBase10( dval, &sign_bit, &fraction10, &exp_10 );
	
		/**
		 * �ϊ������l�̎w�����A-4 ��菬�������A���x�ȏ�̏ꍇ�ɁAe �`�����g�p�����.
		 */
		if( exp_10 < -4 || ( exp_10 >= 0 && (size_t)(exp_10) >= decimal_width ) ){
			p = DoubleToStr_asDecE( p, dval, term_nul,
					decimal_width, exp_width, flags,
					NULL, false );
			if( selected_method != NULL ){ *selected_method = 'e'; }
		} else {
			p = DoubleToStr_asDecF( p, dval, term_nul,
					int_width, decimal_width,
					space_ch, flags, sign_policy,
					NULL, NULL, NULL, false );
			if( selected_method != NULL ){ *selected_method = 'f'; }
		}
	}
	return p;
}


/*****************************************************************************/


size_t
ZnkToStr_Double_getStrF( char* buf, size_t buf_size, double dval,
		size_t int_width, size_t decimal_width,
		char space_ch, ZnkToStrFlags flags, char sign_policy )
{
	if( buf_size < Znk_TOSTR_ENOUGH_BUFSIZE_FOR_DOUBLE_F ){
		return 0;
	}
	return DoubleToStr_asDecF( buf, dval, true, int_width, decimal_width,
				space_ch, flags, sign_policy,
				NULL, NULL, NULL, true ) - buf;
}
size_t
ZnkToStr_Double_getStrE( char* buf, size_t buf_size, double dval,
		size_t decimal_width, size_t exp_width, ZnkToStrFlags flags )
{
	if( buf_size < Znk_TOSTR_ENOUGH_BUFSIZE_FOR_DOUBLE_E ){
		return 0;
	}
	return DoubleToStr_asDecE( buf, dval, true,
			decimal_width, exp_width, flags,
			NULL, true ) - buf;
}
size_t
ZnkToStr_Double_getStrG( char* buf, size_t buf_size, double dval,
		size_t int_width, size_t decimal_width, size_t exp_width,
		char space_ch, ZnkToStrFlags flags, char sign_policy )
{
	if( buf_size < Znk_TOSTR_ENOUGH_BUFSIZE_FOR_DOUBLE_G ){
		return 0;
	}
	return DoubleToStr_asDecG( buf, dval, true,
			int_width, decimal_width, exp_width,
			space_ch, flags, sign_policy,
			NULL,
			NULL, true ) - buf;
}
size_t
ZnkToStr_Double_getStrG_Ex( char* buf, size_t buf_size, double dval,
		size_t int_width, size_t decimal_width, size_t exp_width,
		char space_ch, ZnkToStrFlags flags, char sign_policy,
		char* selected_method )
{
	if( buf_size < Znk_TOSTR_ENOUGH_BUFSIZE_FOR_DOUBLE_G ){
		return 0;
	}
	return DoubleToStr_asDecG( buf, dval, true,
			int_width, decimal_width, exp_width,
			space_ch, flags, sign_policy,
			selected_method,
			NULL, true ) - buf;
}
size_t
ZnkToStr_Double_getStrHexE( char* buf, size_t buf_size, double dval,
		size_t decimal_width, size_t exp_width, ZnkToStrFlags flags )
{
	if( buf_size < Znk_TOSTR_ENOUGH_BUFSIZE_FOR_DOUBLE_X ){
		return 0;
	}
	Znk_CLAMP_MAX( decimal_width, 16 );
	return DoubleToStr_asHexE( buf, dval, true,
			decimal_width, exp_width, flags,
			NULL, true ) - buf;
}

