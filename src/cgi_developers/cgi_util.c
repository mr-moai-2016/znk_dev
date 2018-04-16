#include "cgi_util.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#if _MSC_VER >= 1400
#  pragma warning(disable: 4996)
#endif


/***
 * Target OS�𔻒肷��}�N��.
 * TARGET_* �Ƃ����`���̖��O�ƂȂ�.
 */
#if defined(_WIN32) || defined(_WIN64) || defined(_MSC_VER) || \
	defined(__MINGW32__) || defined(__MINGW64__) || \
	defined(__BORLANDC__) || defined(__DMC__)
/* Windows API */
#  define TARGET_WINDOWS 1

#elif defined(__unix__) || defined(__linux__) || defined(__CYGWIN__) || \
	defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__)
/* POSIX UNIX + X11 ����т��̌݊��V�X�e�� */
#  define TARGET_UNIX 1

#else
#  error "Not supported OS"
#endif


#if defined(TARGET_WINDOWS)
/* windows API */
#  include <windows.h> /* for Sleep */
#  include <io.h>      /* for setmode */
#  include <fcntl.h>   /* for setmode */

#else
/* UNIX�Ƃ�����Posix���g�������� */
#  include <unistd.h> /* for usleep */

#endif

#if defined(TARGET_WINDOWS)
#endif


#define M_MIN(x,y) ( (x)<(y) ? (x) : (y) )


static void
copyStr_safely( char* buf, size_t buf_size, const char* cstr, size_t cstr_leng )
{
	if( buf_size ){
		const size_t cpy_size = M_MIN( cstr_leng, buf_size-1 );
		memmove( buf, cstr, cpy_size );
		buf[ cpy_size ] = '\0';
	}
}


static FILE* Internal_getStdFP( int no )
{
	switch( no ){
	case 0: return stdin;
	case 1: return stdout;
	case 2: return stderr;
	default:
		break;
	}
	return NULL;
}


void
CGIUtil_Internal_setMode( int no, int is_binary_mode )
{
#if defined(TARGET_WINDOWS)
	/* ���[�h�ύX�ɐ旧���Ă܂��m����fflush���Ă����K�v������ */
	fflush( (FILE*)Internal_getStdFP( no ) );
#  if defined(__BORLANDC__)
	setmode( no, is_binary_mode ? _O_BINARY : _O_TEXT );
#  else
	_setmode( no, is_binary_mode ? _O_BINARY : _O_TEXT );
#  endif
#endif
}


void
CGIUtil_sleep( size_t msec )
{
#if defined(TARGET_WINDOWS)
	Sleep( (DWORD)msec );
#else
	usleep( msec * 1000 ); /* �w��̓}�C�N���b */
#endif
}


size_t
CGIUtil_getStdInStr( char* stdin_bfr, size_t content_length )
{
	int chr  = 0;
	size_t count = 0;

	while( count < content_length ){
		/* �n���ꂽ�f�[�^��W�����͂��擾���� */
		chr = fgetc( stdin );

		/* �f�[�^�I���̃`�F�b�N */
		if( chr == EOF ){
			/* content_length�ɖ����Ȃ������� EOF���o�� */
			return count;
		} else {
			stdin_bfr[ count ] = (unsigned char)chr;
			++count;
		}
	}
	return count;
}


static char*
EnvVar_get( const char* varname )
{
	/***
	 * getenv���Ԃ��|�C���^�̓v���O���}���\�����Ȃ��`�Ŕ��ɖ��������₷���댯�ł���.
	 * ���̃|�C���^���������e�́Aputenv��setenv�̌Ăяo���ɂ���ă������������������A
	 * �|�C���^�������������\��������. ���������ꂾ���ł͂Ȃ�. getenv�֐��͂���
	 * ���O�Ƃ͗����Ɋ��ϐ������݂��郁�����̈������������ꍇ������. ����āA
	 * ���̒P�Ȃ�getenv�̌Ăяo���ɂ���Ă��烁�����������������A���̃|�C���^��
	 * �����������\��������. ���̂��Ƃ͈ꌩ�Ȃ�Ƃ��Ȃ��悤�Ȏ��̂悤�ȃR�[�h��
	 * ���S�ɕs�K���ȃR�[�h�ł��邱�Ƃ�����.
	 *
	 * const char* var_TMP  = getenv( "TMP" );
	 * const char* var_TEMP = getenv( "TEMP" ); // <= ���̎��_��var_TMP�͖��������鋰�ꂪ����! 
	 *
	 * �]���ă��[�U�́A���ꂪ�w�����e�𒼂��ɕʃo�b�t�@�փR�s�[���ׂ��ł���.
	 * ����������ł����S�ł͂Ȃ�. getenv�̓X���b�h�Z�[�t�ł��Ȃ����߁A��L�̗��
	 * �ʃo�b�t�@�փR�s�[����悤�ɂ����Ƃ��Ă��A�R�s�[����������O�ɑ��̃X���b�h�ɂ����āA
	 * putenv��getenv���Ăяo����邱�ƂŁAvar_TMP���ˑR����������A�R�s�[�Ɏ��s����V�i���I��
	 * �L�蓾��.
	 *
	 * ����āA����ɂ����h���ɂ́A������GlobalMutex�ɂ�� lock/unlock���|����K�v������.
	 * ����������cgi_util�͓�������ɗp�ӂ��ꂽ���[�e�B���e�B�Ȃ̂ł����܂ł͂��Ȃ�.
	 * �{�i�I�Ȃ��̂ɂ��Ă�libZnk��Znk_envar�Ɏ�������Ă���̂ŁA��������g�p���Ă�����������.
	 */
	const char* unsafe_ptr = NULL;
	char* ans = NULL;
	/* GlobalMutex_lock(); �}���`�X���b�h�̏ꍇ�Ȃ�K�v */
	unsafe_ptr = getenv( varname );
	if( unsafe_ptr ){
		ans = strdup( unsafe_ptr );
	}
	/* GlobalMutex_unlock(); �}���`�X���b�h�̏ꍇ�Ȃ�K�v */
	return ans;
}
static void
EnvVar_free( char* val )
{
	if( val ){
		free( val );
	}
}


CGIEVar*
CGIEVar_create( void )
{
	CGIEVar* evar = malloc( sizeof( struct CGIEVar_tag ) );
	evar->server_name_    = EnvVar_get( "SERVER_NAME" );
	evar->server_port_    = EnvVar_get( "SERVER_PORT" );
	evar->content_type_   = EnvVar_get( "CONTENT_TYPE" );
	evar->content_length_ = EnvVar_get( "CONTENT_LENGTH" );
	evar->remote_addr_    = EnvVar_get( "REMOTE_ADDR" );
	evar->remote_host_    = EnvVar_get( "REMOTE_HOST" );
	evar->remote_port_    = EnvVar_get( "REMOTE_PORT" );
	evar->request_method_ = EnvVar_get( "REQUEST_METHOD" );
	evar->query_string_   = EnvVar_get( "QUERY_STRING" );
	evar->http_cookie_    = EnvVar_get( "HTTP_COOKIE" );
	evar->http_user_agent_= EnvVar_get( "HTTP_USER_AGENT" );
	evar->http_accept_    = EnvVar_get( "HTTP_ACCEPT" );
	return evar;
}
void
CGIEVar_destroy( CGIEVar* evar )
{
	if( evar ){
		EnvVar_free( evar->server_name_ );
		EnvVar_free( evar->server_port_ );
		EnvVar_free( evar->content_type_ );
		EnvVar_free( evar->content_length_ );
		EnvVar_free( evar->remote_addr_ );
		EnvVar_free( evar->remote_host_ );
		EnvVar_free( evar->remote_port_ );
		EnvVar_free( evar->request_method_ );
		EnvVar_free( evar->query_string_ );
		EnvVar_free( evar->http_cookie_ );
		EnvVar_free( evar->http_user_agent_ );
		EnvVar_free( evar->http_accept_ );
		free( evar );
	}
}


int
CGIUtil_getQueryStringToken( const char* query_string, size_t query_idx,
		char* key, size_t key_size,
		char* val, size_t val_size )
{
	/***
	 * �܂� & �ŋ�؂�ꂽquery_idx�Ԗڂ�token�����߂�.
	 * ���̂悤��token�̊J�n�ʒu��begin,
	 * ���̏I���ʒu(token�̍Ō�̕����̎��̈ʒu)��end�Ƃ���.
	 */
	const char* begin = query_string;
	const char* end   = strchr( begin, '&' );
	while( query_idx ){
		if( end == NULL ){
			/* Error : �v�����ꂽquery_idx�Ԗڂ�token�͑��݂��Ȃ� */	
			return 0;
		}
		begin = end+1; /* &�̎� */
		end   = strchr( begin, '&' );
		--query_idx;
	}
	if( end == NULL ){
		/***
		 * �v�����ꂽquery_idx�Ԗڂ�token�͍Ō�ɂ���.
		 * end��query_string�̏I�[�����̂���ʒu�֕␳.
		 */	
		end = query_string + strlen( query_string );
	}

	/***
	 * ���� query_idx�Ԗڂ�token���� = �ʒu��T��.
	 * ������O��key, �オval�̒l�ł��邩��A
	 * ��������ꂼ��̃o�b�t�@�ɃR�s�[����.
	 */
	{
		const char* eq_pos = strchr( begin, '=' );
		if( eq_pos && eq_pos < end ){
			copyStr_safely( key, key_size, begin,    eq_pos-begin );
			copyStr_safely( val, val_size, eq_pos+1, end-(eq_pos+1) );
		} else {
			/* Error : [ begin, end ) ���� '=' �����݂��Ȃ� */	
			return 0;
		}
	}

	return 1;
}

