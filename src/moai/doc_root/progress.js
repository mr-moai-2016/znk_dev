var count = 0;
var st_dat_path = "";
function set_dat_path( dat_path ){
	st_dat_path = dat_path;
}
function refer_state(){
	var xmlhttp = false;
	if( typeof ActiveXObject != "undefined" ){
		try {
			xmlhttp = new ActiveXObject( "Microsoft.XMLHTTP" );
		} catch (e) {
			xmlhttp = false;
		}
	}
	if( !xmlhttp && typeof XMLHttpRequest != "undefined" ){
		xmlhttp = new XMLHttpRequest();
		/* Xhr�͕ϊ�����UTF8�Ɖ��肵�āA�y�[�W�Őݒ肳�ꂽ�����R�[�h�ւƎ����I�ɕϊ����s��.
		 * ���̕ϊ����𖾎��I�Ɏw�肷��̂� overrideMimeType �ł��邪�A�����IE9�ȑO�ł̓T�|�[�g����Ă��Ȃ�.
		 * ����Ă���ɗ��炸�Astate.dat ���̂�UTF8�ŏo�͂��ׂ��ł���. */
		//xmlhttp.overrideMimeType( 'text/plain; charset=Shift_JIS' );
	}
	xmlhttp.open( "GET", st_dat_path + '?date=' + new Date().getTime() );
	xmlhttp.onreadystatechange = function() {
		//if( xmlhttp.readyState == 4 ){
		if( xmlhttp.readyState == 4 && xmlhttp.status == 200 ){
			var ary = xmlhttp.responseText.split( ' ' );
			if( ary.length >= 2 ){
				count = parseInt( ary[ 0 ], 10 );
				ary.shift();
				var msg = ary.join( ' ' );
				if( isNaN(count) ){ count=100; }
				document.getElementById( "progress_state_msg" ).innerHTML = msg;
			} else {
				count = parseInt( xmlhttp.responseText, 10 );
				if( isNaN(count) ){ count=100; }
				document.getElementById( "progress_state_msg" ).innerHTML = "[" + count + "] %";
			}
		} else {
			document.getElementById( "progress_state_msg" ).innerHTML = "[" + count + "]";
			++count;
		}
	};
	xmlhttp.send( null );
	//++count;
	return false;
}
var id = setInterval( function(){
	refer_state();
	if(count >= 100){
		clearInterval(id);
		}}, 1000 );
