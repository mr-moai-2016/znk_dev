# Hints Fingerprint
-----------------------------------

## <a name="index">目次
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

  navigator.userAgentより取得されるいわゆるUser-Agent文字列である.
  現状のふたばのFingerprintにおいてはこれは参照されていない.

  <a href="#user-content-index">目次へ戻る</a>

## <a name="Fgp_language">Fgp_language
-----------------------------------

  navigator.languageより取得されるいわゆるブラウザの言語情報を示す文字列である.
  日本語環境のブラウザの場合、ja、ja-JP、ja-jp、ja-JP-macなどの値を取るが、
  実際どのような値になるかはブラウザやその設定などによっても様々となる場合がある.

  勿論例外もあるがおおよそ以下が目安である.

  <ul>
  <li>Android: ja-JP</li>
  <li>iPhone: ja-jp</li>
  <li>Mac Firefox: ja-JP-mac</li>
  <li>Mac safari: ja-jp</li>
  <li>その他: ja</li>
  </ul>

  <a href="#user-content-index">目次へ戻る</a>

## <a name="Fgp_colorDepth">Fgp_colorDepth
-----------------------------------

  screen.colorDepthより取得されるモニタの色深度ビット数を示す文字列である.
  32、24、16などの値を取るが、実際どのような値になるかはブラウザやその設定などによっても様々となる場合がある.

  <a href="#user-content-index">目次へ戻る</a>

## <a name="Fgp_ScreenResolusion">Fgp_ScreenResolusion
-----------------------------------

  screen.width、screen.heightより取得されるモニタの解像度を示す二つの数字を文字xで連結した文字列である.
  順番はこれとは逆順になることもある.
  現状のふたばのFingerprintにおいてはこれは参照されていない.

  <a href="#user-content-index">目次へ戻る</a>

## <a name="Fgp_TimezoneOffset">Fgp_TimezoneOffset
-----------------------------------

  new Date().getTimezoneOffset() により取得されるタイムゾーンを意味する(世界標準時からの)オフセット値を示す文字列である.
  日本ではこの値は必ず -540 固定と考えてよい.

  <a href="#user-content-index">目次へ戻る</a>

## <a name="Fgp_SessionStorage">Fgp_SessionStorage
-----------------------------------

  !!window.sessionStorage というコードSnippetを実行した結果を示し、trueまたはfalseという文字列となる.
  そのブラウザにsessionStorage機構が存在するか否かを示す.
  ブラウザの対応状況は以下が目安である(MDNによる).

  <ul>
  <li>Chrome: 5以上</li>
  <li>Firefox(Gecko): 2以上</li>
  <li>IE: 8以上</li>
  <li>Opera: 10.50以上</li>
  <li>Safari(Webkit): 4以上</li>
  <li>Android: 2.1以上</li>
  <li>Opera Mobile: 11以上</li>
  <li>Safari Mobile: iOS 3.2以上</li>
  </ul>

  <a href="#user-content-index">目次へ戻る</a>

## <a name="Fgp_LocalStorage">Fgp_LocalStorage
-----------------------------------

  !!window.localStorage というコードSnippetを実行した結果を示し、trueまたはfalseという文字列となる.
  そのブラウザにlocalStorage機構が存在するか否かを示す.
  ブラウザの対応状況は以下が目安である(MDNによる).

  <ul>
  <li>Chrome: 4以上</li>
  <li>Firefox(Gecko): 3.5以上</li>
  <li>IE: 8以上</li>
  <li>Opera: 10.50以上</li>
  <li>Safari(Webkit): 4以上</li>
  <li>Android: 2.1以上</li>
  <li>Opera Mobile: 11以上</li>
  <li>Safari Mobile: iOS 3.2以上</li>
  </ul>

  <a href="#user-content-index">目次へ戻る</a>

## <a name="Fgp_IndexedDB">Fgp_IndexedDB
-----------------------------------

  !!window.indexedDB というコードSnippetを実行した結果を示し、trueまたはfalseという文字列となる.
  そのブラウザにindexedDB機構が存在するか否かを示す.
  ブラウザの対応状況は以下が目安である(MDNによる).

  <ul>
  <li>Chrome: 23以上</li>
  <li>Firefox(Gecko): 10以上</li>
  <li>IE: 10以上</li>
  <li>Opera: 15以上</li>
  <li>Safari(Webkit): 7.1以上</li>
  <li>Edge: OK</li>
  <li>Android: 4.4以上</li>
  <li>Firefox Mobile: 22以上</li>
  <li>Firefox OS: 1.0.1以上</li>
  <li>Opera Mobile: 22以上</li>
  <li>Safari Mobile: 8以上</li>
  </ul>

  <a href="#user-content-index">目次へ戻る</a>

## <a name="Fgp_BodyAddBehavior">Fgp_BodyAddBehavior
-----------------------------------

  typeof(document.body.addBehavior) というコードSnippetを実行した結果を示し、functionまたはobjectまたはundefinedという文字列となる.
  そのブラウザにdocument.body.addBehaviorメソッドが存在するか否かを示す.
  これはおそらくIE10以上にしか存在しない.

  <ul>
  <li>IE10以上: functionを返す.</li>
  <li>IE9以下: objectを返す.</li>
  <li>その他: undefinedを返す.</li>
  </ul>

  <a href="#user-content-index">目次へ戻る</a>

## <a name="Fgp_OpenDatabase">Fgp_OpenDatabase
-----------------------------------

  typeof(window.openDatabase) というコードSnippetを実行した結果を示し、functionまたはobjectまたはundefinedという文字列となる.
  そのブラウザにHTML5 WebSQL Databaseがサポートされているか否かを示す.

  <ul>
  <li>Chrome: 8以上か?</li>
  <li>Firefox(Gecko): 未サポート(undefinedを返す)</li>
  <li>IE: 未サポート(undefinedを返す)</li>
  <li>Opera: 10.6以上か?</li>
  <li>Safari: 3.1以上</li>
  <li>Android: 2.1以上</li>
  </ul>

  <a href="#user-content-index">目次へ戻る</a>

## <a name="Fgp_CpuClass">Fgp_CpuClass
-----------------------------------

  navigator.cpuClass より取得されるこのブラウザの動作しているマシンのCPU情報を示す文字列である.
  x86、PPC などを返すと言われているが、実際のところほとんどのケースで空値となっている.

  <a href="#user-content-index">目次へ戻る</a>

## <a name="Fgp_Platform">Fgp_Platform
-----------------------------------

  navigator.platform より取得されるこのブラウザの動作しているOS環境を示す文字列である.
  おおよそ以下が目安である.

  <ul>
  <li>Windows32/WOW : Win32</li>
  <li>Windows64     : Win64</li>
  <li>Linux32   : Linux i686</li>
  <li>Linux64   : Linux x86_64</li>
  <li>android   : Linux armv7l</li>
  <li>iPhone    : iPhone</li>
  <li>Mac       : MacIntel</li>
  </ul>

  <a href="#user-content-index">目次へ戻る</a>

