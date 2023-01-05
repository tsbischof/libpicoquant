# libpicoquant
`libpicoquant` provides command-line interfaces to various formats used by PicoQuant TCSPC hardware. 
It is intended to provide a generic interface to these formats which is compatible with other analysis code (e.g. [photon_correlation](github.com/tsbischof/photon_correlation.git)). 

## Building
This program is tested primarily on Linux, both 32-bit and 64-bit.
It has no external dependencies apart from a C compiler (tested with clang and gcc).
To build:
```
./bootstrap
./configure
make
```

## Usage
See `man picoquant` or `picoquant --help` for a full listing of options.

The primary use case is to output records to `stdout` or to a file, in text/csv format.

The most typical way to interact with the library is the `picoquant` command:

    picoquant --file-in "data.phd"

This outputs the data contained in `data.phd`, with one record per line.

To view all of the metadata in INI-style format:

    picoquant --file-in "data.phd" --header-only

Or just some:

    picoquant --file-in "data.phd" --resolution-only

For any supported file format, simply change "data.phd" to your own file. `picoquant` is intelligent enough to find the correct decoder, or to tell you the format is not supported.

## Hardware and measurement modes supported
### HydraHarp
* v1: hhd, ht2, ht3
* v2: hhd, ht2, ht3

### PicoHarp
* v2: phd, pt2, pt3

### TimeHarp
* v3: thd, t3r
* v5: thd
* v6: thd, t3r

### Unified formats
* ptu
  * v3: hh v2 (t2, t3)

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
