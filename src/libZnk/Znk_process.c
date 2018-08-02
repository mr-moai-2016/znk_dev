#include "Znk_process.h"	
#include "Znk_str.h"	
#include "Znk_stdc.h"	

#if defined( Znk_TARGET_WINDOWS )
#  include <windows.h>
#else
#  include <unistd.h>
#  include <sys/wait.h>
#endif

#include <stdlib.h>

unsigned int
ZnkProcess_getCurrentProcessId( void )
{
#if defined( Znk_TARGET_WINDOWS )
	return GetCurrentProcessId();
#else
	return 0;
#endif
}


static bool st_req_reprocess = false;

void
ZnkProcess_requestReprocess( void )
{
	st_req_reprocess = true;
}
bool ZnkProcess_isReqReproces( void )
{
	return st_req_reprocess;
}

/***
 * ���݂̃v���Z�X��exit����.
 */
void 
ZnkProcess_exit( bool result )
{
	exit( result ? EXIT_SUCCESS : EXIT_FAILURE );
}

#if defined( Znk_TARGET_WINDOWS )
static HANDLE
create_process( size_t argc, const char** argv, ZnkProcessConsoleType cons_type )
{
	ZnkStr cmd_str = ZnkStr_new( argv[0] );
	PROCESS_INFORMATION pi;
	STARTUPINFO si;
	DWORD dwCreationFlags = NORMAL_PRIORITY_CLASS;
	ZeroMemory( &si, sizeof(si) );
	si.cb = sizeof( si );

	{
		size_t i;
		for( i=1; i<argc; ++i ){
			ZnkStr_add_c( cmd_str, ' ' );
			ZnkStr_add( cmd_str, argv[i] );
		}
	}

	switch( cons_type ){
	case ZnkProcessConsole_e_CreateNew:
		dwCreationFlags |= CREATE_NEW_CONSOLE;
		break;
	case ZnkProcessConsole_e_Detached:
		dwCreationFlags |= DETACHED_PROCESS;
		break;
	default:
		break;
	}

	CreateProcess( NULL, (LPTSTR)ZnkStr_cstr(cmd_str),
			NULL, NULL, FALSE, dwCreationFlags, NULL, NULL, &si, &pi );
	ZnkStr_delete( cmd_str );

	/***
	 * �����Ŏ��ۂɍs���邱�Ƃ͎Q�ƃJ�E���g�̃f�N�������g�ł���.
	 */
	CloseHandle( pi.hThread );

	/***
	 * pi.hProcess�ɂ��Ă͂܂��N���[�Y���Ȃ�.
	 * (���̊֐��̊O���ł܂��g�p���鎩�R��^���邽��.
	 * ���������̑㏞�Ƃ��āA�g���I�������O���ŐӔC�������ăN���[�Y����K�v������.)
	 */

	return pi.hProcess;
}
#endif

#if defined( Znk_TARGET_UNIX )
/***
 * ���̊֐��͌��݂̃v���Z�X���w�肵���V�����v���Z�X�ɒu��������.
 * (���݂̃v���Z�X�̎��s�͂����ŏI������.
 */
static void
exec_restart( size_t argc, const char** argv )
{
	/***
	 * execv�̑�2�����́Amain��argv�̂悤��argv[0]�ɂ͎��s�t�@�C�����̂��̖̂��O��
	 * ���邱�Ƃɒ��ӂ��邱��. �܂�argv�̍Ō�̗v�f�̎��ɂ͕K��NULL���Ȃ���΂Ȃ�Ȃ�.
	 * ���̂��߁A�ȉ��ł͔O�̂��ߕʓr�z����m�ۂ��Ă���n���Ă���.
	 */
	const char* cmd = argv[0];
	char** arg_ary = Znk_malloc( (argc+1)*sizeof(const char*) );
	size_t i;
	for( i=0; i<argc; ++i ){
		arg_ary[i] = (char*)argv[i];
	}
	arg_ary[argc] = NULL;
	execv( cmd, arg_ary );
	Znk_free( arg_ary ); /* ����free�͎��s�����̂� ? (�����炭���s����Ȃ��̂ł�?) */
}
#endif


/**
 * ��:
 * system�֐��͎g���Ȃ�. ����͎��̗��R�ɂ��.
 *
 * Windows�̏ꍇ:
 *   1. system�֐��ł̓p�X��UNC�p�X(���Ȃ킿 "\\remotehost\texteditor.exe \\remotehost\document.txt"�Ƃ����悤�ȃp�X) 
 *      �̏ꍇ�A����ɋ@�\���Ȃ�.
 *
 *   2. path���̒��ɋ󔒃X�y�[�X������ꍇ�AWin32��system�֐��ł͂�������܂��F���ł��Ȃ�.
 *      �Ⴆ�΁AC:\Program Files �̂悤�ȏꍇ�A"C:\Program Files" �ƃ_�u���N�H�[�g��
 *      ��������system�֐��ɓn���Ă���������܂��F�����Ȃ�(C:\Program �Ƃ����R�}���h�͂Ȃ�
 *      �ȂǂƂ����G���[���b�Z�[�W���\�������) 
 *
 *      �ǋL:
 *      WindowsXP(SP3)�ł͂��̖��͔������Ȃ�����.
 *      �����Windows2000�����ŋN������Ȃ̂��H
 *      �v����.
 *
 *   3. system���w�肵���v���Z�X���I������܂ŁA�N�����̃X���b�h���u���b�N(��~)����.
 *
 *   CreateProcess�֐��̏ꍇ�A��L�̖��͔������Ȃ�.
 *
 * UNIX��(Cygwin���܂�)�̏ꍇ:
 *   UNIX��system�֐��̏ꍇ�A��L��Windows�̂悤�ȃp�X�Ɋւ�����͂Ȃ�.
 *   ������system�̏ꍇ����͎q�v���Z�X�̂悤�Ȉ����ɂȂ�A���ꂪ��~����܂ŋN�����̃X���b�h��
 *   �����I�Ƀu���b�N�����. ����𖳌��ɂ�����@���Ȃ����߂�͂�g���Ȃ�.
 */

void 
ZnkProcess_execChild( size_t argc, const char** argv, bool is_wait, ZnkProcessConsoleType cons_type )
{
#if defined(Znk_TARGET_WINDOWS)

	HANDLE hProcess = create_process( argc, argv, cons_type );
	if( is_wait ){
		WaitForSingleObject( hProcess, INFINITE );
	}
	/***
	 * pi.hProcess�͂����g��Ȃ��̂ł����ŃN���[�Y.
	 */
	CloseHandle( hProcess );

#else
	/***
	 * UNIX�n�� fork �͒P�ɐe�v���Z�X�̕��g�ƂȂ�q�v���Z�X��邾���ł���A�S���V�����v���Z�X��
	 * �쐬����킯�ł͂Ȃ�. �܂�exec�͐V�����v���Z�X�Ƃ��ĊJ�n��������̂́A�J�����g�̃v���Z�X��
	 * �I��点�Ă��܂�. �����UNIX�n��Windows��CreateProcess�Ɠ����̂��Ƃ�����ɂ͂��̂Q��
	 * �g�ݍ��킹��. �����UNIX�n�ł͈�ʓI��fork+exec�ƌĂ΂�Ă����@�ł���. 
	 *
	 * ���Ȃ݂�Android-NDK�ɂ����Ă� fork+exec�͎g�p���Ă͂Ȃ�Ȃ��炵��.
	 * D/Zygote ( 909): Process 7668 terminated by signal (11) 
	 * (fork+exec�ŋN������ƓˑRSEGV�ŗ����邱�Ƃ�����Ƃ̂���)
	 * ���̏ꍇ�A�o�b�N�O���E���h�Œ����ԓ����v���O������ android.app.Service�g���̂��������Ƃ̂���.
	 */
	pid_t pid;
	pid = fork();
	
	if( pid == 0 ){
		/***
		 * 0 : �q�v���Z�X.
		 * exec_restart�ŐV�����v���Z�X�ɒu��������.
		 * ����ɂ��A�v���O�����̍ŏ�����̎��s�ƂȂ�.
		 * �܂�ȍ~�̏����͎��s����Ȃ��͂�.
		 */
		exec_restart( argc, argv );

	} else if( pid > 0 ){
		/***
		 * 0���傫���ꍇ�͐e�v���Z�X.
		 * exec_restart�ŐV�����v���Z�X�ɒu��������.
		 */
		/* �q�v���Z�X�̏I���܂Ńu���b�N�������ꍇ */
		if( is_wait ){
	 		/***
			 * Windows �� WaitForSingleObject�Ɠ����̏����̂͂�.
			 */
			wait( NULL );
		}
	} else {
		/***
		 * fork���G���[.
		 */
	}

#endif
}

/***
 * ���݂̃v���Z�X���w�肵���v���O�����Ƃ��čċN������.
 */
void 
ZnkProcess_execRestart( size_t argc, const char** argv, ZnkProcessConsoleType cons_type )
{
#if defined(Znk_TARGET_WINDOWS)

	/***
	 * create_process�ł� pi.hProcess �̃N���[�Y�͍s��Ȃ��d�l�Ƃ��Ă���̂ŁA
	 * �����ŃN���[�Y����K�v������.
	 */
	HANDLE hProcess = create_process( argc, argv, cons_type );
	CloseHandle( hProcess );
	ZnkProcess_exit( true );

#else
	/***
	 * �P�Ƀv���Z�X�̍ċN���Ƃ��������Ȃ�exec�����ł悢. ���̊֐��͂܂��ɂ��������p�r�̂��̂ł���.
	 */
	exec_restart( argc, argv );

#endif
}
