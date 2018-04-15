#include <Znk_envvar.h>
#include <Znk_missing_libc.h>
#include <Znk_mutex.h>
#include <stdlib.h>
#include <string.h>

char*
ZnkEnvVar_get( const char* varname )
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
	 * ���̏�Ń��[�U��getenv��putenv�𒼐ڌĂяo���悤�Ȃ��Ƃ͂����A���ZnkEnvVar_get��
	 * ZnkEnvVar_set���g���悤�ɓO�ꂷ�邱�Ƃł悤�₭���S�ƂȂ�.
	 */
	const char* unsafe_ptr = NULL;
	char* ans = NULL;
	ZnkGlobalMutex_lock();
	unsafe_ptr = getenv( varname );
	if( unsafe_ptr ){
		ans = strdup( unsafe_ptr );
	}
	ZnkGlobalMutex_unlock();
	return ans;
}

typedef struct {
	const char* varname_;
	char* cmd_str_;
} EnvVarInfo;

static EnvVarInfo** st_info_ary;
static size_t       st_info_ary_size = 0;

static EnvVarInfo*
internEnvVarInfo( const char* varname )
{
	EnvVarInfo* info;
	size_t i;
	for( i=0; i<st_info_ary_size; ++i ){
		info = st_info_ary[ i ];
		if( strcmp(info->varname_, varname) == 0 ){
			/* found */
			return info;
		}
	}
	/* not found */
	if( st_info_ary_size ){
		++st_info_ary_size;
		st_info_ary = realloc( st_info_ary, st_info_ary_size * sizeof(EnvVarInfo*) );
	} else {
		st_info_ary_size = 1;
		st_info_ary = malloc( sizeof(EnvVarInfo*) );
	}
	info = malloc( sizeof(EnvVarInfo) );
	info->varname_ = strdup( varname );
	info->cmd_str_ = NULL;
	st_info_ary[ st_info_ary_size-1 ] = info;
	return info;
}

void
ZnkEnvVar_set( const char* varname, const char* val )
{
	/***
	 * putenv�֐��Ɋւ���댯����getenv��������ɐ[���ł���.
	 * putenv�֐��́A�����Ƃ��ēn���ꂽ������̃R�s�[���쐬���Ȃ�.
	 * ���̊֐��͈����Ƃ��ēn���ꂽ������ւ̃|�C���^���O���[�o���Ȋ��z��ɑ}�����邪�A
	 * ���������L������Ԃ����o�b�t�@���w���|�C���^�� putenv �Ɉ����Ƃ��ēn���ꂽ�ꍇ�A
	 * �o�b�t�@�Ɋ��蓖�Ă�ꂽ�������́A�Ăяo�����̊֐������^�[���������ɔj�󂳂��.
	 * �v����� putenv �̈����́Astatic�ȗ̈�ւ̃|�C���^���A�܂��͓K�؂Ȏ����Ǘ����ɒu���ꂽ
	 * �q�[�v�ւ̃|�C���^�łȂ���΂Ȃ�Ȃ�.
	 *
	 * �܂��A�X���b�h�Z�[�t�ł͂Ȃ��Ƃ������Ɋւ��Ă�getenv�Ɠ��l�ł���.
	 */
	const size_t require_size = ( strlen(varname) + 1 + (val ? strlen(val) : 0) + 1 ) / 8 * 8 + 8;
	char* cmd_str = NULL;

	cmd_str = malloc( require_size );
	if( val ){
		Znk_snprintf( cmd_str, require_size, "%s=%s", varname, val );
	} else {
		/* unset */
		Znk_snprintf( cmd_str, require_size, "%s=", varname );
	}

	ZnkGlobalMutex_lock();
	{
		EnvVarInfo* info = internEnvVarInfo( varname );
		putenv( cmd_str );
		/* ��info->cmd_str_�́Aputenv���ł����Q�Ƃ���Ă��Ȃ��͂��Ȃ̂Ŕj�����A
		 * �����ݎQ�ƒ��̐V�������̂ɒu�������� */
		if( info->cmd_str_ ){ free( info->cmd_str_ ); }
		info->cmd_str_ = cmd_str;
	}
	ZnkGlobalMutex_unlock();

	if( val == NULL ){
		/* �{���Ȃ�st_info_ary��shrink���ׂ������A
		 * �Ƃ肠�������Ԃ��Ȃ��̂ŁA���͂��Ȃ� */
	}
}

void
ZnkEnvVar_free( char* val )
{
	if( val ){
		free( val );
	}
}
