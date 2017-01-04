# Hints Fingerprint
-----------------------------------

## <a name="index">目次
-----------------------------------
* [Fgp_userAgent](#Fgp_userAgent)
* [Fgp_language] (#Fgp_language)

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


