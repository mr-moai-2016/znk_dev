#include "Znk_dir.h"
#include "Znk_dir_recursive.h"
#include "Znk_stdc.h"
#include "Znk_s_base.h"
#include "Znk_missing_libc.h"
#include "Znk_sys_errno.h"

#if   defined(Znk_TARGET_WINDOWS)
#  include <windows.h>
#else
#  include <unistd.h>
#  include <sys/types.h>
#  include <sys/stat.h>
#  include <errno.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#if   defined(Znk_TARGET_WINDOWS)
Znk_INLINE void printWinLastError( DWORD last_err, ZnkStr ermsg )
{
	LPVOID lpMsgBuf;
	const char* msg;
	/* �G���[�\��������쐬 */
	FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL, last_err,
			MAKELANGID(LANG_NEUTRAL,SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL );
	msg = (const char*)lpMsgBuf;
	ZnkStr_add( ermsg, msg );
	LocalFree(lpMsgBuf);
}
#endif

/**
 * @code
 * Note:
 * �����n���Ƃ̎g�p�s����
 * ext2 :
 *     '\0'�A'/' �̂�(���͂��ׂĎg�p�\).
 *     ��O:
 *     "."�A".." �Ƃ����t�@�C���������邱�Ƃ͂ł��Ȃ�.
 * NTFS,FAT32 :
 *     '\\'�A'/'�A':'�A'.'�A';'�A'*'�A'?'�A'"'�A'<'�A'>'�A'|' ���g�p�s��.
 *     �������A'.' �Ɍ���A'.'���Q�����ڈȍ~�ɗ���ꍇ�͉��̖����Ȃ��g�p�\.
 *     '.'���ꕶ���ڂɂ���ꍇ�́A�G�N�X�v���[���ォ��͎g�p�����ۂ���邪�A
 *     �v���O������(fopen��)�͉��L�̗�O�������g�p�\.
 *
 *     ��L�����̂����A'\\' �� '|' �̂݁AShiftJIS�R�[�h����(�̑�2�o�C�g��)��
 *     �܂܂��ꍇ������(��). (�t�ɏ�L�����̂��̑��̂��̂ɂ��ẮAShiftJIS��
 *     �܂܂�邱�Ƃ͂Ȃ�). '\\'��'|'�Ɍ���AShiftJIS���Ŏg�p�\�ƂȂ�.
 *     ��O:
 *     " "(���p�X�y�[�X�ꕶ������)�Ƃ����t�@�C���������邱�Ƃ͂ł��Ȃ�.
 *     "."�A".." �Ƃ����t�@�C���������邱�Ƃ͂ł��Ȃ�.
 *
 * (��)ShiftJIS�̑�Q�o�C�g�ڂɂ���ASCII�����́A0x40 �ȍ~(@�ȍ~)�̕����ɂȂ�.
 *
 * Windows�ł̓t�@�C���̓��{��p�X��ShiftJIS�ɂȂ��Ă���.
 * (�����炭wfopen���̃��C�h�����łł�UTF16�A�ʏ��fopen�ł�ShiftJIS���󂯕t����Ƃ������Ƃ��낤)
 * Linux, BSD�ł̓t�@�C���̓��{��p�X��EUC��UTF-8�����蓾��(���ɍŋ߂�UTF-8�������炵��)
 * Mac�ł�UTF-8���g�p���Ă���Ƃ�����񂪂���.
 * glib�ł́AG_FILENAME_ENCODING �Ƃ������ϐ��ŁA����𖾎��I�Ɏw�肳����悤�ɂ��Ă���.
 *
 * Win API�֐�(���Ƃ��� CreateDirectory, GetFileAttributes �Ȃ�) ���󂯕t����p�X��
 * ���{��ł���ꍇ�ASJIS�̏ꍇ�ɂ����Ă��܂����삷��(���Ƃ��΁u�}�C�\�v�Z�\�t�g�v�Ƃ���
 * ������ɑ΂��āA���O�����悤�Ȗ��͔������Ȃ�). �t�ɂ���ȊO�̂��Ƃ��� UTF-8
 * �̂悤�ȕ����R�[�h�ł͕��������������̂��o���オ��A���܂������Ȃ�.
 *
 * @endcode
 */

bool
ZnkDir_changeCurrentDir( const char* dir )
{
	int result;
#if   defined(Znk_TARGET_WINDOWS)
	result = SetCurrentDirectory( dir );	
	if( result == 0 ){
		/* Permition Deny */
		return false;
	}
#else
	result = chdir( dir );
	if( result == -1 ){
		/* Permition Deny */
		return false;
	}
#endif
	return true;
}

const char*
ZnkDir_getCurrentDir( ZnkStr ans )
{
	static const size_t buf_size = 512;
	ZnkBfr_resize( ans, buf_size );

#if   defined(Znk_TARGET_WINDOWS)
	GetCurrentDirectory( (DWORD)ZnkBfr_size(ans), (LPSTR)ZnkBfr_data(ans) );	
#else
	/* UNIX */
	getcwd( (char*)ZnkBfr_data(ans), ZnkBfr_size(ans) );
#endif
	ZnkStr_normalize( ans, true );
	return ZnkStr_cstr( ans );
}


bool
ZnkDir_getFileByteSize( const char* file, int64_t* file_size )
{
	/* ���Ȃ炸binary���[�h�ŊJ������ */
	ZnkFile fp = Znk_fopen( file, "rb" );
	if( fp ){
		Znk_fseek_i64( fp, 0, SEEK_END );
		*file_size = Znk_ftell_i64( fp );
		Znk_fclose( fp );
		return true;
	}
	return false;
}


/***
 * �����ŗ^�����t�@�C�����R�s�[����B
 * (�f�B���N�g���͕s��)
 * �^���������src_file�͂������Adst_file���t�@�C�����łȂ���΂Ȃ�Ȃ�.
 * (�R�s�[��Ƃ��ăf�B���N�g�����͎w��ł��Ȃ�)
 */
bool
ZnkDir_copyFile_byForce( const char* src_file, const char* dst_file, ZnkStr ermsg )
{
#if   defined(Znk_TARGET_WINDOWS)
	/***
	 * CopyFile
	 * ?
	 */
	static const BOOL bFailIfExists = false; /* false�̏ꍇ�͋����R�s�[ */
	bool result = (bool)CopyFile( src_file, dst_file, bFailIfExists );
	if( result ){
		/* success */
		return true;
	} else {
		if( ermsg ){
			DWORD last_err = GetLastError();
			printWinLastError( last_err, ermsg );
		}
	}
	return result;
#else
	/***
	 * �蔲������Znk_snprintf��buf-overflow�����h�䂷��ΈӊO�Ǝ茘�������ł���.
	 * fork, execl, sendfile, mmap �ȂǐF�X�����o���ăS�`���S�`��������͂���.
	 */
	char cmd[ 4096 ] = "";
#  if defined(__ANDROID__)
	Znk_snprintf( cmd, sizeof(cmd), "/system/bin/cp -p '%s' '%s'", src_file, dst_file );
#  else
	Znk_snprintf( cmd, sizeof(cmd), "/bin/cp -p '%s' '%s'", src_file, dst_file );
#  endif
	system( cmd );
	return true;
#endif
}

bool
ZnkDir_deleteFile_byForce( const char* file )
{
#if   defined(Znk_TARGET_WINDOWS)
	/***
	 * DeleteFile:
	 * �t�@�C���݂̂������폜����(�f�B���N�g���͕s��)
	 */
	return (bool) DeleteFile(file);
#else
	/***
	 * unlink:
	 * �t�@�C���݂̂������폜����(�f�B���N�g���͕s��)
	 */
	if( unlink(file) == 0 ){
		return true; /* if 0, then success. */
	}
	return false;
#endif
}

bool
ZnkDir_rmdir( const char* dir )
{
#if   defined(Znk_TARGET_WINDOWS)
	/***
	 * RemoveDirectory:
	 * ��̃f�B���N�g���݂̂��폜����(�t�@�C���͕s��)
	 */
	return (bool) RemoveDirectory(dir);

#else
	/***
	 * rmdir:
	 * ��̃f�B���N�g���݂̂��폜����(�t�@�C���͕s��)
	 */
	if( rmdir(dir) == 0 ){
		/* if 0, then success. */
		return true;
	}
	/* failure. */
	return false;
#endif
}

bool
ZnkDir_rename( const char* src_path, const char* dst_path, ZnkStr ermsg )
{
#if   defined(Znk_TARGET_WINDOWS)
	/***
	 * MoveFile:
	 *  �t�@�C���܂��̓f�B���N�g���̈ړ����\.
	 *  �������A�f�B���N�g���Ɋւ��ẮA�قȂ�h���C�u(�{�����[��)�ւ̈ړ�(C:\��=>D:\����)���ł��Ȃ�.
	 *  (�t�@�C���͉\). �܂�f�B���N�g���̏ꍇ�́Asrc_path, dst_path�Ƃ��ɓ����h���C�u(�{�����[��)��
	 *  �Ȃ���΂Ȃ�Ȃ��Ƃ����. 
	 *  dst_path���w����������(���ꂪ�t�@�C���ł����Ă�)�����ɑ��݂���ꍇ�A
	 *  ���̊֐���FALSE��Ԃ����s���邱�Ƃɒ���.
	 */
	SetLastError(NO_ERROR); /* �G���[�����N���A���� */
	if( MoveFile(src_path, dst_path) ){
		/* success */
		return true;
	} else {
		if( ermsg ){
			DWORD last_err = GetLastError();
			printWinLastError( last_err, ermsg );
		}
	}
	return false;

#else
	/***
	 * rename:
	 *  �t�@�C���܂��̓f�B���N�g���̈ړ����\.
	 *  �������A�f�B���N�g���Ɋւ��ẮA�w�肵���ړ��於�����݂��Ȃ��f�B���N�g����
	 *  ��̃f�B���N�g���ł���K�v������.
	 *  �قȂ�}�E���g�f�o�C�X�ւ̈ړ����ł��Ȃ�.
	 */
	if( rename(src_path, dst_path) == 0 ){
		/* success */
		return true;
	} else {
		if( ermsg ){
			uint32_t sys_errno = ZnkSysErrno_errno();
			ZnkSysErrnoInfo* einfo = ZnkSysErrno_getInfo( sys_errno );
			ZnkStr_addf( ermsg, "Error : ZnkDir_rename src_path=[%s] dst_path=[%s] [%s]\n", src_path, dst_path, einfo->sys_errno_msg_ );
		}
	}
	return false;
#endif

}


/***
 * �����͏����n�ŗL��API��p���Ď������Ă��悢.
 *
 * dir��^���A�����ɐV�K�f�B���N�g������쐬����.
 *
 * dir�ɂ́A�V�����쐬�������f�B���N�g���̃p�X���w�肷��.
 * ���ꂪ�������̂��A���ɑ��݂��Ă��Ă͂Ȃ�Ȃ�.
 * (���݂����ꍇ�̓���ɂ��ẮA�����n�ˑ��ƂȂ�).
 *
 * �܂��Adir���w���f�B���N�g�������̐e�͊��ɑ��݂��Ă��Ȃ���΂Ȃ�Ȃ�.
 * (���݂��Ȃ��ꍇ�̓���ɂ��ẮA�����n�ˑ��ƂȂ�).
 *
 */
static bool
makeDirectory( const char* dir, ZnkStr ermsg )
{
#if   defined(Znk_TARGET_WINDOWS)
	/***
	 * CreateDirectory:
	 *  ������dir�����̐e�͊��ɑ��݂��Ă��Ȃ���΂Ȃ�Ȃ�.
	 *  �����Ȃ���΁A0 ��Ԃ�(�܂莸�s�Ƃ݂Ȃ����).
	 *  �܂��A���ɑ��݂��Ă���f�B���N�g�����w�肵���ꍇ�A
	 *  ���̊֐��� 0 ��Ԃ�(�܂莸�s�Ƃ݂Ȃ����)
	 */
	bool result = (bool)( CreateDirectory( dir, NULL ) != 0 );
	if( result ){
		/* success */
		if( ermsg ){
			ZnkStr_addf( ermsg, "makeDirectory[%s] result=[%d]\n", dir, result );
		}
		return true;
	} else {
		DWORD last_err = GetLastError();
		if( last_err == ERROR_ALREADY_EXISTS ){
			result = true;
		} else if( ermsg ){
			/***
			 * �u���ɑ��݂��Ă��܂��v���������G���[���b�Z�[�W���o��.
			 */
			printWinLastError( last_err, ermsg );
			ZnkStr_addf( ermsg, "makeDirectory[%s]\n", dir );
		}
	}
	return result;

#else
	/***
	 * mkdir:
	 *  �f�B���N�g���̍쐬.
	 *  �ȉ���mode_t �͍쐬�����f�B���N�g���̃p�[�~�b�V����������.
	 *
	 *  S_IRWXU
	 *    Setting Read, Write, and Execute Permissions for the User(Owner) Only.
	 *
	 *  S_IRGRP
	 *    Setting Read Permissions for Group.
	 *
	 *  S_IXGRP
	 *    Setting Execute Permissions for Group.
	 *
	 *  S_IROTH
	 *    Setting Read Permissions for Others.
	 *
	 *  S_IXOTH
	 *    Setting Execute Permissions for Others.
	 *
	 *  ���̊֐������������ꍇ�� 0 ��Ԃ�.
	 *  ( 0 �������̈ӂł���. ���̓_�� WinAPI CreateDirectory�Ƌt�ł��� )
	 *
	 *  dir�̐e�͊��ɑ��݂��Ă��Ȃ���΂Ȃ�Ȃ�(Arch Linux�ɂ����Ċm�F�ς�).
	 *  ���̓_�� Win32 API ��CreateDirectory�Ɠ����ł���.
	 *
	 *  �܂��Adir�Ƃ��Ċ��ɑ��݂��Ă���f�B���N�g�����w�肵���ꍇ�A���̊֐��� -1 ��Ԃ�.
	 *  (�܂莸�s�Ƃ݂Ȃ����Ƃ����_�ŁA������܂� CreateDirectory�Ɠ����ł���).
	 */
	mode_t mode = S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH;
	bool result = ( mkdir( dir, mode ) == 0 );
	if( !result ){
		uint32_t sys_errno = ZnkSysErrno_errno();
		if( sys_errno == EEXIST ){
			result = true;
		} else if( ermsg ){
			/***
			 * �u���ɑ��݂��Ă��܂��v���������G���[���b�Z�[�W���o��.
			 */
			ZnkSysErrnoInfo* einfo = ZnkSysErrno_getInfo( sys_errno );
			ZnkStr_addf( ermsg, "Error : makeDirectory[%s] [%s]\n", dir, einfo->sys_errno_msg_ );
		}
	}
	return result;
#endif
}

/***
 * �����ŏ����ł���`���́A���΃p�X�A'/'�Ŏn�܂��΃p�X�A
 * X:\�܂���X:/(DOS�^)�Ŏn�܂��΃p�X�A\\hostname�܂���//hostname(UNC�^)�Ŏn�܂��΃p�X�ł���.
 * �f�B���N�g������؂�Z�p���[�^����(�ȉ������SEP�Ə���)�Ƃ��āA/ �� \ ���T�|�[�g���Ă���A
 * ����炪��������path���������Ƃ��\�ł���.
 *
 * sep �Ƃ��� / �݂̂�F������悤�Ɏw�����邱�Ƃ��ł��A���̏ꍇ�͓��{��Ȃǂ�Multibyte������
 * �܂�path���������Ƃ��ł���. sep�ɂ���ȊO�̒l���w�肵���ꍇ�́A/ �� \ �̗�����F�����邪�A
 * ���̏ꍇ�A���{��Ȃǂ�Multibyte�������܂�ł��Ă͂Ȃ�Ȃ�. �����SJIS�ɂ����āu�\�v��u�\�v
 * �Ȃǂ̕��������ꂽ�ꍇ�A���̕�����SEP�� \ �ƌ딻�肷�鋰�ꂪ���邩��ł���.
 *
 * ���̊֐��ł́A\ �����ꂽ�ꍇ�ɂ��̒��O�̕��������̂悤�ȕ����̉\�������鎞�_�ŃG���[�Ƃ���
 * �����𒆒f��false��Ԃ�. �����ł͌����Ȕ���͂��Ȃ�. �܂�prev��Multibyte�����ɂ�����1byte��
 * �Ȃ̂�2byte�ڂȂ̂��̔���ł���. 1byte�ڂł���ꍇ�͂���end��SEP�Ƃ݂Ȃ��Ă͂Ȃ炸�A
 * 2byte�ڂ̏ꍇ�͋t��SEP�Ƃ݂Ȃ��Ȃ���΂Ȃ�Ȃ�. ���̔������������ɂ�ZnkS_lfind_one_of��
 * Multibyte�����Ή��Ƃ��Ȃ���΂Ȃ炸�R�[�h�����G������̂Ō��i�K�ł̓T�|�[�g���Ă��Ȃ�.
 */
bool
ZnkDir_mkdirPath( const char* path, size_t path_leng, char sep, ZnkStr ermsg )
{
	bool result = false;
	size_t begin = 0;
	size_t end   = 0;
	const char*  sep_set      = ( sep == '/' ) ? "/" : "\\/";
	const size_t sep_set_leng = ( sep == '/' ) ? 1 : 2;

	switch( path[0] ){
	case '\0' :
		return false;
	case '/':
		begin = 1;
		break;
	case '\\':
		/* DOS��UNC��΃p�X */
		/* hostname�̕�����skip */
		begin = ZnkS_lfind_one_of( path, 2, path_leng, sep_set, sep_set_leng );
		if( begin == Znk_NPOS ){
			return false;
		}
		++begin;
		break;
	default:
		break;
	}

	if( path_leng >= 2 ){
		if( path[ 1 ] == ':' && isalpha( path[0] ) ){
			/* DOS��΃p�X */
			begin = 3;
		}
	}

	{
		/***
		 * makeDirectory����WindowsAPI�����SystemCall�ł�NULL�I�[����path�������
		 * �n���K�v������. �����Ă����ł�sep��ʉ߂��邽�тɃf�B���N�g����e����q�ւ�
		 * �i�K�I�ɍ쐬���Ă����Ȃ���΂Ȃ炸�A���ꂼ��̌Ăяo���ɂ�����path���������
		 * �g�p�͈͂�i�K�I�Ɋg�����Ă����`�ɂȂ�.
		 *
		 * �������������ɂ́Apath����sep�̑��݂���ʒu�Ɉꎞ�I��NULL�I�[�����������A
		 * makeDirectory���Ăяo���K�v������. �������Ȃ���A����path�ɕ����񃊃e������
		 * �w�肳��Ă����ꍇ�A����͑��̊��ł�ROM�̈�ɂ���A����Ă��̕��@�ł�
		 * �Z�O�����e�[�V�����o�C�I���[�V�����������N����.
		 *
		 * �����path�𒼐ڏ��������邱�Ƃ͂����ł͏o���Ȃ�. 
		 * �ʂ̏����݉ȕ�����o�b�t�@�Ɉ�U�S�̂��R�s�[���Ă�����g���K�v������.
		 */
		char path_buf[ 1024 ] = "";
		ZnkS_copy( path_buf, sizeof(path_buf), path, path_leng );

		while( true ){
			end = ZnkS_lfind_one_of( path_buf, begin, path_leng, sep_set, sep_set_leng );
			if( end == Znk_NPOS ){
				result = makeDirectory( path_buf, ermsg ); /* �K��������̊J�n�ʒu(path_buf)����w�� */
				break;
			} else if( end == begin ){
				/***
				 * ��΃p�X�̐擪�ł��邩�A�܂��� SEP����ȏ�A�����Ă���P�[�X�Ȃǂł�
				 * �����ɗ���\��������. ����̓X�L�b�v����.
				 */
				++begin;
			} else {
				if( path_buf[ end ] == '\\' ){
					/***
					 * \ �����̒��O�� ���L�͈̔͂ł���悤�ȕ����R�[�h�ł������ꍇ��
					 * �����ŏ����𒆒f���A�G���[�Ƃ���.
					 */
					size_t prev = end-1;
					int prev_ch = path_buf[ prev ];
					if( prev_ch & 0x80 ){
						if( prev_ch <= 0x9F || prev_ch >= 0xE0 ){ 
							return false;
						}
					}
				}
				/***
				 * ���ɑ��݂��Ă���Directory�̏ꍇ�A�ȉ���result��false�ƂȂ邪
				 * ���̏ꍇ�ł����f�����A�Ō�܂ő�����.
				 */
				path_buf[ end ] = '\0'; /* �ꎞ�I�ɂ����ŏI�[���� */
				result = makeDirectory( path_buf, ermsg ); /* �K��������̊J�n�ʒu(path_buf)����w�� */
				path_buf[ end ] = '/'; /* SEP�ɖ߂� */
				begin = end + 1;
			}
		}
	}
	return result;
}

static bool rmdirAll_force_onEnterDir( ZnkDirRecursive recur, const char* top_dir, void* arg, size_t local_err_num )
{
	ZnkStr ermsg = Znk_force_ptr_cast( ZnkStr, arg );
	if( ermsg ){
		ZnkStr_addf( ermsg, "ZnkDir_rmdirAll_force : onEnterDir : [%s]\n", top_dir );
	}
	return true;
}
static bool rmdirAll_force_processFile( ZnkDirRecursive recur, const char* file_path, void* arg, size_t local_err_num )
{
	bool result = false;
	ZnkStr ermsg = Znk_force_ptr_cast( ZnkStr, arg );
	if( ZnkDir_deleteFile_byForce( file_path ) ){
		if( ermsg ){
			ZnkStr_addf( ermsg, "ZnkDir_rmdirAll_force : deleteFile : [%s] OK.\n", file_path );
		}
		result = true;
	} else {
		if( ermsg ){
			ZnkStr_addf( ermsg, "ZnkDir_rmdirAll_force : deleteFile : [%s] failure.\n", file_path );
		}
		result = false;
	}
	return result;
}
static bool rmdirAll_force_onExitDir( ZnkDirRecursive recur, const char* top_dir, void* arg, size_t local_err_num )
{
	bool result = false;
	ZnkStr ermsg = Znk_force_ptr_cast( ZnkStr, arg );
	if( local_err_num == 0 ){
		if( ZnkDir_rmdir( top_dir ) ){
			if( ermsg ){
				ZnkStr_addf( ermsg, "ZnkDir_rmdirAll_force : rmdir : [%s] OK.\n", top_dir );
			}
			result = true;
		} else {
			if( ermsg ){
				ZnkStr_addf( ermsg, "ZnkDir_rmdirAll_force : rmdir : [%s] failure.\n", top_dir );
			}
			result = false;
		}
	} else {
		if( ermsg ){
			ZnkStr_addf( ermsg, "ZnkDir_rmdirAll_force : onExitDir : local_err_num = [%zu].\n", local_err_num );
		}
		result = false;
	}
	return result;
}
bool
ZnkDir_rmdirAll_force( const char* dir, bool is_err_ignore, ZnkStr ermsg )
{
	bool result = false;
	ZnkDirRecursive recur = ZnkDirRecursive_create( is_err_ignore,
			rmdirAll_force_onEnterDir,  ermsg,
			rmdirAll_force_processFile, ermsg,
			rmdirAll_force_onExitDir,   ermsg );
	result = ZnkDirRecursive_traverse( recur, dir, ermsg );
	ZnkDirRecursive_destroy( recur );
	return result;
}
