

#define EUC		0	// KS X 1001 완성형 + KS X 1003 구분자
#define JOL		1	// KS X 1001 조합형 + KS X 1003	구분자 
#define IW		2	// ISO/IEC 10646-1 완성형 (11,172 글자마디) 구분자
#define IPF		3	// ISO/IEC 10646-1 첫가끝 조합형 구분자 
#define IPF1	4	// ISO/IEC 10646-1 첫가끝 조합형 초성 인덱스
#define IPF2	5	// ISO/IEC 10646-1 첫가끝 조합형 중성 인덱스
#define IPF3	6	// ISO/IEC 10646-1 첫가끝 조합형 종성 인덱스
#define IPFSIZE	7	// ISO/IEC 10646-1 첫가끝 조합형 한글자 워드수 인덱스

#define EUC_START_CODE	(0x8141)	// 갂
#define EUC_END_CODE	(0xC8FE)	// 힝 (힣 X)

/* for 2350 char
#define JOL_START_CODE	(0x8861)	// 가
#define JOL_END_CODE	(0xD3B7)	// 힝
/* for 11172 char */
#define JOL_START_CODE	(0x8861)	// 가
#define JOL_END_CODE	(0xD3BD)	// 힣

/* for 2350 char
#define IW_START_CODE	(0xAC00)	// 가
#define IW_END_CODE		(0xD79D)	// 힝
/* for 11172 char */
#define IW_START_CODE	(0xAC00)	// 가
#define IW_END_CODE		(0xD7A3)	// 힣 

/* for 2350 char
#define IPF_START_CODE	(0x00)		// 가
#define IPF_END_CODE	(0x9A46)	// 힝
/* for 11172 char */
#define IPF_START_CODE	(0x00)		// 가
#define IPF_END_CODE	(0x9A4C)	// 힣


#define	IPF1_START_CODE	(0x1100)	// ㄱ
#define	IPF1_END_CODE	(0x1112)	// ㅎ

#define IPF2_START_CODE	(0x1161)	// ㅏ
#define IPF2_END_CODE	(0x1175)	// ㅣ

#define IPF3_EMPTY_CODE (0x1160)	// 받침없음
#define IPF3_START_CODE	(0x1160)	// 받침없음
#define IPF3_START_CODE2	(0x11A8)// ㄱ	
#define IPF3_END_CODE	(0x11C2)	// ㅎ

#define IPF1_RANGE		(IPF1_END_CODE - IPF1_START_CODE + 1)
#define IPF2_RANGE		(IPF2_END_CODE - IPF2_START_CODE + 1)
//#define IPF3_RANGE		(IPF3_END_CODE - IPF3_START_CODE + 1)	주의!
#define IPF3_RANGE		(IPF3_END_CODE - IPF3_EMPTY_CODE + 1)	// IPF고유값 생성을 위한 범위 


int BinaryHanCodeSearch(int wordvalue, int nCode);
int HanCodeConvert( unsigned int fromvalue, unsigned int * tovalue, int nFromCode,int nToCode);

int UTF8toUCS2(unsigned char *utf8, unsigned int *ucs2);
