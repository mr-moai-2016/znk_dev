#include "Mkf_seek.h"
#include <Znk_dir.h>
#include <Znk_stdc.h>
#include <Znk_s_base.h>
#include <Znk_str_ary.h>

static bool
isIgnoreDirOrFile( ZnkStrAry ignore_list, const char* name )
{
	if( ZnkStrAry_find( ignore_list, 0, name, Znk_strlen(name) ) != Znk_NPOS ){
		return true;
	}
	return false;
}
/***
 * dir����_�Ƃ��ĒT�����A���̔z���ɂ���t�@�C����list�ɁA�f�B���N�g����dir_list��
 * dir���猩�����΃p�X�Ƃ��ēo�^����. ������ignore_list�Ɋ܂܂����̂͏���.
 * �܂��A�t�@�C���̏ꍇ�͊g���q��src,hdr�Ƃ��Ĕ��f�������̈ȊO�̂��̂͏���.
 */
bool
MkfSeek_listDir( ZnkStrAry list, ZnkStrAry dir_list, const char* dir, ZnkStrAry ignore_list,
		MkfSeekFuncT_isInterestExt is_interest_ext )
{
	const char* name;
	ZnkDirId    id = NULL;
	ZnkDirType  dir_type;
	const char* ext = NULL;
	ZnkStr      subname = NULL;
	size_t      subname_leng = 0;
	bool        src_or_hdr_exist = false;

	id = ZnkDir_openDir( dir );
	if( id == NULL ){
		return false; // Error
	}

	subname = ZnkStr_new( "" );
	while( true ){
		name = ZnkDir_readDir( id );
		if( name == NULL ){
			/* ���ׂăC�e���[�g��������. */
			break;
		}
		/*
		 * name �� . �܂��� .. �ɓ��������ۂ��̃`�F�b�N�͕s�v�ł���.
		 * �����̓f�t�H���g�ł� ZnkDir_readDir���Ŏ����I�ɃX�L�b�v�����.
		 */

		if( ZnkS_eq( dir, "." ) ){
			ZnkStr_set( subname, name );
		} else {
			ZnkStr_setf2( subname, "%s/%s", dir, name );
		}
		subname_leng = ZnkStr_leng(subname);

		dir_type = ZnkDir_getType( ZnkStr_cstr(subname) );
		if( dir_type == ZnkDirType_e_Directory ){
			if( !isIgnoreDirOrFile( ignore_list, ZnkStr_cstr(subname) ) ){
				if( !MkfSeek_listDir( list, dir_list, ZnkStr_cstr(subname), ignore_list, is_interest_ext ) ){
					Znk_printf_e( "MkfSeek : Cannot open dir[%s]\n", ZnkStr_cstr(subname) );
				} else {
					Znk_printf_e( "MkfSeek : Open dir[%s]\n", ZnkStr_cstr(subname) );
				}
			}
		} else if( dir_type == ZnkDirType_e_File ){
			ext = ZnkS_get_extension( ZnkStr_cstr(subname), '.' );
			//if( isSrcFileExt( ext ) || isHdrFileExt( ext ) || isRcFileExt( ext ) ){
			if( is_interest_ext == NULL || is_interest_ext( ext ) ){
				if( !isIgnoreDirOrFile( ignore_list, ZnkStr_cstr(subname) ) ){
					ZnkStrAry_push_bk_cstr( list, ZnkStr_cstr(subname), subname_leng );
					src_or_hdr_exist = true;
				}
			}
		}
	}

	ZnkStr_delete( subname );
	ZnkDir_closeDir( id );

	/***
	 * src or hdr�����݂���f�B���N�g���Ƃ��� dir_list�ɒǉ�.
	 */
	if( src_or_hdr_exist ){
		if( ZnkStrAry_find( dir_list, 0, dir, Znk_strlen(dir) ) == Znk_NPOS ){
			ZnkStrAry_push_bk_cstr( dir_list, dir, Znk_strlen(dir) );
		}
	}
	return true;
}
