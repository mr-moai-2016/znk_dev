#include <Est_parser.h>
#include <Znk_mem_find.h>
#include <Znk_str_ex.h>
#include <Znk_str_fio.h>
#include <Znk_missing_libc.h>
#include <Znk_htp_util.h>
#include <string.h>
#include <ctype.h>


Znk_INLINE bool
isASCII( uint32_t c ) { return (bool)( c < 0x80 ); }
Znk_INLINE bool
SJIS_isHankakuKatakana( uint8_t b ){ return ( b >= 0xa1 && b <= 0xdf ); }
Znk_INLINE bool
SJIS_isSecondByte( const char* base, const char* p )
{
	int lbc = 0;
	while( p > base ){
		--p;
		if( isASCII(*p) ){
			/* second or ascii */
			break;
		} else if( SJIS_isHankakuKatakana(*p) ){
			/* second or hankaku_katakana */
			break;
		} else {
			/* second or first */
		}
		++lbc;
	}
	return (bool)(lbc & 1);
}

/**
 * @brief
 *   �N�H�[�g�����ň͂܂�Ă���͈͂̂����A�J�n�N�H�[�g������蒼�ォ��I���N�H�[�g�����܂ł̕����͈̔͂��擾����.
 *   �܂�pp�Ƃ��ė^����ʒu�͊J�n�N�H�[�g�����̒���������Ă��Ȃ���΂Ȃ�Ȃ�.
 *   �܂��I���N�H�[�g������include_end_quote��true�̏ꍇ�̂ݎ擾�����.
 *   escape�V�[�P���X \ �����߂��A���̒���ɂ�����ꕶ���͂��̂܂܂̕����Ƃ��Ď擾����.
 *
 * @param pp 
 *   �J�n�N�H�[�g�����̒��������������|�C���^�̃|�C���^.
 *   
 * @param qch 
 *   �I���N�H�[�g����.
 *
 * @param ans
 *   �l�̎擾�p������.
 *   NULL���w�肷�邱�Ƃ��ł��A���̏ꍇ���̊֐��͒P�ɃN�H�[�g�����ň͂܂�Ă���͈͂�ǂݔ�΂������ɂȂ�.
 *
 * @param include_end_quote
 *   true�̏ꍇ�A�I���N�H�[�g������ans�̍Ō���ɒǉ�����.
 *
 * @return:
 *   �w�肵��qch�ł����ƏI�[���Ă����ꍇ��true��Ԃ�.
 *   qch�������O��NULL�I�[���Ă����ꍇ��false��Ԃ�.
 */
static bool
scanQuote( const char** pp, char qch, ZnkStr ans, bool include_end_quote )
{
	bool result = false;
	const char* p = *pp;
	bool pre_esc = false;
	while( *p ){
		if( *p == qch ){
			if( !pre_esc ){
				result = true;
				if( include_end_quote && ans ){ ZnkStr_add_c( ans, *p ); }
				++p;
				break;
			}
			pre_esc = false;
		} else if( *p == '\\' ){
			if( pre_esc ){
				pre_esc = false;
			} else {
				if( !SJIS_isSecondByte( *pp, p ) ){
					pre_esc = true;
				}
			}
		} else {
			/* �ʏ�̕��� */
			if( pre_esc ){
				/* �Ⴆ��\n�Ȃǂ̃P�[�X. ���̏ꍇ������pre_esc�͉������Ȃ���΂Ȃ�Ȃ�.
				 * �����������ł�\n�Ȃǂ̃P�[�X�����ۂ̉��s�R�[�h�ɕύX������͂������̂܂܂̌`���ێ�������. */
				pre_esc = false;
			}
		}
		if( ans ){ ZnkStr_add_c( ans, *p ); }
		++p;
	}
	*pp = p;
	return result;
}

/**
 * @brief
 *   �^�O���I�����邩�z���C�g�X�y�[�X���o�ꂷ��܂ł͈̔͂�ǂݍ��ݎ擾����.
 *
 * @param pp 
 *   �擾�J�n����������������|�C���^�̃|�C���^.
 *   
 * @param ans
 *   �l�̎擾�p������.
 *
 * @return:
 *   �^�O�I�[�܂��̓z���C�g�X�y�[�X���o�ꂵ���ꍇ��true��Ԃ�.
 *   ����炪�����O��NULL�I�[���Ă����ꍇ��false��Ԃ�.
 */
static bool
scanToDelimiter( const char** pp, ZnkStr ans )
{
	const char* p = *pp;

	/***
	 * XHTML��empty-tag�I���͌����ɂ� " />" �� ���p�X�y�[�X���ŏ��ɓ����K�v������.
	 * �]���āA����scanToDelimiter������ł́A���̂悤��tag�I�����o�ꂷ��\�����l������K�v�͂Ȃ�.
	 *
	 * �ł́A���p�X�y�[�X����n�܂炸�P��"/>" �Ƃ���ꍇ���g���Ƃ���empty-tag�I���Ƃ݂Ȃ����ǂ����ł��邪�A
	 * ���ۂɂ� <A HREF=http://info.5ch.net/>�Ƃ����Ⴊ���݂��A���̂Ƃ��AHREF�̒l�Ƃ��Ă� http://info.5ch.net/ 
	 * �ƂȂ�ׂ��ł���. �܂� "/>" ��tag�I�[�Ƃ݂Ȃ��̂͂܂���.
	 */
	while( *p ){
		switch( *p ){
		case ' ':
		case '\t':
			*pp = p;
			return true;
		case '>':
			/* tag�I��. */
			*pp = p;
			return true;
		default:
			/* �l���\�����镶���̈�Ƃ݂Ȃ� */
			ZnkStr_add_c( ans, *p );
			break;
		}
		++p;
	}
	/* tag�I�����Ȃ�������NULL�I�[����. */
	*pp = p;
	return false;
}


/**
 * @brief
 *   varname=varval�Ƃ����`����attribute�w������߂���.
 *   varval���N�H�[�g�����ň͂܂�Ă���ꍇ�̓N�H�[�g�������܂߂Ď擾�����.
 *
 * @return:
 *   tag�I�������ꍇ�App �� tag�I������ '>' ���w����������Ԃ�true��Ԃ�.
 *   tag�I�������� NUL�I�[�����ꍇ false ��Ԃ�.
 */
static bool
scanAttribute( const char** pp, ZnkStr varname, ZnkVarpAry attr_ary )
{
	bool exist_assignment = false;
	const char* p = *pp;
	ZnkVarp attr = NULL;
	ZnkStr varval = NULL;
	char qch = 0;

	/* skip whitespace */
	while( *p && *p == ' ' ){ ++p; }

	if( *p == '>' ){
		/* tag�I�� */
		*pp = p;
		return true;
	}

	/* lexical-scan varname */
	ZnkStr_clear( varname );
	while( *p && *p != '=' && *p != ' ' && *p != '>' ){
		ZnkStr_add_c( varname, *p );
		++p;
	}

	/* skip whitespace or assignment */
	while( *p && *p == ' ' ){ ++p; }
	if( *p == '=' ){
		exist_assignment = true;
		++p;
	}
	while( *p && *p == ' ' ){ ++p; }

	if( *p == '\0' ){
		/* tag�I�����Ȃ��ɂ����Ȃ蕶���񂪏I�����Ă���. */
		*pp = p;
		return false;
	}

	if( !exist_assignment ){
		/* varname�Ƃ����`�Œl�Ȃ��ɃX�e�[�g�����g���I�����Ă���.
		 * ������_������ :
		 * script�^�O��async, option�^�O��selected, input�^�O��checked
		 * �Ȃǂł���͂��蓾��. */
		/* ZnkVarp�ł�Int�^�Ƃ��Ă����ێ�������̂Ƃ��A�w�肳��Ă���ꍇ���̒l��1�Ƃ��� */
		*pp = p;
		attr = ZnkVarp_create( "", "", 0, ZnkPrim_e_Int, NULL );
		ZnkStr_swap( attr->name_, varname );
		attr->prim_.u_.i32_ = 1;
		ZnkVarpAry_push_bk( attr_ary, attr );
		return true;
	} else {
		attr = ZnkVarp_create( "", "", 0, ZnkPrim_e_Str, NULL );
		ZnkStr_swap( attr->name_, varname );
		ZnkVarpAry_push_bk( attr_ary, attr );
	}

	/* �����l���葮�� : ���̒l�����. */
	/* �N�H�[�g������filename�Ɋi�[������̂Ƃ��� */
	varval = ZnkVar_str( attr );
	switch( *p ){
	case '\'':
	case '"':
		qch = *p;
		EstHtmlAttr_set_quote_char( attr, *p );
		++p;
		if( !scanQuote( &p, qch, varval, true ) ){
			/* unexpected null-terminated */
			return false;
		}
		ZnkStr_cut_back_c( varval ); /* �I���N�H�[�g�̂݃J�b�g */
		break;
	case '>':
		/* varname=�Ƃ����`�� = �����݂���ɂ�������炸�E�Ӓl�̎w�肪�Ȃ�tag�I�����Ă���. */
		/* ���̏ꍇ�A�ꉞ�󕶎��Ƃ݂Ȃ� */

		/* �X�y�[�X�̏ꍇ�A�N�H�[�e�B���O����Ă��Ȃ��l�̌`���ł��邱�Ƃ����� */
		EstHtmlAttr_set_quote_char( attr, ' ' );
		break;
	default:
		/* lexical-scan varval-end */

		/* �X�y�[�X�̏ꍇ�A�N�H�[�e�B���O����Ă��Ȃ��l�̌`���ł��邱�Ƃ����� */
		EstHtmlAttr_set_quote_char( attr, ' ' );

		if( !scanToDelimiter( &p, varval ) ){
			/* unexpected null-terminated */
			return false;
		}
		break;
	}
	*pp = p;
	return true;
}

static bool
scanComment( const char** pp, ZnkStr tagname, ZnkStr tagend, const char* begin, const char* end )
{
	const char* p = *pp;
	const size_t end_leng = Znk_strlen(end);
	/* comment out of HTML */
	ZnkStr_set( tagname, begin );
	p += Znk_strlen(begin);
	ZnkStr_clear( tagend );
	while( *p ){
		if( ZnkS_isBegin( p, end ) ){
			/* endof comment */
			ZnkStr_add( tagend, end );
			p += end_leng;
			*pp = p;
			return true;
		}
		ZnkStr_add_c( tagend, *p );
		++p;
	}
	*pp = p;
	return false;
}

static bool
scanTag( const char** pp, ZnkStr tagname, ZnkStr varname, ZnkVarpAry attr_ary, ZnkStr tagend )
{
	/* scan tag name */
	bool result = false;
	const char* p = *pp;

	switch( *p ){
	case '!':
		if( ZnkS_isBegin( p, "!--" ) ){
			/* comment out of HTML */
			result = scanComment( &p, tagname, tagend, "!--", "-->" );
		} else if( ZnkS_isBegin( p, "!DOCTYPE" ) ){
			result = scanComment( &p, tagname, tagend, "!DOCTYPE", ">" );
		}
		goto FUNC_END;
	case '?':
		/***
		 * �T�^�I�ɂ�<?php .... ?> <?xml ... ?> �Ȃǂ��l������.
		 */
		result = scanComment( &p, tagname, tagend, "?", "?>" );
		goto FUNC_END;
	case '/':
		/* �I��tag�ƍl������. */
		break;
	default:
		if( isalpha( (uint8_t)(*p) ) ){
			/* empty-tag�܂��͊J�ntag�ƍl������. */
		} else {
			/* ��L�ƃA���t�@�x�b�g�ȊO�̋L���������ꍇ��tag�ł͂Ȃ��Ƃ݂Ȃ��A�����𒆎~���ׂ� */
			result = false;
			goto FUNC_END;
		}
		break;
	}

	/***
	 * �I��tag(</h1>�Ȃ�)�̏ꍇ�Atagname�Ƃ���/�̕������܂߂Ď擾������̂Ƃ���.
	 * �܂�empty-tag�Ƃ�<br /> �̂悤�ȒP�ƂŊ������Ă���^�O�w��ł���.
	 * (���̊��ł͒P��<br>�ł����Ȃ����AXHTML�ł� <br />�̂悤�ɂ���̂������̂悤�ł���)
	 */
	ZnkStr_clear( tagname );
	ZnkStr_clear( tagend );
	if( *p == '/' ){
		ZnkStr_add_c( tagname, '/' );
		++p;
	}
	while( *p ){
		switch( *p ){
		case ' ': case '\t': case '\r': case '\n': case '>':
			goto EndOfTagname;
		default:
			break;
		}
		if( ZnkS_isBegin( p, "/>" ) ){ /* endof empty-tag */ 
			ZnkStr_add_c( tagend, '/' );
			++p;
			break;
		}
		if( isalnum( (uint8_t)(*p) ) ){
			ZnkStr_add_c( tagname, *p );
			++p;
		} else {
			/* tagname�ł͂Ȃ��ƍl������.
			 * ���̏ꍇ�������ɏ����𒆒f����. */
			result = false;
			goto FUNC_END;
		}
	}
EndOfTagname:
	if( ZnkStr_empty(tagname) || ZnkStr_eq(tagname, "/") ){
		/* tagname�ł͂Ȃ��ƍl������.
		 * ���̏ꍇ�������ɏ����𒆒f����. */
		result = false;
		goto FUNC_END;
	}
	while( *p == ' ' || *p == '\t' || *p == '\r' || *p == '\n' ){ ++p; }
	if( *p == '\0' ){
		/* unexpected null-terminated */
		result = false;
		goto FUNC_END;
	}

	/* attribute�w�肪���݂���tag�I�����Ă���ꍇ */
	if( *p == '>' ){
		ZnkStr_add_c( tagend, '>' );
		++p;
		result = true;
		goto FUNC_END;
	} else if( ZnkS_isBegin( p, "/>" ) ){
		ZnkStr_set( tagend, "/>" );
		p += 2;
		result = true;
		goto FUNC_END;
	}
	
	ZnkStr_clear( tagend );
	ZnkVarpAry_clear( attr_ary );
	while( *p && *p != '>' ){
		const char* q = p;
		if( *p == '<' ){
			/* tag�̒���tag������.
			 * �������������tag�ł͂Ȃ������Ɣ��f���ׂ��Œ��f */
			result = false;
			goto FUNC_END;
		}
		if( ZnkS_isBegin( p, "/>" ) ){ /* endof empty-tag */
			ZnkStr_add_c( tagend, '/' );
			++p;
			break;
		}
		if( !scanAttribute( &p, varname, attr_ary ) ){
			/* unexpected null-terminated */
			result = false;
			goto FUNC_END;
		}
		Znk_UNUSED( q );
		assert( q != p );
		while( *p && *p == ' ' ){ ++p; }
	}
	if( *p == '>' ){
		ZnkStr_add_c( tagend, '>' );
		++p;
	}
	result = true;
FUNC_END:

	*pp = p;
	return result;
}

static void
extendAttributeVarpAry( ZnkStr tmp, const ZnkVarpAry attr_ary )
{
	const size_t size = ZnkVarpAry_size( attr_ary );
	size_t       idx;
	char         qch = '\0';
	const char*  val = NULL;
	for( idx=0; idx<size; ++idx ){
		const ZnkVarp attr = ZnkVarpAry_at( attr_ary, idx );
		const bool  is_logical = EstHtmlAttr_isLogical( attr );
		const char* attr_name  = EstHtmlAttr_name(attr);
		ZnkStr_add( tmp, attr_name );
		if( !is_logical ){
			qch = EstHtmlAttr_quote_char( attr );
			ZnkStr_add_c( tmp, '=' );
			val = EstHtmlAttr_val(attr);
			if( qch == ' ' ){
				/* qch���X�y�[�X�̏ꍇ�A�N�H�[�e�B���O���s��Ȃ����Ƃ����� */
				ZnkStr_add( tmp, val );
			} else {
				/* qch���X�y�[�X�ȊO�̏ꍇ�A���̕����ŃN�H�[�e�B���O���s�����Ƃ����� */
				ZnkStr_add_c( tmp, qch );
				ZnkStr_add( tmp, val );
				ZnkStr_add_c( tmp, qch );
			}
		}
		if( idx != size-1 ){
			ZnkStr_add_c( tmp, ' ' );
		}
	}
}

static const char*
dumpAround( const char* p, char* buf, size_t max_size )
{
	ZnkS_copy( buf, max_size, p, Znk_NPOS );
	return buf;
}

static void
invokePlaneTxt( ZnkStr ans, ZnkStr planetxt, const EstParserPlaneTxtProcessFuncT planetxt_event_handler, void* planetxt_event_arg )
{
	if( ZnkStr_leng( planetxt ) ){
		if( planetxt_event_handler ){
			planetxt_event_handler( planetxt, planetxt_event_arg );
		}
		ZnkStr_add( ans, ZnkStr_cstr(planetxt) );
		ZnkStr_clear( planetxt );
	}
}
bool
EstParser_invokeHtmlTagEx( ZnkStr text,
		const EstParserProcessFuncT         tag_event_handler,      void* tag_event_arg,
		const EstParserPlaneTxtProcessFuncT planetxt_event_handler, void* planetxt_event_arg,
		ZnkStr msg )
{
	const char* p = ZnkStr_cstr(text);
	const char* q = NULL;
	char qch    = 0;
	bool result = true;
	char buf[ 32 ] = "";

	size_t line_count = 0;

	/***
	 * �X�e�[�g:quotable_region
	 * quotable_region�Ƃ�quote�ň͂܂ꂽ�̈悪�N�H�[�e�B���O�Ƃ��ĈӖ������̈�ł��邱�Ƃ�����.
	 * �T�^�I�ɂ�<script>...</script>�ň͂܂ꂽ�͈͂ł���.
	 * �t�Ɍ����΂���ȊO�̗̈�ł�scanQuote�Ƃ��Ă̈Ӗ��ł��̕�������荞��ł͂Ȃ炸�A
	 * �v���[���e�L�X�g�Ƃ��ĕ����ʂ�̈Ӗ��Ŏ擾���Ȃ���΂Ȃ�Ȃ�.
	 * �ʏ�AHTML�J�n���_�ł͂��̐�����Off�ł���.
	 */
	bool quotable_region = false;

	/***
	 * �X�e�[�g:script_region
	 * script_region�Ƃ�<script>...</script>�ň͂܂ꂽ�͈͂ł���.
	 * ���͈̔͂ł�<>�ň͂܂ꂽ�̈�͊�{�I��Tag�Ƃ��Ă̈Ӗ��������Ȃ�.
	 * �������Ȃ���script_region���I�[���邽�߂�tag�����͓��ʂɔF�����Ȃ���΂Ȃ�Ȃ�.
	 * �]���āA���̗̈���ł��Ƃ肠����scanTag�����͕K�v�ł���ƍl������.
	 * ������̏ꍇ�ɂ���ans�ɂ͌��X��tag�Ƃ��Ă̌`�󂪕����W�J�����.
	 * �B��̈Ⴂ�́Atag�Ƃ��Ă̈Ӗ������ꍇ�̂� tag_event_handler ���R�[�������K�v�����邱�Ƃł���.
	 * �����œ��ɒ��ӂ��Ȃ���΂Ȃ�Ȃ��̂͂��̗̈���J�n/�I�����邽�߂̃^�O���̂�tag_event_handler���R�[�������K�v�����邱�Ƃł���.
	 * �ʏ�AHTML�J�n���_�ł͂��̐�����Off�ł���.
	 */
	bool script_region = false;

	/***
	 * �X�e�[�g:ahref_region
	 * ahref_region�Ƃ�<a href=...>...</a>�ň͂܂ꂽ�͈͂ł���.
	 * ���͈̔͂ł͂���Ȃ郊���N���܂߂�ׂ��ł͂Ȃ��A�]���ăI�[�g�����N�����ɂ����Ă͏��O�ΏۂƂ��ׂ��ł���.
	 * �ʏ�AHTML�J�n���_�ł͂��̐�����Off�ł���.
	 */
	bool ahref_region = false;

	/***
	 * tag_event_handler ���R�[������Ȃ���΂Ȃ�Ȃ���Ԃł��邱�Ƃ�����.
	 * �ʏ�AHTML�J�n���_�ł͂��̐�����Off�ł���.
	 */
	bool call_tag_event_handler = false;

	/***
	 * planetxt �Ƃ� quotable_region, script_region, ahref_region �̂ǂ�ɂ��Y�����Ȃ��͈͂ł���.
	 * �ʏ�AHTML�J�n���_�ł͂��̐�����On�ł���.
	 */

	ZnkStr ans     = ZnkStr_new( "" );
	ZnkStr tagend  = ZnkStr_new( "" );
	ZnkStr tagname = ZnkStr_new( "" );
	ZnkStr varname = ZnkStr_new( "" );
	ZnkVarpAry varp_ary = ZnkVarpAry_create( true );
	ZnkStr planetxt = ZnkStr_new( "" );

	while( *p ){
		switch( *p ){
		case '\'':
		case '"':
			q = p;
			qch = *p;
			++p;
			if( quotable_region ){
				/* ���O�Ŏ擾����planetxt�����܂��Ă���Ȃ�Ώ������� */
				invokePlaneTxt( ans, planetxt, planetxt_event_handler, planetxt_event_arg );

				/***
				 * �����ł͒P�ɃN�H�[�g�ň͂܂ꂽ�̈�����̂܂�ans�֓]�ʂ���.
				 * ����Aplanetxt �ɂ͓]�ʂ��Ȃ�.
				 */
				ZnkStr_add_c( ans, *q );
				if( !scanQuote( &p, qch, ans, true ) ){
					/* unexpected null-terminated */
					if( msg ){
						ZnkStr_addf( msg, "EstParser_invokeHtmlTag : Error : scanQuote : line=[%zu] around [%s]\n",
								line_count, dumpAround( q, buf, 8 ) );
					}
					result = false;
					goto FUNC_END;
				}

			} else if( ahref_region ){
				/* ���O�Ŏ擾����planetxt�����܂��Ă���Ȃ�Ώ������� */
				invokePlaneTxt( ans, planetxt, planetxt_event_handler, planetxt_event_arg );

				/***
				 * �����ł͒P��<a href...>...</a>�ň͂܂ꂽ�̈�����̂܂�ans�֓]�ʂ���.
				 * ����Aplanetxt �ɂ͓]�ʂ��Ȃ�.
				 */
				ZnkStr_add_c( ans, *q );

			} else {
				/***
				 * planetxt �ɓ]��.
				 * ����Aans �ɂ͓]�ʂ��Ȃ�.
				 */
				ZnkStr_add_c( planetxt, *q );
			}
			break;
		case '<':
			/* begin of tag scan */
			q = p;
			++p;

			ZnkStr_clear( tagname );
			ZnkVarpAry_clear( varp_ary );
			ZnkStr_clear( tagend );

			if( !scanTag( &p, tagname, varname, varp_ary, tagend ) ){
				/* tag�̊J�n�ł͂Ȃ��Ɣ��f���ꂽ�ꍇ */
				/* p�̈ʒu�� < �̎��̈ʒu�֋����I��Rollback */
				if( msg ){
					ZnkStr_addf( msg, "EstParser_invokeHtmlTag : Error : scanTag : line=[%zu] around [%s]\n",
							line_count, dumpAround( q, buf, 8 ) );
				}
				ZnkStr_add_c( planetxt, *q );
				p = q+1; /* Rollback */
				break;
			} else {
				/* tag�̊J�n�Ɣ��f���ꂽ�ꍇ.
				 * �������X��script_region�ł��邩�ۂ��𒲂ׁA���̓����ł������ꍇ��
				 * tag�̊J�n�Ƃ������f���p������.
				 */
				if( script_region ){
					if( ZnkS_eqCase(ZnkStr_cstr(tagname), "/script") ){
						/* script�^�O�̏I�� */
						/* ���O�Ŏ擾����planetxt����U��������(�{����scripttxt�Ƃł����ׂ���) */
						invokePlaneTxt( ans, planetxt, planetxt_event_handler, planetxt_event_arg );
						ZnkStr_add_c( ans, *q );
					} else {
						/* tag�J�n�̔��f���p�� */
						ZnkStr_add_c( planetxt, *q );
						p = q+1; /* Rollback */
						break;
					}
				} else {
					/* ���O�Ŏ擾����planetxt����U�������� */
					invokePlaneTxt( ans, planetxt, planetxt_event_handler, planetxt_event_arg );
					ZnkStr_add_c( ans, *q );
				}
			}

			/***
			 * ����^�O�̉�͂ƃX�e�[�g�̕ύX.
			 */
			call_tag_event_handler = true;
			if( ZnkS_eqCase(ZnkStr_cstr(tagname), "script") ){
				/***
				 * script�^�O�̊J�n:
				 * script_region���ɂ͊�{�I�ɔC�ӂ̃p�^�[�����o��������Ƃ݂Ȃ��ׂ��ł���.
				 * �����ł̓X�e�[�g�J�ڃ}�V���Ƃ��Ă������������.
				 *
				 * script_region�̊J�n�ƏI�����Ӗ�����^�O���̂�tag_event_handler��
				 * ���s����Ȃ���΂Ȃ�Ȃ�.
				 */
				if( script_region ){
					/***
					 * ����script_region���Ȃ�΁A"<script" ��tag�Ƃ݂Ȃ��ׂ��ł͂Ȃ��A
					 * �]����call_tag_event_handler���ׂ��ł͂Ȃ�.
					 */
					call_tag_event_handler = false;
				} else {
					quotable_region = true;
					script_region = true;
				}

			} else if( ZnkS_eqCase(ZnkStr_cstr(tagname), "/script") ){
				/* script�^�O�̏I�� */
				quotable_region = false;
				script_region = false;

			} else if( ZnkS_eqCase(ZnkStr_cstr(tagname), "a") ){
				/***
				 * a�^�O�̊J�n:
				 * ahref_region���ɂ͊�{�I�ɔC�ӂ̃p�^�[�����o��������Ƃ݂Ȃ��ׂ��ł���.
				 * �����ł̓X�e�[�g�J�ڃ}�V���Ƃ��Ă������������.
				 */
				ZnkVarp href = ZnkVarpAry_find_byName_literal( varp_ary, "href", true );
				if( href ){
					ahref_region = true;
				} else {
					/* href �����w�肪���݂��Ȃ��ꍇ��ahref_region�Ƃ݂͂Ȃ��Ȃ� */
				}
			} else if( ZnkS_eqCase(ZnkStr_cstr(tagname), "/a") ){
				/***
				 * a�^�O�̏I��:
				 * �Ƃ肠���������_�ł́A�����炭�����̃u���E�U�ł��������Ă���ʂ�A
				 * �l�X�g����Ă���ُ�ȃP�[�X�ł����Ă� /a �̎��_��ahref_region�͋����I��������̂Ƃ���.
				 */
				ahref_region = false;
			}

			/***
			 * call_tag_event_handler:
			 */
			if( call_tag_event_handler ){
				/* invoke event callback. */
				tag_event_handler( tagname, varp_ary, tag_event_arg, tagend );
			}

			ZnkStr_add( ans, ZnkStr_cstr(tagname) );
			if( ZnkVarpAry_size( varp_ary ) ){
				ZnkStr_add_c( ans, ' ' );
				extendAttributeVarpAry( ans, varp_ary );
			}
			ZnkStr_add( ans, ZnkStr_cstr(tagend) );
			break;
		case '\n':
			if( ahref_region ){
				ZnkStr_add_c( ans, *p );
			} else {
				ZnkStr_add_c( planetxt, *p );
			}
			++p;
			++line_count;
			break;
		default:
			if( ahref_region ){
				ZnkStr_add_c( ans, *p );
			} else {
				ZnkStr_add_c( planetxt, *p );
			}
			++p;
			break;
		}
	}
	/* ���O�Ŏ擾����planetxt�����܂��Ă���Ȃ�Ώ������� */
	invokePlaneTxt( ans, planetxt, planetxt_event_handler, planetxt_event_arg );

	result = true;
	ZnkStr_swap( text, ans );
FUNC_END:
	ZnkStr_delete( ans );
	ZnkStr_delete( tagend );
	ZnkStr_delete( tagname );
	ZnkStr_delete( varname );
	ZnkVarpAry_destroy( varp_ary );
	ZnkStr_delete( planetxt );
	return result;
}

static int
filterTest( ZnkStr tagname, ZnkVarpAry varp_ary, void* arg, ZnkStr tagend )
{
	if( ZnkS_eqCase( ZnkStr_cstr(tagname), "a" ) ){
		ZnkVarp varp = ZnkVarpAry_find_byName_literal( varp_ary, "href", true );
		if( varp ){
			ZnkStr str = ZnkVar_str( varp );
			Znk_printf_e( "a: tagend=[%s], href=[%s]\n", ZnkStr_cstr(tagend), ZnkStr_cstr(str) );
		}
	}
	return 0;
}

void
EstParser_test( const char* r_filename, const char* w_filename )
{
	ZnkFile r_fp = NULL;
	ZnkFile w_fp = NULL;
	ZnkStr text = ZnkStr_new( "" );

	r_fp = Znk_fopen( r_filename, "rb" );
	if( r_fp ){
		ZnkStr tmp = ZnkStr_new( "" );

		while( ZnkStrFIO_fgets( tmp, 0, 4096, r_fp ) ){
			ZnkStr_add( text, ZnkStr_cstr(tmp) );
		}
		ZnkStr_delete( tmp );

		EstParser_invokeHtmlTagEx( text, filterTest, NULL, NULL, NULL, NULL );
		Znk_fclose( r_fp );
	}

	w_fp = Znk_fopen( w_filename, "wb" );
	if( w_fp ){
		Znk_fwrite( (uint8_t*)ZnkStr_cstr(text), ZnkStr_leng(text), w_fp );
		Znk_fclose( w_fp );
	}
	ZnkStr_delete( text );
}

