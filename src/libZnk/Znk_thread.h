#ifndef INCLUDE_GUARD__Znk_thread_h__
#define INCLUDE_GUARD__Znk_thread_h__

#include <Znk_base.h>

Znk_EXTERN_C_BEGIN

typedef void*  (*ZnkThreadFunc)( void* );

Znk_DECLARE_HANDLE( ZnkThreadHandle )

void
ZnkThread_sleep( size_t msec );

void ZnkThreadList_initiate( size_t list_num );
void ZnkThreadList_finalize( void );

/**
 * @brief
 * �V�����X���b�h���쐬����.
 * �߂�l�̓X���b�hNo�ł���.
 *
 * ���̊֐��������Ԃ������_�� active �t���O�͌����Ɏq�X���b�h��
 * �����̎����𔽉f����. �܂�q�X���b�h���d�����܂��Ƃ����A�I�������ꍇ�ȊO
 * ���ꂪ false �ɂȂ�\���͂Ȃ�.
 * ( ����������΁Aactive �t���O�� true �ɐݒ肳�����O�̒i�K��
 * �q�X���b�h�̏������n�܂邱�Ƃ͂��肦�Ȃ��Ƃ������� )
 */
size_t ZnkThread_create( ZnkThreadFunc func, void* arg );

/**
 * @brief
 * �w�肵���X���b�hNo�ɑ�������X���b�h�̏I����҂�.
 * ���̊֐����Ăяo�������̃X���b�h�́A���̑҂��̊Ԃ͐��䂪�u���b�N����A
 * ��~����.
 *
 * �߂�l�͏����n���񋟂���Thread�I���҂��֐����Ԃ������l�ł���A
 * ���̈Ӗ��͏����n�Ɉˑ�����.
 * (Windows�̏ꍇWaitForSingleObject�Apthread �̏ꍇ pthread_join ���Ԃ��l�ƂȂ�)
 */
int ZnkThread_waitEnd( size_t thno );
void ZnkThread_waitEndAll( void );

/**
 * @brief
 * �^����ꂽ�X���b�hNo���AZnkThreadHandle �𓾂�.
 */
ZnkThreadHandle ZnkThread_getHandle( size_t thno );

/**
 * @brief
 * �^����ꂽ�X���b�hNo��active�ł��邩�ۂ���Ԃ�.
 * Znk_NPOS��^�����ꍇ�A���C���X���b�h�ɂ��Ă�Query�ł���Ƃ݂Ȃ�.
 * ���̏ꍇ�A���R�Ȃ����� true ��Ԃ�.
 */
Znk_INLINE bool ZnkThread_isActive( size_t thno ){
	if( thno == Znk_NPOS ){ return true; }
	/* ���̎�̏����́Apthread�Ȃ�Έꌩ�����pthread_kill��p�����
	 * �����ł����������A���ۂ͏����n�ɂ���Ă͂��܂������Ȃ����Ƃ�
	 * ����炵��. (Web��̏�񂾂��A�X���b�h���Ȃ��ꍇ�N���b�V��������
	 * �e�X���b�h�����������肷��P�[�X������Ƃ̂���)
	 */
	return (bool)( ZnkThread_getHandle( thno ) != NULL );
}

/**
 * @brief
 * ���݂̃X���b�h�̃X���b�hNo �𓾂�.
 * �������A���C���X���b�h�ɂ����ČĂ΂ꂽ�ꍇ�AZnk_NPOS��Ԃ�.
 */
size_t ZnkThread_getCurrentNo( void );

/**
 * @brief
 * ���݂̃X���b�h��ZnkThreadHandle �𓾂�.
 * �������A���C���X���b�h�ɂ����ČĂ΂ꂽ�ꍇ�ANULL��Ԃ�.
 */
Znk_INLINE ZnkThreadHandle ZnkThread_getCurrentHandle( void ){
	return ZnkThread_getHandle( ZnkThread_getCurrentNo() );
}

bool ZnkThread_terminateForce( size_t thno );


Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
