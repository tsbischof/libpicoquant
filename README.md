# libpicoquant
`libpicoquant` provides command-line interfaces to various formats used by PicoQuant TCSPC hardware. 
It is intended to provide a generic interface to these formats which is compatible with other analysis code (e.g. [photon_correlation](github.com/tsbischof/photon_correlation.git)). 

## Building
This program is tested primarily on Linux (64-bit).
It has no external dependencies apart from a C compiler for build (tested mostly with gcc) and python3 for test.
To build:
```
./bootstrap
./configure
make check
make install
```

## Usage
See `man picoquant` or `picoquant --help` for a full listing of options.

The primary use case is to output records to `stdout` or to a file, in text/csv format.

The most typical way to interact with the library is the `picoquant` command:
```
$ picoquant --file-in "data.phd"
0,0,128,0
0,128,256,0
0,256,384,0
0,384,512,0
0,512,640,0
0,640,768,0
0,768,896,0
0,896,1024,0
0,1024,1152,0
0,1152,1280,0
... (many lines omitted)
```

This outputs the data contained in `data.phd`, with one record per line.
In this case we have histogram records for channel 0, with bins of `[0ps, 128ps) 0 counts`, `[128ps, 256ps) 0 counts`, etc.

To view all of the metadata in INI-style format:
```
$ picoquant --file-in "data.phd" --header-only
Ident = PicoHarp 300
FormatVersion = 2.0
CreatorName = PicoHarp Software
CreatorVersion = 2.3.0.0
FileTime = 14/05/11 17:55:48
Comment = Untitled
NumberOfCurves = 7
BitsPerRecord = 32
RoutingChannels = 4
... (many lines omitted)
```

Or just some:
```
$ picoquant --file-in "data.phd" --resolution-only
0,128.00
1,128.00
2,128.00
3,128.00
4,128.00
5,512.00
6,128.00
```
Here we have the resolution per curve, in picoseconds. 

For any supported file format, simply change "data.phd" to your own file. 
`picoquant` is intelligent enough to find the correct decoder, or to tell you the format is not supported.

## Hardware and measurement modes supported
### HydraHarp
* v1: hhd, ht2, ht3
* v2: hhd, ht2, ht3
* v3: ptu (t2, t3)

### PicoHarp
* v2: phd, pt2, pt3
* v3: ptu (t2, t3)

### TimeHarp
* v3: thd, t3r
* v5: thd
* v6: thd, t3r

## Overview of how the program works
The `picoquant` command uses the binary header for the data file to determine the hardware identity, version, measurement mode, and any other relevant metadata. 
Using these metadata, it outputs the photon arrival or other data in a common format. Measurement modes currently supported:
* Histogramming: also known as lifetime data, this is a histogram of counts per time bin, accumulated by integrating for some amount of time during a measurement
* t2: the absolute arrival time of each detection event during a measurment. Often used for CW excitation
* t3: the absolute arrival pulse and relative arrival time for each detection event durin a measurement. Used with pulsed excitation.

These measurements vary in their resolution and implementation across hardware, but in `picoquant` the data types are normalized. On the command line the output is comma-delimited records of the following formats:
* histograming: detection channel, time bin (left edge), time bin (right edge), counts
* t2: detection channel, arrival time
* t3: detection channel, arrival pulse, arrival time

## Hacking and extending
The main entry point for the program is to start reading the file, then branching based on the magic header values.
After identifying the file type, hardware type, version, and measurement mode, the library finishes reading the header and prepares the decoding methods.
You can call these decoding routines (e.g. `ph_v20_t2_decode`) directly in your own code. 

To add new hardware or a new version, find the appropriate subroutine for that format and hardware to link your new code (e.g. in `ph_dispatch`).
