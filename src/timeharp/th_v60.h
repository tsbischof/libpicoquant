/*
 * Copyright (c) 2011-2014, Thomas Bischof
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice, 
 *    this list of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form must reproduce the above copyright notice, 
 *    this list of conditions and the following disclaimer in the documentation 
 *    and/or other materials provided with the distribution.
 * 
 * 3. Neither the name of the Massachusetts Institute of Technology nor the 
 *    names of its contributors may be used to endorse or promote products 
 *    derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE 
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef TH_V60_H_
#define TH_V60_H_

#include <stdio.h>

#include "../picoquant.h"
#include "../tttr.h"

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
	int32_t TTTRGlobClock;
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
	uint32_t TimeTag: 16;
	uint32_t Data: 12;
	uint32_t Route: 2;
	uint32_t Valid: 1;
	uint32_t Reserved: 1;
} th_v60_tttr_record_t;

int th_v60_dispatch(FILE *stream_in, FILE *stream_out, pq_header_t *pq_header,
		options_t *options);

th_v60_header_t *th_v60_header_alloc(int boards, int router_channels);
int th_v60_header_read(FILE *stream_in, th_v60_header_t **th_header);
void th_v60_header_free(th_v60_header_t **th_header);
void th_v60_header_printf(FILE *stream_out, th_v60_header_t *th_header);
void th_v60_header_fwrite(FILE *stream_out, th_v60_header_t *th_header);

int th_v60_interactive_stream(FILE *stream_in, FILE *stream_out,
		pq_header_t *pq_header, th_v60_header_t *th_header, 
		options_t *options);

int th_v60_interactive_read(FILE *stream_in, 
		th_v60_header_t *th_header,
		th_v60_interactive_t **interactive);
void th_v60_interactive_free(th_v60_header_t *th_header,
		th_v60_interactive_t **interactive);
/* Since the interactive data is folded in with the header, it does not make
 * sense to have the two available for separate binary writes.
 */
void th_v60_interactive_header_printf(FILE *stream_out, 
		th_v60_header_t *th_header,
		th_v60_interactive_t **interactive);
void th_v60_interactive_data_print(FILE *stream_out,
		th_v60_header_t *th_header,
		th_v60_interactive_t **interactive,
		options_t *options);

void th_v60_t3_init(th_v60_header_t *th_header,
		th_v60_tttr_header_t *tttr_header,
		tttr_t *tttr);
int th_v60_t3_decode(FILE *stream_in, tttr_t *tttr, t3_t *t3);
int th_v60_tttr_stream(FILE *stream_in, FILE *stream_out, 
		pq_header_t *pq_header, th_v60_header_t *th_header, 
		options_t *options);
int th_v60_t3_stream(FILE *stream_in, FILE *stream_out, 
		th_v60_header_t *th_header, th_v60_tttr_header_t *tttr_header,
		options_t *options);

int th_v60_tttr_header_read(FILE *stream_in, 
		th_v60_tttr_header_t **tttr_header);
void th_v60_tttr_header_free(th_v60_tttr_header_t **tttr_header);
void th_v60_tttr_header_printf(FILE *stream_out, 
		th_v60_tttr_header_t *tttr_header);
void th_v60_tttr_header_fwrite(FILE *stream_out,
		th_v60_tttr_header_t *tttr_header);

#endif
