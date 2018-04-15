#ifndef INCLUDE_GUARD__Rano_plugin_dev_h__
#define INCLUDE_GUARD__Rano_plugin_dev_h__

#include <Znk_myf.h>

Znk_EXTERN_C_BEGIN

typedef enum {
	 RanoPluginState_e_Error      = -1
	,RanoPluginState_e_NotUpdated = 0
	,RanoPluginState_e_Updated    = 1
}RanoPluginState;

/**
 * @brief
 *  target�ŗL��send�t�B���^�̏������������s��.
 *  Web Configuration�́uVirtual USERS Initiation�v�{�^���������Ƃ��̊֐����Ăяo�����.
 * 
 * @param ftr_send:
 *  �������ΏۂƂȂ�t�B���^�ւ̎Q�Ƃł���.
 *  plugin�͂��̊֐����ł��̒l���Q�Ƃ���ѕύX���Ă��܂�Ȃ�.
 * 
 * @param parent_proxy:
 *  Rano�����ݎg�p���̊O���v���L�V��hostname:port�̌`���Őݒ肳��Ă���.
 *  (�O���v���L�V���g�p���Ă��Ȃ��ꍇ�͂��̒l����l�ł��邩�܂���NONE�ł��邩�܂���:0���w�肳��Ă���)
 *  plugin�͂��̊֐����ł��̒l�̎Q�Ƃ͉\�����ύX���邱�Ƃ͂ł��Ȃ�.
 * 
 * @param result_msg:
 *  �����ɂ͂��̊֐��̏����������Ȃ������ʂ̃��b�Z�[�W���i�[���Ȃ���΂Ȃ�Ȃ�.
 *  ����͏��������������ꍇ�͂�������|�[�g���b�Z�[�W�ł���A�G���[�����������ꍇ�͂��������
 *  �G���[���b�Z�[�W�ƂȂ�.
 *  �uVirtual USERS Initiation�v�{�^���������Ƃ������ɂ��̃��b�Z�[�W���\�������`�ɂȂ�.
 */
RanoPluginState
initiate( ZnkMyf ftr_send, const char* parent_proxy, ZnkStr result_msg );

/**
 * @brief
 *  target�ŗL��POST���O���̏������s��.
 *  POST���O�ɖ���Ăяo�����.
 * 
 * @param ftr_send:
 *  �����ΏۂƂȂ�t�B���^�ւ̎Q�Ƃł���.
 *  plugin�͂��̊֐����ł��̒l���Q�Ƃ���ѕύX���Ă��܂�Ȃ�.
 *
 * @param hdr_vars:
 *  �����ΏۂƂȂ郊�N�G�X�gHTTP Header�̎Q�Ƃł���.
 *  plugin�͂��̊֐����ł��̒l���Q�Ƃ���ѕύX���Ă��܂�Ȃ�.
 *
 * @param post_vars:
 *  �����ΏۂƂȂ�POST�ϐ��̎Q�Ƃł���.
 *  plugin�͂��̊֐����ł��̒l���Q�Ƃ���ѕύX���Ă��܂�Ȃ�.
 */
RanoPluginState
on_post( ZnkMyf ftr_send,
		ZnkVarpAry hdr_vars, ZnkVarpAry post_vars );

/**
 * @brief
 * target�ŗL�̃��X�|���X�w�b�_���󂯎�����ꍇ�ɂ����鏈�����s��.
 * ���X�|���X���󂯎�����ꍇ�ɖ���Ăяo�����.
 *
 * @param ftr_send:
 *  �����ΏۂƂȂ�t�B���^�ւ̎Q�Ƃł���.
 *  plugin�͂��̊֐����ł��̒l���Q�Ƃ���ѕύX���Ă��܂�Ȃ�.
 *
 * @param hdr_vars:
 *  �����ΏۂƂȂ郌�X�|���XHTTP�w�b�_�ւ̎Q�Ƃł���.
 *  plugin�͂��̊֐����ł��̒l���Q�Ƃ���ѕύX���Ă��܂�Ȃ�.
 *
 * @param text:
 *  �����ΏۂƂȂ郌�X�|���X�e�L�X�g�f�[�^�ւ̎Q�Ƃł���.
 *  plugin�͂��̊֐����ł��̒l���Q�Ƃ���ѕύX���Ă��܂�Ȃ�.
 *
 * @param req_urp:
 *  �����ΏۂƂȂ�URI��path����(URI�ɂ�����I�[�\���e�B�����̕���)���n�����.
 *  plugin�͂��̊֐����ł��̒l�̎Q�Ƃ͉\�����ύX���邱�Ƃ͂ł��Ȃ�.
 */
RanoPluginState
on_response( ZnkMyf ftr_send,
		ZnkVarpAry hdr_vars, ZnkStr text, const char* req_urp );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
