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


#include "error.h"
#include "picoquant.h"

int pq_header_read(FILE *stream_in, pq_header_t *pq_header) {
	size_t n_read;

	n_read = fread(pq_header, sizeof(pq_header_t), 1, stream_in);
	if ( n_read == 1 ) {
		return(PQ_SUCCESS);
	} else {
		return(PQ_ERROR_IO);
	}
}

int ptu_header_parse(FILE *in_stream, ptu_header_t *ptu_header){
  char Magic[8];
  char Version[8];
  char Buffer[40];
  char* AnsiBuffer;
  //WCHAR* WideBuffer;
  int Result;

  //long long NumRecords = -1;
  //long long RecordType = 0;

  TagHead_t TagHead;
  fseek(in_stream, sizeof(Magic)+sizeof(Version), SEEK_SET);//make sure we start at the right place
  // read tagged header
  do{
    Result = fread( &TagHead, 1, sizeof(TagHead) ,in_stream);
    if (Result!= sizeof(TagHead))
    {
        error("Incomplete file in header of ptu");
    }

    strcpy(Buffer, TagHead.Ident);
    if (TagHead.Idx > -1)
    {
      sprintf(Buffer, "%s(%d)", TagHead.Ident,TagHead.Idx);
    }
    //fprintf(fpout, "\n%-40s", Buffer);
    switch (TagHead.Typ)
    {
      case tyEmpty8:
        if (strcmp(TagHead.Ident, name_Fast_Load_End)==0){
          ptu_header->Fast_Load_End = TagHead.TagValue ;//just kept everything  
        }
        //fprintf(fpout, "<empty Tag>");
        break;
      case tyBool8:
        if (strcmp(TagHead.Ident, name_MeasDesc_StopOnOvfl)==0){
          ptu_header->MeasDesc_StopOnOvfl = TagHead.TagValue ;  
        } else if (strcmp(TagHead.Ident, name_MeasDesc_Restart)==0){
          ptu_header->MeasDesc_Restart = TagHead.TagValue;   
        } else if (strcmp(TagHead.Ident, name_CurSWSetting_DispLog)==0){
          ptu_header->CurSWSetting_DispLog = TagHead.TagValue ;  
        } else if (strcmp(TagHead.Ident, name_CurSWSetting_DispCurve_Show0)==0){
          ptu_header->CurSWSetting_DispCurve_Show0 = TagHead.TagValue;   
        } else if (strcmp(TagHead.Ident, name_CurSWSetting_DispCurve_Show1)==0){
          ptu_header->CurSWSetting_DispCurve_Show1 = TagHead.TagValue;   
        } else if (strcmp(TagHead.Ident, name_CurSWSetting_DispCurve_Show2)==0){
          ptu_header->CurSWSetting_DispCurve_Show2 = TagHead.TagValue;   
        } else if (strcmp(TagHead.Ident, name_CurSWSetting_DispCurve_Show3)==0){
          ptu_header->CurSWSetting_DispCurve_Show3 = TagHead.TagValue;   
        } else if (strcmp(TagHead.Ident, name_CurSWSetting_DispCurve_Show4)==0){
          ptu_header->CurSWSetting_DispCurve_Show4 = TagHead.TagValue  ; 
        } else if (strcmp(TagHead.Ident, name_CurSWSetting_DispCurve_Show5)==0){
          ptu_header->CurSWSetting_DispCurve_Show5 = TagHead.TagValue;   
        } else if (strcmp(TagHead.Ident, name_CurSWSetting_DispCurve_Show6)==0){
          ptu_header->CurSWSetting_DispCurve_Show6 = TagHead.TagValue;   
        } else if (strcmp(TagHead.Ident, name_CurSWSetting_DispCurve_Show7)==0){
          ptu_header->CurSWSetting_DispCurve_Show7 = TagHead.TagValue ;  
        } else if (strcmp(TagHead.Ident, name_HW_ExternalRefClock)==0){
          ptu_header->HW_ExternalRefClock = TagHead.TagValue ;  
        } else if (strcmp(TagHead.Ident, name_HWInpChan_Enabled0)==0){
          ptu_header->HWInpChan_Enabled0 = TagHead.TagValue;   
        } else if (strcmp(TagHead.Ident, name_HWInpChan_Enabled1)==0){
          ptu_header->HWInpChan_Enabled1 = TagHead.TagValue ;  
        } else if (strcmp(TagHead.Ident, name_HWMarkers_RisingEdge0)==0){
          ptu_header->HWMarkers_RisingEdge0 = TagHead.TagValue  ; 
        } else if (strcmp(TagHead.Ident, name_HWMarkers_RisingEdge1)==0){
          ptu_header->HWMarkers_RisingEdge1 = TagHead.TagValue ;  
        } else if (strcmp(TagHead.Ident, name_HWMarkers_RisingEdge2)==0){
          ptu_header->HWMarkers_RisingEdge2 = TagHead.TagValue ;  
        } else if (strcmp(TagHead.Ident, name_HWMarkers_RisingEdge3)==0){
          ptu_header->HWMarkers_RisingEdge3 = TagHead.TagValue;   
        } else if (strcmp(TagHead.Ident, name_HWMarkers_Enabled0)==0){
          ptu_header->HWMarkers_Enabled0 = TagHead.TagValue  ; 
        } else if (strcmp(TagHead.Ident, name_HWMarkers_Enabled1)==0){
          ptu_header->HWMarkers_Enabled1 = TagHead.TagValue ;  
        } else if (strcmp(TagHead.Ident, name_HWMarkers_Enabled2)==0){
          ptu_header->HWMarkers_Enabled2 = TagHead.TagValue ;  
        } else if (strcmp(TagHead.Ident, name_HWMarkers_Enabled3)==0){
          ptu_header->HWMarkers_Enabled3 = TagHead.TagValue;   
        }
        //fprintf(fpout, "%s", bool(TagHead.TagValue)?"True":"False");
        //fprintf(fpout, "  tyBool8");
        break;
      case tyInt8:
        if (strcmp(TagHead.Ident, name_Measurement_Mode)==0){
          ptu_header->Measurement_Mode = TagHead.TagValue ;  
        } else if (strcmp(TagHead.Ident, name_Measurement_SubMode)==0){
          ptu_header->Measurement_SubMode = TagHead.TagValue;
        } else if (strcmp(TagHead.Ident, name_TTResult_StopReason)==0){
          ptu_header->TTResult_StopReason = TagHead.TagValue;
        } else if (strcmp(TagHead.Ident, name_TTResultFormat_TTTRRecType)==0){
          ptu_header->TTResultFormat_TTTRRecType = TagHead.TagValue;
        } else if (strcmp(TagHead.Ident, name_TTResultFormat_BitsPerRecord)==0){
          ptu_header->TTResultFormat_BitsPerRecord = TagHead.TagValue;
        } else if (strcmp(TagHead.Ident, name_MeasDesc_BinningFactor)==0){
          ptu_header->MeasDesc_BinningFactor = TagHead.TagValue;
        } else if (strcmp(TagHead.Ident, name_MeasDesc_Offset)==0){
          ptu_header->MeasDesc_Offset = TagHead.TagValue;
        } else if (strcmp(TagHead.Ident, name_MeasDesc_AcquisitionTime)==0){
          ptu_header->MeasDesc_AcquisitionTime = TagHead.TagValue;
        } else if (strcmp(TagHead.Ident, name_MeasDesc_StopAt)==0){
          ptu_header->MeasDesc_StopAt = TagHead.TagValue;
        } else if (strcmp(TagHead.Ident, name_CurSWSetting_DispAxisTimeFrom)==0){
          ptu_header->CurSWSetting_DispAxisTimeFrom = TagHead.TagValue;
        } else if (strcmp(TagHead.Ident, name_CurSWSetting_DispAxisTimeTo)==0){
          ptu_header->CurSWSetting_DispAxisTimeTo = TagHead.TagValue;
        } else if (strcmp(TagHead.Ident, name_CurSWSetting_DispAxisCountFrom)==0){
          ptu_header->CurSWSetting_DispAxisCountFrom = TagHead.TagValue;
        } else if (strcmp(TagHead.Ident, name_CurSWSetting_DispAxisCountTo)==0){
          ptu_header->CurSWSetting_DispAxisCountTo = TagHead.TagValue;
        } else if (strcmp(TagHead.Ident, name_CurSWSetting_DispCurves)==0){
          ptu_header->CurSWSetting_DispCurves = TagHead.TagValue;
        } else if (strcmp(TagHead.Ident, name_CurSWSetting_DispCurve_MapTo0)==0){
          ptu_header->CurSWSetting_DispCurve_MapTo0 = TagHead.TagValue;
        } else if (strcmp(TagHead.Ident, name_CurSWSetting_DispCurve_MapTo1)==0){
          ptu_header->CurSWSetting_DispCurve_MapTo1 = TagHead.TagValue;
        } else if (strcmp(TagHead.Ident, name_CurSWSetting_DispCurve_MapTo2)==0){
          ptu_header->CurSWSetting_DispCurve_MapTo2 = TagHead.TagValue;
        } else if (strcmp(TagHead.Ident, name_CurSWSetting_DispCurve_MapTo3)==0){
          ptu_header->CurSWSetting_DispCurve_MapTo3 = TagHead.TagValue;
        } else if (strcmp(TagHead.Ident, name_CurSWSetting_DispCurve_MapTo4)==0){
          ptu_header->CurSWSetting_DispCurve_MapTo4 = TagHead.TagValue;
        } else if (strcmp(TagHead.Ident, name_CurSWSetting_DispCurve_MapTo5)==0){
          ptu_header->CurSWSetting_DispCurve_MapTo5 = TagHead.TagValue;
        } else if (strcmp(TagHead.Ident, name_CurSWSetting_DispCurve_MapTo6)==0){
          ptu_header->CurSWSetting_DispCurve_MapTo6 = TagHead.TagValue;
        } else if (strcmp(TagHead.Ident, name_CurSWSetting_DispCurve_MapTo7)==0){
          ptu_header->CurSWSetting_DispCurve_MapTo7 = TagHead.TagValue;
        } else if (strcmp(TagHead.Ident, name_HW_Modules )==0){
          ptu_header->HW_Modules  = TagHead.TagValue;
        } else if (strcmp(TagHead.Ident, name_HWModule_TypeCode0)==0){
          ptu_header->HWModule_TypeCode0 = TagHead.TagValue;
        } else if (strcmp(TagHead.Ident, name_HWModule_TypeCode1)==0){
          ptu_header->HWModule_TypeCode1 = TagHead.TagValue;
        } else if (strcmp(TagHead.Ident, name_HWModule_TypeCode2)==0){
          ptu_header->HWModule_TypeCode2 = TagHead.TagValue;
        } else if (strcmp(TagHead.Ident, name_HWModule_VersCode0)==0){
          ptu_header->HWModule_VersCode0 = TagHead.TagValue;
        } else if (strcmp(TagHead.Ident, name_HWModule_VersCode1)==0){
          ptu_header->HWModule_VersCode1 = TagHead.TagValue;
        } else if (strcmp(TagHead.Ident, name_HWModule_VersCode2)==0){
          ptu_header->HWModule_VersCode2 = TagHead.TagValue;
        } else if (strcmp(TagHead.Ident, name_HW_InpChannels)==0){
          ptu_header->HW_InpChannels = TagHead.TagValue;
        } else if (strcmp(TagHead.Ident, name_HW_ExternalDevices)==0){
          ptu_header->HW_ExternalDevices = TagHead.TagValue;
        } else if (strcmp(TagHead.Ident, name_HWSync_Divider)==0){
          ptu_header->HWSync_Divider = TagHead.TagValue;
        } else if (strcmp(TagHead.Ident, name_HWSync_CFDLevel)==0){
          ptu_header->HWSync_CFDLevel = TagHead.TagValue;
        } else if (strcmp(TagHead.Ident, name_HWSync_CFDZeroCross)==0){
          ptu_header->HWSync_CFDZeroCross = TagHead.TagValue;
        } else if (strcmp(TagHead.Ident, name_HWSync_Offset)==0){
          ptu_header->HWSync_Offset = TagHead.TagValue;
        } else if (strcmp(TagHead.Ident, name_HWInpChan_ModuleIdx0)==0){
          ptu_header->HWInpChan_ModuleIdx0 = TagHead.TagValue;
        } else if (strcmp(TagHead.Ident, name_HWInpChan_ModuleIdx1)==0){
          ptu_header->HWInpChan_ModuleIdx1 = TagHead.TagValue;
        } else if (strcmp(TagHead.Ident, name_HWInpChan_CFDLevel0)==0){
          ptu_header->HWInpChan_CFDLevel0 = TagHead.TagValue;
        } else if (strcmp(TagHead.Ident, name_HWInpChan_CFDLevel1)==0){
          ptu_header->HWInpChan_CFDLevel1 = TagHead.TagValue;
        } else if (strcmp(TagHead.Ident, name_HWInpChan_CFDZeroCross0)==0){
          ptu_header->HWInpChan_CFDZeroCross0 = TagHead.TagValue;
        } else if (strcmp(TagHead.Ident, name_HWInpChan_CFDZeroCross1)==0){
          ptu_header->HWInpChan_CFDZeroCross1 = TagHead.TagValue;
        } else if (strcmp(TagHead.Ident, name_HWInpChan_Offset0)==0){
          ptu_header->HWInpChan_Offset0 = TagHead.TagValue;
        } else if (strcmp(TagHead.Ident, name_HWInpChan_Offset1)==0){
          ptu_header->HWInpChan_Offset1 = TagHead.TagValue;
        } else if (strcmp(TagHead.Ident, name_HW_Markers)==0){
          ptu_header->HW_Markers = TagHead.TagValue;
        } else if (strcmp(TagHead.Ident, name_HWMarkers_HoldOff )==0){
          ptu_header->HWMarkers_HoldOff = TagHead.TagValue;
        } else if (strcmp(TagHead.Ident, name_TTResult_SyncRate)==0){
          ptu_header->TTResult_SyncRate = TagHead.TagValue;
        } else if (strcmp(TagHead.Ident, name_TTResult_InputRate0)==0){
          ptu_header->TTResult_InputRate0 = TagHead.TagValue;
        } else if (strcmp(TagHead.Ident, name_TTResult_InputRate1)==0){
          ptu_header->TTResult_InputRate1 = TagHead.TagValue;
        } else if (strcmp(TagHead.Ident, name_TTResult_StopAfter)==0){
          ptu_header->TTResult_StopAfter = TagHead.TagValue;
        } else if (strcmp(TagHead.Ident, name_TTResult_NumberOfRecords)==0){
          ptu_header->TTResult_NumberOfRecords = TagHead.TagValue;
        }
        /*fprintf(fpout, "%lld", TagHead.TagValue);
        fprintf(fpout, "  tyInt8");
        // get some Values we need to analyse records
        if (strcmp(TagHead.Ident, TTTRTagNumRecords)==0) // Number of records
          NumRecords = TagHead.TagValue;
        if (strcmp(TagHead.Ident, TTTRTagTTTRRecType)==0) // TTTR RecordType
          RecordType = TagHead.TagValue;*/ 
        break;
      /*case tyBitSet64: //not sure when this is used
        //fprintf(fpout, "0x%16.16X", TagHead.TagValue);
        //fprintf(fpout, "  tyBitSet64");
        break;*/
      /*case tyColor8:
        fprintf(fpout, "0x%16.16X", TagHead.TagValue);
        fprintf(fpout, "  tyColor8");
        break;*/
      case tyFloat8:
        if (strcmp(TagHead.Ident, name_HW_BaseResolution)==0){ //this one probs not needed
          ptu_header->HW_BaseResolution = TagHead.TagValue;   
        } else if (strcmp(TagHead.Ident, name_MeasDesc_Resolution)==0){
          ptu_header->MeasDesc_Resolution = TagHead.TagValue;
        } else if (strcmp(TagHead.Ident, name_MeasDesc_GlobalResolution)==0){
          ptu_header->MeasDesc_GlobalResolution = TagHead.TagValue;//in ns
        }/*
        fprintf(fpout, "%E", *(double*)&(TagHead.TagValue));
        fprintf(fpout, "  tyFloat8");
        if (strcmp(TagHead.Ident, TTTRTagRes)==0) // Resolution for TCSPC-Decay
          Resolution = *(double*)&(TagHead.TagValue);
        if (strcmp(TagHead.Ident, TTTRTagGlobRes)==0) // Global resolution for timetag
          GlobRes = *(double*)&(TagHead.TagValue); // in ns*/
        break;
      /*case tyFloat8Array: //is this used when there are multiple records in one file?
        fprintf(fpout, "<Float Array with %d Entries>", TagHead.TagValue / sizeof(double));
        fprintf(fpout, "  tyFloat8Array");
        // only seek the Data, if one needs the data, it can be loaded here
        fseek(in_stream, (long)TagHead.TagValue, SEEK_CUR);
        break;*/
      case tyTDateTime:
	  	;//need empty statement before declaration after a :
	  	time_t ftime;
		ftime = TDateTime_TimeT(*((double*)&(TagHead.TagValue)));
        strftime(ptu_header->FileTime, 20, "%Y-%m-%d %H:%M:%S", localtime(&ftime));
        //fprintf(fpout, "%s", asctime(gmtime(&CreateTime)), "\0");
        //fprintf(fpout, "  tyTDateTime");
        break;
      case tyAnsiString:
        AnsiBuffer = (char*)calloc((size_t)TagHead.TagValue,1);
        Result = fread(AnsiBuffer, 1, (size_t)TagHead.TagValue, in_stream);
        if (Result!= TagHead.TagValue){
          printf("\nIncomplete File at AnsiBuffer.");
        } else if (strcmp(TagHead.Ident, name_File_GUID)==0){
		  sprintf(ptu_header->File_GUID, "%lld", TagHead.TagValue);
        } else if (strcmp(TagHead.Ident, name_File_AssuredContent)==0){
          sprintf(ptu_header->File_AssuredContent,"%lld",TagHead.TagValue);
        } else if (strcmp(TagHead.Ident, name_CreatorSW_ContentVersion)==0){
          sprintf(ptu_header->CreatorSW_ContentVersion ,"%lld",TagHead.TagValue);
        } else if (strcmp(TagHead.Ident, name_CreatorSW_Name)==0){
          sprintf(ptu_header->CreatorName ,"%lld",TagHead.TagValue);//creatorname,version is consistent with other files
        } else if (strcmp(TagHead.Ident, name_CreatorSW_Version)==0){
          sprintf(ptu_header->CreatorVersion ,"%lld", TagHead.TagValue);
        } else if (strcmp(TagHead.Ident, name_File_Comment)==0){//T2 Mode!!!!!!!!!
          sprintf(ptu_header->Comment ,"%lld", TagHead.TagValue);
        } else if (strcmp(TagHead.Ident, name_HW_Type)==0){//HydraHarp400!!!!!!!!
          sprintf(ptu_header->HW_Type ,"%lld", TagHead.TagValue);
        } else if (strcmp(TagHead.Ident, name_HW_PartNo)==0){
          sprintf(ptu_header->HW_PartNo ,"%lld", TagHead.TagValue);
        } else if (strcmp(TagHead.Ident, name_HW_Version)==0){//Version 2.0!!!!!!!
          sprintf(ptu_header->HW_Version ,"%lld", TagHead.TagValue);
        } else if (strcmp(TagHead.Ident, name_HW_SerialNo)==0){
          sprintf(ptu_header->HW_SerialNo ,"%lld", TagHead.TagValue);
        }
        //fprintf(fpout, "%s", AnsiBuffer);
        //fprintf(fpout, "  tyAnsiString");
        free(AnsiBuffer);
        break;
      /*case tyWideString:
        WideBuffer = (WCHAR*)calloc((size_t)TagHead.TagValue,1);
        Result = fread(WideBuffer, 1, (size_t)TagHead.TagValue, in_stream);
        if (Result!= TagHead.TagValue){
          printf("\nIncomplete File at WideBuffer");
        }
        //fwprintf(fpout, L"%s", WideBuffer);
        fprintf(fpout, "  tyWideString");
        free(WideBuffer);
        break;*/
      /*case tyBinaryBlob:
        fprintf(fpout, "<Binary Blob contains %d Bytes>", TagHead.TagValue);
        fprintf(fpout, "  tyBinaryBlob");
        // only seek the Data, if one needs the data, it can be loaded here
        fseek(in_stream, (long)TagHead.TagValue, SEEK_CUR);
        break;*/
      default:
        	//error  ("Illegal Type identifier found! Broken file?");
			return(PQ_ERROR_IO);
        break;
    }
  }
  while((strncmp(TagHead.Ident, name_Header_End, sizeof(name_Header_End))));
  return (PQ_SUCCESS);
// End Header loading
}

time_t TDateTime_TimeT(double Convertee){
    const int EpochDiff = 25569; // days between 30/12/1899 and 01/01/1970
    const int SecsInDay = 86400; // number of seconds in a day
    time_t Result; 
    Result = ((long)(((Convertee) - EpochDiff) * SecsInDay));
    return Result;
}

void pq_header_printf(FILE *stream_out, pq_header_t *pq_header) {
	fprintf(stream_out, "Ident = %.*s\n", 
			(int)sizeof(pq_header->Ident), pq_header->Ident);
	fprintf(stream_out, "FormatVersion xxx = %.*s\n", 
			(int)sizeof(pq_header->Ident), pq_header->FormatVersion);
}


void pq_header_fwrite(FILE *stream_out, pq_header_t *pq_header) {
	fwrite(pq_header, sizeof(pq_header_t), 1, stream_out);
}

void ptu_header_fwrite(FILE *stream_out, ptu_header_t *ptu_header){
	fwrite(ptu_header, sizeof(ptu_header_t), 1, stream_out);
}

void ptu_header_printf(FILE *stream_out, ptu_header_t *ptu_header) {
	fprintf(stream_out, "CreatorName = %.*s\n", 
			(int)sizeof(ptu_header->CreatorName), ptu_header->CreatorName);
	fprintf(stream_out, "CreatorVersion = %.*s\n", 
			(int)sizeof(ptu_header->CreatorVersion), ptu_header->CreatorVersion);  
	fprintf(stream_out, "FileTime = %.*s\n", 
			(int)sizeof(ptu_header->FileTime), ptu_header->FileTime);

  fprintf(stream_out, "Comment = %.*s\n", 
			(int)sizeof(ptu_header->Comment), ptu_header->Comment);
  fprintf(stream_out, "NumberOfCurves = %.*d\n", 
			(int)sizeof(ptu_header->NumberOfCurves), ptu_header->NumberOfCurves);

  fprintf(stream_out, "MeasDesc_StopOnOvfl = %.*u\n", 
			(int)sizeof(ptu_header->MeasDesc_StopOnOvfl), ptu_header->MeasDesc_StopOnOvfl);
	fprintf(stream_out, "MeasDesc_Restart = %.*u\n", 
			(int)sizeof(ptu_header->MeasDesc_Restart), ptu_header->MeasDesc_Restart);  
	fprintf(stream_out, "CurSWSetting_DispLog = %.*u\n", 
			(int)sizeof(ptu_header->CurSWSetting_DispLog), ptu_header->CurSWSetting_DispLog);
  fprintf(stream_out, "CurSWSetting_DispCurve_Show0 = %.*u\n", 
			(int)sizeof(ptu_header->CurSWSetting_DispCurve_Show0), ptu_header->CurSWSetting_DispCurve_Show0);
  fprintf(stream_out, "CurSWSetting_DispCurve_Show1 = %.*u\n", 
			(int)sizeof(ptu_header->CurSWSetting_DispCurve_Show1), ptu_header->CurSWSetting_DispCurve_Show1);
  fprintf(stream_out, "CurSWSetting_DispCurve_Show2 = %.*u\n", 
			(int)sizeof(ptu_header->CurSWSetting_DispCurve_Show2), ptu_header->CurSWSetting_DispCurve_Show2);
  fprintf(stream_out, "CurSWSetting_DispCurve_Show3 = %.*u\n", 
			(int)sizeof(ptu_header->CurSWSetting_DispCurve_Show3), ptu_header->CurSWSetting_DispCurve_Show3);
  fprintf(stream_out, "CurSWSetting_DispCurve_Show4 = %.*u\n", 
			(int)sizeof(ptu_header->CurSWSetting_DispCurve_Show4), ptu_header->CurSWSetting_DispCurve_Show4);
  fprintf(stream_out, "CurSWSetting_DispCurve_Show5 = %.*u\n", 
			(int)sizeof(ptu_header->CurSWSetting_DispCurve_Show5), ptu_header->CurSWSetting_DispCurve_Show5);
  fprintf(stream_out, "CurSWSetting_DispCurve_Show6 = %.*u\n", 
			(int)sizeof(ptu_header->CurSWSetting_DispCurve_Show6), ptu_header->CurSWSetting_DispCurve_Show6);
  fprintf(stream_out, "CurSWSetting_DispCurve_Show7 = %.*u\n", 
			(int)sizeof(ptu_header->CurSWSetting_DispCurve_Show7), ptu_header->CurSWSetting_DispCurve_Show7);
  fprintf(stream_out, "HW_ExternalRefClock = %.*u\n", 
			(int)sizeof(ptu_header->HW_ExternalRefClock), ptu_header->HW_ExternalRefClock);
  fprintf(stream_out, "HWInpChan_Enabled0 = %.*u\n", 
			(int)sizeof(ptu_header->HWInpChan_Enabled0), ptu_header->HWInpChan_Enabled0);
  fprintf(stream_out, "HWInpChan_Enabled1 = %.*u\n", 
			(int)sizeof(ptu_header->HWInpChan_Enabled1), ptu_header->HWInpChan_Enabled1);
  fprintf(stream_out, "HWMarkers_RisingEdge0 = %.*u\n", 
			(int)sizeof(ptu_header->HWMarkers_RisingEdge0), ptu_header->HWMarkers_RisingEdge0);
  fprintf(stream_out, "HWMarkers_RisingEdge1 = %.*u\n", 
			(int)sizeof(ptu_header->HWMarkers_RisingEdge1), ptu_header->HWMarkers_RisingEdge1);
  fprintf(stream_out, "HWMarkers_RisingEdge2 = %.*u\n", 
			(int)sizeof(ptu_header->HWMarkers_RisingEdge2), ptu_header->HWMarkers_RisingEdge2);
  fprintf(stream_out, "HWMarkers_RisingEdge3 = %.*u\n", 
			(int)sizeof(ptu_header->HWMarkers_RisingEdge3), ptu_header->HWMarkers_RisingEdge3);
  fprintf(stream_out, "HWMarkers_Enabled0 = %.*u\n", 
			(int)sizeof(ptu_header->HWMarkers_Enabled0), ptu_header->HWMarkers_Enabled0);
  fprintf(stream_out, "HWMarkers_Enabled1 = %.*u\n", 
			(int)sizeof(ptu_header->HWMarkers_Enabled1), ptu_header->HWMarkers_Enabled1);
  fprintf(stream_out, "HWMarkers_Enabled2 = %.*u\n", 
			(int)sizeof(ptu_header->HWMarkers_Enabled2), ptu_header->HWMarkers_Enabled2);
  fprintf(stream_out, "HWMarkers_Enabled3 = %.*u\n", 
			(int)sizeof(ptu_header->HWMarkers_Enabled3), ptu_header->HWMarkers_Enabled3);

  fprintf(stream_out, "Fast_Load_End = %.*d\n", 
			(int)sizeof(ptu_header->Fast_Load_End), ptu_header->Fast_Load_End);
  fprintf(stream_out, "Measurement_Mode = %.*d\n", 
			(int)sizeof(ptu_header->Measurement_Mode), ptu_header->Measurement_Mode);
  fprintf(stream_out, "Measurement_SubMode = %.*d\n", 
			(int)sizeof(ptu_header->Measurement_SubMode), ptu_header->Measurement_SubMode);
  fprintf(stream_out, "TTResult_StopReason = %.*d\n", 
			(int)sizeof(ptu_header->TTResult_StopReason), ptu_header->TTResult_StopReason);
  fprintf(stream_out, "TTResultFormat_TTTRRecType = %.*d\n", 
			(int)sizeof(ptu_header->TTResultFormat_TTTRRecType), ptu_header->TTResultFormat_TTTRRecType);
  fprintf(stream_out, "TTResultFormat_BitsPerRecord = %.*d\n", 
			(int)sizeof(ptu_header->TTResultFormat_BitsPerRecord), ptu_header->TTResultFormat_BitsPerRecord);
  fprintf(stream_out, "MeasDesc_BinningFactor = %.*d\n", 
			(int)sizeof(ptu_header->MeasDesc_BinningFactor), ptu_header->MeasDesc_BinningFactor);
  fprintf(stream_out, "MeasDesc_Offset = %.*d\n", 
			(int)sizeof(ptu_header->MeasDesc_Offset), ptu_header->MeasDesc_Offset);
  fprintf(stream_out, "MeasDesc_AcquisitionTime = %.*d\n", 
			(int)sizeof(ptu_header->MeasDesc_AcquisitionTime), ptu_header->MeasDesc_AcquisitionTime);
  fprintf(stream_out, "MeasDesc_StopAt = %.*d\n", 
			(int)sizeof(ptu_header->MeasDesc_StopAt), ptu_header->MeasDesc_StopAt);
  fprintf(stream_out, "CurSWSetting_DispAxisTimeFrom = %.*d\n", 
			(int)sizeof(ptu_header->CurSWSetting_DispAxisTimeFrom), ptu_header->CurSWSetting_DispAxisTimeFrom);
  fprintf(stream_out, "CurSWSetting_DispAxisTimeTo = %.*d\n", 
			(int)sizeof(ptu_header->CurSWSetting_DispAxisTimeTo), ptu_header->CurSWSetting_DispAxisTimeTo);
  fprintf(stream_out, "CurSWSetting_DispAxisCountFrom = %.*d\n", 
			(int)sizeof(ptu_header->CurSWSetting_DispAxisCountFrom), ptu_header->CurSWSetting_DispAxisCountFrom);
  fprintf(stream_out, "CurSWSetting_DispAxisCountTo = %.*d\n", 
			(int)sizeof(ptu_header->CurSWSetting_DispAxisCountTo), ptu_header->CurSWSetting_DispAxisCountTo);
  fprintf(stream_out, "CurSWSetting_DispCurves = %.*d\n", 
			(int)sizeof(ptu_header->CurSWSetting_DispCurves), ptu_header->CurSWSetting_DispCurves);
  fprintf(stream_out, "CurSWSetting_DispCurve_MapTo0 = %.*d\n", 
			(int)sizeof(ptu_header->CurSWSetting_DispCurve_MapTo0), ptu_header->CurSWSetting_DispCurve_MapTo0);
  fprintf(stream_out, "CurSWSetting_DispCurve_MapTo1 = %.*d\n", 
			(int)sizeof(ptu_header->CurSWSetting_DispCurve_MapTo1), ptu_header->CurSWSetting_DispCurve_MapTo1);
  fprintf(stream_out, "CurSWSetting_DispCurve_MapTo2 = %.*d\n", 
			(int)sizeof(ptu_header->CurSWSetting_DispCurve_MapTo2), ptu_header->CurSWSetting_DispCurve_MapTo2);
  fprintf(stream_out, "CurSWSetting_DispCurve_MapTo3 = %.*d\n", 
			(int)sizeof(ptu_header->CurSWSetting_DispCurve_MapTo3), ptu_header->CurSWSetting_DispCurve_MapTo3);
  fprintf(stream_out, "CurSWSetting_DispCurve_MapTo4 = %.*d\n", 
			(int)sizeof(ptu_header->CurSWSetting_DispCurve_MapTo4), ptu_header->CurSWSetting_DispCurve_MapTo4);
  fprintf(stream_out, "CurSWSetting_DispCurve_MapTo5 = %.*d\n", 
			(int)sizeof(ptu_header->CurSWSetting_DispCurve_MapTo5), ptu_header->CurSWSetting_DispCurve_MapTo5);
  fprintf(stream_out, "CurSWSetting_DispCurve_MapTo6 = %.*d\n", 
			(int)sizeof(ptu_header->CurSWSetting_DispCurve_MapTo6), ptu_header->CurSWSetting_DispCurve_MapTo6);
  fprintf(stream_out, "CurSWSetting_DispCurve_MapTo7 = %.*d\n", 
			(int)sizeof(ptu_header->CurSWSetting_DispCurve_MapTo7), ptu_header->CurSWSetting_DispCurve_MapTo7);
  fprintf(stream_out, "HW_Modules = %.*d\n", 
			(int)sizeof(ptu_header->HW_Modules), ptu_header->HW_Modules);
  fprintf(stream_out, "HWModule_TypeCode0 = %.*d\n", 
			(int)sizeof(ptu_header->HWModule_TypeCode0), ptu_header->HWModule_TypeCode0);
  fprintf(stream_out, "HWModule_TypeCode1 = %.*d\n", 
			(int)sizeof(ptu_header->HWModule_TypeCode1), ptu_header->HWModule_TypeCode1);
  fprintf(stream_out, "HWModule_TypeCode2 = %.*d\n", 
			(int)sizeof(ptu_header->HWModule_TypeCode2), ptu_header->HWModule_TypeCode2);
  fprintf(stream_out, "HWModule_VersCode0 = %.*d\n", 
			(int)sizeof(ptu_header->HWModule_VersCode0), ptu_header->HWModule_VersCode0);
  fprintf(stream_out, "HWModule_VersCode1 = %.*d\n", 
			(int)sizeof(ptu_header->HWModule_VersCode1), ptu_header->HWModule_VersCode1);
  fprintf(stream_out, "HWModule_VersCode2 = %.*d\n", 
			(int)sizeof(ptu_header->HWModule_VersCode2), ptu_header->HWModule_VersCode2);
  fprintf(stream_out, "HW_InpChannels = %.*d\n", 
			(int)sizeof(ptu_header->HW_InpChannels), ptu_header->HW_InpChannels);
  fprintf(stream_out, "HW_ExternalDevices = %.*d\n", 
			(int)sizeof(ptu_header->HW_ExternalDevices), ptu_header->HW_ExternalDevices);
  fprintf(stream_out, "HWSync_Divider = %.*d\n", 
			(int)sizeof(ptu_header->HWSync_Divider), ptu_header->HWSync_Divider);
  fprintf(stream_out, "HWSync_CFDLevel = %.*d\n", 
			(int)sizeof(ptu_header->HWSync_CFDLevel), ptu_header->HWSync_CFDLevel);
  fprintf(stream_out, "HWSync_CFDZeroCross = %.*d\n", 
			(int)sizeof(ptu_header->HWSync_CFDZeroCross), ptu_header->HWSync_CFDZeroCross);
  fprintf(stream_out, "HWSync_Offset = %.*d\n", 
			(int)sizeof(ptu_header->HWSync_Offset), ptu_header->HWSync_Offset);
  fprintf(stream_out, "HWInpChan_ModuleIdx0 = %.*d\n", 
			(int)sizeof(ptu_header->HWInpChan_ModuleIdx0), ptu_header->HWInpChan_ModuleIdx0);
  fprintf(stream_out, "HWInpChan_ModuleIdx1 = %.*d\n", 
			(int)sizeof(ptu_header->HWInpChan_ModuleIdx1), ptu_header->HWInpChan_ModuleIdx1);
  fprintf(stream_out, "HWInpChan_CFDLevel0 = %.*d\n", 
			(int)sizeof(ptu_header->HWInpChan_CFDLevel0), ptu_header->HWInpChan_CFDLevel0);
  fprintf(stream_out, "HWInpChan_CFDLevel1 = %.*d\n", 
			(int)sizeof(ptu_header->HWInpChan_CFDLevel1), ptu_header->HWInpChan_CFDLevel1);
  fprintf(stream_out, "HWInpChan_CFDZeroCross0 = %.*d\n", 
			(int)sizeof(ptu_header->HWInpChan_CFDZeroCross0), ptu_header->HWInpChan_CFDZeroCross0);
  fprintf(stream_out, "HWInpChan_CFDZeroCross1 = %.*d\n", 
			(int)sizeof(ptu_header->HWInpChan_CFDZeroCross1), ptu_header->HWInpChan_CFDZeroCross1);
  fprintf(stream_out, "HWInpChan_Offset0 = %.*d\n", 
			(int)sizeof(ptu_header->HWInpChan_Offset0), ptu_header->HWInpChan_Offset0);
    fprintf(stream_out, "HWInpChan_Offset1 = %.*d\n", 
			(int)sizeof(ptu_header->HWInpChan_Offset1), ptu_header->HWInpChan_Offset1);
  fprintf(stream_out, "HW_Markers = %.*d\n", 
			(int)sizeof(ptu_header->HW_Markers), ptu_header->HW_Markers);
  fprintf(stream_out, "HWMarkers_HoldOff = %.*d\n", 
			(int)sizeof(ptu_header->HWMarkers_HoldOff), ptu_header->HWMarkers_HoldOff);
  fprintf(stream_out, "TTResult_SyncRate = %.*d\n", 
			(int)sizeof(ptu_header->TTResult_SyncRate), ptu_header->TTResult_SyncRate);
  fprintf(stream_out, "TTResult_InputRate0 = %.*d\n", 
			(int)sizeof(ptu_header->TTResult_InputRate0), ptu_header->TTResult_InputRate0);
  fprintf(stream_out, "TTResult_InputRate1 = %.*d\n", 
			(int)sizeof(ptu_header->TTResult_InputRate1), ptu_header->TTResult_InputRate1);
  fprintf(stream_out, "TTResult_StopAfter = %.*d\n", 
			(int)sizeof(ptu_header->TTResult_StopAfter), ptu_header->TTResult_StopAfter);
  fprintf(stream_out, "TTResult_NumberOfRecords = %.*d\n", 
			(int)sizeof(ptu_header->TTResult_NumberOfRecords), ptu_header->TTResult_NumberOfRecords);

  fprintf(stream_out, "HW_BaseResolution = %.*f\n", 
			(int)sizeof(ptu_header->HW_BaseResolution), ptu_header->HW_BaseResolution);
  fprintf(stream_out, "MeasDesc_Resolution = %.*f\n", 
			(int)sizeof(ptu_header->MeasDesc_Resolution), ptu_header->MeasDesc_Resolution);
  fprintf(stream_out, "MeasDesc_GlobalResolution = %.*f\n", 
			(int)sizeof(ptu_header->MeasDesc_GlobalResolution), ptu_header->MeasDesc_GlobalResolution);

  fprintf(stream_out, "File_GUID = %.*s\n", 
			(int)sizeof(ptu_header->File_GUID), ptu_header->File_GUID);
  fprintf(stream_out, "File_AssuredContent = %.*s\n", 
			(int)sizeof(ptu_header->File_AssuredContent), ptu_header->File_AssuredContent);
  fprintf(stream_out, "CreatorSW_ContentVersion = %.*s\n", 
			(int)sizeof(ptu_header->CreatorSW_ContentVersion), ptu_header->CreatorSW_ContentVersion);
  fprintf(stream_out, "HW_Type = %.*s\n", 
			(int)sizeof(ptu_header->HW_Type), ptu_header->HW_Type);
  fprintf(stream_out, "HW_PartNo = %.*s\n", 
			(int)sizeof(ptu_header->HW_PartNo), ptu_header->HW_PartNo);
  fprintf(stream_out, "HW_Version = %.*s\n", 
			(int)sizeof(ptu_header->HW_Version), ptu_header->HW_Version);
  fprintf(stream_out, "HW_SerialNo = %.*s\n", 
			(int)sizeof(ptu_header->HW_SerialNo), ptu_header->HW_SerialNo);
/*  fprintf(stream_out, " = %.*s\n", 
			(int)sizeof(ptu_header->), ptu_header->);
  fprintf(stream_out, " = %.*s\n", 
			(int)sizeof(ptu_header->), ptu_header->);
  fprintf(stream_out, " = %.*s\n", 
			(int)sizeof(ptu_header->), ptu_header->);
  fprintf(stream_out, " = %.*s\n", 
			(int)sizeof(ptu_header->), ptu_header->);
  fprintf(stream_out, " = %.*s\n", 
			(int)sizeof(ptu_header->), ptu_header->);
  fprintf(stream_out, " = %.*s\n", 
			(int)sizeof(ptu_header->), ptu_header->);
  fprintf(stream_out, " = %.*s\n", 
			(int)sizeof(ptu_header->), ptu_header->);
  fprintf(stream_out, " = %.*s\n", 
			(int)sizeof(ptu_header->), ptu_header->);
  fprintf(stream_out, " = %.*s\n", 
			(int)sizeof(ptu_header->), ptu_header->);
  fprintf(stream_out, " = %.*s\n", 
			(int)sizeof(ptu_header->), ptu_header->);*/


      
}