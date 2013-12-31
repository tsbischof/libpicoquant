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

#ifndef PH_V20_H_
#define PH_V20_H_

#include <stdio.h>

#include "../picoquant.h"
#include "../tttr.h"

#define PH_V20_BASE_RESOLUTION 4e-12

#pragma pack(2)
typedef struct {
	int32_t MapTo;
	int32_t Show;
} ph_v20_display_curve_t;

typedef struct {
	float32_t Start;
	float32_t Step;
	float32_t Stop;
} ph_v20_param_t;

typedef struct {
	int32_t InputType;
	int32_t InputLevel;
	int32_t InputEdge;
	int32_t CFDPresent;
	int32_t CFDLevel;
	int32_t CFDZCross;
} ph_v20_router_channel_t;

typedef struct {
	int32_t CurveIndex;
	time32_t TimeOfRecording;
	char HardwareIdent[16];
	char HardwareVersion[8];
	int32_t HardwareSerial;
	int32_t SyncDivider;
	int32_t CFDZeroCross0;
	int32_t CFDLevel0;
	int32_t CFDZeroCross1;
	int32_t CFDLevel1;
	int32_t Offset;
	int32_t RoutingChannel;
	int32_t ExtDevices;
	int32_t MeasMode;
	int32_t SubMode;
	float32_t P1;
	float32_t P2;
	float32_t P3;
	int32_t RangeNo;
	float32_t Resolution;
	int32_t Channels;
	int32_t AcquisitionTime;
	int32_t StopAfter;
	int32_t StopReason;
	int32_t InpRate0;
	int32_t InpRate1;
	int32_t HistCountRate;
	int64_t IntegralCount;
	int32_t Reserved;
	int32_t DataOffset;
	int32_t RouterModelCode;
	int32_t RouterEnabled;
	int32_t RtCh_InputType;
	int32_t RtCh_InputLevel;
	int32_t RtCh_InputEdge;
	int32_t RtCh_CFDPresent;
	int32_t RtCh_CFDLevel;
	int32_t RtCh_CFDZeroCross;
} ph_v20_curve_t;

typedef struct {
	char HardwareIdent[16];
	char HardwareVersion[8];
	int32_t HardwareSerial;
	int32_t SyncDivider;
	int32_t CFDZeroCross0;
	int32_t CFDLevel0;
	int32_t CFDZeroCross1;
	int32_t CFDLevel1;
	float32_t Resolution;
	int32_t RouterModelCode;
	int32_t RouterEnabled;
	ph_v20_router_channel_t *RtCh;
} ph_v20_board_t;

typedef struct {
	char CreatorName[18];
	char CreatorVersion[12];
	char FileTime[18];
	char CRLF[2];
	char Comment[256];
	int32_t NumberOfCurves;

	/* Note that Records is the only difference between interactive and 
	 * tttr main headers. Interactive calls this BitsPerHistogBin.
	 */
	int32_t BitsPerRecord;

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
	ph_v20_display_curve_t DisplayCurve[8];
	ph_v20_param_t Param[3];
	int32_t RepeatMode;
	int32_t RepeatsPerCurve;
	int32_t RepeatTime;
	int32_t RepeatWaitTime;
	char ScriptName[20];
	ph_v20_board_t *Brd;
} ph_v20_header_t;

typedef struct {
	ph_v20_curve_t *Curve;
	uint32_t **Counts;
} ph_v20_interactive_t;

typedef struct {
	int32_t ExtDevices;
	int32_t Reserved[2];
	int32_t InpRate0;
	int32_t InpRate1;
	int32_t StopAfter;
	int32_t StopReason;
	int32_t NumRecords;
	int32_t ImgHdrSize;
	uint32_t *ImgHdr;
} ph_v20_tttr_header_t;

typedef struct {
	/* Note that this order is opposite that of the manual (see pt2demo.c)*/
	uint32_t time: 28;
	uint32_t channel: 4;
} ph_v20_t2_record_t;

typedef struct {
	uint32_t nsync: 16;
	uint32_t dtime: 12;
	uint32_t channel: 4;
} ph_v20_t3_record_t;

/* After defining the structures, there a few well-characterized things we want
 * to do to them, such as reading and freeing.
 */

int ph_v20_dispatch(FILE *stream_in, FILE *stream_out, pq_header_t *pq_header,
		options_t *options);

ph_v20_header_t *ph_v20_header_alloc(int boards, int router_channels);
int ph_v20_header_read(FILE *stream_in, ph_v20_header_t **ph_header);
void ph_v20_header_free(ph_v20_header_t **ph_header);
void ph_v20_header_printf(FILE *stream_out, ph_v20_header_t *ph_header);
void ph_v20_header_fwrite(FILE *stream_out, ph_v20_header_t *ph_header);

int ph_v20_interactive_stream(FILE *stream_in, FILE *stream_out,
		pq_header_t *pq_header, ph_v20_header_t *ph_header, 
		options_t *options);

int ph_v20_interactive_header_read(FILE *stream_in, 
		ph_v20_header_t *ph_header,
		ph_v20_interactive_t **interactive);
void ph_v20_interactive_header_free(ph_v20_interactive_t **interactive);
void ph_v20_interactive_header_printf(FILE *stream_out, 
		ph_v20_header_t *ph_header,
		ph_v20_interactive_t *interactive);
void ph_v20_interactive_header_fwrite(FILE *stream_out, 
		ph_v20_header_t *ph_header,
		ph_v20_interactive_t *interactive);

int ph_v20_interactive_data_read(FILE *stream_in,
		ph_v20_header_t *ph_header,
		ph_v20_interactive_t *interactive);
void ph_v20_interactive_data_free(ph_v20_header_t *ph_header,
		ph_v20_interactive_t *interactive);
void ph_v20_interactive_data_print(FILE *stream_out,
		ph_v20_header_t *ph_header,
		ph_v20_interactive_t *interactive,
		options_t *options);

void ph_v20_t2_init(ph_v20_header_t *ph_header,
		ph_v20_tttr_header_t *tttr_header,
		tttr_t *tttr);
int ph_v20_t2_decode(FILE *stream_in, tttr_t *tttr, t2_t *t2);
void ph_v20_t3_init(ph_v20_header_t *ph_header,
		ph_v20_tttr_header_t *tttr_header,
		tttr_t *tttr);
int ph_v20_t3_decode(FILE *stream_in, tttr_t *tttr, t3_t *t3);
int ph_v20_tttr_stream(FILE *stream_in, FILE *stream_out, 
		pq_header_t *pq_header, ph_v20_header_t *ph_header, 
		options_t *options);
int ph_v20_t2_stream(FILE *stream_in, FILE *stream_out, 
		ph_v20_header_t *ph_header, ph_v20_tttr_header_t *tttr_header,
		options_t *options);
int ph_v20_t3_stream(FILE *stream_in, FILE *stream_out, 
		ph_v20_header_t *ph_header, ph_v20_tttr_header_t *tttr_header,
		options_t *options);

int ph_v20_tttr_header_read(FILE *stream_in, 
		ph_v20_tttr_header_t **tttr_header);
void ph_v20_tttr_header_free(ph_v20_tttr_header_t **tttr_header);
void ph_v20_tttr_header_printf(FILE *stream_out, 
		ph_v20_tttr_header_t *tttr_header);
void ph_v20_tttr_header_fwrite(FILE *stream_out,
		ph_v20_tttr_header_t *tttr_header);

#endif
