#ifndef INCLUDE_GUARD__Znk_str_ex_h__
#define INCLUDE_GUARD__Znk_str_ex_h__

#include <Znk_base.h>
#include <Znk_str.h>
#include <Znk_str_ary.h>
#include <Znk_stdc.h>

Znk_EXTERN_C_BEGIN

Znk_INLINE char*
ZnkStrEx_strchr( ZnkStr str, char c ){
	return Znk_strchr( ZnkStr_cstr( str ), c );
}
Znk_INLINE char*
ZnkStrEx_strrchr( ZnkStr str, char c ){
	return Znk_strrchr( ZnkStr_cstr( str ), c );
}
Znk_INLINE char*
ZnkStrEx_strstr( ZnkStr str, const char* ptn ){
	return Znk_strstr( ZnkStr_cstr( str ), ptn );
}

void
ZnkStrEx_addSplitC( ZnkStrAry ans_list,
		const char* str, size_t str_leng,
		char delimit_c, const bool count_void_str, size_t expect_size );


void
ZnkStrEx_addSplitCSet( ZnkStrAry ans_list,
		const char* str,   size_t str_leng,
		const char* chset, size_t chset_leng,
		size_t expect_size );

void
ZnkStrEx_addSplitStr( ZnkStrAry ans_vstr, const char* str, size_t str_leng,
		const char* delimiter, size_t delimiter_leng,
		const bool count_void_str, size_t expect_size );

void
ZnkStrEx_addJoin( ZnkStr ans, const ZnkStrAry str_list,
		size_t begin, size_t end,
		const char* connector, size_t connector_leng, size_t expect_elem_leng );

void
ZnkStrEx_addQuotedTokens( ZnkStrAry ans_vstr,
		const char* str, size_t str_leng,
		const char* begin_quot, size_t begin_quot_leng,
		const char* end_quot,   size_t end_quot_leng,
		size_t expect_size );

/**
 * str ��^���A���� begin�Ŏw�肵���ʒu�ȍ~���X�L�������Aptn �Ŏw�肵��
 * �p�^�[���������ŏ��̈ʒu�� end �Ƃ���. ���̂Ƃ� str �ɂ�����
 * [ begin, end ) �͈̔͂̕�����������ł� Front �ƌĂсA���̊֐��ł�
 * ������擾����. ������ ptn �����݂��Ȃ��ꍇ�� end �� Znk_NPOS�ƂȂ�A
 * ���̏ꍇ Front �� [ begin, str_leng ) �܂ł͈̔͂��������̂Ƃ���.
 *
 * @param begin
 *  str �ɂ�����X�L�����J�n�ʒu.
 *  Znk_NPOS �� str_leng ���z����l���w�肵���ꍇ�AFront�͋�Ƃ݂Ȃ����.
 *  ���Ȃ킿 begin�̒l���̂��̂��Ԃ�Aans�ɂ͉����A������Ȃ�.
 *
 * @param ans:
 *  ������ ��NULL�� ZnkStr�l���w�肵���ꍇ�́A��L�ɂ����� Front �̕�����
 *  ans �̌��ɘA�������. ans �� NULL�̏ꍇ�͂��̏����͏ȗ������.
 *
 * @return 
 *  end ��Ԃ�.
 *  �������Aptn�����݂��Ȃ������ꍇ�� Znk_NPOS ��Ԃ�.
 *  ���̂Ƃ� ans �ɂ͏�L�Ɏ���Front�͈̔͂��A���������̂Ƃ���.
 *  �����ŁAZnk_NPOS �̑���� str_leng��Ԃ��d�l�ɂ��Ă��悩�������A���̏ꍇ
 *  �Ăяo������ str �̎��ۂ̒�������ɔc�����Ă����K�v����������. ����͏ꍇ��
 *  ����Ă͖��ʂȂ̂ŁAZnk_NPOS��Ԃ��d�l�ɗ��������`�ƂȂ���.
 *
 *  �����l�Ƃ��� begin < str_leng && ptn_leng ����������Ȃ������ꍇ�́A
 *  ptn �̒l�����ł���Abegin �̒l���̂��̂��Ԃ�(���̂Ƃ� ans �ɂ�
 *  �����A������Ȃ�).
 *
 * @exam
 *  str = 'ABCdefghiJKLmn'
 *  begin=3,
 *  pattern='JKL'
 *  end(�߂�l) = 9
 *  ans = 'defghi'
 */
size_t
ZnkStrEx_getFront_byPattern( const char* str, size_t str_leng,
		size_t begin, const char* ptn, size_t ptn_leng,
		ZnkStr ans );

size_t
ZnkStrEx_getBack_byPattern( const char* str, size_t str_leng,
		size_t begin, const char* ptn, size_t ptn_leng,
		ZnkStr ans );


size_t
ZnkStrEx_getKeyAndValEx( const char* str, size_t begin, size_t end,
		size_t* ans_key_begin, size_t* ans_key_end,
		size_t* ans_val_begin, size_t* ans_val_end,
		const char* delimiter,  size_t delimiter_leng,
		const char* skip_chset, size_t skip_chset_leng,
		ZnkStr ans_key, ZnkStr ans_val );
Znk_INLINE size_t
ZnkStrEx_getKeyAndVal( const char* str, size_t begin, size_t end,
		const char* delimiter, const char* skip_chset,
		ZnkStr ans_key, ZnkStr ans_val )
{
	return ZnkStrEx_getKeyAndValEx( str, begin, end,
			NULL, NULL, NULL, NULL,
			delimiter,  Znk_NPOS,
			skip_chset, Znk_NPOS,
			ans_key, ans_val );
}

bool
ZnkStrEx_removeFrontCSet( ZnkStr str, const char* skip_chset, size_t skip_chset_leng );
bool
ZnkStrEx_removeBackCSet( ZnkStr str, const char* skip_chset, size_t skip_chset_leng );
/**
 * @brief
 * ZnkStrEx_removeBackCSet �� ZnkStrEx_removeFrontCSet �����̏��Ԃōs��.
 */
Znk_INLINE void 
ZnkStrEx_removeSideCSet( ZnkStr str, const char* skip_chset, size_t skip_chset_leng ){
	ZnkStrEx_removeBackCSet ( str, skip_chset, skip_chset_leng );
	ZnkStrEx_removeFrontCSet( str, skip_chset, skip_chset_leng );
}

bool 
ZnkStrEx_chompStr( ZnkStr str, size_t pos, const char* ptn, size_t ptn_leng );
Znk_INLINE bool 
ZnkStrEx_chompFirstStr( ZnkStr str, const char* ptn, size_t ptn_leng ){
	return ZnkStrEx_chompStr( str, 0, ptn, ptn_leng );
}
Znk_INLINE bool 
ZnkStrEx_chompLastStr( ZnkStr str, const char* ptn, size_t ptn_leng ){
	return ZnkStrEx_chompStr( str, Znk_NPOS, ptn, ptn_leng );
}

void
ZnkStrEx_addRepeatC( ZnkStr ans, char c, size_t num );

void
ZnkStrEx_addEmbededStr( ZnkStr ans,
		char c, size_t num,
		const char* emb_str, size_t emb_str_leng,
		char positioning_mode );

/**
 * @brief
 *   str ���ɂ��� old_ptn �� seek_depth �Ŏw�肵���񐔕� new_ptn �ɒu��������.
 *
 * @param str:
 *   replace�ΏۂƂȂ镶����.
 *
 * @param begin:
 *   str�ɂ����Ă��̈ʒu����Ō�܂ł�replace�����͈̔͂Ƃ���.
 *   (�܂肱����O�͈̔͂͑ΏۊO)
 *
 * @param old_ptn:
 *   str���ɂ��̃p�^�[�����o�������Ȃ�A���̕�����new_ptn�Œu��������.
 *
 * @param old_ptn_leng:
 *   old_ptn �̕�����.
 *
 * @param new_ptn:
 *   str����old_ptn�p�^�[�����o�������ꍇ�ɒu��������ׂ��V����������p�^�[��.
 *
 * @param new_ptn_leng:
 *   new_ptn �̕�����.
 *
 * @param seek_depth:
 *   �u������������������E�։�����s���邩���w�肷��.
 *   �Ⴆ�� 1 �̏ꍇ�͈�ԍŏ��Ɍ��ꂽ old_ptn �����u������Ȃ�.
 *   Znk_NPOS ���w�肵���ꍇ�͕����񒆂ɏo�����邷�ׂĂ� old_ptn ���u�������.
 *
 * @param delta_to_next:
 *   �u�������������s��������A�J�[�\�����ǂꂾ���E�ֈړ������邩������.
 *   Znk_NPOS ���w�肵���ꍇ��new_ptn_leng �������ړ�����.
 *
 * @note:
 *   delta_to_next�ɂ���:
 *   ���ꂪ�Ȃ��K�v���ɂ��ẮA�Ⴆ�Έȉ��̂悤�ȃP�[�X���l����Ƃ킩��Ղ�.
 *   /../../../../ �Ƃ����悤�Ȑe�f�B���N�g���u..�v�������p�^�[�������ꂽ�ꍇ��
 *   ���������ׂ� /__/__/__/__/ �̂悤�ɒu���������Ƃ���. �Ƃ肠�����m���� / �Ɉ͂܂ꂽ�p�^�[���݂̂�
 *   �u�����邽�߁Aold_ptn�Ƃ��� /../  new_ptn�Ƃ��� /__/ ���w�肵���Ƃ���.
 *   ���̏ꍇ�Adelta_to_next �� new_ptn_leng �ł� /__/../__/../ �̂悤�Ȍ��ʂɂȂ��Ă��܂�.
 *   �܂�����Ԗڂ̃p�^�[������������Ă��܂�. ���̂悤�ȃP�[�X�ł� delta_to_next �� new_ptn_leng-1 ��
 *   �w�肷��΂��܂�����.
 */
size_t
ZnkStrEx_replace_BF( ZnkStr str, size_t begin,
		const char* old_ptn, size_t old_ptn_leng,
		const char* new_ptn, size_t new_ptn_leng,
		size_t seek_depth, size_t delta_to_next );

/**
 * @brief
 *   str ���ɂ��� old_ptn �� seek_depth �Ŏw�肵���񐔕� new_ptn �ɒu��������.
 *   ZnkStrEx_replace_BF �� BMS�A���S���Y���g�p�o�[�W�����ł���A�������ʂ͑S�������ł���.
 *
 * @param str:
 *   replace�ΏۂƂȂ镶����.
 *
 * @param begin:
 *   str�ɂ����Ă��̈ʒu����Ō�܂ł�replace�����͈̔͂Ƃ���.
 *   (�܂肱����O�͈̔͂͑ΏۊO)
 *
 * @param old_ptn:
 *   str���ɂ��̃p�^�[�����o�������Ȃ�A���̕�����new_ptn�Œu��������.
 *
 * @param old_ptn_leng:
 *   old_ptn �̕�����.
 *
 * @param old_ptn_occ_table:
 *   old_ptn ��occ_table(BMS�A���S���Y���Ŏg�p����鍂�����̂��߂̕⏕�f�[�^).
 *
 * @param new_ptn:
 *   str����old_ptn�p�^�[�����o�������ꍇ�ɒu��������ׂ��V����������p�^�[��.
 *
 * @param new_ptn_leng:
 *   new_ptn �̕�����.
 *
 * @param seek_depth:
 *   �u������������������E�։�����s���邩���w�肷��.
 *   �Ⴆ�� 1 �̏ꍇ�͈�ԍŏ��Ɍ��ꂽ old_ptn �����u������Ȃ�.
 *   Znk_NPOS ���w�肵���ꍇ�͕����񒆂ɏo�����邷�ׂĂ� old_ptn ���u�������.
 *
 * @param delta_to_next:
 *   �u�������������s��������A�J�[�\�����ǂꂾ���E�ֈړ������邩������.
 *   Znk_NPOS ���w�肵���ꍇ��new_ptn_leng �������ړ�����.
 *
 * @note
 *   delta_to_next �̑��݈Ӌ`�ɂ��Ă� ZnkStrEx_replace_BF �ɂ�������ƑS�������ł���.
 */
size_t
ZnkStrEx_replace_BMS( ZnkStr str, size_t begin,
		const char* old_ptn, size_t old_ptn_leng, const size_t* old_ptn_occ_table,
		const char* new_ptn, size_t new_ptn_leng,
		size_t seek_depth, size_t delta_to_next );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
