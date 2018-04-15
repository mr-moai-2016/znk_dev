#ifndef INCLUDE_GUARD__Znk_htp_rar_h__
#define INCLUDE_GUARD__Znk_htp_rar_h__

#include <Znk_bfr.h>
#include <Znk_varp_ary.h>
#include <Znk_socket.h>
#include <Znk_htp_hdrs.h>
#include <Znk_err.h>

Znk_EXTERN_C_BEGIN

/***
 * recv���ꂽdata���ǂ����邩���K�肷��֐�.
 */
typedef int (*ZnkHtpOnRecvFunc)( void* param, const uint8_t* data, size_t data_size );
typedef struct {
	ZnkHtpOnRecvFunc func_;
	void* arg_;
} ZnkHtpOnRecvFuncArg;

/**
 * @brief
 *  url�����߂��Ahostname��port�𕪗����Ď擾����.
 */
void
ZnkHtpRAR_getHostnameAndPort( const char* url,
		char* hostname_buf, size_t hostname_buf_size, uint16_t* port );

/**
 * @brief
 *  HTTP Request��send���AResponse��recv����.
 *
 * @param cnct_hostname:
 * @param cnct_port:
 *  �\�P�b�g�ɂ��ڑ�����z�X�g�ƃ|�[�g.
 *  cnct_hostname��send_hdrs����Host�t�B�[���h�̒l���قȂ�ꍇ��
 *  cnct_hostname�Acnct_port ��Proxy�̃z�X�g���ƃ|�[�g�ԍ��ł���Ƃ݂Ȃ�.
 *  cnct_hostname �� hostname:port�Ƃ����`�����w�肷�邱�Ƃ͂ł��Ȃ�.
 *
 * @param send_hdrs:
 *  send����HTTP��Request�w�b�_���܂����ׂĂ����ɃZ�b�g���Ă���n���K�v������.
 *  ����ł͂��̎w��͕K�{�ł���ANULL�͎w��ł��Ȃ�.
 *
 * @param send_body:
 *  send����HTTP�̃{�f�B���w�肷��.
 *  ���������̏�񂪕s�v�ȏꍇ��NULL���w�肷�邱�Ƃ��ł��A���̏ꍇ��
 *  �{�f�B�̑��݂��Ȃ�Request�ƂȂ�.
 *
 * @param recv_hdrs:
 *  recv�������ʂ�HTTP Response�w�b�_���i�[�����.
 *  ����ł͂��̎w��͕K�{�ł���ANULL�͎w��ł��Ȃ�.
 *
 * @param recv_fnca:
 *  recv���ꂽdata�����H���邽�߂̃R�[���o�b�NFuncArg.
 *
 * @param cookie:
 *  recv���V�[�u����HTTP��Set-Cookie�t�B�[���h���cookie�����擾���A����Ɋi�[����.
 *  ���������̏�񂪕s�v�ȏꍇ��NULL���w�肷�邱�Ƃ��ł���.
 *
 * @param try_connect_num:
 *  �T�[�o�ւ̐ڑ��̍ő厎�s�񐔂��w�肷��.
 *
 * @param is_proxy:
 *  Proxy�T�[�o�����sendAndRecv���s�������ꍇ:
 *    is_proxy��true�ɂ���.
 *    ���̏ꍇ�A�����Ƃ��ė^����cnct_hostname��cnct_port��proxy�T�[�o�̂��̂ł���K�v������.
 *    ���Areq_uri���z�X�g������n�܂��Ă��Ȃ��ꍇ�͂��̑O�Ƀz�X�g���������I�ɒǉ������.
 *  Proxy�T�[�o�����sendAndRecv���s�������ꍇ:
 *    is_proxy��false�ɂ���.
 *    ���̏ꍇ�A�����Ƃ��ė^����cnct_hostname��cnct_port�̓_�C���N�g�Ȑڑ���ł���.
 *    ���A���̂Ƃ���req_uri�̓z�X�g������n�܂��Ă��Ă͂Ȃ�Ȃ�.
 *
 * @param wk_bfr:
 *  ����͓�����Ɨp�̈ꎞ�o�b�t�@�Ƃ��Ďg����.
 *  NULL���w�肷�邱�Ƃ��ł��邪�A���̏ꍇ�͂��̊֐����Ă΂��x�ɓ����Ŋm��/������s����.
 *  ���̊֐������x���Ăяo���ꍇ�͗\�ߊm�ۂ���wk_bfr�������ŗ^�����������ʂ��Ȃ��A
 *  ���������͂悭�Ȃ�.
 *
 * @param ermsg:
 *  �G���[�����|�[�g���b�Z�[�W�i�[�p������.
 */
bool
ZnkHtpRAR_sendAndRecv( const char* cnct_hostname, uint16_t cnct_port,
		ZnkHtpHdrs send_hdrs, ZnkBfr send_body,
		ZnkHtpHdrs recv_hdrs, ZnkHtpOnRecvFuncArg recv_fnca,
		ZnkVarpAry cookie,
		size_t try_connect_num, bool is_proxy, ZnkBfr wk_bfr, ZnkStr ermsg );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
