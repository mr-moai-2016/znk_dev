#include "Est_config.h"
#include "Est_base.h"
#include "Est_assort.h"
#include <Rano_sset.h>
#include <Rano_log.h>
#include <Rano_file_info.h>
#include <Znk_dir.h>
#include <Znk_dir_recursive.h>
#include <Znk_str_ex.h>
#include <Znk_htp_util.h>
#include <Znk_mem_find.h>
#include <stdio.h>

typedef struct {
	const char* title_;
	char        src_dir_[ 256 ];
	char        dst_dir_[ 256 ];
	ZnkStr      ermsg_;
	ZnkMyf      src_myf_;
	ZnkMyf      dst_myf_;
} MoveInfo;

static bool st_is_delete_src = false;

static bool report_onEnterDir( ZnkDirRecursive recur, const char* cur_dir, void* arg, size_t local_err_num )
{
	MoveInfo* info = Znk_force_ptr_cast( MoveInfo*, arg );
	const char* title    = info->title_;
	const char* src_dir  = info->src_dir_;
	const char* dst_dir  = info->dst_dir_;
	ZnkStr      ermsg    = info->ermsg_;

	if( ermsg ){
		ZnkStr_addf( ermsg, "%s : onEnterDir : [%s]\n", title, cur_dir );
	}
	{
		char src_myf_path[ 256 ] = "";
		char dst_myf_path[ 256 ] = "";
		char dst_dir_path[ 256 ] = "";
		const char* under_dir = cur_dir + Znk_strlen(src_dir);
		bool src_myf_exist = false;
		bool dst_myf_exist = false;

		if( under_dir[ 0 ] == '/' ){
			++under_dir;
		}

		Znk_snprintf( src_myf_path, sizeof(src_myf_path), "%s/info_list.myf", cur_dir );
		Znk_snprintf( dst_myf_path, sizeof(dst_myf_path), "%s/%s/info_list.myf", dst_dir, under_dir );
		Znk_snprintf( dst_dir_path,  sizeof(dst_dir_path),  "%s/%s", dst_dir, under_dir );

		ZnkMyf_clear( info->src_myf_ );
		ZnkMyf_clear( info->dst_myf_ );

		if( ZnkDir_getType( src_myf_path ) == ZnkDirType_e_File ){
			if( ZnkMyf_load(  info->src_myf_, src_myf_path ) ){
				src_myf_exist = true;
			}
		}
		if( ZnkDir_getType( dst_myf_path ) == ZnkDirType_e_File ){
			if( ZnkMyf_load(  info->dst_myf_, dst_myf_path ) ){
				dst_myf_exist = true;
			}
		} else {
			ZnkMyf_set_quote( info->dst_myf_, "['", "']" );
			ZnkDir_mkdirPath( dst_dir_path, Znk_NPOS, '/', NULL );
		}

		ZnkStr_addf( ermsg, "dst_myf_path=[%s]\n", dst_myf_path );
		ZnkStr_addf( ermsg, "src_myf_exist=[%d] dst_myf_exist=[%d]\n", src_myf_exist, dst_myf_exist );

		//if( src_myf_exist && dst_myf_exist ){
		if( src_myf_exist ){
			ZnkStrAry tkns = ZnkStrAry_create( true );
			char src_file_path[ 256 ] = "";
			char dst_file_path[ 256 ] = "";

			ZnkStrEx_addSplitC( tkns,
					under_dir, Znk_NPOS,
					'/', false, 4 );
			if( ZnkStrAry_size( tkns ) >= 2 ){
				const char* extdir = ZnkStrAry_at_cstr( tkns, 0 );
				const char* subdir_xx = ZnkStrAry_at_cstr( tkns, 1 );
				ZnkDirId dirid = ZnkDir_openDir( cur_dir );

				while( dirid ){
					const char* name = ZnkDir_readDir( dirid );
					const char* ext  = NULL;
					if( name == NULL ){
						break; /* すべてイテレートしつくした. */
					}
					ext = ZnkS_get_extension( name, '.' );

					if( ZnkS_eq( name, "info_list.myf" ) ){
						continue; /* skip */
					}

					if( ZnkS_eqCase( extdir, ext ) && ZnkS_eqCaseEx( subdir_xx, name, 2 ) ){
						ZnkVarpAry src_vars = ZnkMyf_intern_vars( info->src_myf_, name );
						ZnkVarpAry dst_vars = ZnkMyf_intern_vars( info->dst_myf_, name );
						ZnkVarp    src_var  = ZnkVarpAry_findObj_byName( src_vars, "file_tags", Znk_NPOS, false );
						ZnkVarp    dst_var  = ZnkVarpAry_findObj_byName( dst_vars, "file_tags", Znk_NPOS, false );

						if( src_var == NULL ){
							src_var = ZnkVarp_create( "file_tags", "", 0, ZnkPrim_e_Str, NULL );
							ZnkVarpAry_push_bk( src_vars, src_var );
						}
						if( dst_var == NULL ){
							dst_var = ZnkVarp_create( "file_tags", "", 0, ZnkPrim_e_Str, NULL );
							ZnkVarpAry_push_bk( dst_vars, dst_var );
						}
						{
							ZnkStr src_str = ZnkVar_str( src_var );
							ZnkStr dst_str = ZnkVar_str( dst_var );
							RanoSSet_addSeq( dst_str, ZnkStr_cstr(src_str) );
						}

						Znk_snprintf( src_file_path, sizeof(src_file_path), "%s/%s/%s", src_dir, under_dir, name );
						Znk_snprintf( dst_file_path, sizeof(dst_file_path), "%s/%s/%s", dst_dir, under_dir, name );
						if( ermsg ){
							ZnkStr_addf( ermsg, "src_file=[%s] => dst_file=[%s] OK.\n", src_file_path, dst_file_path );
						}

						{
							ZnkDate  date = { 0 };
							ZnkStr str_dst = NULL;
							RanoFileInfo_getLastUpdatedTime( src_file_path, &date );
							dst_var = ZnkVarpAry_find_byName_literal( dst_vars, "file_time", false );
							if( dst_var == NULL ){
								dst_var = ZnkVarp_create( "file_time", "", 0, ZnkPrim_e_Str, NULL );
								ZnkVarpAry_push_bk( dst_vars, dst_var );
							}
							str_dst = ZnkVar_str( dst_var );
							ZnkDate_getStr( str_dst, 0, &date, ZnkDateStr_e_Std );
						}

						{
							uint64_t file_size = 0;
							ZnkStr str_dst = NULL;
							RanoFileInfo_getFileSize( src_file_path, &file_size );
							dst_var = ZnkVarpAry_find_byName_literal( dst_vars, "file_size", false );
							if( dst_var == NULL ){
								dst_var = ZnkVarp_create( "file_size", "", 0, ZnkPrim_e_Str, NULL );
								ZnkVarpAry_push_bk( dst_vars, dst_var );
							}
							str_dst = ZnkVar_str( dst_var );
							ZnkStr_setf( str_dst, "%I64u", file_size );
						}

						if( ZnkDir_copyFile_byForce( src_file_path, dst_file_path, ermsg ) ){
							if( st_is_delete_src ){
								if( ZnkDir_deleteFile_byForce( src_file_path ) ){
								}
							}
						}

					} else {
						ZnkStr_addf( ermsg, "file is not match to extdir and subdir_xx. cur_dir=[%s] file=[%s]\n", cur_dir, name );
					}
				}
				ZnkDir_closeDir( dirid );

				if( ZnkMyf_save(  info->dst_myf_, dst_myf_path ) ){
					if( ermsg ){
						ZnkStr_addf( ermsg, "ZnkMyf_save:dst_myf_path=[%s]OK.\n", dst_myf_path );
					}
					if( st_is_delete_src ){
						if( ZnkDir_deleteFile_byForce( src_myf_path ) ){
						}
					}
				}

			} else{
				ZnkStr_addf( ermsg, "tkns num is not match. under_dir=[%s]\n", under_dir );
			}
			ZnkStrAry_destroy( tkns );
		} else if( src_myf_exist ){
		} else if( dst_myf_exist ){
		}

	}

	return true;
}
static bool move_processFile( ZnkDirRecursive recur, const char* file_path, void* arg, size_t local_err_num )
{
	return true;
}
static bool rmdir_onExitDir( ZnkDirRecursive recur, const char* cur_dir, void* arg, size_t local_err_num )
{
	bool result = false;
	MoveInfo* info = Znk_force_ptr_cast( MoveInfo*, arg );
	const char* title = info->title_;
	const char* src_dir  = info->src_dir_;
	ZnkStr      ermsg = info->ermsg_;

	{
		const char* under_dir = cur_dir + Znk_strlen(src_dir);

		if( under_dir[ 0 ] == '/' ){
			++under_dir;
		}

		if( st_is_delete_src && !ZnkS_empty(under_dir) ){
			if( ZnkDir_rmdir( cur_dir ) ){
				if( ermsg ){
					ZnkStr_addf( ermsg, "%s : rmdir : [%s] OK.\n", title, cur_dir );
				}
				result = true;
			} else {
				if( ermsg ){
					ZnkStr_addf( ermsg, "%s : rmdir : [%s] failure.\n", title, cur_dir );
				}
			}
		}

	}
	if( local_err_num ){
		if( ermsg ){
			ZnkStr_addf( ermsg, "%s : rmdir_onExitDir : local_err_num = [%zu].\n", title, local_err_num );
		}
		result = false;
	}
	return result;
}

static bool
importDir( const char* src_dir, const char* dst_dir, ZnkStr ermsg )
{
	bool is_err_ignore = true;
	MoveInfo info = { 0 };
	ZnkDirRecursive recur = NULL;

	info.ermsg_ = ermsg;
	info.title_ = "importDir";
	ZnkS_copy( info.src_dir_, sizeof(info.src_dir_), src_dir, Znk_NPOS );
	ZnkS_copy( info.dst_dir_, sizeof(info.dst_dir_), dst_dir, Znk_NPOS );
	info.src_myf_ = ZnkMyf_create();
	info.dst_myf_ = ZnkMyf_create();

	if( ZnkS_empty( dst_dir ) ){
		return false;
	}
	if( ZnkS_empty( src_dir ) ){
		return false;
	}
	
	recur = ZnkDirRecursive_create( is_err_ignore,
			report_onEnterDir, &info,
			move_processFile,  &info,
			rmdir_onExitDir,   &info );
	ZnkDirRecursive_traverse( recur, src_dir, NULL );
	ZnkDirRecursive_destroy( recur );

	ZnkMyf_destroy( info.src_myf_ );
	ZnkMyf_destroy( info.dst_myf_ );

	return true;
}

int main(int argc, char **argv)
{
	const char* src_dir = NULL;
	const char* dst_dir = NULL;
	ZnkStr ermsg = ZnkStr_new( "" );

	if( argc < 3 ){
		return 1;
	}
	src_dir = argv[ 1 ];
	dst_dir = argv[ 2 ];

	if( argc >= 4 ){
		st_is_delete_src = ZnkS_eq( argv[ 3 ], "move" );
	}

	if( importDir( src_dir, dst_dir, ermsg ) ){
		Znk_printf( "importDir is success.\n" );
	} else {
		Znk_printf( "importDir is failure.\n" );
	}
	Znk_printf( "ermsg=[%s].\n", ZnkStr_cstr(ermsg) );

	return 0;
}
