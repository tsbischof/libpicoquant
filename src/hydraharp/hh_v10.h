#ifndef HH_V10_H_
#define HH_V10_H_

#include <stdio.h>
#include "../picoquant.h"

#pragma pack(2)
typedef struct {
	int32_t MapTo;
	int32_t Show;
} hh_v10_display_curve_t;

typedef struct {
	float32_t Start;
	float32_t Step;
	float32_t Stop;
} hh_v10_param_t;

typedef struct {
	int32_t Model;
	int32_t Version;
} hh_v10_module_t;

typedef struct {
	int32_t ModuleIdx;
	int32_t CFDLevel;
	int32_t CFDZeroCross;
	int32_t Offset;
} hh_v10_input_channel_t;

typedef struct {
	int32_t CurveIndex;
	time32_t TimeOfRecording;
	char HardwareIdent[16];
	char HardwareVersion[8];
	int32_t HardwareSerial;
	int32_t NoOfModules;
	hh_v10_module_t Module[10];
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
} hh_v10_curve_t;

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
	int32_t DisplayTimeAxisFrom;
	int32_t DisplayTimeAxisTo;
	int32_t DisplayCountAxisFrom;
	int32_t DisplayCountAxisTo;
	hh_v10_display_curve_t DisplayCurve[8];
	hh_v10_param_t Param[3];
	int32_t RepeatMode;
	int32_t RepeatsPerCurve;
	int32_t RepeatTime;
	int32_t RepeatWaitTime;
	char ScriptName[20];
	char HardwareIdent[16];
	char HardwarePartNo[8];
	int32_t HardwareSerial;
	int32_t NumberOfModules;
	hh_v10_module_t ModuleInfo[10];
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
	hh_v10_input_channel_t *InpChan;
	int32_t *InputRate;
} hh_v10_header_t;

typedef struct {
	hh_v10_curve_t *Curve;
	uint32_t **Counts;
} hh_v10_interactive_t;

typedef struct {
	int32_t SyncRate;
	int32_t StopAfter;
	int32_t StopReason;
	int32_t ImgHdrSize;
	int64_t NumRecords;
	uint32_t *ImgHdr;
} hh_v10_tttr_header_t;

typedef struct {
	/* Note that this order is opposite that of the manual (see ht2demo.c)*/
	uint32_t time: 25;
	uint32_t channel: 6;
	uint32_t special: 1;
} hh_v10_t2_record_t;

typedef struct {
	uint32_t nsync: 10;
	uint32_t dtime: 15;
	uint32_t channel: 6;
	uint32_t special: 1;
} hh_v10_t3_record_t;

/* After defining the structures, there a few well-characterized things we want
 * to do to them, such as reading and freeing.
 */
int hh_v10_dispatch(FILE *in_stream, FILE *out_stream, pq_header_t *pq_header,
		options_t *options);

int hh_v10_header_read(FILE *in_stream, hh_v10_header_t *hh_header,
		options_t *options);
void hh_v10_header_free(hh_v10_header_t *hh_header);
void hh_v10_header_print(FILE *out_stream,
	       hh_v10_header_t *hh_header);

int hh_v10_interactive_header_read(FILE *in_stream, 
		hh_v10_header_t *hh_header,
		hh_v10_interactive_t *interactive, options_t *options);
int hh_v10_interactive_data_read(FILE *in_stream,
		hh_v10_header_t *hh_header,
		hh_v10_interactive_t *interactive, options_t *options);
void hh_v10_interactive_header_free(hh_v10_interactive_t *interactive);
void hh_v10_interactive_data_free(hh_v10_header_t *hh_header,
		hh_v10_interactive_t *interactive);
void hh_v10_interactive_header_print(FILE *out_stream, 
		hh_v10_header_t *hh_header,
		hh_v10_interactive_t *interactive);
void hh_v10_interactive_data_print(FILE *out_stream,
		hh_v10_header_t *hh_header,
		hh_v10_interactive_t *interactive,
		options_t *options);
int hh_v10_interactive_stream(FILE *in_stream, FILE *out_stream,
		pq_header_t *pq_header, hh_v10_header_t *hh_header, 
		options_t *options);

int hh_v10_tttr_header_read(FILE *in_stream, hh_v10_tttr_header_t *tttr_header,
		options_t *options);
void hh_v10_tttr_header_print(FILE *out_stream, 
		hh_v10_tttr_header_t *tttr_header);
void hh_v10_tttr_header_free(hh_v10_tttr_header_t *tttr_header);

int hh_v10_t2_record_stream(FILE *in_stream, FILE *out_stream, 
		hh_v10_header_t *hh_header,
		hh_v10_tttr_header_t *tttr_header, options_t *options);
int hh_v10_t3_record_stream(FILE *in_stream, FILE *out_stream,
		hh_v10_header_t *hh_header,
		hh_v10_tttr_header_t *tttr_header, options_t *options);
int hh_v10_tttr_stream(FILE *in_stream, FILE *out_stream, 
		pq_header_t *pq_header, hh_v10_header_t *hh_header, 
		options_t *options);

#endif
