/*
 * tools.h
 *
 *  Created on: 2017Äê3ÔÂ30ÈÕ
 *      Author: potato-cat
 */

#ifndef INCLUDE_TOOLS_H_
#define INCLUDE_TOOLS_H_

typedef struct FLAG_STRUCT {
	char SamCom	:	1;
	char RecCom :	1;
	char reserve :	6;
}	FLAG;


typedef struct VOICE_STRUCT {
	short start;
	short end;
}	VOICE;

typedef struct MFCC_STRUCT{
	short len;
	float fullresult[26][24];
} MFCC;

#endif /* INCLUDE_TOOLS_H_ */
