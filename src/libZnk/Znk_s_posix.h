#ifndef INCLUDE_GUARD__Znk_s_posix_h__
#define INCLUDE_GUARD__Znk_s_posix_h__

#include <Znk_base.h>

Znk_EXTERN_C_BEGIN

/**
 * @brief
 *   �啶���Ə������𖳎�����strcmp�ł���A���m�ɂ͎w�肵��������s1,s2�����ׂ�
 *   �������ɒ������Ƃ���strcmp�������ʂ�Ԃ�(�������ւ̕ϊ������̓e�[�u���Q�Ƃ�
 *   �s���邽�ߒʏ��strcmp�ɔ�ׁA���x�͂���قǒቺ���Ȃ�).
 *   �������A�w�肷�镶�����ASCII������������Ȃ���̂ł��邩�A�܂��̓}���`�o�C�g
 *   �������܂ޏꍇ�ł����̕����R�[�h��EUC�܂���UTF-8�Ɍ�����̂Ƃ���.
 *   SJIS�̑S�p�����ȂǁA2byte�ڈȍ~��ASCII�̗̈�Ɣ��悤�ȃ}���`�o�C�g������
 *   �܂܂��ꍇ�A���̌��ʂ͈Ӗ��̂�����̂ɂȂ�Ȃ��\��������.
 *
 * @note
 *   glibc��BSD�ŕW���񋟂���Ă���strcasecmp/strncasecmp�Ɠ��@�\�̊֐��ł���.
 *   windows�ɂ�����_stricmp/_strnicmp�����l�̋@�\��񋟂���֐��ł��邪�A
 *   windows�ɂ����邱���̊֐��ł�LC_CTYPE�̒l�ɂ���āAASCII������������Ȃ�
 *   �ꍇ�ƁASJIS�̂悤�ȃ}���`�o�C�g�������܂܂�Ă��Ă����Ȃ��悤�ȏꍇ��
 *   ��ʂ�ɕ����ď������Ă���. ��҂͂����炭�ᑬ�Ǝv����.
 */
int
ZnkS_strcasecmp( const char* s1, const char* s2 );
int
ZnkS_strncasecmp( const char* s1, const char* s2, size_t n );


/**
 * @brief
 *   NULL�I�[����ʒu��Ԃ�.
 *   �������Abuf����'\0'���݂���Ȃ��ꍇ��buf_size��Ԃ�.
 *
 * @note
 *   POSIX�ŕW���񋟂���Ă���strnlen�Ɠ��@�\�̊֐��ł���.
 *   strnlen �́A���m�̃T�C�Y�̃o�b�t�@(�l�b�g���[�N�p�P�b�g�Ȃ�)���̎�M����
 *   �M���ł��Ȃ��f�[�^���̕����񒷂��擾����悤�ȏꍇ�Ɏg�p�ł���.
 */
size_t 
ZnkS_strnlen( const char* buf, size_t buf_size );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
