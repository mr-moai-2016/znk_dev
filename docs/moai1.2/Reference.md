# Moai ���t�@�����X�}�j���A��
-----------------------------------

## <a name="index">�ڎ�
-----------------------------------
* [�^�[�Q�b�g�Ƃ́H    ](#user-content-target)
* [��M�t�B���^�ɂ���](#user-content-filter_recv)
* [���M�t�B���^�ɂ���](#user-content-filter_send)
* [���p�ȃz�X�g�ւ̐ڑ����u���b�N����(ignore_hosts�@�\)](#user-content-ignore_hosts) 
* [POST���̊m�F���b�Z�[�W�\��(post_confirm�@�\)](#user-content-post_confirm)
* [���̃}�V������̐ڑ�������/��������](#user-content-acceptable_host)
* [�O���v���L�V���g�������ꍇ�ǂ�����̂��H](#user-content-proxy1)
* [�O���v���L�V�̓K�p���ꕔ�̃T�C�g�݂̂Ɍ��肷��](#user-content-proxy2)
* [���̑��̃��[�J���v���L�V�Ƀ`�F�[������ꍇ](#user-content-proxy3)
* [Moai Web Configuration](#user-content-web_config)
* [�v���O�C���@�\�ɂ���](#user-content-plugin)


## <a name="target"></a>�^�[�Q�b�g�Ƃ́H
-----------------------------------
�^�[�Q�b�g�Ƃ́A��̒Z���L�[���[�h(�^�[�Q�b�g��)�ɂ��A�������̃z�X�g�̏W�����������߂̂��̂ł���.
��̃^�[�Q�b�g�ɂ͂������̃z�X�g����񋓂��āA���̃^�[�Q�b�g���ɂ��O���[�v��������`�ɂȂ�.
�ȉ��̗�����Ă݂悤.

**�y��z**  
�Ⴆ�Όf���u�Q�����˂�v��ΏۂƂ��āA�����̃t�B���^��K�p�������Ƃ��悤.
����ɕʂ̌f���u�ӂ��΂����˂�v��ΏۂƂ��āA�܂��ʂ̃t�B���^�@�\��K�p�������Ƃ���.
target.myf ���ňȉ��̂悤��2ch��futaba����target�����`��(����target���̓��[�U�����R�Ɍ��߂邱�Ƃ��ł���)�A
���ꂼ��Ɂu�Q�����˂�v�Ɓu�ӂ��΂����˂�v�̃z�X�g����񋓂��Ă���.

~~~
@@L 2ch
*.2ch.net
*.bbspink.com
@@.

@@L futaba
*.2chan.net
@@.
~~~

����ɂ��A���̑��̉ӏ��ł͒P�� "2ch" �� "futaba" �Ƃ����L�[���[�h�ŁA��L�̃z�X�g�̏W�����������Ƃ��ł���.
���Ȃ݂ɂ����ł̃p�^�[���̋L�q�ɂ����Ă͈�s�ɂ���ӏ��݂̂Ƀ��C���h�J�[�h���g�����Ƃ��ł���.
�����E�����邪�A�Ⴆ�Έȉ��̋L�q��OK�ł���A�A�X�^���X�N�̕����͔C�ӂ̕�����ƍl���Ă悢.
�Ⴆ�� may.2chan.net�Ajun.2chan.net �Ȃǂ�����Ƀ}�b�`����.

~~~
*.2chan.net
~~~

����A�Ⴆ�Έȉ��̂悤�Ɉ�s�ɂ��A�X�^���X�N���Q�ӏ��ȏ゠��L�q�͈Ӑ}�����ʂ�̂��̂Ƃ͂Ȃ�Ȃ�.
~~~
*.2chan.*
~~~
���̏ꍇ�A��ԖڂɌ��ꂽ�A�X�^���X�N�݂̂����C���h�J�[�h�Ƃ��Ĉ����A
��ԖڂɌ��ꂽ�A�X�^���X�N�́A�����ʂ�A�X�^���X�N���̂��̂Ƃ��Ĉ�����.

myf�Ƃ����t�@�C���͂��̃v���W�F�N�g�S�ʂɂ����Đݒ�t�@�C���Ȃǂ��L�q����̂ɗp����ėp�̃t�H�[�}�b�g�ł���.
����̎d�l�ɂ��Ēm�肽������[myf_spec][1]���Q�Ƃ��Ă�����������.

  <a href="#user-content-index">�ڎ��֖߂�</a>


## <a name="filter_recv"></a>��M�t�B���^�ɂ���
-----------------------------------
Moai�ł́AHTTP�ɂ�����GET�ɂĎ�M�����HTML��Javascript��CSS�ɂ�����
���̎�M����������R�ɒu��(replace)������A���H�ł���t�B���^�[�@�\������Ă���.  
������w�肵�Ă���̂��Afilters/**TARGET_NAME**_recv.myf �ɂȂ�.
**TARGET_NAME**�̕����ɂ�target.myf�ɂ����Ē�`�����^�[�Q�b�g��������.


#### html_filter, js_filter, css_filter �Z�N�V���� 
���̃t�@�C������ html_filter�Ajs_filter�Acss_filter�Ƃ���������
���ꂼ��HTML�AJavascript�ACSS�ɂ����镶����̒u�����s�����Ƃ��ł���.
���̕����ɂ�filters�R�}���h�ƌĂ΂�閽�ߗ���L�q����.
���݃T�|�[�g����Ă���R�}���h��replace�݂̂ŁA�ȉ��̌`���ŋL�q����.

~~~
    replace ['�u���O�̕�����'] ['�u����̕�����']
~~~

����́u�O�҂���҂Œu��(replace)����v�Ƃ������߂ɂȂ�.
�����html_filter���ŏ����Ă����΁A**TARGET_NAME**�Ŏw�肳�ꂽ�z�X�g�ɂ�����
�g���q��HTML�̃t�@�C���̂��ׂĂ̍s�ɑ΂��A���̕�����u�����s����.
js_filter�Acss_filter�ɂ��Ă����l�ł���.

�u���O�̕�����ƒu����̕������
�K�� [' �� '] �Ƃ����N�H�[�e�B���O�L���ň͂��K�v�����邱�Ƃɒ��ӂ��邱��.
���̗p�r�ł́A���̃N�H�[�e�B���O�L���Ŗ��͋N����Ȃ��Ǝv���邪�A
�u���Ώە�������ɂ��̋L�����܂܂��ȂǂŁA���̋L�����Ɠs���������ꍇ�́A
�t�@�C���̍ŏ��̍s�ɂ��� @def_quote �f�B���N�e�B�u�ł��̋L�������R�ɕύX���邱�Ƃ��ł���.
�Ⴆ�� [' �� '] �̑ւ��� -[ �� ]- ���g�������Ƃ������ꍇ�́Amyf�t�@�C���̈�ԍŏ��̍s��
~~~
    @def_quote -[ ]-
~~~
�Ə����Ă���( -[ �� ]- �̊Ԃɂ͕K���X�y�[�X�����邱�� ).


**�y��z**  
�Ⴆ�Όf���u�Q�����˂�v��ΏۂƂ��āA���̎�MHTML��CSS�����H�������Ƃ��悤.
target.myf���ŗႦ��2ch�Ƃ������O��target���`���Afilters/2ch_recv.myf�Ƃ����t�@�C�����쐬����.
���̃t�@�C�����ňȉ��̃t�B���^�R�}���h���w�肷��.

~~~
@@L html_filter
replace ['<b>�f�t�H���g�̖���������</b>'] ['<b>�f�t�H���g�̃��A�C����</b>']
replace ['">�f�t�H���g�̖���������</a>'] ['">�f�t�H���g�̃��A�C����</a>']
@@.

@@L css_filter
replace ['{background:rgb(239,239,239)}'] ['{background:rgb(240,224,214); color:rgb(127,0,0)}']
@@.
~~~

�t�@�C�����쐬�ł�����AMoai���N��(�ċN��)���A���̏�Ԃŕ��ʂɁu�Q�����˂�v�ւƃA�N�Z�X����.
�ȉ��͂��̕\�����ʂł���.  

![screenshot](../imgs/screenshot_recv.png)

HTML�̓��e�����H����āA���O���̕����񂪕ύX����Ă��邱�Ƃ��킩��.
�܂�CSS�̓��e�����H����āA�w�i�F�ƕ����F���ύX����Ă���.  
�������������ݒ肵���ɂ�������炸�\�����ς��Ȃ��Ȃ�A�u���E�U�̃L���b�V���ɌÂ���񂪎c���Ă���
�\��������. �L���b�V�����N���A���Ă��������x�ړI�̃T�C�g�փA�N�Z�X���悤.


#### css_additional �Z�N�V���� 
���̃t�@�C������ css_additional �Ƃ��������ŁA�����s�ɓn�郆�[�U�Ǝ���css�̋L�q��ǉ����邱�Ƃ��ł���.
CSS�̒m����������́A�T�C�g�ɂ���Ďw�肵�Ă���X�^�C�������̕����ɂ��㏑������Ȃǂ��邱�Ƃ�
�Ǝ��̃y�[�W�f�U�C���ɃJ�X�^�}�C�Y����ȂǂƂ��������Ƃ��\��.


  <a href="#user-content-index">�ڎ��֖߂�</a>


## <a name="filter_send"></a>���M�t�B���^�ɂ���
-----------------------------------
Moai�ł́AHTTP�ɂ�����POST�ɂđ��M�����w�b�_��POST�ϐ��A�N�b�L�[�̒l�ɂ�����
���̒l�̓��e��������x���R�ɒu��(replace)�ł���t�B���^�[�@�\������Ă���.  
������w�肵�Ă���̂��Afilters/**TARGET_NAME**_send.myf �ɂȂ�.
**TARGET_NAME**�̕����ɂ�target.myf�ɂ����Ē�`�����^�[�Q�b�g��������.

���̃t�@�C������ header_vars�Apost_vars�Acookie_vars�Acookie_force�Ƃ���������
���ꂼ��HTTP�w�b�_�APOST�ϐ��ACookie�ɂ�����l�̒u�����s�����Ƃ��ł���.
�����ɂ͈ȉ��̕ϐ�����`�����L�q����.

~~~
    varname = ['�u����̒l']
~~~

> ���̂悤��Moai�ł́A����POST�ϐ��̃t�B���^�����O���r�I�ȈՂɎ����ł���.
> ����Ɋւ��Ă͓���̃c�[���ł���Proxomitron�Ȃǂɔ�ׂėL���ȓ_�̈�ł���.

�����ɂ����Ďw�肳��Ă��Ȃ��ϐ��Ɋւ��Ă͉������H�C���͂��ꂸ�A�u���E�U�ɂ����Đݒ肳�ꂽ��Ԃ��P�ɂ��̂܂ܑ�����.
�܂��E�ӂ̒u����̒l����l�̂Ƃ��́A�����ʂ��l�ւƒu�������.
(������cookie_vars ����� cookie_force �ɂ�����ϐ�����l�̏ꍇ�́A���̃N�b�L�[�ϐ������݂��Ȃ����ƂƓ����ł���)

�܂����ԏ����̂��߁A���ۂɑ��M�����POST�ϐ��ɑ��݂��Ȃ��ϐ��Ȃǂ��L�q���Ă������Ƃ��ł���.
���̏ꍇ�A�t�B���^�����ɂ����Ă��̕ϐ��͒P�ɖ��������.

�ȉ��ɂ��ꂼ��Ɋւ���ڍׂ��q�ׂ�.


#### header_vars(HTTP�w�b�_�̒l�̏C��)  
��ʂ�HTTP�ɂ����ăT�C�g�փA�N�Z�X����ꍇ�A���M���M�����f�[�^�{�̂̒��O��
HTTP�w�b�_�ƌĂ΂����̂��t������ĒʐM�����.
�Ⴆ�ΒP���Ƀu���E�U���� http://may.2chan.net/b/res/77777777.htm ��URL�w�肵����A
���̃y�[�W�Łu�ēǂݍ��݁v�{�^���Ȃǂ��������ꍇ�́A�T�O�I�ɂ͈ȉ��̂悤�ȃw�b�_��������
�i���ۂ͂����ƕ��G�ł��邩������Ȃ�).

~~~
GET /b/res/77777777.htm HTTP/1.1
Host: may.2chan.net
User-Agent: My Sexy Browser
Accept: */*
Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8
Accept-Language: ja,en-US;q=0.7,en;q=0.3
Accept-Encoding: gzip, deflate
Referer: may.2chan.net/b/res/77777777.htm
Cookie: posttime=1464336000000; uuc=1
Connection: keep-alive

~~~

���邢�� http://may.2chan.net/b/futaba.php?guid=on �����Đ�ƂȂ��Ă���悤�Ȍf����
���X���悤�Ƃ����ꍇ�A�T�O�I�ɂ͈ȉ��̂悤�ȃw�b�_��������i���ۂ͂����ƕ��G�ł��邩������Ȃ�).
~~~
POST /b/futaba.php?guid=on HTTP/1.1
Host: may.2chan.net
User-Agent: Mozilla/5.0 Gecko/20041122 Firefox/1.0
Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8
Accept-Language: ja,en-US;q=0.7,en;q=0.3
Accept-Encoding: gzip, deflate
Referer: http://may.2chan.net/b/res/77777777.htm
Cookie: posttime=1464336000000; uuc=1
Connection: keep-alive
Content-Type: multipart/form-data; boundary=---------------------------134281275020820
Content-Length: 2050

(�ȉ��AContent-Length�Ŏw�肳�ꂽ�o�C�g(�����ł�2050�o�C�g)��POST�f�[�^�{��(�����HTTP��Body���ƌĂ�)������)
~~~

Host ���Ƃ�User-Agent ���Ƃ� **�u:�v �L��**�̑O�ɂ��镔����HTTP�w�b�_�[�̕ϐ����ƂȂ�A
�������ɂ��镔�������̒l���Ӗ�����.
��L��肨������Ղ���Ǝv�����A������User-Agent�̒l��HTTP�w�b�_�[�̕ϐ��̈��Ƃ��Ċi�[�����`�ɂȂ�.
�]���āA���̃w�b�_�[�ϐ��̒l���C�����邱�ƂŁA�T�C�g�ɑ�����User-Agent���U�����邱�Ƃ��ł���.
������s���ɂ́Aheader_vars�ɂ����Ĉȉ��̂悤�ɋL�q����.

~~~
@@V header_vars
User-Agent = ['My Sexy Browser']
@@.
~~~
����ɂ����User-Agent�̕ϐ��l���{���̒l�ł��� Mozilla/5.0 Gecko/20041122 Firefox/1.0 ����
My Sexy Browser�Ƃ����l�ɏ���������ꂽ�`�ŃT�C�g�֑��M�����`�ƂȂ�.
�ȏオUser-Agent�U���̏ڂ������J�j�Y���ƂȂ�.

####  �Q�l
> ��M�t�B���^�ɂ����� header_vars ����User-Agent�s���폜���邱�Ƃɂ���āA**������Moai�ɂ�����User-Agent�U�����s�킹�Ȃ�**�悤�ɂ��邱�Ƃ��ł���.
> �Ⴆ�΁A�u���E�U��User-Agent�U���A�h�I���Ȃǂɂ���Ċ���User-Agent��C�ӂ̒l�ɋU�����Ă���ꍇ�Ȃ�
> ���̋U���l��Moai�ōďC�����邱�ƂȂ����̂܂ܑ��M���ė~�������Ƃ�����.
> �����Moai��Javascript�ɂ�����navigator.userAgent�̒l��HTTPS�ɂ���ĈÍ������ꂽHTTP�w�b�_�ɂ�����User-Agent�܂ł�
> �C���ł��Ȃ��̂ŁA�ꍇ�ɂ���Ă͂��̂悤�ȃA�h�I�����g�������m���Ȃ��Ƃ����邩������Ȃ�.
> ���ʂ�Moai�ɂ����User-Agent���U��������悤�ɂ���ɂ́AUser-Agent�s��ǉ�����΂悢(���̂Ƃ��̉E�Ӓl�͓K���Ȃ��̂ł悢).

���AUser-Agent�ȊO�̂��̑��̃w�b�_�[�ϐ��̒l�����l�ɕύX���邱�Ƃ��ł��邪�ʏ킻�̕K�v�͂Ȃ���A
�s�p�ӂɕς���ƒʐM���̂��̂��s�\�ɂȂ鋰�ꂪ����̂ŁA�[���m���̂�����ȊO�͘M��Ȃ�����.


#### post_vars(POST�ϐ��̒l�̏C��)  
���X���\�Ȍf����HTML�̃\�[�X�Ȃǂ��J���Ă��炤�ƁA�܂��ȉ��̂悤�� form�^�O�ň͂܂ꂽ�̈悪����.

~~~
<form action=... >
...
</form>

~~~

����ɂ��̕������悭�ώ@����ƈȉ��̂悤��input�^�O��������������Ă��邱�Ƃ��킩��.
~~~
<input type=�c name=... value=... >
<input type=�c name=... value=... >
...

~~~
����**input�^�O�ɏ����ꂽ���e**���ŏI�I�ɂ�POST�ϐ��ƂȂ�APOST���s���ꍇ(���X���M���Ȃ�)�ɑ��M�����f�[�^�̖{�̂ɑ�������.
���ۂ�HTTP�ʐM�ɂ�����POST�ϐ���HTTP Body���̃f�[�^�Ƃ���HTTP�w�b�_�[�����ɔz�u�����.

�ȉ��ɂ��Ȃ��̃g���[�j���O�̂��߁A�������̗����o���Ă�����.

**�y�g���[�j���O�p��� Lv1�z**  
�Ⴆ�΁A�f����HTML�̃\�[�X�Ɉȉ��̂悤�ȕ������܂܂�Ă����Ƃ��悤.

~~~html
<form action="http://www.example.net/bbs.php" method="POST" enctype="multipart/form-data">

<input type=hidden name="himitu_no_data" value="12345678">
<input type=hidden name="thread"         value="1000">

<b>�R�����g</b><textarea name="comment" cols="48" rows="4"></textarea><br><br>

<b>�Y�tFile</b><input type=file name="upload_file" size="35"><br><br>

<input type=checkbox name="text_only" value=on>�摜�Ȃ�

</form>
~~~

��������ۂɃu���E�U�ŊJ���ƁA�Ⴆ��Firefox�Ȃǂł͈ȉ��̂悤�ɕ\�������͂��ł���.

![screenshot](../imgs/post_example_01.png)

�܂��A�ȉ��ɂ���HTML�̈Ӗ��ɂ��ăq���g��񋓂��Ă���.

<ul>
<li>���X�̑��M���form��action�̒l�ɋL�q����Ă���A���̗Ⴞ�ƁAhttp://www.example.net/bbs.php �ƂȂ�.</li>
<li>method�Aenctype�̕����͂Ƃ肠�����C�ɂ��Ȃ��Ă悢(���̂�����̎w���Moai���K�؂Ɏ��v�炤).</li>
<li>input�^�O����type=hidden�Ƃ����w��́A���ꂪ���ۂ̉�ʂɂ͕\������Ȃ��B���ꂽPOST�ϐ��ł��邱�Ƃ��Ӗ�����.
    ���̗�ł� himitu_no_data �� thread ���Y������.</li>
<li>textarea�^�O����comment�������񃌃X�̓��e�ƂȂ�A��ʏ�ł̓e�L�X�g���̓t�H�[���ɑ�������.
    input�^�O�ł͂Ȃ����A����Ƃ��Ă����POST�ϐ��ƂȂ�.</li>
<li>input�^�O����type=file�Ƃ����w��́A��ʏ�ł͓Y�t�t�@�C���p�̃_�C�A���O���o�����߂̃{�^���ɑ�������.
    ���̗�ł�upload_file�Ƃ������O��POST�ϐ��ƂȂ�A���̒l�͓Y�t�t�@�C���̑S���e�ł���.
	(���A���̍ۂɓY�t�t�@�C���̃t�@�C�������A���̃t���p�X���������ꂽ�`�ŕt�������)</li>
<li>input�^�O����type=checkbox�Ƃ����w��́A��ʏ�ł͕����ʂ�`�F�b�N�{�b�N�X�ɑ�������.
    ���̃`�F�b�N�{�b�N�X�Ƀ`�F�b�N����ꂽ�ꍇ�̂�POST�ϐ�text_only���t�������.
	(�`�F�b�N�����ĂȂ��ꍇ�͂��̕ϐ��͕t������Ȃ�).
</ul>

�܂Ƃ߂�ƁA���̃t�H�[���ɂ���đ����邷�ׂĂ�POST�ϐ��͈ȉ��ƂȂ�A
����炪�Ahttp://www.example.net/bbs.php �ɑ��M�����.

~~~
 �ϐ��� : himitu_no_data   �l : 12345678
 �ϐ��� : thread           �l : 1000
 �ϐ��� : comment          �l : ���[�U���e�L�X�g�t�H�[�������͂����R�����g������
 �ϐ��� : upload_file      �l : �Y�t�t�@�C���̑S���e(�t�@�C����Y�t���Ă��Ȃ��ꍇ�͋�l�ƂȂ�)
 �ϐ��� : text_only        �l : on(�������`�F�b�N�����Ă��Ȃ��ꍇ�͂��̕ϐ����̂��̂����݂��Ȃ�)
~~~

���āA�O�u���������Ȃ������A���̌f����example�ł�himitu_no_data�Ƃ���POST�ϐ��ɂ��A
���[�U�̎��ʂ��s���Ă�����̂Ƃ��悤.
**Moai�ɂ���Ă��̒l��K���Ȓl(�Ⴆ��07210721)�ɂł����グ��I�I**

�y��� Lv1�̉𓚁z  

�܂�target.myf���ŗႦ��example�Ƃ������O��target���`����.
���̃t�@�C���Ɉȉ��̂悤�Ȏw�������������΂悢���낤.

~~~
@@L example
www.example.net
@@.
~~~

����filters/example_send.myf�Ƃ����t�@�C�����쐬���A���̓��e���ȉ��̂悤�ɂ���.
~~~
@def_quote [' ']

@@V header_vars
@@.

@@V post_vars
himitu_no_data = ['07210721']
@@.

@@V cookie_vars
@@.

@@V cookie_force
@@.
~~~

�Ō�Ɋ���Moai���N�����Ă���ꍇ��target.myf�ƃt�B���^�t�@�C��filters/example_send.myf�������[�h���Ȃ���΂Ȃ�Ȃ�.
��xMoai���I�����Ă����������x�N�����邩�A���邢��Web Configuration��ʂŁuRestart Moai�v�{�^���������΂悢.

�ȏ�ł���.


#### cookie_vars ����� cookie_force (Cookie�̒l�̏C��)  
Cookie�ɂ��Ă͒m���Ă�����������ł��낤.
�u���E�U�̐ݒ��ʂ�����ȒP�Ɋm�F�ł��邵�A���̓��e����������̂����l�ɊȒP�ł���.
��ʂɂ��̒l�̓T�C�g���ɕʂɋL�^�����.

�Ⴆ�� www.example.net �� www.moai-chan.net �Ƃ�����̈قȂ�T�C�g���������Ƃ��āA
�����͕ʂ̃J�e�S���Ƃ��ĕ������ċL�^�����. ���̂Ƃ�www.example.net�����www.moai-chan.net��
�J�e�S���ɂ���Cookie�̒l�����邱�Ƃ��X�V���邱�Ƃ��ł��Ȃ����A���̋t���R��ł���.

�܂��A�ӂ��΂����˂�Ȃǂł́A2chan.net�Ƃ����ӂ��ΑS�̂Ŏg����Cookie �� may.2chan.net
�Ƃ����悤��may�����Ŏg����Cookie�̂Q���A���݂͋L�^�����悤�ł���.

> �Ƃ���Ŋ��̂悢���͋C�Â��ꂽ��������Ȃ����A��LHTTP�w�b�_�[�ϐ��̉���ɂ�����Cookie�Ƃ������O�̕ϐ�������Ă���.
> ���̂Ƃ���ACookie�f�[�^�͍ŏI�I�ɂ�HTTP�w�b�_����Cookie�ϐ��Ƃ��đ��M���s���Ă���A
> �܂��L�Ő�������header_vars�ɂ����Cookie�ϐ����C�����邱�Ƃ��ʓ|�����s�\�ł͂Ȃ�.
> �܂�����AHTTP�w�b�_�ϐ�Set-Cookie�ɂ���āACookie�f�[�^�̐V�K�쐬��X�V�Ȃǂ��s���A
> ��������l�̍l�����ŏC�����邱�Ƃ��ł���.
> �������A�w�b�_�ϐ�Cookie�܂���Set-Cookie�̒l�͕��G�ȃt�H�[�}�b�g�������Ă���A
> ����header_vars�ɂ���Ă��̒l���C������ƂȂ�΁A���̃t�H�[�}�b�g�����͂ŉ��߂��Ȃ���΂Ȃ�Ȃ�.
> cookie_vars ����� cookie_force ��p����΂��̍�Ƃ͕s�v�ƂȂ�A�ȈՂ�Cookie�l�̉��H���\�ƂȂ�.

cookie_vars �ɂ����Ďw�肳�ꂽ�ϐ��́A�u���E�U�ɂ�����^�̒l���㏑������.
������target�ƂȂ�T�C�g��Set-Cookie�ɂ���ĐV����Cookie�l��ݒ肵�ė����ꍇ�A
cookie_vars�̕ϐ��l�͂��̒l�Ŏ����I�ɏ�����������. 
���邢��cookie_vars�ɂ܂����̕ϐ������݂��Ȃ��ꍇ�͎����I�ɐV�K�ǉ������.
���̕ӂ��header_vars��post_vars�ɔ�ׁA��╡�G�Ȏd�l�ł���.
�������A**�T�C�g��Set-Cookie�𔭍s����܂ł̊ԁA������Cookie�ϐ��̒l���ꎞ�I�ɋ�ɂ��Ă��������P�[�X**�Ƃ������̂����\����A
���̎d�l�Ȃ炻�ꂪ���R�Ɏ����ł���̂ł���.

����Acookie_force �ɂ����Ďw�肳�ꂽ�ϐ��́A�u���E�U�ɂ�����^�̒l���㏑�����A
�����cookie_vars�ɂ�����w�肳���������I�ɏ㏑������.
�܂��Acookie_force�ɂ�����w���cookie_vars�ƈقȂ�A��{�I�ɏ����������邱�Ƃ͂Ȃ�.
�Ⴆ�΂���Cookie�ϐ���ⓚ���p�ŏ�ɍ폜���Ă��������ꍇ�́Acookie_force�ɂ����Ă��̕ϐ�����l�Ɏw�肵�Ă����΂悢.


�܂Ƃ߂�ƁA�T�O�I��Cookie�̒l�͈ȉ��̂悤�ȏ��ԂŃt�B���^�����O����A�T�C�g�ւƗ���Ă���.

~~~
 �u���E�U�ɂ�����^��Cookie�l => cookie_vars�ɂ��ύX => cookie_force�ɂ�鋭���ύX => �ړI�̃T�C�g�֑��M

~~~

**�y�g���[�j���O�p��� Lv2�z**  
�Ⴆ�΁A�f����moai-chan�Ƃ����ˋ�̌f�����l���悤.
���̌f���ɂ� mei.moai-chan.net �� imoge.moai-chan.net�Ƃ�����̃T�[�o�����݂��A
������HTML�̃\�[�X�͂�������ȉ��̂悤�ȕ������܂܂�Ă�����̂Ƃ���.

~~~html
<head>
<base href="http://mei.moai-chan.net/">
</head>

<form action="moai.php" method="POST">

<input type=hidden name="entry_time"     value="2016/06/01/12:05:25">
<input type=hidden name="secret_of_mana" value="192837465">

<b>���Ȃ܂�</b><input type=text name="your_name" size="28"><br><br>
<b>���[����</b><input type=text name="your_mail" size="28"><br><br>
<b>�R�����g</b><textarea name="comment" cols="48" rows="4"></textarea><br><br>

<b>�Y�tFile</b><input type=file name="upload_file" size="35"><br><br>

<input type=checkbox name="text_only" value=on>�摜�Ȃ�

</form>
~~~

�Q�ڂ̗��Ȃ̂ŁA����͂�����x�ȗ����悤.
���Ȃ݂ɂ��̗�͑啪�ӂ��΂̏󋵂ɋ߂�(��).

�ȉ��ɊȒP�ɂ���HTML�Ɋւ���q���g��񋓂��Ă���.

<ul>
<li>���̗�ł�form�^�O��action�ł�URL���t���p�X�ŋL�q����Ă��Ȃ����A����͂������base�^�O�ɂ����
    ���̎w�����ɗ���ׂ��p�X���w�肵�Ă��邽�߂ł���.
	���A�����ł�base�^�O�̒l�́Amei.moai-chan.net �����̂��̂ƂȂ��Ă��邪�A
	������̃T�[�oimoge.moai-chan.net�̏ꍇ�͓��l�ɂ�������������̒l�ɂȂ��Ă���ƍl����΂悢.</li>
<li>���̗�ł�form�^�O����enctype���w�肳��Ă��Ȃ����A���̏ꍇapplication/x-www-form-urlencoded���w�肳�ꂽ�̂Ɠ����ɂȂ�.
    ������ɂ��悱��Ɋւ��ċC�ɂ���K�v�͂Ȃ�.</li>
<li>input�^�O����type=text�́A��ʏ�ł́u���Ȃ܂��v��u���[�����v�Ȃǂ̈�s��������̓t�H�[���ɑ�������.
    �ܘ_�����POST�ϐ��ƂȂ�.</li>
</ul>

���āA���̉ˋ�̌f����moai-chan�ł�secret_of_mana�ɂ����Ă��Ȃ��̃}�V������Fingerprint�l��ݒ肵�Ă�����̂Ƃ��悤.
Fingerprint�Ƃ̓}�V���ŗL�̐F�X�ȏ��������W�߂đ������킹�A������n�b�V���l�Ƃ������̂ł���.
�܂��A���Ȃ����n�߂Ă��̌f���ɓ��ꂵ���ꍇ�Amoai-chan�� cok_entry_time �Ƃ���Cookie�ϐ��������
���̎�����ݒ肷����̂Ƃ���.
�����Ĉȍ~��POST�ł́AJavascript�ɂ�� cok_entry_time�̒l���Q�Ƃ��� POST�ϐ�entry_time�ɂ��S�������l��ݒ肵�Ă�����̂Ƃ��悤.
moai-chan�ł͂����������ă��[�U�̎��ʂ��s���Ă�����̂Ƃ���.

**Moai�ɂ���Ă܂�Cookie�ϐ�cok_entry_time���폜���A�����������߂Ă��̌f���ɓ��ꂵ�����̂悤�ȏ󋵂��V�~�����[�g����I�I**
**���ɂ��Ȃ��̃}�V����������secret_of_mana��K���Ȓl�ɂł����グ��I�I**


�y��� Lv2�̉𓚁z  

�܂�target.myf���ŗႦ��moai-chan�Ƃ������O��target���`����.
���̃t�@�C���Ɉȉ��̂悤�Ȏw�������������΂悢���낤.

~~~
@@L moai-chan
mei.moai-chan.net
imoge.moai-chan.net
@@.
~~~

�T�[�o�̐��������ȂǗ񋓂���̂��ʓ|�ȏꍇ�́A�ւ��Ɉȉ��̂悤�ɋL�q���邱�Ƃ��ł���.

~~~
@@L moai-chan
*.moai-chan.net
@@.
~~~

����filters/moai-chan_send.myf�Ƃ����t�@�C�����쐬���A���̓��e���ȉ��̂悤�ɂ���.
~~~
@def_quote [' ']

@@V header_vars
@@.

@@V post_vars
entry_time = ['']
secret_of_mana = ['07210721']
@@.


# Cookie�l�̃t�B���^�����O
# �����ł̎w��̓T�C�g��Set-Cookie�𔭍s�������_�Ŏ����I�ɏ����ς����邱�Ƃ�����.
@@V cookie_vars
cok_entry_time = ['']
@@.

# Cookie�l�̋����ύX
# �����ł̎w��̓T�C�g��Set-Cookie�𔭍s���Ă������ς����邱�Ƃ͂Ȃ�.
@@V cookie_force
@@.
~~~

cok_entry_time����l�ɐݒ肷�邱�Ƃɂ��Amoai-chan.net������ɐV���Ȓl�𖾎��I�ɐݒ肷��܂ł̊��Ԃ�
����Cookie�̒l�͑��݂��Ă��Ȃ��̂Ɠ����ɂȂ�.
(cookie_vars�ɐݒ肵�Ă���ϐ��Ɋւ��āAmoai-chan.net����̓u���E�U�Ɏ��ۂɐݒ肳��Ă���^�̒l�͌����Ȃ�.)
�܂��Amoai-chan.net������ɐV���Ȓl�𖾎��I�ɐݒ肵���u�ԁAfilters/moai-chan_send.myf���X�V����A
��Lcookie_vars��cok_entry_time�̒l�͂��̒l�Ɏ����I�ɏ��������.

����������낤���H
���̕ӂ�̏�����Moai�����ׂĂ��܂����v����Ă����.
�܂����̏ꍇ�A�f���Ɏ��ۂ̃u���E�U��Cookie���폜���������������낤���A
�����ł�cookie_vars�t�B���^�̗��K��Ƃ��āA���̂悤�ȕ��@���Љ�Ă���.

����APOST�ϐ�entry_time�ɂ��Ă�moai-chan.net��Javascript������Ƀu���E�U�ɂ���^��Cookie�̒l��ݒ肷��̂�
�Ƃ肠����entry_time����l�ɐݒ肵�Ă���.
�����Łu�Ƃ肠�����v�ƌ������t���g�����̂́A���̂܂܂ł͕s�\���ł��邩�炾.
���cok_entry_time�ɂǂ̂悤�Ȓl���ݒ肳�ꂽ�������āA���̒l���蓮�ŃR�s�y���Ȃ���΂Ȃ�Ȃ�.
(���̕ӂ��plugin��p����΂��̕K�v���Ȃ��Ȃ肳��ɃX�}�[�g�ɍs���邪�A�����ł͂����܂ł͓��ݍ��܂Ȃ�.)

�����Ŋ���Moai���N�����Ă���ꍇ��target.myf�ƃt�B���^�t�@�C��filters/moai-chan_send.myf�������[�h���Ȃ���΂Ȃ�Ȃ�.
��xMoai���I�����Ă����������x�N�����邩�A���邢��Web Configuration��ʂŁuRestart Moai�v�{�^���������΂悢.

�����moai-chan.net�ɂ����������߂ē��ꂷ��̂Ɠ����󋵂����o�������ƂɂȂ�.
�����ň�Umoai-chan.net�ɃA�N�Z�X����ƁAentry_time�̒l����ł���̂ŁA�uCookie�܂���Javascript��L���ɂ��ĉ������v
�Ƃ������|�̃��b�Z�[�W�������炭�\�������͂��ł���.
����Acookie_vars ���� cok_entry_time �ϐ��́A���̎��_�� moai-chan��蔭�s���ꂽ�l�ɏ���������Ă���͂��ł���.
���̒l��entry_time�ɂ��̂܂܃R�s�y����΁A�~�b�V���������ł���.



�Ō�Ɏ��ۂ̓K�p����o���Ă�����.
(plugin�͗p���Ȃ����@�Ȃ̂ňꕔ�ʓ|����)

**�y������z**  
�Ⴆ�Όf���u�ӂ��΂����˂�v��ΏۂƂ��āA���̑��M�w�b�_��POST�ϐ���Cookie�̒l�����H�������Ƃ��悤.
target.myf���ŗႦ��futaba�Ƃ������O��target���`���Afilters/futaba_send.myf�Ƃ����t�@�C�����쐬����.
���̃t�@�C�����ňȉ��̎w����s��.

~~~
@@V header_vars
User-Agent = ['Mozilla/5.0 MyBrowser_MoreMoreFireSexy']
@@.

@@V post_vars
pthb = ['']
pthc = ['']
pthd = ['']
flrv = ['2739407074']
flvv = ['3792482965']
scsz = ['800x600x32']
pwd = ['']
js = ['on']
@@.

# Cookie�l�̃t�B���^�����O
# �����ł̎w��̓T�C�g��Set-Cookie�𔭍s�������_�Ŏ����I�ɏ����ς����邱�Ƃ�����.
@@V cookie_vars
posttime = ['']
pwdc = ['']
cxyl = ['5x3x2x0x2']
@@.

# Cookie�l�̋����ύX
# �����ł̎w��̓T�C�g��Set-Cookie�𔭍s���Ă������ς����邱�Ƃ͂Ȃ�.
@@V cookie_force
namec = ['']
@@.

~~~

���̗�ł́AHTTP�w�b�_�ϐ��̂����AUser-Agent �̒l���uMozilla/5.0 MyBrowser_MoreMoreFireSexy�v�ւƕύX���Ă���.
�܂��Aflrv, flvv, scsz�̒l���K���Ȓl�ɂł��������Ă���.
�t�@�C�����쐬�ł�����AMoai���N��(�ċN��)���A���̏�Ԃŕ��ʂɁu�ӂ��΂����˂�v�̃J�^���O(�ǂ̃J�^���O�ł��悢)���J��.

����̃J�^���O�A�N�Z�X�ɂ����āA��L�̂����Acookie_vars����posttime�̒l���ӂ��΂ɂ���Ĕ��s���ꂽ�l�ɏ��������͂��ł���.
���̌������x filters/futaba_send.myf ���J���Apthb��pthc�̒l��V����posttime�̒l�ɏ��������Ă���.
(pthb��pthc����̂܂܂ɂ��Ă����Ă�������P�[�X�����邩������Ȃ�.
�܂������Ɍ�����pthc�ɐݒ肷�ׂ��l��/bin/cachemt7.php�ɂ���Ĕ��s�����ŐV�̒l�ł���K�v�����邪�A
���̗��K�p���ł͂Ƃ肠���������͑Ë�����)

�ȏ�Ńt�B���^�̏������������ł���.
����ɉ���IP���ύX���Ă����΁A�ȍ~��Moai�N�����͓��ɉ��������Ƃ����ʂɃ��X��X�����Ă��o����悤�ɂȂ��Ă���n�Y��.

���A�����ł͗��K�p���Ƃ��Ă��̂悤�ȕ��@���Љ�����A���ʂ͂���Ȃ��Ƃ����Ȃ��Ă��uVirtual USERS Initiation�v
�{�^���������Έꔭ�ŏ����������ł���.
���̃{�^���ɂ��Moai�ɕt�����Ă���futaba�v���O�C�����Ăяo����A�܂��ɏ�L�̏���������ŏu���ɍs���Ă����.


���ƃI�}�P�Ƃ���Cookie�ϐ��̂����A��L�Ŏw�肵�����̂̒l��ύX���Ă���.
������cxyl�Ƃ����̂́A
~~~
    ���̃X���� x �c�̃X���� x �e�X���ł̕����� x �����ʒu(0:��,1:�E) x �摜�T�C�Y(0����6�܂ł�0���ŏ��ŋ����̕\��)
~~~
�Ƃ����t�H�[�}�b�g�����L�q�q�ł���A������w�肵�Ȃ��ꍇ�Afutaba.php�̓f�t�H���g�Ƃ��� ['14x6x4x0x0'] ���w�肵�����̂Ƃ݂Ȃ��悤�ł���.

�܂�namec�Ƃ����̂́A�Ō�̃��X�ɂ����Ďg�p���������O���̓��e�ł���A
�����ł͂���������I�ɋ�l�ւƃ��Z�b�g(Cookie�ϐ����폜)���Ă���.


  <a href="#user-content-index">�ڎ��֖߂�</a>


## <a name="ignore_hosts"></a>���p�ȃz�X�g�ւ̐ڑ����u���b�N����(ignore_hosts�@�\) 
-----------------------------------
Moai�ł́Aconfig.myf ���� ignore_hosts �ɂ����ċL�ڂ���Ă���z�X�g�փu���E�U���ڑ����悤�Ƃ����ꍇ
������u���b�N���邱�Ƃ��ł���.
�Ⴆ�΂��Ȃ����������Ă���T�C�g���ɑ�ʂ̍L��������A�����̃o�i�[�Ȃǂ��O���T�C�g�ɂ���ꍇ
���̂܂܂ł͂����̊O���T�C�g���ׂĂɑ΂��āuTCP�R�l�N�V�����̊m���v���s��Ȃ���΂Ȃ�Ȃ�.
�uTCP�R�l�N�V�����̊m���v�Ƃ��������͔��ɏd��(���Ԃ�������)�̂ŁA�{���ɕ\���̍�������}�肽���Ȃ�
���ʂȐڑ��𖢑R�ɖh���̂���Ԍ��ʓI�Ȃ̂ł���.

�ȉ��ɗ������.
���Ȃ݂ɂ����ł̃p�^�[���̋L�q�ɂ����Ă�target�ł̎w��Ɠ��l�̌`�Ń��C���h�J�[�h���g�����Ƃ��ł���.

~~~
@@L ignore_hosts
free2.jp
api.digiket.com
js.ad-stir.com
aladdin.genieesspv.jp
*.socdm.com
j.microad.net
www.assoc-amazon.jp
@@.
~~~

config�t�@�C�����ɗႦ�΂��̂悤�ɋL�q���Ă����ƁA���Ȃ����������Ă���T�C�g�̃y�[�W����
��L�̊O���T�C�g�փA�N�Z�X���悤�Ƃ��镔��������ꍇ�A�����̊O���T�C�g�ւ̐ڑ��𖢑R�ɖh�~����.
�����đւ��ɂ��̕����ɂ͈ȉ��̂悤�ȃ��b�Z�[�W���\�������.

  Moai : Ignored Host Blocking[**blocked_hostname**] sock=[1234].

**blocked_hostname**�̕����ɂ́A�ǂ̃z�X�g�ւ̃A�N�Z�X���u���b�N���������\�������.  
�܂��L�Ŏw�肵���z�X�g�̂����ꂩ���\�������͂��ł���.  
sock�̕����͋C�ɂ���K�v�͂Ȃ����A�ڑ��̍ۂɎg�����\�P�b�g�̔ԍ��������Ă���.

  <a href="#user-content-index">�ڎ��֖߂�</a>


## <a name="post_confirm"></a>POST���̊m�F���b�Z�[�W�\��(post_confirm�@�\) 
-----------------------------------
�����ŊȒP�ɂ����炢���悤.
POST�Ƃ͌f���ȂǂփX�����Ă⃌�X���e������ۂɍs����HTTP���N�G�X�g�̂��Ƃł���.
�����Ă��̂Ƃ�HTTP�w�b�_��T�[�o����`�����ϐ�(POST�ϐ��ƌĂ�)�A�N�b�L�[�̒l�Ȃǂ����M�����.

config.myf����post_confirm�̒l�� on �̂Ƃ��A�����̒l�����ׂĊm�F�\�������ʂ��o�����Ƃ��ł���.
(�ȉ��̉摜�̂悤�Ƀe�L�X�g�����̔��ɒn���ȉ�ʂł͂��邪�A���ꂩ�炻�̃T�C�g�ɉ��������邩���]�����ƂȂ��\������A
 �󋵂�c���╪�͂�����ɂ͏\�����ɗ����낤).
���̉�ʂ��o�Ă���i�K�ł́A�܂����e�͍s���Ă��Ȃ�. ����̈�ԉ��ɂ���uSend�v�{�^�����������ƂŁA���̓��e��**���ۂ�**���e�����.

![screenshot](../imgs/screenshot_post_confirm.png)

�������A���̉�ʂ��\�������̂�config.myf���ɂ���post_confirm_hosts�ɋL�ڂ��ꂽ�z�X�g�ɑ΂���POST����ꍇ�݂̂ł���.
(�����ɋL�ڂ���Ă��Ȃ��z�X�g�ɑ΂��Ă�post_confirm��on�ł����Ă����̉�ʂ͕\������Ȃ�).
���Ȃ݂ɂ����ł̃p�^�[���̋L�q�ɂ����Ă�target�ł̎w��Ɠ��l�̌`�Ń��C���h�J�[�h���g�����Ƃ��ł���.

post_confirm_hosts�Ȃǂł킴�킴�͈͂��i�炸�ɂ��ׂẴT�C�g��Ώۂɂ���΂悢�̂ł͂Ȃ����Ǝv����������邾�낤��
Nico����Ȃǈꕔ�̃T�C�g�ł́A�X�N���v�g�ȂǂŎ����I��POST�������ōs���Ă���ꍇ������A
���̂悤�ȏꍇ�����L���ɂ��Ă����Ɩ�肪�������邽�߁A���̉�ʂ���������z�X�g��I���w��ł���悤�ɂ��Ă���.

����Apost_confirm �̒l�� off �̂Ƃ��A���ׂẴT�C�g�ɑ΂��Ă��̊m�F��ʂ͕\���͖����ƂȂ�.

�y���Ӂz  
�Ⴆ�΂ӂ��Ηp�̃A�h�I���ԕ��Ȃǂ��g�p���Ă���ꍇ�A���̃��[�h��on�̏ꍇ�ɓ����������������A
���X���M�Ȃǂ����܂��s���Ȃ��悤�ł���.
�ԕ����L���ȏ�ԂŁA���̖������܂����������@�͍��̂Ƃ���݂����Ă��Ȃ�.
���̏ꍇ����͂�post_confirm���̂��̂𖳌��ɂ��Ă����K�v������.
�����Ƃ����̊m�F��ʕ\���͍��ƂȂ��Ă͉�͗p�Ɏg�p���邭�炢�ł���A���ʂ͕K�v�Ȃ����̂ł��邽�߁A
�o�[�W����1.0���A���̎w��̓f�t�H���g�ł�off�Ƃ��Ă���.
�ǂ����Ă��m�F��ʂ��o�������ꍇ�͐ԕ����ꎞ�I�ɖ����ɂ��ă��X���e���悤.

  <a href="#user-content-index">�ڎ��֖߂�</a>


## <a name="acceptable_host"></a>���̃}�V������̐ڑ�������/��������
-----------------------------------
Moai�͑��̃}�V������̐ڑ��̎�t���T�|�[�g����.

����͂��ƒ�ɂ���Q��ڂR��ڂ̃}�V��������AMoai�̋N�����Ă���P��ڂ̃}�V���֐ڑ�����
Moai�̒񋟂���@�\�𗘗p����Ƃ������悤�ȗp�r��z�肵�Ă���.
�T�O�I�ɂ͈ȉ��̂悤�Ȑڑ��ɂȂ�.

~~~
���̃}�V����ɂ���u���E�U => Moai�̋N�����Ă���}�V����ɂ���Moai => ���M��̃T�C�g
~~~

���̐ڑ��̋���/�s���̖h�q�@�\�Ƃ���Moai�͂Q�i�K�݂��Ă���A����IP����̐ڑ���ʉ߂�����ɂ́A
���̑o����������K�v������.

#### ��P�̒i�K : config.myf����acceptable_host�ɂ�����w��
���̒l��ANY���w�肷�邱�Ƃɂ�葼�̃}�V������̐ڑ����\�ƂȂ�ALOOPBACK���w�肷�邱�Ƃɂ��A
���}�V���ȊO�͖ⓚ���p�ŎՒf����悤�ɂȂ�.
�f�t�H���g�ł͈��S�̂��߁A�ꉞLOOPBACK�Ƃ��Ă���.

#### ��Q�̒i�K : config.myf����access_allow_ips �� access_deny_ips �̎w��
access_allow_ips �ɂ����Đڑ���������IP�Q���w�肷��(�z�X�g���ł͂Ȃ��K��IP�łȂ���΂Ȃ�Ȃ�).
access_allow_ips�ɉ����L�q���Ȃ��ꍇ�́Alocalhost���������ׂẴ}�V������̐ڑ��͕s���ƂȂ�
(���̏ꍇ�A�ڑ����ɂ�Forbidden���b�Z�[�W��Ԃ��`�ɂȂ�).

����access_allow_ips�̎w�肾���ł��ʏ�\���ł��邪�A�������炳��ɗ�O�I�ɐڑ���s�Ƃ���
IP�Q��access_deny_ips�ɂ����Ďw�肷�邱�Ƃ��ł���.

�Ⴆ��LAN������݂̂̐ڑ���������ɂ́A�T�^�I�ɂ͈ȉ��̋L�q�ł悢���낤.
���Ȃ݂Ƀp�^�[���̋L�q�ɂ����Ă͈�s�ɂ���ӏ��݂̂Ƀ��C���h�J�[�h���g�����Ƃ��ł���.

~~~
@@L access_allow_ips
192.168.*
@@.
~~~

����ɗႦ��LAN����192.168.1.66�̃}�V���̂݁A���R�����Đڑ���s���Ƃ������ꍇ�́A
access_allow_ips�Ɉȉ����L�q���Ă����Ƃ悢.

~~~
@@L access_deny_ips
192.168.1.66
@@.
~~~

�ܘ_�A���[�^��OS�Ȃǂɂ���t�@�C�A�E�H�[���@�\��WAN(�O���C���^�[�l�b�g)����̃|�[�g8124�ւ�
�s�v�Ȑڑ���h�~����̂��Z�L�����e�B���{�I�ȑ΍�.

  <a href="#user-content-index">�ڎ��֖߂�</a>


## <a name="proxy1"></a>�O���v���L�V���g�������ꍇ�ǂ�����̂��H
-----------------------------------

�u���E�U�̃v���L�V�ݒ�_�C�A���O�ɂ͊���localhost(127.0.0.1)���w�肵�Ă��܂��Ă���.
����ł͊O���v���L�V�𗘗p�ł��Ȃ��Ȃ�̂ō���Ƃ������������悤.

Moai�ł͎��̐ڑ������Ƃ��ĊO���v���L�V(parent proxy, �e�v���L�V�ȂǂƂ��Ă΂��)��ݒ肷�邱�Ƃ��ł���.
���̂Ƃ��́A�T�O�I�Ɏ��̂悤�Ȑڑ��ɂȂ�.

~~~
���Ȃ��̃u���E�U�� => ���[�J���v���L�VMoai => �O���v���L�V => ���M��̃T�C�g
~~~

������������邽�߂ɂ� config.myf �� parent_proxy �̒l���ȉ��̌`���ŗ^����.

~~~
     �O���v���L�V�̃z�X�g���܂���IP:�O���v���L�V�̃|�[�g
~~~

�Ⴆ�Έȉ��̂悤�ɋL�q���邱�ƂŊO���v���L�Vproxy.example.net:3128���o�R���ĖړI�̃T�C�g��
�A�N�Z�X���邱�Ƃ��ł���(proxy.example.net:3128���L���ł���΂̘b����).

~~~
     parent_proxy = ['proxy.example.net:3128']
~~~

�O���v���L�V���g��Ȃ��ꍇ(�ړI�̃T�C�g�֒��ڃA�N�Z�X����ꍇ)�͒l�Ƃ��Ĉȉ��̂悤��NONE���w�肷��.
(���邢�͋�l�܂��� :0 �ł��悢)

~~~
     parent_proxy = ['NONE']
~~~

���Amoai�Ɠ��t�H���_����parent_proxy.txt�Ƃ����t�@�C�������A�����Ɏg�p�������v���L�V�̌���
�����񋓂��Ă������ƂŁAMoai Web Configuration��ł����̃v���L�V�����j���[�\�������悤�ɂȂ�.
���̃��j���[���A���ݎg�p����v���L�V���ȒP���u���ɐ؂�ւ����邱�Ƃ��\��.

�Ƃ����IP�A�h���X��ς��邾���Ȃ�ʏ�̓��[�J�`�ŏ\���ł��邵�A�L���ȊO���v���L�V��������̂������ȒP�ł͂Ȃ�.
�Ƃ����킯�Ń��[�J�`���g����󋵂Ȃ�΂�����̎g�p�������߂���.

  <a href="#user-content-index">�ڎ��֖߂�</a>


## <a name="proxy2"></a>�O���v���L�V�̓K�p���ꕔ�̃T�C�g�݂̂Ɍ��肷��
-----------------------------------
�Ⴆ�΁A����T�C�gdanger.net������ꍇ�̂ݗ��R�����ĊO���v���L�V���g�p�������Ƃ���.
(���̑����ׂẴT�C�g�ł͊O���v���L�V���g�p�������Ȃ��Ƃ���).
�O���v���L�V�����ƕ\�����x���Ȃ����肷��̂ŁA�g���K�v���Ȃ��T�C�g�Ȃ�Ύg��Ȃ��������K��.

���̏ꍇ�Aconfig.myf����proxy_apply�ɊO���v���L�V���g�p�������T�C�g���L�q���Ă���.
�܂��A������config.myf����proxy_except�ɗ�O�I�ɊO��proxy���g��Ȃ��T�C�g���L�q���Ă���.
���Aproxy_apply�Ɣ����̂ɂ��Ă�proxy_except�̎w�肪�D�悳���.

�ȉ��ɗ������.


~~~
@@L proxy_apply
*.2chan.net
taruo.net
@@.

@@L proxy_except
jun.2chan.net
@@.
~~~

��L�̂悤�ɋL�q�����ꍇ�A�ŏI�I�ɃA�N�Z�X����ړI�̃T�C�g���Ⴆ�� may.2chan.net �� img.2chan.net �̏ꍇ�́A
proxy_apply�Ɉ�v����p�^�[�������݂���̂ŁA���݂̊O���v���L�V�𒆌p���Đڑ����s����.
������ jun.2chan.net������ proxy_except�Ŏw�肳��Ă���̂ŗ�O�I�ɊO���v���L�V���g��Ȃ��`�ɂȂ�.
(���Ƃ��΂���摜�������葁�����W�����������Ȃ̂ɊԂɏ������x���Ȃ�proxy�����ނ͖̂��ʂł��낤)
��L�̋L�q�ɑ��݂��Ȃ����̑��̈�ʃT�C�g�ɃA�N�Z�X����ꍇ�͊O���v���L�V�͎g���Ȃ�.

�܂��A���̍l�������t�ɂ����w����ł���.

�Ⴆ�΁A����T�C�g������ꍇ�̂݊O���v���L�V���g�p���Ȃ��Ƃ���.
(���̑����ׂẴT�C�g�ł͊O���v���L�V���g�p�������Ƃ���).
���̏ꍇ�͈ȉ��̂悤�ɂ���΂悢.

~~~
@@L proxy_apply
*
@@.

@@L proxy_except
safe.net
127.0.0.1
localhost
192.168.*
@@.
~~~

��L�̂悤�ɋL�q�����ꍇ�A�܂�proxy_apply�ɂ����đS�ẴT�C�g���O���v���L�V�̓K�p�ΏۂƂȂ�.
����proxy_except�ɂ��A���������O�I�Ɏg�p�����O�����T�C�g���w�肳���`�ƂȂ�.
���̗�ł́A�ŏI�I�ɃA�N�Z�X����ړI�̃T�C�g��safe.net�܂���localhost��127.0.0.1�A
���邢��IP�� 192.168.* ��LAN��̃}�V���̏ꍇ�A�O���v���L�V�͎g���Ȃ�.
���̑��̈�ʃT�C�g�ɃA�N�Z�X����ꍇ�͌��݂̊O���v���L�V���g����.

  <a href="#user-content-index">�ڎ��֖߂�</a>


## <a name="proxy3"></a>���̑��̃��[�J���v���L�V�Ƀ`�F�[������ꍇ
-----------------------------------

Moai�͂��̑��̃��[�J���v���L�V�ƒ���Ɍq���邱�Ƃ��ł���.
�Ⴆ��Polipo�Ƃ̃R���{���\�ł��邱�Ƃ��m�F���Ă���.
����ɂ����Polipo��Moai�̉��b�𓯎��ɋ���ł���.
�T�O�I�Ɏ��̂悤�Ȑڑ��ɂȂ�.

���Ȃ��̃u���E�U�� => ���[�J���v���L�VMoai => ���[�J���v���L�VPolipo => ���M��̃T�C�g

�ݒ�菇�͊O���v���L�V���w�肷��ꍇ�ƑS�����l�ɍs����.
�����炭�f�t�H���g�ł�Polipo��8123�|�[�g���g���Ă���̂ŁAparent_proxy�ɒ���localhost:8123���w�肷�邩
���邢�� parent_proxy.txt �� localhost:8123 ��ǉ����Ă����A�K�v�ɂȂ����� Moai Web configration�ɂ�����
parent_proxy���炻���I�ׂ΂悢.

���邢�͈ȉ��̂悤�ɏ��Ԃ��t�ł��\�ł���.

���Ȃ��̃u���E�U�� => ���[�J���v���L�VPolipo => ���[�J���v���L�VMoai => ���M��̃T�C�g

���̏ꍇ��Polipo���̐ݒ�t�@�C����ύX���AMoai(�|�[�g8124)�֐ڑ�����悤�ɂ��Ȃ���΂Ȃ�Ȃ�.
���Ȃ킿Polipo��config.cfg�ɂ����� parentProxy��localhost:8124�Ƃ���`�ɂȂ邾�낤.

  <a href="#user-content-index">�ڎ��֖߂�</a>


## <a name="web_config"></a>Moai Web Configuration
-----------------------------------
  �u���E�U��URL�w�藓�� http://127.0.0.1:8124/config �Ǝw�肷�邱�Ƃŕ\�������邱�Ƃ��ł���.
  ����܂łɏq�ׂ�config.myf���̕ϐ���Web�ォ��m�F����ѐݒ�ł���Ƃ������̂ł���.

  ![screenshot](../imgs/screenshot_config.png)

  **Filters and Plugins**  
  �����ł̓v���O�C���Ƃ��ēo�^����Ă���^�[�Q�b�g��[**Virtual USERS Initiation**][2]�@�\���Ăяo��.
  ����ɂ��A��̓I�ɉ������s����邩�̓v���O�C���ɂ�邪�A���̎��s���ʂ��������ɕ\�������.
  �Ⴆ�΁A�f���̓��e�psend�t�B���^�̕ϐ��l�������_�}�C�Y�������艼�z��������Ȃǂ�
  �uVirtual USERS Initiation�v�{�^���ɂ��s����.

  **Functional Configuration**  
  �����ł�Moai�̋@�\�Ɋւ���ݒ���s��.
~~~
    parent_proxy:
      ���̃��j���[�ł� parent_proxy.txt �̓��e���\�������.
      �������猻�ݎg�p����v���L�V��I��ŁuUpdate�v�������Ύg�p����O���v���L�V�̏u���؂�ւ����ł���.  

    post_confirm:
      POST���̊m�F��ʕ\�����[�h��on/off��؂�ւ���.
      �`�F�b�N�{�b�N�X���N���b�N���Ēl��؂�ւ�����ŁuUpdate�v�������ΐݒ肪���f�����.

    enable_log_file:
      moai_log.log�֑SLog���������o�����ۂ���؂�ւ���.
      �`�F�b�N�{�b�N�X���N���b�N���Ēl��؂�ւ�����ŁuUpdate�v�������ΐݒ肪���f�����.

    enable_log_verbose:
      Log��������ɏڍׂɏ����o�����ۂ���؂�ւ���.
      �`�F�b�N�{�b�N�X���N���b�N���Ēl��؂�ւ�����ŁuUpdate�v�������ΐݒ肪���f�����.

    blocking_mode:
      �\�P�b�g�ʐM�ɂ�����u���b�L���O�ڑ����[�h��on/off��؂�ւ���.
      on�̂Ƃ��u���b�L���O�ڑ��Aoff�̂Ƃ���u���b�L���O�ڑ��ƂȂ邪�A�ʏ��҂̕������K�ɉ{�����\�ł���A
      ���ɖ��Ȃ������off�ł悢.
      �`�F�b�N�{�b�N�X���N���b�N���Ēl��؂�ւ�����ŁuUpdate�v�������ΐݒ肪���f�����.
~~~

  **System Configuration**  
  �����ł�Moai�̒ʐM�V�X�e���̍��{�Ɋւ��ݒ���s���邪�A�ݒ��ύX�����ꍇ�͍אS�̒��ӂ��K�v�ł���. 
  �܂����̐ݒ�ύX��Moai���N�����Ă���}�V���ォ�炵���s���Ȃ��悤�ɂȂ��Ă���.
  (�O���}�V�����烊���[�g�ł͍s���Ȃ��Ƃ�������).
  �uUpdate System�v�{�^�����������Ƃ�Moai���ċN������悤�������b�Z�[�W���\�������.
  �������炳��ɁuRestart Moai�v�{�^�����������Ƃ�Moai�T�[�o���ċN������ݒ肪���f�����`�ƂȂ�.
~~~
    acceptable_host:
      ���̒l��LOOPBACK�ł���ꍇ�͖ⓚ���p�ő��}�V������̐ڑ���r������.
      ���̂Ƃ�accept���̂��̂��s���Ȃ����߁ADOS�U���ɑ΂���h��͂͑���.
      ����A���̒l��ANY�ł���ꍇ�͂��ׂẴ}�V������̐ڑ���F�߂�.
      �������A���̂܂܂��Ƌɒ[�Șb�A���[�^�Ȃǂ̃t�@�C�A�E�H�[���@�\���Ȃ����ł�WAN����̐ڑ���
      �F�߂Ă��܂����ƂɂȂ�.  �uLAN ����̐ڑ��̂݋��v�Ƃ������Ӗ��������ʂȐݒ�l��񋟂������Ƃ���ł͂��邪�A
      �c�O�Ȃ���AOS�̃l�b�g���[�NAPI�ɂ��̂悤�Ȓl���񋟂���Ă��Ȃ����߁A���ꂪ�ł��Ȃ�.
      Moai�ł́A����ɑΏ����邽�� config.myf���� access_allow_ips, access_deny_ips �ϐ��𓱓����A
      �ڑ�������/�s���Ƃ���IP�Q���w��ł���悤�ɂ��Ă���.
      ���̐ݒ�ɂ��Ă͌���Web Configuration����͍s�����Aconfig.myf�𒼐ڕҏW����K�v������.

    server_name:
      Web Configuration�ɂ�����POST��̃z�X�g���Ƃ��Ďg����.
      acceptable_host�̒l��LOOPBACK/ANY�ɕύX�����ꍇ�A���̒l�������I�ɕύX�����ꍇ������.

    moai_port:
      Moai���g�p����(���b�X������)�|�[�g�ԍ��ł���.
~~~

  <a href="#user-content-index">�ڎ��֖߂�</a>


## <a name="plugin"></a>�v���O�C���@�\�ɂ���
-----------------------------------
  Moai�̃v���O�C���ł͌��o�[�W�����ł͎��̂悤�Ȋg�������������ł���.

  <ul>
  <li>initiate :
      Web Configuration��ʂ́uVirtual USERS Initiation�v�{�^�����������Ƃ��ɌĂяo�����
      �}�V�����̉��z������у����_�}�C�Y�������̎���.</li>

  <li>on_post :
      POST��(���X���e���Ȃ�)�ɌĂяo����Ēǉ��Ŏ��s����鏈���̎���.
      ����͂���Ȃ鍂�x��send_filter�������v���O���~���O�Ŏ������邽�߂̋@�\�ł�����.</li>
      
  <li>on_response :
      ���N�G�X�g�ɑ΂��鉞��(�Ⴆ�ΒP����HTML�ɃA�N�Z�X���Ă��̓��e����M����ꍇ�Ȃ�)�ɌĂяo�����
      �ǉ��Ŏ��s����鏈���̎���.
      ����͂���Ȃ鍂�x��recv_filter�������v���O���~���O�Ŏ������邽�߂̋@�\�ł�����.</li>
  </ul>

  plugin�̎��̂�plugins�t�H���_���ɂ���**TARGET_NAME**.dll(�܂���Linux�Ȃǂ̏ꍇ**TARGET_NAME**.so)�Ƃ���
  �t�@�C���ɂȂ�. �����̓Ǝ��ɍ���Ă݂����Ƃ����v���O���}�ȕ���[Hacking][4]�ɂ��̃q���g���L�q���Ă������̂�
  ����������ΎQ�Ƃ��Ă�����������.


  <a href="#user-content-index">�ڎ��֖߂�</a>


[1]: https://github.com/mr-moai-2016/znk_project/blob/master/src/libZnk/myf_spec.md
[2]: https://github.com/mr-moai-2016/znk_project/blob/master/src/virtual_users/VirtualUSERS.md
[3]: https://github.com/mr-moai-2016/znk_project/blob/master/src/HowToCompile.md
[4]: https://github.com/mr-moai-2016/znk_project/blob/master/src/Hacking.md
[5]: https://github.com/mr-moai-2016/znk_project/blob/master/src/moai/FAQ.md
