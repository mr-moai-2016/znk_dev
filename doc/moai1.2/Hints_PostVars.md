# Hints PostVars
-----------------------------------

## <a name="index">�ڎ�
-----------------------------------
* [Post�ϐ� js](#pv_js)
* [Post�ϐ� pthb](#pv_pthb)
* [Post�ϐ� pthc](#pv_pthc)
* [Post�ϐ� pthd](#pv_pthd)
* [Post�ϐ� ptua](#pv_ptua)
* [Post�ϐ� scsz](#pv_scsz)
* [Post�ϐ� pwd](#pv_pwd)
* [Post�ϐ� flrv](#pv_flrv)
* [Post�ϐ� flvv](#pv_flvv)

## <a name="pv_js">Post�ϐ� js
-----------------------------------
  ���̒l�͏��q���ws�c�ł͂Ȃ��AJavaScript�����s�������ǂ����𔻒f���邽�߂̂��̂ł���A
  /bin/base4.js �����s����� on �ɐݒ肳���d�g�݂ƂȂ��Ă���.
  ( ���_�AJavaScript��/bin/base4.js�����s�������Ƃ��A�ŏ����炱��Post�ϐ��� on �l��ݒ肵�Ă����Ηǂ�. )
  ����� on �ɂ��Ă����Ȃ��Ɓu���ϐ�������܂���(JavaScript���I�t�H)�v�Ƃ����G���[���\������A���e�Ɏ��s����.
  ����Ă��̒l�͕K�� on �ɂ��Ă����Ȃ���΂Ȃ�Ȃ�.

  <a href="#user-content-index">�ڎ��֖߂�</a>

## <a name="pv_pthb"><a name="pv_pthc"><a name="pv_pthd">Post�ϐ� pthb, pthc, pthd
-----------------------------------

  pt��PosT �̈Ӗ��ł��낤. h �͕s���ł���. �Ȃ� a ����ł͂Ȃ� b ����n�܂�̂����s���ł���.

  ���̒l��futaba.php�ɏ��A�N�Z�X��������(����т���ɗނ����l)�ł���. ����ł�
  �J�^���O�ɏ��߂ăA�N�Z�X���邩�A�܂��͏��߂ă��X���e�����ۂɂ܂�pthc�ɐݒ肳���悤�ł���.
  ���邢�� /bin/cachemt7.php �ɃA�N�Z�X�����Ƃ��ɂ���ɗނ����l�����������`�ƂȂ�.
  ��x�ڂ̓��e�ɂ����ẮAlocalStorage(futabapt)��pthc�̃R�s�[�l���ݒ肳���.
  �܂���x�ڂ̓��e�ɂ����āA����ɂ���localStorage(futabapt)�̃R�s�[�l��pthb�ւƐݒ肳���.
  �����炭pthb�̒l�̑��݂ɂ����localStorage�Ɋm���ɒl���ݒ肳�ꂽ���Ƃ��m�F����Ӑ}������Ǝv����.
  pthd�͌���ł͎g�p����Ă���C�z�͂Ȃ�.

  ��U�ݒ肳���ƁACookie����уu���E�U�L���b�V�������localStorage �����S�Ă�
  �������Ȃ����蓯���l���c�葱����.

  <a href="#user-content-index">�ڎ��֖߂�</a>

## <a name="pv_ptua">Post�ϐ� ptua
-----------------------------------

  ���̒l�͕p�ɂɂ��̈Ӗ����ύX�������j������.
  �ŏ�����navigator.userAgent�̕�����l�����̂܂܊i�[���Ă������߁A���O�� ua �Ƃ����������܂܂�Ă���̂ł��낤.

  ����ł̈Ӗ��́A33�̂��̃u���E�U�Ƃ��̃o�[�W�����ŗL�̐^�U�l���Ƃ�Javascript�R�[�h��(Snippet�ƌĂ�)�̌��ʒl���A
  33bit�̃t���O�l�Ƀp�b�N�������10�i���ϊ��������̂ł���.
  ���̌v�Z�̓��X���e(ptfk�֐�)���ɖ��� /bin/base4.js�ōs���Ă���Asph�Ƃ������O�̔z��ϐ�����������ƊY���������s���Ă���s�������邱�Ƃ��ł���.
  (���A�ő�l��32bit�񕉐�����Max�𒴂���\�������邽�߁A���������_�����x�[�X�Ƃ��čs���Ă���͗l).

  �v�̓u���E�U�Ƃ��̃o�[�W�����ŗL�̒l�̊񂹏W�߂ł��邽�߁A�u���E�U���̂��̂��邢�͏ꍇ�ɂ���Ă͂��̃o�[�W�����݂̂ł��ǂ����A
  ���̂����ꂩ��ύX���Ȃ������{�I�ɓ����l���Ƃ葱����.

  ���A���̒l�𗐐�������Ȃǂŏo�͂���銮�S�ȃ����_���Ȑ����l�Ƃ��Ċ��蓖�ĂĂ��A���Ȃ��Ƃ��M�҂̊��ł͌�����Ȃ��悤�ł���.
  �����A����16bit��IE�Ɋւ�����̂ł��邽�߁AIE�ȊO�̃u���E�U�Ƃ��ċU���������Ȃ�A���̐�����K��256�̔{���ɂ���΁u����炵���v�U���ł���.
  ����ȊO�ɂ��eSnippet�̈Ӗ������ׂ��ɒ��ׂău���E�U�ƃo�[�W�������ɔ��f�����Ă����Βl�̐��x�͂�荂�܂�ł��낤.
  �����Œl���U���������ł��̕ӂ�ɍS��̂�����͎Q�l�ɂ���Ă͂ǂ����낤���H

  <a href="#user-content-index">�ڎ��֖߂�</a>

## <a name="pv_scsz">Post�ϐ� scsz
-----------------------------------

  ���O�̗R���� SCreenSiZe �ł��낤. 

  ���̒l�́A���Ȃ��̎g���Ă��郂�j�^�̉𑜓x�ƐF�[�x�̏�������.
  (Javascript�ɂ�����擾����API������A���X���e(ptfk�֐�)���ɖ��� /bin/base4.js�ł�����擾���Ă���悤�ł���)

~~~
    ���j�^�𑜓x�̕� x ���j�^�𑜓x�̍��� x ���j�^�̐F�[�x�r�b�g��
~~~

  �Ƃ����t�H�[�}�b�g�����L�q�q�ƂȂ�.
  �Ⴆ�΁A640x480x24�A1024x768x32�Ƃ������l���Ƃ�.

  <a href="#user-content-index">�ڎ��֖߂�</a>


## <a name="pv_pwd">Post�ϐ� pwd
-----------------------------------

  ���̒l�́A���X���e���̂�����u�폜�L�[�v�ɑ���������̂ł���.
  ���[�U�������I�Ɏw�肵�Ă��悢���A�ʏ��ɂ��Ă����ΓK���ȃL�[�l�������I�Ɋ��蓖�Ă���̂�
  ���蓖�Ă�ꂽ��͂�������̂܂܎g���΂悢.
  ���[�U�������Z�b�g�������ꍇ�́A�������U��ɂ��Ă����Ƃ悢.

  <a href="#user-content-index">�ڎ��֖߂�</a>

## <a name="pv_flrv"><a name="pv_flvv">Post�ϐ� flrv flvv
-----------------------------------

  ���̒l�́AFingerprint�l�ƌĂ΂����̂��i�[�����.

  ����͂��Ȃ��̃u���E�U����}�V�����̗l�X�ȏ���Javascript�ɂ��擾���A
  �������W�߂ĘA��������̋��啶���������32bit�����̃n�b�V���l���v�Z�������̂ł���.
  �u���E�U��}�V�����Ɉˑ��������ł��邽�߁A����炪�ς��΂���Fingerprint�l�͕ω�����.
  flrv��flvv�̈Ⴂ�͂��̎擾������̎�ނ����ł���.
  flrv���擾����S���ɉ����A�u���E�U�̃v���O�C����񕶎���܂ŉ����������̂�flvv�ł���.

  �n�b�V���l�����̂��Ƃ��킩��Ȃ����́A���̂悤�ɍl����΂悢.
  �Ⴆ�΂��Ȃ��̃}�V���̓�����������񂪂R�������Ƃ��Ă�������ꂼ�ꉼ��
  ������ 2, 4, 7 �Ƃ���. �����𑫂��Z���č��v����� 2 + 4 + 7 = 13 �ƂȂ邪�A
  ���� 13 ���n�b�V���l�ɑ����������ł͂��ꂪ���M�����. ���ɑ�G�c�����A
  �Ƃ肠�������̂悤�ȃC���[�W�ŏ\���ł���.

  �����ł̃|�C���g�� 13 ���猳�̒l 2, 4, 7 ����ӂɂ͕����ł��Ȃ��Ƃ������Ƃł���.
  ����Ă��̒l�͎��̂Ƃ���f�^�����ɐݒ肵�Ă���ʂɂ��̑Ó��������؂���p�͂Ȃ�.  
  ���̓_��ptua�Ƃ͑傫���قȂ�.

  <a href="#user-content-index">�ڎ��֖߂�</a>


[1]: https://github.com/mr-moai-2016/znk_dev/blob/master/src/moai/Hints_PostVars.md