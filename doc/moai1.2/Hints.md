# Hints
-----------------------------------

## <a name="index">目次
-----------------------------------
* [Cookie posttime](#ck_posttime)
* [Cookie posttime](#ck___cfduid)

## <a name="ck_posttime">Cookie posttime
-----------------------------------
  この値はfutaba.phpに初アクセスした時間(およびそれに類した値)である. 現状では
  カタログに初めてアクセスするか、または初めてレス投稿した際に設定されるようである.
  あるいは /bin/cachemt7.php にアクセスしたときにそれに類した値が生成される形となる.
  一旦設定されると、CookieおよびブラウザキャッシュおよびlocalStorage これら全てを
  消去しない限り同じ値が残り続ける. この値はPOST変数 pthc (過去にはpthb や pthdに設定されていたこともあった)
  とCookieのposttime値として送信される.  

  <a href="#user-content-index">目次へ戻る</a>

## <a name="ck___cfduid">Cookie __cfduid
-----------------------------------

  この値はfutaba.phpに初アクセスしたときに未設定である場合は新たに設定される.
  現在ふたばが導入しているクラウドフレアが発行するものであると思われる.
  一旦設定されると、Cookieを消去しない限り同じ値が残り続ける.
  またブラウザ等の環境を変えない限り、同様の値が再発行される可能性もある.
  この値はCookieの__cfduid値としてふたばへと送信される.  

  <a href="#user-content-index">目次へ戻る</a>

[1]: https://github.com/mr-moai-2016/znk_dev/blob/master/src/moai/Hints.md
