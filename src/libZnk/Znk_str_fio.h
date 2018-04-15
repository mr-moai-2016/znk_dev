#ifndef INCLUDE_GUARD__Znk_str_fio_h__
#define INCLUDE_GUARD__Znk_str_fio_h__

#include <Znk_str.h>
#include <Znk_stdc.h>

Znk_EXTERN_C_BEGIN

/**
 * @brief
 *   line �� pos �̈ʒu�� fp �Ŏw�肵��text�t�@�C���̓��e��1�sfgets�œǂݍ���.
 *   ���̂Ƃ��Aline�̃T�C�Y�́Apos �̒l�Ɏ��ۂɓǂݍ��܂ꂽ�T�C�Y�����Z���ꂽ
 *   �l�ɕύX�����(�܂�A���Ƃ��Ƃ�line�̃T�C�Y�����̒l�����傫���ꍇ�́A
 *   line�̃T�C�Y�͌��ʓI�Ɍ������邱�Ƃ�����). line��capacity�́A�K�v�ɉ�����
 *   �����I�Ɋg�������.
 *
 *   pos �� 0 ���w�肵���ꍇ�A�T�^�I��fgets�����ɂ����Ƃ��߂������ɂȂ�.
 *   �܂�pos��Znk_NPOS��line�̌��݂̃T�C�Y�ȏ�̒l���w�肷�邱�Ƃ��ł��A
 *   ���̏ꍇ�́Aline �̌��ɓǂݍ��܂ꂽ���e���A�������.
 *
 *   block_size �́A�����Ŏg�p���� fgets ����x�ɓǂݍ��ރo�b�t�@�T�C�Y�ɂȂ�.
 *   �܂�����ł��̒l�Ŏw�肵���T�C�Y����P�ʂƂ���capacity�͑��債�Ă���.
 *   ���̒l�����܂�ɏ������ꍇ�A������fgets�����x���Ăяo�����ƂɂȂ��Ă��܂����߁A
 *   �ǂݍ��ލs�̒������傫���ꍇ������������. ����ł��܂�ɑ傫�Ȓl���w�肵���ꍇ
 *   line��capacity�ɑ��݂��閳�ʂȗ̈�̃T�C�Y���傫���Ȃ�A�������������ቺ����.
 *   �ʏ��block_size=4096���x�𐄏�����. ���A4�����̒l���w�肵���ꍇ�A������
 *   �����I��4�ɕ␳�����.
 */
bool ZnkStrFIO_fgets( ZnkStr line, size_t pos, size_t block_size, ZnkFile fp );


Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
