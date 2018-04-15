#ifndef INCLUDE_GUARD__Znk_mutex_h__
#define INCLUDE_GUARD__Znk_mutex_h__

#include <Znk_base.h>

Znk_EXTERN_C_BEGIN

Znk_DECLARE_HANDLE( ZnkMutex )

/**
 * @brief
 * �O���[�o��Mutex
 * ����Mutex�͂��ׂẴ^�C�~���O�A���ׂĂ�Thread�ɂ����Ďg�p���邱�Ƃ��ł���.
 * �A�v���P�[�V������ɂ��B�ꑶ�݂�����̂ł���.
 * (����䂦destroy�n�̊֐��͒񋟂���Ȃ�)
 * �������A���̕��Փ˂���������\���͑������߁A�g�p�͕K�v�ŏ����ɗ��߂�ׂ�.
 */
void ZnkGlobalMutex_init( void );
void ZnkGlobalMutex_delete( void );
void ZnkGlobalMutex_lock( void );
void ZnkGlobalMutex_unlock( void );

/**
 * @brief
 * ZnkMutex�͕����̃C���X�^���X�������Ƃ��ł���.
 * �p�r�ɂ����Mutex��ʃC���X�^���X�ɂ��邱�ƂŁA���̕��Փ˂̔�������p�x��
 * �����邱�Ƃ��ł���.
 */
ZnkMutex ZnkMutex_create( void );
void     ZnkMutex_destroy( ZnkMutex mtx );
void     ZnkMutex_lock( ZnkMutex mtx );
void     ZnkMutex_unlock( ZnkMutex mtx );

/**
 * @brief
 * Singleton Mutex ��Ԃ��֐����`����.
 */
#define Znk_THE_MUTEX( theMutexPtr, theMutex ) \
	static ZnkMutex* theMutexPtr( void ) { \
		static ZnkMutex st_mutex = NULL; \
		if( st_mutex == NULL ){ \
			ZnkGlobalMutex_lock(); \
			if( st_mutex == NULL ){ st_mutex = ZnkMutex_create(); } \
			ZnkGlobalMutex_unlock(); \
		} \
		return &st_mutex; \
	} \
	Znk_INLINE ZnkMutex theMutex( void ){ return *(theMutexPtr()); }


Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
