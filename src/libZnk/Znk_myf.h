#ifndef INCLUDE_GUARD__Znk_myf_h__
#define INCLUDE_GUARD__Znk_myf_h__

#include <Znk_var.h>
#include <Znk_str_ary.h>
#include <Znk_varp_ary.h>
#include <Znk_primp_ary.h>

Znk_EXTERN_C_BEGIN

typedef struct ZnkMyfSectionImpl* ZnkMyfSection;
typedef struct ZnkMyfImpl* ZnkMyf;

typedef enum {
	 ZnkMyfSection_e_None = 0
	,ZnkMyfSection_e_Lines
	,ZnkMyfSection_e_Vars
	,ZnkMyfSection_e_Prims
	,ZnkMyfSection_e_OutOfSection
} ZnkMyfSectionType;

ZnkStrAry
ZnkMyfSection_lines( const ZnkMyfSection sec );

ZnkVarpAry
ZnkMyfSection_vars( const ZnkMyfSection sec );

ZnkPrimpAry
ZnkMyfSection_prims( const ZnkMyfSection sec );

const char*
ZnkMyfSection_name( const ZnkMyfSection sec );
ZnkMyfSectionType
ZnkMyfSection_type( const ZnkMyfSection sec );


ZnkMyf
ZnkMyf_create( void );
void
ZnkMyf_destroy( ZnkMyf myf );
void
ZnkMyf_clear( ZnkMyf myf );

const char*
ZnkMyf_quote_begin( const ZnkMyf myf );
const char*
ZnkMyf_quote_end( const ZnkMyf myf );
void
ZnkMyf_set_quote( ZnkMyf myf, const char* quote_begin, const char* quote_end );

/**
 * @brief
 * �����̃Z�N�V�������݂��m�[�`�F�b�N�ŐV�K�Z�N�V�������Ō���ɒǉ����A�����Ԃ�.
 *
 * @note
 * �ł����������d���`�F�b�N���Ȃ���Ȃ����߁Amyf�f�[�^�̐������ɖ����N�����\��������.
 * �g�p�͗^����f�[�^�ɏd�����Ȃ��Ɗm���ɂ킩���Ă���ꍇ�Ɍ��邱��.
 */
ZnkMyfSection
ZnkMyf_addSection( ZnkMyf myf, const char* sec_name, ZnkMyfSectionType sec_type );
/**
 * @brief
 * �����̃Z�N�V���������݂����ꍇ�͂������U�폜���A�V�����m�ۂ����V�K�Z�N�V���������̈ʒu�ɏ㏑�����A�����Ԃ�.
 * ���݂��Ȃ��ꍇ�͐V�K�Z�N�V�������Ō���ɒǉ����A�����Ԃ�.
 *
 * @note
 * ���̊֐��ł͑��ݐ��̃`�F�b�N��sec_name�����ōs��.
 * sec_type�͔�r����Ȃ�(�܂�sec_type���قȂ�sec_name�������ꍇ�ł������Z�N�V�����ƌ��Ȃ�).
 */
ZnkMyfSection
ZnkMyf_registSection( ZnkMyf myf, const char* sec_name, ZnkMyfSectionType sec_type );
/**
 * @brief
 * �����̃Z�N�V���������݂����ꍇ�͂������������̂܂ܕԂ�.
 * ���݂��Ȃ��ꍇ�͐V�K�Z�N�V�������Ō���ɒǉ����A�����Ԃ�.
 *
 * @note
 * ���̊֐��ł͑��ݐ��̃`�F�b�N��sec_name�����ōs��.
 * sec_type�͔�r����Ȃ�(�܂�sec_type���قȂ�sec_name�������ꍇ�ł������Z�N�V�����ƌ��Ȃ�).
 */
ZnkMyfSection
ZnkMyf_internSection( ZnkMyf myf, const char* sec_name, ZnkMyfSectionType sec_type );

size_t
ZnkMyf_numOfSection( const ZnkMyf myf );
ZnkMyfSection
ZnkMyf_atSection( const ZnkMyf myf, size_t sec_idx );
size_t
ZnkMyf_findIdx_withSecType( const ZnkMyf myf, const char* sec_name, ZnkMyfSectionType sec_type );
size_t
ZnkMyf_findIdx( const ZnkMyf myf, const char* sec_name );
bool
ZnkMyf_erase_byIdx( ZnkMyf myf, size_t sec_idx );

bool
ZnkMyf_load( ZnkMyf myf, const char* filename );
bool
ZnkMyf_save( ZnkMyf myf, const char* filename );

ZnkVarp
ZnkMyf_refVar( ZnkMyf myf, const char* sec_name, const char* var_name );

Znk_INLINE ZnkMyfSection
ZnkMyf_findSection( const ZnkMyf myf, const char* sec_name, ZnkMyfSectionType sec_type )
{
	const size_t sec_idx = ZnkMyf_findIdx_withSecType( myf, sec_name, sec_type );
	return ( sec_idx == Znk_NPOS ) ? NULL : ZnkMyf_atSection( myf, sec_idx );
}
Znk_INLINE ZnkStrAry
ZnkMyf_find_lines( const ZnkMyf myf, const char* sec_name )
{
	ZnkMyfSection sec = ZnkMyf_findSection( myf, sec_name, ZnkMyfSection_e_Lines );
	return sec ? ZnkMyfSection_lines( sec ) : NULL;
}
Znk_INLINE ZnkVarpAry
ZnkMyf_find_vars( const ZnkMyf myf, const char* sec_name )
{
	ZnkMyfSection sec = ZnkMyf_findSection( myf, sec_name, ZnkMyfSection_e_Vars );
	return sec ? ZnkMyfSection_vars( sec ) : NULL;
}
Znk_INLINE ZnkPrimpAry
ZnkMyf_find_prims( const ZnkMyf myf, const char* sec_name )
{
	ZnkMyfSection sec = ZnkMyf_findSection( myf, sec_name, ZnkMyfSection_e_Prims );
	return sec ? ZnkMyfSection_prims( sec ) : NULL;
}

Znk_INLINE ZnkStrAry
ZnkMyf_intern_lines( const ZnkMyf myf, const char* sec_name )
{
	ZnkMyfSection sec = ZnkMyf_internSection( myf, sec_name, ZnkMyfSection_e_Lines );
	return ZnkMyfSection_lines( sec );
}
Znk_INLINE ZnkVarpAry
ZnkMyf_intern_vars( const ZnkMyf myf, const char* sec_name )
{
	ZnkMyfSection sec = ZnkMyf_internSection( myf, sec_name, ZnkMyfSection_e_Vars );
	return ZnkMyfSection_vars( sec );
}
Znk_INLINE ZnkPrimpAry
ZnkMyf_intern_prims( const ZnkMyf myf, const char* sec_name )
{
	ZnkMyfSection sec = ZnkMyf_internSection( myf, sec_name, ZnkMyfSection_e_Prims );
	return ZnkMyfSection_prims( sec );
}

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
