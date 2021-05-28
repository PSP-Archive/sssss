#ifndef __COMMON_H__
#define __COMMON_H__

// タイプ(拡張子)の設定
#ifdef MODE_JPEG
#define MODULE_TYPE		"jpg"
#endif

#ifdef MODE_PNG
#define MODULE_TYPE		"png"
#endif

#ifdef MODE_BMP
#define MODULE_TYPE		"bmp"
#endif

#define	BUF_SIZE	256

#define INI_KEY_BUTTONS "buttons"

typedef unsigned char   BYTE;

#endif //__COMMON_H__
