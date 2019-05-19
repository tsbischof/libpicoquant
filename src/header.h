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

#ifndef HEADER_H_
#define HEADER_H_

#include <stdio.h>
#include "picoquant.h"


#include  <ncurses.h>

#include  <stddef.h>
#include  <stdlib.h>
#include  <time.h>


// TagTypes  (TagHead.Typ)
#define tyEmpty8      0xFFFF0008
#define tyBool8       0x00000008
#define tyInt8        0x10000008
#define tyBitSet64    0x11000008
#define tyColor8      0x12000008
#define tyFloat8      0x20000008
#define tyTDateTime   0x21000008
#define tyFloat8Array 0x2001FFFF
#define tyAnsiString  0x4001FFFF
#define tyWideString  0x4002FFFF
#define tyBinaryBlob  0xFFFFFFFF

// RecordTypes
#define rtPicoHarpT3     0x00010303    // (SubID = $00 ,RecFmt: $01) (V1), T-Mode: $03 (T3), HW: $03 (PicoHarp)
#define rtPicoHarpT2     0x00010203    // (SubID = $00 ,RecFmt: $01) (V1), T-Mode: $02 (T2), HW: $03 (PicoHarp)
#define rtHydraHarpT3    0x00010304    // (SubID = $00 ,RecFmt: $01) (V1), T-Mode: $03 (T3), HW: $04 (HydraHarp)
#define rtHydraHarpT2    0x00010204    // (SubID = $00 ,RecFmt: $01) (V1), T-Mode: $02 (T2), HW: $04 (HydraHarp)
#define rtHydraHarp2T3   0x01010304    // (SubID = $01 ,RecFmt: $01) (V2), T-Mode: $03 (T3), HW: $04 (HydraHarp)
#define rtHydraHarp2T2   0x01010204    // (SubID = $01 ,RecFmt: $01) (V2), T-Mode: $02 (T2), HW: $04 (HydraHarp)
#define rtTimeHarp260NT3 0x00010305    // (SubID = $00 ,RecFmt: $01) (V2), T-Mode: $03 (T3), HW: $05 (TimeHarp260N)
#define rtTimeHarp260NT2 0x00010205    // (SubID = $00 ,RecFmt: $01) (V2), T-Mode: $02 (T2), HW: $05 (TimeHarp260N)
#define rtTimeHarp260PT3 0x00010306    // (SubID = $00 ,RecFmt: $01) (V1), T-Mode: $02 (T3), HW: $06 (TimeHarp260P)
#define rtTimeHarp260PT2 0x00010206    // (SubID = $00 ,RecFmt: $01) (V1), T-Mode: $02 (T2), HW: $06 (TimeHarp260P)

// some important Tag Idents (TagHead.Ident) that we will need to read the most common content of a PTU file
// check the output of this program and consult the tag dictionary if you need more
#define name_TTResultFormat_TTTRRecType "TTResultFormat_TTTRRecType"
#define name_TTResult_NumberOfRecords  "TTResult_NumberOfRecords"  // Number of TTTR Records in the File;
#define name_MeasDesc_Resolution         "MeasDesc_Resolution"       // Resolution for the Dtime (T3 Only)
#define name_MeasDesc_GlobalResolution     "MeasDesc_GlobalResolution" // Global Resolution of TimeTag(T2) /NSync (T3)
#define name_Header_End         "Header_End"                // Always appended as last tag (BLOCKEND)

// The rest of the Tag Idents: We do this so that if the name in the header given changes, you only
// need to change it up here, not in the rest of the code
#define name_File_GUID "File_GUID" //tyAnsiString
#define name_File_AssuredContent "File_AssuredContent" //tyAnsiString
#define name_CreatorSW_ContentVersion "CreatorSW_ContentVersion" //tyAnsiString
#define name_CreatorSW_Name "CreatorSW_Name" //tyAnsiString
#define name_CreatorSW_Version "CreatorSW_Version" //tyAnsiString
#define name_File_CreatingTime "File_CreatingTime" //tyTDateTime
#define name_File_Comment "File_Comment" //tyAnsiString
#define name_Measurement_Mode "Measurement_Mode" //tyInt8
#define name_Measurement_SubMode "Measurement_SubMode" //tyInt8
#define name_TTResult_StopReason "TTResult_StopReason" //tyInt8
#define name_Fast_Load_End "Fast_Load_End" //empty tag?

#define name_TTResultFormat_BitsPerRecord "TTResultFormat_BitsPerRecord" //tyInt8
#define name_MeasDesc_BinningFactor "MeasDesc_BinningFactor" //tyInt8
#define name_MeasDesc_Offset "MeasDesc_Offset" //tyInt8
#define name_MeasDesc_AcquisitionTime "MeasDesc_AcquisitionTime" //tyInt8
#define name_MeasDesc_StopAt "MeasDesc_StopAt" //tyInt8
#define name_MeasDesc_StopOnOvfl "MeasDesc_StopOnOvfl" //tyBool8
#define name_MeasDesc_Restart "MeasDesc_Restart" //tyBool8
#define name_CurSWSetting_DispLog "CurSWSetting_DispLog" //tyBool8
#define name_CurSWSetting_DispAxisTimeFrom "CurSWSetting_DispAxisTimeFrom" //tyInt8
#define name_CurSWSetting_DispAxisTimeTo "CurSWSetting_DispAxisTimeTo" //tyInt8
#define name_CurSWSetting_DispAxisCountFrom "CurSWSetting_DispAxisCountFrom" //tyInt8
#define name_CurSWSetting_DispAxisCountTo "CurSWSetting_DispAxisCountTo" //tyInt8
#define name_CurSWSetting_DispCurves "CurSWSetting_DispCurves" //tyInt8
#define name_CurSWSetting_DispCurve_MapTo0 "CurSWSetting_DispCurve_MapTo(0)" //tyInt8
#define name_CurSWSetting_DispCurve_Show0 "CurSWSetting_DispCurve_Show(0)" //tyBool8
#define name_CurSWSetting_DispCurve_MapTo1 "CurSWSetting_DispCurve_MapTo(1)" //tyInt8
#define name_CurSWSetting_DispCurve_Show1 "CurSWSetting_DispCurve_Show(1)" //tyBool8
#define name_CurSWSetting_DispCurve_MapTo2 "CurSWSetting_DispCurve_MapTo(2)" //tyInt8
#define name_CurSWSetting_DispCurve_Show2 "CurSWSetting_DispCurve_Show(2)" //tyBool8
#define name_CurSWSetting_DispCurve_MapTo3 "CurSWSetting_DispCurve_MapTo(3)" //tyInt8
#define name_CurSWSetting_DispCurve_Show3 "CurSWSetting_DispCurve_Show(3)" //tyBool8
#define name_CurSWSetting_DispCurve_MapTo4 "CurSWSetting_DispCurve_MapTo(4)" //tyInt8
#define name_CurSWSetting_DispCurve_Show4 "CurSWSetting_DispCurve_Show(4)" //tyBool8
#define name_CurSWSetting_DispCurve_MapTo5 "CurSWSetting_DispCurve_MapTo(5)" //tyInt8
#define name_CurSWSetting_DispCurve_Show5 "CurSWSetting_DispCurve_Show(5)" //tyBool8
#define name_CurSWSetting_DispCurve_MapTo6 "CurSWSetting_DispCurve_MapTo(6)" //tyInt8
#define name_CurSWSetting_DispCurve_Show6 "CurSWSetting_DispCurve_Show(6)" //tyBool8
#define name_CurSWSetting_DispCurve_MapTo7 "CurSWSetting_DispCurve_MapTo(7)" //tyInt8
#define name_CurSWSetting_DispCurve_Show7 "CurSWSetting_DispCurve_Show(7)" //tyBool8
#define name_HW_Type "HW_Type" //tyAnsiString
#define name_HW_PartNo "HW_PartNo" //tyAnsiString
#define name_HW_Version "HW_Version" //tyAnsiString
#define name_HW_SerialNo "HW_SerialNo" //tyAnsiString
#define name_HW_Modules "HW_Modules" //tyInt8
#define name_HWModule_TypeCode0 "HWModule_TypeCode(0)" //tyInt8
#define name_HWModule_VersCode0 "HWModule_VersCode(0)" //tyInt8
#define name_HWModule_TypeCode1 "HWModule_TypeCode(1)" //tyInt8
#define name_HWModule_VersCode1 "HWModule_VersCode(1)" //tyInt8
#define name_HWModule_TypeCode2 "HWModule_TypeCode(2)" //tyInt8
#define name_HWModule_VersCode2 "HWModule_VersCode(2)" //tyInt8
#define name_HW_BaseResolution "HW_BaseResolution" //tyFloat8
#define name_HW_InpChannels "HW_InpChannels" //tyInt8
#define name_HW_ExternalRefClock "HW_ExternalRefClock" //tyBool8
#define name_HW_ExternalDevices "HW_ExternalDevices" //tyInt8
#define name_HWSync_Divider "HWSync_Divider" //tyInt8
#define name_HWSync_CFDLevel "HWSync_CFDLevel" //tyInt8
#define name_HWSync_CFDZeroCross "HWSync_CFDZeroCross" //tyInt8
#define name_HWSync_Offset "HWSync_Offset" //tyInt8
#define name_HWInpChan_ModuleIdx0 "HWInpChan_ModuleIdx(0)" //tyInt8
#define name_HWInpChan_CFDLevel0 "HWInpChan_CFDLevel(0)" //tyInt8
#define name_HWInpChan_CFDZeroCross0 "HWInpChan_CFDZeroCross(0)" //tyInt8
#define name_HWInpChan_Offset0 "HWInpChan_Offset(0)" //tyInt8
#define name_HWInpChan_Enabled0 "HWInpChan_Enabled(0)" //tyBool8
#define name_HWInpChan_ModuleIdx1 "HWInpChan_ModuleIdx(1)" //tyInt8
#define name_HWInpChan_CFDLevel1 "HWInpChan_CFDLevel(1)" //tyInt8
#define name_HWInpChan_CFDZeroCross1 "HWInpChan_CFDZeroCross(1)" //tyInt8
#define name_HWInpChan_Offset1 "HWInpChan_Offset(1)" //tyInt8
#define name_HWInpChan_Enabled1 "HWInpChan_Enabled(1)" //tyBool8

#define name_HW_Markers "HW_Markers" //tyInt8
#define name_HWMarkers_RisingEdge0 "HWMarkers_RisingEdge(0)" //tyBool8
#define name_HWMarkers_RisingEdge1 "HWMarkers_RisingEdge(1)" //tyBool8
#define name_HWMarkers_RisingEdge2 "HWMarkers_RisingEdge(2)" //tyBool8
#define name_HWMarkers_RisingEdge3 "HWMarkers_RisingEdge(3)" //tyBool8
#define name_HWMarkers_Enabled0 "HWMarkers_Enabled(0)" //tyBool8
#define name_HWMarkers_Enabled1 "HWMarkers_Enabled(1)" //tyBool8
#define name_HWMarkers_Enabled2 "HWMarkers_Enabled(2)" //tyBool8
#define name_HWMarkers_Enabled3 "HWMarkers_Enabled(3)" //tyBool8
#define name_HWMarkers_HoldOff "HWMarkers_HoldOff" //tyInt8

#define name_TTResult_SyncRate "TTResult_SyncRate" //tyInt8
#define name_TTResult_InputRate0 "TTResult_InputRate(0)" //tyInt8
#define name_TTResult_InputRate1 "TTResult_InputRate(1)" //tyInt8
#define name_TTResult_StopAfter "TTResult_StopAfter" //tyInt8


// RecordTypes
#define rtPicoHarpT3     0x00010303    // (SubID = $00 ,RecFmt: $01) (V1), T-Mode: $03 (T3), HW: $03 (PicoHarp)
#define rtPicoHarpT2     0x00010203    // (SubID = $00 ,RecFmt: $01) (V1), T-Mode: $02 (T2), HW: $03 (PicoHarp)
#define rtHydraHarpT3    0x00010304    // (SubID = $00 ,RecFmt: $01) (V1), T-Mode: $03 (T3), HW: $04 (HydraHarp)
#define rtHydraHarpT2    0x00010204    // (SubID = $00 ,RecFmt: $01) (V1), T-Mode: $02 (T2), HW: $04 (HydraHarp)
#define rtHydraHarp2T3   0x01010304    // (SubID = $01 ,RecFmt: $01) (V2), T-Mode: $03 (T3), HW: $04 (HydraHarp)
#define rtHydraHarp2T2   0x01010204    // (SubID = $01 ,RecFmt: $01) (V2), T-Mode: $02 (T2), HW: $04 (HydraHarp)
#define rtTimeHarp260NT3 0x00010305    // (SubID = $00 ,RecFmt: $01) (V2), T-Mode: $03 (T3), HW: $05 (TimeHarp260N)
#define rtTimeHarp260NT2 0x00010205    // (SubID = $00 ,RecFmt: $01) (V2), T-Mode: $02 (T2), HW: $05 (TimeHarp260N)
#define rtTimeHarp260PT3 0x00010306    // (SubID = $00 ,RecFmt: $01) (V1), T-Mode: $02 (T3), HW: $06 (TimeHarp260P)
#define rtTimeHarp260PT2 0x00010206    // (SubID = $00 ,RecFmt: $01) (V1), T-Mode: $02 (T2), HW: $06 (TimeHarp260P)

// Enforce byte alignment to ensure cross-platform compatibility.
//#pragma pack(4)


// Define the common file header.
typedef struct {
	char Ident[16];
	char FormatVersion[6];
} pq_header_t;

typedef struct {
	char CreatorName[18];
	char CreatorVersion[12];
	char FileTime[20];
	//char CRLF[2];//seems to be unused for everyone, what does this even stand for?
	char Comment[256];
	int32_t NumberOfCurves;



	/* Note that Records is the only difference between interactive and
	 * tttr main headers. Interactive calls this BitsPerHistogBin.
	 */

	 //booleans
	 uint32_t MeasDesc_StopOnOvfl;
	 uint32_t MeasDesc_Restart;
	 uint32_t CurSWSetting_DispLog;
	 uint32_t CurSWSetting_DispCurve_Show0;
	 uint32_t CurSWSetting_DispCurve_Show1;
	 uint32_t CurSWSetting_DispCurve_Show2;
	 uint32_t CurSWSetting_DispCurve_Show3;
	 uint32_t CurSWSetting_DispCurve_Show4;
	 uint32_t CurSWSetting_DispCurve_Show5;
	 uint32_t CurSWSetting_DispCurve_Show6;
	 uint32_t CurSWSetting_DispCurve_Show7;
	 uint32_t HW_ExternalRefClock;
	 uint32_t HWInpChan_Enabled0;
	 uint32_t HWInpChan_Enabled1;
	 uint32_t HWMarkers_RisingEdge0;
	 uint32_t HWMarkers_RisingEdge1;
	 uint32_t HWMarkers_RisingEdge2;
	 uint32_t HWMarkers_RisingEdge3;
	 uint32_t HWMarkers_Enabled0;
	 uint32_t HWMarkers_Enabled1;
	 uint32_t HWMarkers_Enabled2;
	 uint32_t HWMarkers_Enabled3;

	 //int8
	 int32_t Fast_Load_End;
	 int32_t Measurement_Mode;
	 int32_t Measurement_SubMode;
	 int32_t TTResult_StopReason;
	 int32_t TTResultFormat_TTTRRecType;
	 int32_t TTResultFormat_BitsPerRecord;
	 int32_t MeasDesc_BinningFactor;
	 int32_t MeasDesc_Offset;
	 int32_t MeasDesc_AcquisitionTime;
	 int32_t MeasDesc_StopAt;
	 int32_t CurSWSetting_DispAxisTimeFrom;
	 int32_t CurSWSetting_DispAxisTimeTo;
	 int32_t CurSWSetting_DispAxisCountFrom;
	 int32_t CurSWSetting_DispAxisCountTo;
	 int32_t CurSWSetting_DispCurves;
	 int32_t CurSWSetting_DispCurve_MapTo0;
	 int32_t CurSWSetting_DispCurve_MapTo1;
	 int32_t CurSWSetting_DispCurve_MapTo2;
	 int32_t CurSWSetting_DispCurve_MapTo3;
	 int32_t CurSWSetting_DispCurve_MapTo4;
	 int32_t CurSWSetting_DispCurve_MapTo5;
	 int32_t CurSWSetting_DispCurve_MapTo6;
	 int32_t CurSWSetting_DispCurve_MapTo7;
	 int32_t HW_Modules;
	 int32_t HWModule_TypeCode0;
	 int32_t HWModule_TypeCode1;
	 int32_t HWModule_TypeCode2;
	 int32_t HWModule_VersCode0;
	 int32_t HWModule_VersCode1;
	 int32_t HWModule_VersCode2;
	 int32_t HW_InpChannels;
	 int32_t HW_ExternalDevices;
	 int32_t HWSync_Divider;
	 int32_t HWSync_CFDLevel;
	 int32_t HWSync_CFDZeroCross;
	 int32_t HWSync_Offset;
	 int32_t HWInpChan_ModuleIdx0;
	 int32_t HWInpChan_ModuleIdx1;
	 int32_t HWInpChan_CFDLevel0;
	 int32_t HWInpChan_CFDLevel1;
	 int32_t HWInpChan_CFDZeroCross0;
	 int32_t HWInpChan_CFDZeroCross1;
	 int32_t HWInpChan_Offset0;
	 int32_t HWInpChan_Offset1;
	 int32_t HW_Markers;
	 int32_t HWMarkers_HoldOff;
	 int32_t TTResult_SyncRate;
	 int32_t TTResult_InputRate0;
	 int32_t TTResult_InputRate1;
	 int32_t TTResult_StopAfter;
	 int32_t TTResult_NumberOfRecords;

	 //Float8
	 float64_t HW_BaseResolution;
	 float64_t MeasDesc_Resolution;
	 float64_t MeasDesc_GlobalResolution;

	 //AnsiStrings not in general part
	 char File_GUID[40];
	 char File_AssuredContent[32];
	 char CreatorSW_ContentVersion[4];
	 char HW_Type[15];
	 char HW_PartNo[8];
	 char HW_Version[5];
	 char HW_SerialNo[8];

/*
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

	//whats up with this??? Why only point?? *******************************************
	hh_v20_input_channel_t *InpChan;
	int32_t *InputRate;*/
} ptu_header_t;

// A Tag entry
typedef struct {
  char Ident[32];     // Identifier of the tag
  int Idx;            // Index for multiple tags or -1
  uint32_t Typ;  // Type of tag ty..... see const section
    long long TagValue; // Value of tag.
} TagHead_t;

time_t TDateTime_TimeT(double Convertee);

int pq_header_read(FILE *in_stream, pq_header_t *pq_header);
int ptu_header_parse(FILE *in_stream, ptu_header_t *ptu_header);

void pq_header_printf(FILE *out_stream, pq_header_t *pq_header);
void pq_header_fwrite(FILE *out_stream, pq_header_t *pq_header);
void ptu_header_fwrite(FILE *stream_out, ptu_header_t *ptu_header);
void ptu_header_printf(FILE *out_stream, ptu_header_t *ptu_header);

#endif
