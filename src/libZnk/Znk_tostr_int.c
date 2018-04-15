#include <Znk_tostr_int.h>
#include <Znk_base.h>
#include <Znk_s_base.h>
#include <Znk_def_util.h>
#include <Znk_stdc.h>
#include <assert.h>

#undef  UINT32_MAX
#define UINT32_MAX 4294967295UL

#undef  INT32_MAX
#define INT32_MAX 2147483647

#undef  INT32_MIN
#define INT32_MIN (-INT32_MAX - 1)

#undef  INT64_MAX
#define INT64_MAX INT64_C(9223372036854775807)

#undef  INT64_MIN
#define INT64_MIN (-INT64_MAX - 1)

/***
 * �R���p�C���ɂ����64bit������printf���f�B�t�@�C���͈قȂ邽�߁A
 * �}�N���ɂ���Ē��ۉ����Ă���.
 */
#define M_P_FMTI64 "%" Znk_PFMD_64 "d"
#define M_P_FMTU64 "%" Znk_PFMD_64 "u"
#define M_P_FMTX64 "%" Znk_PFMD_64 "x"
#define M_P_FMTO64 "%" Znk_PFMD_64 "o"

#if defined(Znk_C99_SUPPORTED) || ( defined(__linux__) && defined(Znk_RECENT_GNUC) )
#  define M_P_FMTZU "%zu"
#else
#  if   Znk_CPU_BIT == 32
#    define M_P_FMTZU "%u"
#  elif Znk_CPU_BIT == 64
#    define M_P_FMTZU "%" Znk_PFMD_64 "u"
#  else
#    define M_P_FMTZU "%u"
#  endif
#endif


/***
 * �f�[�^�T�C�Y�̊�{�����ɂ��ẮADOC_base.n3 ���Q��.
 *
 * ���ȉ��ł́A��т��� *p-- = val, *p++ = val �Ƃ����C�f�B�I����p���Ă���.
 *   (�܂�*p �ɑ��������ɃC���N�������g����.)
 */

/***
 * {UINT32_MAX�̌���, INT32_MIN��'-'������������, INT32_MAX�̌���}
 * �Ƃ������R�[�h����Ȃ�.
 * 2�i������36�i���܂�
 */
static size_t st_digit_num_table32[][3] = {
	{32, 32, 31}, // 2�i��
	{21, 20, 20},
	{16, 16, 16},
	{14, 14, 14},
	{13, 12, 12},
	{12, 12, 12},
	{11, 11, 11}, // 8�i��
	{11, 10, 10},
	{10, 10, 10}, // 10�i��
	{10, 9,  9},
	{9,  9,  9},
	{9,  9,  9},
	{9,  9,  9},
	{9,  8,  8},
	{8,  8,  8}, // 16�i��
	{8,  8,  8},
	{8,  8,  8},
	{8,  8,  8},
	{8,  8,  8},
	{8,  8,  8},
	{8,  7,  7},
	{8,  7,  7},
	{7,  7,  7},
	{7,  7,  7},
	{7,  7,  7},
	{7,  7,  7},
	{7,  7,  7},
	{7,  7,  7},
	{7,  7,  7},
	{7,  7,  7},
	{7,  7,  7},
	{7,  7,  7},
	{7,  7,  7},
	{7,  7,  7},
	{7,  6,  6},
};
/***
 * {UINT64_MAX�̌���, INT64_MIN��'-'������������, INT64_MAX�̌���}
 * �Ƃ������R�[�h����Ȃ�.
 * 2�i������36�i���܂�
 */
static size_t st_digit_num_table64[][3] = {
	{64, 64, 63}, // 2�i��
	{41, 40, 40},
	{32, 32, 32},
	{28, 28, 28},
	{25, 25, 25},
	{23, 23, 23},
	{22, 22, 21}, // 8�i��
	{21, 20, 20},
	{20, 19, 19}, // 10�i��
	{19, 19, 19},
	{18, 18, 18},
	{18, 18, 18},
	{17, 17, 17},
	{17, 17, 17},
	{16, 16, 16}, // 16�i��
	{16, 16, 16},
	{16, 16, 16},
	{16, 15, 15},
	{15, 15, 15},
	{15, 15, 15},
	{15, 15, 15},
	{15, 14, 14},
	{14, 14, 14},
	{14, 14, 14},
	{14, 14, 14},
	{14, 14, 14},
	{14, 14, 14},
	{14, 13, 13},
	{14, 13, 13},
	{13, 13, 13},
	{13, 13, 13},
	{13, 13, 13},
	{13, 13, 13},
	{13, 13, 13},
	{13, 13, 13},
};


Znk_INLINE size_t getDigitNum_UMAX32( size_t radix ){
	assert( radix >= 2 && radix <= 36 );
	return st_digit_num_table32[ radix-2 ][0];
}
Znk_INLINE size_t getDigitNum_IMIN32( size_t radix ){
	assert( radix >= 2 && radix <= 36 );
	return st_digit_num_table32[ radix-2 ][1];
}
Znk_INLINE size_t getDigitNum_IMAX32( size_t radix ){
	assert( radix >= 2 && radix <= 36 );
	return st_digit_num_table32[ radix-2 ][2];
}
Znk_INLINE size_t getDigitNum_UMAX64( size_t radix ){
	assert( radix >= 2 && radix <= 36 );
	return st_digit_num_table64[ radix-2 ][0];
}
Znk_INLINE size_t getDigitNum_IMIN64( size_t radix ){
	assert( radix >= 2 && radix <= 36 );
	return st_digit_num_table64[ radix-2 ][1];
}
Znk_INLINE size_t getDigitNum_IMAX64( size_t radix ){
	assert( radix >= 2 && radix <= 36 );
	return st_digit_num_table64[ radix-2 ][2];
}



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
 * �w��N��z��C���f�b�N�X�Ƃ��ė^����� 64bit 10^N ��������.
 * N�� 0�ȏ�19�ȉ��łȂ���΂Ȃ�Ȃ�.
 */
static const uint64_t st_pow10_table64[] = {
	UINT64_C(1),
	UINT64_C(10),
	UINT64_C(100),
	UINT64_C(1000),
	UINT64_C(10000),
	UINT64_C(100000),
	UINT64_C(1000000),
	UINT64_C(10000000),
	UINT64_C(100000000),
	UINT64_C(1000000000), //32bit�̏ꍇ�A�����܂�.
	UINT64_C(10000000000),
	UINT64_C(100000000000),
	UINT64_C(1000000000000),
	UINT64_C(10000000000000),
	UINT64_C(100000000000000),
	UINT64_C(1000000000000000),
	UINT64_C(10000000000000000),
	UINT64_C(100000000000000000),
	UINT64_C(1000000000000000000),
	UINT64_C(10000000000000000000), //64bit�̏ꍇ�A�����܂�
};


Znk_INLINE uint32_t getPow10U32( size_t exp )
{
	assert( exp < 10 );
	return st_pow10_table32[ exp ];
}
Znk_INLINE uint64_t getPow10U64( size_t exp )
{
	assert( exp < 20 );
	return st_pow10_table64[ exp ];
}




/***
 * @brief ��������Ōォ��擪�ɕ��וς���
 *
 * @param begin
 *   �ϊ�������̊J�n�ʒu�ɂ�����o�b�t�@��̃A�h���X.
 *   ���בւ��������񂪂��̈ʒu����o�b�t�@���g�ɏ㏑�������.
 *
 * @param last
 *   �ϊ�������̍ŏI����������ʒu�ɂ�����o�b�t�@��̃A�h���X.
 */
Znk_INLINE void
reverseStr(char* begin, char* last)
{
	char c;
	while(last > begin) {
		// swap *begin and *last
		c = *begin; *begin = *last; *last = c;

		++begin; --last;
	}
	return;
}



/***
 * ival �� 2^idx �{�������̂������v�Z.
 */
#define M_P_MLTP_2N( ival, idx ) ((ival)<<idx)
/***
 * ival �� 2^idx �Ŋ��������̂������v�Z.
 */
#define M_P_DIV_2N(  ival, idx ) ((ival)>>idx)


/***
 * 36�i���܂ŕ\�����邽�߂̐����e�[�u��.
 */
static const char st_digits_table_lower[] = "0123456789abcdefghijklmnopqrstuvwxyz";
static const char st_digits_table_upper[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

/***
 * table��idx�Ԗڂ̗v�f�����o�����߂̊ȈՃ}�N��.
 */
#define M_P_TABLE(table, idx) ((table)[ (size_t)(idx) ])




/*****************************************************************************/
/***
 * @brief
 * N�i���\�� �ėp�o�[�W����(�S�^�ɑΉ�).
 * unsigned ������radix�i��������ɔėp�I�ɕϊ�����.
 *
 * @param p 
 *   �ϊ���̐��l��������i�[����o�b�t�@�̊J�n�A�h���X
 *
 * @param uval 
 *   �ϊ����̐���
 *
 * @param radix
 *   ���i���\���ɂ��邩�������.
 *   2�ȏ�36�ȉ��łȂ���΂Ȃ�Ȃ�.
 *
 * @param is_lower
 *   ���ʂ��������ɂ��邩�ۂ�( ff or FF ��)
 *
 * @param T
 *   �K�� unsigned �^���w�肵�Ȃ���΂Ȃ�Ȃ�.
 *   (uint8_t, uint16_t, uint32_t, uint64_t ��)
 *   �����t����(int��)���w�肳�ꂽ�ꍇ�A���܂����삵�Ȃ�.
 *
 * @note
 * @code
 *   ������itoa�Ɠ����̊֐�(�����������̋@�\���������Ă���).
 *   atoi�ƈႢ�Aitoa �͕W���ł͑��݂��Ȃ�.
 *   itoa �� VC �ł� stdlib.h �Ɋ܂܂�Ă���.
 *   ���A���̊֐���VC��_itoa�Ɩ{���͓������������Ă���Ǝv���邪�A
 *   _itoa ��菭�������x��.
 *
 *   ���� radix �Ɋւ��ĕ⑫:
 *   2�i��,4�i��,8�i��,10�i��,16�i���ɂ��ẮA�ʓr�����A���S���Y����p�ӂ�������
 *   �悢�ƍl�����邽�߁A�ʏ�́A�����ȊO�̒l���w�肵�Ďg�����Ƃ��z�肳���.
 *   digits table �̃T�C�Y�̓s����A�ő�36�܂ł��T�|�[�g����.
 *   ����table�����p�ӂ���΁A���_�I�ɂ͍ő�l�͊��ł��傫���ݒ�ł���.
 *   ���Ȃ݂ɕW���֐��� strtol �ɗ^���� radix �͈̔͂����l�� 2 �ȏ� 36 �ȉ��ƂȂ��Ă���.
 * @endcode
 *
 */
Znk_INLINE char*
UIntToStr_generalFlex_32( char *p, uint32_t uval, bool term_nul, bool is_lower, size_t radix )
{
	const char* table = is_lower ?  st_digits_table_lower : st_digits_table_upper;
	char*       begin = p;
	uint32_t    div;
	/* �l�����ʂ̌����珇�ɕ����ɂ���. */
	do {
		/***
		 * Cygwin gcc �ł͂Ȃ��� % ��p�����ق�����������.
		 * VC, Linux gcc �ł� % ��p���Ȃ��ق�����������.
		 */
		div  = uval / (uint32_t)(radix);
		*p++ = M_P_TABLE(table, uval - div * radix);
		uval = div;
	} while ( uval );

	if( term_nul ){ *p = '\0'; }
	--p;
	
	/* ������̏�ʂ̌�����n�܂�悤�ɕ��בւ���B*/
	reverseStr( begin, p );
	++p;
	return p;
}
Znk_INLINE char*
UIntToStr_generalFlex_64( char *p, uint64_t uval, bool term_nul, bool is_lower, size_t radix )
{
	const char* table = is_lower ?  st_digits_table_lower : st_digits_table_upper;
	char*       begin = p;
	uint64_t    div;
	/* �l�����ʂ̌����珇�ɕ����ɂ���. */
	do {
		/***
		 * Cygwin gcc �ł͂Ȃ��� % ��p�����ق�����������.
		 * VC, Linux gcc �ł� % ��p���Ȃ��ق�����������.
		 */
		div  = uval / (uint64_t)(radix);
		*p++ = M_P_TABLE(table, uval - div * radix);
		uval = div;
	} while ( uval );

	if( term_nul ){ *p = '\0'; }
	--p;
	
	/* ������̏�ʂ̌�����n�܂�悤�ɕ��בւ���B*/
	reverseStr( begin, p );
	++p;
	return p;
}


Znk_INLINE char*
UIntToStr_generalFix_32( char *p, uint32_t uval, bool term_nul,
		size_t width, char space_ch, bool is_lower, size_t radix )
{
	const char* table = is_lower ?  st_digits_table_lower : st_digits_table_upper;
	char* end;
	p += width;
	end = p; /* '\0'������ׂ��ꏊ. */
	--p;

	if( uval == 0 ){
		/* ���ʏ������K�v. �K��'0'�̈ꕶ���͓���. */
		*p-- = '0'; --width;
	} else {
		uint32_t div;
		/* width > 0 ���� uval > 0 �ł������A�e���̒l�̎擾��������J��Ԃ�. */
		while( width && uval ){
			div  = uval / (uint32_t)(radix);
			*p-- = M_P_TABLE(table, uval - div * radix);
			uval = div;
			--width;
		}
	}
	/* p ���O�� �c���� width ���� space_ch ����. */
	while( width ){ *p-- = space_ch; --width; }

	if( term_nul ){ *end = '\0'; }
	return end;
}
Znk_INLINE char*
UIntToStr_generalFix_64( char *p, uint64_t uval, bool term_nul,
		size_t width, char space_ch, bool is_lower, size_t radix )
{
	const char* table = is_lower ?  st_digits_table_lower : st_digits_table_upper;
	char* end;
	p += width;
	end = p; /* '\0'������ׂ��ꏊ. */
	--p;

	if( uval == 0 ){
		/* ���ʏ������K�v. �K��'0'�̈ꕶ���͓���. */
		*p-- = '0'; --width;
	} else {
		uint64_t div;
		/* width > 0 ���� uval > 0 �ł������A�e���̒l�̎擾��������J��Ԃ�. */
		while( width && uval ){
			div  = uval / (uint64_t)(radix);
			*p-- = M_P_TABLE(table, uval - div * radix);
			uval = div;
			--width;
		}
	}
	/* p ���O�� �c���� width ���� space_ch ����. */
	while( width ){ *p-- = space_ch; --width; }

	if( term_nul ){ *end = '\0'; }
	return end;
}

/*
 * endof N�i���\�� �ėp�o�[�W����
 ***/
/*****************************************************************************/




/*****************************************************************************/
/***
 * 16�i���\�������o�[�W����
 * (�S�^�ɑΉ�)
 *
 * @brief
 *   unsigned ������16�i��������ɕϊ�����.
 *   �Œ蕝 or �L�k�̗������T�|�[�g����.
 *
 * @param
 * p: 
 *   �o�b�t�@�̊J�n�ʒu�������|�C���^.
 *   ���̊J�n�ʒu���猋�ʂ��o�b�t�@�֏㏑�������.
 *   �K�{�o�b�t�@�T�C�Y�� T �̎w��^�ɂ���ĈقȂ邪
 *   32bit�����Ȃ�� 8+1, 64bit�����Ȃ�� 16+1�̗̈悪�K�v�Ȃ��߁A
 *   32 byte �ȏオ���������.
 *
 * hex: 
 *   �ϊ��Ώۂ̐���
 *
 * width:
 *   �Œ茅�\���ɂ����錅��.
 *   �Ⴆ�� uint32_t �� 4byte�ł��邽�߁A���ׂă_���v����ɂ�8���K�v�ƂȂ�.
 *
 * is_lower:
 *   ���ʂ��������ɂ��邩�ۂ�( ff or FF ��)
 *
 * space_ch:
 *   �Œ茅�\���̎��ɏ�ʋ󔒂ɖ��߂�ׂ�����.
 *
 * T:
 *   �K�� unsigned �^���w�肵�Ȃ���΂Ȃ�Ȃ�.
 *   (uint8_t, uint16_t, uint32_t, uint64_t ��)
 *   ���ɕ����t����(int��)���w�肳�ꂽ�ꍇ�A���̐����󂯕t���邱�ƂɂȂ邪
 *   ���܂����삵�Ȃ�.
 *
 * @example:
 *   hex = 0xff, width = 8 �̂Ƃ��A���ʂ� 000000ff
 *   hex = 0xff, width = 0 �̂Ƃ��A���ʂ� ff
 *   hex = 0xff, width = 8, is_lower=false, space_ch=@ �̂Ƃ��A���ʂ� @@@@@@FF
 *
 * @note
 *   UIntToStr_general_* �����������̕�������.
 *   8bit�ȏ�̔C�ӂ�4*N bit �����Ŗ��Ȃ����������͂��ł���.
 */
Znk_INLINE char*
UIntToStr_asHexFlex_32( char* p, uint32_t hex, bool term_nul, bool is_lower )
{
	const char* table = is_lower ?  st_digits_table_lower : st_digits_table_upper;
	/***
	 * �V�t�gbit��(Bit Shift Size)�̏����l = (�ړ��������̏����l)*4 = (sizeof(uint32_t)*2-1)*4
	 * �����ł�sizeof(uint32_t)��0�ɂȂ邱�Ƃ͐�΂ɂȂ��Ɖ��肷�邽�߁Abss �� size_t �Ƃ��Ă���.
	 */
	size_t bss = M_P_MLTP_2N( sizeof(uint32_t), 1 ) - 1; /* 16�i���ł̌����ōl�����ꍇ�̈ړ��ʂ��܂�����. */
	bss <<= 2; /* bss��4�{���Abit�ł̈ړ��ʂ𓾂�. */
	/*
	 * �ȉ� bss �͕K�� 4 �̔{���ł���.
	 ***/

	/***
	 * hex�̏�ʌ����珇��0�łȂ����̂�����܂Œ��ׂ�. �������A�ŉ��ʌ��ɂ��Ă͒��ׂȂ�.
	 * �ŉ��ʌ��́A0 or ��0 �ɂ�����炸�Atable�Q�Ƃ���l�𓾂邽�߂ł���.
	 * ( bss �� 0 �ɂȂ������_�Ń��[�v���I�����邽�߁A�ŉ��ʌ��ɂ��Ă͒��ׂĂ��Ȃ��`�ɂȂ� )
	 */
	while( bss && ((hex>>bss) & 0xF) == 0 ){
		/* 4bit���V�t�g�ʂ����炷. */
		bss -= 4;
	}

	/* �ŉ��ʌ�����Ŕ�[���̌����e�[�u���Q�Ƃ���. */
	while( bss ){
		*p++ = M_P_TABLE( table, (hex>>bss) & 0xF );
		/* 4bit���V�t�g�ʂ����炷. */
		bss -= 4;
	}
	/* �ŉ��ʌ��͕K���e�[�u���Q�Ƃ���. */
	*p++ = M_P_TABLE( table, hex & 0xF );

	if( term_nul ){ *p = '\0'; }
	return p;
}
Znk_INLINE char*
UIntToStr_asHexFlex_64( char* p, uint64_t hex, bool term_nul, bool is_lower )
{
	const char* table = is_lower ?  st_digits_table_lower : st_digits_table_upper;
	/***
	 * �V�t�gbit��(Bit Shift Size)�̏����l = (�ړ��������̏����l)*4 = (sizeof(uint64_t)*2-1)*4
	 * �����ł�sizeof(uint64_t)��0�ɂȂ邱�Ƃ͐�΂ɂȂ��Ɖ��肷�邽�߁Abss �� size_t �Ƃ��Ă���.
	 */
	size_t bss = M_P_MLTP_2N( sizeof(uint64_t), 1 ) - 1; /* 16�i���ł̌����ōl�����ꍇ�̈ړ��ʂ��܂�����. */
	bss <<= 2; /* bss��4�{���Abit�ł̈ړ��ʂ𓾂�. */
	/*
	 * �ȉ� bss �͕K�� 4 �̔{���ł���.
	 ***/

	/***
	 * hex�̏�ʌ����珇��0�łȂ����̂�����܂Œ��ׂ�. �������A�ŉ��ʌ��ɂ��Ă͒��ׂȂ�.
	 * �ŉ��ʌ��́A0 or ��0 �ɂ�����炸�Atable�Q�Ƃ���l�𓾂邽�߂ł���.
	 * ( bss �� 0 �ɂȂ������_�Ń��[�v���I�����邽�߁A�ŉ��ʌ��ɂ��Ă͒��ׂĂ��Ȃ��`�ɂȂ� )
	 */
	while( bss && ((hex>>bss) & 0xF) == 0 ){
		/* 4bit���V�t�g�ʂ����炷. */
		bss -= 4;
	}

	/* �ŉ��ʌ�����Ŕ�[���̌����e�[�u���Q�Ƃ���. */
	while( bss ){
		*p++ = M_P_TABLE( table, (hex>>bss) & 0xF );
		/* 4bit���V�t�g�ʂ����炷. */
		bss -= 4;
	}
	/* �ŉ��ʌ��͕K���e�[�u���Q�Ƃ���. */
	*p++ = M_P_TABLE( table, hex & 0xF );

	if( term_nul ){ *p = '\0'; }
	return p;
}


Znk_INLINE char*
UIntToStr_asHexFix_32( char* p, uint32_t hex, bool term_nul, size_t width, char space_ch, bool is_lower )
{
	const char* table = is_lower ?  st_digits_table_lower : st_digits_table_upper;
	char* end;
	p += width;
	end = p; /* '\0'������ׂ��ꏊ. */

	/* width == 0 ��^�����ꍇ�́A��������؂Ȃ��󕶎����ݒ肳�����̂Ƃ���.  */
	if( width == 0 ){
		if( term_nul ){ *end = '\0'; }
		return end;
	}

	--p;
	if( hex == 0 ){
		/* hex == 0 �̏ꍇ�͓��ʏ������K�v. */
		*p-- = '0'; --width;
	} else {
		while( width && hex ){
			*p-- = M_P_TABLE( table, hex & 0x0F );
			hex >>= 4;
			--width;
		}
	}
	while( width ){ *p-- = space_ch; --width; }
	if( term_nul ){ *end = '\0'; }
	return end;
}
Znk_INLINE char*
UIntToStr_asHexFix_64( char* p, uint64_t hex, bool term_nul, size_t width, char space_ch, bool is_lower )
{
	const char* table = is_lower ?  st_digits_table_lower : st_digits_table_upper;
	char* end;
	p += width;
	end = p; /* '\0'������ׂ��ꏊ. */

	/* width == 0 ��^�����ꍇ�́A��������؂Ȃ��󕶎����ݒ肳�����̂Ƃ���.  */
	if( width == 0 ){
		if( term_nul ){ *end = '\0'; }
		return end;
	}

	--p;
	if( hex == 0 ){
		/* hex == 0 �̏ꍇ�͓��ʏ������K�v. */
		*p-- = '0'; --width;
	} else {
		while( width && hex ){
			*p-- = M_P_TABLE( table, hex & 0x0F );
			hex >>= 4;
			--width;
		}
	}
	while( width ){ *p-- = space_ch; --width; }
	if( term_nul ){ *end = '\0'; }
	return end;
}


/*
 * endof 16�i���\�������o�[�W����
 ***/
/*****************************************************************************/




/*****************************************************************************/
/***
 * 10�i���\�������o�[�W����
 * uint32_t/uint64_t �Ɍ���.
 */

/***
 * 2���܂ł̐����̕������100�e�[�u���Ƃ��ėp�ӂ�������.
 * (�������̂��߁A������2�����Ƃɍs���ɂ�����A���̂悤�ȃe�[�u�����K�v�ɂȂ�)
 */
static const char* st_dec_digit2_table[] = {
	"00", "01", "02", "03", "04", "05", "06", "07", "08", "09",
	"10", "11", "12", "13", "14", "15", "16", "17", "18", "19",
	"20", "21", "22", "23", "24", "25", "26", "27", "28", "29",
	"30", "31", "32", "33", "34", "35", "36", "37", "38", "39",
	"40", "41", "42", "43", "44", "45", "46", "47", "48", "49",
	"50", "51", "52", "53", "54", "55", "56", "57", "58", "59",
	"60", "61", "62", "63", "64", "65", "66", "67", "68", "69",
	"70", "71", "72", "73", "74", "75", "76", "77", "78", "79",
	"80", "81", "82", "83", "84", "85", "86", "87", "88", "89",
	"90", "91", "92", "93", "94", "95", "96", "97", "98", "99",
};

/***
 * uval�������e�[�u���v�f�̉E�ꌅ���̕����𕶎���p�֑O����ǉ�����.
 */
#define M_P_ADD1( p, uval ) {\
		*(p)++ = st_dec_digit2_table[ uval ][1]; \
}

/***
 * uval�������e�[�u���v�f�Q�����̕����𕶎���p�֑O����ǉ�����.
 */
#define M_P_ADD2( p, uval ) {\
		*(p)++ = st_dec_digit2_table[ uval ][0]; \
		*(p)++ = st_dec_digit2_table[ uval ][1]; \
}

/***
 * uval1�Auval2�������e�[�u���v�f�S�����̕����𕶎���p�֑O����ǉ�����.
 */
#define M_P_ADD4( p, uval1, uval2 ) {\
		M_P_ADD2( p, uval1 ) \
		M_P_ADD2( p, uval2 ) \
}

/***
 * uval�������e�[�u���v�f�̕����𕶎���p�֒ǉ�����.
 * �������Auval���P���̐����̏ꍇ�́A�v�f�̉E�ꌅ�����A
 * uval���Q���̐����̏ꍇ�́A�v�f�Q���������ǉ�����.
 */
#define M_P_ADD2_FLEX( p, uval ) \
	switch( uval ){ \
	case 0: case 1: case 2: case 3: case 4: \
	case 5: case 6: case 7: case 8: case 9: \
		M_P_ADD1( (p), (uval) ) \
		break; \
	default: \
		M_P_ADD2( (p), (uval) ) \
		break; \
	} \


/***
 * uval�������e�[�u���v�f�̉E�ꌅ���̕����𕶎���p�֌�납��ǉ�����.
 */
#define M_P_ADD1_RV( p, uval ) {\
		*(p)-- = st_dec_digit2_table[ uval ][1]; \
}

/***
 * uval�������e�[�u���v�f�Q�����̕����𕶎���p�֌�납��ǉ�����.
 */
#define M_P_ADD2_RV( p, uval ) {\
		*(p)-- = st_dec_digit2_table[ uval ][1]; \
		*(p)-- = st_dec_digit2_table[ uval ][0]; \
}

/***
 * uval1�Auval2�������e�[�u���v�f�S�����̕����𕶎���p�֌�납��ǉ�����.
 */
#define M_P_ADD4_RV( p, uval1, uval2 ) {\
		M_P_ADD2_RV( p, uval1 ) \
		M_P_ADD2_RV( p, uval2 ) \
}



/***
 * Move Upper
 *
 * �s���Ă��鏈���𖾉��ȋ[���R�[�h�Ƃ��ĕ\������ƈȉ��ɂȂ�.
 * {
 *   upper = uval / base; // �����擾
 *   uval  = uval % base; // ��]���擾
 * }
 *
 * base �����̗ݏ�(= B^N )�ł���Ƃ���΁A
 * ����͂������� uval �̑�N�ʂ���ʂ̌��� upper �ֈړ������悤�Ɍ����邽�߁A
 * MOVE �Ƃ����l�[�~���O�ɂ��Ă���. uval �ɂ� ��N���݂̂��c��.
 *
 * ��:
 *   uval = 12345(10�i), base = 10^2 = 100(10�i)  �̂Ƃ� upper = 123, uval=45
 *   uval = 12345(10�i), base = 10^3 = 1000(10�i) �̂Ƃ� upper = 12,  uval=345
 *   uval = 0x37BF(16�i), base = 0x100(16�i) �̂Ƃ�  upper = 0x37,  uval=0xBF
 *   uval = 0x37BF(16�i), base = 0x1000(16�i) �̂Ƃ� upper = 0x3,   uval=0x7BF
 *
 * �������A16,8,4,2�i���̏ꍇ�A����������ƍ����ȕ��@���l�����邽�߁A
 * ����͒ʏ�͎g�p���Ȃ�.
 */
#define M_P_MOVE_UPPER_BASE( upper, uval, base ) {\
		(upper) = (uval) / (base); \
		(uval) -= (upper) * (base); \
}


/***
 * @brief
 * M_P_MOVE_UPPER_BASE �� 10�i�W�J�Ŏg�p����Ƃ��ɁA
 * base �̑ւ��� ��10�ɑ΂���w��idx���w�肷�邱�Ƃ��ł���.
 * ���̂Ƃ��Aidx��uval�Ɏc�錅���ɂ�������.
 *
 * uval��32bit unsigned int �ł���ꍇ�ɂ̂ݎg�p�ł���.
 *
 * @note:
 * (uval) / (st_pow10_table32[idx]) �ɂ����
 * uval �� �� idx+1 �ʈȏ�̌����擾���邱�ƂɂȂ�.
 */
#define M_P_MOVE_UPPER_DEC32( upper, uval, idx )  M_P_MOVE_UPPER_BASE( (upper), (uval), st_pow10_table32[idx] )


/***
 * uval �ɂ����� 10�i������ base ���ȏ�̌����J�b�g���Č��ʂ� uval�ɏ㏑������.
 * �܂��A���̏������s��ꂽ�ꍇ�Ais_cut �t���O�� true ���ݒ肳���.
 * 10�i������base�������Ȃ牽�����Ȃ�.
 */
#define M_P_CUT_LARGE_DIGIT( uval, base, is_cut ) if( (uval) >= st_pow10_table32[base] ){\
	(uval) %= st_pow10_table32[base];\
	(is_cut) = true;\
}


/***
 * 4���̐������ꊇ�擾�o�^.
 * 4�������傫��������uval���w�肵�Ă͂Ȃ�Ȃ�.
 * 4�������̌�����uval���w�肳�ꂽ�ꍇ�A��ʂɂ�'0'���i�[�����.
 */
Znk_INLINE char* 
UInt32ToStr_asDec_Digit_Fix4( char* p, uint32_t uval )
{
	/***
	 * �����̍������̂��߂Ɉꎞ�I�Ɋi�[����0�ȏ�99�ȉ��̐���.
	 */
	uint32_t upper_2dig = 0;

	/***
	 * ��4,3�ʂ��擾�o�^.
	 */
	M_P_MOVE_UPPER_DEC32( upper_2dig, uval, 2 )
	M_P_ADD2( p, upper_2dig )

	/***
	 * ��2,1�ʂ�o�^.
	 */
	M_P_ADD2( p, uval )

	return p;
}

/***
 * 6���̐������ꊇ�擾�o�^.
 * 6�������傫��������uval���w�肵�Ă͂Ȃ�Ȃ�.
 * 6�������̌�����uval���w�肳�ꂽ�ꍇ�A��ʂɂ�'0'���i�[�����.
 */
Znk_INLINE char*
UInt32ToStr_asDec_Digit_Fix6( char* p, uint32_t uval )
{
	/***
	 * �����̍������̂��߂Ɉꎞ�I�Ɋi�[����0�ȏ�99�ȉ��̐���.
	 */
	uint32_t upper_2dig = 0;

	/***
	 * ��6,5�ʂ��擾�o�^.
	 */
	M_P_MOVE_UPPER_DEC32( upper_2dig, uval, 4 )
	M_P_ADD2( p, upper_2dig )

	/***
	 * ��4,3�ʂ��擾�o�^.
	 */
	M_P_MOVE_UPPER_DEC32( upper_2dig, uval, 2 )
	M_P_ADD2( p, upper_2dig )

	/***
	 * ��2,1�ʂ�o�^.
	 */
	M_P_ADD2( p, uval )

	return p;
}


/***
 * uval �� 4294000000(32bit�ł�UINT32_MAX�����̂�����ɂ���) �ȏ�ł���ꍇ�A
 * �����̂��ߓ��ʂɏ���.
 */
Znk_INLINE char*
processUInt32MaxBound4294( char* p, uint32_t uval )
{
	*p++ = '4'; *p++ = '2'; *p++ = '9'; *p++ = '4';
	/***
	 * ��6���ɂ���.
	 */
	uval -= 4294000000U;

	if( uval >= 967200U ){
		*p++ = '9'; *p++ = '6'; *p++ = '7'; *p++ = '2';
		/***
		 * ��2���ɂ���.
		 */
		uval -= 967200U;

		/***
		 * ��2,1�ʂ�o�^.
		 */
		M_P_ADD2( p, uval )

	} else if( uval >= 960000U ){
		*p++ = '9'; *p++ = '6';
		/***
		 * ��4���ɂ���.
		 */
		uval -= 960000U;

		/***
		 * ��4�����ꊇ�擾�o�^.
		 */
		p = UInt32ToStr_asDec_Digit_Fix4( p, uval );

	} else {
		/***
		 * ��6���̈ꊇ�擾�o�^.
		 */
		p = UInt32ToStr_asDec_Digit_Fix6( p, uval );
	}
	return p;
}


/***
 * 9���ȏ゠��ꍇ(1���ȏ�)�̏���.
 */
Znk_INLINE char*
UInt32ToStr_asDec_DigitOver9( char* p, uint32_t uval )
{
	/***
	 * UINT32_MAX = 4294967295 �𗘗p������������
	 * overflow���N�����Ƃ��A���̂�������ӂ̒l�͕p�ɂɎQ�Ƃ���邽�߁A
	 * ��������}��.
	 */
	if( uval >= 4294000000U ){
		return processUInt32MaxBound4294( p, uval );
	} else {
		uint32_t upper_2dig = 0;
	
		/***
		 * ��10,9�ʂ��擾�o�^.
		 * (��10�ʂ͑��݂��Ȃ��\��������)
		 */
		M_P_MOVE_UPPER_DEC32( upper_2dig, uval, 8 )
		M_P_ADD2_FLEX( p, upper_2dig )
	
		/***
		 * ��8,7�ʂ��擾�o�^.
		 */
		M_P_MOVE_UPPER_DEC32( upper_2dig, uval, 6 )
		M_P_ADD2( p, upper_2dig )
	
		/***
		 * ��6���̈ꊇ�擾�o�^.
		 */
		p = UInt32ToStr_asDec_Digit_Fix6( p, uval );
	}
	return p;
}


/***
 * @brief
 * uval ��10�i���Ƃ݂Ȃ��ĕ�����ɕϊ�����.
 * �Œ蕝�ł͂Ȃ��A�����ɉ����ĕ����񂪐L�k�擾�����.
 *
 * 2���P�ʂŃe�[�u���Q�Ƃ��Ă��邽�߁A������itoa�̎������͂��Ȃ荂��.
 *
 * @param
 * p:
 *   �o�b�t�@�̊J�n�ʒu�������|�C���^.
 *   ���̊J�n�ʒu���猋�ʂ��o�b�t�@�֏㏑�������.
 *   �o�b�t�@�T�C�Y�� 16 byte �ȏ��K�{�Ƃ���.
 *
 * uval:
 *   �ϊ��ΏۂƂȂ�32bit unsigned ����.
 *
 * term_nul:
 *   �Ō��'\0'�I�[���邩�ۂ�.
 *
 * @return
 *   p �̓o�b�t�@���𑖍�����J�[�\���Ƃ��ē��삵�A�ŏI�I��'\0'������ׂ�
 *   �ꏊ�ɓ��B����. ���̍ŏI�I�� p ��߂�l�Ƃ��ĕԂ�.
 *
 *   ���̂悤�ɂ��āA���ʕ�����('\0'�����͊܂܂Ȃ�)�̒�����
 *   �Ăяo�������瓱�o���邱�Ƃ��ł���.
 *
 *   size_t str_leng = UInt32ToStr_asDecFlex( buf, uval, term_nul ) - buf;
 *
 * @throw
 * @assert
 *
 * @note
 *   ������32bit�����p�ɍ��������Ă���.
 *   UIntToStr_general_T �����������̕�������.
 */
Znk_INLINE char*
UInt32ToStr_asDecFlex( char* p, uint32_t uval, bool term_nul )
{
	/*
	 * 1-2��(100����)
	 */
	if( uval < st_pow10_table32[2] ){
		M_P_ADD2_FLEX( p, uval )

		if( term_nul ){ *p='\0'; }
		return p;
	} else {
		/*
		 * �����̍������̂��߂Ɉꎞ�I�Ɋi�[����0�ȏ�99�ȉ��̐���.
		 */
		uint32_t upper_2dig = 0;
	
		/*
		 * 3-4��(1������)
		 */
		if( uval < st_pow10_table32[4] ){
			/*
			 * ��4,3�ʂ��擾�o�^.
			 */
			M_P_MOVE_UPPER_DEC32( upper_2dig, uval, 2 )
			M_P_ADD2_FLEX( p, upper_2dig )
	
			/*
			 * ��2,1�ʂ�o�^.
			 */
			M_P_ADD2( p, uval )
	
			if( term_nul ){ *p='\0'; }
			return p;
		}
	
		/*
		 * 5-6��(1���ȏ�100������)
		 */
		if( uval < st_pow10_table32[6] ){
			/*
			 * ��6,5�ʂ��擾�o�^.
			 * (��6�ʂ͑��݂��Ȃ��\��������)
			 */
			M_P_MOVE_UPPER_DEC32( upper_2dig, uval, 4 )
			M_P_ADD2_FLEX( p, upper_2dig )
		
			/*
			 * ��4�����ꊇ�擾�o�^.
			 */
			p = UInt32ToStr_asDec_Digit_Fix4( p, uval );
	
			if( term_nul ){ *p='\0'; }
			return p;
		}
	
		/*
		 * 7-8�� (100���ȏ�1������)
		 */
		if( uval < st_pow10_table32[8] ){
			/*
			 * ��8,7�ʂ��擾�o�^.
			 * (��8�ʂ͑��݂��Ȃ��\��������)
			 */
			M_P_MOVE_UPPER_DEC32( upper_2dig, uval, 6 )
			M_P_ADD2_FLEX( p, upper_2dig )
		
			/*
			 * ��6�����ꊇ�擾�o�^.
			 */
			p = UInt32ToStr_asDec_Digit_Fix6( p, uval );
	
			if( term_nul ){ *p='\0'; }
			return p;
		}
	
		/*
		 * 1���ȏ�(32bit���̏ꍇ�A1�`42��9�疜���x).
		 */
		p = UInt32ToStr_asDec_DigitOver9( p, uval );
	
		if( term_nul ){ *p='\0'; }
	}
	return p;
}


/***
 * @brief
 * uval ��10�i���Ƃ݂Ȃ��ĕ�����ɕϊ�����.
 * �Œ蕝���w�肵�A���̕��ɖ����Ȃ��ꍇ�� space_ch �����Ŗ��߁A
 * ���̕��𒴂���ꍇ�͏�ʂ̌���؂�̂Ă�.
 *
 * 2���P�ʂŃe�[�u���Q�Ƃ��Ă��邽�߁A������itoa�̎������͂��Ȃ荂��.
 *
 * @param
 * p: 
 *   �o�b�t�@�̊J�n�ʒu�������|�C���^.
 *   ���̊J�n�ʒu���猋�ʂ��o�b�t�@�֏㏑�������.
 *   �o�b�t�@�T�C�Y�� 16 byte �ȏ��K�{�Ƃ���.
 *
 * uval: 
 *   �ϊ��ΏۂƂȂ�32bit unsigned ����.
 *
 * term_nul:
 *   �Ō��'\0'�I�[���邩�ۂ�.
 *
 * width:
 *   �i�[����錋�ʕ�����̍ő啝.
 *   �K�� 1 �ȏ� 10 �ȉ����w�肵�Ȃ���΂Ȃ�Ȃ�.
 *
 *   �����鐔���̌��������̕����ɖ����Ȃ��ꍇ�͍����� space_ch �Ŏw�肵��
 *   ���������߂���.
 *   ��:
 *     uval = 21, width=4, space_ch='@' �̏ꍇ�A���ʂ� @@21
 *     uval = 0,  width=4, space_ch='@' �̏ꍇ�A���ʂ� @@@0
 *
 *   �����鐔���̌��������̕������z����ꍇ�͏�ʂ̌��͐؂�̂Ă���. 
 *   ��:
 *     uval = 12345, width=4, space_ch='@' �̏ꍇ�A���ʂ� 2345
 *
 *   �l�����ׂ�����ȃP�[�X�Ƃ��āA�ŏ�ʂ���0���A�����ďo������ꍇ������.
 *   ���̂Ƃ��Aspace_ch �Ƃ��� '0' �ȊO���w�肳��Ă��Ă��A�K��
 *   ���� 0 �̘A�������ɂ� '0' �����߂���.
 *   (�I�[�o�[�t���[�𖾊m�ɂ��邽�߁A���̂悤�Ȏd�l�ɂ��Ă���)
 *   ��:
 *     uval = 10000, width=4, space_ch='@' �̏ꍇ�A���ʂ� 0000
 *
 * space_ch:
 *   width�Ŏw�肵�������ɖ����Ȃ��Ƃ��A�ŏ�ʂ���̋󔒂̕�����
 *   ���� space_ch �Ŗ��߂�( space_ch �͑��̏ꍇ '0' or ' ' �Ǝv���� ).
 *   width �ɂ��Ă��Q�Ƃ̂���.
 *
 * @return
 *   �ŏI�I��'\0'������ׂ��ꏊ�������A�h���X p + width ��߂�l�Ƃ��ĕԂ�.
 *   (���̈ʒu��'\0'���i�[����Ă��邩�ۂ��� term_nul �̎w��ɂ��)
 *
 *   ���̂悤�ɂ��āA���ʕ�����('\0'�����͊܂܂Ȃ�)�̒�����
 *   �Ăяo�������瓱�o���邱�Ƃ��ł���.
 *
 *   size_t str_leng = buf + width;
 *
 * @assert
 *   width �ňُ�l���^����ꂽ�ꍇ.
 *
 * @throw
 *
 * @note
 *   ������32bit�����p�ɍ��������Ă���.
 *   UIntToStr_general_T �����������̕�������.
 */
Znk_INLINE char*
UInt32ToStr_asDecFix( char* p, uint32_t uval, bool term_nul, size_t width, char space_ch )
{
	/***
	 * �����菇�̉��.
	 *  uval ����������2���Â������čl����.
	 *  �ȉ��� width=5�̏ꍇ�ł���.
	 *
	 *  uval = 00123 =
	 *  	00000 +
	 *  	 0100 +
	 *  	   23
	 *  uval = 01234 =
	 *  	00000 +
	 *  	 1230 +
	 *  	   34
	 *  uval = 12345 =
	 *  	10000 +
	 *  	 2300 +
	 *  	   45
	 *  uval = 123456 => 23456
	 *  	 20000 +
	 *  	  3400 +
	 *  	    56
	 *
	 *  �ȉ��͎��ۂ̃R�[�h��(width=5�̏ꍇ)�ł���.
	 *
	 *  // width�����z����ꍇ�A�܂� M_P_CUT_LARGE_DIGIT �ŗ]�v�ȏ�ʌ����J�b�g.
	 *  // �J�b�g���s�������ۂ��� is_cut_large_digit �t���O�ɕۑ����Ă���.
	 *  // (���̃t���O�́A���space_ch�Ŗ��߂鏈���ŕK�v�ƂȂ�)
	 *  M_P_CUT_LARGE_DIGIT( uval, width, is_cut_large_digit )
	 *  
	 *  // uval �� ��4�ʂ���̌�����Ȃ�V���������� dig[0] �Ɉړ�.
	 *  // (width=5�̂��߁Adig[0]��99�ȉ��ł��邱�Ƃ��ۏ؂����. uval�͉�4�����c��)
	 *  M_P_MOVE_UPPER_DEC32( dig[0], uval, 4 )
	 *
	 *  // uval �� ��2�ʂ���̌�����Ȃ�V���������� dig[1] �Ɉړ�.
	 *  // (uval��4���ȉ��̂��߁Adig[1]��99�ȉ��ł��邱�Ƃ��ۏ؂����. uval�͉�2�����c��)
	 *  M_P_MOVE_UPPER_DEC32( dig[1], uval, 2 )
	 *  
	 *  // uval, dig[1], dig[0] �ɂ�0�ȏ�99�ȉ��̐������i�[����Ă���A
	 *  // �����table�̃C���f�b�N�X�ɗ^���ĊY�������𓾂āAp ��
	 *  // �t����(�z��̊J�n�ʒu�̕���)�Ɋi�[����.
	 *  M_P_ADD4_RV( p, uval, dig[1] )
	 *  M_P_ADD1_RV( p, dig[0] )
	 */

	bool is_cut_large_digit = false;
	uint32_t dig[4];
	char* end;

	/*
	 * �擾���ʂ�width�Ŏw�肳�ꂽ�����ŌŒ�.
	 */
	p += width;
	end = p; /* '\0'���i�[�����ׂ��ʒu. */
	--p;

	switch( width ){
	case 1:
		/*
		 * is_cut_large_digit �� �擾����K�v�͂Ȃ�.
		 * width=1�̂Ƃ��Aspace_ch �Ŗ��߂��邱�Ƃ͂��蓾�Ȃ�����.
		 */
		uval %= 10;
		M_P_ADD1_RV( p, uval )
		break;
	case 2:
	{
		M_P_CUT_LARGE_DIGIT( uval, width, is_cut_large_digit )
		M_P_ADD2_RV( p, uval )
		break;
	}
	case 3:
	{
		M_P_CUT_LARGE_DIGIT( uval, width, is_cut_large_digit )

		M_P_MOVE_UPPER_DEC32( dig[0], uval, 2 )

		M_P_ADD2_RV( p, uval )
		M_P_ADD1_RV( p, dig[0] )
		break;
	}
	case 4:
	{
		M_P_CUT_LARGE_DIGIT( uval, width, is_cut_large_digit )

		M_P_MOVE_UPPER_DEC32( dig[0], uval, 2 )

		M_P_ADD2_RV( p, uval )
		M_P_ADD2_RV( p, dig[0] )

		break;
	}
	case 5:
		/*
		 * 4���ȉ��̏ꍇ�A��薳�ʂ̂Ȃ����������s����悤�ɂ��Ă���.
		 * �ȉ�����铯�l�̕���́A���l�̈Ӑ}������.
		 */
		if( uval < st_pow10_table32[4] ){
			M_P_MOVE_UPPER_DEC32( dig[0], uval, 2 )
			M_P_ADD4_RV( p, uval, dig[0] )

			*p-- = '0';

		} else {
			M_P_CUT_LARGE_DIGIT( uval, width, is_cut_large_digit )

			M_P_MOVE_UPPER_DEC32( dig[0], uval, 4 )
			M_P_MOVE_UPPER_DEC32( dig[1], uval, 2 )

			M_P_ADD4_RV( p, uval, dig[1] )
			M_P_ADD1_RV( p, dig[0] )
		}

		break;
	case 6:
		if( uval < st_pow10_table32[4] ){
			M_P_MOVE_UPPER_DEC32( dig[0], uval, 2 )
			M_P_ADD4_RV( p, uval, dig[0] )

			*p-- = '0'; *p-- = '0';

		} else {
			M_P_CUT_LARGE_DIGIT( uval, width, is_cut_large_digit )

			M_P_MOVE_UPPER_DEC32( dig[0], uval, 4 )
			M_P_MOVE_UPPER_DEC32( dig[1], uval, 2 )

			M_P_ADD4_RV( p, uval, dig[1] )
			M_P_ADD2_RV( p, dig[0] )
		}
		break;
	case 7:
		if( uval < st_pow10_table32[4] ){
			M_P_MOVE_UPPER_DEC32( dig[0], uval, 2 )
			M_P_ADD4_RV( p, uval, dig[0] )

			*p-- = '0'; *p-- = '0';
			*p-- = '0';

		} else {
			M_P_CUT_LARGE_DIGIT( uval, width, is_cut_large_digit )

			M_P_MOVE_UPPER_DEC32( dig[0], uval, 6 )
			M_P_MOVE_UPPER_DEC32( dig[1], uval, 4 )
			M_P_MOVE_UPPER_DEC32( dig[2], uval, 2 )

			M_P_ADD4_RV( p, uval, dig[2] )
			M_P_ADD2_RV( p, dig[1] )
			M_P_ADD1_RV( p, dig[0] )
		}
		break;
	case 8:
		if( uval < st_pow10_table32[4] ){
			M_P_MOVE_UPPER_DEC32( dig[0], uval, 2 )
			M_P_ADD4_RV( p, uval, dig[0] )

			*p-- = '0'; *p-- = '0';
			*p-- = '0'; *p-- = '0';

		} else {
			M_P_CUT_LARGE_DIGIT( uval, width, is_cut_large_digit )

			M_P_MOVE_UPPER_DEC32( dig[0], uval, 6 )
			M_P_MOVE_UPPER_DEC32( dig[1], uval, 4 )
			M_P_MOVE_UPPER_DEC32( dig[2], uval, 2 )

			M_P_ADD4_RV( p, uval,   dig[2] )
			M_P_ADD4_RV( p, dig[1], dig[0] )
		}
		break;
	case 9:
		if( uval < st_pow10_table32[4] ){
			M_P_MOVE_UPPER_DEC32( dig[0], uval, 2 )
			M_P_ADD4_RV( p, uval, dig[0] )

			*p-- = '0'; *p-- = '0';
			*p-- = '0'; *p-- = '0';
			*p-- = '0';

		} else if( uval < st_pow10_table32[8] ){
			M_P_MOVE_UPPER_DEC32( dig[0], uval, 6 )
			M_P_MOVE_UPPER_DEC32( dig[1], uval, 4 )
			M_P_MOVE_UPPER_DEC32( dig[2], uval, 2 )

			M_P_ADD4_RV( p, uval,   dig[2] )
			M_P_ADD4_RV( p, dig[1], dig[0] )

			*p-- = '0';

		} else {
			M_P_CUT_LARGE_DIGIT( uval, width, is_cut_large_digit )

			M_P_MOVE_UPPER_DEC32( dig[0], uval, 8 )
			M_P_MOVE_UPPER_DEC32( dig[1], uval, 6 )
			M_P_MOVE_UPPER_DEC32( dig[2], uval, 4 )
			M_P_MOVE_UPPER_DEC32( dig[3], uval, 2 )

			M_P_ADD4_RV( p, uval,   dig[3] )
			M_P_ADD4_RV( p, dig[2], dig[1] )
			M_P_ADD1_RV( p, dig[0] )
		}
		break;
	case 10:
		/*
		 * is_cut_large_digit �͕K�� false
		 * width=10�̂Ƃ��A����������ɖ����Ȃ��ꍇ�́A
		 * �K���ŏ�ʂ��� space_ch �Ŗ��߂���.
		 */
		if( uval < st_pow10_table32[4] ){
			M_P_MOVE_UPPER_DEC32( dig[0], uval, 2 )
			M_P_ADD4_RV( p, uval, dig[0] )

			*p-- = '0'; *p-- = '0';
			*p-- = '0'; *p-- = '0';
			*p-- = '0'; *p-- = '0';

		} else if( uval < st_pow10_table32[8] ){
			M_P_MOVE_UPPER_DEC32( dig[0], uval, 6 )
			M_P_MOVE_UPPER_DEC32( dig[1], uval, 4 )
			M_P_MOVE_UPPER_DEC32( dig[2], uval, 2 )

			M_P_ADD4_RV( p, uval,   dig[2] )
			M_P_ADD4_RV( p, dig[1], dig[0] )

			*p-- = '0'; *p-- = '0';

		} else {
			M_P_MOVE_UPPER_DEC32( dig[0], uval, 8 )
			M_P_MOVE_UPPER_DEC32( dig[1], uval, 6 )
			M_P_MOVE_UPPER_DEC32( dig[2], uval, 4 )
			M_P_MOVE_UPPER_DEC32( dig[3], uval, 2 )

			M_P_ADD4_RV( p, uval,   dig[3] )
			M_P_ADD4_RV( p, dig[2], dig[1] )
			M_P_ADD2_RV( p, dig[0] )
		}
		break;
	default:
		{
			assert( 0 && "Invalid width" );
			break;
		}
	}

	/*
	 * ��ʌ��̃J�b�g���s���Ă��Ȃ��ꍇ�Ɍ���A
	 * �ŏ�ʂ���A������'0'�̘A���� space_ch �ɒu������.
	 * ���̒u���́A��'0'�̕������ŏ��Ɍ����܂ōŉ��ʂ̕����֌������čs���邪�A
	 * �ŏI����(��1���ɊY�����镶��)�́A�u�����Ȃ�.
	 *
	 * ��:
	 *   space_ch='@', 0010 �̏ꍇ�A@@10 �ƒu��
	 *   space_ch='@', 0000 �̏ꍇ�A@@@0 �ƒu��
	 */
	if( !is_cut_large_digit && space_ch != '0' ){
		char* last = end-1;
		++p;
		while( p < last && *p == '0' ){
			*p++ = space_ch;
		} 
	}

	if( term_nul ){ *end = '\0'; }

	return end;
}


/***
 * �������֐��Ƃ��� 32�ł��Ăяo���Ă��镔��������.
 */
Znk_INLINE char*
UInt64ToStr_asDecFlex( char* p, uint64_t uval, bool term_nul )
{
	const uint64_t billion = st_pow10_table64[9];

	/*
	 * uint64_t �̍ő�l�͈ȉ���20��.
	 * 1844,6744,0737,0955,1615 
	 *
	 * �����9���Â������
	 * 18,446744073,709551615 
	 *
	 * ����Ă����� upper, middle, lower �̂R�p�[�g�ɕ�����.
	 */

	if( uval < billion ){
		return UInt32ToStr_asDecFlex( p, (uint32_t)uval, term_nul );
	} else {
		/*
		 * �ȉ��A�ϐ���`�𔺂����߂�����else���֋L�q.
		 * ���̎��_�� uval �� 10���ȏ�
		 */
	
		/*
		 * ��9��������ʂ̌�(uint64_t�Ŏ擾����K�v����)
		 * ��[���̂͂�.
		 */
		uint64_t middle = uval / billion;
		/*
		 * ��9���𓾂�.
		 */
		uint32_t lower  = (uint32_t)(uval % billion);
	
		if( middle < billion ){
			p =    UInt32ToStr_asDecFlex( p, (uint32_t)middle, false ); /* null�I�[���Ȃ�. */
			return UInt32ToStr_asDecFix ( p, lower,            term_nul, 9, '0' );
		}

		/***
		 * ���̎��_�� uval �� 19���ȏ�
		 */
		{
			/*
			 * �ő�ł�2���̂͂�. ��[���̂͂�.
			 */
			uint32_t upper = (uint32_t)(middle / billion);
			/*
			 * ��9���𓾂�.
			 */
			middle %= billion;
		
			p = UInt32ToStr_asDecFlex( p, upper,            false ); /* null�I�[���Ȃ�. */
			p = UInt32ToStr_asDecFix ( p, (uint32_t)middle, false,    9, '0' ); /* null�I�[���Ȃ�. */
			p = UInt32ToStr_asDecFix ( p, lower,            term_nul, 9, '0' );
		}
	}
	return p;
}
/***
 * �������֐��Ƃ��� 32�ł��Ăяo���Ă��镔��������.
 */
Znk_INLINE char*
UInt64ToStr_asDecFix( char* p, uint64_t uval, bool term_nul, size_t width, char space_ch )
{
	const uint64_t billion = st_pow10_table64[9];

	/*
	 * width��20���傫���ꍇ�ANG.
	 */

	static const uint64_t uint32_max = UINT32_MAX;

	/*
	 * uint64_t �̍ő�l�͈ȉ���20��.
	 * 1844,6744,0737,0955,1615 
	 *
	 * �����9���Â������
	 * 18,446744073,709551615 
	 *
	 * ����Ă����� upper, middle, lower �̂R�p�[�g�ɕ�����.
	 */

	if( width <= 9 || uval < billion ){
		/*
		 * 0<width<=9, uval �l�͖�킸.
		 * or
		 * width>=10, uval ��9���ȉ�.
		 *
		 * 9���ڂ���ʂ̗]��X�y�[�X��space_ch�Ŗ��߂�.
		 */
		while( width > 9 ){
			*p++ = space_ch;
			--width;
		}
		/*
		 * uint32_max���傫���ꍇ�A�P���ɃL���X�g����Ɗe���̐��l���ς���Ă��܂�.
		 * �L���X�g����O�ɂ���ȉ��ɂ��Ȃ���΂Ȃ�Ȃ�.
		 * ���A�ő�9���K�v�Ȃ̂ŁA���̏����𖞂����ɂ� billion�̏�]���Ƃ�΂悢.
		 * uint32_max�ȉ��̏ꍇ�́A�]�v�ȏ�]���Ƃ�Ȃ��悤�ɒ��ӂ���.
		 * ���̏������s�����ꍇ�́A�I�[�o�[�t���[��\�����邽�߁Aspace_ch�������I��'0'�ɂ���.
		 */
		if( uval > uint32_max ){
			uval %= billion;
			space_ch = '0';
		}
		return UInt32ToStr_asDecFix( p, (uint32_t)uval, term_nul, width, space_ch );
	}

	/***
	 * ���̎��_�� uval �� 10���ȏ� ���� width ��10�ȏ�.
	 */
	{
		/*
		 * ��9��������ʂ̌�(uint64_t�Ŏ擾����K�v����)
		 * ��[���̂͂�.
		 */
		uint64_t middle = uval / billion;
		/*
		 * ��9���𓾂�.
		 */
		uint32_t lower  = (uint32_t)(uval % billion);
	
		if( width <= 18 || middle < billion ){
			/*
			 * 10<=width<=18, uval �l�͖�킸.
			 * or
			 * width>=19, uval ��18���ȉ�.
			 *
			 * 18���ڂ���ʂ̗]��X�y�[�X��space_ch�Ŗ��߂�.
			 */
			while( width > 18 ){
				*p++ = space_ch;
				--width;
			}
			/*
			 * uint32_max���傫���ꍇ�A�P���ɃL���X�g����Ɗe���̐��l���ς���Ă��܂�.
			 * �L���X�g����O�ɂ���ȉ��ɂ��Ȃ���΂Ȃ�Ȃ�.
			 * ���A�ő�9���K�v�Ȃ̂ŁA���̏����𖞂����ɂ� billion�̏�]���Ƃ�΂悢.
			 * uint32_max�ȉ��̏ꍇ�́A�]�v�ȏ�]���Ƃ�Ȃ��悤�ɒ��ӂ���.
			 * ���̏������s�����ꍇ�́A�I�[�o�[�t���[��\�����邽�߁Aspace_ch�������I��'0'�ɂ���.
			 */
			if( middle > uint32_max ){
				middle %= billion;
				space_ch='0';
			}
			p =    UInt32ToStr_asDecFix( p, (uint32_t)middle, false,    width-9, space_ch ); /* null�I�[���Ȃ�. */
			return UInt32ToStr_asDecFix( p, lower,            term_nul, 9,       '0' );
		}
		/***
		 * ���̎��_�� uval �� 19���ȏ� ���� width ��19�ȏ�.
		 */
		{
			/*
			 * �ő�ł�2���̂͂�.
			 * ��[���̂͂�.
			 */
			uint32_t upper = (uint32_t)(middle / billion);
			/*
			 * ��9���𓾂�.
			 */
			middle %= billion;
		
			p = UInt32ToStr_asDecFix( p, upper,            false,    width-18, space_ch ); /* null�I�[���Ȃ�. */
			p = UInt32ToStr_asDecFix( p, (uint32_t)middle, false,    9,        '0' );      /* null�I�[���Ȃ�. */
			p = UInt32ToStr_asDecFix( p, lower,            term_nul, 9,        '0' );
		}
	}
	return p;
}


/*
 * endof 10�i���\�������o�[�W����
 ***/
/*****************************************************************************/



/*****************************************************************************/
/***
 * ALL:
 * radix,is_supplement,is_truncate�z��IF
 *
 *
 * is_truncate:
 *   width > 0 �̂Ƃ��݈̂Ӗ�������.
 *   true �̂Ƃ�
 *     uval �̌�����width�𒴂���ꍇ�A��ʌ���ł��؂�.
 *   false �̂Ƃ�
 *     uval �̌�����width�𒴂���ꍇ�A��ʌ���K�؂Ɋg�����ĕ\������.
 *     printf�̓���ɓ�����.
 *
 *   printf�̓��� �� is_truncate == false && is_supplement == true �Ɠ���.
 *
 *
 * uint*_t 10�i���\���̑����C���^�[�t�F�[�X.
 * �^����ꂽwidth ����Ƃ��� 0���߁A�ł��؂�̂��ׂĂ̏󋵂ɑΉ�����.
 * �������Ap���w������buf�ɂ�uint*_t�p�K�{�T�C�Y���m�ۂ���Ă���Ɖ��肷��.
 *
 * is_supplement :
 *   true  �̂Ƃ��Auval �̌����� width �ɖ����Ȃ��ꍇ�� space_ch �ŕs�����𖄂߂�.
 *   false �̂Ƃ��Auval �̌����� width �ɖ����Ȃ��ꍇ�ł������t�����Ȃ�.
 * is_truncate :
 *   true   �̂Ƃ��Auval �̌����� width �𒴂���ꍇ�͏�ʂ�ł��؂�.
 *   false  �̂Ƃ��Auval �̌����� width �𒴂���ꍇ�ł���ʂ����ׂĕ\������.
 */
Znk_INLINE char*
UIntToStr_ALL_flex_32( char* p, uint32_t uval, bool term_nul, size_t radix, bool is_lower, bool use_general )
{
	if( use_general ){ return UIntToStr_generalFlex_32( p, uval, term_nul, is_lower, radix ); }
	switch( radix ){
	case 10: return UInt32ToStr_asDecFlex(  p, uval, term_nul );
	case 16: return UIntToStr_asHexFlex_32( p, uval, term_nul, is_lower );
	default: break;
	}
	return UIntToStr_generalFlex_32( p, uval, term_nul, is_lower, radix );
}
Znk_INLINE char*
UIntToStr_ALL_flex_64( char* p, uint64_t uval, bool term_nul, size_t radix, bool is_lower, bool use_general )
{
	if( use_general ){ return UIntToStr_generalFlex_64( p, uval, term_nul, is_lower, radix ); }
	switch( radix ){
	case 10: return UInt64ToStr_asDecFlex(  p, uval, term_nul );
	case 16: return UIntToStr_asHexFlex_64( p, uval, term_nul, is_lower );
	default: break;
	}
	return UIntToStr_generalFlex_64( p, uval, term_nul, is_lower, radix );
}

Znk_INLINE char*
UIntToStr_ALL_fix_32( char* p, uint32_t uval, bool term_nul,
		size_t width, char space_ch, size_t radix, bool is_lower, bool use_general )
{
	if( use_general ){ return UIntToStr_generalFix_32( p, uval, term_nul, width, space_ch, is_lower, radix ); }
	switch( radix ){
	case 10: return UInt32ToStr_asDecFix(  p, uval, term_nul, width, space_ch );
	case 16: return UIntToStr_asHexFix_32( p, uval, term_nul, width, space_ch, is_lower );
	default: break;
	}
	return UIntToStr_generalFix_32( p, uval, term_nul, width, space_ch, is_lower, radix );
}
Znk_INLINE char*
UIntToStr_ALL_fix_64( char* p, uint64_t uval, bool term_nul,
		size_t width, char space_ch, size_t radix, bool is_lower, bool use_general )
{
	if( use_general ){ return UIntToStr_generalFix_64( p, uval, term_nul, width, space_ch, is_lower, radix ); }
	switch( radix ){
	case 10: return UInt64ToStr_asDecFix(  p, uval, term_nul, width, space_ch );
	case 16: return UIntToStr_asHexFix_64( p, uval, term_nul, width, space_ch, is_lower );
	default: break;
	}
	return UIntToStr_generalFix_64( p, uval, term_nul, width, space_ch, is_lower, radix );
}

Znk_INLINE uintmax_t
Pow10U( size_t exp )
{
	switch( sizeof(uintmax_t) ){
	case 4: return (uintmax_t)getPow10U32( exp );
	case 8: return (uintmax_t)getPow10U64( exp );
	default: break;
	}
	return (uintmax_t)getPow10U64( exp );
}

/***
 * ���R��radix�̎��R��exp��𓾂�.
 * �������Aradix �� 0�`100 �܂�.
 */
static uintmax_t
getPowU( uintmax_t radix, size_t exp )
{
	switch( radix ){
	case 0:
		if( exp == 0 ){
			/***
			 * ���w�I�ɂ͕s��`
			 * C99 math.h �ł̓h���C���G���[��Ԃ��Ƃ���邪�A
			 * �����ł́A�񍀒藝�𐬗������邽�߁A0^0 = 1 �Ƃ���������Ƃ�.
			 */
			return 1;
		}
		return 0;
	case  1: return 1;
	case  2: return (uintmax_t)(0x1) << exp;      // exp*1 bit���V�t�g
	case  4: return (uintmax_t)(0x1) << (exp<<1); // exp*2 bit���V�t�g
	case  8: return (uintmax_t)(0x1) << (exp*3);  // exp*3 bit���V�t�g
	case 10: return Pow10U( exp );
	case 16: return (uintmax_t)(0x1) << (exp<<2); // exp*4 bit���V�t�g
	case 32: return (uintmax_t)(0x1) << (exp*5);  // exp*5 bit���V�t�g
	default:
		break;
	}

	switch( exp ){
	case 0: return 1;
	case 1: return radix;
	case 2: return radix * radix;
	case 3: return radix * radix * radix;
	case 4: { uintmax_t tmp = radix * radix; return tmp * tmp; }
	case 5: { uintmax_t tmp = radix * radix; return tmp * tmp * radix; }
	case 6: { uintmax_t tmp = radix * radix; return tmp * tmp * tmp; }
	case 7: { uintmax_t tmp = radix * radix; return tmp * tmp * tmp * radix; }
	case 8: { uintmax_t tmp = radix * radix; tmp = tmp * tmp; return tmp * tmp; }
	default: break;
	}

	/***
	 * exp >= 9 �ȍ~�͂Q���ؓI�ȍċA�����ō�������}��.
	 * ���A�Ȃ�ׂ��ċA�̊K�w��󂭂��邽�߁A
	 * ��Ȃ��9�敪���A�����Ȃ��8�敪����U����o���ĂQ�����Ă���.
	 */
	if( exp & 0x1 ){ /* exp �� � */
		exp -= 9;
		exp >>= 1; /* 2 �Ŋ���. */
		return getPowU( radix, exp ) * getPowU( radix, exp ) * getPowU( radix, 8 ) * radix;
	}
	exp -= 8;
	exp >>= 1; /* 2 �Ŋ���. */
	return getPowU( radix, exp ) * getPowU( radix, exp ) * getPowU( radix, 8 );
}



Znk_INLINE char*
UIntToStr_ALL_32( char *p, uint32_t uval, bool term_nul,
		size_t width, char space_ch,
		size_t radix, bool is_lower,
		bool is_supplement, bool is_truncate,
		bool use_general )
{
	//size_t digit_num_umax = B1::Private::DigitNumMax<T>()( radix );
	const size_t digit_num_umax = getDigitNum_UMAX32( radix );
	assert( width <= digit_num_umax );

	/*
	 * uval == 0 && width == 0 �̓���P�[�X�������邽�߁A
	 * ��� width == 0 �̏ꍇ����ʂɏ�������.
	 */
	if( width == 0 ){
		if( is_truncate ){
			if( term_nul ){ *p = '\0'; }
			return p;
		}
		/*
		 * �S���\��.
		 */
		return UIntToStr_ALL_flex_32( p, uval, term_nul, radix, is_lower, use_general );
	}


	/*
	 * ���x����̂��߁A�����ɔ��f�ł���p�^�[���ɂ��Ă͏E���Ă���.
	 */
	if( !is_supplement && !is_truncate ){
		/*
		 * �S���\��.
		 */
		return UIntToStr_ALL_flex_32( p, uval, term_nul, radix, is_lower, use_general );
	}
	if( is_supplement && is_truncate ){
		/*
		 * width ���őł��؂� ���� �s������space_ch�Ŗ��߂�.
		 */
		return UIntToStr_ALL_fix_32( p, uval, term_nul, width, space_ch, radix, is_lower, use_general );
	}


	/*
	 * getPowU ��width�Ƃ��� digit_num_umax �̓T�C�Y�I�[�o�[�̂��ߎg�p�ł��Ȃ�.
	 * width == digit_num_umax �̂Ƃ�������ɓ��ʂɈ���.
	 * ���̂Ƃ��utruncate�v�̕K�v���͂Ȃ�.
	 */
	if( width == digit_num_umax ){
		if( is_supplement ){
			/*
			 * �S���\�� ���� �s������ space_ch �Ŗ��߂�.
			 */
			return UIntToStr_ALL_fix_32( p, uval, term_nul, width, space_ch, radix, is_lower, use_general );
		}
		/*
		 * �S���\��.
		 */
		return UIntToStr_ALL_flex_32( p, uval, term_nul, radix, is_lower, use_general );
	}
	/*
	 * ���̎��_�� width < digit_num_umax
	 **/

	/*
	 * uval �̌����� width �ȉ��ł���Ƃ�
	 * ���̂Ƃ��utruncate�v�̕K�v���͂Ȃ�.
	 *
	 * �����ł� getPowU �͐��������삷�邪�A��͂�d�������ɂȂ�Ǝv����.
	 * �V���v����tmp buf��p�ӂ��Ĉ�UFlex�őS�̂����߁A���̌��width�ɉ�����memcpy�������
	 * ������������Ȃ��ƃe�X�g���Ă݂����AgetPowU���g���������т͂悩����.
	 *  is_truncate==true,is_supplement==false �̏ꍇ�Awidth�����Ȃ��i�K�ł�getPowU �̕�������.
	 *  is_truncate==false,is_supplement==true �̏ꍇ�A�ǂ�������܂�ς��Ȃ�.
	 */
	if( (uintmax_t)uval < getPowU( (uintmax_t)radix, width ) ){
		if( is_supplement ){
			/*
			 * �S���\�� ���� �s������ space_ch �Ŗ��߂�.
			 */
			return UIntToStr_ALL_fix_32( p, uval, term_nul, width, space_ch, radix, is_lower, use_general );
		}
		/***
		 * �S���\��.
		 */
		return UIntToStr_ALL_flex_32( p, uval, term_nul, radix, is_lower, use_general );
	}
	/*
	 * ���̎��_�� uval�̌����� width �����傫��.
	 * ����ȍ~�usupplement�v�̕K�v���͂Ȃ�.
	 **/

	if( is_truncate ){
		/***
		 * width ���őł��؂�.
		 */
		return UIntToStr_ALL_fix_32( p, uval, term_nul, width, space_ch, radix, is_lower, use_general );
	}
	/***
	 * �S���\��.
	 */
	return UIntToStr_ALL_flex_32( p, uval, term_nul, radix, is_lower, use_general );

}
/***
 * ��{�I�ɉ������֐��̌Ăяo����64�ɂȂ��Ă���Ƃ���ȊO�� UIntToStr_ALL_32 �Ɠ��l.
 */
Znk_INLINE char*
UIntToStr_ALL_64( char *p, uint64_t uval, bool term_nul,
		size_t width, char space_ch,
		size_t radix, bool is_lower,
		bool is_supplement, bool is_truncate,
		bool use_general )
{
	const size_t digit_num_umax = getDigitNum_UMAX64( radix );
	assert( width <= digit_num_umax );

	/*
	 * uval == 0 && width == 0 �̓���P�[�X�������邽�߁A
	 * ��� width == 0 �̏ꍇ����ʂɏ�������.
	 */
	if( width == 0 ){
		if( is_truncate ){
			if( term_nul ){ *p = '\0'; }
			return p;
		}
		/*
		 * �S���\��.
		 */
		return UIntToStr_ALL_flex_64( p, uval, term_nul, radix, is_lower, use_general );
	}


	/*
	 * ���x����̂��߁A�����ɔ��f�ł���p�^�[���ɂ��Ă͏E���Ă���.
	 */
	if( !is_supplement && !is_truncate ){
		/*
		 * �S���\��.
		 */
		return UIntToStr_ALL_flex_64( p, uval, term_nul, radix, is_lower, use_general );
	}
	if( is_supplement && is_truncate ){
		/*
		 * width ���őł��؂� ���� �s������space_ch�Ŗ��߂�.
		 */
		return UIntToStr_ALL_fix_64( p, uval, term_nul, width, space_ch, radix, is_lower, use_general );
	}


	/*
	 * getPowU ��width�Ƃ��� digit_num_umax �̓T�C�Y�I�[�o�[�̂��ߎg�p�ł��Ȃ�.
	 * width == digit_num_umax �̂Ƃ�������ɓ��ʂɈ���.
	 * ���̂Ƃ��utruncate�v�̕K�v���͂Ȃ�.
	 */
	if( width == digit_num_umax ){
		if( is_supplement ){
			/*
			 * �S���\�� ���� �s������ space_ch �Ŗ��߂�.
			 */
			return UIntToStr_ALL_fix_64( p, uval, term_nul, width, space_ch, radix, is_lower, use_general );
		}
		/*
		 * �S���\��.
		 */
		return UIntToStr_ALL_flex_64( p, uval, term_nul, radix, is_lower, use_general );
	}
	/*
	 * ���̎��_�� width < digit_num_umax
	 **/

	/*
	 * uval �̌����� width �ȉ��ł���Ƃ�
	 * ���̂Ƃ��utruncate�v�̕K�v���͂Ȃ�.
	 *
	 * �����ł� getPowU �͐��������삷�邪�A��͂�d�������ɂȂ�Ǝv����.
	 * �V���v����tmp buf��p�ӂ��Ĉ�UFlex�őS�̂����߁A���̌��width�ɉ�����memcpy�������
	 * ������������Ȃ��ƃe�X�g���Ă݂����AgetPowU���g���������т͂悩����.
	 *  is_truncate==true,is_supplement==false �̏ꍇ�Awidth�����Ȃ��i�K�ł�getPowU �̕�������.
	 *  is_truncate==false,is_supplement==true �̏ꍇ�A�ǂ�������܂�ς��Ȃ�.
	 */
	if( (uintmax_t)uval < getPowU( (uintmax_t)radix, width ) ){
		if( is_supplement ){
			/*
			 * �S���\�� ���� �s������ space_ch �Ŗ��߂�.
			 */
			return UIntToStr_ALL_fix_64( p, uval, term_nul, width, space_ch, radix, is_lower, use_general );
		}
		/***
		 * �S���\��.
		 */
		return UIntToStr_ALL_flex_64( p, uval, term_nul, radix, is_lower, use_general );
	}
	/*
	 * ���̎��_�� uval�̌����� width �����傫��.
	 * ����ȍ~�usupplement�v�̕K�v���͂Ȃ�.
	 **/

	if( is_truncate ){
		/***
		 * width ���őł��؂�.
		 */
		return UIntToStr_ALL_fix_64( p, uval, term_nul, width, space_ch, radix, is_lower, use_general );
	}
	/***
	 * �S���\��.
	 */
	return UIntToStr_ALL_flex_64( p, uval, term_nul, radix, is_lower, use_general );

}

/*
 * endof radix,is_supplement,is_truncate�z��IF
 ***/
/*****************************************************************************/




/*****************************************************************************/
/***
 * SIntToStr_T
 * (digit_num_max �ȓ���body�������擾����.
 * �����Ɋւ���v���v���Z�X���s������AUIntToStr_ALL �ɏ������Ϗ�����)
 */
Znk_INLINE char*
setCompactiveSignCh( char* p, char* begin, char space_ch, char sign_ch )
{
	if( begin[0] != space_ch ){ return p; }
	Znk_memmove( begin, begin+1, p - begin );
	--p;
	return p;
}


Znk_INLINE char*
UIntToStr_withCast_I32( char* p, int32_t ival, bool term_nul,
		size_t width, char space_ch, size_t radix, bool is_lower,
		bool is_supplement, bool is_truncate,
		bool use_general )
{
	return UIntToStr_ALL_32( p, (uint32_t)(ival), term_nul,
			width, space_ch, radix, is_lower,
			is_supplement, is_truncate,
			use_general );
}
Znk_INLINE char*
UIntToStr_withCast_I64( char* p, int64_t ival, bool term_nul,
		size_t width, char space_ch, size_t radix, bool is_lower,
		bool is_supplement, bool is_truncate,
		bool use_general )
{
	return UIntToStr_ALL_64( p, (uint64_t)(ival), term_nul,
			width, space_ch, radix, is_lower,
			is_supplement, is_truncate,
			use_general );
}


/***
 * Note:
 * ���L A �̕����ɂ���.
 *
 * p �͏\���ȃT�C�Y(st_uint*_str_buf_size�ȏ�)�̃o�b�t�@���w���Ɖ���.
 *
 * int32_t �̏ꍇ
 * INT32_MAX  2147483647 : 0111 1111 1111 1111 1111 1111 1111 1111
 * INT32_MIN -2147483648 : 1000 0000 0000 0000 0000 0000 0000 0000
 *
 * ���� ival �� INT32_MIN ���w�肳�ꂽ�ꍇ�A-ival �Ƃ��ƃI�[�o�[�t���[���N�������߁A
 * ���ʂɏ�������. �������Avc,bcc,gcc���ɁA-INT32_MIN �Ƃ��Ƃ���� INT32_MIN �Ɋ��S�ɓ�����.
 * bit���x���Ŕ�r���Ă� -INT32_MIN �� INT32_MIN �͓��������ʂƂȂ���.
 * (gcc�ł͂��̂悤�Ȏg�p�ɑ΂��Čx�����o��).
 *
 * int64_t �̏ꍇ
 * INT64_MAX  9223372036854775807LL : 0x7fffffffffffffff 
 * INT64_MIN -9223372036854775808LL : 0x8000000000000000 
 * ���� ival �� INT64_MIN ���w�肳�ꂽ�ꍇ�A-ival �Ƃ��ƃI�[�o�[�t���[���N�������߁A
 * ���ʂɏ�������. �������Avc,bcc,gcc���ɁA-INT64_MIN �Ƃ��Ƃ���� INT64_MIN �Ɋ��S�ɓ�����.
 * bit���x���Ŕ�r���Ă� -INT64_MIN �� INT64_MIN �͓��������ʂƂȂ���.
 * (gcc�ł͂��̂悤�Ȏg�p�ɑ΂��Čx�����o��).
 *
 *
 * ���L B �̕����ɂ���.
 * �Ⴆ�΁Auint32_t �̏ꍇ
 * INT32_MIN + INT32_MAX == (���ׂĂ�bit��1) == INT32_MAX * 2 + 1
 * �� INT32_MIN == INT32_MAX + 1 == static_cast<uint32_t>(INT32_MAX) + 1
 * �� INT32_MIN �� bit�z��́Auint32_t�ł� INT32_MAX+1 ��bit�z��ɓ������A
 *    ����͂��Ȃ킿�Astatic_cast<uint32_t>(INT32_MIN) == static_cast<uint32_t>(INT32_MAX)+1
 *    �����藧���Ƃ�����.
 * �ȉ��ł͂���𗘗p���āAstatic_cast<uint32_t>(INT32_MIN) ��l�Ƃ��ēn�����ƂŁA
 * INT32_MIN �� �����Ȃ��ł�radix�i��������𓾂Ă���.
 *
 * uint64_t �̏ꍇ�����l.
 */
/***
 * width <= digit_num_max �łȂ���΂Ȃ�Ȃ�.
 */
static char*
SIntToStr_I32( char* p, int32_t ival, bool term_nul,
		size_t width, char space_ch, size_t radix, ZnkToStrFlags flags )
{
	bool is_supplement      = Znk_IS_FLAG32(flags, ZnkToStr_e_IS_SUPPLEMENT);
	bool is_truncate        = Znk_IS_FLAG32(flags, ZnkToStr_e_IS_TRUNCATE);
	bool is_lower           = Znk_IS_FLAG32(flags, ZnkToStr_e_IS_LOWER);
	bool use_general        = Znk_IS_FLAG32(flags, ZnkToStr_e_IS_USE_GENERAL);

	const size_t digit_num_max = ( ival < 0 ) ?
		getDigitNum_IMIN32( radix ) :
		getDigitNum_IMAX32( radix );

	if( ival == 0 ){
		bool is_zero_sign_white = Znk_IS_FLAG32(flags, ZnkToStr_e_IS_ZERO_SIGN_WHITE);
		bool is_zero_sign_plus  = Znk_IS_FLAG32(flags, ZnkToStr_e_IS_ZERO_SIGN_PLUS);
		bool is_zero_sign_minus = Znk_IS_FLAG32(flags, ZnkToStr_e_IS_ZERO_SIGN_MINUS);

		if( is_zero_sign_minus ){
			*p++ = '-';
		}else if( is_zero_sign_plus ){
			*p++ = '+';
		}else if( is_zero_sign_white ){
			*p++ = ' ';
		}else{
			/* none */
		}
		return UIntToStr_withCast_I32( p, ival, term_nul, Znk_MIN(width,digit_num_max), space_ch,
				radix, is_lower,
				is_supplement, is_truncate,
				use_general );
	}

	/* A */
	if( ival == INT32_MIN ){
		*p++ = '-';
		/* B */
		return UIntToStr_withCast_I32( p, ival, term_nul, Znk_MIN(width,digit_num_max), space_ch,
				radix, is_lower,
				is_supplement, is_truncate,
				use_general );

	} else {
		bool is_plus = Znk_IS_FLAG32(flags, ZnkToStr_e_IS_PLUS);
		char sign_ch = '\0';
		bool is_compact;

		if( ival < 0 ){
			ival = -ival;
			sign_ch = '-';
		} else {
			if( is_plus ){ sign_ch = '+'; }
		}
	
		if( sign_ch == '\0' ){
			return UIntToStr_withCast_I32( p, ival, term_nul, Znk_MIN(width,digit_num_max), space_ch,
					radix, is_lower,
					is_supplement, is_truncate,
					use_general );
		}
	
		is_compact = Znk_IS_FLAG32(flags, ZnkToStr_e_IS_COMPACT);
		if( is_compact ){
			char* begin;
			*p++ = sign_ch;
			begin = p;
			p = UIntToStr_withCast_I32( p, ival, term_nul, width, space_ch,
					radix, is_lower,
					is_supplement, is_truncate,
					use_general );
			p = setCompactiveSignCh( p, begin, space_ch, sign_ch );
		} else {
			*p++ = sign_ch;
			p = UIntToStr_withCast_I32( p, ival, term_nul, Znk_MIN(width,digit_num_max), space_ch,
					radix, is_lower,
					is_supplement, is_truncate,
					use_general );
		}
	}
	return p;
}
/***
 * ��{�I��UIntToStr_withCast_I64�̌Ăяo���ȊO��I32�̏ꍇ�Ɠ��l.
 */
static char*
SIntToStr_I64( char* p, int64_t ival, bool term_nul,
		size_t width, char space_ch, size_t radix, ZnkToStrFlags flags )
{
	bool is_supplement      = Znk_IS_FLAG32(flags, ZnkToStr_e_IS_SUPPLEMENT);
	bool is_truncate        = Znk_IS_FLAG32(flags, ZnkToStr_e_IS_TRUNCATE);
	bool is_lower           = Znk_IS_FLAG32(flags, ZnkToStr_e_IS_LOWER);
	bool use_general        = Znk_IS_FLAG32(flags, ZnkToStr_e_IS_USE_GENERAL);

	const size_t digit_num_max = ( ival < 0 ) ?
		getDigitNum_IMIN64( radix ) :
		getDigitNum_IMAX64( radix );

	if( ival == 0 ){
		bool is_zero_sign_white = Znk_IS_FLAG32(flags, ZnkToStr_e_IS_ZERO_SIGN_WHITE);
		bool is_zero_sign_plus  = Znk_IS_FLAG32(flags, ZnkToStr_e_IS_ZERO_SIGN_PLUS);
		bool is_zero_sign_minus = Znk_IS_FLAG32(flags, ZnkToStr_e_IS_ZERO_SIGN_MINUS);

		if( is_zero_sign_minus ){
			*p++ = '-';
		}else if( is_zero_sign_plus ){
			*p++ = '+';
		}else if( is_zero_sign_white ){
			*p++ = ' ';
		}else{
			/* none */
		}
		return UIntToStr_withCast_I64( p, ival, term_nul, Znk_MIN(width,digit_num_max), space_ch,
				radix, is_lower,
				is_supplement, is_truncate,
				use_general );
	}

	/* A */
	if( ival == INT64_MIN ){
		*p++ = '-';
		/* B */
		return UIntToStr_withCast_I64( p, ival, term_nul, Znk_MIN(width,digit_num_max), space_ch,
				radix, is_lower,
				is_supplement, is_truncate,
				use_general );

	} else {
		bool is_plus = Znk_IS_FLAG32(flags, ZnkToStr_e_IS_PLUS);
		char sign_ch = '\0';
		bool is_compact;

		if( ival < 0 ){
			ival = -ival;
			sign_ch = '-';
		} else {
			if( is_plus ){ sign_ch = '+'; }
		}
	
		if( sign_ch == '\0' ){
			return UIntToStr_withCast_I64( p, ival, term_nul, Znk_MIN(width,digit_num_max), space_ch,
					radix, is_lower,
					is_supplement, is_truncate,
					use_general );
		}
	
		is_compact = Znk_IS_FLAG32(flags, ZnkToStr_e_IS_COMPACT);
		if( is_compact ){
			char* begin;
			*p++ = sign_ch;
			begin = p;
			p = UIntToStr_withCast_I64( p, ival, term_nul, width, space_ch,
					radix, is_lower,
					is_supplement, is_truncate,
					use_general );
			p = setCompactiveSignCh( p, begin, space_ch, sign_ch );
		} else {
			*p++ = sign_ch;
			p = UIntToStr_withCast_I64( p, ival, term_nul, Znk_MIN(width,digit_num_max), space_ch,
					radix, is_lower,
					is_supplement, is_truncate,
					use_general );
		}
	}
	return p;
}


/*
 * endof SIntToStr_T
 ***/
/*****************************************************************************/



/*****************************************************************************/
/***
 * UIntToStr_T
 */

/***
 * width <= digit_num_max �łȂ���΂Ȃ�Ȃ�.
 */
Znk_INLINE char*
UIntToStr_32( char* p, uint32_t uval, bool term_nul,
		size_t width, char space_ch, size_t radix, const ZnkToStrFlags flags )
{
	bool is_lower      = Znk_IS_FLAG32(flags, ZnkToStr_e_IS_LOWER);
	bool is_supplement = Znk_IS_FLAG32(flags, ZnkToStr_e_IS_SUPPLEMENT);
	bool is_truncate   = Znk_IS_FLAG32(flags, ZnkToStr_e_IS_TRUNCATE);
	bool use_general   = Znk_IS_FLAG32(flags, ZnkToStr_e_IS_USE_GENERAL);
	return UIntToStr_ALL_32( p, uval, term_nul,
			width, space_ch, radix, is_lower,
			is_supplement, is_truncate,
			use_general );
}
Znk_INLINE char*
UIntToStr_64( char* p, uint64_t uval, bool term_nul,
		size_t width, char space_ch, size_t radix, const ZnkToStrFlags flags )
{
	bool is_lower      = Znk_IS_FLAG32(flags, ZnkToStr_e_IS_LOWER);
	bool is_supplement = Znk_IS_FLAG32(flags, ZnkToStr_e_IS_SUPPLEMENT);
	bool is_truncate   = Znk_IS_FLAG32(flags, ZnkToStr_e_IS_TRUNCATE);
	bool use_general   = Znk_IS_FLAG32(flags, ZnkToStr_e_IS_USE_GENERAL);
	return UIntToStr_ALL_64( p, uval, term_nul,
			width, space_ch, radix, is_lower,
			is_supplement, is_truncate,
			use_general );
}

/***
 * B1_double_to_str �Ŏg�p���Ă��镔�������邽�߁A
 * �Ƃ肠�����c���Ă���
 */
char*
ZnkToStr_Private_UInt32ToStr( char* p, uint32_t uval, bool term_nul,
		size_t width, char space_ch, size_t radix, ZnkToStrFlags flags )
{
	return UIntToStr_32( p, uval, term_nul,
			width, space_ch, radix, flags );
}

/*
 * endof UIntToStr_T
 ***/
/*****************************************************************************/


/*****************************************************************************/
/***
 * getCStr IF
 */
Znk_INLINE bool
getIsSign( intmax_t ival, ZnkToStrFlags flags )
{
	if( ival < 0 ) { return true; }

	if( ival == 0 ) {
		if ( Znk_IS_FLAG32(flags, ZnkToStr_e_IS_ZERO_SIGN_WHITE) 
		  || Znk_IS_FLAG32(flags, ZnkToStr_e_IS_ZERO_SIGN_PLUS)
		  || Znk_IS_FLAG32(flags, ZnkToStr_e_IS_ZERO_SIGN_MINUS) )
		{
			return true;
		}
		return false;
	}

	if( Znk_IS_FLAG32(flags, ZnkToStr_e_IS_PLUS) ){ return true; }
	return false;
}

Znk_INLINE size_t
getRemainWidth( ZnkToStrFlags* flags, bool is_sign, size_t width, size_t digit_num_max )
{
	const bool is_compact = Znk_IS_FLAG32(*flags, ZnkToStr_e_IS_COMPACT);
	if( !is_sign || !is_compact ){
		return width - digit_num_max;
	}

	/*
	 * �����ň���炷���߁A
	 * �ȍ~�� compact�������s��Ȃ�.
	 */
	*flags &= ~ZnkToStr_e_IS_COMPACT;
	return width - digit_num_max - 1;
}

Znk_INLINE char*
fillRemainU( char* p, size_t width, char space_ch,
		ZnkToStrFlags flags, size_t digit_num_max )
{
	/*
	 * width �� digit_num_max �𒴂���ꍇ�Ais_supplement �� true �Ȃ��
	 * ���ߕ��� space_ch �Ŗ��߂�. ( is_supplement �� false �̏ꍇ�͉������Ȃ����A
	 * ����͌��ǁAwidth �̗^�������A���̃f�[�^�^��radix�ɑ΂��đ傫�������Ƃ������ƂɂȂ� )
	 * �ȍ~�̏����� width <= digit_num_maxdigit_num_umax �ƂȂ�悤�ɂ��Ȃ���΂Ȃ�Ȃ�.
	 */
	bool is_supplement = Znk_IS_FLAG32(flags, ZnkToStr_e_IS_SUPPLEMENT);
	if( is_supplement && width > digit_num_max ){
		size_t remain_width = width - digit_num_max;
		if( remain_width ){
			Znk_memset( p, space_ch, remain_width );
			p += remain_width;
		}
	}
	return p;
}
Znk_INLINE char*
fillRemainI( char* p, bool is_sign,
		size_t width, char space_ch,
		ZnkToStrFlags* flags, size_t digit_num_max )
{
	/*
	 * width �� digit_num_max �𒴂���ꍇ�Ais_supplement �� true �Ȃ��
	 * ���ߕ��� space_ch �Ŗ��߂�. ( is_supplement �� false �̏ꍇ�͉������Ȃ����A
	 * ����͌��ǁAwidth �̗^�������A���̃f�[�^�^��radix�ɑ΂��đ傫�������Ƃ������ƂɂȂ� )
	 * �ȍ~�̏����� width <= digit_num_maxdigit_num_umax �ƂȂ�悤�ɂ��Ȃ���΂Ȃ�Ȃ�.
	 *
	 * is_compact �� true �̂Ƃ��A�ȉ��̂悤�ɕ����čl����.
	 * 1. width == digit_num_max �̂Ƃ�
	 *    �ō���space_ch�̂Ƃ�body�擾�֐�������𕄍��ɒu��������.
	 *
	 * 2. width > digit_num_max
	 *    �����ŕ�������������������space_ch�𖄂߂�.
	 *
	 * 3. width < digit_num_max
	 *    �ō���space_ch�̂Ƃ�body�擾�֐�������𕄍��ɒu��������.
	 */
	bool is_supplement = Znk_IS_FLAG32(*flags, ZnkToStr_e_IS_SUPPLEMENT);
	if( is_supplement && width > digit_num_max ){
		size_t remain_width = getRemainWidth( flags, is_sign, width, digit_num_max );
		if( remain_width ){
			Znk_memset( p, space_ch, remain_width );
			p += remain_width;
		}
	}
	return p;
}

Znk_INLINE size_t
getMustBufSize( size_t width, size_t digit_num_max, size_t buf_size_for_body )
{
	size_t additional_size = 0;
	if( width > digit_num_max ){
		additional_size = width - digit_num_max;
	}
	return buf_size_for_body + additional_size;
}



size_t
ZnkToStr_U32_getStr( char* buf, size_t buf_size, uint32_t uval,
		size_t width, char space_ch, size_t radix, ZnkToStrFlags flags )
{
	static const bool term_nul = true;
	size_t digit_num_max = getDigitNum_UMAX32( radix );
	char* p = buf;

	if( buf_size <= digit_num_max ){ return 0; }
	if( width >= buf_size ){
		width = buf_size-1;
	}

	/***
	 * �K�v�Ȃ�΁Adigit_num_max �ȍ~�ɂ����ʌ���space_ch�Ŗ��߂�.
	 */
	p = fillRemainU( p, width, space_ch, flags, digit_num_max );
	/***
	 * UIntToStr_T �� width <= digit_num_max �ł���K�v������.
	 */
	width = Znk_MIN(width, digit_num_max);
	/***
	 * �擾�����񒷂�Ԃ�.
	 */
	return UIntToStr_32( p, uval, term_nul, width, space_ch, radix, flags ) - buf;
}
size_t
ZnkToStr_U64_getStr( char* buf, size_t buf_size, uint64_t uval,
		size_t width, char space_ch, size_t radix, ZnkToStrFlags flags )
{
	static const bool term_nul = true;
	size_t digit_num_max = getDigitNum_UMAX64( radix );
	char* p = buf;

	if( buf_size <= digit_num_max ){ return 0; }
	if( width >= buf_size ){
		width = buf_size-1;
	}

	/***
	 * �K�v�Ȃ�΁Adigit_num_max �ȍ~�ɂ����ʌ���space_ch�Ŗ��߂�.
	 */
	p = fillRemainU( p, width, space_ch, flags, digit_num_max );
	/***
	 * UIntToStr_T �� width <= digit_num_max �ł���K�v������.
	 */
	width = Znk_MIN(width, digit_num_max);
	/***
	 * �擾�����񒷂�Ԃ�.
	 */
	return UIntToStr_64( p, uval, term_nul, width, space_ch, radix, flags ) - buf;
}


size_t
ZnkToStr_I32_getStr( char* buf, size_t buf_size, int32_t ival,
		size_t width, char space_ch, size_t radix, ZnkToStrFlags flags )
{
	static const bool term_nul = true;
	size_t digit_num_max = (ival >= 0) ?
		getDigitNum_IMAX32( radix ) :
		getDigitNum_IMIN32( radix );
	char*  p = buf;
	size_t mid_sign_pos;
	size_t str_leng = 0;
	bool   is_sign_top;

	if( buf_size <= digit_num_max ){ return 0; }
	if( width >= buf_size ){
		width = buf_size-1;
	}

	/***
	 * �K�v�Ȃ�΁Adigit_num_max �ȍ~�ɂ����ʌ���space_ch�Ŗ��߂�.
	 */
	p = fillRemainI( p, getIsSign(ival, flags),
			width, space_ch, &flags, digit_num_max );

	/***
	 * SIntToStr_I32�Ŏ擾����镶����̐擪�ʒu(����������\���̂���ʒu)��
	 * buf �ł̃C���f�b�N�X�Ƃ��ĕۑ����Ă���.
	 */
	mid_sign_pos = p - buf;

	/***
	 * SIntToStr_I32 �� width <= digit_num_max �ł���K�v������.
	 */
	width = Znk_MIN(width, digit_num_max);
	str_leng = SIntToStr_I32( p, ival, term_nul, width, space_ch, radix, flags ) - buf;

	/***
	 * is_sign_top �̏���:
	 * ���� mid_sign_pos > 0 �Ȃ�΁ASIntToStr_I32 �Ŏ擾��������������
	 * �擪�ɗ��Ă��Ȃ��Ƃ������Ƃł���.
	 * ���̏ꍇ�Abuf�̍ŏ��ɂ���space_ch �� mid_sign_pos �Ŏ������ʒu��
	 * �i�[����Ă���͂��̕������������ւ���.
	 */
	is_sign_top = Znk_IS_FLAG32(flags, ZnkToStr_e_IS_SIGN_TOP);
	if( is_sign_top && mid_sign_pos ){
		if ( buf[ mid_sign_pos ] == '-' || buf[ mid_sign_pos ] == '+'){
			Znk_SWAP( char, buf[ 0 ], buf[ mid_sign_pos ] );
		}
	}
	return str_leng;
}
size_t
ZnkToStr_I64_getStr( char* buf, size_t buf_size, int64_t ival,
		size_t width, char space_ch, size_t radix, ZnkToStrFlags flags )
{
	static const bool term_nul = true;
	size_t digit_num_max = (ival >= 0) ?
		getDigitNum_IMAX64( radix ) :
		getDigitNum_IMIN64( radix );
	char*  p = buf;
	size_t mid_sign_pos;
	size_t str_leng = 0;
	bool   is_sign_top;

	if( buf_size <= digit_num_max ){ return 0; }
	if( width >= buf_size ){
		width = buf_size-1;
	}

	/***
	 * �K�v�Ȃ�΁Adigit_num_max �ȍ~�ɂ����ʌ���space_ch�Ŗ��߂�.
	 */
	p = fillRemainI( p, getIsSign(ival, flags),
			width, space_ch, &flags, digit_num_max );

	/***
	 * SIntToStr_I64�Ŏ擾����镶����̐擪�ʒu(����������\���̂���ʒu)��
	 * buf �ł̃C���f�b�N�X�Ƃ��ĕۑ����Ă���.
	 */
	mid_sign_pos = p - buf;

	/***
	 * SIntToStr_I64 �� width <= digit_num_max �ł���K�v������.
	 */
	width = Znk_MIN(width, digit_num_max);
	str_leng = SIntToStr_I64( p, ival, term_nul, width, space_ch, radix, flags ) - buf;

	/***
	 * is_sign_top �̏���:
	 * ���� mid_sign_pos > 0 �Ȃ�΁ASIntToStr_I64 �Ŏ擾��������������
	 * �擪�ɗ��Ă��Ȃ��Ƃ������Ƃł���.
	 * ���̏ꍇ�Abuf�̍ŏ��ɂ���space_ch �� mid_sign_pos �Ŏ������ʒu��
	 * �i�[����Ă���͂��̕������������ւ���.
	 */
	is_sign_top = Znk_IS_FLAG32(flags, ZnkToStr_e_IS_SIGN_TOP);
	if( is_sign_top && mid_sign_pos ){
		if ( buf[ mid_sign_pos ] == '-' || buf[ mid_sign_pos ] == '+'){
			Znk_SWAP( char, buf[ 0 ], buf[ mid_sign_pos ] );
		}
	}
	return str_leng;
}


/*
 * endof getCStr IF
 ***/
/*****************************************************************************/

const char* 
ZnkToStr_Bool_getStr( bool bool_val, char type )
{
#define M_BOOL_STR( T, F ) bool_val ? T : F
	switch( type ){
	case 'T' : return M_BOOL_STR( "T", "F" );
	case 't' : return M_BOOL_STR( "t", "f" );
	case '1' : return M_BOOL_STR( "1", "0" );
	case 'A' : return M_BOOL_STR( "TRUE", "FALSE" );
	case 'L' : return M_BOOL_STR( "True", "False" );
	case 'a' :
	default: break;
	}
	return M_BOOL_STR( "true", "false" );
}

