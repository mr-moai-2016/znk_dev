# Hints PostVars
-----------------------------------

## <a name="index">目次
-----------------------------------
* [Post変数 js](#pv_js)
* [Post変数 pthb](#pv_pthb)

## <a name="pv_js">Post変数 js
-----------------------------------
  この値は女子小学s…ではなく、JavaScriptを実行したかどうかを判断するためのものであり、
  /bin/base4.js を実行すると on に設定される仕組みとなっている.
  ( 無論、JavaScriptの/bin/base4.jsを実行させずとも、このPost変数の値に最初から直接 on 値を設定しておけば良い. )
  これを on にしておかないと「環境変数がありません(JavaScriptがオフ？)」というエラーが表示され、投稿に失敗する.
  よってこの値は必ず on にしておかなければならない.

  <a href="#user-content-index">目次へ戻る</a>

## <a name="pv_pthb"><a name="pv_pthc"><a name="pv_pthd">Post変数 pthb, pthc, pthd
-----------------------------------

  この値はfutaba.phpに初アクセスした時間(およびそれに類した値)である. 現状では
  カタログに初めてアクセスするか、または初めてレス投稿した際にまずpthcに設定されるようである.
  あるいは /bin/cachemt7.php にアクセスしたときにそれに類した値が生成される形となる.
  一度目の投稿においては、localStorage(futabapt)にpthcのコピー値も設定される.
  また二度目の投稿において、さらにそのlocalStorage(futabapt)のコピー値がpthbへと設定される.
  おそらくpthbの値の存在によってlocalStorageに確実に値が設定されたことを確認する意図があると思われる.
  pthdは現状では使用されている気配はない.

  一旦設定されると、CookieおよびブラウザキャッシュおよびlocalStorage これら全てを
  消去しない限り同じ値が残り続ける.

  <a href="#user-content-index">目次へ戻る</a>

[1]: https://github.com/mr-moai-2016/znk_dev/blob/master/src/moai/Hints_PostVars.md
