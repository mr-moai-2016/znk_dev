#ifndef INCLUDE_GUARD__Znk_s_base_h__
#define INCLUDE_GUARD__Znk_s_base_h__

#include <Znk_stdc.h>
#include <Znk_s_posix.h>
#include <stdarg.h>

Znk_EXTERN_C_BEGIN

typedef struct {
	const char* cstr_;
	uintptr_t   leng_;
} ZnkSRef;

#define ZnkSRef_set_literal( sref, literal_str ) \
	(sref)->cstr_ = literal_str, (sref)->leng_ = Znk_strlen_literal( literal_str )


Znk_INLINE void
Znk_setStrLen_ifNPos( size_t* leng, const char* cstr ){
	if( *leng == Znk_NPOS ){ *leng = Znk_strlen(cstr); }
}



Znk_INLINE bool
ZnkS_empty( const char* str )
{
	return (bool)( str == NULL || str[0] == '\0' );
}
/**
 * @brief
 *   �������A������.
 *   buf��cstr���w���̈�͏d�Ȃ��Ă��Ă��悢.
 *   buf����'\0'���܂܂�Ă��Ȃ��悤�Ȉُ�ȕ������^���Ă��悢��
 *   ���̏ꍇ�Ȃɂ�������NULL��Ԃ�.
 *   �܂��A����̕�����̒�����buf_size�𒴂���ꍇ�A�o�b�t�@�I�[�ŋ����I�ɑł��؂��A
 *   �Ō��'\0'���t�������.
 *   ���̊֐��ł� strcat��strncat��buffer overflow�̊댯�����قڊ��S�ɔr������Ă���.
 *   ���̂���葬�x�͎኱���.
 * @return
 *   buf�ɂ�����'\0'�I�[���Ă���ʒu�������|�C���^�Ԃ�.
 *   (�v����ɘA����̕�����̏I�[�ʒu�ł���. ���̓_��strcat��strncat�̎d�l�ƈقȂ�_�ɒ���)
 *   ������buf����'\0'��������Ȃ��ꍇ�͉�������NULL��Ԃ�.
 */
char*
ZnkS_concatEx( char* buf, size_t buf_size, const char* cstr, size_t cstr_leng );
char*
ZnkS_concat( char* buf, size_t buf_size, const char* cstr );

void
ZnkS_copy( char* buf, size_t buf_size, const char* cstr, size_t cstr_leng );

#define ZnkS_copy_literal( buf, buf_size, literal_str ) \
	ZnkS_copy( (buf), (buf_size), literal_str, Znk_strlen_literal(literal_str) )

/**
 * @brief
 *  ������vsnprintf��snprintf
 *  �����̗^�����͂����悻�W���Ɠ��������AC99�̐V�@�\�ɂ��Ă�
 *  �T�|�[�g����Ă��Ȃ��ꍇ������. �߂�l�ɂ��Ă�C99�ƈقȂ�.
 *
 * @return
 *  �ŋ߂�VC(�Ƃ�����PlatformSDK�Ƃ����ׂ���)�ł�vsnprintf���T�|�[�g����Ă��邪
 *  �Â�PlatformSDK���g���Ă���ꍇ�AVC �ł� _vsnprintf ���g���K�v������. 
 *  ���� _vsnprintf �̎d�l��C99�Ƃ͈قȂ邽�߁A�����ł̖߂�l�́A�����H�v���K�v�ł���.
 *  �ȉ��̂悤�Ɏ�茈�߂�. (���L��str_len�Ƃ́Afmt �� ap �ɂ�鍇�����ʂ̕�����̒���������.)
 *
 *   buf_size == 0 �̏ꍇ
 *     0 �ȏ�̗\��������(���Lnote���Q��)���Ԃ�.
 *     buf �Ɉ�ؐG�ꂸ�Ȃɂ����Ȃ�.
 *
 *   �����G���[�����������ꍇ
 *     -1 ��Ԃ�.
 *     ���̊֐����g�����͂���ȏ�̏����͒��f����ׂ��ł���.
 *
 *   str_len < buf_size �̏ꍇ
 *     str_len ���Ԃ�.
 *     buf[ str_len ] �̈ʒu��NULL��������������.
 *
 *   str_len >= buf_size �̏ꍇ
 *     buf_size�ȏ�̗\��������(���Lnote���Q��)���Ԃ�.
 *     buf[ buf_size-1 ] �̈ʒu��NULL������������������.
 *     ���Ȃ��Ƃ�1�����ȏ�̕����񌇑����������Ă���.
 * 
 * @note:
 *  ��1)
 *  �ʏ�AC99�̎d�l�ł� buf_size �Ɋ����� 0 ���w�肷�邱�Ƃɂ��A������W�J��̐��m�ȃT�C�Y��
 *  �߂�l�Ƃ��ė\�ߒm�邱�Ƃ��ł���.
 *  �������AVC�� _vsnprintf �͂��̋@�\���Ȃ����߁A��ނ𓾂��\�������񒷂Ƃ����`�ɂ���.
 *  ����͎��Ƀ��g���C����Ƃ���buf_size�Ɏw�肷��l�Ƃ��ĉ������ė~�����\�������񒷂ł���A
 *  �����̏ꍇ�͎��ɂ��̃T�C�Y + 1���w�肷�邱�Ƃł��܂������Ǝv���邪�A�{���ɏ\���ł���Ƃ����⏞�͂Ȃ����߁A
 *  ���̃��g���C�ł����[�U�͂��̌��ʂ̖߂�l��������Ɗm�F����K�v������.
 *
 *  (�����ڍׂȂ̂Ŗ{�����̏�ɏ����ׂ��ł͂Ȃ���)�Q�l�̂��߁A���݃T�|�[�g����Ă���e�R���p�C��
 *  �����ɂǂ̂悤�Ȓl��Ԃ������ȉ��ɗ񋓂��Ă���.
 *
 *  VC�̏ꍇ:
 *    buf_size = 0 ���w�肵���ꍇ:
 *      �܂�strchr( fmt, '%' ) �ɂ��Afmt���ɂ��������%���܂܂�邩�ۂ����X�L��������.
 *      ������ % �����݂���ꍇ�� strlen( fmt ) + 512 ��Ԃ�.
 *      % �����݂��Ȃ��ꍇ�� strlen( fmt ) ��Ԃ�.
 *    buf_size > 0 ���w�肵���ꍇ:
 *      �W�J���Over���������Ȃ������ꍇ�́A�\�������񒷂ł͂Ȃ����ۂɓW�J���ꏑ�����܂ꂽ�����񒷂�Ԃ�.
 *      (�����̏ꍇ�́A�v���O���}��buf_size�\�z�͐������Ƃ��čŏ��̈��ڂ͎w�肳�ꂽbuf_size�����̂܂܎���).
 *      �W�J���Over�����������ꍇ�́A�\�������񒷂Ƃ��� MAX( strlen( fmt ) + 512, buf_size*2 ) ��Ԃ�.
 *      (�v���O���}��buf_size�\�z���O���ꍇ�́A���̏ꍇ�͑�ʂ̕����񂪗^����ꂽ�ꍇ�Ȃ̂�
 *      �T�C�Y�͋}����grow������ׂ��ƍl������)
 *
 *  GCC�Ȃ�C99�Ή����Ă���vsnprintf���Ă΂���:
 *    buf_size = 0 ���w�肵���ꍇ:
 *      C99�̎d�l�ʂ�ł���.
 *      ���m�ɓW�J��̃T�C�Y��\�������񒷂Ƃ��ĕԂ�.
 *    buf_size > 0 ���w�肵���ꍇ:
 *      C99�̎d�l�ʂ�ł���.
 *      �W�J���Over���������Ȃ������ꍇ�́A���ۂɓW�J���ꏑ�����܂ꂽ�����񒷂�Ԃ�.
 *      �W�J���Over�����������ꍇ�́A���m�ɓW�J��̃T�C�Y��\�������񒷂Ƃ��ĕԂ�.
 */
int // Znk_vsnprintf_C99 �ւ̈ڍs���I���܂ł��΂炭�̊ԁA���̌���__���������Ȗ��O�ɂ��Ă���.
ZnkS_vsnprintf_sys__( char* buf, size_t buf_size, const char* fmt, va_list ap );
int
ZnkS_snprintf_sys__( char* buf, size_t buf_size, const char* fmt, ... );


/**
 * �������10/16/8�i������, double,float �Ƃ��ĉ��߂��Č��ʂ𓾂�.
 * sscanf�̃��b�p�[�ł���.
 * ���������ꍇ��true, �����Ȃ����false��Ԃ�.
 */
bool ZnkS_getIntD( int* ans, const char* str );
bool ZnkS_getIntX( int* ans, const char* str );
bool ZnkS_getIntO( int* ans, const char* str );

bool ZnkS_getI16D( int16_t*  ans, const char* str );
bool ZnkS_getI32D( int32_t*  ans, const char* str );
bool ZnkS_getI64D( int64_t*  ans, const char* str );

bool ZnkS_getU16U( uint16_t* ans, const char* str );
bool ZnkS_getU32U( uint32_t* ans, const char* str );
bool ZnkS_getU64U( uint64_t* ans, const char* str );
bool ZnkS_getU16X( uint16_t* ans, const char* str );
bool ZnkS_getU32X( uint32_t* ans, const char* str );
bool ZnkS_getU64X( uint64_t* ans, const char* str );

bool ZnkS_getSzU( size_t* ans, const char* str );
bool ZnkS_getSzX( size_t* ans, const char* str );

bool ZnkS_getReal( double* ans, const char* str );
bool ZnkS_getRealF( float* ans, const char* str );
/**
 * �����񂪁AT, 1, true �̏ꍇ�Ɍ���true��Ԃ��B
 * ����ȊO�͂����Ȃ�ꍇ��false��Ԃ�.
 */
bool ZnkS_getBool( const char* str );

bool ZnkS_getPtrX( void** ans, const char* str );


/**
 * @brief
 *   ���̊֐��^�́Astrncmp��strncasecmp�ɂ���悤�ȂR�̈����ɉ����A
 *   �Ō�� user param ���w��\�Ȃ悤�Ɋg���������̂ł���.
 */
typedef int (*ZnkS_FuncT_StrCompare)(
		const char* /* s1 */, const char* /* s2 */, size_t /* leng */,
		void* /* user param */ );
typedef struct { ZnkS_FuncT_StrCompare func_; void* arg_; } ZnkS_FuncArg_StrCompare;

/**
 * @brief
 *   ���̊֐��^�́Ach������W���Ɋ܂܂�邩�ۂ��𔻒肷�邽�߂̂��̂ł���.
 */
typedef bool (*ZnkS_FuncT_IsKeyChar)( uint32_t ch, void* /* user param */ );
typedef struct { ZnkS_FuncT_IsKeyChar  func_; void* arg_; } ZnkS_FuncArg_IsKeyChar;

/**
 * @brief
 *   ���̊֐��^�́Aptn�Ŏw�肵���K����query���}�b�`���邩�ۂ���Ԃ�.
 */
typedef bool (*ZnkS_FuncT_IsMatch)( const char* ptn, size_t ptn_leng, const char* query, size_t query_leng );

/**
 * @brief
 *   ��2������ strncmp�^�̊֐��ւ̃|�C���^���w�肵�A������_�C���N�g��
 *   �Ăяo���悤�� ZnkS_FuncArg_StrCompare �^���쐬����.
 *   ���ʂ͑�1�����֊i�[�����.
 *   ��2�����̒l�́Astrcompare_funcarg��arg_�����o�Ɋi�[����邽�߁A
 *   ���̊֐��Ăяo����ɂ��̃����o���㏑�����邱�Ƃ͂ł��Ȃ�.
 *
 * @note
 *   ���̊֐����g������ɂ������ȉ��̂悤��ZnkS_FuncT_StrCompare�^�֐���
 *   �Ǝ��ɒ�`���A����𒼐�ZnkS_FuncArg_StrCompare��func_�֊i�[����Ƃ�����
 *   ���@���Ƃ��Ă��悢(�Ƃ�����肱�̕��@���ނ��됳���ȕ��@�ł���).
 *
 *   static int
 *   StrCompare_strncmp( const char* s1, const char* s2, size_t n, void* arg )
 *   { return strncmp( s1, s2, n ); }
 *
 *   test(){
 *     ZnkS_FuncArg_StrCompare strcompare_funcarg = { StrCompare_strncmp, NULL };
 *     // ZnkS_FuncArg_StrCompare ��v������֐�...
 *   }
 *
 *   ���̊֐��́A�W���֐��ł���strncmp�������Ăяo�������ł���A����arg��S��
 *   �g��Ȃ��悤�ȏꍇ�ɁA��L�̎�Ԃ��y�����邽�߂ɗp�ӂ��ꂽ���̂ł���.
 *   ���̏ꍇ�́A�ȉ��̂悤�ɏ�����.
 *
 *   test(){
 *     ZnkS_FuncArg_StrCompare strcompare_funcarg;
 *     ZnkS_makeStrCompare_fromStdFunc( &strcompare_funcarg, strncmp );
 *     // ���̌�Astrcompare_funcarg.arg_ �͕ύX�s��.
 *     // ZnkS_FuncArg_StrCompare ��v������֐�...
 *   }
 */
void
ZnkS_makeStrCompare_fromStdFunc(
		ZnkS_FuncArg_StrCompare* strcompare_funcarg,
		int (*strncmp_func)( const char*, const char*, size_t ) );

/**
 * �������r�֐��̈�ʉ�
 */
bool
ZnkS_compareBegin(
		const char* str, size_t str_leng,
		const char* ptn, size_t ptn_leng, const ZnkS_FuncArg_StrCompare* strcompare_funcarg );
bool
ZnkS_compareEnd(
		const char* str, size_t str_leng,
		const char* ptn, size_t ptn_leng, const ZnkS_FuncArg_StrCompare* strcompare_funcarg );
bool
ZnkS_compareContain(
		const char* str, size_t str_leng, size_t pos,
		const char* ptn, size_t ptn_leng, const ZnkS_FuncArg_StrCompare* strcompare_funcarg );

/***
 * strcmp/strncmp���g�����������r
 */
Znk_INLINE bool
ZnkS_eq( const char* s1, const char* s2 ){
	return (bool)( Znk_strcmp( s1, s2 ) == 0 );
}
Znk_INLINE bool
ZnkS_eqEx( const char* s1, const char* s2, size_t leng ){
	return (bool)( Znk_strncmp( s1, s2, leng ) == 0 );
}
bool
ZnkS_isBegin( const char* str, const char* ptn );
bool
ZnkS_isEnd( const char* str, const char* ptn );
bool
ZnkS_isBeginEx( const char* str, const size_t str_leng, const char* ptn, size_t ptn_leng );
bool
ZnkS_isEndEx( const char* str, const size_t str_leng, const char* ptn, size_t ptn_leng );

#define ZnkS_isBegin_literal( str, ptn_literal ) \
	ZnkS_isBeginEx( (str), Znk_NPOS, ptn_literal, Znk_strlen_literal(ptn_literal) )
#define ZnkS_isEnd_literal( str, ptn_literal ) \
	ZnkS_isEndEx( (str), Znk_NPOS, ptn_literal, Znk_strlen_literal(ptn_literal) )

/***
 * strcasecmp/strncasecmp���g�����������r
 */
Znk_INLINE bool
ZnkS_eqCase( const char* s1, const char* s2 )
{
	return (bool)( ZnkS_strcasecmp( s1, s2 ) == 0 );
}
Znk_INLINE bool
ZnkS_eqCaseEx( const char* s1, const char* s2, size_t leng )
{
	return (bool)( ZnkS_strncasecmp( s1, s2, leng ) == 0 );
}
bool
ZnkS_isCaseBegin( const char* str, const char* ptn );
bool
ZnkS_isCaseEnd( const char* str, const char* ptn );
bool
ZnkS_isCaseBeginEx( const char* str, size_t str_leng, const char* ptn, size_t ptn_leng );
bool
ZnkS_isCaseEndEx( const char* str, size_t str_leng, const char* ptn, size_t ptn_leng );

#define ZnkS_isCaseBegin_literal( str, ptn_literal ) \
	ZnkS_isCaseBeginEx( (str), Znk_NPOS, ptn_literal, Znk_strlen_literal(ptn_literal) )
#define ZnkS_isCaseEnd_literal( str, ptn_literal ) \
	ZnkS_isCaseEndEx( (str), Znk_NPOS, ptn_literal, Znk_strlen_literal(ptn_literal) )

/**
 * @brief
 *   str �̊J�n�ɂ���L�[���[�h�� ptn �ƃ}�b�`���邩�ǂ����𒲂ׂ�.
 *
 *   �����Łu�}�b�`����v�Ƃ́Astr��ptn�Ŏw�肳��镶���񂩂�n�܂��Ă���A
 *   ���A���̒���̓L�[���[�h�����W���ȊO�̕����ɂȂ��Ă���ꍇ������.
 *   �܂�A�t�Ɍ�����str��ptn�Ŏw�肳��镶���񂩂�n�܂��Ă����Ƃ��Ă��A
 *   str�̂��̒���̈ʒu�ɂ����Ă���ɃL�[���[�h�����W���Ɋ܂܂�镶����
 *   �A�����đ����Ă���ꍇ�́A�u�}�b�`����v�Ƃ݂͂Ȃ��Ȃ�.
 *   ����͈ȉ��̗�����������킩��₷�����낤. ���̗�ł̓L�[���[�h�����W����
 *   ���ׂăA���t�@�x�b�g�������ł���Ƃ���.
 *
 *   ��1. str="function", ptn="func"
 *     ���̏ꍇ�́Afunc�Ŏn�܂��Ă͂��邪�A��������'t'�Ƃ����L�[���[�h�����W��
 *     �Ɋ܂܂�镶���������̂Ń}�b�`���Ă���Ƃ݂͂Ȃ��Ȃ�.
 *
 *   ��2. str="func:tion", ptn="func"
 *     ���̏ꍇ�́Afunc�Ŏn�܂��Ă���A���A��������':'�Ƃ����L�[���[�h�����W��
 *     �Ɋ܂܂�Ȃ������������̂Ń}�b�`���Ă���.
 *
 *   �L�[���[�h�����W���̒�`�́Akeychars�ŗ^���邱�Ƃɂ��s��.
 *
 * @return
 *   str �̊J�n�� ptn �ƃ}�b�`����� true ��Ԃ�. �����Ȃ���� false ��Ԃ�.
 */
/**
 * @brief
 *   str �̊J�n�ɂ���L�[���[�h�� ptn �ƃ}�b�`���邩�ǂ����𒲂ׂ�.
 *   �L�[���[�h�̒�`�́A�q��֐���^���邱�Ƃɂ��s��.
 *
 * @param predicate_func
 *   ���̕������L�[���[�h�����ł���� true ���A
 *   �����Ȃ���� false ��Ԃ��悤�ȏq��֐��ւ̃|�C���^���w�肷��.
 *   �����NULL�ɂ����ꍇ�́AZnkS_isBegin�Ƌ������S�������ɂȂ���̂Ƃ���.
 *
 * @return
 *   str �̊J�n�� ptn �ƃ}�b�`����� true ��Ԃ�. �����Ȃ���� false ��Ԃ�.
 */
bool
ZnkS_isMatchBeginEx(
		const char* str, size_t str_leng,
		const char* ptn, size_t ptn_leng,
		const ZnkS_FuncArg_IsKeyChar*  iskeychar_funcarg,
		const ZnkS_FuncArg_StrCompare* strcompare_funcarg );
bool
ZnkS_isMatchBegin( const char* str, const char* ptn, const char* keychars );
bool
ZnkS_isMatchSWC( const char* ptn, size_t ptn_leng,
		const char* query, size_t query_leng );


/**
 * @brief
 *   4�����ȉ�/8�����ȉ��̒����̕�����s�����̂܂܃f�V���A���C�Y����32bit/64bit�����l
 *   �Ƃ��ĕԂ�. �����ł�Endian���͈�؍l���Ȃ�. ���������āA�擾����鐮���̎Z�p�I�Ӗ��́A
 *   LE��BE�̊��ɂ���ĈقȂ錋�ʂɂȂ�. �����ł͂�����������̂��߂�ID�ƍl����.
 *
 * @exam
 *   // �ȉ���ZnkS_get_id32�̏ꍇ�̗�ł��邪�AZnkS_get_id64�̏ꍇ��4�����̂Ƃ����8�����ɓǂݑւ��ė~����.
 *   // type_str4��4�����ȓ��̕����񂪗^������Ɖ��肵�A���ꂻ���蒷���ꍇ�́A4�����őł��؂�.
 *   bool checkType( const char* type_str4 )
 *   {
 *     static uint32_t TEXT = 0; 
 *     static uint32_t PICT = 0; 
 *     static bool     initialized = false;
 *
 *     if( !initialized ){
 *       // ���̊֐������߂Ď��s�����ꍇ�́A�����p�^�[����������.
 *       TEXT = ZnkS_get_id32( "TEXT", Znk_NPOS ); 
 *       PICT = ZnkS_get_id32( "PICT", Znk_NPOS ); 
 *       initialized = true;
 *     }
 *
 *     // �ȉ��A�����ł̔�r���\(�������r��荂��).
 *     // ���̗�ł̓p�^�[����TEXT��PICT�̓�����Ȃ��̂Œm��Ă��邪�A
 *     // �p�^�[���̐�����ʂ��A���̔�r�����A���^�C���ɏo���邾�������ɍs��Ȃ���΂Ȃ�Ȃ��ꍇ���Ɍ��ʂ����҂ł���.
 *     // ���̎��4����ID�̃e�N�j�b�N��Adobe Photoshop File �̃t�H�[�}�b�g�Ȃǂɂ�������.
 *     {type
 *       const uint32_t type_id32 = ZnkS_get_id32( type_str4, Znk_NPOS ); 
 *       if( type_id32 == TEXT ){
 *         ...
 *       } else if( type_id32 == PICT ){
 *         ...
 *       }
 *     }
 *   }
 *    
 */
uint32_t ZnkS_get_id32( const char* s, size_t leng );
uint64_t ZnkS_get_id64( const char* s, size_t leng );

/**
 * @brief
 *   C������str(�T�^�I�ɂ̓t�@�C���p�X�Ȃ�)�ƃZ�p���[�^�ƂȂ镶��dot_ch���w�肵�A
 *   str�ɂ�����dot_ch���Ō�Ɍ����ʒu�̎��̈ʒu�������|�C���^��Ԃ�.
 *   str�ɂ�����dot_ch���܂܂�Ă��Ȃ��ꍇ�͊g���q�͋�ł���ƍl���A�����񃊃e����
 *   "" ��Ԃ�(NULL�ł͂Ȃ����Ƃɒ���). �܂��Adot_ch�Ƃ���NULL����'\0'(=0)���w��
 *   �����ꍇ�A����Ƃ��ē����� �����񃊃e���� "" ��Ԃ�.
 *
 * @note
 *   �����Ȃ�ꍇ���`�F�b�N���邱�ƂȂ��ɖ߂�l�����̂܂�strcmp�n�̊֐��ֈ����n��
 *   ���Ƃ��\�ł���Astrrchr�֐��Ɣ�ׂ��ꍇ�̗��_�Ƃ��ċ�������.
 */
const char* ZnkS_get_extension( const char* str, char dot_ch );

/**
 * @brief
 *   C������str���̍ŏ��ɏo��������s�R�[�h��������
 *   �����ŋ����I��NUL�I�[������.
 */
Znk_INLINE void
ZnkS_chompNL( char* str ){
	while( *str ){
		switch( *str ){
		case '\n': case '\r':
			*str = '\0';
			return;
		default: break;
		}
		++str;
	}
}

/**
 * ZnkS_lfind_one_of:
 *  str �ɂ����� [ begin, end ) �͈̔͂ɂ��镶����ɂ����āAbegin�̈ʒu����o�����A
 *  �������փX�L�������Ă����Achset �Ŏw�肵�������̂����ꂩ���ŏ��Ɍ����ʒu��Ԃ�.
 *  �o���_�ł��� begin �̈ʒu�ɂ����Ȃ茩����P�[�X�����蓾��.
 *  end �ʒu�ɓ��B���Ă�������Ȃ��ꍇ�� Znk_NPOS��Ԃ�.
 *
 * ZnkS_lfind_one_not_of:
 *  str �ɂ����� [ begin, end ) �͈̔͂ɂ��镶����ɂ����āAbegin�̈ʒu����o�����A
 *  �������փX�L�������Ă����Achset �Ŏw�肵�������ȊO���ŏ��Ɍ����ʒu��Ԃ�.
 *  �o���_�ł��� begin �̈ʒu�ɂ����Ȃ茩����P�[�X�����蓾��.
 *  end �ʒu�ɓ��B���Ă�������Ȃ��ꍇ�� Znk_NPOS��Ԃ�.
 *
 * ZnkS_rfind_one_of:
 *  str �ɂ����� [ begin, end ) �͈̔͂ɂ��镶����ɂ����āAend-1 �̈ʒu����o�����A
 *  �t�����փX�L�������Ă����Achset �Ŏw�肵�������̂����ꂩ���ŏ��Ɍ����ʒu��Ԃ�.
 *  �o���_�ł��� begin �̈ʒu�ɂ����Ȃ茩����P�[�X�����蓾��.
 *  end �������ʒu�͒��ׂ��Ȃ����Ƃɒ��ӂ���.
 *  begin-1 �̈ʒu�ɓ��B���Ă�������Ȃ��ꍇ�� Znk_NPOS��Ԃ�.
 *
 * ZnkS_rfind_one_not_of:
 *  str �ɂ����� [ begin, end ) �͈̔͂ɂ��镶����ɂ����āAend-1 �̈ʒu����o�����A
 *  �t�����փX�L�������Ă����Achset �Ŏw�肵�������ȊO���ŏ��Ɍ����ʒu��Ԃ�.
 *  �o���_�ł��� begin �̈ʒu�ɂ����Ȃ茩����P�[�X�����蓾��.
 *  end �������ʒu�͒��ׂ��Ȃ����Ƃɒ��ӂ���.
 *  begin-1 �̈ʒu�ɓ��B���Ă�������Ȃ��ꍇ�� Znk_NPOS��Ԃ�.
 *
 * @param str:
 *  �����ΏۂƂȂ�o�C�g��ł���.
 *  C������ȊO�ł��悢.
 *
 * @param begin:
 *  �����ΏۂƂȂ�o�C�g��̊J�n�ʒu���w�肷��.
 *  ������ Znk_NPOS���w�肳�ꂽ�ꍇ�͉��������� Znk_NPOS ��Ԃ�.
 *
 * @param end:
 *  �����ΏۂƂȂ�o�C�g��̏I���ʒu(�Ō�̕�����̎��̈ʒu)���w�肷��.
 *  str��C������ł���(NULL�I�[���Ă���ꍇ)�̂݁A������Znk_NPOS ���w�肷�邱�Ƃ��ł��A
 *  ���̏ꍇ�� strlen(str)���w�肳�ꂽ���̂Ƃ݂Ȃ����.
 *  ������ 0 ���w�肳�ꂽ�ꍇ�� end <= begin �ł������ꍇ�́A�������� Znk_NPOS��Ԃ�.
 *
 * @param chset
 *  �����p�^�[���ƂȂ镶���̏W�����`����o�C�g���^����.
 *  C������ȊO�ł��悢.
 *
 * @param chset_size
 *  chset �̌�(�o�C�g�T�C�Y)���w�肷��.
 *  chset ��C������ł���(NULL�I�[���Ă���ꍇ)�̂݁A������Znk_NPOS ���w�肷�邱�Ƃ��ł��A
 *  ���̏ꍇ�� strlen(chset)���w�肳�ꂽ���̂Ƃ݂Ȃ����.
 *
 * @return
 *  ���߂�ׂ��p�^�[���̌��������ʒu��Ԃ�.
 *  ���ꂪ������Ȃ������ꍇ�� begin �� end �̒l����O�I�ł������ꍇ�� Znk_NPOS ��Ԃ�.
 *
 * @note
 *  ZnkS_lfind_one_of     : std::string��find_first_of�Ɠ���
 *  ZnkS_lfind_one_not_of : std::string��find_first_not_of�Ɠ���
 *  ZnkS_rfind_one_of     : std::string��find_last_of�Ɠ���
 *  ZnkS_rfind_one_not_of : std::string��find_last_not_of�Ɠ���
 */
size_t
ZnkS_lfind_one_of( const char* str,
		size_t begin, size_t end, const char* chset, size_t chset_size );
size_t
ZnkS_lfind_one_not_of( const char* str,
		size_t begin, size_t end, const char* chset, size_t chset_size );
size_t
ZnkS_rfind_one_of( const char* str,
		size_t begin, size_t end, const char* chset, size_t chset_size );
size_t
ZnkS_rfind_one_not_of( const char* str,
		size_t begin, size_t end, const char* chset, size_t chset_size );

Znk_INLINE void
ZnkS_find_side_skip( const char* str, size_t begin, size_t end,
		size_t* ans_begin, size_t* ans_end,
		const char* chset, size_t chset_leng )
{
	/***
	 * begin, end �̊֌W�`�F�b�N, chset_leng �� Znk_NPOS�ł���ꍇ�̃`�F�b�N�ɂ��ẮA
	 * ZnkS_lfind_one_not_of �Ȃ�т� ZnkS_rfind_one_not_of �̓����Ŏ����I�ɍs���邽�߁A
	 * �����ł͕s�v�ł���.
	 */
	*ans_begin = ZnkS_lfind_one_not_of( str, begin, end, chset, chset_leng );
	if( *ans_begin == Znk_NPOS ){ *ans_end = Znk_NPOS; return; }
	/***
	 * end ����t�����֒��ׁAchset �ł͂Ȃ������������ʒu�𒲂ׂ�.
	 * *ans_begin �ɂ��镶���� chset �Ɋ܂܂�Ȃ����߁A���̏����ł� not found �͋N���肦�Ȃ�.
	 * ans_end �� �ړI�̃g�[�N���Ō�̕����̎��̈ʒu���������̂ł��邽�߁A+1 ������ ans_end �͊m�肷��.
	 */
	*ans_end = ZnkS_rfind_one_not_of( str, *ans_begin, end, chset, chset_leng ) + 1;
}

/**
 * str �� [ begin, end ) �Ŏ������͈͂ɂ����āA������ delimiter ������ŋ�؂�ꂽ
 * ������̂悤�Ȍ`���̕����񂪂���Ƃ���. ���̂Ƃ��A���ӂɊY������ key �̕����� �E�ӂ�
 * �Y������ val �̕����͈̔͂��Askip ��������l�������`�ŃT�[�`����.
 *
 * �����ł͐����̂��߁A���͈̔͂ɂ��镶���񂪗Ⴆ�Έȉ��̂悤�Ȃ��̂ł���ꍇ���l����.
 * delimiter ������� "<=" �Ƃ��Askip_chset = " \t" (whitespace�ƃ^�u����)�Ƃ���.
 *
 *  "  My Key <= My Val  "
 *   01234567890123456789
 *
 * ���̂Ƃ��Akey �ɊY�����镔���� "My Key" �ł���Aval �ɊY�����镔���� "My Val" �ƂȂ�.
 * �����̗����ɑ��݂��� whitespace �݂̂����������K�v������( "My Key" �̂悤��
 * skip�����񂪓r���Ɋ܂܂��ꍇ�͏����̑ΏۊO�ł��� ).
 * ����āA���̏ꍇ�擾����� key �͈̔͂� [2, 8)�Aval �͈̔͂� [12, 18) �ƂȂ�.
 * �܂����̊֐��� delimiter �̊J�n�ʒu��Ԃ�. ���̗�̏ꍇ�� 9 �ƂȂ�.
 *
 * @note
 *  key_begin, key_end, val_begin, val_end �� NULL ���w�肷�邱�Ƃ͂ł��Ȃ�.
 *  �]���Ă��ꂪ�s�v�ȏꍇ�ł��_�~�[�ϐ��̃A�h���X���w�肷��K�v������.
 */
size_t
ZnkS_find_key_and_val( const char* str, size_t begin, size_t end,
		size_t* key_begin, size_t* key_end,
		size_t* val_begin, size_t* val_end,
		const char* delimiter,  size_t delimiter_leng,
		const char* skip_chset, size_t skip_chset_leng );

/**
 * str �� [ begin, end ) �Ŏ������͈͂ɂ����āA������X�y�[�X�����ŋ�؂�ꂽ
 * �g�[�N��������̗��񂪂���Ƃ��āA�����̂����ŏ����琔���� arg_idx �Ԗڂɂ���
 * �g�[�N���ɂ�����J�n�ʒu�ƕ����񒷂����߂�. �߂�l�͂��̃g�[�N���̊J�n�ʒu�ƂȂ�A
 * arg_leng �ɂ����Ă��̃g�[�N���̕����񒷂��i�[�����.
 * ���̏ꍇ�� arg_idx �̎w��� 0 �̏ꍇ��ԍŏ��Ɉʒu����g�[�N���A 1 �̏ꍇ��
 * 2 �ԖڂɈʒu����g�[�N���������A�ȉ����l�Ƃ���.
 * arg_idx�Ŏw�肵���g�[�N����������Ȃ������ꍇ�� Znk_NPOS ��Ԃ��Aarg_leng�ɂ�
 * 0���Z�b�g�����.
 *
 * @note
 *  arg_leng �� NULL ���w�肷�邱�Ƃ͂ł��Ȃ�.
 *  �]���Ă��ꂪ�s�v�ȏꍇ�ł��_�~�[�ϐ��̃A�h���X���w�肷��K�v������.
 */
size_t
ZnkS_lfind_arg( const char* str, size_t begin, size_t end,
		size_t arg_idx, size_t* arg_leng,
		const char* skip_chset, size_t skip_chset_leng );
/**
 * str �� [ begin, end ) �Ŏ������͈͂ɂ����āA������X�y�[�X�����ŋ�؂�ꂽ
 * �g�[�N��������̗��񂪂���Ƃ��āA�����̂����Ōォ�琔���� arg_idx �Ԗڂɂ���
 * �g�[�N���ɂ�����J�n�ʒu�ƕ����񒷂����߂�. �߂�l�͂��̃g�[�N���̊J�n�ʒu�ƂȂ�A
 * arg_leng �ɂ����Ă��̃g�[�N���̕����񒷂��i�[�����.
 * ���̏ꍇ�� arg_idx �̎w��� 0 �̏ꍇ��ԍŌ�Ɉʒu����g�[�N���A 1 �̏ꍇ��
 * ��ԍŌォ�� 2 �ԖڂɈʒu����g�[�N���������A�ȉ����l�Ƃ���.
 * arg_idx�Ŏw�肵���g�[�N����������Ȃ������ꍇ�� Znk_NPOS ��Ԃ��Aarg_leng�ɂ�
 * 0���Z�b�g�����.
 *
 * @note
 *  arg_leng �� NULL ���w�肷�邱�Ƃ͂ł��Ȃ�.
 *  �]���Ă��ꂪ�s�v�ȏꍇ�ł��_�~�[�ϐ��̃A�h���X���w�肷��K�v������.
 */
size_t
ZnkS_rfind_arg( const char* str, size_t begin, size_t end,
		size_t arg_idx, size_t* arg_leng,
		const char* skip_chset, size_t skip_chset_leng );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
