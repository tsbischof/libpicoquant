/* Functions exported by the PicoHarp programming library PHLib*/

/* Ver. 2.3      April 2009 */

#ifndef _WIN32
#define _stdcall
#endif

extern int _stdcall PH_GetLibraryVersion(char* vers);
extern int _stdcall PH_GetErrorString(char* errstring, int errcode);

extern int _stdcall PH_OpenDevice(int devidx, char* serial); //new since v2.0
extern int _stdcall PH_CloseDevice(int devidx);  //new since v2.0
extern int _stdcall PH_Initialize(int devidx, int mode);

//all functions below can only be used after PH_Initialize

extern int _stdcall PH_GetHardwareVersion(int devidx, char* model, char* vers);
extern int _stdcall PH_GetSerialNumber(int devidx, char* serial);
extern int _stdcall PH_GetBaseResolution(int devidx);

extern int _stdcall PH_Calibrate(int devidx);
extern int _stdcall PH_SetSyncDiv(int devidx, int div);
extern int _stdcall PH_SetCFDLevel(int devidx, int channel, int value);
extern int _stdcall PH_SetCFDZeroCross(int devidx, int channel, int value);

extern int _stdcall PH_SetStopOverflow(int devidx, int stop_ovfl, int stopcount);	
extern int _stdcall PH_SetRange(int devidx, int range);
extern int _stdcall PH_SetOffset(int devidx, int offset);

extern int _stdcall PH_ClearHistMem(int devidx, int block);
extern int _stdcall PH_StartMeas(int devidx, int tacq);
extern int _stdcall PH_StopMeas(int devidx);
extern int _stdcall PH_CTCStatus(int devidx);

extern int _stdcall PH_GetBlock(int devidx, unsigned int *chcount, int block);
extern int _stdcall PH_GetResolution(int devidx);
extern int _stdcall PH_GetCountRate(int devidx, int channel);
extern int _stdcall PH_GetFlags(int devidx);
extern int _stdcall PH_GetElapsedMeasTime(int devidx);

extern int _stdcall PH_GetWarnings(int devidx); //new since v.2.3
extern int _stdcall PH_GetWarningsText(int devidx, char* text, int warnings); //new since v.2.3

//for TT modes
extern int _stdcall PH_TTSetMarkerEdges(int devidx, int me0, int me1, int me2, int me3);
extern int _stdcall PH_TTReadData(int devidx, unsigned int* buffer, unsigned int count);

//for Routing
extern int _stdcall PH_GetRouterVersion(int devidx, char* model, char* vers);  
extern int _stdcall PH_GetRoutingChannels(int devidx);
extern int _stdcall PH_EnableRouting(int devidx, int enable);
extern int _stdcall PH_SetPHR800Input(int devidx, int channel, int level, int edge);  
extern int _stdcall PH_SetPHR800CFD(int devidx, int channel, int dscrlevel, int zerocross); 
 
