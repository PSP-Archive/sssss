#ifndef _LIB_INI_H_
#define _LIB_INI_H_

#include <stdbool.h>
#include <pspiofilemgr.h>

#ifdef __cplusplus
extern "C" {
#endif



/*
	iniLoad/iniSave/iniSplit�ɂ���
	�����Ă���Key/Value�͓ǂݍ��񂾂܂܂̏�ԂȂ̂�
	�R�[���o�b�N�֐��ŏ�������ۂ͑啶��/�������̋�ʂ�
	�ǂ����邩�ӎ�����̂�Y��Ȃ��ŉ�����
	
	�啶��/��������ʂ����F������悤�ɂ���̂���ʓI
*/



//iniAddKey�Ŏw�肷��Key�̃f�[�^�^
typedef enum
{
	INI_TYPE_NONE = 0,	//����
	INI_TYPE_DEC,		//10�i��
	INI_TYPE_HEX,		//16�i��
	INI_TYPE_OCT,		//8�i��
	INI_TYPE_BOOL,		//BOOL
	INI_TYPE_STR,		//������
	
	//DEC/OCT/HEX��iniLoad/iniSave�Œl�𐳂����擾/�ۑ�����ׂɎw��
}ini_type;


//Key,Value�̃y�A
typedef struct
{
	ini_type	type;	//�f�[�^�^�C�v
	const char	*key;	//Key��
	
	void *val_p;
	int val_def;
	bool flag;
} ini_pair;


//iniParseLine
typedef struct
{
	char *key;			//key
	char *key_end;		//key�̍Ō�
	char key_end_cd;	//key_end�̕���
	
	char *val;			//value
	char *val_end;		//value�̍Ō�
	char val_end_cd;	//value_end�̕���
	
	//key_end�Aval_end��iniParseLine�������
	//����������̏�Ԃ֖߂��ׂɗp�ӂ���Ă���
} ini_line;



typedef struct
{
	ini_pair *pair;		//Key/Value��ǉ�/�ݒ�/�폜����ۂɎg�p����ini_pair�z��
	int size;			//pair�Ŏw�肵���z��T�C�Y( sizeof(pair) )
	
	char *buf;			//iniReadLine/iniSplitVal�Ŏg����o�b�t�@�[�w��
	int bufLen;			//buf�̃T�C�Y�i�����Ŏw�肵���l��iniReadLine�œǂݍ��߂�1�s�̍ő啶�����j
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
    �t�@�C������1�s��ǂݍ���
    
    @param: SceUID fd
    SceIoOpen�ŊJ�����t�@�C��
    
    @param: char *buf
    �ǂݍ��񂾕�������i�[����o�b�t�@�[
    
    @param: int bufLen
    �o�b�t�@�[�T�C�Y
	�T�C�Y�������������1�s�ۂ��Ǝ擾�o���Ȃ��̂ŕK�v�ɉ����Ďw��
    
    @return: �ȉ��̒ʂ�
	
	INIREADLINE_END		= �t�@�C���I�[		��buf -> '\0'�̂�
    INIREADLINE_CR		= ���s�R�[�h(CR) 	��buf -> '\0'�̂�
    INIREADLINE_LF		= ���s�R�[�h(LF)	��buf -> '\0'�̂�
    INIREADLINE_CRLF	= ���s�R�[�h(CRLF)	��buf -> '\0'�̂�
	����ȊO			= �ǂݍ��񂾕�����	��buf -> �ǂݍ��񂾕�����
 */
int iniReadLine( SceUID fd, char *buf, int bufLen );

/*	iniParseLine
    iniReadLine�ɂ���ēǂݍ��񂾕������Key�AValue�֕�����
    
    @param: ini_line *line
    ���ʂ��󂯎�� ini_line�ϐ�
    
    @param: void *buf
    �Ώە�����
	
    @return: true = �����Afalse = ���s(Key = Value�̌`�ł͂Ȃ�)
    
    ��Key/Value�̎n�܂�/�I����
    �@�^�u(\t)�A���p/�S�p�X�y�[�X�����O�����ʒu�ɂȂ�܂�
 */
bool iniParseLine( ini_line *line ,void *buf );


/*	iniSplitVal
    iniParseLine���瓾��Value�i������j����ؕ����ŕ�������
    
    @param: char *val
    �Ώە�����
    
    @param: const char *delim
    ��؂蕶��
    
    @param: libini_cbsp callback_sp
    ��؂�ꂽ��������󂯎��R�[���o�b�N�֐�
	
	[�R�[���o�b�N�֐��֑����Ă���l]
    val 			= ��؂�ꂽ������
    callback_opt	= iniSplitVal�Ŏw�肵���f�[�^�i�C�Ӂj
    
    @param: void *callback_opt
    �R�[���o�b�N�֐��֓n���f�[�^
 */
void iniSplitVal( char *val ,const char *delim ,libini_cbsp callback_sp ,void *callback_opt );



/*	iniSearchKey
  	data(ini_data*) ����w�肵��Key���܂܂��f�[�^(ini_pair*)���擾
    
    @param: ini_data *data
    �����Ώۂ�ini_data�ϐ�
    
    @param: const char *key
    �����Ώۂ�Key
    NULL���w�肷��Ƌ�(Key���Z�b�g����Ă��Ȃ��ꏊ�j��T��
	
    @return: �Ώ�Key���܂܂�Ă����f�[�^(ini_pair*)
    NULL = ���݂��Ȃ�����
 */
ini_pair* iniSearchKey( ini_data *data ,const char *key );

/*	iniAddKey
  	data(ini_data*)��Key(Value)��ǉ�����
    
    @param: ini_data *data
    �Ώۂ�ini_data�ϐ�
    
    @param: ini_data *data
    �ǉ��Ώۂ�Key
	
    @param: ini_type type
    �f�[�^�̃^�C�v���w��iINI_TYPE_NONE�ȊO)
    
    @param: void *val_p
    iniLoad�����s���鎞�ɁA�擾���������l��ۑ�����ϐ��̃|�C���^�[
    (�����^�ł͂Ȃ��AiniLoad���Ȃ��ꍇ��NULL���w��j
	
    void *�ƂȂ��Ă��܂����Aint_type�ɂ���Ďw�肷��|�C���^�[�ϐ��̌^�����܂��Ă��܂�
    
    *bool = INI_TYPE_BOOL
    *int  = INI_TYPE_DEC / INI_TYPE_OCT / INI_TPYE_HEX
    
    
    @param: int val
    �ǉ�����Key�֊֘A�t����l
    
    iniAddKey�������iniLoad / iniSave���邩�Œl�̈������قȂ�܂�
    
    iniLoad => �t�@�C������擾���s�����ꍇ�̃f�t�H���g�l
    iniSave => �t�@�C���֕ۑ�����l
	
    @return: true = �����Afalse = ���s�i�z��̃T�C�Y������Ȃ��A���ɑ��݂���Key�A�����ȃ^�C�v)
 */
bool iniAddKey( ini_data *data ,const char *key ,ini_type type ,void *val_p ,int val );

/*	iniRmvKey
  	data(ini_data*)����Key���폜�i�����Ɂj����
    
    @param: ini_data *data
    �Ώۂ�ini_data�ϐ�
    
    @param: ini_data *data
    �폜�Ώۂ�Key
	
    @return: true = �����Afalse = ���s�iKey�����݂��Ȃ�)
 */
bool iniRmvKey( ini_data *data ,const char *key );



/*	iniLoad
  	�t�@�C������ݒ���擾����
  	
  	data(ini_data*)�Ŏw�肳��Ă���eKey�̃f�[�^�^�ɉ����Ēl���擾
    
    @param: const char *path
    �ǂݍ��ރt�@�C����
    
    @param: ini_data *data
    �Ώۂ�ini_data�ϐ�
    
    @param: libini_cbl callback_load
    �l��������^��Key����������R�[���o�b�N�֐�
    �����Ă���Key�ɉ�����Value��ϊ�����
    ���t�@�C���ɑΏ�Key�̋L�q���Ȃ���ΌĂяo����Ȃ�
    
    [�R�[���o�b�N�֐��֑����Ă���l]
    const char *key		= �t�@�C������擾����Key
    char *val			= �t�@�C������擾����Value
    void *opt			= �R�[���o�b�N�֓n���ꂽ�f�[�^�iiniLoad�Ŏw�肵��callback_opt)
    ��������𐳂��������o�������ǂ������Abool�^�Ŗ߂�l�Ƃ��ĕԋp
	
    @param: void *callback_opt
    �R�[���o�b�N�֐��֓n���f�[�^
    �R�[���o�b�N���w��Ȃ�NULL
	
    @return: iniAddKey�Œǉ�����Key���Ɣ�ׂāA���ۂɃt�@�C������擾�o�����ݒ萔
	
    0 = �t�@�C�������݂��Ȃ��A�����̐ݒ�s���A����ȊO = ���炩�̐ݒ肪�擾�o���Ă���
   	
  	��Key/Value�̎n�܂�E�I���͑O��̃^�u(\t)�A���p/�S�p�X�y�[�X�����O�����ʒu�Ŏ���
  	�@�啶���������͋�ʂ����ɔ�r���܂��iINI_TYPE_STR�̂ݎg�p�҂Ŕ��f�j
  	�@
  	�@�l�������l��Key��iniAddKey�Ń^�C�v(8�i��/10�i��/16�i��)���w�肵�Ȃ���
  	�@�������l���擾����Ȃ��̂Œ���
 */
int iniLoad( const char *path ,ini_data *data ,libini_cbl callback_load ,void *callback_opt );

/*	iniSave
	�t�@�C���֐ݒ��ۑ�����
	
  	data(ini_data*)�Ŏw�肳��Ă���Key/Value��
  	���ꂼ��̃f�[�^�^�ɉ����ăt�@�C���֕ۑ�����
    
    @param: const char *path
    �ۑ�����t�@�C����
    
    @param: ini_data *data
    �Ώۂ�ini_data�ϐ�
    
    @param: libini_cbs callback_save
    �l��������^��Key����������R�[���o�b�N�֐�
    �����Ă���Key�ɉ����ăt�@�C���֏������ޕ������ԋp
    ���t�@�C���ɑΏ�Key�̋L�q���Ȃ���ΌĂяo����Ȃ�
	
	[�R�[���o�b�N�֐��֑����Ă���l]
    const char *key		= �t�@�C���֕ۑ�����Key
    void *opt			= �R�[���o�b�N�֓n���ꂽ�f�[�^�iiniSave�Ŏw�肵��callback_opt)
	
    @param: void *callback_opt
    �R�[���o�b�N�֐��֓n���f�[�^
    �R�[���o�b�N���w��Ȃ�NULL
	
    @return: true = �����Afalse = ���s�i�󂫗e�ʕs�����Ńt�@�C�����������߂Ȃ��A�����̐ݒ�s���j
   	
   	���ۑ���t�@�C�������ɑ��݂���ꍇ��
   	�@�ΏۊO�̕������ۂ����܂܏㏑�������̂Ŗ��Ȃ�
   	�@
  	�@�l�������l��Key��iniAddKey�Ń^�C�v(8�i��/10�i��/16�i��)���w�肵�Ȃ���
  	�@��]�����Ԃŕۑ�����Ȃ��̂Œ���
*/
bool iniSave( const char *path ,ini_data *data ,libini_cbs callback_save ,void *callback_opt );


/*
	iniCounterPbrk
	������(src)�̐擪���當���Q(search)���܂܂�Ȃ��ŏ��̈ʒu��Ԃ�
	
    @param: const char * src
    �������̕�����
    
    @param: const char *search
    �����Ώۂ̕�����
	
	@return: �����Q���܂܂�Ȃ��ŏ��̈ʒu�i�擪����j
	NULL = ���݂��Ȃ�
*/
char* iniCounterPbrk( const char *src, const char *search );

/*
	iniCounterReversePbrk
	������(src)�̖������當���Q(search)���܂܂�Ȃ��ŏ��̈ʒu��Ԃ�
	
    @param: const char * src
    �������̕�����
    
    @param: const char *search
    �����Ώۂ̕�����
	
	@return: �����Q���܂܂�Ȃ��ŏ��̈ʒu�i��������j
	NULL = ���݂��Ȃ�
*/
char* iniCounterReversePbrk( const char *src, const char *search );

/*
	iniGetArraySize
	data->size����data->pair�z��̗v�f�����擾����
	(data->pair�z��̐ݒ�󋵂��`�F�b�N�j
	
	@param: ini_data *data
    �Ώۂ�ini_data�ϐ�
	
	@return: data->pair�z��̗v�f��
	��data->size�̒l���ُ�ȏꍇ��0
*/
int iniGetArraySize( ini_data *data );


#ifdef __cplusplus
}
#endif

#endif
