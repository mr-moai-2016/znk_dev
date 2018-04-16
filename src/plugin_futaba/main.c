#include <Moai_plugin_dev.h>
#include <Znk_stdc.h>
#include <Znk_def_util.h>
#include <Znk_cookie.h>
#include <Znk_htp_hdrs.h>
#include <Znk_s_base.h>
#include <Znk_str_ptn.h>
#include <Znk_str_ex.h>
#include <Znk_missing_libc.h>
#include <Znk_mem_find.h>
#include <string.h>
#include <ctype.h>

Znk_INLINE ZnkVarp
refPostVar( ZnkMyf myf, const char* var_name )
{
	return ZnkMyf_refVar( myf, "post_vars", var_name );
}
Znk_INLINE ZnkVarp
refCookieVar( ZnkMyf myf, const char* var_name )
{
	return ZnkMyf_refVar( myf, "cookie_vars", var_name );
}

/***
 * myf��USERS_password�́A���[�U����̖����I�ȍ폜�L�[�w����V�~�����[�V�������邽�߂�
 * ���̂ł���. pwd��pwdc�̒l�����Ɏ����ݒ肳���ꍇ������A���[�U����̖����I�Ȏw�肩
 * �ۂ��̋�ʂ����Ȃ����߁A���̂悤�ȕϐ���ʓr�������Ă���.
 */
static void
update_pwd( ZnkVarp pwd, ZnkVarp pwdc, ZnkVarp USERS_password )
{
	if( pwd ){
		if( USERS_password && !ZnkS_empty(ZnkVar_cstr(USERS_password)) ){
			/***
			 * ���̏ꍇ�A�Ȍ�pwdc��USERS_password�̒l�Ƃ��Ĕ��s�����͂��ł���.
			 */
			ZnkVar_set_val_Str( pwd, ZnkVar_cstr(USERS_password),
					Znk_MIN(ZnkVar_str_leng(USERS_password),8) );
		} else if( pwdc ){
			/***
			 * pwdc��cookie_vars��ɂ����Đݒ肳��Ă���Ȃ�A���̒l���̗p.
			 */
			ZnkVar_set_val_Str( pwd, ZnkVar_cstr(pwdc), ZnkVar_str_leng(pwdc) );
		} else {
			/***
			 * pwdc��cookie_vars��ɂ����Đݒ肳��Ă��Ȃ��Ȃ�A��l�Ƃ���.
			 * ���̏ꍇ�A�Ȍ�pwdc���ӂ��΂���K���Ȓl�Ƃ��Ď������s�����͂��ł���.
			 */
			ZnkVar_set_val_Str( pwd, "", 0 );
		}
	}
}

static bool
isInteger( const char* cstr )
{
	const char* p = cstr;
	while( *p ){
		if( !isdigit( *p ) ){
			return false;
		}
		++p;
	}
	return (bool)( p != cstr );
}
static bool
isUntouchable_ptua( ZnkVarp dst_ptua )
{
	/* 2016/10/07: �ŐV�̎d�l�ɂ����Ă͂�����false�ł悢 */
	return false;
}

MoaiPluginState
on_post( ZnkMyf ftr_send, ZnkVarpAry hdr_vars, ZnkVarpAry post_vars )
{
	ZnkVarp USERS_futabapt = refPostVar( ftr_send, "USERS_futabapt" );
	ZnkVarp USERS_password = refPostVar( ftr_send, "USERS_password" );
	ZnkVarp USERS_ptua     = refPostVar( ftr_send, "USERS_ptua" );
	ZnkVarp pthb = refPostVar( ftr_send, "pthb" );
	ZnkVarp pthc = refPostVar( ftr_send, "pthc" );
	ZnkVarp pwd  = refPostVar( ftr_send, "pwd" );
	ZnkVarp pwdc = refCookieVar( ftr_send, "pwdc" );
	ZnkVarp dst_ptua = ZnkVarpAry_find_byName( post_vars, "ptua", Znk_strlen_literal("ptua"), false );

	/***
	 * ftr_send��USERS_futabapt����`����Ă��Ȃ��ꍇ��
	 * localStorage�������ł�����Ɠ������ƂȂ�.
	 */
	if( USERS_futabapt ){
		/***
		 * ptfk�֐��̃G�~�����[�g.
		 * �I���W�i���ł͉E�ӂ���̂Ƃ��A���ӂւ̑�����s��Ȃ��悤�ɂ��Ă��邪�A
		 * �����ł͖������ōX�V����. ���ӂ̌Â��l���c���Ă͂Ȃ�Ȃ����߂ł���.
		 */
		if( pthb ){
			ZnkVar_set_val_Str( pthb, ZnkVar_cstr(USERS_futabapt), ZnkVar_str_leng(USERS_futabapt) );
		}
		if( pthc ){
			ZnkVar_set_val_Str( USERS_futabapt, ZnkVar_cstr(pthc), ZnkVar_str_leng(pthc) );
		}
	}

	if( dst_ptua ){
		/* �܂��ȈՂȃP�[�X��e�� */
		if( !isUntouchable_ptua( dst_ptua ) ){
			/***
			 * ptua�̎��l��transient�Ȏd�l�Ƃ��邱�ƂŋU�������킷�\��������.
			 */
			ZnkVar_set_val_Str( dst_ptua, ZnkVar_cstr(USERS_ptua), ZnkVar_str_leng(USERS_ptua) );
		}
	}

	/***
	 * ����ɂ��u���E�U��ɂ���pwd�̒l�͎Ւf����A
	 * ftr_send��ɂ���l���ւ��ɍ̗p�����.
	 */
	update_pwd( pwd, pwdc, USERS_password );
	return true;
}


struct PtnInfo {
	const char* old_ptn_;
	const char* new_ptn_;
};
static int
replaceStr_BMS( ZnkStr str, void* arg )
{
	struct PtnInfo* ptn_info = Znk_force_ptr_cast( struct PtnInfo*, arg );
	const char* old_ptn = ptn_info->old_ptn_;
	const char* new_ptn = ptn_info->new_ptn_;
	ZnkSRef sref_old;
	ZnkSRef sref_new;
	size_t occ_tbl_target[ 256 ];
	sref_old.cstr_   = old_ptn;
	sref_old.leng_   = strlen( old_ptn );
	ZnkMem_getLOccTable_forBMS( occ_tbl_target, (uint8_t*)sref_old.cstr_,   sref_old.leng_ );
	sref_new.cstr_   = new_ptn;
	sref_new.leng_   = strlen( new_ptn );

	ZnkStrEx_replace_BMS( str, 0,
			sref_old.cstr_, sref_old.leng_, occ_tbl_target,
			sref_new.cstr_, sref_new.leng_,
			Znk_NPOS, Znk_NPOS );
	return 0;
}
static int
processQuote( ZnkStr str, void* arg )
{
	ZnkStrPtn_invokeInQuote( str, "\"", "\"", "\\", NULL,
			replaceStr_BMS, arg, false );
	ZnkStrPtn_invokeInQuote( str, "\'", "\'", "\\", NULL,
			replaceStr_BMS, arg, false );
	return 0;
}

static const char*
skipProtocolPrefix( const char* src )
{
	if( Znk_strncmp( src, "http://", Znk_strlen_literal("http://") ) == 0 ){
		src += Znk_strlen_literal("http://");
	} else if( Znk_strncmp( src, "https://", Znk_strlen_literal("https://") ) == 0 ){
		src += Znk_strlen_literal("https://");
	} else if( Znk_strncmp( src, "file://", Znk_strlen_literal("file://") ) == 0 ){
		src += Znk_strlen_literal("file://");
	}
	return src;
}

MoaiPluginState
on_response( ZnkMyf ftr_send,
		ZnkVarpAry hdr_vars, ZnkStr text, const char* url )
{
	/***
	 * TODO:
	 * ������Moai�C���X�g�[����A����N�����ɂ�����send�t�B���^��VirtualUSERS��Step2������
	 * �����I�ɍs������������ׂ�.
	 *
	 * ���̂悤�ɂ���.
	 * 1. Moai�N�����ɒʐM�̔���Ȃ�Step1�����͏I�点�Ă���.
	 *    �P����GET�ł͓��R�Ȃ���POST�ϐ��̎Q�Ƃ͔��������AUserAgent�̃V���b�t���݂̂ł悢����.
	 *
	 * 2. on_response �ɂ����� ftr_send�� js=off�̏ꍇ�� �܂�Step2�����𖢎��s�Ƃ݂Ȃ��A
	 *    ��������s���A��POST�ɔ�����.
	 */
	ZnkVarp js = refPostVar( ftr_send, "js" );
	if( js && !ZnkS_eq( ZnkVar_cstr( js ), "on" ) ){
	}

	if( text ){
		{
			ZnkSRef old_ptn = { 0 };
			ZnkSRef new_ptn = { 0 };
			size_t  old_occ[ 256 ] = { 0 };

			ZnkSRef_set_literal( &old_ptn, "���ӂ���</span>" );
			ZnkSRef_set_literal( &new_ptn, "���ӂ��� <font size=\"-1\" color=\"#800080\">via Moai</font></span>" );
			ZnkMem_getLOccTable_forBMS( old_occ, (uint8_t*)old_ptn.cstr_,   old_ptn.leng_ );
			ZnkStrEx_replace_BMS( text, 0,
					old_ptn.cstr_, old_ptn.leng_, old_occ,
					new_ptn.cstr_, new_ptn.leng_,
					Znk_NPOS, Znk_NPOS );
		}

		/* Moai��LocalProxy�Ƃ��Ďg���Ȃ�A�ȉ���filter�͂Ƃ肠�����܂��K�v�ł��� */
		/* boiiinize */
		{
			struct PtnInfo ptn_info = { "iframe", "zenkakuboiiin" };
			ZnkStrPtn_invokeInQuote( text, "<!--", "-->", NULL, NULL,
					processQuote, &ptn_info, false );
			ZnkStrPtn_invokeInQuote( text, "<script", "</script", NULL, NULL,
					processQuote, &ptn_info, false );
		}

		/* dayoonize */
		ZnkStrPtn_replaceInQuote( text, "<!--", "-->", NULL, NULL,
				"<iframe", "<zenkakudayoon", false, Znk_NPOS );
		ZnkStrPtn_replaceInQuote( text, "<script", "</script", NULL, NULL,
				"<iframe", "<zenkakudayoon", false, Znk_NPOS );

		ZnkStrPtn_replaceInQuote( text, "<!--", "-->", NULL, NULL,
				"</iframe", "</zenkakudayoon", false, Znk_NPOS );
		ZnkStrPtn_replaceInQuote( text, "<script", "</script", NULL, NULL,
				"</iframe", "</zenkakudayoon", false, Znk_NPOS );
	
		/* disable iframe */
		{
			struct PtnInfo ptn_info = { "<iframe", "<!-- iframe" };
			replaceStr_BMS( text, &ptn_info );
		}
		{
			struct PtnInfo ptn_info = { "</iframe", "</iframe --><noscript></noscript" };
			replaceStr_BMS( text, &ptn_info );
		}
	
		/* de-boiiinize */
		ZnkStrPtn_replaceInQuote( text, "<script", "</script", "/*", "*/",
				"zenkakuboiiin", "iframe", false, Znk_NPOS );

	}
	return true;
}

