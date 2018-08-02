#include <Moai_cgi.h>

#include <Rano_conf_util.h>

#include <Znk_str_ary.h>
#include <Znk_str_ex.h>
#include <Znk_myf.h>
#include <Znk_varp_ary.h>
#include <Znk_missing_libc.h>
#include <Znk_envvar.h>
#include <Znk_bird.h>

#include <string.h>

static ZnkMyf st_config_cgi_myf = NULL;

void
MoaiCGIManager_makeHeader( ZnkStr html, const char* title, bool inline_script )
{
	ZnkStr_add(  html, "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\" \"http://www.w3.org/TR/html4/loose.dtd\">\n" );
	ZnkStr_add(  html, "<html><head>\n" );
	ZnkStr_add(  html, "\t<META http-equiv=\"Content-type\"        content=\"text/html; charset=Shift_JIS\">\n" );
	ZnkStr_add(  html, "\t<META http-equiv=\"Content-Script-Type\" content=\"text/javascript\">\n" );
	ZnkStr_add(  html, "\t<META http-equiv=\"Content-Style-Type\"  content=\"text/css\">\n" );
	if( inline_script ){
	} else {
		ZnkStr_add(  html, "\t<link href=\"/msty.css\" rel=\"stylesheet\" type=\"text/css\" />\n" );
	}
	ZnkStr_addf( html, "\t<title>%s</title>\n", title );
	ZnkStr_add(  html, "</head>\n" );
}

bool
MoaiCGIManager_load( void )
{
	if( st_config_cgi_myf == NULL ){
		st_config_cgi_myf = ZnkMyf_create();
	}
	return ZnkMyf_load( st_config_cgi_myf, "config_cgi.myf" );
}
bool
MoaiCGIManager_save( void )
{
	if( st_config_cgi_myf ){
		return ZnkMyf_save( st_config_cgi_myf, "config_cgi.myf" );
	}
	return false;
}

const ZnkVarpAry
MoaiCGIManager_getURPAliasList( void )
{
	return st_config_cgi_myf ?
		ZnkMyf_find_vars( st_config_cgi_myf, "urp_alias_list" ) : NULL;
}

static const ZnkVarpAry
getInterpreter( void )
{
#if defined(__CYGWIN__)
	return ZnkMyf_find_vars( st_config_cgi_myf, "interpreter_cygwin" );
#elif defined(Znk_TARGET_WINDOWS)
	return ZnkMyf_find_vars( st_config_cgi_myf, "interpreter_windows" );
#elif defined(__ANDROID__)
	return ZnkMyf_find_vars( st_config_cgi_myf, "interpreter_android" );
#else
	return ZnkMyf_find_vars( st_config_cgi_myf, "interpreter_linux" );
#endif
}

ZnkVarp
MoaiCGIManager_parseURPTail( const char* urp,
		ZnkStr dir, ZnkStr filename,
		ZnkStr path_info, ZnkStr query_str, bool is_cgi_exec_deny )
{
	/***
	 * ������CGI��URL�d�l�ɂ����āA�X�N���v�g�{�̂ɑ����ĉ��̖ڈ���Ȃ� / ��؂��
	 * PATH_INFO �����w��ł���Ƃ����ƂĂ��C�����������d�l������.
	 * ���ɃT�u�f�B���N�g�����܂ނ悤�ȕ��G��CGI���^�p����Ƃ��\�����ʖ��������N�������������A
	 * �Ƃ肠����CGI�̎d�l�Ȃ̂�MoaiCGI�ł�����ɑΉ�������𓾂Ȃ�.
	 * ���̂��߂ɂ�CGI�X�N���v�g�{�̂̃t�@�C�������ǂ�ɂȂ邩���p�^�[���}�b�`���O�Ŏ��ʂ���K�v������.
	 * ���������������.
	 *
	 * /cgi-bin/script.cgi
	 *   �ŏ��ɏo������*.cgi�Ō��o.
	 *   script.cgi���N��.
	 *   #!�`�F�b�N���s��.
	 *
	 * /cgi-bin/dir/script.cgi
	 *   �ŏ��ɏo������*.cgi�Ō��o.
	 *   script.cgi���N��.
	 *   #!�`�F�b�N���s��.
	 *
	 * /cgi-bin/dir/script.cgi?arg=val
	 *   �ŏ��ɏo������*.cgi�Ō��o.
	 *   script.cgi���N��.
	 *   #!�`�F�b�N���s��.
	 *   query_str��arg=val�Ƃ���.
	 *
	 * /cgi-bin/script.cgi/pi1/pi2?arg=val
	 *   �ŏ��ɏo������*.cgi�Ō��o.
	 *   script.cgi���N��.
	 *   #!�`�F�b�N���s��.
	 *   query_str��arg=val�Ƃ���.
	 *   path_info��/pi1/pi2�Ƃ���.
	 *
	 * /cgi-bin/script.cgi/pi1/pi2?arg=val/abc
	 *   �ŏ��ɏo������*.cgi�Ō��o.
	 *   script.cgi���N��.
	 *   #!�`�F�b�N���s��.
	 *   query_str��arg=val/abc�Ƃ���.
	 *   path_info��/pi1/pi2�Ƃ���.
	 *
	 * /cgi-bin/dir/script.cgi/pi1/pi2?arg=val/abc
	 *   �ŏ��ɏo������*.cgi�Ō��o.
	 *   script.cgi���N��.
	 *   #!�`�F�b�N���s��.
	 *   query_str��arg=val/abc�Ƃ���.
	 *   path_info��/pi1/pi2�Ƃ���.
	 *
	 * /cgi-bin/dir/script.cgi/pi1/pi2?arg=val/.cgi
	 *   �ŏ��ɏo������*.cgi�Ō��o.
	 *   script.cgi���N��.
	 *   #!�`�F�b�N���s��.
	 *   query_str��arg=val/.cgi�Ƃ���.
	 *   path_info��/pi1/pi2�Ƃ���.
	 *
	 * /cgi-bin/dir/script.cgi/pi1/pi2.cgi?arg=val/.cgi
	 *   �ŏ��ɏo������*.cgi�Ō��o.
	 *   script.cgi���N��.
	 *   #!�`�F�b�N���s��.
	 *   query_str��arg=val/.cgi�Ƃ���.
	 *   path_info��/pi1/pi2.cgi�Ƃ���.
	 *
	 * /cgi-bin/dir/script
	 *   �ŏ��ɏo������script�Ƃ������O�Ō��o.
	 *   script���N��.
	 *   #!�`�F�b�N���s��.
	 *
	 * /cgi-bin/dir/script/script
	 *   �ŏ��ɏo������script�Ƃ������O�Ō��o.
	 *   ��Ԗڂ�script���N��.
	 *   #!�`�F�b�N���s��.
	 *   path_info��/script�Ƃ���.
	 *
	 * �܂���URL�̍ŏI��?������p�X�����܂ł����o����.
	 * �p�X�����͕K�� / ��؂�̃p�X�ƂȂ��Ă���ƍl���Ă悢.
	 * '/'��split���z��Ƃ�����Ńp�^�[���Ƀ}�b�`����ŏ��̗v�f�����o����.
	 * ���ꂪ�X�N���v�g�{�̂ł���A����ȍ~��PATH_INFO�Ɣ��f�ł���.
	 * �܂��p�^�[���Ƀ}�b�`����v�f�����݂��Ȃ������ꍇ�A�P�Ȃ�_�E�����[�h���ׂ��t�@�C����
	 * �f�B���N�g���Ɣ��f����.
	 */
	const ZnkVarpAry interpreter = getInterpreter();
	const char*  q = NULL;
	const size_t tbl_size  = ZnkVarpAry_size( interpreter );
	size_t       elem_idx  = 0;
	size_t       rec_idx   = 0;
	ZnkStrAry    elem_list = ZnkStrAry_create( true );
	size_t       elem_list_size;
	const char*  elem;
	ZnkVarp      varp = NULL;
	const char*  ptn = NULL;
	const char*  ext = NULL;

	q = strchr( urp, '?' );
	if( q ){
		/* [urp,q)�܂ł͈̔͂�split���� */
		ZnkStrEx_addSplitC( elem_list,
				urp, q-urp, '/', false, 4 );

		ZnkStr_set( query_str, q+1 );
	} else {
		/* urp�̑S�͈͂�split���� */
		ZnkStrEx_addSplitC( elem_list,
				urp, Znk_NPOS, '/', false, 4 );
	}

	elem_list_size = ZnkStrAry_size( elem_list );
	if( is_cgi_exec_deny ){
		/***
		 * ���̏ꍇ��PATH_INFO�����l�����Ȃ�.
		 * �܂�varp = NULL�Ƃ��ĕԂ�.
		 */
		elem_idx = elem_list_size - 1;
		varp = NULL;
	} else {
		/***
		 * PATH_INFO���̌��o...
		 */
		for( elem_idx=0; elem_idx<elem_list_size; ++elem_idx ){
			elem = ZnkStrAry_at_cstr( elem_list, elem_idx );
			ext  = ZnkS_get_extension( elem, '.' );
			for( rec_idx=0; rec_idx<tbl_size; ++rec_idx ){
				varp = ZnkVarpAry_at( interpreter, rec_idx );
				ptn  = ZnkVar_name_cstr( varp );
				if( ZnkS_eqCase( ext, ptn ) ){
					goto LOOP_END;
				}
			}
		}
	}
LOOP_END:
	if( elem_idx != elem_list_size ){
		/* found */
		ZnkStr_clear( dir );
		ZnkStrEx_addJoin( dir, elem_list,
				0, elem_idx,
				"/", 1,  8 );
		ZnkStr_set( filename, ZnkStrAry_at_cstr( elem_list, elem_idx ) );

		if( elem_list_size > elem_idx+1 ){
			ZnkStr_set( path_info, "/" );
			ZnkStrEx_addJoin( path_info, elem_list,
					elem_idx+1, elem_list_size,
					"/", 1,  8 );
		}
	} else {
		/* not found */
		varp = NULL;
		if( elem_list_size ){
			ZnkStr_clear( dir );
			ZnkStrEx_addJoin( dir, elem_list,
					0, elem_list_size-1,
					"/", 1,  8 );
			ZnkStr_set( filename, ZnkStrAry_at_cstr( elem_list, elem_list_size-1 ) );
		} else {
			ZnkStr_set( filename, "index.html" );
		}
	}

	ZnkStrAry_destroy( elem_list );

	return varp;
}

bool
MoaiCGIManager_isMatchedReqUrp( const char* req_urp, const char* urp_ptn_list_name )
{
	const ZnkStrAry ptn_list = st_config_cgi_myf ?
		ZnkMyf_find_lines( st_config_cgi_myf, urp_ptn_list_name ) : NULL;
	return (bool)( ZnkStrAry_find_isMatch( ptn_list,
				0, req_urp, Znk_strlen(req_urp), ZnkS_isMatchSWC ) != Znk_NPOS );
}

static bool
isEndOfCGIScriptName( char ch )
{
	switch( ch ){
	case '/': case '?': case '#': case '\0':
		return true;
	default:
		break;
	}
	return false;
}

#if 0
bool
MoaiCGIManager_isAuthenticReqUrp( const char* req_urp )
{
	if( ZnkS_isBegin( req_urp, "/cgis/" ) ){
		/* /cgis/authentic/... �Ƃ����p�^�[���ł��邩�ǂ���. */
		const char* p = req_urp + Znk_strlen_literal("/cgis/");
		const char* q = Znk_strchr( p, '/' );
		if( q && q > p ){
			if( (q-p) == Znk_strlen_literal("authentic") &&  ZnkS_eqEx( p, "authentic", q-p ) ){
				return true;
			}
		}
	}
	return false;
}
#endif

bool
MoaiCGIManager_isValidReqUrp_forRun( const char* req_urp )
{
	if( ZnkS_isBegin( req_urp, "/cgis/" ) ){
		/* /cgis/<��ȏ�̃f�B���N�g��>/CGI�X�N���v�g.cgi �Ƃ����p�^�[���ȊO�͔F�߂Ȃ�. */
		const char* p = req_urp + Znk_strlen_literal("/cgis/");
		const char* q = Znk_strchr( p, '/' );
		const ZnkVarpAry interpreter = getInterpreter();
		if( q && q > p ){
			const size_t tbl_size  = ZnkVarpAry_size( interpreter );
			size_t rec_idx;
			for( rec_idx=0; rec_idx<tbl_size; ++rec_idx ){
				ZnkVarp     varp = ZnkVarpAry_at( interpreter, rec_idx );
				const char* r    = NULL;
				char        ptn[ 256 ] = "";
				Znk_snprintf( ptn, sizeof(ptn), ".%s", ZnkVar_name_cstr( varp ) );
				r = Znk_strstr( q+1, ptn );
				if( r ){
					r += Znk_strlen(ptn);
					if( isEndOfCGIScriptName( *r ) ){
						return true;
					}
				}
			}
		}
	}
	return false;
}
bool
MoaiCGIManager_isValidReqUrp_forPathBegin( const char* req_urp, const char* path_begin_list_name )
{
	if( ZnkS_isBegin( req_urp, "/cgis/" ) ){
		/* /cgis/<��ȏ�̃f�B���N�g��>/CGI�X�N���v�g �Ƃ����p�^�[���łȂ���΂Ȃ�Ȃ�. */
		const char* p = req_urp + Znk_strlen_literal("/cgis/");
		const char* q = Znk_strchr( p, '/' );
		const ZnkStrAry path_begin_list = ZnkMyf_find_lines( st_config_cgi_myf, path_begin_list_name );
		if( q && q > p ){
			const size_t tbl_size  = ZnkStrAry_size( path_begin_list );
			size_t rec_idx;
			for( rec_idx=0; rec_idx<tbl_size; ++rec_idx ){
				const char* path_begin = ZnkStrAry_at_cstr( path_begin_list, rec_idx );
				if( ZnkS_isBegin( req_urp, path_begin ) ){
					char end = req_urp[ Znk_strlen(path_begin) ];
					if( isEndOfCGIScriptName( end ) ){
						return true;
					}
				}
			}
		}
	}
	return false;
}

#if 0
bool
MoaiCGIManager_isValidReqUrp_forBox( const char* req_urp, const char* box_list_name )
{
	if( ZnkS_isBegin( req_urp, "/cgis/" ) ){
		/* /cgis/<��ȏ�̃f�B���N�g��>/<box>/... �Ƃ����p�^�[���ȊO�͔F�߂Ȃ�. */
		const char* p = req_urp + Znk_strlen_literal("/cgis/");
		const char* q = Znk_strchr( p, '/' );
		const ZnkStrAry box_list = ZnkMyf_find_lines( st_config_cgi_myf, box_list_name );
		if( q && q > p ){
			const size_t tbl_size  = ZnkStrAry_size( box_list );
			size_t rec_idx;
			for( rec_idx=0; rec_idx<tbl_size; ++rec_idx ){
				const char* box  = ZnkStrAry_at_cstr( box_list, rec_idx );
				const char* r          = NULL;
				char        ptn[ 256 ] = "";
				Znk_snprintf( ptn, sizeof(ptn), "%s", box );
				r = Znk_strstr( q+1, ptn );
				if( r ){
					r += Znk_strlen(ptn);
					switch( *r ){
					case '/': case '\0':
						return true;
					default:
						break;
					}
				}
			}
		}
	}
	return false;
}
#endif

const bool
MoaiCGIManager_mapFSysDir( ZnkStr fsys_path, const char* profile_dir )
{
	if( ZnkStr_isBegin( fsys_path, "./" ) ){
		const char* p = ZnkStr_cstr( fsys_path ) + 1;
		ZnkVarpAry fsys_map_list = st_config_cgi_myf ?
			ZnkMyf_find_vars( st_config_cgi_myf, "fsys_map_list" ) : NULL;
		if( fsys_map_list ){
			const size_t size = ZnkVarpAry_size( fsys_map_list );
			size_t idx;
			for( idx=0; idx<size; ++idx ){
				ZnkVarp var = ZnkVarpAry_at( fsys_map_list, idx );
				const ZnkStrAry sda = ZnkVar_str_ary( var );
				const char* req_url = ZnkStrAry_at_cstr( sda, 0 );
				if( ZnkS_isBegin( p, req_url ) ){
					/* found */
					ZnkBird bird = ZnkBird_create( "#[", "]#" );
					static const size_t default_idx = 1;
					static const size_t envvar_idx  = 2;
					RanoConfUtil_initByStrAry_withEnvVar( fsys_path, sda, default_idx, envvar_idx,
							0, 1 /* strlen(".") */ + Znk_strlen(req_url) );
					ZnkBird_regist( bird, "profile_dir", profile_dir );
					ZnkBird_extend_self( bird, fsys_path, ZnkStr_leng(fsys_path) );
					ZnkBird_destroy( bird );
					return true;
				}
			}
		}
	}
	return false;
}
