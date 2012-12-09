/* 
	PHLib programming library for PicoHarp 300
	PicoQuant GmbH, April 2009
*/


#define LIB_VERSION "2.3"

#define MAXDEVNUM	8

#define HISTCHAN	65536	// number of histogram channels
#define TTREADMAX   131072  // 128K event records
#define RANGES		8

#define MODE_HIST	0
#define MODE_T2		2
#define MODE_T3		3

#define FLAG_OVERFLOW     0x0040 
#define FLAG_FIFOFULL     0x0003


#define ZCMIN		0			//mV
#define ZCMAX		20			//mV
#define DISCRMIN	0			//mV
#define DISCRMAX	800			//mV

#define OFFSETMIN	0			//ps
#define OFFSETMAX	1000000000	//ps
#define ACQTMIN		1			//ms
#define ACQTMAX		360000000	//ms  (100*60*60*1000ms = 100h) 

#define PHR800LVMIN -1600		//mV
#define PHR800LVMAX  2400		//mV


//The following are bitmasks for return values from GetWarnings()

#define WARNING_INP0_RATE_ZERO				0x0001
#define WARNING_INP0_RATE_TOO_LOW			0x0002
#define WARNING_INP0_RATE_TOO_HIGH			0x0004

#define WARNING_INP1_RATE_ZERO				0x0010
#define WARNING_INP1_RATE_TOO_HIGH			0x0040

#define WARNING_INP_RATE_RATIO				0x0100
#define WARNING_DIVIDER_GREATER_ONE			0x0200
#define WARNING_TIME_SPAN_TOO_SMALL			0x0400
#define WARNING_OFFSET_UNNECESSARY			0x0800
