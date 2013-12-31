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

#ifndef HH_V20_H_
#define HH_V20_H_

#include <stdio.h>
#include "../picoquant.h"

#pragma pack(2)
typedef struct {
	int32_t MapTo;
	int32_t Show;
} hh_v20_display_curve_t;

typedef struct {
	float32_t Start;
	float32_t Step;
	float32_t Stop;
} hh_v20_param_t;

typedef struct {
	int32_t Model;
	int32_t Version;
} hh_v20_module_t;

typedef struct {
	int32_t ModuleIdx;
	int32_t CFDLevel;
	int32_t CFDZeroCross;
	int32_t Offset;
} hh_v20_input_channel_t;

typedef struct {
	int32_t CurveIndex;
	time32_t TimeOfRecording;
	char HardwareIdent[16];
	char HardwareVersion[8];
	int32_t HardwareSerial;
	int32_t NoOfModules;
	hh_v20_module_t Module[10];
	float64_t BaseResolution;
	int64_t InputsEnabled;
	int32_t InpChanPresent;
	int32_t RefClockSource;
	int32_t ExtDevices;
	int32_t MarkerSettings;
	int32_t SyncDivider;
	int32_t SyncCFDLevel;
	int32_t SyncCFDZero;
	int32_t SyncOffset;
	int32_t InpModuleIdx;
	int32_t InpCFDLevel;
	int32_t InpCFDZeroCross;
	int32_t InpOffset;
	int32_t InpChannel;
	int32_t MeasMode;
	int32_t SubMode;
	int32_t Binning;
	float64_t Resolution;
	int32_t Offset;
	int32_t AcquisitionTime;
	int32_t StopAfter;
	int32_t StopReason;
	float32_t P1;
	float32_t P2;
	float32_t P3;
	int32_t SyncRate;
	int32_t InputRate;
	int32_t HistCountRate;
	int64_t IntegralCount;
	int32_t HistogramBins;
	int32_t DataOffset;
} hh_v20_curve_t;

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

	int32_t ActiveCurve;
	int32_t MeasurementMode;
	int32_t SubMode;
	int32_t Binning;
	float64_t Resolution;
	int32_t Offset;
	int32_t AcquisitionTime;
	int32_t StopAt;
	int32_t StopOnOvfl;
	int32_t Restart;
	int32_t DisplayLinLog;
	uint32_t DisplayTimeAxisFrom;
	uint32_t DisplayTimeAxisTo;
	uint32_t DisplayCountAxisFrom;
	uint32_t DisplayCountAxisTo;
	hh_v20_display_curve_t DisplayCurve[8];
	hh_v20_param_t Param[3];
	int32_t RepeatMode;
	int32_t RepeatsPerCurve;
	int32_t RepeatTime;
	int32_t RepeatWaitTime;
	char ScriptName[20];
	char HardwareIdent[16];
	char HardwarePartNo[8];
	int32_t HardwareSerial;
	int32_t NumberOfModules;
	hh_v20_module_t ModuleInfo[10];
	float64_t BaseResolution;
	int64_t InputsEnabled;
	int32_t InputChannelsPresent;
	int32_t RefClockSource;
	int32_t ExtDevices;
	int32_t MarkerSettings;
	int32_t SyncDivider;
	int32_t SyncCFDLevel;
	int32_t SyncCFDZeroCross;
	int32_t SyncOffset;
	hh_v20_input_channel_t *InpChan;
	int32_t *InputRate;
} hh_v20_header_t;

typedef struct {
	hh_v20_curve_t *Curve;
	uint32_t **Counts;
} hh_v20_interactive_t;

typedef struct {
	int32_t SyncRate;
	int32_t StopAfter;
	int32_t StopReason;
	int32_t ImgHdrSize;
	int64_t NumRecords;
	uint32_t *ImgHdr;
} hh_v20_tttr_header_t;

typedef struct {
	/* Note that this order is opposite that of the manual (see ht2demo.c)*/
	uint32_t time: 25;
	uint32_t channel: 6;
	uint32_t special: 1;
} hh_v20_t2_record_t;

typedef struct {
	uint32_t nsync: 10;
	uint32_t dtime: 15;
	uint32_t channel: 6;
	uint32_t special: 1;
} hh_v20_t3_record_t;

/* After defining the structures, there a few well-characterized things we want
 * to do to them, such as reading and freeing.
 */
int hh_v20_dispatch(FILE *stream_in, FILE *stream_out, pq_header_t *pq_header,
		options_t *options);

hh_v20_header_t *hh_v20_header_alloc(int input_channels);
int hh_v20_header_read(FILE *stream_in, hh_v20_header_t **hh_header);
void hh_v20_header_free(hh_v20_header_t **hh_header);
void hh_v20_header_printf(FILE *stream_out, hh_v20_header_t *hh_header);
void hh_v20_header_fwrite(FILE *stream_out, hh_v20_header_t *hh_header);

int hh_v20_interactive_stream(FILE *stream_in, FILE *stream_out,
		pq_header_t *pq_header, hh_v20_header_t *hh_header, 
		options_t *options);

int hh_v20_interactive_header_read(FILE *stream_in, 
		hh_v20_header_t *hh_header,
		hh_v20_interactive_t **interactive);
void hh_v20_interactive_header_free(hh_v20_interactive_t **interactive);
void hh_v20_interactive_header_printf(FILE *stream_out, 
		hh_v20_header_t *hh_header,
		hh_v20_interactive_t *interactive);
void hh_v20_interactive_header_fwrite(FILE *stream_out,
		hh_v20_header_t *hh_header,
		hh_v20_interactive_t *interactive);

int hh_v20_interactive_data_read(FILE *stream_in,
		hh_v20_header_t *hh_header,
		hh_v20_interactive_t *interactive);
void hh_v20_interactive_data_free(hh_v20_header_t *hh_header,
		hh_v20_interactive_t *interactive);
void hh_v20_interactive_data_print(FILE *stream_out,
		hh_v20_header_t *hh_header,
		hh_v20_interactive_t *interactive,
		options_t *options);

void hh_v20_t2_init(hh_v20_header_t *hh_header, 
		hh_v20_tttr_header_t *tttr_header,
		tttr_t *tttr);
int hh_v20_t2_decode(FILE *stream_in, tttr_t *tttr, t2_t *t2);
void hh_v20_t3_init(hh_v20_header_t *hh_header, 
		hh_v20_tttr_header_t *tttr_header,
		tttr_t *tttr);
int hh_v20_t3_decode(FILE *stream_in, tttr_t *tttr, t3_t *t3);
int hh_v20_tttr_stream(FILE *stream_in, FILE *stream_out, 
		pq_header_t *pq_header, hh_v20_header_t *hh_header, 
		options_t *options);
int hh_v20_t2_stream(FILE *stream_in, FILE *stream_out, 
		hh_v20_header_t *hh_header,
		hh_v20_tttr_header_t *tttr_header, options_t *options);
int hh_v20_t3_stream(FILE *stream_in, FILE *stream_out,
		hh_v20_header_t *hh_header, hh_v20_tttr_header_t *tttr_header, 
		options_t *options);

int hh_v20_tttr_header_read(FILE *stream_in, 
		hh_v20_tttr_header_t **tttr_header);
void hh_v20_tttr_header_free(hh_v20_tttr_header_t **tttr_header);
void hh_v20_tttr_header_printf(FILE *stream_out, 
		hh_v20_tttr_header_t *tttr_header);
void hh_v20_tttr_header_fwrite(FILE *stream_out, 
		hh_v20_tttr_header_t *tttr_header);

#endif
