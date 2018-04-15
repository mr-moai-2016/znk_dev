#ifndef INCLUDE_GUARD__Rano_module_h__
#define INCLUDE_GUARD__Rano_module_h__

#include <Znk_varp_ary.h>
#include <Znk_myf.h>
#include <Rano_txt_filter.h>
#include <Rano_type.h>

Znk_EXTERN_C_BEGIN

typedef struct RanoModuleImpl* RanoModule;

typedef bool (*RanoInitiateFunc)( ZnkMyf ftr_send, const char* parent_proxy, ZnkStr result_msg );
typedef bool (*RanoOnPostFunc)( ZnkMyf ftr_send, ZnkVarpAry hdr_vars, ZnkVarpAry post_vars );
typedef bool (*RanoOnResponse)( ZnkMyf ftr_send, ZnkVarpAry hdr_vars, ZnkStr text, const char* req_urp );

RanoModule
RanoModule_create( void );

void
RanoModule_destroy( RanoModule mod );

bool
RanoModule_load( RanoModule mod, const char* target_name,
		const char* filters_dir, const char* plugins_dir );

bool
RanoModule_saveFilter( const RanoModule mod );

const char*
RanoModule_target_name( const RanoModule mod );

ZnkMyf
RanoModule_ftrSend( const RanoModule mod );

RanoTxtFilterAry
RanoModule_ftrHtml( const RanoModule mod );
RanoTxtFilterAry
RanoModule_ftrJS( const RanoModule mod );
RanoTxtFilterAry
RanoModule_ftrCSS( const RanoModule mod );
ZnkStrAry
RanoModule_ftrCSSAdditional( const RanoModule mod );

bool
RanoModule_invokeInitiate( const RanoModule mod, const char* parent_proxy, ZnkStr result_msg );
bool
RanoModule_invokeOnPost( const RanoModule mod, ZnkVarpAry hdr_vars, ZnkVarpAry post_vars, bool* updated );
bool
RanoModule_invokeOnResponse( const RanoModule mod, ZnkVarpAry hdr_vars, ZnkStr text, const char* req_urp );

/**
 * @brief
 * �w�b�_�ϐ��̔z�� hdr_vars �� �t�B���^ ftr_send �Ńt�B���^�����O����.
 */
size_t
RanoModule_filtHtpHeader( const RanoModule mod, ZnkVarpAry hdr_vars );

/**
 * @brief
 * POST�ϐ��̔z�� post_vars �� �t�B���^ ftr_send �Ńt�B���^�����O����.
 */
size_t
RanoModule_filtPostVars( const RanoModule mod, ZnkVarpAry post_vars );

/**
 * @brief
 * �w�b�_�ϐ��̔z�� hdr_vars ����Cookie�t�B�[���h�����݂���ꍇ�́A
 * ����� �t�B���^ ftr_send �Ńt�B���^�����O����.
 *
 * @param is_all_replace:
 *   �u���E�U��Cookie�����S�B�����郂�[�h.
 *   ���ꂪtrue�̏ꍇ�Acookie_vars/cookie_force�ɂȂ����m�̕ϐ����u���E�U���Ɏc���Ă���ꍇ�ł�
 *   �����ʂ��Ȃ�.
 *   ���ꂪfalse�̏ꍇ�́A��������̂܂ܒʂ�.
 *
 * @param extra_vars:
 *   cookie_vars, cookie_force �̂�����ɂ��܂܂�Ȃ�Cookie�ϐ����u���E�U���ɑ��݂����ꍇ
 *   ���̕ϐ��Q���擾���邱�Ƃ��ł���.
 *   ���̏�񂪕s�v�ȏꍇ��NULL���w�肷�邱�Ƃ��ł���.
 */
size_t
RanoModule_filtCookieVars( const RanoModule mod, ZnkVarpAry hdr_vars, bool is_all_replace, ZnkVarpAry extra_vars );

/**
 * @brief
 * �T�C�g���甭�s�����Set-Cookie�ɂ��Cookie�̒l�ύX���w�����ꂽ�ꍇ��
 * �����D�悵��filter�̒l���ς���.
 */
void
RanoFltr_updateCookieFilter_bySetCookie( const ZnkVarpAry hdr_vars, ZnkMyf ftr_send );

/**
 * @brief
 * �w�肵�� hostname ������RanoModule�̏����Ώۂł��邩�ۂ���Ԃ�.
 */
bool
RanoModule_isTargetHost( const RanoModule mod, const ZnkMyf mtgt, const char* hostname );


const char*
RanoTarget_findTargetName( const ZnkMyf mtgt, const char* hostname );

void
RanoModule_filtTxt( const RanoModule mod, ZnkStr text, RanoTextType txt_type );

/**
 * @brief
 * filter�t�@�C���̍X�V�������قȂ�ꍇ�̂ݍēǂݍ���.
 * (�����_�ł�plugin�͑ΏۊO)
 */
bool
RanoModule_reloadFilters_byDate( RanoModule mod, const char* target_name, const char* filters_dir );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
