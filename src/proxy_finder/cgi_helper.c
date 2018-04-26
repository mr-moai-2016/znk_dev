#include "cgi_helper.h"

#include <Znk_varp_ary.h>
#include <Znk_str_path.h>
#include <Znk_missing_libc.h>
#include <Znk_htp_util.h>

#if defined(Znk_TARGET_WINDOWS)
static bool st_is_dos_path = true;
#else
static bool st_is_dos_path = false;
#endif

const char*
CGIHelper_theMoaiDir( ZnkStr ermsg )
{
	static ZnkStr st_moai_dir = NULL;
	if( st_moai_dir == NULL ){
		const char dsp = st_is_dos_path ? '\\' : '/';
		static const size_t depth = 5;
		st_moai_dir = ZnkStr_new( "../" );
		if( !ZnkStrPath_searchParentDir( st_moai_dir, depth, "target.myf", ZnkDirType_e_File, dsp ) ){
			if( ermsg ){
				ZnkStr_add( ermsg, "Error : Searching moai_dir is failure. target.myf does not found." );
			}
			return NULL;
		}
	}
	return ZnkStr_cstr( st_moai_dir );
}

const char*
CGIHelper_loadAuthenticKey( ZnkStr ermsg )
{
	static char st_moai_authentic_key[ 256 ] = "";
	const char* moai_dir = CGIHelper_theMoaiDir( ermsg );
	char        path[ 256 ] = "";
	ZnkFile     fp = NULL;

	Znk_snprintf( path, sizeof(path), "%sauthentic_key.dat", moai_dir );
	fp = Znk_fopen( path, "rb" );
	if( fp ){
		Znk_fgets( st_moai_authentic_key, sizeof(st_moai_authentic_key), fp );
		Znk_fclose( fp );
		return st_moai_authentic_key;
	}
	return NULL;
}

bool
CGIHelper_isAuthenticated( ZnkVarpAry post_vars, const char* authentic_key )
{
	ZnkVarp varp = ZnkVarpAry_findObj_byName_literal( post_vars, "Moai_AuthenticKey", false );
	if( varp ){
		/* post_vars��Moai_AuthenticKey���^�����Ă���A���� authentic_key�ɓ������ꍇ�̂ݔF�� */
		if( ZnkS_eq( authentic_key, ZnkVar_cstr( varp ) ) ){
			return true;
		}
	}
	return false;
}

bool
CGIHelper_initEsscential( ZnkVarpAry post_vars,
		const char** ans_moai_dir, const char** ans_authentic_key, bool* ans_is_authenticated,
		ZnkStr ermsg )
{
	bool        result = false;
	const char* moai_dir = NULL;
	const char* authentic_key = NULL;
	bool        is_authenticated = false;

	/***
	 * moai�f�B���N�g���̎������o.
	 */
	moai_dir = CGIHelper_theMoaiDir( ermsg );
	if( moai_dir == NULL ){
		/* error */
		if( ermsg ){
			ZnkStr_addf( ermsg, "Error : moai_dir is not found.\n" );
		}
		goto FUNC_END;
	}
	if( ermsg ){
		ZnkStr_addf( ermsg, "Report : moai_dir is detected : [%s].\n", moai_dir );
	}

	/***
	 * authentic_key �̃��[�h.
	 * (���̃L�[�̎��̂́Amoai���N�����Ă���ꍇ�K��moai_dir�����ɑ��݂��܂�)
	 */
	authentic_key = CGIHelper_loadAuthenticKey( ermsg );
	if( authentic_key == NULL ){
		/* error */
		if( ermsg ){
			ZnkStr_addf( ermsg, "Error : Moai authentic_key cannot load.\n" );
		}
		goto FUNC_END;
	}

	/***
	 * Moai�F��.
	 *
	 * ���ꃆ�[�U�̈Ӑ}���Ȃ����炩�̃X�N���v�g�����Ŏ��s���ꂽ�ꍇ�A
	 * CGI�̎d�l��A���K��UI����Ȃ��Ă����N�G�X�g�𔭍s���邱�Ƃ��ł��Ă��܂��܂�.
	 * ���N�G�X�g�Ɋ܂܂��Moai_AuthenticKey�̒l�𒲂ׂ邱�ƂŁA���̃��N�G�X�g���m����
	 * ����CGI�A�v���P�[�V�����̐��K��UI����ă��[�U���炪���s�������̂ł��邱�Ƃ����؂��܂�.
	 *
	 * ���Ƀt�@�C����ǂݏ���������A���炩�̃V�X�e���̃��\�[�X�փA�N�Z�X����ȂǁA
	 * �Z�L�����e�B����茘�łȏ�ԂŎ��s�����������Ɋւ��ẮA
	 * ���̎��؂ɍ��i�����ꍇ�̂ݎ��s�������邱�Ƃ����������߂��܂�.
	 */
	is_authenticated = CGIHelper_isAuthenticated( post_vars, authentic_key );

	result = true;
FUNC_END:
	if( ans_moai_dir         ){ *ans_moai_dir = moai_dir; }
	if( ans_authentic_key    ){ *ans_authentic_key = authentic_key; }
	if( ans_is_authenticated ){ *ans_is_authenticated = is_authenticated; }

	return result;
}

bool
CGIHelper_printTemplateHTML( RanoCGIEVar* evar, ZnkBird bird, const char* template_html_file, const char* authentic_key, ZnkStr ermsg )
{
	if( authentic_key ){
		ZnkBird_regist( bird, "Moai_AuthenticKey", authentic_key );
	}

	ZnkHtpURL_negateHtmlTagEffection( ermsg ); /* for XSS */
	RanoCGIUtil_replaceNLtoHtmlBR( ermsg );
	ZnkBird_regist( bird, "ermsg", ZnkStr_cstr(ermsg) );
	return RanoCGIUtil_printTemplateHTML( evar, bird, template_html_file );
}
