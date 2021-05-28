#ifndef _LIB_INI_H_
#define _LIB_INI_H_

#include <stdbool.h>
#include <pspiofilemgr.h>

#ifdef __cplusplus
extern "C" {
#endif



/*
	iniLoad/iniSave/iniSplitについて
	送られてくるKey/Valueは読み込んだままの状態なので
	コールバック関数で処理する際は大文字/小文字の区別を
	どうするか意識するのを忘れないで下さい
	
	大文字/小文字区別せず認識するようにするのが一般的
*/



//iniAddKeyで指定するKeyのデータ型
typedef enum
{
	INI_TYPE_NONE = 0,	//無効
	INI_TYPE_DEC,		//10進数
	INI_TYPE_HEX,		//16進数
	INI_TYPE_OCT,		//8進数
	INI_TYPE_BOOL,		//BOOL
	INI_TYPE_STR,		//文字列
	
	//DEC/OCT/HEXはiniLoad/iniSaveで値を正しく取得/保存する為に指定
}ini_type;


//Key,Valueのペア
typedef struct
{
	ini_type	type;	//データタイプ
	const char	*key;	//Key名
	
	void *val_p;
	int val_def;
	bool flag;
} ini_pair;


//iniParseLine
typedef struct
{
	char *key;			//key
	char *key_end;		//keyの最後
	char key_end_cd;	//key_endの文字
	
	char *val;			//value
	char *val_end;		//valueの最後
	char val_end_cd;	//value_endの文字
	
	//key_end、val_endはiniParseLineした後に
	//文字列を元の状態へ戻す為に用意されている
} ini_line;



typedef struct
{
	ini_pair *pair;		//Key/Valueを追加/設定/削除する際に使用するini_pair配列
	int size;			//pairで指定した配列サイズ( sizeof(pair) )
	
	char *buf;			//iniReadLine/iniSplitValで使われるバッファー指定
	int bufLen;			//bufのサイズ（ここで指定した値がiniReadLineで読み込める1行の最大文字数）
} ini_data;



typedef void (*libini_cbsp)(const char *val,void *callback_opt);
typedef const char* (*libini_cbs)(const char *key,void *opt);
typedef bool (*libini_cbl)(const char *key, char *val,void *opt);


enum
{
	INIREADLINE_END		= 0,
	INIREADLINE_CR		= -1,
	INIREADLINE_LF		= -2,
	INIREADLINE_CRLF	= -3,
};



/*	iniReadLine
    ファイルから1行を読み込む
    
    @param: SceUID fd
    SceIoOpenで開いたファイル
    
    @param: char *buf
    読み込んだ文字列を格納するバッファー
    
    @param: int bufLen
    バッファーサイズ
	サイズが小さすぎると1行丸ごと取得出来ないので必要に応じて指定
    
    @return: 以下の通り
	
	INIREADLINE_END		= ファイル終端		※buf -> '\0'のみ
    INIREADLINE_CR		= 改行コード(CR) 	※buf -> '\0'のみ
    INIREADLINE_LF		= 改行コード(LF)	※buf -> '\0'のみ
    INIREADLINE_CRLF	= 改行コード(CRLF)	※buf -> '\0'のみ
	それ以外			= 読み込んだ文字数	※buf -> 読み込んだ文字列
 */
int iniReadLine( SceUID fd, char *buf, int bufLen );

/*	iniParseLine
    iniReadLineによって読み込んだ文字列をKey、Valueへ分ける
    
    @param: ini_line *line
    結果を受け取る ini_line変数
    
    @param: void *buf
    対象文字列
	
    @return: true = 成功、false = 失敗(Key = Valueの形ではない)
    
    ※Key/Valueの始まり/終わりは
    　タブ(\t)、半角/全角スペースを除外した位置になります
 */
bool iniParseLine( ini_line *line ,void *buf );


/*	iniSplitVal
    iniParseLineから得たValue（文字列）を区切文字で分解する
    
    @param: char *val
    対象文字列
    
    @param: const char *delim
    区切り文字
    
    @param: libini_cbsp callback_sp
    区切られた文字列を受け取るコールバック関数
	
	[コールバック関数へ送られてくる値]
    val 			= 区切られた文字列
    callback_opt	= iniSplitValで指定したデータ（任意）
    
    @param: void *callback_opt
    コールバック関数へ渡すデータ
 */
void iniSplitVal( char *val ,const char *delim ,libini_cbsp callback_sp ,void *callback_opt );



/*	iniSearchKey
  	data(ini_data*) から指定したKeyが含まれるデータ(ini_pair*)を取得
    
    @param: ini_data *data
    検索対象のini_data変数
    
    @param: const char *key
    検索対象のKey
    NULLを指定すると空き(Keyがセットされていない場所）を探す
	
    @return: 対象Keyが含まれていたデータ(ini_pair*)
    NULL = 存在しなかった
 */
ini_pair* iniSearchKey( ini_data *data ,const char *key );

/*	iniAddKey
  	data(ini_data*)へKey(Value)を追加する
    
    @param: ini_data *data
    対象のini_data変数
    
    @param: ini_data *data
    追加対象のKey
	
    @param: ini_type type
    データのタイプを指定（INI_TYPE_NONE以外)
    
    @param: void *val_p
    iniLoadを実行する時に、取得した整数値を保存する変数のポインター
    (整数型ではない、iniLoadしない場合はNULLを指定）
	
    void *となっていますが、int_typeによって指定するポインター変数の型が決まっています
    
    *bool = INI_TYPE_BOOL
    *int  = INI_TYPE_DEC / INI_TYPE_OCT / INI_TPYE_HEX
    
    
    @param: int val
    追加するKeyへ関連付ける値
    
    iniAddKeyした後にiniLoad / iniSaveするかで値の扱いが異なります
    
    iniLoad => ファイルから取得失敗した場合のデフォルト値
    iniSave => ファイルへ保存する値
	
    @return: true = 成功、false = 失敗（配列のサイズが足りない、既に存在するKey、無効なタイプ)
 */
bool iniAddKey( ini_data *data ,const char *key ,ini_type type ,void *val_p ,int val );

/*	iniRmvKey
  	data(ini_data*)からKeyを削除（無効に）する
    
    @param: ini_data *data
    対象のini_data変数
    
    @param: ini_data *data
    削除対象のKey
	
    @return: true = 成功、false = 失敗（Keyが存在しない)
 */
bool iniRmvKey( ini_data *data ,const char *key );



/*	iniLoad
  	ファイルから設定を取得する
  	
  	data(ini_data*)で指定されている各Keyのデータ型に応じて値を取得
    
    @param: const char *path
    読み込むファイル名
    
    @param: ini_data *data
    対象のini_data変数
    
    @param: libini_cbl callback_load
    値が文字列型のKeyを処理するコールバック関数
    送られてくるKeyに応じてValueを変換する
    ※ファイルに対象Keyの記述がなければ呼び出されない
    
    [コールバック関数へ送られてくる値]
    const char *key		= ファイルから取得したKey
    char *val			= ファイルから取得したValue
    void *opt			= コールバックへ渡されたデータ（iniLoadで指定したcallback_opt)
    ※文字列を正しく処理出来たかどうかを、bool型で戻り値として返却
	
    @param: void *callback_opt
    コールバック関数へ渡すデータ
    コールバック未指定ならNULL
	
    @return: iniAddKeyで追加したKey数と比べて、実際にファイルから取得出来た設定数
	
    0 = ファイルが存在しない、引数の設定不備、それ以外 = 何らかの設定が取得出来ている
   	
  	※Key/Valueの始まり・終わりは前後のタブ(\t)、半角/全角スペースを除外した位置で識別
  	　大文字小文字は区別せずに比較します（INI_TYPE_STRのみ使用者で判断）
  	　
  	　値が整数値のKeyはiniAddKeyでタイプ(8進数/10進数/16進数)を指定しないと
  	　正しく値が取得されないので注意
 */
int iniLoad( const char *path ,ini_data *data ,libini_cbl callback_load ,void *callback_opt );

/*	iniSave
	ファイルへ設定を保存する
	
  	data(ini_data*)で指定されているKey/Valueを
  	それぞれのデータ型に応じてファイルへ保存する
    
    @param: const char *path
    保存するファイル名
    
    @param: ini_data *data
    対象のini_data変数
    
    @param: libini_cbs callback_save
    値が文字列型のKeyを処理するコールバック関数
    送られてくるKeyに応じてファイルへ書き込む文字列を返却
    ※ファイルに対象Keyの記述がなければ呼び出されない
	
	[コールバック関数へ送られてくる値]
    const char *key		= ファイルへ保存するKey
    void *opt			= コールバックへ渡されたデータ（iniSaveで指定したcallback_opt)
	
    @param: void *callback_opt
    コールバック関数へ渡すデータ
    コールバック未指定ならNULL
	
    @return: true = 成功、false = 失敗（空き容量不足等でファイルが書き込めない、引数の設定不備）
   	
   	※保存先ファイルが既に存在する場合は
   	　対象外の文字列を保ったまま上書きされるので問題なし
   	　
  	　値が整数値のKeyはiniAddKeyでタイプ(8進数/10進数/16進数)を指定しないと
  	　希望する状態で保存されないので注意
*/
bool iniSave( const char *path ,ini_data *data ,libini_cbs callback_save ,void *callback_opt );


/*
	iniCounterPbrk
	文字列(src)の先頭から文字群(search)が含まれない最初の位置を返す
	
    @param: const char * src
    検索元の文字列
    
    @param: const char *search
    検索対象の文字列
	
	@return: 文字群が含まれない最初の位置（先頭から）
	NULL = 存在しない
*/
char* iniCounterPbrk( const char *src, const char *search );

/*
	iniCounterReversePbrk
	文字列(src)の末尾から文字群(search)が含まれない最初の位置を返す
	
    @param: const char * src
    検索元の文字列
    
    @param: const char *search
    検索対象の文字列
	
	@return: 文字群が含まれない最初の位置（末尾から）
	NULL = 存在しない
*/
char* iniCounterReversePbrk( const char *src, const char *search );

/*
	iniGetArraySize
	data->sizeからdata->pair配列の要素数を取得する
	(data->pair配列の設定状況をチェック）
	
	@param: ini_data *data
    対象のini_data変数
	
	@return: data->pair配列の要素数
	※data->sizeの値が異常な場合は0
*/
int iniGetArraySize( ini_data *data );


#ifdef __cplusplus
}
#endif

#endif
