#include "Znk_thread.h"
#include "Znk_bfr.h"
#include "Znk_stdc.h"
#include "Znk_mutex.h"

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

#else
/* Unix �Ƃ����� pthread ���g�������� */
#  include <pthread.h>
#  include <unistd.h> /* for usleep */
#  define M_PTHREAD_MUTEX_INIT( mutex ) pthread_mutex_init( mutex, 0 );

#endif

#include <assert.h>

void
ZnkThread_sleep( size_t msec )
{
#if defined(Znk_TARGET_WINDOWS)
	Sleep( (DWORD)msec );
#else
	usleep( msec * 1000 ); /* �w��̓}�C�N���b */
#endif
}


typedef struct ZnkThreadInfoImpl* ZnkThreadInfo;
struct ZnkThreadInfoImpl {
	ZnkThreadFunc   func_;
	void*           arg_;
	ZnkThreadHandle handle_;
	uintptr_t       id_;
	size_t          thno_;
	bool            active_;
};

static ZnkBfr st_pair_list = NULL;

struct znThreadListElem {
	ZnkThreadInfo info_;
	bool          used_;
};
static size_t
znThreadList_getListNum( ZnkBfr thlist )
{
	return ZnkBfr_size( thlist ) / sizeof( struct znThreadListElem );  
}
static ZnkBfr
znThreadList_theList( void )
{
	if( st_pair_list == NULL ){
		st_pair_list = ZnkBfr_create_null();
	}
	return st_pair_list;
}
static void znThreadList_deleteList()
{
	if( st_pair_list ){
		ZnkBfr_destroy( st_pair_list );
		st_pair_list = NULL;
	}
}

static ZnkMutex*
znThreadList_theMutex( void )
{
	static ZnkMutex st_mutex = NULL;
	return &st_mutex;
}
Znk_INLINE bool
znThreadList_getElem( size_t thno, struct znThreadListElem* ans )
{
	bool result = false;
	ZnkMutex mtx = *(znThreadList_theMutex());
	ZnkMutex_lock( mtx );
	{
		ZnkBfr thlist = znThreadList_theList();
		const size_t num = znThreadList_getListNum( thlist );
		struct znThreadListElem* data = (struct znThreadListElem*)ZnkBfr_data( thlist );
		if( thno < num ){
			*ans = data[ thno ];
			result = true;
		}
	}
	ZnkMutex_unlock( mtx );
	return result;
}
Znk_INLINE ZnkThreadInfo
znThreadList_getInfo( size_t thno )
{
	/* �܂�elem��deep�R�s�[�Ŏ��o�� */
	struct znThreadListElem elem = { NULL, false };
	znThreadList_getElem( thno, &elem );
	return elem.info_;
}
Znk_INLINE bool
znThreadList_used( size_t thno )
{
	/* �܂�elem��deep�R�s�[�Ŏ��o�� */
	struct znThreadListElem elem = { NULL, false };
	znThreadList_getElem( thno, &elem );
	return elem.used_;
}
Znk_INLINE bool
znThreadList_setUsed( size_t thno, bool used )
{
	bool result = false;
	ZnkMutex mtx = *(znThreadList_theMutex());
	ZnkMutex_lock( mtx );
	{
		ZnkBfr thlist = znThreadList_theList();
		const size_t num = znThreadList_getListNum( thlist );
		struct znThreadListElem* data = (struct znThreadListElem*)ZnkBfr_data( thlist );
		if( thno < num ){
			data[ thno ].used_ = used;
			result = true;
		}
	}
	ZnkMutex_unlock( mtx );
	return result;
}

Znk_INLINE void
allocInternal( ZnkBfr thlist, size_t pos, size_t num )
{
	const size_t size = num * sizeof(struct znThreadListElem);
	struct znThreadListElem* data = (struct znThreadListElem*)ZnkBfr_data( thlist );
	struct znThreadListElem* elemp;
	size_t idx;
	const size_t end = pos + num;
	ZnkBfr_fill( thlist, pos * sizeof(struct znThreadListElem), 0, size );
	for( idx=pos; idx<end; ++idx ){
		elemp = data + idx;
		elemp->info_ = (ZnkThreadInfo)Znk_malloc( sizeof( struct ZnkThreadInfoImpl ) );
		elemp->info_->thno_ = idx;
		elemp->info_->handle_ = NULL;
		elemp->info_->func_   = NULL;
		elemp->info_->arg_    = NULL;
		elemp->info_->id_     = 0;
		elemp->info_->active_ = false;
	}
}
/***
 * ���̊֐��́Alock�͊|�����Ă����ԂŌĂ΂�邱�Ƃ�O��Ƃ���.
 */
static ZnkThreadInfo
issueInfoInteranal( size_t* thno )
{
	static size_t begin = 0;
	ZnkBfr thlist = znThreadList_theList();
	const size_t num = znThreadList_getListNum( thlist );
	struct znThreadListElem* elemp = NULL;
	size_t idx;
	struct znThreadListElem* data = (struct znThreadListElem*)ZnkBfr_data( thlist );

	/***
	 * begin�̈ʒu�̓T�C�N���b�N�Ɉړ�������.
	 */
	if( begin >= num ){ begin = num; }
	for( idx=begin; idx<num; ++idx ){
		elemp = data + idx;
		if( !elemp->used_ ){
			/* ���ݖ��g�p */
			begin = idx; /* ���񌟍������������邽�߂̃q���[���X�e�B�b�N */
			*thno = idx;
			elemp->used_ = true; /* �g�p���ɐ؂�ւ��� */
			return elemp->info_;
		}
	}
	for( idx=0; idx<begin; ++idx ){
		elemp = data + idx;
		if( !elemp->used_ ){
			/* ���ݖ��g�p */
			begin = idx; /* ���񌟍������������邽�߂̃q���[���X�e�B�b�N */
			*thno = idx;
			elemp->used_ = true; /* �g�p���ɐ؂�ւ��� */
			return elemp->info_;
		}
	}

	/***
	 * �p�ӂ��Ă���Thread�̒�������I�[�o�[����.
	 * ���̏�Ԃ͂��Ȃ�C���M�����[�ł���.
	 * �{���Ȃ�΂��̂悤�ȏ󋵂��������Ȃ��悤��init���ɗ^����num���\���ɗp�ӂ��ׂ��ł���.
	 * �����ł́Anum ������܂ł� 2 �{�ɑ��₷.
	 */
	ZnkBfr_resize( thlist, 2 * num * sizeof(struct znThreadListElem) );
	allocInternal( thlist, num, num );
	data = (struct znThreadListElem*)ZnkBfr_data( thlist );
	*thno = num;
	elemp = data + num;
	elemp->used_ = true; /* �g�p���ɐ؂�ւ��� */
	return elemp->info_;
}
/***
 * ���݋󂫂̂��郊�X�g�ʒu���V�[�N���Ă��̗̈�ւ̃|�C���^��Ԃ�.
 * ���̊֐��͓�����lock���|���A�������T�����s��.
 * �ꌩ���Ԃ������肻���ł��邪�A���̂Ƃ���T���̊J�n�l���T�C�N���b�N�ɕϓ�
 * �����Ă��邽�߁AList�̗v�f�����\���ł���΁A�������̒T���͈�u�ōςނƊ��҂����.
 */
Znk_INLINE ZnkThreadInfo
znThreadList_issueInfo( size_t* thno )
{
	ZnkThreadInfo ans;
	ZnkMutex mtx = *(znThreadList_theMutex());
	ZnkMutex_lock( mtx );
	ans = issueInfoInteranal( thno );
	ZnkMutex_unlock( mtx );
	return ans;
}

/***
 * ���̎��_�Ŏq�X���b�h�͑��݂��Ȃ����̂Ɖ��肷��.
 * �ʏ�̃A�v���ł���� num��128���炢����Ώ\���ł��낤.
 * ���̒l�̎w��ɏ���͂Ȃ�(�P��List�̃������T�C�Y�ɉe������݂̂�
 * �傫�Ȓl���w�肵�Ă��������m��Ă���).
 * ���ۂɎg�p���鐔������⑽�߂Ɏw�肵�������悢.
 * ���肬�肭�炢�ɐݒ肷���issue�����̍ہA�T���Ɏ��Ԃ�
 * ������\���������Ȃ��Ă��܂�.
 */
void
ZnkThreadList_initiate( size_t num )
{
	ZnkBfr thlist;
	ZnkMutex* mtxp = znThreadList_theMutex();
	if( *mtxp == NULL ){
		*mtxp = ZnkMutex_create();
	}
	thlist = znThreadList_theList();
	ZnkBfr_resize( thlist, num * sizeof( struct znThreadListElem ) );
	allocInternal( thlist, 0, num );
}
void
ZnkThreadList_finalize( void )
{
	/***
	 * ���̎��_�Ŏq�X���b�h�͑��݂��Ȃ����̂Ɖ��肷��.
	 */
	ZnkMutex* mtxp;
	size_t idx;
	ZnkBfr thlist = znThreadList_theList();
	const size_t num = znThreadList_getListNum( thlist );
	for( idx=0; idx<num; ++idx ){
		ZnkThreadInfo info = znThreadList_getInfo( idx );
		if( info == NULL ){ continue; }
		Znk_free( info );
	}

	znThreadList_deleteList();

	mtxp = znThreadList_theMutex();
	Znk_DELETE_PTR( *mtxp, ZnkMutex_destroy, NULL );
}

Znk_INLINE size_t
findNo_bySysId( uintptr_t sys_id )
{
	ZnkMutex mtx = *(znThreadList_theMutex());
	ZnkMutex_lock( mtx );
	{
		ZnkBfr thlist = znThreadList_theList();
		const size_t num = znThreadList_getListNum( thlist );
		size_t i;
		struct znThreadListElem* data = (struct znThreadListElem*)ZnkBfr_data( thlist );
		struct znThreadListElem* elemp = NULL;
		for( i=0; i<num; ++i ){
			elemp = data + i;
			if( elemp->info_->id_ == sys_id ){
				ZnkMutex_unlock( mtx );
				return i; /* found */
			}
		}
	}
	ZnkMutex_unlock( mtx );
	return Znk_NPOS;
}
Znk_INLINE size_t
findNo_bySysHandle( ZnkThreadHandle handle )
{
	ZnkMutex mtx = *(znThreadList_theMutex());
	ZnkMutex_lock( mtx );
	{
		ZnkBfr thlist = znThreadList_theList();
		const size_t num = znThreadList_getListNum( thlist );
		size_t i;
		struct znThreadListElem* data = (struct znThreadListElem*)ZnkBfr_data( thlist );
		struct znThreadListElem* elemp = NULL;
		for( i=0; i<num; ++i ){
			elemp = data + i;
			if( elemp->info_->handle_ == handle ){
				ZnkMutex_unlock( mtx );
				return i; /* found */
			}
		}
	}
	ZnkMutex_unlock( mtx );
	return Znk_NPOS;
}
Znk_INLINE ZnkThreadHandle
findHandle_bySysId( uintptr_t sys_id )
{
	ZnkMutex mtx = *(znThreadList_theMutex());
	ZnkMutex_lock( mtx );
	{
		ZnkBfr thlist = znThreadList_theList();
		const size_t num = znThreadList_getListNum( thlist );
		size_t i;
		struct znThreadListElem* data = (struct znThreadListElem*)ZnkBfr_data( thlist );
		struct znThreadListElem* elemp = NULL;
		for( i=0; i<num; ++i ){
			elemp = data + i;
			if( elemp->info_->id_ == sys_id ){
				ZnkMutex_unlock( mtx );
				return elemp->info_->handle_; /* found */
			}
		}
	}
	ZnkMutex_unlock( mtx );
	return NULL;
}

size_t ZnkThread_getCurrentNo( void )
{
#if defined(Znk_TARGET_WINDOWS)
  	const uintptr_t sys_id = (uintptr_t)GetCurrentThreadId();
	return findNo_bySysId( sys_id );
#elif defined(Znk_TARGET_UNIX)
	const ZnkThreadHandle handle = (ZnkThreadHandle)pthread_self();
	return findNo_bySysHandle( handle );
#endif
}

/***
 * ZnkThread_create
 *
 * �{����active_��false�ɂȂ鎞�_��handle���N���[�Y�������Ƃ��낾���A
 * ���������waitEnd�n�̊֐����g���Ȃ��Ȃ��Ă��܂��\��������.
 * WinAPI��pthread���񋟂���waitEnd�n�֐��͗L����handle�������Ƃ���
 * �v�����邽�߁AThreadFunc�����E�I�ɂ�����N���[�Y���Ă��܂��ƁA
 * ������waitEnd�n�֐�������`�̓���������N�����\��������.
 * (���ہApthread_join�ł́A���ꂪ�Q�d�ɌĂ΂ꂽ�ꍇ�̓���͖���`�ł���)
 *
 * �����ŁA������N���[�Y����^�C�~���O��waitEnd���ƐV�K��Thread�����
 * create�֐����Ɍ��肵�Ă���. �܂��AThreadInfo���ɂ͈ȑO��Thread�n���h����
 * �����Ă��̂܂܎c���|���V�[�Ƃ��A�N���[�Y�����ꍇ�͂��̒l��NULL�ŏ㏑��
 * ������̂Ƃ���.
 */

/***
 * createThread �ɂ���ČĂ΂������֐�.
 * �������Thread��Entry�֐��ƍl���Ă悢.
 */
static void*
Znk__Internal__ThreadFunc( void* internal_info )
{
	ZnkThreadInfo thinfo = (ZnkThreadInfo)internal_info;
	void* result;
	/* createThread�֐��̏I����active_�t���O���m���ɗ��̂�҂�.
	 * active_ �������ƂŁAthinfo �̕K�v�ȏ������͂��ׂĊ��������Ƃ݂Ȃ�. */
	while( !thinfo->active_ ){ ZnkThread_sleep( 1 ); }
	/* ���̎��_�� active_ �� true �ł��邱�Ƃ��ۏ؂���� */
	result = (*thinfo->func_)( thinfo->arg_ );
	thinfo->active_ = false;
	znThreadList_setUsed( thinfo->thno_, false );
	return result;
}

#if defined(Znk_TARGET_WINDOWS)

static unsigned int __stdcall
Znk__Internal__ThreadFunc_stdcall( void* internal_info )
{
	/***
	 * VC�̌x��C4311��64bit�łȂ�΂킩�邪�A32bit�łł͖��Ӗ�.
	 */
#  if (_MSC_VER >= 1300) && defined(_WIN32)
#    pragma warning(push)
#    pragma warning(disable:4311)
#  endif
	void* result = Znk__Internal__ThreadFunc( internal_info );
	return Znk_force_ptr_cast( unsigned int, result );
#  if (_MSC_VER >= 1300) && defined(_WIN32)
#    pragma warning(pop)
#  endif
}
static ZnkThreadHandle
createThread( void* internal_info, uintptr_t* sys_id )
{
	/***
	 * _beginthread �ɗ^����֐��̌Ăяo���K��� __cdecl �Ȃ̂ɑ΂���
	 * _beginthreadex �ɗ^����֐��̌Ăяo���K��� __stdcall �łȂ���΂Ȃ�Ȃ����Ƃɒ��ӂ���.
	 */
	unsigned int thread_id = 0;
	ZnkThreadHandle handle = (ZnkThreadHandle)_beginthreadex( NULL, 0,
			Znk__Internal__ThreadFunc_stdcall, internal_info,
			0,
			&thread_id );
	if( sys_id ){ *sys_id = thread_id; }
	return handle;
}
Znk_INLINE int
closeHandle( ZnkThreadInfo thinfo )
{
	int result = 0;
	CloseHandle( (HANDLE)thinfo->handle_ );
	thinfo->handle_ = NULL;
	return result;
}
Znk_INLINE int waitEnd( ZnkThreadInfo thinfo )
{
	int result = (int)WaitForSingleObject( (HANDLE)thinfo->handle_, INFINITE ); 
	closeHandle( thinfo );
	return result;
}

#elif defined(Znk_TARGET_UNIX)

static ZnkThreadHandle
createThread( void* internal_info, uintptr_t* sys_id )
{
	pthread_t handle;
	pthread_create( &handle, NULL, Znk__Internal__ThreadFunc, internal_info );
	if( sys_id ){ *sys_id = 0; /* nouse */ }
	return (ZnkThreadHandle)handle;
}
Znk_INLINE int
closeHandle( ZnkThreadInfo thinfo )
{
	int result = 0;
    void* thread_result;
	result = pthread_join( (pthread_t)thinfo->handle_, &thread_result );
	thinfo->handle_ = NULL;
	return result;
}
Znk_INLINE int
waitEnd( ZnkThreadInfo thinfo )
{
	return closeHandle( thinfo );
}

#endif

size_t
ZnkThread_create( ZnkThreadFunc func, void* arg )
{
	uintptr_t thread_id = 0;
	size_t thno = Znk_NPOS;
	ZnkThreadInfo thinfo;
	ZnkMutex* mtx = znThreadList_theMutex();
	if( *mtx == NULL ){
		/* �܂�����������Ă��Ȃ� */
		ZnkThreadList_initiate( 128 );
	}

	thinfo = znThreadList_issueInfo( &thno );

	/***
	 * �ȉ��Athinfo �� used_ �� true �ɂȂ��Ă��邽�߁A
	 * ���̃X���b�h�����̓��e�����������邱�Ƃ͂Ȃ��͂�.
	 */
	assert( func );
	thinfo->func_ = func;
	thinfo->arg_  = arg;
	if( thinfo->handle_ ){
		/* �ߋ��̎c�[�����݂���. */
		closeHandle( thinfo );
	}

	thinfo->handle_ = createThread( thinfo, &thread_id );
	thinfo->id_ = thread_id;
	thinfo->active_ = true;
	/* ���̎��_�� active_ �� true �ł��邱�Ƃ��ۏ؂���� */
	return thno;
}

int
ZnkThread_waitEnd( size_t thno )
{
	const size_t current_thno = ZnkThread_getCurrentNo();
	if( current_thno != thno ){
		/***
		 * ���݂̃X���b�h�Ɠ����ԍ����w�肳�ꂽ�ꍇ�͏��O����.
		 * (�I������͂����Ȃ��̂�)
		 */
		int result = 0;
		ZnkThreadInfo thinfo = znThreadList_getInfo( thno );
		if( thinfo && thinfo->handle_ ){
			result = waitEnd( thinfo );
		}
		return result;
	}
	return 0;
}

/***
 * ���ׂĂ̎q�X���b�h���I���̂�҂�.
 * ���̊֐��͒ʏ�A�v���I�����Ƀ��C���X���b�h�ŌĂяo�����Ƃ�z�肵�����̂ł��邪�A
 * �q�X���b�h�ŌĂяo�����Ƃ��ł��Ȃ��͂Ȃ�. ���̏ꍇ�A���̎q�X���b�h�ƃ��C���X���b�h
 * �����������ׂĂ̎q�X���b�h�̏I���҂����s�����ƂɂȂ�.
 */
void
ZnkThread_waitEndAll( void )
{
	ZnkBfr thlist = znThreadList_theList();
	const size_t num = znThreadList_getListNum( thlist );
	size_t idx;
	for( idx=0; idx<num; ++idx ){
		ZnkThread_waitEnd( idx );
	}
}

ZnkThreadHandle
ZnkThread_getHandle( size_t thno )
{
	ZnkThreadInfo thinfo = znThreadList_getInfo( thno );
	/* active�ł͂Ȃ��ꍇ�A���̃n���h���l�͂��͂�L���ł͂Ȃ��ƍl����. */
	if( thinfo == NULL || !thinfo->active_ ){ return NULL; }
	return thinfo->handle_;
}

bool
ZnkThread_terminateForce( size_t thno )
{
#if defined(Znk_TARGET_WINDOWS)
	HANDLE handle = (HANDLE)ZnkThread_getHandle( thno );
	if( handle ){
		TerminateThread( handle, 0 );
		return true;
	}
#endif
	return false;
}

