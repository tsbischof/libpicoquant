#ifndef TH_V60_H_
#define TH_V60_H_

#include <stdio.h>

#include "../picoquant.h"

#pragma pack(2)
typedef struct {
	int32_t MapTo;
	int32_t Show;
} th_v60_display_curve_t;

typedef struct {
	float32_t Start;
	float32_t Step;
	float32_t Stop;
} th_v60_param_t;

typedef struct {
	char HardwareIdent[16];
	char HardwareVersion[8];
	int32_t BoardSerial;
	int32_t CFDZeroCross;
	int32_t CFDDiscriminatorMin;
	int32_t SYNCLevel;
	int32_t CurveOffset;
	float32_t Resolution;
} th_v60_board_t;

typedef struct {
	int32_t CurveIndex;
	time32_t TimeOfRecording; /* time_t TimeOfRecording */
	int32_t BoardSerial;
	int32_t CFDZeroCross;
	int32_t CFDDiscrMin;
	int32_t SyncLevel;
	int32_t CurveOffset;
	int32_t RoutingChannel;
	int32_t SubMode;
	int32_t MeasMode;
	float32_t P1;
	float32_t P2;
	float32_t P3;
	int32_t RangeNo;
	int32_t Offset;
	int32_t AcquisitionTime;
	int32_t StopAfter;
	int32_t StopReason;
	int32_t SyncRate;
	int32_t CFDCountRate;
	int32_t TDCCountRate;
	int32_t IntegralCount;
	float32_t Resolution;
	int32_t ExtDevices;
	int32_t Reserved;
	uint32_t *Counts;
} th_v60_interactive_t;

typedef struct {
	char CreatorName[18];
	char CreatorVersion[12];
	char FileTime[18];
	char CRLF[2];
	char Comment[256];
	int32_t NumberOfChannels;
	int32_t NumberOfCurves;
	int32_t BitsPerChannel;
	int32_t RoutingChannels;
	int32_t NumberOfBoards;
	int32_t ActiveCurve;
	int32_t MeasurementMode;
	int32_t SubMode;
	int32_t RangeNo;
	int32_t Offset;
	int32_t AcquisitionTime;
	int32_t StopAt;
	int32_t StopOnOvfl;
	int32_t Restart;
	int32_t DisplayLinLog;
	int32_t DisplayTimeAxisFrom;
	int32_t DisplayTimeAxisTo;
	int32_t DisplayCountAxisFrom;
	int32_t DisplayCountAxisTo;
	th_v60_display_curve_t DisplayCurve[8];
	th_v60_param_t Param[3];
	int32_t RepeatMode;
	int32_t RepeatsPerCurve;
	int32_t RepeatTime;
	int32_t RepeatWaitTime;
	char ScriptName[20];
	th_v60_board_t *Brd;
} th_v60_header_t;

typedef struct {
	int32_t TTTRGlobclock;
	int32_t ExtDevices;
	int32_t Reserved[5];
	int32_t SyncRate;
	int32_t AverageCFDRate;
	int32_t StopAfter;
	int32_t StopReason;
	int32_t NumberOfRecords;
	int32_t SpecHeaderLength;
	int32_t *SpecHeader;
} th_v60_tttr_header_t;

typedef struct {
	uint32_t Reserved: 1;
	uint32_t Valid: 1;
	uint32_t Route: 2;
	uint32_t Data: 12;
	uint32_t TimeTag: 16;
} th_v60_tttr_record_t;

int th_v60_dispatch(FILE *in_stream, FILE *out_stream, pq_header_t *pq_header,
		options_t *options);

int th_v60_header_read(FILE *in_stream, th_v60_header_t *th_header,
		options_t *options);
void th_v60_header_free(th_v60_header_t *th_header);
void th_v60_header_print(FILE *out_stream, th_v60_header_t *th_header);


int th_v60_interactive_read(FILE *in_stream, th_v60_header_t *th_header,
		th_v60_interactive_t **interactive, options_t *options);

void th_v60_interactive_header_free(th_v60_interactive_t **interactive,
		th_v60_header_t *th_header);
void th_v60_interactive_header_print(FILE *out_stream, 
		th_v60_header_t *th_header, th_v60_interactive_t **interactive);

void th_v60_interactive_data_print(FILE *out_stream, 
		th_v60_header_t *th_header, th_v60_interactive_t **interactive, 
		options_t *options);

int th_v60_interactive_stream(FILE *in_stream, FILE *out_stream,
		pq_header_t *pq_header, th_v60_header_t *th_header, 
		options_t *options);

#endif
