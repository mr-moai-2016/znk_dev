#ifndef INCLUDE_GUARD__Znk_htp_util_h__
#define INCLUDE_GUARD__Znk_htp_util_h__

#include <Znk_bfr.h>
#include <Znk_str.h>

Znk_EXTERN_C_BEGIN

size_t
ZnkHtpB64_decode_size( const char *bs64_str, size_t bs64_str_leng );

size_t
ZnkHtpB64_decode( ZnkBfr ans, const char* bs64_str, size_t bs64_str_leng );

size_t
ZnkHtpB64_encode_leng( size_t data_size );

size_t
ZnkHtpB64_encode( ZnkStr ans, const uint8_t* data, size_t data_size );

/**
 * @brief
 * �w�肳�ꂽdata�o�C�g��Ɋւ��� URL Encoding���s���A���̌��ʂ�ans�ɒǉ��I�Ɋi�[����.
 */
void
ZnkHtpURL_escape( ZnkStr ans, const uint8_t* data, size_t data_size );

/**
 * @brief
 * / ���������͂��̂܂܎c���ȊO��ZnkHtpURL_escape�Ɠ���.
 */
void
ZnkHtpURL_escape_withoutSlash( ZnkStr ans, const uint8_t* data, size_t data_size );

void
ZnkHtpURL_unescape( ZnkBfr ans, const char* esc_str, size_t esc_str_leng );

Znk_INLINE void
ZnkHtpURL_unescape_toStr( ZnkStr ans, const char* esc_str, size_t esc_str_leng )
{
	ZnkBfr_pop_bk_ex( ans, NULL, 1 ); /* ��U�Ō��NULL�����������폜���� */
	ZnkHtpURL_unescape( ans, esc_str, esc_str_leng );
	ZnkStr_terminate_null( ans, true );
}

/**
 * @brief
 *  req urp(���Lnote�Q��) �ɂ����āA����Ƀf�B���N�g���ɑ������镔���������ł� req_urp_dir�ƌď̂���.
 *  ���̊֐��ł͂���req_urp_dir��req_urp��蒊�o����.
 *
 *  req_urp_dir�̂������������.
 *    /my_dir/my_data.html �Ȃ�� /my_dir
 *    / �Ȃ�� �󕶎�. 
 *  �N�G���[�X�g�����O�̎n�܂������ '?' ���܂܂��ꍇ�A? ���O�͈݂̔͂̂ōl�����Ȃ���΂Ȃ�Ȃ�.
 *  �������������.
 *    /my_dir/my.cgi?arg=val �Ȃ�� /my_dir
 *    /my_dir/my.cgi?arg=val&arg_dir=special_dir/special_data.dat �Ȃ�� /my_dir ( ? �ȍ~�� / �͍l���Ȃ� )
 *
 * @param req_urp_dir:
 *   req_urp_dir�̌��ʂ��i�[����.
 *
 * @param req_urp_tail:
 *   tail�������K�v�ȏꍇ�w�肷��.
 *   �s�v�ȏꍇ��NULL���w�肷�邱�Ƃ��ł���.
 *
 * @return
 *   req_urp_dir �� ����ȍ~��tail�����𕪂��� / �̈ʒu������req_urp�ɂ�����|�C���^��Ԃ�.
 *   (req_urp�ɂ�����|�C���^�ł���. req_urp_dir�ɂ����邻��ł͂Ȃ�.)
 *   req_urp ���ɂ��̂悤�� / �����݂��Ȃ������ꍇ�� NULL ��Ԃ��A����͓�����req_urp_dir����ł��邱�Ƃ�����.
 *
 * @note
 *  �����Ŏg�p���� req urp �Ƃ����p��� URL�ɂ����� hostname �ȍ~�̕������Ӗ�������̂Ƃ���.
 *  �����GET/POST���N�G�X�g�Ȃǂ̑�P�s�ڂ̂Q�Ԗڂ̃g�[�N���Ŏg������̂�z�肵�Ă���.
 *  �Ⴆ�� http://127.0.0.1:8080/users/taro/my_data.html �Ȃ�� /my_dir/my_data.html ������(�n�܂��/���܂ނ��̂Ƃ���).
 *  �z�X�g�������ł���悤�ȏꍇ�A�Ⴆ�� http://127.0.0.1:8080 �Ȃ�� / �ꕶ���ƒ�߂�.
 *  �N�G���[�X�g�����O�̎n�܂������ '?' ���܂܂��ꍇ�A�Ⴆ�� http://127.0.0.1:8080/my_dir/my.cgi?arg=val �Ȃ��
 *  GET���N�G�X�g�̏ꍇ�� /my_dir/my.cgi?arg=val�APOST���N�G�X�g�̏ꍇ�� /my_dir/my_data.cgi �ŏ��������ƍl������.
 */
void
ZnkHtpURL_getReqUrpDir( ZnkStr req_urp_dir, ZnkStr req_urp_tail, const char* req_urp );

/**
 * @brief
 *  req_urp_dir��sanitize����(req_urp_dir�ɂ��Ă�ZnkHtpURL_getReqUrpDir�̃R�����g���Q��).
 *
 * @note:
 *  �����ł͂�����e�f�B���N�g�����Ӗ����� .. �����o���ꂽ�ꍇ�͖ⓚ���p�� __ �֕ϊ�����.
 *  �܂� :// ���܂܂��ꍇ�͂���� URL Encoding����.
 *  ����Ɉȉ��̕�����URL Encoding����.
 *    '\' ( DOS�ł�DSP�Ɣ��f����Ă��܂������̌��ʂ͑ł������Ă����ׂ��ł��� )
 *    '?'�A':'�A';' '*' �Ȃǃt�@�C���V�X�e���ł͖������邢�͊댯�ƂȂ�\���̂��镶��
 *  ������ShiftJIS�Ȃǂ̃t�@�C�����̎w���L���Ƃ��������ʂȏꍇ�Ȃǂ͋t�� \ ��Escape���Ă��܂��Ă͍���.
 *  ���̏ꍇ�͂���ɓ��ʂȏ������K�v�ƂȂ邽�߁A�����ł� is_url_encode �ł��̏������s�����ۂ��̎w�肪�ł���悤�ɂȂ��Ă���.
 */
void
ZnkHtpURL_sanitizeReqUrpDir( ZnkStr req_urp_dir, bool is_url_encode );

void
ZnkHtpURL_negateHtmlTagEffection( ZnkStr str );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
