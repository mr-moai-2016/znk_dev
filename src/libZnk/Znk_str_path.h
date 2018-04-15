#ifndef INCLUDE_GUARD__Znk_str_path_h__
#define INCLUDE_GUARD__Znk_str_path_h__

#include <Znk_str.h>
#include <Znk_dir.h>

Znk_EXTERN_C_BEGIN

void
ZnkStrPath_replaceDSP( ZnkStr path, char dsp );

/**
 * @brief:
 *  �^����ꂽpath�ɂ�����Tail�̊J�n�ʒu��Ԃ�.
 *
 * @note:
 *  �ȉ��̂����ꂩ�𖞂����Ƃ��A�����path��Tail�ƌĂ�.
 *  1. path�̍Ō�Ɍ����dsp����̕�����.
 *  2. path����dsp���o�����Ȃ��ꍇ�A���ꂪUNC�p�X�ł���΂��̊J�n������(\\)����̕�����.
 *  3. path����dsp���o�����Ȃ��ꍇ�A���ꂪUNC�p�X�łȂ����path�S��.
 *
 * @exam:
 *  path = /abc/def �̂Ƃ� d �̈ʒu�A���Ȃ킿 5 ��Ԃ�.
 *  path = abc �̂Ƃ� a �̈ʒu�A���Ȃ킿 0 ��Ԃ�.
 */
size_t
ZnkStrPath_getTailPos( const char* path );

/**
 * @brief:
 * PATH�^���ϐ�path_envvar�ɐV�����p�Xnew_path��ǉ�����.
 * �����Ō���PATH�^���ϐ��Ƃ́AWindows�ɂ�����PATH���ϐ��̂悤�� ; �ŋ�؂�ꂽ�v�f��A��������ł���.
 * ���邢��UNIX�ɂ�����PATH�̂悤�� : �ŋ�؂��邱�Ƃ����邪�A���̋�؂蕶����psp�Ƃ��Ĉ����ɗ^����.
 * ���Apath_envvar���Ɋ���new_path�����݂���ꍇ�͏d�����Ēǉ�����Ȃ�.
 */
void
ZnkStrPath_addPathEnvVar( ZnkStr path_envvar, char psp, const char* new_path );

/**
 * @brief:
 * path�̐擪�� Cygwin�^�̃h���C�u�v���t�B�b�N�X(/cygdrive/x/) �����݂���ꍇ��
 * ���̕�����DOS�^�̃h���C�u�v���t�B�b�N�X(x:)�ɕϊ���true��Ԃ�.
 * ���݂��Ȃ��ꍇ�͉�������false��Ԃ�.
 */
bool
ZnkStrPath_convertCygDrive_toDosDrive( ZnkStr path );

/**
 * @brief:
 * ���̊֐��� �܂�ans_dir �ɐݒ肳�ꂽ�p�X����n�߁A���̔z����landmark�Ŏw�肳�ꂽ�t�@�C���܂��̓f�B���N�g����
 * �t�@�C���V�X�e����ɑ��݂��邩�𒲂ׂ�.
 * ���݂����Ȃ�΂����ŒT����ł��؂���true��Ԃ�.
 * ���Ȃ��Ȃ�΁Aans_dir�̍Ō���� ../ ��ǉ����Ă�����x������s��.
 * �����depth�Ŏw�肵���񐔎����Ă����݂��Ȃ������ꍇ�͏�����ł��؂���false��Ԃ�.
 *
 */
bool
ZnkStrPath_searchParentDir( ZnkStr ans_dir, size_t depth, const char* landmark, ZnkDirType landmark_type, char dsp );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
