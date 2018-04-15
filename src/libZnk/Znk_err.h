#ifndef INCLUDE_GUARD__Znk_err_h__
#define INCLUDE_GUARD__Znk_err_h__

#include <Znk_base.h>
#include <Znk_str.h>
#include <Znk_s_atom.h>

Znk_EXTERN_C_BEGIN

/***
 * ZnkErr��Error���b�Z�[�W���i�[���ێ�����.
 * Error ���b�Z�[�W���i�[���邽�߂�ZnkStr��^���Ă����l�̂��Ƃ͂ł��邪
 * ������ɂ͎�Ƀp�t�H�[�}���X�̓_�ɂ����ĈႢ������.
 * �ȉ��A�����̑���_������.
 *
 * ZnkErr:
 *   ZnkErr��Error�����ۂɔ��������ꍇ�ɂ̂�Error���b�Z�[�W�p�̗̈��V�K�ɐ������Ċi�[���ێ�����.
 *   �������Ȃ���΂��̏����͍s���Ȃ�.
 *
 *   ���[�U����Error���b�Z�[�W��K�v�Ƃ��邩�ۂ��Ɋւ�炸�A���[�U�͒P��ZnkErr_D( err ); �ȂǂƐ錾����΂悢.
 *   ���̐錾�͒P�Ȃ�auto�ϐ��̐錾�ł���A�������̓��I�m�ۂȂǂ͈�؍s���Ȃ����߂��̐錾�ɂ��f�����b�g�͂Ȃ��Ƃ݂Ȃ���.
 *   ����������Ƃ��Ĕz������֐������A���ꂪ�K���w�肳�����̂Ƃ݂Ȃ��Ă悭���ʂȔz�����s���K�v�͂Ȃ�.
 *
 *   �G���[���b�Z�[�W���P�񂾂��\�z����Ύ������悤�ȗp�r�Ɍ����Ă���.
 *   ���|�[�g���b�Z�[�W�ȂǏ�ɑ�ʂ̃��b�Z�[�W�̒ǉ�����������悤�ȗp�r�ɂ͕s�����ł���.
 *   �Ȃ��Ȃ�Ίe���b�Z�[�W�̒ǉ��̂��тɗ̈�̐����������������Ă��܂����߂ł���.
 *   ���̐��������͓��R�Ȃ���P�Ȃ�ZnkStr_add�Ɣ�ׂĎ��Ԃ�������.
 *
 * ZnkStr:
 *   ���[�U����Error���b�Z�[�W��K�v�Ƃ���ꍇ�́A
 *   ���ۂɃG���[���������邩�ۂ��Ɋւ�炸�AZnkStr�̐����Ɖ�����������[�U�͍s��Ȃ���΂Ȃ�Ȃ�.
 *   ���[�U����Error���b�Z�[�W��K�v�Ƃ��Ȃ��ꍇ�́A����������Ƃ��Ĕz������֐�����NULL���w�肳�ꂽ�ꍇ��
 *   ����𖳎�����Ƃ����d�l�ɂ���ƃ��[�U���̕��S�����炷���Ƃ��ł���.
 *   ��������������Ɠ��R�A�֐�����ZnkStr_add���ĂԂ��т�NULL�`�F�b�N���s��Ȃ���΂Ȃ�Ȃ��Ƃ������S�͔�������.
 *
 *   ��U�m�ۂ��Ă��܂��΁A��͒P�Ȃ�ZnkStr_add�̌Ăяo���̘A���ł悭�A����͌y�ʂł���.
 *   ����ă��|�[�g���b�Z�[�W�ȂǏ�ɑ�ʂ̃��b�Z�[�W���擾����悤�ȗp�r�ɍł������őΉ��ł���.
 *   �ܘ_�G���[���b�Z�[�W���i�[����̂ƌ��p���Ă��悢.
 */

/**
 * @brief
 *   ZnkErr�^�̒�`.
 *   ���[�U�� ���������o pr__ �ɃA�N�Z�X���Ă͂Ȃ�Ȃ�.
 */
typedef struct { ZnkSAtomPr pr__; } ZnkErr;
#define ZnkErr_D( name ) ZnkErr name = {{ 0,0 }}

/***
 * �\���̂�ZnkErr�����o�����������ꍇ�� 0 �N���A�������ꍇ�ɗp���邱�Ƃ�z�肵������.
 * ���̏������͌���K�{�ł͂Ȃ�.
 */
Znk_INLINE void ZnkErr_set_null( ZnkErr* err ){
	ZnkErr_D( tmp ); *err = tmp;
}

/**
 * @brief
 *   ZnkErr �̓���AtomDB�֐V�����G���[���b�Z�[�W��o�^���A
 *   ����ID�ƂȂ�ZnkErr(���̂�ZnkSAtomPr�^�ł���)��Ԃ�.
 *   ���̊֐����Ăяo���̂Ƀ��[�U�͂Ȃ�珀���͂���Ȃ�.
 *   (ZnkErr_initiate�֐����Ăяo���K�v���͓��ɂȂ�)
 *   internf �֐��ɂ����ẮAprintf�`���̃t�H�[�}�b�g�w����s�����Ƃ��ł��A
 *   ���̌��ʐ�������镶������L�[�Ƃ��ē���AtomDB�֓o�^�����Ƃ������̂ł���.
 *
 * @param ans_err:
 *   intern���ꂽ���ʕԂ����ZnkErr�l���i�[�����������|�C���^���w�肷��.
 *   err�����[�U���擾����K�v���Ȃ��ꍇ������NULL���w�肷�邱�Ƃ��ł���.
 *   �������A���̏ꍇ�ł�������intern����(�܂蕶�����err�̓o�^)���͍̂s����.
 *
 * @exam
 *   �o�^����������́A�ԋp���ꂽZnkErr��ZnkErr_cstr�֐��Ɉ����n�����ƂŎg�p���邱�Ƃ��ł���.
 *   �ȉ��ɁAZnkErr�̓T�^�I�Ȏg�p�������.
 *
 *     bool some_func( ZnkErr* ans_err )
 *     {
 *       // some process...
 *       if( ... ){
 *         // Error A.
 *         ZnkErr_intern( ans_err, "Error A occur." );
 *         return false;
 *       }
 *       if( ... ){
 *         // Error B. (use printf-like interface)
 *         ZnkErr_internf( ans_err, "Error B occur on idx=[%d].", idx );
 *         return false;
 *       }
 *       return true;
 *     }
 *
 *     void test()
 *     {
 *       ZnkErr_D( err ); // ����0�������͕K�{�ł͂Ȃ����A�s�������ꍇ�͂��̂悤�ɂ���.
 *       bool result = some_func( &err );
 *       if( !result ){
 *         // Error Message�̓��e��\��
 *         fprintf( stderr, "ErrorMsg is [%s]\n", ZnkErr_cstr(err) );
 *       }
 *     }
 *
 * @note
 *   ���̊֐��̓X���b�h�Z�[�t�ł���.
 */
void ZnkErr_intern(  ZnkErr* ans_err, const char* err_msg );
void ZnkErr_internf( ZnkErr* ans_err, const char* fmt, ... );

/**
 * @brief
 *   ZnkErr �������������Ԃ�.
 *   ���ꂪ����������Ƃ́AZnkErr_intern(ZnkErr_internf)�Ăяo�����ɓo�^����������ł���.
 *
 * @return
 *   ZnkErr ������������̃|�C���^�ł���AC������ł���.
 *   ���ꂪ����������|�C���^�́AZnkErr_finalize���Ă΂�Ȃ����薳���ɂȂ邱�Ƃ͂Ȃ�.
 *
 * @note
 *   ���̊֐��̓X���b�h�Z�[�t�ł���.
 */
const char* ZnkErr_cstr( const ZnkErr err );

void
ZnkErr_initiate( void );
void
ZnkErr_finalize( void );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
