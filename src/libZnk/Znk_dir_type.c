#include "Znk_dir.h"
#include "Znk_sys_errno.h"
#include "Znk_stdc.h"


/***
 * �����n�ˑ��V�X�e���R�[��(API)
 */
#if   defined(Znk_TARGET_WINDOWS)
#  include <windows.h>
#elif defined(Znk_TARGET_UNIX)
/* UNIX(Linux) */
#  include <dirent.h>
#  include <unistd.h>
/* for stat. */
#  include <sys/types.h>
#  include <sys/stat.h>
#else
#  error "not supported"
#endif

/* for errno of system call */
#include <errno.h>
#include <string.h>
#include <stdio.h> /* for perror */

/***
 * Windows �� _errno() �� mterrno.c �� dosmaperr.c�̂Q�ɒ�`����Ă���.
 * �O�҂ł́A_MT ����`����Ă��Ȃ��ꍇ�̎��̂��`���A��҂ł� _MT ����`
 * ����Ă���ꍇ�̎��̂��`���Ă���. (�Ȃ�����ȕʁX�̃t�@�C���ł��̂悤��
 * ��`�̎d�������Ă���̂��͕s��). _MT ����`����Ă���A�܂�}���`�X���b�h
 * �ł�C���C�u�����ł���ꍇ�A�ȉ��̂悤�ɂȂ��Ă���A���̂�_getptd()�֐����
 * �擾���邱�ƂɂȂ�. ���� _getptd �֐��� tidtable.c�ɒ�`����Ă���.
 *
 * int * __cdecl _errno( void) { return ( &(_getptd()->_terrno) ); }
 *
 * ����_getptd �� TLS����̗̈悩��\���̃f�[�^���擾���Ă���悤�����A
 * �����ł͂���ȏ�̐[����͂��Ȃ�.
 * �Ȃɂ͂Ƃ�����AWindows�� errno ��(�}���`�X���b�h��C���C�u�������g���Ă���
 * ����) thread-safe�ɂȂ��Ă���悤�ł���.
 */

#if defined(Znk_TARGET_WINDOWS)

struct errentry {
	uint32_t oscode_;    /* OS return value */
	uint32_t errnocode_; /* System V error code */
};

static struct errentry errtable[] = {
	{ ERROR_INVALID_FUNCTION,       EINVAL    },  /* 1 */
	{ ERROR_FILE_NOT_FOUND,         ENOENT    },  /* 2 */
	{ ERROR_PATH_NOT_FOUND,         ENOENT    },  /* 3 */
	{ ERROR_TOO_MANY_OPEN_FILES,    EMFILE    },  /* 4 */
	{ ERROR_ACCESS_DENIED,          EACCES    },  /* 5 */
	{ ERROR_INVALID_HANDLE,         EBADF     },  /* 6 */
	{ ERROR_ARENA_TRASHED,          ENOMEM    },  /* 7 */
	{ ERROR_NOT_ENOUGH_MEMORY,      ENOMEM    },  /* 8 */
	{ ERROR_INVALID_BLOCK,          ENOMEM    },  /* 9 */
	{ ERROR_BAD_ENVIRONMENT,        E2BIG     },  /* 10 */
	{ ERROR_BAD_FORMAT,             ENOEXEC   },  /* 11 */
	{ ERROR_INVALID_ACCESS,         EINVAL    },  /* 12 */
	{ ERROR_INVALID_DATA,           EINVAL    },  /* 13 */
	{ ERROR_INVALID_DRIVE,          ENOENT    },  /* 15 */
	{ ERROR_CURRENT_DIRECTORY,      EACCES    },  /* 16 */
	{ ERROR_NOT_SAME_DEVICE,        EXDEV     },  /* 17 */
	{ ERROR_NO_MORE_FILES,          ENOENT    },  /* 18 */
	{ ERROR_LOCK_VIOLATION,         EACCES    },  /* 33 */
	{ ERROR_BAD_NETPATH,            ENOENT    },  /* 53 */
	{ ERROR_NETWORK_ACCESS_DENIED,  EACCES    },  /* 65 */
	{ ERROR_BAD_NET_NAME,           ENOENT    },  /* 67 */
	{ ERROR_FILE_EXISTS,            EEXIST    },  /* 80 */
	{ ERROR_CANNOT_MAKE,            EACCES    },  /* 82 */
	{ ERROR_FAIL_I24,               EACCES    },  /* 83 */
	{ ERROR_INVALID_PARAMETER,      EINVAL    },  /* 87 */
	{ ERROR_NO_PROC_SLOTS,          EAGAIN    },  /* 89 */
	{ ERROR_DRIVE_LOCKED,           EACCES    },  /* 108 */
	{ ERROR_BROKEN_PIPE,            EPIPE     },  /* 109 */
	{ ERROR_DISK_FULL,              ENOSPC    },  /* 112 */
	{ ERROR_INVALID_TARGET_HANDLE,  EBADF     },  /* 114 */
	{ ERROR_INVALID_HANDLE,         EINVAL    },  /* 124 */
	{ ERROR_WAIT_NO_CHILDREN,       ECHILD    },  /* 128 */
	{ ERROR_CHILD_NOT_COMPLETE,     ECHILD    },  /* 129 */
	{ ERROR_DIRECT_ACCESS_HANDLE,   EBADF     },  /* 130 */
	{ ERROR_NEGATIVE_SEEK,          EINVAL    },  /* 131 */
	{ ERROR_SEEK_ON_DEVICE,         EACCES    },  /* 132 */
	{ ERROR_DIR_NOT_EMPTY,          ENOTEMPTY },  /* 145 */
	{ ERROR_NOT_LOCKED,             EACCES    },  /* 158 */
	{ ERROR_BAD_PATHNAME,           ENOENT    },  /* 161 */
	{ ERROR_MAX_THRDS_REACHED,      EAGAIN    },  /* 164 */
	{ ERROR_LOCK_FAILED,            EACCES    },  /* 167 */
	{ ERROR_ALREADY_EXISTS,         EEXIST    },  /* 183 */
	{ ERROR_FILENAME_EXCED_RANGE,   ENOENT    },  /* 206 */
	{ ERROR_NESTING_NOT_ALLOWED,    EAGAIN    },  /* 215 */
	{ ERROR_NOT_ENOUGH_QUOTA,       ENOMEM    }   /* 1816 */
};

/* size of the table */
#define ERRTABLESIZE (sizeof(errtable)/sizeof(errtable[0]))

/* The following two constants must be the minimum and maximum
   values in the (contiguous) range of Exec Failure errors. */
/* WinError 188 */
#define MIN_EXEC_ERROR ERROR_INVALID_STARTING_CODESEG
/* WinError 202 */
#define MAX_EXEC_ERROR ERROR_INFLOOP_IN_RELOC_CHAIN

/* These are the low and high value in the range of errors that are
   access violations */
/* WinError 19 */
#define MIN_EACCES_RANGE ERROR_WRITE_PROTECT
/* WinError 36 */
#define MAX_EACCES_RANGE ERROR_SHARING_BUFFER_EXCEEDED


/***
 * void _dosmaperr(oserrno) - Map function number
 * 
 * Purpose:
 *        This function takes an OS error number, and maps it to the
 *        corresponding errno value (based on UNIX System V values). The
 *        OS error number is stored in _doserrno (and the mapped value is
 *        stored in errno)
 * 
 * Entry:
 *        ULONG oserrno = OS error value
 * 
 * Exit:
 *        sets _doserrno and errno.
 * 
 * Exceptions:
 */
static uint32_t
getUNIXSystemVErrno_fromWinErrCode( uint32_t oserrno )
{
	uint32_t ans_errno;
	int i;
	/* check the table for the OS error code */
	for( i=0; i < ERRTABLESIZE; ++i ){
		if (oserrno == errtable[i].oscode_) {
			ans_errno = errtable[i].errnocode_;
			return ans_errno;
		}
	}
	
	/* The error code wasn't in the table.  We check for a range of */
	/* EACCES errors or exec failure errors (ENOEXEC).  Otherwise   */
	/* EINVAL is returned.                                          */
	if (oserrno >= MIN_EACCES_RANGE && oserrno <= MAX_EACCES_RANGE){
		/***
		 * ERROR_WRITE_PROTECT(=19) ���� ERROR_SHARING_BUFFER_EXCEEDED(=36) �̊Ԃł���΁A
		 * �S��EACCES�Ƃ݂Ȃ�.
		 */
		ans_errno = EACCES;
	} else if (oserrno >= MIN_EXEC_ERROR && oserrno <= MAX_EXEC_ERROR) {
		/***
		 * ERROR_INVALID_STARTING_CODESEG(=188) ���� ERROR_INFLOOP_IN_RELOC_CHAIN(=202) �̊Ԃł���΁A
		 * �S��ENOEXEC�Ƃ݂Ȃ�.
		 */
		ans_errno = ENOEXEC;
	} else {
		/***
		 * ��L�ȊO�͂��ׂ� EINVAL �Ƃ݂Ȃ�.
		 */
		ans_errno = EINVAL;
	}
	return ans_errno;
}

#if 0
static void
getLastErrMsg( ZnkStr msg, uint32_t sys_err_code )
{
	LPVOID lpMsgBuf;
	/* �G���[�\��������쐬 */
	FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL, (DWORD)sys_err_code,
			MAKELANGID(LANG_NEUTRAL,SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL );
	ZnkStr_append( msg, (const char*)lpMsgBuf, Znk_NPOS );
	LocalFree(lpMsgBuf);
}
#endif
static void
printLastErrMsg( const char* prefix, uint32_t sys_err_code )
{
	LPVOID lpMsgBuf;
	/* �G���[�\��������쐬 */
	FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL, (DWORD)sys_err_code,
			MAKELANGID(LANG_NEUTRAL,SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL );
	Znk_printf_e( "%s : %s\n", prefix, (const char*)lpMsgBuf );
	LocalFree(lpMsgBuf);
}

static ZnkDirType
getType_byFindFirstFile( const char* path )
{
	WIN32_FIND_DATA FindData;
	ZnkDirType ans = ZnkDirType_e_CannotKnow;
	HANDLE hFind;
	ZnkStr path_str = ZnkStr_new( path );
	while( ZnkStr_last( path_str ) == '/' || ZnkStr_last( path_str ) == '\\' ){
		ZnkStr_cut_back( path_str, 1 );
	}
	//Znk_printf_e( "ZnkDir_getType(use FindFirstFile) : modified path=[%s]\n", ZnkStr_cstr(path_str) );
	if( ZnkStr_empty( path_str ) ){
		/* �����ȃp�X. �܂葶�݂��Ȃ��Ƃ݂Ȃ���. */
		ZnkStr_delete( path_str );
		return ZnkDirType_e_NotFound;
	}
	hFind = FindFirstFile( ZnkStr_cstr(path_str), &FindData);
	ZnkStr_delete( path_str );

	if( hFind == INVALID_HANDLE_VALUE ){
		/***
		 * �z��O�̏�...
		 * ���邢�́AGetFileAttributes �� FindFirstFile���Ă΂�邿�傤�ǂ��̊Ԃ̏u�Ԃ�
		 * (���Ɋ�ՓI�ȃ^�C�~���O����)�Y���t�@�C�����폜���ꂽ�Ȃǂ̉\���͂���.
		 */
		uint32_t win_errcode = (uint32_t)GetLastError();
		uint32_t sys_errno = getUNIXSystemVErrno_fromWinErrCode( win_errcode );
		Znk_printf_e( "ZnkDir_getType : getType_byFindFirstFile : hFind is INVALID_HANDLE_VALUE. Very rare case.\n" );
		switch( sys_errno ){
		case ENOENT:
			/* file/directory does not exist. */
			Znk_printf_e( "ZnkDir_getType : getType_byFindFirstFile : Recognize NotFound. win_errcode=[%I32u]\n", win_errcode );
			printLastErrMsg( "  OSErrMsg=", win_errcode );
			return ZnkDirType_e_NotFound;
		default:
		{
			/***
			 * GetFileAttributes��FindFirstFile�̗����ł��擾�s��
			 * �����݂��Ă���Ƃ����Ȃ�炩�̏�Ԃɂ���.
			 */
			ZnkSysErrnoInfo* info = ZnkSysErrno_getInfo( sys_errno );
			Znk_printf_e( "ZnkDir_getType : getType_byFindFirstFile : sys_errno=[%s](%s)\n",
					info->sys_errno_key_, info->sys_errno_msg_ );
			break;
		}
		}
		Znk_printf_e( "ZnkDir_getType : getType_byFindFirstFile : Recognize CannotKnow. win_errcode=[%I32u]\n", win_errcode );
		printLastErrMsg( "  OSErrMsg=", win_errcode );
		return ZnkDirType_e_CannotKnow;
	}
	FindClose(hFind);

	if( FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ){
		ans = ZnkDirType_e_Directory;
	} else {
		ans = ZnkDirType_e_File;
	}
	return ans;
}
#endif


#if defined(Znk_TARGET_UNIX)
Znk_INLINE ZnkDirType
getStatErrorType( uint32_t syscall_err )
{
	ZnkDirType dir_type = ZnkDirType_e_CannotKnow; 
	bool is_irregular = true;
	/***
	 * stat���G���[�ƂȂ����ꍇ�A-1 ��Ԃ��A
	 * errno�ɂ͈ȉ��̒l���ݒ肳���.
	 *
	 * EACCES:
	 *   ������ Permition denied �G���[�̐��̂͂���ł���.
	 *   ����������̓p�X�̍Ō�ɓ��B����O�̃I�u�W�F�N�g�ɂ�����T�[�`�p�[�~�b�V����
	 *   �Ɋւ�����̂ł���.
	 *
	 *   �Ⴆ�΁A A/B/C �ƌ����`�Ńp�X���w�肳�ꂽ�Ƃ��A�Ō��C���O�̂���
	 *   �܂肱�̏ꍇ�AA,B�̂��������ꂩ�̃T�[�`�p�[�~�b�V����(Directory��
	 *   �t�����ꂽ x �p�[�~�b�V�����̂���)�̃t���O�������Ă��Ȃ��ꍇ�A
	 *   ������[���̒T���͕s�\�ɂȂ�A�Ō��C�܂ł��ǂ蒅���Ȃ�.
	 *   ���̏ꍇ�AEACCES����������. ���̃G���[�����������ꍇ�AC�̑��ݐ���
	 *   �m�F����͕̂s�\�ł���Ƃ������ƂɂȂ�(���݂��邩������Ȃ������Ȃ�
	 *   ��������Ȃ����A���݂̃��[�U�̌����ł͂���ȏ�m�F���邷�ׂ��Ȃ�).
	 *
	 * EBADF:
	 *   Bad fd �ł���A�ƋL�ڂ���Ă���.
	 *   �����炭�́AA/B/C�Ƃ������ꍇ�ɂ����̂����̂����ꂩ��fd�����Ă����
	 *   �������Ƃ��낤. ����̓t�@�C���V�X�e�����ꕔ�j�󂳂�Ă���Ƃ������Ƃ���
	 *   ����Ȃ����A�p�X���̂͑��݂��Ă���\��������. ���̏ꍇ�� CannotKnow �Ƃ��Ă悢���낤.
	 *
	 * EFAULT:
	 *   �r����Segmentation Falt�����������Ƃ������Ƃ炵��.
	 *   �p�X������NULL�I�[���Ă��Ȃ������邢�͕s���ȃ|�C���^�l�ł������ꍇ�Ȃ�
	 *   �ɔ���������̂Ǝv����. ������ɂ���s���ȃp�X�Ƃ������ƂȂ̂�
	 *   NotFound�Ƃ��Ă悢�Ǝv����.
	 *
	 * ELOOP:
	 *   �p�X��H���Ă���Ƃ��ɓr���ŃV���{���b�N�����N�ɏo���킵���Ƃ���.
	 *   ���ꂪ�Ō���Ȃ�A���̃V���{���b�N�����N���̂��t�@�C���Ƃ݂Ȃ��ď����擾
	 *   �Ƃ����l�������ł��邪�A�����ł͂Ȃ��Ƃ��A�������炳��ɐ[���֍s���Ȃǂ���ɂ�
	 *   �܂����̃V���{���b�N�����N����������������˂΂Ȃ�Ȃ�. �������A�V���{���b�N�����N��
	 *   �悪����ɃV���{���b�N�����N�ł���ꍇ������A�����Ă��ꂪ���܂�ɑ������A
	 *   ���邢�̓��[�v���Ă���ꍇ�́A�G���[�Ƃ݂Ȃ��Ē��f����.
	 *   ���̏ꍇ�A�^����ꂽ�p�X�͂����̑��݂��Ă��Ȃ����̂������Ă���Ƃ��Ƃ��̂�
	 *   NotFound�Ƃ��Ă悢�Ǝv����.
	 *
	 * ENAMETOOLONG:
	 *   �p�X�����񂪒�������Ƃ������Ƃ��낤.
	 *   ���̏ꍇ��NotFound�Ƃ��Ă悢���낤.
	 *
	 * ENOENT:
	 *   ������ No such file or directory �G���[�̐��̂͂���ł���.
	 *   �������NotFound���̂��̂ł���.
	 *
	 * ENOMEM:
	 *   �J�[�l���ɂ����郁�����m�ۃG���[.
	 *   ���̂悤�ȏ󋵂���������ꍇ�͂ނ���v���Z�X���I�����ׂ����Ǝv���邪�A
	 *   �����ł� CannotKnow��Ԃ�.
	 *
	 * ENOTDIR:
	 *   No Directory �G���[�ƌĂ΂��.
	 *   �Ⴆ�΁A A/B/C �ƌ����`�Ńp�X���w�肳�ꂽ�Ƃ��A�r����B���f�B���N�g���ł�
	 *   �Ȃ��t�@�C���ł������ꍇ�A����͘_�������ł���A���̃G���[�����s�����.
	 *   �v����ɁA�w�肳�ꂽ�p�X�� C �͑��݂��Ȃ��Ƃ������ƂȂ̂ŁANotFound�ł���.
	 *
	 * EOVERFLOW:
	 *   �t�@�C���T�C�Y�擾�Ɋւ��āA���܂�ɑ傫�ȃt�@�C���T�C�Y�ł��������߁A
	 *   ���̎擾�Ɏ��s����(�T�C�Y�i�[�p�ϐ������e�ʂ𒴂��A�I�[�o�[�t���[���N������)
	 *   ���Ƃ�����. �t�@�C�����̂͌����Ă���̂ŁA����͑��݂͂��Ă���.
	 */
	switch( syscall_err ){
	case EACCES:
		dir_type = ZnkDirType_e_CannotKnow;
		break;
	case EBADF:
		dir_type = ZnkDirType_e_CannotKnow;
		break;
	case EFAULT:
		dir_type = ZnkDirType_e_NotFound;
		break;
	case ELOOP:
		dir_type = ZnkDirType_e_NotFound;
		break;
	case ENAMETOOLONG:
		dir_type = ZnkDirType_e_NotFound;
		break;
	case ENOENT:
		dir_type = ZnkDirType_e_NotFound;
		is_irregular = false;
		break;
	case ENOMEM:
		/* exit ���ׂ��� ? */
		dir_type = ZnkDirType_e_CannotKnow;
		break;
	case ENOTDIR:
		dir_type = ZnkDirType_e_NotFound;
		is_irregular = false;
		break;
	case EOVERFLOW:
		/***
		 * �t�@�C���Ƃ��đ��݂��邱�Ƃ͊m��.
		 */
		dir_type = ZnkDirType_e_File;
		is_irregular = false;
		break;
	default:
		dir_type = ZnkDirType_e_CannotKnow;
		break;
	}
	if( is_irregular ){
		perror( "ZnkDir_getType" );
	}
	return dir_type;
}
#endif


ZnkDirType
ZnkDir_getType( const char* path )
{

#if   defined(Znk_TARGET_WINDOWS)

/***
 * INVALID_FILE_ATTRIBUTES:
 * ����� 0xFFFFFFFF �Ƃ��đ�p����邱�Ƃ�����.
 * �܂������n�ɂ���Ă͒�`����Ă��Ȃ����Ƃ����邽�߁A�����Ŋm���ɒ�`���Ă���.
 */
#ifndef   INVALID_FILE_ATTRIBUTES
#  define INVALID_FILE_ATTRIBUTES ((DWORD)-1)
#endif

	/* Windows */
	uint32_t attr = GetFileAttributes( path );
	//Znk_printf_e( "ZnkDir_getType(use GetFileAttributes) : path=[%s]\n", path );
	if( attr == INVALID_FILE_ATTRIBUTES ){
		/***
		 * GetLastError:
		 * Retrieves the calling thread's last-error code value.
		 * The last-error code is maintained on a per-thread basis.
		 * Multiple threads do not overwrite each other's last-error code.;
		 */
		uint32_t win_errcode = (uint32_t)GetLastError();
		uint32_t sys_errno = getUNIXSystemVErrno_fromWinErrCode( win_errcode );
		switch( sys_errno ){
		case ENOENT:
			/* file/directory does not exist. */
			//Znk_printf_e( "ZnkDir_getType : Recognize NotFound. win_errcode=[%I32u]\n", win_errcode );
			//printLastErrMsg( "  OSErrMsg=", win_errcode );
			return ZnkDirType_e_NotFound;
		default:
		{
			/***
			 * ����� win_errcode �� ERROR_SHARING_VIOLATION �̏ꍇ�A
			 * FindFirstFile�ɂ��_�u���`�F�b�N���s��.
			 * ���ݐ������Ȃ�� ERROR_SHARING_VIOLATION �̎��_�Ŋm��͂��Ă��邪�A
			 * ���ꂪFile�Ȃ̂�Directory�Ȃ̂����킩��Ȃ�.
			 *
			 * path���t�@�C���Ȃ�΁A�ŌオDSP�ł���\���͏��O�ł���.
			 * ���̂悤�ȏꍇ�AGetFileAttributes �͗D��I��ERROR_INVALID_NAME �G���[�Ƃ���.
			 * ���ꂪ���Ƃ��AERROR_SHARING_VIOLATION �𔭐�������悤�ȃt�@�C���ł�
			 * �܂��� ERROR_INVALID_NAME ���D�悳���.
			 * path���f�B���N�g���Ȃ�΁A�ŌオDSP�ł���\���͔r���ł��Ȃ�.
			 * �v����ɂ����ł́A�ŌオDSP�ł���ꍇ�͂܂�������J�b�g���Ȃ���΂Ȃ�Ȃ����낤.
			 */
			if( win_errcode == ERROR_SHARING_VIOLATION ){
				return getType_byFindFirstFile( path );
			} 
			break;
		}
		}
		//C1_log( "ZnkDir_getType : Recognize CannotKnow. win_errcode=[%u]\n", win_errcode );
		//printLastErrMsg( "  OSErrMsg=", win_errcode );
		return ZnkDirType_e_CannotKnow;
	}
	if( attr & FILE_ATTRIBUTE_DIRECTORY ){
		/***
		 * �B���t�@�C�����������f�B���N�g�����A�K������ attr == FILE_ATTRIBUTE_DIRECTORY ��
		 * �Ȃ�Ȃ��ꍇ������. ���̂��߁A�����ł͕K��FILE_ATTRIBUTE_DIRECTORY �r�b�g�������Ă���
		 * ���ۂ��Ŕ��肵�Ȃ���΂Ȃ�Ȃ�.
		 */
		return ZnkDirType_e_Directory;
	}
	/* ��L�ȊO�̓t�@�C���Ƃ݂Ȃ� */
	return ZnkDirType_e_File;

#elif defined(Znk_TARGET_UNIX)
	/* Unix */
	ZnkDirType dir_type = ZnkDirType_e_CannotKnow;
	struct stat statbuf;
	if( stat(path, &statbuf) == -1 ){
		/***
		 * errno��thread local�ł��邽�߁A����X���b�h��errno�������݂�
		 * ���X���b�h��errno�̒l�ɉe�����邱�Ƃ͂Ȃ�.
		 */
		uint32_t syscall_err = errno; /* look global errno */
		/***
		 * ���̂Ƃ��Astatbuf.st_mode�̒l�͐������Ȃ����߁A�Q�Ƃ���Ӗ��͂Ȃ�.
		 * (���ہAPermition Denied �n�̃G���[�̏ꍇ�ȂǁA�������ݒ肳��Ă͂��Ȃ�.)
		 */
		dir_type = getStatErrorType( syscall_err );
		return dir_type;
	}

	if( statbuf.st_mode & S_IFMT & S_IFDIR ){
		/* this is directory */
		return ZnkDirType_e_Directory;
	}
	return ZnkDirType_e_File;

#else
#  error "not supported"
#endif
}


ZnkDirIdentity
ZnkDir_getIdentity( const char* path1, const char* path2 )
{
#if   defined(Znk_TARGET_WINDOWS)
	ZnkDirIdentity identity = ZnkDirIdentity_e_CannotKnow;
	ZnkDirType dir_type1 = ZnkDir_getType( path1 );
	ZnkDirType dir_type2 = ZnkDir_getType( path2 );

	/***
	 * �܂��A�@���Ȃ�ꍇ���ǂ��炩����܂��͗����� NotFound�̏ꍇ��
	 * NotFound��Ԃ�.
	 */
	if( dir_type1 == ZnkDirType_e_NotFound || dir_type2 == ZnkDirType_e_NotFound ){
		return ZnkDirIdentity_e_NotFound;
	}
	/***
	 * ���ɁA�@���Ȃ�ꍇ���ǂ��炩����܂��͗����� CannotKnow�̏ꍇ��
	 * CannotKnow��Ԃ�.
	 */
	if( dir_type1 == ZnkDirType_e_CannotKnow || dir_type2 == ZnkDirType_e_CannotKnow ){
		return ZnkDirIdentity_e_CannotKnow;
	}
	/***
	 * �ȍ~�͂�����̏ꍇ��Directory�܂���File�ȊO���肦�Ȃ�.
	 */
	if( path1 == path2 || strcmp( path1, path2 ) == 0 ){
		/***
		 * ������̃��x���œ�����. Same�Ɣ��f����.
		 */
		return ZnkDirIdentity_e_Same;
	}

	{
		bool is_directory1 = (bool)( dir_type1 == ZnkDirType_e_Directory );
		bool is_directory2 = (bool)( dir_type2 == ZnkDirType_e_Directory );
		DWORD dwFlagsAndAttributes1 = FILE_ATTRIBUTE_NORMAL;
		DWORD dwFlagsAndAttributes2 = FILE_ATTRIBUTE_NORMAL;
		HANDLE hFile1 = NULL;
		HANDLE hFile2 = NULL;
		BY_HANDLE_FILE_INFORMATION FileInfo1;
		BY_HANDLE_FILE_INFORMATION FileInfo2;
	
		/***
		 * Directory Handler ��CreateFile�ɂĎ擾����ɂ�
		 * FILE_FLAG_BACKUP_SEMANTICS �����Ă����K�v������.
		 * ����ɂ���ē���ꂽ HANDLE �́AGetFileInformationByHandle ��
		 * �����Ƃ��ēn�����Ƃ��ł���.
		 */
		if( is_directory1 ){ dwFlagsAndAttributes1 |= FILE_FLAG_BACKUP_SEMANTICS; }
		if( is_directory2 ){ dwFlagsAndAttributes2 |= FILE_FLAG_BACKUP_SEMANTICS; }
	
		hFile1 = CreateFile( path1, 0,
			FILE_SHARE_READ | FILE_SHARE_WRITE,
			NULL, OPEN_EXISTING, dwFlagsAndAttributes1, NULL);
	
		hFile2 = CreateFile( path2, 0,
			FILE_SHARE_READ | FILE_SHARE_WRITE,
			NULL, OPEN_EXISTING, dwFlagsAndAttributes2, NULL);
	
		if( GetFileInformationByHandle( hFile1, &FileInfo1 ) == 0 ){
			/* ���s */
			identity = ( dir_type1 != dir_type2 ) ?
				ZnkDirIdentity_e_DifferAndCannotKnowDevice : ZnkDirIdentity_e_CannotKnow;
			goto FUNC_END;
		}
		if( GetFileInformationByHandle( hFile2, &FileInfo2 ) == 0 ){
			/* ���s */
			identity = ( dir_type1 != dir_type2 ) ?
				ZnkDirIdentity_e_DifferAndCannotKnowDevice : ZnkDirIdentity_e_CannotKnow;
			goto FUNC_END;
		}

		if( FileInfo1.dwVolumeSerialNumber == FileInfo2.dwVolumeSerialNumber ){
			if(  FileInfo1.nFileIndexHigh == FileInfo2.nFileIndexHigh
			  && FileInfo1.nFileIndexLow  == FileInfo2.nFileIndexLow  )
			{
				identity = ZnkDirIdentity_e_Same;
			} else {
				identity = ZnkDirIdentity_e_DifferAndSameDevice;
			}
		} else {
			identity = ZnkDirIdentity_e_DifferAndDifferDevice;
		}

FUNC_END:
		CloseHandle(hFile1);
		CloseHandle(hFile2);
	}

	return identity;

#elif defined(Znk_TARGET_UNIX)

	/* Unix */
	ZnkDirType dir_type1 = ZnkDirType_e_CannotKnow;
	ZnkDirType dir_type2 = ZnkDirType_e_CannotKnow;
	struct stat statbuf1;
	struct stat statbuf2;
	bool statbuf_success1 = true;
	bool statbuf_success2 = true;

	if( stat(path1, &statbuf1) == -1 ){
		uint32_t syscall_err = errno; /* look global errno */
		dir_type1 = getStatErrorType( syscall_err );
		statbuf_success1 = false;
	}
	if( stat(path2, &statbuf2) == -1 ){
		uint32_t syscall_err = errno; /* look global errno */
		dir_type2 = getStatErrorType( syscall_err );
		statbuf_success2 = false;
	}

	if( statbuf_success1 && statbuf_success2 ){
		if( statbuf1.st_dev == statbuf2.st_dev ){
			if( statbuf1.st_ino == statbuf2.st_ino ){
				return ZnkDirIdentity_e_Same;
			} else {
				return ZnkDirIdentity_e_DifferAndSameDevice;
			}
		} else {
			return ZnkDirIdentity_e_DifferAndDifferDevice;
		}
	} else {
		/***
		 * �܂��A�@���Ȃ�ꍇ���ǂ��炩����܂��͗����� NotFound�̏ꍇ��
		 * NotFound��Ԃ�.
		 */
		if( dir_type1 == ZnkDirType_e_NotFound || dir_type2 == ZnkDirType_e_NotFound ){
			return ZnkDirIdentity_e_NotFound;
		}
		/***
		 * ���ɁA�@���Ȃ�ꍇ���ǂ��炩����܂��͗����� CannotKnow�̏ꍇ��
		 * CannotKnow��Ԃ�.
		 */
		if( dir_type1 == ZnkDirType_e_CannotKnow || dir_type2 == ZnkDirType_e_CannotKnow ){
			return ZnkDirIdentity_e_CannotKnow;
		}
		/***
		 * �ȍ~�͂�����̏ꍇ��Directory�܂���File�ȊO���肦�Ȃ�.
		 */
		if( path1 == path2 || strcmp( path1, path2 ) == 0 ){
			/***
			 * ������̃��x���œ�����. Same�Ɣ��f����.
			 */
			return ZnkDirIdentity_e_Same;
		}
	}
	return ( dir_type1 != dir_type2 ) ?
		ZnkDirIdentity_e_DifferAndCannotKnowDevice : ZnkDirIdentity_e_CannotKnow;

#else
#  error "not supported"
#endif
}

