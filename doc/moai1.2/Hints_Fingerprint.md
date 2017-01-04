# Hints Fingerprint
-----------------------------------

## <a name="index">�ڎ�
-----------------------------------
* [Fgp_userAgent]       (#Fgp_userAgent)
* [Fgp_language]        (#Fgp_language)
* [Fgp_colorDepth]      (#Fgp_colorDepth)
* [Fgp_ScreenResolusion](#Fgp_ScreenResolusion)
* [Fgp_TimezoneOffset]  (#Fgp_TimezoneOffset)
* [Fgp_SessionStorage]  (#Fgp_SessionStorage)
* [Fgp_LocalStorage]    (#Fgp_LocalStorage)
* [Fgp_IndexedDB]       (#Fgp_IndexedDB)
* [Fgp_BodyAddBehavior] (#Fgp_BodyAddBehavior)
* [Fgp_OpenDatabase]    (#Fgp_OpenDatabase)
* [Fgp_CpuClass]        (#Fgp_CpuClass)
* [Fgp_Platform]        (#Fgp_Platform)

## <a name="Fgp_userAgent">Fgp_userAgent
-----------------------------------

  navigator.userAgent���擾����邢����User-Agent������ł���.
  ����̂ӂ��΂�Fingerprint�ɂ����Ă͂���͎Q�Ƃ���Ă��Ȃ�.

  <a href="#user-content-index">�ڎ��֖߂�</a>

## <a name="Fgp_language">Fgp_language
-----------------------------------

  navigator.language���擾����邢����u���E�U�̌����������������ł���.
  ���{����̃u���E�U�̏ꍇ�Aja�Aja-JP�Aja-jp�Aja-JP-mac�Ȃǂ̒l����邪�A
  ���ۂǂ̂悤�Ȓl�ɂȂ邩�̓u���E�U�₻�̐ݒ�Ȃǂɂ���Ă��l�X�ƂȂ�ꍇ������.

  �ܘ_��O�����邪�����悻�ȉ����ڈ��ł���.

  <ul>
  <li>Android: ja-JP</li>
  <li>iPhone: ja-jp</li>
  <li>Mac Firefox: ja-JP-mac</li>
  <li>Mac safari: ja-jp</li>
  <li>���̑�: ja</li>
  </ul>

  <a href="#user-content-index">�ڎ��֖߂�</a>

## <a name="Fgp_colorDepth">Fgp_colorDepth
-----------------------------------

  screen.colorDepth���擾����郂�j�^�̐F�[�x�r�b�g��������������ł���.
  32�A24�A16�Ȃǂ̒l����邪�A���ۂǂ̂悤�Ȓl�ɂȂ邩�̓u���E�U�₻�̐ݒ�Ȃǂɂ���Ă��l�X�ƂȂ�ꍇ������.

  <a href="#user-content-index">�ڎ��֖߂�</a>

## <a name="Fgp_ScreenResolusion">Fgp_ScreenResolusion
-----------------------------------

  screen.width�Ascreen.height���擾����郂�j�^�̉𑜓x��������̐����𕶎�x�ŘA������������ł���.
  ���Ԃ͂���Ƃ͋t���ɂȂ邱�Ƃ�����.
  ����̂ӂ��΂�Fingerprint�ɂ����Ă͂���͎Q�Ƃ���Ă��Ȃ�.

  <a href="#user-content-index">�ڎ��֖߂�</a>

## <a name="Fgp_TimezoneOffset">Fgp_TimezoneOffset
-----------------------------------

  new Date().getTimezoneOffset() �ɂ��擾�����^�C���]�[�����Ӗ�����(���E�W���������)�I�t�Z�b�g�l������������ł���.
  ���{�ł͂��̒l�͕K�� -540 �Œ�ƍl���Ă悢.

  <a href="#user-content-index">�ڎ��֖߂�</a>

## <a name="Fgp_SessionStorage">Fgp_SessionStorage
-----------------------------------

  !!window.sessionStorage �Ƃ����R�[�hSnippet�����s�������ʂ������Atrue�܂���false�Ƃ���������ƂȂ�.
  ���̃u���E�U��sessionStorage�@�\�����݂��邩�ۂ�������.
  �u���E�U�̑Ή��󋵂͈ȉ����ڈ��ł���(MDN�ɂ��).

  <ul>
  <li>Chrome: 5�ȏ�</li>
  <li>Firefox(Gecko): 2�ȏ�</li>
  <li>IE: 8�ȏ�</li>
  <li>Opera: 10.50�ȏ�</li>
  <li>Safari(Webkit): 4�ȏ�</li>
  <li>Android: 2.1�ȏ�</li>
  <li>Opera Mobile: 11�ȏ�</li>
  <li>Safari Mobile: iOS 3.2�ȏ�</li>
  </ul>

  <a href="#user-content-index">�ڎ��֖߂�</a>

## <a name="Fgp_LocalStorage">Fgp_LocalStorage
-----------------------------------

  !!window.localStorage �Ƃ����R�[�hSnippet�����s�������ʂ������Atrue�܂���false�Ƃ���������ƂȂ�.
  ���̃u���E�U��localStorage�@�\�����݂��邩�ۂ�������.
  �u���E�U�̑Ή��󋵂͈ȉ����ڈ��ł���(MDN�ɂ��).

  <ul>
  <li>Chrome: 4�ȏ�</li>
  <li>Firefox(Gecko): 3.5�ȏ�</li>
  <li>IE: 8�ȏ�</li>
  <li>Opera: 10.50�ȏ�</li>
  <li>Safari(Webkit): 4�ȏ�</li>
  <li>Android: 2.1�ȏ�</li>
  <li>Opera Mobile: 11�ȏ�</li>
  <li>Safari Mobile: iOS 3.2�ȏ�</li>
  </ul>

  <a href="#user-content-index">�ڎ��֖߂�</a>

## <a name="Fgp_IndexedDB">Fgp_IndexedDB
-----------------------------------

  !!window.indexedDB �Ƃ����R�[�hSnippet�����s�������ʂ������Atrue�܂���false�Ƃ���������ƂȂ�.
  ���̃u���E�U��indexedDB�@�\�����݂��邩�ۂ�������.
  �u���E�U�̑Ή��󋵂͈ȉ����ڈ��ł���(MDN�ɂ��).

  <ul>
  <li>Chrome: 23�ȏ�</li>
  <li>Firefox(Gecko): 10�ȏ�</li>
  <li>IE: 10�ȏ�</li>
  <li>Opera: 15�ȏ�</li>
  <li>Safari(Webkit): 7.1�ȏ�</li>
  <li>Edge: OK</li>
  <li>Android: 4.4�ȏ�</li>
  <li>Firefox Mobile: 22�ȏ�</li>
  <li>Firefox OS: 1.0.1�ȏ�</li>
  <li>Opera Mobile: 22�ȏ�</li>
  <li>Safari Mobile: 8�ȏ�</li>
  </ul>

  <a href="#user-content-index">�ڎ��֖߂�</a>

## <a name="Fgp_BodyAddBehavior">Fgp_BodyAddBehavior
-----------------------------------

  typeof(document.body.addBehavior) �Ƃ����R�[�hSnippet�����s�������ʂ������Afunction�܂���object�܂���undefined�Ƃ���������ƂȂ�.
  ���̃u���E�U��document.body.addBehavior���\�b�h�����݂��邩�ۂ�������.
  ����͂����炭IE10�ȏ�ɂ������݂��Ȃ�.

  <ul>
  <li>IE10�ȏ�: function��Ԃ�.</li>
  <li>IE9�ȉ�: object��Ԃ�.</li>
  <li>���̑�: undefined��Ԃ�.</li>
  </ul>

  <a href="#user-content-index">�ڎ��֖߂�</a>

## <a name="Fgp_OpenDatabase">Fgp_OpenDatabase
-----------------------------------

  typeof(window.openDatabase) �Ƃ����R�[�hSnippet�����s�������ʂ������Afunction�܂���object�܂���undefined�Ƃ���������ƂȂ�.
  ���̃u���E�U��HTML5 WebSQL Database���T�|�[�g����Ă��邩�ۂ�������.

  <ul>
  <li>Chrome: 8�ȏォ?</li>
  <li>Firefox(Gecko): ���T�|�[�g(undefined��Ԃ�)</li>
  <li>IE: ���T�|�[�g(undefined��Ԃ�)</li>
  <li>Opera: 10.6�ȏォ?</li>
  <li>Safari: 3.1�ȏ�</li>
  <li>Android: 2.1�ȏ�</li>
  </ul>

  <a href="#user-content-index">�ڎ��֖߂�</a>

## <a name="Fgp_CpuClass">Fgp_CpuClass
-----------------------------------

  navigator.cpuClass ���擾����邱�̃u���E�U�̓��삵�Ă���}�V����CPU��������������ł���.
  x86�APPC �Ȃǂ�Ԃ��ƌ����Ă��邪�A���ۂ̂Ƃ���قƂ�ǂ̃P�[�X�ŋ�l�ƂȂ��Ă���.

  <a href="#user-content-index">�ڎ��֖߂�</a>

## <a name="Fgp_Platform">Fgp_Platform
-----------------------------------

  navigator.platform ���擾����邱�̃u���E�U�̓��삵�Ă���OS��������������ł���.
  �����悻�ȉ����ڈ��ł���.

  <ul>
  <li>Windows32/WOW : Win32</li>
  <li>Windows64     : Win64</li>
  <li>Linux32   : Linux i686</li>
  <li>Linux64   : Linux x86_64</li>
  <li>android   : Linux armv7l</li>
  <li>iPhone    : iPhone</li>
  <li>Mac       : MacIntel</li>
  </ul>

  <a href="#user-content-index">�ڎ��֖߂�</a>

