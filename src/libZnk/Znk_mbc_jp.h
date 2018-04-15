#ifndef INCLUDE_GUARD__Znk_mbc_jp_h__
#define INCLUDE_GUARD__Znk_mbc_jp_h__

#include <Znk_bif.h>
#include <Znk_err.h>

Znk_EXTERN_C_BEGIN

/**
 * @brief
 * �����R�[�h�������񋓌^.
 *
 * ZnkMbcType_BINARY�͎w�莞���Ƃ��āA������ł͂Ȃ���ʓI�ȃo�C�i���ł��邱�Ƃ�����.
 * ZnkMbcType_AUTO�͎w�莞���Ƃ��āA�����R�[�h�������I�ɔ��f���ė~�������Ƃ�����.
 * ZnkMbcType_UNKNOWN�͏I���R�[�h�Ƃ��āA�^����ꂽ������̕����R�[�h�����m�̂��̂ł��������Ƃ�����.
 * ZnkMbcType_UTF16:
 *   �v���O�������bit���Z�ŏ����������LE���ł��낤��BE���ł��낤�Ƃ��̃R�[�h�ɍ��͐����Ȃ�.
 *   �����LE/BE�̍��̓������C���[�W���d�v�ɂȂ�ꍇ�ɉ��߂ĕϊ���������΂悢���ƂƂ���
 *   �����ł͂��̋�ʂ��s��Ȃ�.
 */
typedef enum {
	 ZnkMbcType_BINARY=0
	,ZnkMbcType_ASCII
	,ZnkMbcType_EUC
	,ZnkMbcType_SJIS
	,ZnkMbcType_JIS
	,ZnkMbcType_UTF8
	,ZnkMbcType_UTF16
	,ZnkMbcType_AUTO
	,ZnkMbcType_UNKNOWN
} ZnkMbcType;

/**
 * @note:
 *  Windows�ɂ����郏�C�h�������UTF16LE�Ɠ������̂ƍl���Ă悢.
 */
bool
ZnkMbc_convertUTF8_toUTF16( const char* utf8s, size_t utf8s_leng, ZnkBif utf16s, ZnkErr* err );

/***
 * Convert UTF-16-LE (st. byte order is reverse) to UTF-8
 * ���ʂ�utf8s�̍Ō���֒ǉ������.
 */
bool
ZnkMbc_convertUTF16_toUTF8( const uint16_t* utf16s, size_t utf16s_leng, ZnkBif utf8s, bool eliminate_bom, ZnkErr* err );

void
ZnkMbc_convertUTF16_toEUC( const uint16_t* utf16s, size_t utf16s_leng, ZnkBif eucs );

bool
ZnkMbc_convertEUC_toUTF16( const char* eucs, size_t eucs_leng, ZnkBif utf16s );

/***
 * UTF8�ł���utf8s��^���A�����EUC�ɕϊ��������ʂ�eucs�̍Ō���ɒǉ�����.
 */
bool
ZnkMbc_convertUTF8_toEUC( const char* utf8s, size_t utf8s_leng, ZnkBif eucs, ZnkBif ws, ZnkErr* err );
/***
 * UTF8�ł���self��^���A�����EUC�ɕϊ��������ʂ�self�ɏ㏑������.
 */
bool
ZnkMbc_convertUTF8_toEUC_self( ZnkBif self, ZnkBif ws, ZnkErr* err );

/***
 * EUC�ł���eucs��^���A�����UTF8�ɕϊ��������ʂ�utf8s�̍Ō���ɒǉ�����.
 */
bool
ZnkMbc_convertEUC_toUTF8( const char* eucs, size_t eucs_leng, ZnkBif utf8s, ZnkBif ws, ZnkErr* err );
/***
 * EUC�ł���self��^���A�����UTF8�ɕϊ��������ʂ�self�ɏ㏑������.
 */
bool
ZnkMbc_convertEUC_toUTF8_self( ZnkBif self, ZnkBif ws, ZnkErr* err );

void
ZnkMbc_convert_self( ZnkBif self, ZnkMbcType src_mbc_type, ZnkMbcType dst_mbc_type, ZnkBif tmp, ZnkErr* err );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
