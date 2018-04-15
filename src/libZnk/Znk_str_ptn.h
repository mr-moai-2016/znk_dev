#ifndef INCLUDE_GUARD__Znk_str_ptn_h__
#define INCLUDE_GUARD__Znk_str_ptn_h__

#include <Znk_str.h>

Znk_EXTERN_C_BEGIN

typedef int (*ZnkStrPtnProcessFuncT)( ZnkStr str, void* arg );

const uint8_t*
ZnkStrPtn_getBetween( ZnkStr ans, const uint8_t* data, const size_t data_size,
		const char* ptn_begin, const char* ptn_end );

bool
ZnkStrPtn_replaceBetween( ZnkStr ans,
		const uint8_t* data, size_t data_size,
		const char* ptn_begin, const char* ptn_end,
		const char* replace_str, bool replaced_with_ptn );

/**
 * @brief
 * quo_beg �� quo_end �ň͂܂ꂽ�͈͂����o�����ꍇ�A
 * �����InQuote�̈�Ƃ݂Ȃ��A���̗̈�����H���邽�߂� event_handler( inquoted_str, event_arg ) ���Ăяo��.
 *
 * coe_beg �� coe_end �̓R�����g�A�E�g�L���Ƃ݂Ȃ��A
 * ���̃R�����g�A�E�g���ɂ���quo_beg, quo_end ��InQuote�̈�̋��E�Ƃ݂͂Ȃ�����������.
 * coe_beg, coe_end�ɂ�NULL���w��ł��A���̏ꍇ�̓R�����g�A�E�g�̈�͑��݂��Ȃ����̂Ƃ���.
 * ���� coe_end �݂̂� NULL�̏ꍇ�Acoe_beg �̓G�X�P�[�v�V�[�P���X�Ƃ݂Ȃ��Acoe_beg�̒����
 * quo_end�����ꂽ�ꍇ�A����𖳎�������̂Ƃ���.
 *
 * @param include_quote:
 *   true�̂Ƃ���event_handler�̏����ΏۂƂ��Ĉ����n��str��quo_beg, quo_end���̂��̂܂Ŋ܂܂���.
 *   false�̂Ƃ��͂���str��quo_beg, quo_end���܂܂��Ȃ�(quo_beg, quo_end�Ɉ͂܂ꂽ���g�����������n��).
 */
void
ZnkStrPtn_invokeInQuote( ZnkStr str,
		const char* quo_beg, const char* quo_end,
		const char* coe_beg, const char* coe_end,
		const ZnkStrPtnProcessFuncT event_handler, void* event_arg,
		bool include_quote );

/**
 * @brief
 * quo_beg �� quo_end �ň͂܂ꂽ�͈͂����o�����ꍇ�A
 * �����InQuote�̈�Ƃ݂Ȃ��A���̗̈�ɂ���old_ptn���܂܂�Ă����ꍇ��new_ptn�Œu��������.
 *
 * coe_beg �� coe_end �� ZnkStrPtn_invokeInQuote �Ɠ����Ӗ��ł���.
 *
 * @param include_quote:
 *   true�̂Ƃ���old_ptn��T�����鏈���ΏۂƂ��Ĉ����n��str��quo_beg, quo_end���̂��̂܂Ŋ܂܂���.
 *   false�̂Ƃ��͂���str��quo_beg, quo_end���܂܂��Ȃ�(quo_beg, quo_end�Ɉ͂܂ꂽ���g�����������n��).
 *
 * @param delta_to_next:
 *   ZnkStrEx_replace_BF �ɂ�����delta_to_next�ƑS�������Ӗ��ł���.
 *   ���������Ώ۔͈͓��ɂ����Ēu���������I���������ɃJ�[�\�����ǂꂾ�������������w�肷��.
 */
void
ZnkStrPtn_replaceInQuote( ZnkStr str,
		const char* quo_beg, const char* quo_end,
		const char* coe_beg, const char* coe_end,
		const char* old_ptn, const char* new_ptn, bool include_quote,
		size_t delta_to_next );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
