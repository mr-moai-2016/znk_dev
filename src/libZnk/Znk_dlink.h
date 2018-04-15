#ifndef INCLUDE_GUARD__Znk_dlink_h__
#define INCLUDE_GUARD__Znk_dlink_h__

#include <Znk_base.h>

Znk_EXTERN_C_BEGIN

typedef void* ZnkDLinkHandler;

/**
 * @brief
 *   Dynamic Load Library���������փ��[�h����.
 */
ZnkDLinkHandler ZnkDLink_open( const char* lib_name );
/**
 * @brief
 *   Dynamic Load Library���̊֐��|�C���^���擾����.
 */
void* ZnkDLink_getFunc( ZnkDLinkHandler handle, const char* func_name );
/**
 * @brief
 *   ���[�h����Dynamic Load Library������������폜����.
 *   ���̊֐����Ăяo������́AZnkDLink_getFunc �Ŏ擾�����֐��|�C���^�������ƂȂ�.
 *   �]���Ă������������S�Ɏg��Ȃ��Ȃ������_�ŏ��߂ČĂяo���K�v������.
 */
void ZnkDLink_close( ZnkDLinkHandler handle );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
