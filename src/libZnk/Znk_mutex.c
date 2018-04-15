#include "Znk_mutex.h"
#include "Znk_bfr.h"
#include "Znk_stdc.h"

/***
 * �����̂ł̎����͍ő����\�������S�����d������.
 * �Ƃ͂����AThread�֌W�͌�������ǋy����Ƃ��肪�Ȃ�(������API������Ȃ����G�����Ă��܂�)�̂ŁA
 * �K���Ɏg�p�Ɋւ���|���V�[�Ő܂荇���ǂ���������đË�����ق��������I�ł���Ƃ͎v��.
 */

/***
 * Global Mutex
 */
#if defined(Znk_TARGET_WINDOWS)
/* windows API */
#  include <process.h>
#  include <windows.h>
typedef CRITICAL_SECTION SysAtomicCS;

#else
/* Unix �Ƃ����� pthread ���g�������� */
#  include <pthread.h>
#  include <unistd.h> /* for usleep */
#  define M_PTHREAD_MUTEX_INIT( mutex ) pthread_mutex_init( mutex, 0 );
typedef pthread_mutex_t SysAtomicCS;

#endif

#include <assert.h>


#if defined(Znk_TARGET_WINDOWS)
Znk_INLINE void SysAtomicCS_init( SysAtomicCS* cs ){
	InitializeCriticalSection( cs );
}
Znk_INLINE void SysAtomicCS_delete( SysAtomicCS* cs ){
	DeleteCriticalSection( cs );
}
Znk_INLINE void SysAtomicCS_lock( SysAtomicCS* cs ){
	EnterCriticalSection( cs );
}
Znk_INLINE void SysAtomicCS_unlock( SysAtomicCS* cs ){
	LeaveCriticalSection( cs );
}

#else
Znk_INLINE void SysAtomicCS_init( SysAtomicCS* cs ){
	M_PTHREAD_MUTEX_INIT( cs );
}
Znk_INLINE void SysAtomicCS_delete( SysAtomicCS* cs ){
	pthread_mutex_destroy( cs );
}
Znk_INLINE void SysAtomicCS_lock( SysAtomicCS* cs ){
	pthread_mutex_lock( cs );
}
Znk_INLINE void SysAtomicCS_unlock( SysAtomicCS* cs ){
	pthread_mutex_unlock( cs );
}

#endif


static SysAtomicCS*
theGlobalSysAtomicCS( void )
{
	/***
	 * �����̓I�u�W�F�N�g�����\�z�̏�Ԃł͌����ɂ�thread-safe�Ƃ͌����Ȃ�.
	 * mutex �\�z���ɕʂ̃X���b�h�����荞��ł���\�������邩��ł���.
	 * ������m���ɖh���ɂ́A�����A�N�Z�X�͂��蓾�Ȃ��Ƃ킩���Ă���i�K��
	 * ���X�ɍ\�z���������Ă��܂����Ƃł���.
	 */
	/* Meyers Singleton */
	static bool is_initialized = false;
	static SysAtomicCS st_cs;
	if( !is_initialized ){
		is_initialized = true;
		SysAtomicCS_init( &st_cs );
	}
	return &st_cs;
}
void ZnkGlobalMutex_init( void )
{
	theGlobalSysAtomicCS(); /* �\�z�𑣂� */
}
void ZnkGlobalMutex_delete( void )
{
	SysAtomicCS_delete( theGlobalSysAtomicCS() );
}
void ZnkGlobalMutex_lock( void )
{
	SysAtomicCS_lock( theGlobalSysAtomicCS() );
}
void ZnkGlobalMutex_unlock( void )
{
	SysAtomicCS_unlock( theGlobalSysAtomicCS() );
}

/***
 * ZnkMutex_create :
 *   ����̓X���b�h�Z�[�t�ł͂Ȃ����A��������Mutex���قȂ�X���b�h���瓯����
 *   �쐬���悤�Ƃ��邱�Ƃ͐���ȏ����Ƃ͂������A���̂悤�ȏ󋵂͏��O���Ă悢���낤.
 *
 * ZnkMutex_destroy :
 *   Mutex�������̃X���b�h���瓯���ɍ폜�����悤�ȏ����͐���Ƃ͂����Ȃ����߁A
 *   ���̉\�����l������K�v�͂Ȃ����낤. Singleton ��Mutex�ɂ����Ă����l�ł���.
 */
struct ZnkMutexImpl{
	SysAtomicCS cs_;
};

#define GET_CS( mtx ) (&((struct ZnkMutexImpl*)(mtx))->cs_)

ZnkMutex ZnkMutex_create( void )
{
	ZnkMutex mtx = (ZnkMutex)Znk_malloc( sizeof(struct ZnkMutexImpl) );
	SysAtomicCS_init( GET_CS(mtx) );
	return mtx;
}
void ZnkMutex_destroy( ZnkMutex mtx )
{
	if( mtx ){
		SysAtomicCS_delete( GET_CS(mtx) );
		Znk_free( mtx );
	}
}
void ZnkMutex_lock( ZnkMutex mtx )
{
	if( mtx ){ SysAtomicCS_lock( GET_CS(mtx) ); }
}
void ZnkMutex_unlock( ZnkMutex mtx )
{
	if( mtx ){ SysAtomicCS_unlock( GET_CS(mtx) ); }
}

