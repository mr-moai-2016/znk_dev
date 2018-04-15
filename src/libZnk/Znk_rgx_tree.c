#include "Znk_rgx_tree.h"
#include <Znk_base.h>
#include <Znk_stdc.h>
#include <Znk_str.h>
#include <stdlib.h>

#define DEBUG   1


/**
 * ���K�\�����p�[�X���č\���؂𐶐�����.
 * ���̍\���Œ�`����鐳�K�\�����󗝂���.
 *   <regexp>        ::= <term> | <term> "|" <regexp>
 *   <term>          ::= EMPTY  | <factor> <term>
 *   <factor>        ::= <primary> | <primary> "*" | <primary> "+"
 *   <primary>       ::=  CHARACTER | "(" <regexp> ")"
 */

/* token_t�̓g�[�N����\���^ */
typedef enum {
	tk_char,  /* �����i���ۂ̕�����st_token_char�ɃZ�b�g�����j */
	tk_union, /* '|' */
	tk_lpar,  /* '(' */
	tk_rpar,  /* ')' */
	tk_star,  /* '*' */
	tk_plus,  /* '+' */
	tk_dot,   /* '.' */
	tk_end    /* ������̏I��� */
} token_t;


typedef struct Context_tag {
	/* ���ݏ������̃g�[�N�� */
	token_t current_token_;
	
	/* current_token_��tk_char�ł���Ƃ��̕����l */
	ZnkUChar64 token_char_;
	
	/* ��͂��镶����ւ̃|�C���^ */
	const char* strbuff_;

	ZnkUChar64FuncT_get func_get_;

	/* �G���[/���|�[�g�i�[�p������ */
	ZnkStr rep_msg_;
} Context;

static void
addMsgf( Context* ctx, const char* fmt, ... )
{
	if( ctx->rep_msg_ ){
		va_list ap;
		va_start(ap, fmt);
		ZnkStr_vsnprintf2( ctx->rep_msg_, Znk_NPOS, Znk_NPOS, fmt, ap );
		va_end(ap);
	}
}

#define SYNTAX_ERROR( ctx, msg ) addMsgf( ctx, "ZnkRgxTree : Syntax Error : " msg "\n" )

static size_t st_allocated_node_count = 0;
static ZnkRgxTree* allocTree( void )
{
	++st_allocated_node_count;
	return (ZnkRgxTree*) Znk_alloc0( sizeof(ZnkRgxTree) );
}
static void freeTree( ZnkRgxTree* tree )
{
	--st_allocated_node_count;
	Znk_free( tree );
}
size_t
ZnkRgxTree_getAllocatedNodeCount( void )
{
	return st_allocated_node_count;
}


/* �g�[�N����1�ǂݍ���ŕԂ� */
static token_t get_token( Context* ctx )
{
	ZnkUChar64 uc = { 0 };
	
	/* ������̏I���ɓ��B������tk_end��Ԃ� */
	if( *(ctx->strbuff_) == '\0' ){
		ctx->current_token_ = tk_end;
	} else {
		/* �������g�[�N���ɕϊ����� */
		ctx->strbuff_ = (const char*)( (*ctx->func_get_)( &uc, (const uint8_t*)ctx->strbuff_ ) );
		switch( uc.u64_ ){
		case '|': ctx->current_token_ = tk_union; break;
		case '(': ctx->current_token_ = tk_lpar;  break;
		case ')': ctx->current_token_ = tk_rpar;  break;
		case '*': ctx->current_token_ = tk_star;  break;
		case '+': ctx->current_token_ = tk_plus;  break;
		case '.':
			ctx->current_token_ = tk_dot;
			ctx->token_char_.u64_ = ZnkRgx_DOT;
			break;
		default:
			ctx->current_token_ = tk_char;
			ctx->token_char_ = uc;
			break;
		}
	}
	return ctx->current_token_;
}

/* ���K�\������͂���p�[�T������������ */
static void
initialize_regexp_parser( Context* ctx, const char *str, ZnkUChar64FuncT_get func_get )
{
	ctx->strbuff_  = str;
	ctx->func_get_ = func_get;
	get_token( ctx );
}

/* �\���؂̃m�[�h���쐬����B
   op�̓m�[�h���\�����Z�Aleft�͍��̎q�Aright�͉E�̎q */
static ZnkRgxTree*
make_tree_node( ZnkRgxOP_t op, ZnkRgxTree* left, ZnkRgxTree* right )
{
	ZnkRgxTree* p;

	/* �m�[�h�����蓖�Ă� */
	p = allocTree();
	/* �m�[�h�ɏ���ݒ肷�� */
	p->op = op;
	p->u.x.left_  = left;
	p->u.x.right_ = right;
	return p;
}

/* �\���؂̗t�����B
   c�͂��̗t���\������ */
static ZnkRgxTree*
make_atom( ZnkUChar64 c )
{
	ZnkRgxTree* p;

	/* �t�����蓖�Ă� */
	p = allocTree();
	/* �t�ɏ���ݒ肷�� */
	p->op = ZnkRgxOP_char;
	p->u.c_ = c;
	return p;
}
static ZnkRgxTree*
make_dot( void )
{
	ZnkRgxTree* p;

	/* �t�����蓖�Ă� */
	p = allocTree();
	/* �t�ɏ���ݒ肷�� */
	p->op = ZnkRgxOP_dot;
	p->u.c_.u64_ = ZnkRgx_DOT;
	return p;
}

static ZnkRgxTree* term( Context* ctx );
static ZnkRgxTree* factor( Context* ctx );
static ZnkRgxTree* primary( Context* ctx );

/* <regexp>���p�[�X���āA����ꂽ�\���؂�Ԃ��B
   �I��X|Y����͂��� */
static ZnkRgxTree* regexp( Context* ctx )
{
	ZnkRgxTree* x = term( ctx );
	while( ctx->current_token_ == tk_union ){
		get_token( ctx );
		x = make_tree_node(ZnkRgxOP_union, x, term( ctx ));
	}
	return x;
}

/* <term>���p�[�X���āA����ꂽ�\���؂�Ԃ��B
   �A��XY����͂��� */
static ZnkRgxTree*
term( Context* ctx )
{
	ZnkRgxTree* x;
	
	switch( ctx->current_token_ ){
	case tk_union:
	case tk_rpar:
	case tk_end:
		x = make_tree_node(ZnkRgxOP_empty, NULL, NULL);
		break;
	default:
		x = factor( ctx );
		while( ctx->current_token_ != tk_union &&
		       ctx->current_token_ != tk_rpar &&
		       ctx->current_token_ != tk_end )
		{
			x = make_tree_node(ZnkRgxOP_concat, x, factor( ctx ));
		}
		break;
	}
	return x;
}

/* <factor>���p�[�X���āA����ꂽ�\���؂�Ԃ��B
   �J��Ԃ�X*�AX+����͂��� */
static ZnkRgxTree* factor( Context* ctx )
{
	ZnkRgxTree* p;
	ZnkRgxTree* x = NULL;
	
	p = primary( ctx );
	if( ctx->current_token_ == tk_star ){
		x = make_tree_node(ZnkRgxOP_closure, p, NULL);
		get_token( ctx );
	} else if ( ctx->current_token_ == tk_plus ) {
		/* �����̂� p �����L�Q�Ƃ��Ă���.
		 * ���������ĉ�����ɍH�v���K�v. */
		x = make_tree_node(ZnkRgxOP_concat, p, make_tree_node(ZnkRgxOP_closure, p, NULL));
		get_token( ctx );
	} else {
		x = p;
	}
	return x;
}

/* <primary>���p�[�X���āA����ꂽ�\���؂�Ԃ��B
   �������̂��́A(X)����͂��� */
static ZnkRgxTree*
primary( Context* ctx )
{
	ZnkRgxTree* x = NULL;
	
	if (ctx->current_token_ == tk_char) {
		x = make_atom(ctx->token_char_);
		get_token( ctx );
	} else if (ctx->current_token_ == tk_dot) {
		x = make_dot();
		get_token( ctx );
	} else if (ctx->current_token_ == tk_lpar) {
		get_token( ctx );
		x = regexp( ctx );
		if (ctx->current_token_ != tk_rpar){
			SYNTAX_ERROR( ctx, "primary : Close paren is expected." );
			return NULL;
		}
		get_token( ctx );
	} else {
		if( ctx->rep_msg_ ){
			SYNTAX_ERROR( ctx, "primary : Normal character or open paren is expected." );
		}
	}
	return x;
}

#if     DEBUG
/* �\���؂�\������i�f�o�b�O�p�j */
static bool
dumpTree( Context* ctx, ZnkRgxTree* p )
{
	switch (p->op) {
	case ZnkRgxOP_char:
		addMsgf( ctx, "\"%c\"", p->u.c_);
		break;
	case ZnkRgxOP_dot:
		addMsgf( ctx, "\"[%x]\"", p->u.c_);
		break;
	case ZnkRgxOP_concat:
		addMsgf( ctx, "(concat ");
		if( !dumpTree(ctx, p->u.x.left_) ){ return false; }
		addMsgf( ctx, " ");
		if( !dumpTree(ctx, p->u.x.right_) ){ return false; }
		addMsgf( ctx, ")");
		break;
	case ZnkRgxOP_union:
		addMsgf( ctx, "(or ");
		if( !dumpTree(ctx, p->u.x.left_) ){ return false; }
		addMsgf( ctx, " ");
		if( !dumpTree(ctx, p->u.x.right_) ){ return false; }
		addMsgf( ctx, ")");
		break;
	case ZnkRgxOP_closure:
		addMsgf( ctx, "(closure ");
		if( !dumpTree(ctx, p->u.x.left_) ){ return false; }
		addMsgf( ctx, ")");
		break;
	case ZnkRgxOP_empty:
		addMsgf( ctx, "ZnkRgx_EMPTY");
		break;
	default:
		addMsgf( ctx, "ZnkRgxTree : Internal Error : dumpTree : This cannot happen.\n");
		return false;
	}
	return true;
}
#endif  /* DEBUG */


/***
 * ���K�\�����p�[�X���āA���K�\���ɑΉ�����\���؂�Ԃ��B
 * str�͐��K�\���������Ă��镶����.
 */
ZnkRgxTree*
ZnkRgxTree_create( const char* str, ZnkUChar64FuncT_get func_get, ZnkStr rep_msg )
{
	ZnkRgxTree* t = NULL;
	Context ctx = { 0 };
	ctx.rep_msg_ = rep_msg;
	
	/* �p�[�T������������ */
	initialize_regexp_parser( &ctx, str, func_get );
	
	/* ���K�\�����p�[�X���� */
	t = regexp( &ctx );
	
	/* ���̃g�[�N����tk_end�łȂ���΃G���[ */
	if( ctx.current_token_ != tk_end ){
		SYNTAX_ERROR( &ctx, "create : Extra character at end of pattern." );
		goto FUNC_ERROR;
	}

#if     DEBUG
	/* ����ꂽ�\���؂̓��e��\������ */
	if( !dumpTree( &ctx, t ) ){
		goto FUNC_ERROR;
	}
	addMsgf( &ctx, "\n");
#endif  /* DEBUG */

	/* ���������\���؂�Ԃ� */
	return t;

FUNC_ERROR:
	ZnkRgxTree_destroy( t, ctx.rep_msg_ );
	return NULL;
}

void
ZnkRgxTree_destroy( ZnkRgxTree* tree, ZnkStr rep_msg )
{
	if( tree ){

    	if( tree->op == ZnkRgxOP_concat ){
			ZnkRgxTree* left  = tree->u.x.left_;
			ZnkRgxTree* right = tree->u.x.right_;
			if( right ){
				if( right->op == ZnkRgxOP_closure ){
					/* plus closure�̏ꍇ�Aleft == right->left�����L�Q�ƂƂȂ��Ă��邱�Ƃ����蓾��. */
					if( left == right->u.x.left_ ){
						if( rep_msg ){
							ZnkStr_add( rep_msg, "ZnkRgxTree_destroy : Report : sharing reference of plus_closure detect.\n" );
						}
						ZnkRgxTree_destroy( left, rep_msg );
						/* right->left�͊��ɉ������Ă���Aright->right�͂��̏ꍇ�K��NULL�ł���.
						 * ���������ĒP��right��P��free���Ȃ���΂Ȃ�Ȃ�. */
						freeTree( right );

						freeTree( tree );
						return;
					}
				}
			}
		}

		if( tree->op != ZnkRgxOP_char && tree->op != ZnkRgxOP_dot ){
			if( tree->u.x.left_ ){
				ZnkRgxTree_destroy( tree->u.x.left_, rep_msg );
			}
			if( tree->u.x.right_ ){
				ZnkRgxTree_destroy( tree->u.x.right_, rep_msg );
			}
		}
		freeTree( tree );
	}
}
