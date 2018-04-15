#include <Znk_dir_recursive.h>
#include <Znk_dir.h>
#include <Znk_str.h>
#include <Znk_str_ary.h>
#include <Znk_err.h>
#include <Znk_stdc.h>

struct ZnkDirRecursiveImpl {
	ZnkStrAry name_stack_;
	ZnkStr    wkstr_;
	ZnkDirRecursiveFuncArg funcarg_onEnterDir_;
	ZnkDirRecursiveFuncArg funcarg_processFile_;
	ZnkDirRecursiveFuncArg funcarg_onExitDir_;
	int       level_;
	bool      is_err_ignore_;
};


ZnkDirRecursive
ZnkDirRecursive_create( bool is_err_ignore,
		const ZnkDirRecursiveFuncT func_onEnterDir,  void* arg_onEnterDir,
		const ZnkDirRecursiveFuncT func_processFile, void* arg_processFile,
		const ZnkDirRecursiveFuncT func_onExitDir,   void* arg_onExitDir )
{
	ZnkDirRecursive recur = Znk_malloc( sizeof( struct ZnkDirRecursiveImpl ) );
	recur->name_stack_ = ZnkStrAry_create( true );
	recur->wkstr_         = ZnkStr_new( "" );
	//recur->err_id_list_   = ZnkBfr_create_null();
	Znk_FUNCARG_INIT( recur->funcarg_onEnterDir_,  func_onEnterDir,  arg_onEnterDir );
	Znk_FUNCARG_INIT( recur->funcarg_processFile_, func_processFile, arg_processFile );
	Znk_FUNCARG_INIT( recur->funcarg_onExitDir_,   func_onExitDir,   arg_onExitDir );
	recur->level_         = 0;
	recur->is_err_ignore_ = is_err_ignore;
	return recur;
}
void
ZnkDirRecursive_destroy( ZnkDirRecursive recur )
{
	if( recur ){
		ZnkStrAry_destroy( recur->name_stack_ );
		ZnkStr_destroy( recur->wkstr_ );
		//ZnkBfr_destroy( recur->err_id_list_ );
		recur->level_ = 0;
		Znk_free( recur );
	}
}

static const char*
pushName( ZnkDirRecursive recur, const char* name, size_t name_leng )
{
	const char* last_name;
	ZnkStrAry_push_bk_cstr( recur->name_stack_, name, name_leng ); /* deep-copy */
	last_name = ZnkStrAry_at_cstr( recur->name_stack_, ZnkStrAry_size( recur->name_stack_ ) - 1 );
	++recur->level_;
	return last_name;
}
static void
popName( ZnkDirRecursive recur )
{
	ZnkStrAry_pop_bk( recur->name_stack_ );
	if( recur->level_ > 0 ){
		--recur->level_;
	}
}

ZnkDirRecursiveFuncArg*
ZnkDirRecursive_configFnca( ZnkDirRecursive recur, ZnkDirRecursiveFncaType fnca_type )
{
	switch( fnca_type ){
	case ZnkDirRecursiveFnca_e_onEnterDir:  return &recur->funcarg_onEnterDir_;
	case ZnkDirRecursiveFnca_e_processFile: return &recur->funcarg_processFile_;
	case ZnkDirRecursiveFnca_e_onExitDir:   return &recur->funcarg_onExitDir_;
	default: break;
	}
	return NULL;
}

void
ZnkDirRecursive_setErrorIgnore( ZnkDirRecursive recur, bool is_err_ignore )
{
	recur->is_err_ignore_ = is_err_ignore;
}

bool
ZnkDirRecursive_traverse( ZnkDirRecursive recur, const char* top_dir, ZnkStr ermsg )
{
	size_t top_dir_leng = 0;
	/***
	 * ����top_dir�����Ɍ��肵���͈͂ł̃G���[������.
	 */
	size_t    local_err_num = 0;
	bool      last_result;
	char      last_ch;
	ZnkDirType dir_type;
	ZnkDirId   dir_id;
	
	if( recur->funcarg_onEnterDir_.func_ ){
		last_result = recur->funcarg_onEnterDir_.func_( recur,
				top_dir, recur->funcarg_onEnterDir_.arg_, local_err_num );
		if( !last_result ){
			if( ermsg ){
				const char* action_str = recur->is_err_ignore_ ? "Ignored" : "Interrupted";
				ZnkStr_addf( ermsg,
						"ZnkDirRecursive_traverse : Error %s. : onEnterDir failure. directory=[%s].",
						action_str, top_dir );
			}
			++local_err_num;
		}
	}

	dir_id = ZnkDir_openDir( top_dir );
	if( dir_id == NULL ){
		/* Cannot open top_dir */
		if( ermsg ){
			ZnkStr_addf( ermsg, "ZnkDirRecursive_traverse : Error : Cannot open top_dir=[%s]\n", top_dir );
		}
		return false;
	}

	top_dir_leng = Znk_strlen( top_dir );
	/***
	 * top_dir ���X�^�b�N�Ƀv�b�V�����A�ȉ��̏����ł͂��̗̈�ւ̃|�C���^�Ƃ݂Ȃ�.
	 * ���AVStr�̐�������e�v�f��C1MarshalBuf���w����̐��f�[�^�ł��镶����ւ̃|�C���^�l�́A
	 * �X�^�b�N�̊g���k���Ɋւ�炸���݂������ɂ����Ė����Ƃ͂Ȃ�Ȃ�.
	 */
	top_dir = pushName( recur, top_dir, top_dir_leng );

	while( true ){
		const char* obj = ZnkDir_readDir( dir_id );
		if( obj == NULL ){ break; }

		/***
		 * The path should be absolute path.
		 */
		ZnkStr_assign( recur->wkstr_, 0, top_dir, top_dir_leng );
		last_ch = ZnkStr_last( recur->wkstr_ );
		if( last_ch != '/' && last_ch != '\\' ){
			ZnkStr_add_c( recur->wkstr_, '/' );
		}
		ZnkStr_add( recur->wkstr_, obj );

		dir_type = ZnkDir_getType( ZnkStr_cstr(recur->wkstr_) );
		switch( dir_type ){
		case ZnkDirType_e_Directory:
			/* obj is a directory. */
			/* ���̌Ăяo���ȍ~�́Awkstr_�̓��e�͖����ƂȂ��Ă���ƍl���˂΂Ȃ�Ȃ�. */
			last_result = ZnkDirRecursive_traverse( recur, ZnkStr_cstr(recur->wkstr_), ermsg );
			if( !last_result ){
				++local_err_num;
				/***
				 * �����Ŗ������đ����邩�r���Œ��f����̂���is_err_ignore_�Ő؂�ւ���.
				 */
				if( recur->is_err_ignore_ ){ continue; } else { goto FUNC_END; }
			}
			break;
		case ZnkDirType_e_File:
			/* obj is a file */
			if( recur->funcarg_processFile_.func_ ){
				last_result = recur->funcarg_processFile_.func_( recur,
						ZnkStr_cstr(recur->wkstr_), recur->funcarg_processFile_.arg_, local_err_num );
				if( !last_result ){
					if( ermsg ){
						const char* action_str = recur->is_err_ignore_ ? "Ignored" : "Interrupted";
						ZnkStr_addf( ermsg,
								"ZnkDirRecursive_traverse : Error %s. : processFile failure : file=[%s].",
								action_str, ZnkStr_cstr(recur->wkstr_) );
					}
					++local_err_num;

					/***
					 * �����Ŗ������đ����邩�r���Œ��f����̂���is_err_ignore_�Ő؂�ւ���.
					 */
					if( recur->is_err_ignore_ ){ continue; } else { goto FUNC_END; }
				}
			}
			break;
		case ZnkDirType_e_NotFound:
		{
			/* ����͒ʏ�N���蓾�Ȃ��͂��ł��邪�A���������ꍇ��Error�Ƃ��� */
			if( ermsg ){
				ZnkStr_addf( ermsg,
						"ZnkDirRecursive_traverse : Error : listing obj=[%s] type is NotFound.",
						ZnkStr_cstr(recur->wkstr_) );
			}
			if( recur->is_err_ignore_ ){ continue; } else { goto FUNC_END; }
			break;
		}
		case ZnkDirType_e_CannotKnow:
		default:
		{
			/* ����͒ʏ�N���蓾�Ȃ��͂��ł��邪�A���������ꍇ��Error�Ƃ��� */
			if( ermsg ){
				ZnkStr_addf( ermsg,
						"ZnkDirRecursive_traverse : Error : listing obj=[%s] type is CannotKnow.",
						ZnkStr_cstr(recur->wkstr_) );
			}
			if( recur->is_err_ignore_ ){ continue; } else { goto FUNC_END; }
			break;
		}
		}
	}

FUNC_END:
	ZnkDir_closeDir( dir_id );

	/***
	 * �����܂ł����ׂĐ����ł������ꍇ�́Atop_dir �̒��g�͂��ׂċ�ł���͂��ł���.
	 * �����ōŌ�� top_dir ���̂��̂�����.
	 */
	if( recur->funcarg_onExitDir_.func_ ){
		last_result = recur->funcarg_onExitDir_.func_( recur,
				top_dir, recur->funcarg_onExitDir_.arg_, local_err_num );
		if( !last_result ){
			if( ermsg ){
				const char* action_str = recur->is_err_ignore_ ? "Ignored" : "Interrupted";
				ZnkStr_addf( ermsg,
						"ZnkDirRecursive_traverse : Error %s. : onExitDir failure. directory=[%s].",
						action_str, top_dir );
			}
			++local_err_num;
		}
	}
	/***
	 * ���͂�top_dir�͕s�v.
	 * ���ꂪ�w���̈���J��(�X�^�b�N����|�b�v����).
	 */
	popName( recur );
	return (bool)( local_err_num == 0 );
}


