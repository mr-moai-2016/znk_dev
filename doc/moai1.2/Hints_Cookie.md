# Hints Cookie
-----------------------------------

## <a name="index">目次
-----------------------------------
* [Cookie posttime](#ck_posttime)
* [Cookie __cfduid](#ck___cfduid)
* [Cookie cxyl]    (#ck_cxyl)
* [Cookie namec]   (#ck_namec)
* [Cookie pwdc]    (#ck_pwdc)

## <a name="ck_posttime">Cookie posttime
-----------------------------------
  この値はfutaba.phpに初アクセスした時間(およびそれに類した値)である. 現状では
  カタログに初めてアクセスするか、または初めてレス投稿した際に設定されるようである.
  あるいは /bin/cachemt7.php にアクセスしたときにそれに類した値が生成される形となる.
  一旦設定されると、CookieおよびブラウザキャッシュおよびlocalStorage これら全てを
  消去しない限り同じ値が残り続ける. この値はPOST変数 pthb, pthc (pthd)とCookieのposttime値として送信される.  

  <a href="#user-content-index">目次へ戻る</a>

## <a name="ck___cfduid">Cookie __cfduid
-----------------------------------

  この値はfutaba.phpに初アクセスしたときに未設定である場合は新たに設定される.
  現在ふたばが導入しているクラウドフレアが発行するものであると思われる.
  一旦設定されると、Cookieを消去しない限り同じ値が残り続ける.
  またブラウザ等の環境を変えない限り、同様の値が再発行される可能性もある.
  この値はCookieの__cfduid値としてふたばへと送信される.  

  <a href="#user-content-index">目次へ戻る</a>

## <a name="ck_cxyl">Cookie cxyl
-----------------------------------

  この値はカタログのフォーマットを指定するために存在する.
  これが示す内容の意味はカタログの「設定」画面から行える指定そのものである.

~~~
    横のスレ個数 x 縦のスレ個数 x 各スレでの文字数 x 文字位置(0:下,1:右) x 画像サイズ(0から6までで0が最小で旧来の表示)
~~~
  というフォーマットを持つ記述子となる.

  デフォルトではこの値は指定されておらず、その場合futaba.phpは ['14x6x4x0x0'] という値が指定されたものとみなすようである.
  基本的にはユーザが自由に設定するものであるからユーザ識別とは関係がないと思われる.

  <a href="#user-content-index">目次へ戻る</a>

## <a name="ck_namec">Cookie namec
-----------------------------------

  この値は最後のレスにおいて使用した「お名前欄」の内容を示す.
  値は全角文字などを含む可能性があり、URLエンコーディングされる.
  何のために使われるものかは不明であるが、コテ対策で導入したものである可能性もある.
  いずれにせよ空値以外の値を設定していても何一つ良いことはないので、基本的には消去しておくことが望ましい.
  futaba_send.myf の cookie_force において namec = [''] を指定しておくことをお勧めする
  (futaba_send.myfのデフォルトではそうなっている).

  <a href="#user-content-index">目次へ戻る</a>

## <a name="ck_pwdc">Cookie pwdc
-----------------------------------

  名前の由来は PassWorD Cookie と思われる.

  この値は、レス投稿時のいわゆる「削除キー」に相当するものである.
  ユーザが明示的に指定してもよいが、空にしておけば適当な削除キー値が自動的に割り当てられる.
  このときCookieのpwdc変数にその値が格納され、レス投稿時においてはJavascriptを介してこの値がさらにPost変数pwdへセットされる.
  通常は自動的に割り当てられた値をそのまま使えばよい.
  ユーザ情報をリセットしたい場合は、Cookieのpwdc値を一旦空にしておくとよい.

  <a href="#user-content-index">目次へ戻る</a>

## <a name="unknown_cookie_var">未知のCookie変数について
-----------------------------------

  CustomCookieにおいて「Unknown」と表記されている変数は、CustomBoyが知らないあるいは把握していない変数である.
  ふたばの仕様変更や、ある種の特別な規制にのみ発現するレアな変数などが存在する可能性があり、
  そのようなCookie変数が検出された場合、この表示がされる場合がある.
  CustomCookieに「Unknown」と表記された変数が登場した場合、CustomBoyの作者に報告してもらえれば幸いである.
  その変数の仕様が確認できればこれに対応出来る可能性がある.

  <a href="#user-content-index">目次へ戻る</a>

[1]: https://github.com/mr-moai-2016/znk_dev/blob/master/src/moai/Hints_Cookie.md
