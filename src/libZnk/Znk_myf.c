#include <Znk_myf.h>
#include <Znk_str.h>
#include <Znk_str_fio.h>
#include <Znk_str_ary.h>
#include <Znk_stdc.h>
#include <Znk_s_base.h>
#include <Znk_varp_ary.h>
#include <Znk_primp_ary.h>
#include <string.h>

struct ZnkMyfSectionImpl {
	ZnkStr            name_;
	ZnkMyfSectionType type_;
	union Data_tag {
		ZnkStrAry   lines_;
		ZnkVarpAry  vars_;
		ZnkPrimpAry prims_;
	} u_;
};

ZnkStrAry
ZnkMyfSection_lines( const ZnkMyfSection sec )
{
	assert( sec->type_ == ZnkMyfSection_e_Lines );
	return sec->u_.lines_;
}
ZnkVarpAry
ZnkMyfSection_vars( const ZnkMyfSection sec )
{
	assert( sec->type_ == ZnkMyfSection_e_Vars );
	return sec->u_.vars_;
}
ZnkPrimpAry
ZnkMyfSection_prims( const ZnkMyfSection sec )
{
	assert( sec->type_ == ZnkMyfSection_e_Prims );
	return sec->u_.prims_;
}
const char*
ZnkMyfSection_name( const ZnkMyfSection sec )
{
	return ZnkStr_cstr( sec->name_ );
}
ZnkMyfSectionType
ZnkMyfSection_type( const ZnkMyfSection sec )
{
	return sec->type_;
}

static void
makeUnionData( union Data_tag* u, ZnkMyfSectionType sec_type )
{
	switch( sec_type ){
	case ZnkMyfSection_e_Lines:
	case ZnkMyfSection_e_OutOfSection:
		u->lines_ = ZnkStrAry_create( true );
		break;
	case ZnkMyfSection_e_Vars:
		u->vars_ = ZnkVarpAry_create( true );
		break;
	case ZnkMyfSection_e_Prims:
		u->prims_ = ZnkPrimpAry_create( true );
		break;
	default:
		break;
	}
}
static void
deleteUnionData( union Data_tag* u, ZnkMyfSectionType sec_type )
{
	switch( sec_type ){
	case ZnkMyfSection_e_Lines:
	case ZnkMyfSection_e_OutOfSection:
		ZnkStrAry_destroy( u->lines_ );
		break;
	case ZnkMyfSection_e_Vars:
		ZnkVarpAry_destroy( u->vars_ );
		break;
	case ZnkMyfSection_e_Prims:
		ZnkPrimpAry_destroy( u->prims_ );
		break;
	default:
		break;
	}
}

static ZnkMyfSection
makeMyfSection( const char* sec_name, ZnkMyfSectionType sec_type )
{
	ZnkMyfSection sec = (ZnkMyfSection)Znk_alloc0( sizeof( struct ZnkMyfSectionImpl ) );
	sec->name_ = ZnkStr_new( sec_name );
	sec->type_ = sec_type;
	makeUnionData( &sec->u_, sec_type );
	return sec;
}
static void
deleteMyfSection( void* elem )
{
	ZnkMyfSection sec = (ZnkMyfSection)elem;
	if( sec ){
		ZnkStr_delete( sec->name_ );
		deleteUnionData( &sec->u_, sec->type_ );
		sec->type_ = ZnkMyfSection_e_None;
		Znk_free( sec );
	}
}

struct ZnkMyfImpl {
	char       quote_begin_[ 32 ];
	char       quote_end_[ 32 ];
	char       nl_[ 32 ];
	/***
	 * MyfSection�i�[����ю����Ǘ��p
	 */
	ZnkObjAry sec_ary_;
	/***
	 * OutOfSection�i�[����ю����Ǘ��p.
	 * OutOfSection�������I�ɂ�ZnkMyfSection�Ƃ��Ĉ���.
	 */
	ZnkObjAry oos_ary_;
	/***
	 * �����ł̏����L�^��ZnkMyfSection�̎Q�Ƃ̔z��Ƃ��āA
	 * MyfSection�����OutOfSection�̋L�ڂ���Ă��鏇����ۑ�����.
	 * �����load => save�̍ۂɂ����̌��̍\���Ə��Ԃ������Ȃ��悤�ɂ��邽�߂ł���.
	 * ���̔z��ł͎Q�Ƃ݂̂�ێ����A�e�v�f�̎����͊Ǘ����Ȃ�.
	 */
	ZnkObjAry order_;
};

ZnkMyf
ZnkMyf_create( void )
{
	ZnkMyf myf = Znk_alloc0( sizeof( struct ZnkMyfImpl ) );
	myf->quote_begin_[ 0 ] = '\0';
	myf->quote_end_[ 0 ]   = '\0';
	ZnkS_copy_literal( myf->nl_, sizeof(myf->nl_), "\n" );
	myf->sec_ary_ = ZnkObjAry_create( deleteMyfSection );
	myf->oos_ary_ = ZnkObjAry_create( deleteMyfSection );
	/***
	 * order_�ł�ZnkMyfSection�̎Q�Ƃ݂̂��Ǘ�(�����͊Ǘ����Ȃ�).
	 */
	myf->order_   = ZnkObjAry_create( NULL );
	return myf;
}

void
ZnkMyf_destroy( ZnkMyf myf )
{
	if( myf ){
		ZnkObjAry_destroy( myf->sec_ary_ );
		ZnkObjAry_destroy( myf->oos_ary_ );
		ZnkObjAry_destroy( myf->order_ );
		Znk_free( myf );
	}
}
void
ZnkMyf_clear( ZnkMyf myf )
{
	if( myf ){
		ZnkObjAry_clear( myf->sec_ary_ );
		ZnkObjAry_clear( myf->oos_ary_ );
		ZnkObjAry_clear( myf->order_ );
	}
}

const char*
ZnkMyf_quote_begin( const ZnkMyf myf )
{
	return myf->quote_begin_;
}
const char*
ZnkMyf_quote_end( const ZnkMyf myf )
{
	return myf->quote_end_;
}
void
ZnkMyf_set_quote( ZnkMyf myf, const char* quote_begin, const char* quote_end )
{
	ZnkS_copy( myf->quote_begin_, sizeof(myf->quote_begin_), quote_begin, Znk_NPOS );
	ZnkS_copy( myf->quote_end_,   sizeof(myf->quote_end_),   quote_end,   Znk_NPOS );
}

ZnkMyfSection
ZnkMyf_addSection( ZnkMyf myf, const char* sec_name, ZnkMyfSectionType sec_type )
{
	ZnkMyfSection sec = NULL;
	switch( sec_type ){
	case ZnkMyfSection_e_Lines:
	case ZnkMyfSection_e_Vars:
	case ZnkMyfSection_e_Prims:
		/***
		 * ���g����̐V�KSection��o�^����.
		 */
		sec = makeMyfSection( sec_name, sec_type );
		ZnkObjAry_push_bk( myf->sec_ary_, (ZnkObj)sec );
		ZnkObjAry_push_bk( myf->order_, (ZnkObj)sec );
		break;
	default:
		break;
	}
	return sec;
}

ZnkMyfSection
ZnkMyf_registSection( ZnkMyf myf, const char* sec_name, ZnkMyfSectionType sec_type )
{
	size_t size = ZnkObjAry_size( myf->sec_ary_ );
	size_t idx;
	ZnkMyfSection sec = NULL;
	for( idx=0; idx<size; ++idx ){
		sec = (ZnkMyfSection)ZnkObjAry_at( myf->sec_ary_, idx );
		if( ZnkStr_eq( sec->name_, sec_name ) ){
			/* already exist */
			switch( sec_type ){
			case ZnkMyfSection_e_Lines:
			case ZnkMyfSection_e_Vars:
			case ZnkMyfSection_e_Prims:
				if( sec->type_ != sec_type ){
					/* remaking */
					deleteUnionData( &sec->u_, sec->type_ );
					sec->type_ = sec_type;
					makeUnionData( &sec->u_, sec->type_ );
				}
				break;
			case ZnkMyfSection_e_None:
				/* deleting */
				deleteUnionData( &sec->u_, sec->type_ );
				sec->type_ = sec_type;
				return NULL;
			default:
				assert( 0 );
				break;
			}
			return sec;
		}
	}
	sec = ZnkMyf_addSection( myf, sec_name, sec_type );
	return sec;
}
ZnkMyfSection
ZnkMyf_internSection( ZnkMyf myf, const char* sec_name, ZnkMyfSectionType sec_type )
{
	size_t size = ZnkObjAry_size( myf->sec_ary_ );
	size_t idx;
	ZnkMyfSection sec = NULL;
	for( idx=0; idx<size; ++idx ){
		sec = (ZnkMyfSection)ZnkObjAry_at( myf->sec_ary_, idx );
		if( ZnkStr_eq( sec->name_, sec_name ) ){
			/* already exist */
			return sec;
		}
	}
	sec = ZnkMyf_addSection( myf, sec_name, sec_type );
	return sec;
}

size_t
ZnkMyf_numOfSection( const ZnkMyf myf )
{
	return ZnkObjAry_size( myf->sec_ary_ );
}
ZnkMyfSection
ZnkMyf_atSection( const ZnkMyf myf, size_t sec_idx )
{
	return (ZnkMyfSection)ZnkObjAry_at( myf->sec_ary_, sec_idx );
}
size_t
ZnkMyf_findIdx_withSecType( const ZnkMyf myf, const char* sec_name, ZnkMyfSectionType sec_type )
{
	const size_t sec_size = ZnkObjAry_size( myf->sec_ary_ );
	size_t sec_idx;
	ZnkMyfSection sec;
	for( sec_idx=0; sec_idx<sec_size; ++sec_idx ){
		sec = (ZnkMyfSection)ZnkObjAry_at( myf->sec_ary_, sec_idx );
		if( sec->type_ == sec_type && ZnkStr_eq( sec->name_, sec_name ) ){
			/* found */
			return sec_idx;
		}
	}
	return Znk_NPOS;
}
size_t
ZnkMyf_findIdx( const ZnkMyf myf, const char* sec_name )
{
	const size_t sec_size = ZnkObjAry_size( myf->sec_ary_ );
	size_t sec_idx;
	ZnkMyfSection sec;
	for( sec_idx=0; sec_idx<sec_size; ++sec_idx ){
		sec = (ZnkMyfSection)ZnkObjAry_at( myf->sec_ary_, sec_idx );
		if( ZnkStr_eq( sec->name_, sec_name ) ){
			/* found */
			return sec_idx;
		}
	}
	return Znk_NPOS;
}
bool
ZnkMyf_erase_byIdx( ZnkMyf myf, size_t sec_idx )
{
	return ZnkObjAry_erase_byIdx( myf->sec_ary_, sec_idx );
}

static bool
parseDefQuote( ZnkMyf myf, const char* p, size_t p_leng, size_t count, const char* defblk_name )
{
	size_t pos = 0;
	size_t arg_leng = 0;
	const char* end = p + p_leng;

	/* 1th arg */
	p += pos;
	pos = ZnkS_lfind_arg( p, 0, end-p, 0, &arg_leng, " \t", 2 );
	if( arg_leng != strlen(defblk_name) ){
		/* defblk_name�Ŏn�܂镶���ł��邪�A���ꂻ�̂��̂ł͂Ȃ� */
		goto FUNC_ERROR;
	}
	if( pos == Znk_NPOS ){ goto FUNC_ERROR; }

	/* 2th arg */
	p += pos + arg_leng;
	pos = ZnkS_lfind_arg( p, 0, end-p, 0, &arg_leng, " \t", 2 );
	if( pos == Znk_NPOS ){ goto FUNC_ERROR; }
	ZnkS_copy( myf->quote_begin_, sizeof(myf->quote_begin_), p+pos, arg_leng );

	/* 3th arg */
	p += pos + arg_leng;
	pos = ZnkS_lfind_arg( p, 0, end-p, 0, &arg_leng, " \t", 2 );
	if( pos == Znk_NPOS ){ goto FUNC_ERROR; }
	ZnkS_copy( myf->quote_end_, sizeof(myf->quote_end_), p+pos, arg_leng );

	return true;

FUNC_ERROR:
	Znk_printf_e( "Znk_myf load : Syntax Error : line=[%zu]\n", count );
	return false;
}


/***
 * @return
 *  1: quoted_one�������Ɏ擾.
 *  0: VarDirective�I�[('}')�����o.
 * -1: Error����.
 */
static bool
gainQuotedOneElem( ZnkStr quoted_one, const char* sec_name, ZnkStr line, ZnkFile fp, size_t* count, ZnkStr lhs_tmp,
		const char* quote_begin, const char* quote_end, size_t quote_begin_leng, size_t quote_end_leng )
{
	/***
	 * lhs_tmp�̊J�n�ʒu��quoted����Ă��Ȃ���ԓ��ł��邱�Ƃ�O��Ƃ���.
	 */
	const char* mistake_p;
	const char* breath_end;

	while( !ZnkStr_isBegin( lhs_tmp, quote_begin ) ){
		const char* begin = strstr( ZnkStr_cstr(lhs_tmp), quote_begin );

		if( begin ){
			mistake_p = strchr( ZnkStr_cstr(lhs_tmp), '=' );
			if( mistake_p && mistake_p < begin ){
				goto MistakeDetection;
			}
			/* lhs_tmp�ɂ�����0�ʒu������quote_begin�J�n�ʒu�ɂȂ�悤�ɍ��킹�Ă��� */
			ZnkStr_cut_front( lhs_tmp, begin-ZnkStr_cstr(lhs_tmp) );
			break;
		}

		breath_end = strchr( ZnkStr_cstr(lhs_tmp), '}' );
		if( breath_end ){
	 		/* VarDirective�I�[�����o */
			return 0;
		}

		/***
		 * lhs_tmp���ɂ�quote_begin�����݂��Ȃ�.
		 * ����ɂ��ꂪ�܂�VarDirective�I�[�łȂ���΂�����[����K�v������.
		 */
		if( !ZnkStrFIO_fgets( line, 0, 4096, fp ) ){
			/* Unexpected eof */
			Znk_printf_e( "ZnkMyf_load : Unexpected EOF in SectionVars %s : line:%zu quote_begin is not found.\n",
					sec_name, *count );
			return -1;
		}
		++*count;
		ZnkStr_add( lhs_tmp, ZnkStr_cstr(line) );
	}

	/***
	 * �ȍ~lhs_tmp��quote_begin�Ŏn�܂�.
	 */
	while( true ){
		const char* begin = ZnkStr_cstr( lhs_tmp ) + quote_begin_leng;
		const char* end   = strstr( begin, quote_end );

		if( end ){
			const char* next;
			/***
			 * quote_end�����o.
			 * ���݃X�g�b�N����Ă���lhs_tmp���Řd����ꍇ.
			 */
			ZnkStr_assign( quoted_one, 0, begin, end-begin );
			next = end + quote_end_leng;
			begin = strstr( next, quote_begin );
			mistake_p  = strchr( next, '=' );

			if( begin ){
				if( mistake_p && mistake_p < begin ){
					goto MistakeDetection;
				}
				/* begin�ȑO�܂ł̏����ςݕ������J�b�g.
				 * lhs_tmp�ɂ�����0�ʒu������quote_begin�J�n�ʒu�ɂȂ�悤�ɍ��킹�Ă��� */
				ZnkStr_cut_front( lhs_tmp, begin-ZnkStr_cstr(lhs_tmp) );
			} else {
				if( mistake_p ){
					goto MistakeDetection;
				}
				/* ����quote_begin��������Ȃ������ꍇ. */
				/* next�ȑO�܂ł̏����ςݕ������J�b�g. */
				ZnkStr_cut_front( lhs_tmp, next-ZnkStr_cstr(lhs_tmp) );
			}
			break;
		}
		/***
		 * lhs_tmp���ɂ�quote_end�����݂��Ȃ�.
		 * quote_end�����o�����܂ŁA������[����K�v������.
		 */
		if( !ZnkStrFIO_fgets( line, 0, 4096, fp ) ){
			/* Unexpected eof */
			Znk_printf_e( "ZnkMyf_load : Unexpected EOF in SectionVars %s : line:%zu quote_end is not found.\n",
					sec_name, *count );
			return -1;
		}
		++*count;
		ZnkStr_add( lhs_tmp, ZnkStr_cstr(line) );
	}
	return 1;

MistakeDetection:
	/***
	 * quoted����Ă��Ȃ���ԂɁA�s����'='�L�������o���ꂽ.
	 * '}'�L���Ȃǂ̏����Y��Ŏ���VarDirective���Q�Ƃ��Ă���\��������.
	 */
	/* Unexpected '=' */
	Znk_printf_e( "ZnkMyf_load : Unexpected '=' in SectionVars %s : line:%zu End of this VarDirective may be broken.\n",
			sec_name, *count );
	Znk_printf_e( "            : lhs_tmp=[%s]\n", ZnkStr_cstr(lhs_tmp) );
	return -1;
}
static bool
parseVarDirective( ZnkVarpAry vars, const char* sec_name, ZnkStr line,
		ZnkFile fp, const char* nl, size_t* count, ZnkStr lhs_tmp,
		const char* quote_begin, const char* quote_end, size_t quote_begin_leng, size_t quote_end_leng )
{
	size_t key_begin; size_t key_end;
	size_t val_begin; size_t val_end;
	ZnkVarp varp = NULL;
	const char* p      = ZnkStr_cstr(line);
	size_t      p_leng = ZnkStr_leng(line);
	int  state = -1;

	ZnkS_find_key_and_val( p, 0, p_leng,
			&key_begin, &key_end,
			&val_begin, &val_end,
			"=", 1,
			" \t", 2 );
	if( key_begin == Znk_NPOS || key_end == Znk_NPOS || val_begin == Znk_NPOS ){
		Znk_printf_e( "ZnkMyf_load : Syntax Error in SectionVars %s : line:%zu : '=' is not found.\n",
				sec_name, *count );
		return false;
	}

	/***
	 * �܂�name��""�Ƃ��Ēl�����o�^���A�������name��ݒ�.
	 */
	varp = ZnkVarp_create( "", "", 0, ZnkPrim_e_None, NULL );
	//ZnkVar_set_val_Str( varp, p+val_begin, val_end-val_begin );
	ZnkVarpAry_push_bk( vars, varp );
	ZnkStr_assign( varp->name_, 0, p+key_begin, key_end-key_begin );


	ZnkStr_assign( lhs_tmp, 0, p+val_begin, val_end-val_begin );
	if( ZnkStr_isBegin( lhs_tmp, "{" ) ){
		/* �x�N�g���^ */
		ZnkStr quoted_one = ZnkStr_new( "" );
		ZnkStrAry sda;
		ZnkPrim_compose( &varp->prim_, ZnkPrim_e_StrAry, NULL );
		sda = ZnkVar_str_ary( varp );

		while( true ){
			state = gainQuotedOneElem( quoted_one, sec_name, line, fp, count, lhs_tmp,
					quote_begin, quote_end, quote_begin_leng, quote_end_leng );
			if( state != 1 ){
				break;
			}
			ZnkStrAry_push_bk_cstr( sda, ZnkStr_cstr(quoted_one), Znk_NPOS );
		}
		ZnkStr_delete( quoted_one );
		if( state == -1 ){
			return false;
		}
	} else {
		/* �X�J���[�^ */
		ZnkStr str;
		ZnkPrim_compose( &varp->prim_, ZnkPrim_e_Str, NULL );
		str = ZnkVar_str( varp );
		state = gainQuotedOneElem( str, sec_name, line, fp, count, lhs_tmp,
				quote_begin, quote_end, quote_begin_leng, quote_end_leng );
		if( state != 1 ){
			return false;
		}
	}
	return true;
}
static bool
parsePrimDirective( ZnkPrimpAry prims, const char* sec_name, ZnkStr line,
		ZnkFile fp, const char* nl, size_t* count, ZnkStr lhs_tmp,
		const char* quote_begin, const char* quote_end, size_t quote_begin_leng, size_t quote_end_leng )
{
	size_t val_begin; size_t val_end;
	ZnkPrimp prim = NULL;
	const char* p      = ZnkStr_cstr(line);
	//size_t      p_leng = ZnkStr_leng(line);
	int  state = -1;

	ZnkS_find_side_skip( ZnkStr_cstr(line), 0, Znk_NPOS,
			&val_begin, &val_end,
			" \t", 2 );

	prim = ZnkPrimp_create( ZnkPrim_e_None, NULL );
	ZnkPrimpAry_push_bk( prims, prim );

	ZnkStr_assign( lhs_tmp, 0, p+val_begin, val_end-val_begin );
	if( ZnkStr_isBegin( lhs_tmp, "{" ) ){
		/* �x�N�g���^ */
		ZnkStr quoted_one = ZnkStr_new( "" );
		ZnkStrAry sda;
		ZnkPrim_compose( prim, ZnkPrim_e_StrAry, NULL );
		sda = ZnkPrim_strAry( prim );

		while( true ){
			state = gainQuotedOneElem( quoted_one, sec_name, line, fp, count, lhs_tmp,
					quote_begin, quote_end, quote_begin_leng, quote_end_leng );
			if( state != 1 ){
				break;
			}
			ZnkStrAry_push_bk_cstr( sda, ZnkStr_cstr(quoted_one), Znk_NPOS );
		}
		ZnkStr_delete( quoted_one );
		if( state == -1 ){
			return false;
		}
	} else {
		/* �X�J���[�^ */
		ZnkStr str;
		ZnkPrim_compose( prim, ZnkPrim_e_Str, NULL );
		str = ZnkPrim_str( prim );
		state = gainQuotedOneElem( str, sec_name, line, fp, count, lhs_tmp,
				quote_begin, quote_end, quote_begin_leng, quote_end_leng );
		if( state != 1 ){
			return false;
		}
	}
	return true;
}

static bool
parseSectionLines( ZnkMyf myf, const char* p, size_t p_leng,
		ZnkFile fp, size_t* count, ZnkStr line )
{
	size_t pos = 0;
	size_t arg_leng = 0;
	ZnkMyfSection sec;

	/***
	 * sec_ary_�����order_�ւ̓o�^.
	 */
	sec = makeMyfSection( "", ZnkMyfSection_e_Lines );
	ZnkObjAry_push_bk( myf->sec_ary_, (ZnkObj)sec );
	ZnkObjAry_push_bk( myf->order_, (ZnkObj)sec );

	/* 1th arg */
	pos = ZnkS_lfind_arg( p+pos, 0, p_leng-pos, 0, &arg_leng, " \t", 2 ) + pos;
	ZnkStr_assign( sec->name_, 0, p+pos, arg_leng );

	while( true ){
		if( !ZnkStrFIO_fgets( line, 0, 4096, fp ) ){
			/* Unexpected eof */
			Znk_printf_e( "ZnkMyf_load : Unexpected EOF in SectionLines %s : line:%zu\n",
					ZnkStr_cstr(sec->name_), *count );
			return false;
		}
		++*count;
		ZnkStr_chompNL( line );

#if 0
		if( ZnkStr_empty( line ) ){
			continue;
		}
#endif
		if( ZnkStr_first( line ) == '#' ){
			/* treat as comment out */
			continue;
		}
		if( ZnkStr_eq( line, "@@." ) ){
			/* endof this Section */
			break;
		}
		if( ZnkStr_eq( line, "@@^" ) ){
			/* Special empty */
			ZnkStr_cut_front( line, 3 );
			break;
		}
		ZnkStrAry_push_bk_cstr( sec->u_.lines_, ZnkStr_cstr(line), Znk_NPOS );
	}
	return true;
}

/***
 * ���̃X�J���[�^�\�L��F�߂�.
 *
 * key = ['abc']
 *
 * key = ['abc
 * defg
 * hij']
 *
 * ���̃x�N�g���^�\�L��F�߂�.
 *
 * key = { ['abc'] ['def'] }
 *
 * key = {
 * ['abc']
 * ['def']
 * }
 *
 * key = {
 * ['abc']
 * ['def
 * ghij']
 * ['lmn
 * opq']
 * }
 *
 * key = { ['abc'] ['def
 * ghij'] ['lmn
 * opq'] }
 *
 * �X�J���[�^���x�N�g���^��v���ɔ��f�\�Ƃ��邽�߁A= �̉E���ɂ͕K����1�Ԗڂ� [' �܂��� { ��
 * ���݂��Ȃ���΂Ȃ�Ȃ��d�l�Ƃ���. �܂�ȉ��̂悤�ȕ\�L��NG�ł���.
 * key =
 *  ['abc']
 *
 * key =
 * { ['abc'] ['def'] }
 *
 */
static bool
parseSectionVars( ZnkMyf myf, const char* p, size_t p_leng,
		ZnkFile fp, size_t* count, ZnkStr line )
{
	size_t pos = 0;
	size_t arg_leng = 0;
	const char*  quote_begin = myf->quote_begin_;
	const char*  quote_end   = myf->quote_end_;
	const size_t quote_begin_leng = strlen( quote_begin );
	const size_t quote_end_leng   = strlen( quote_end );
	const char* nl = myf->nl_;
	ZnkMyfSection sec;
	ZnkStr lhs_tmp = ZnkStr_new( "" );
	bool result = false;

	/***
	 * sec_ary_�����order_�ւ̓o�^.
	 */
	sec = makeMyfSection( "", ZnkMyfSection_e_Vars );
	ZnkObjAry_push_bk( myf->sec_ary_, (ZnkObj)sec );
	ZnkObjAry_push_bk( myf->order_, (ZnkObj)sec );

	/* 1th arg */
	pos = ZnkS_lfind_arg( p+pos, 0, p_leng-pos, 0, &arg_leng, " \t", 2 ) + pos;
	ZnkStr_assign( sec->name_, 0, p+pos, arg_leng );


	while( true ){
		if( !ZnkStrFIO_fgets( line, 0, 4096, fp ) ){
			/* Unexpected eof */
			Znk_printf_e( "ZnkMyf_load : Unexpected EOF in SectionVars %s : line:%zu\n",
					ZnkStr_cstr(sec->name_), *count );
			goto FUNC_END;
		}
		++*count;
		ZnkStr_chompNL( line );

		if( ZnkStr_empty( line ) ){
			continue;
		}
		if( ZnkStr_first( line ) == '#' ){
			/* treat as comment out */
			continue;
		}
		if( ZnkStr_eq( line, "@@." ) ){
			/* endof this Section */
			break;
		}
		if( ZnkStr_eq( line, "@@^" ) ){
			/* Special empty */
			ZnkStr_cut_front( line, 3 );
			break;
		}
		if( !parseVarDirective( sec->u_.vars_, ZnkStr_cstr(sec->name_), line,
					fp, nl, count, lhs_tmp,
					quote_begin, quote_end, quote_begin_leng, quote_end_leng ) )
		{
			goto FUNC_END;
		}
	}
	result = true;

FUNC_END:
	ZnkStr_delete( lhs_tmp );
	return result;
}
static bool
parseSectionPrims( ZnkMyf myf, const char* p, size_t p_leng,
		ZnkFile fp, size_t* count, ZnkStr line )
{
	size_t pos = 0;
	size_t arg_leng = 0;
	const char*  quote_begin = myf->quote_begin_;
	const char*  quote_end   = myf->quote_end_;
	const size_t quote_begin_leng = strlen( quote_begin );
	const size_t quote_end_leng   = strlen( quote_end );
	const char* nl = myf->nl_;
	ZnkMyfSection sec;
	ZnkStr lhs_tmp = ZnkStr_new( "" );
	bool result = false;

	/***
	 * sec_ary_�����order_�ւ̓o�^.
	 */
	sec = makeMyfSection( "", ZnkMyfSection_e_Prims );
	ZnkObjAry_push_bk( myf->sec_ary_, (ZnkObj)sec );
	ZnkObjAry_push_bk( myf->order_, (ZnkObj)sec );

	/* 1th arg */
	pos = ZnkS_lfind_arg( p+pos, 0, p_leng-pos, 0, &arg_leng, " \t", 2 ) + pos;
	ZnkStr_assign( sec->name_, 0, p+pos, arg_leng );


	while( true ){
		if( !ZnkStrFIO_fgets( line, 0, 4096, fp ) ){
			/* Unexpected eof */
			Znk_printf_e( "ZnkMyf_load : Unexpected EOF in SectionPrims %s : line:%zu\n",
					ZnkStr_cstr(sec->name_), *count );
			goto FUNC_END;
		}
		++*count;
		ZnkStr_chompNL( line );

		if( ZnkStr_empty( line ) ){
			continue;
		}
		if( ZnkStr_first( line ) == '#' ){
			/* treat as comment out */
			continue;
		}
		if( ZnkStr_eq( line, "@@." ) ){
			/* endof this Section */
			break;
		}
		if( ZnkStr_eq( line, "@@^" ) ){
			/* Special empty */
			ZnkStr_cut_front( line, 3 );
			break;
		}
		if( !parsePrimDirective( sec->u_.prims_, ZnkStr_cstr(sec->name_), line,
					fp, nl, count, lhs_tmp,
					quote_begin, quote_end, quote_begin_leng, quote_end_leng ) )
		{
			goto FUNC_END;
		}
	}
	result = true;

FUNC_END:
	ZnkStr_delete( lhs_tmp );
	return result;
}

static void
registOutOfSection( ZnkMyf myf, ZnkStrAry text )
{
	ZnkMyfSection sec;

	/***
	 * oos_ary_�����order_�ւ̓o�^.
	 */
	sec = makeMyfSection( "", ZnkMyfSection_e_OutOfSection );
	ZnkObjAry_push_bk( myf->oos_ary_, (ZnkObj)sec );
	ZnkObjAry_push_bk( myf->order_, (ZnkObj)sec );

	/***
	 * swap�ɂ�茋�ʓI��text�̓��e��lines_�ֈړ�����`�ƂȂ�.
	 * (text�̕��̓N���A�����`�ɂȂ�)
	 */
	ZnkStrAry_swap( sec->u_.lines_, text );

}

bool
ZnkMyf_load( ZnkMyf myf, const char* filename )
{
	bool result = false;
	ZnkFile fp = Znk_fopen( filename, "rb" );
	if( fp ){
		ZnkStr line = ZnkStr_new( "" );
		size_t count = 0;
		/***
		 * OutOfSection�ɂ�����Text���L�^����.
		 */
		ZnkStrAry oos_text = ZnkStrAry_create( true );
		bool is_outof_section = true;

		ZnkMyf_clear( myf );

		if( !ZnkStrFIO_fgets( line, 0, 4096, fp ) ){
			/* eof */
			Znk_printf_e( "ZnkMyf_load : Syntax Error : line:%zu\n", count );
			Znk_printf_e( "  File is empty.\n");
			goto FUNC_END;
		}
		++count;

		/***
		 * ���s�R�[�h���ǂ��Ȃ��Ă���̂����܂����.
		 */
		{
			if( ZnkStr_isEnd( line, "\r\n" ) ){
				ZnkS_copy_literal( myf->nl_, sizeof(myf->nl_), "\r\n" );
			} else if( ZnkStr_last( line ) == '\n' ){
				ZnkS_copy_literal( myf->nl_, sizeof(myf->nl_), "\n" );
			} else if( ZnkStr_last( line ) == '\r' ){
				ZnkS_copy_literal( myf->nl_, sizeof(myf->nl_), "\r" );
			} else {
				/* default���̗p */
			}
		}
		ZnkStr_chompNL( line );

		/***
		 * �J�n�s�ɂ�����@def_quote�w��͕K�{�Ƃ���.
		 * (���̕����g�p����N�H�[�g�L����myf�t�@�C�����ł�����������
		 * �����e�i���X����ꍇ�ł��]�܂����Ǝv��)
		 */
		if( ZnkStr_isBegin( line, "@def_quote" ) ){
			const char*  p      = ZnkStr_cstr( line ) + 1;
			const size_t p_leng = ZnkStr_leng( line ) - 1;
			if( !parseDefQuote( myf, p, p_leng, count, "def_quote" ) ){
				goto FUNC_END;
			}
		} else {
			Znk_printf_e( "ZnkMyf_load : Syntax Error : line:%zu\n", count );
			Znk_printf_e( "  @def_quote is not found.\n");
			goto FUNC_END;
		}

		while( true ){
			if( !ZnkStrFIO_fgets( line, 0, 4096, fp ) ){
				/* eof */
				break;
			}
			++count;
			ZnkStr_chompNL( line );

			if( ZnkStr_isBegin( line, "@@" ) ){
				const char*  p      = ZnkStr_cstr( line ) + 3;
				const size_t p_leng = ZnkStr_leng( line ) - 3;
				char sec_type_ch = *(p-1);
				switch( sec_type_ch ){
				case 'V':
					registOutOfSection( myf, oos_text );
					if( !parseSectionVars( myf, p, p_leng, fp, &count, line ) ){
						goto FUNC_END;
					}
					is_outof_section = false;
					break;
				case 'L':
					registOutOfSection( myf, oos_text );
					if( !parseSectionLines( myf, p, p_leng, fp, &count, line ) ){
						goto FUNC_END;
					}
					is_outof_section = false;
					break;
				case 'P':
					registOutOfSection( myf, oos_text );
					if( !parseSectionPrims( myf, p, p_leng, fp, &count, line ) ){
						goto FUNC_END;
					}
					is_outof_section = false;
					break;
				default:
					/* ����ȊO�̕����������ꍇ��OutOfSection�̃e�L�X�g�̂܂܂Ƃ��Ĉ��� */
					break;
				}
			}

			if( is_outof_section ){
				ZnkStrAry_push_bk_cstr( oos_text, ZnkStr_cstr(line), Znk_NPOS );
			} else {
				/* OutOfSection�̍ĊJ */
				is_outof_section = true;
			}
		}
		result = true;
FUNC_END:
		ZnkStr_delete( line );
		Znk_fclose( fp );
		ZnkStrAry_destroy( oos_text );
	}
	return result;
}

bool
ZnkMyf_save( ZnkMyf myf, const char* filename )
{
	ZnkFile fp = Znk_fopen( filename, "wb" );
	if( fp ){
		const char* nl = myf->nl_;
		size_t order_size;
		size_t order_idx;
		size_t size;
		size_t idx;
	
		Znk_fprintf( fp, "@def_quote %s %s%s", myf->quote_begin_, myf->quote_end_, nl );

		order_size = ZnkObjAry_size( myf->order_ );
		for( order_idx=0; order_idx<order_size; ++order_idx ){
			ZnkMyfSection sec = (ZnkMyfSection)ZnkObjAry_at( myf->order_, order_idx );
			switch( sec->type_ ){
			case ZnkMyfSection_e_Lines:
				Znk_fprintf( fp, "@@L %s%s", ZnkStr_cstr(sec->name_),  nl );
				size = ZnkStrAry_size( sec->u_.lines_ );
				for( idx=0; idx<size; ++idx ){
					Znk_fprintf( fp, "%s%s", ZnkStrAry_at_cstr( sec->u_.lines_, idx ), nl );
				}
				Znk_fprintf( fp, "@@.%s", nl );
				break;
			case ZnkMyfSection_e_Vars:
			{
				ZnkVarp  varp  = NULL;
				Znk_fprintf( fp, "@@V %s%s", ZnkStr_cstr(sec->name_),  nl );
				size = ZnkVarpAry_size( sec->u_.vars_ );
				for( idx=0; idx<size; ++idx ){
					varp = ZnkVarpAry_at( sec->u_.vars_, idx );
					if( ZnkVar_prim_type( varp ) == ZnkPrim_e_Str ){
						Znk_fprintf( fp, "%s = %s%s%s%s",
								ZnkStr_cstr( varp->name_ ),
								myf->quote_begin_, ZnkVar_cstr( varp ), myf->quote_end_,
								nl );
					} else if( ZnkVar_prim_type( varp ) == ZnkPrim_e_StrAry ){
						ZnkStrAry sda = ZnkVar_str_ary( varp );
						const size_t  sda_size = ZnkStrAry_size( sda );
						size_t sda_idx;
						Znk_fprintf( fp, "%s = { ", ZnkStr_cstr( varp->name_ ) );
						for( sda_idx=0; sda_idx<sda_size; ++sda_idx ){
							Znk_fprintf( fp, "%s%s%s%s",
									myf->quote_begin_, ZnkStrAry_at_cstr( sda, sda_idx ), myf->quote_end_, nl );
						}
						Znk_fprintf( fp, "}%s", nl );
					}
				}
				Znk_fprintf( fp, "@@.%s", nl );
				break;
			}
			case ZnkMyfSection_e_Prims:
			{
				ZnkPrimp primp = NULL;
				Znk_fprintf( fp, "@@P %s%s", ZnkStr_cstr(sec->name_),  nl );
				size = ZnkPrimpAry_size( sec->u_.prims_ );
				for( idx=0; idx<size; ++idx ){
					primp = ZnkPrimpAry_at( sec->u_.prims_, idx );
					if( ZnkPrim_type( primp ) == ZnkPrim_e_Str ){
						Znk_fprintf( fp, "%s%s%s%s",
								myf->quote_begin_, ZnkPrim_cstr( primp ), myf->quote_end_,
								nl );
					} else if( ZnkPrim_type( primp ) == ZnkPrim_e_StrAry ){
						ZnkStrAry sda = ZnkPrim_strAry( primp );
						const size_t  sda_size = ZnkStrAry_size( sda );
						size_t sda_idx;
						Znk_fprintf( fp, "{ " );
						for( sda_idx=0; sda_idx<sda_size; ++sda_idx ){
							Znk_fprintf( fp, "%s%s%s%s",
									myf->quote_begin_, ZnkStrAry_at_cstr( sda, sda_idx ), myf->quote_end_, nl );
						}
						Znk_fprintf( fp, "}%s", nl );
					}
				}
				Znk_fprintf( fp, "@@.%s", nl );
				break;
			}
			case ZnkMyfSection_e_OutOfSection:
				size = ZnkStrAry_size( sec->u_.lines_ );
				for( idx=0; idx<size; ++idx ){
					Znk_fprintf( fp, "%s%s", ZnkStrAry_at_cstr( sec->u_.lines_, idx ), nl );
				}
				break;
			default:
				break;
			}
		}
	
		Znk_fclose( fp );
		return true;
	}
	return false;
}

ZnkVarp
ZnkMyf_refVar( ZnkMyf myf, const char* sec_name, const char* var_name )
{
	ZnkVarpAry vars = ZnkMyf_find_vars( myf, sec_name );
	if( vars ){
		size_t size = ZnkVarpAry_size( vars );
		size_t idx;
		ZnkVarp varp;
		for( idx=0; idx<size; ++idx ){
			varp = ZnkVarpAry_at( vars, idx );
			if( ZnkStr_eq( varp->name_, var_name ) ){
				return varp;
			}
		}
	}
	return NULL;
}

