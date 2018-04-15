#include "Znk_err.h"
#include "Znk_mutex.h"
#include "Znk_str.h"
#include <stdarg.h>

#define M_getZnkSAtomPr( err ) &((err).pr__)

Znk_THE_MUTEX( theMutexPtr, theMutex )

static ZnkSAtomDB* theErrAtomDBPtr( void )
{
	static ZnkSAtomDB st_atomdb = NULL;
	return &st_atomdb;
}
Znk_INLINE ZnkSAtomDB theErrAtomDB( void )
{
	/***
	 * ���̊֐��͕K����ʂ̊֐���lock/unlock�̒��ŌĂ΂����̂ł���ƒ�߂�.
	 * �]���āA�t�Ɍ����΂��̊֐��̒��ł����lock/unlock���g�p���Ă͂Ȃ�Ȃ�.
	 * ��d���b�N�ƂȂ��ăt���[�Y���Ă��܂�.
	 * (lock/unlock��S�������Ă��Ȃ���Ԃł��̊֐����ĂԂƃX���b�h�Z�[�t�ł�
	 * �Ȃ��Ȃ邪�A����ł�����l�X�g���ē�d�ɂ�����ƁA���x�͓�d���b�N�ɂ��
	 * �f�b�h���b�N��ԂɊׂ�)
	 */
	ZnkSAtomDB* the_err_atomdb = theErrAtomDBPtr();
	if( *the_err_atomdb == NULL ){
		*the_err_atomdb = ZnkSAtomDB_create();
	}
	return *the_err_atomdb;
}


static ZnkStr st_zkbfr = NULL;

static ZnkStr theBuf( void )
{
	if( st_zkbfr == NULL ){
		st_zkbfr = ZnkStr_new( "" );
	}
	return st_zkbfr;
}


void
ZnkErr_intern( ZnkErr* ans_err, const char* err_msg )
{
	ZnkErr_D( err );
	ZnkMutex mtx = theMutex();
	ZnkMutex_lock( mtx );
	{
		ZnkSAtomDB the_err_atomdb = theErrAtomDB();
		ZnkSAtomPr_intern( M_getZnkSAtomPr(err), the_err_atomdb, err_msg );
	}
	ZnkMutex_unlock( mtx );
	if( ans_err ){ *ans_err = err; }
}
void
ZnkErr_internf( ZnkErr* ans_err, const char* fmt, ... )
{
	ZnkErr_D( err );
	ZnkMutex mtx = theMutex();
	ZnkMutex_lock( mtx );
	{
		ZnkStr the_buf = theBuf();
		ZnkSAtomDB the_err_atomdb = theErrAtomDB();
		va_list ap;
		va_start(ap, fmt);
		ZnkStr_vsnprintf( the_buf, 0, Znk_NPOS, fmt, ap );
		va_end(ap);
		ZnkSAtomPr_intern( M_getZnkSAtomPr(err), the_err_atomdb, ZnkStr_cstr(the_buf) );
	}
	ZnkMutex_unlock( mtx );
	if( ans_err ){ *ans_err = err; }
}
const char*
ZnkErr_cstr( const ZnkErr c1err )
{
	const char* ans = NULL;
	const ZnkSAtomPr* atompr;
	ZnkMutex mtx = theMutex();
	ZnkMutex_lock( mtx );
	atompr = M_getZnkSAtomPr( c1err );
	ans = atompr->db_ ? ZnkSAtomPr_cstr( atompr ) : "";
	ZnkMutex_unlock( mtx );
	return ans;
}

void
ZnkErr_initiate( void )
{
	theMutex();
	theErrAtomDB();
}
void
ZnkErr_finalize( void )
{
	/* thread unsafe */
	ZnkMutex*  mtx_ptr = theMutexPtr();
	ZnkSAtomDB* the_err_atomdb = theErrAtomDBPtr();
	if( *the_err_atomdb != NULL ){
		ZnkSAtomDB_destroy( *the_err_atomdb );
	}
	if( *mtx_ptr != NULL ){
		ZnkMutex_destroy( *mtx_ptr );
	}
	if( st_zkbfr ){
		ZnkStr_destroy( st_zkbfr );
		st_zkbfr = NULL;
	}
}

