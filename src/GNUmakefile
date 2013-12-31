#
# Copyright (c) 2011-2014, Thomas Bischof
# All rights reserved.
# 
# Redistribution and use in source and binary forms, with or without 
# modification, are permitted provided that the following conditions are met:
# 
# 1. Redistributions of source code must retain the above copyright notice, 
#    this list of conditions and the following disclaimer.
# 
# 2. Redistributions in binary form must reproduce the above copyright notice, 
#    this list of conditions and the following disclaimer in the documentation 
#    and/or other materials provided with the distribution.
# 
# 3. Neither the name of the Massachusetts Institute of Technology nor the 
#    names of its contributors may be used to endorse or promote products 
#    derived from this software without specific prior written permission.
# 
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE 
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
# POSSIBILITY OF SUCH DAMAGE.
#

CC = clang
LD = clang

CCFLAGS := -Wall 
# On 32-bit systems wtih large files, the file offset is 64 bits. This causes
# errors if not adjusted, so fix it here as needed. This should not (in 
# principle) affect 64-bit systems adversely, though if we ever get to 
# 128-bit filesystems we will need to switch this to 128. 
CCFLAGS += -D _FILE_OFFSET_BITS=64 
CCFLAGS += -D VERSION="0.2"
CCFLAGS += -O3

CCFLAGS_SHARE := ${CCFLAGS} -fpic

LDFLAGS = -lm

MKDIR = mkdir -p
RM = rm -f
TEST = test
CP = cp -p

# Define the targets and their properties.
PREFIX = ~
INSTALL_DIR = ${PREFIX}/bin
LIB_DIR = ${PREFIX}/lib
INCLUDE_DIR = ${PREFIX}/include/photon_correlation

PICOQUANT_OBJ = picoquant_main.o picoquant.o \
		error.o types.o options.o files.o \
		header.o continuous.o interactive.o tttr.o t2.o t3.o \
		picoharp.o \
		picoharp/ph_v20.o picoharp/ph_v20_tttr.o picoharp/ph_v20_interactive.o\
		hydraharp.o \
		hydraharp/hh_v10.o hydraharp/hh_v10_tttr.o \
		hydraharp/hh_v10_interactive.o hydraharp/hh_v10_continuous.o \
		hydraharp/hh_v20.o hydraharp/hh_v20_tttr.o \
		hydraharp/hh_v20_interactive.o hydraharp/hh_v20_continuous.o \
		timeharp.o \
		timeharp/th_v20.o timeharp/th_v20_tttr.o \
		timeharp/th_v20_interactive.o timeharp/th_v20_continuous.o \
		timeharp/th_v30.o timeharp/th_v30_tttr.o \
		timeharp/th_v30_interactive.o timeharp/th_v30_continuous.o \
		timeharp/th_v50.o timeharp/th_v50_tttr.o \
		timeharp/th_v50_interactive.o timeharp/th_v50_continuous.o \
		timeharp/th_v60.o timeharp/th_v60_tttr.o \
		timeharp/th_v60_interactive.o timeharp/th_v60_continuous.o 

# Now the heavy lifting: build the objects and include files for inclusion
# in the library.
LIBPQ_OBJ = ${patsubst %.o, %.os, ${filter-out %main.o, ${PICOQUANT_OBJ}}}
INCLUDES = ${patsubst %.o, %.h, ${LIBPQ_OBJ}}
LIBPQ_FLAGS = ${LDFLAGS} -shared

TARGETS = picoquant 
LIB_TARGETS = libpicoquant.so

all: ${TARGETS} ${LIB_TARGETS}

install: ${TARGETS} ${LIB_TARGETS}
	${TEST} -d ${INSTALL_DIR} || ${MKDIR} ${INSTALL_DIR}
	${CP} ${TARGETS} ${INSTALL_DIR}
	${TEST} -d ${LIB_DIR} || ${MKDIR} ${LIB_DIR}
	${CP} ${LIB_TARGETS} ${LIB_DIR}
	${TEST} -d ${INCLUDE_DIR} || ${MKDIR} ${INCLUDE_DIR}
	${CP} --parents ${INCLUDES} ${INCLUDE_DIR}

uninstall: 
	${RM} ${addprefix ${INSTALL_DIR}, ${TARGETS}}

picoquant: ${PICOQUANT_OBJ}
	${LD} ${LDFLAGS} -o $@ $^ 

libpicoquant.so: ${LIBPQ_OBJ}
	${LD} ${LIBPQ_FLAGS} -o $@ $^

%.o: %.c
	${CC} ${CCFLAGS} -c -o $@ $< 

%.os: %.c
	${CC} ${CCFLAGS_SHARE} -c -o $@ $<

clean:
	${RM} ${TARGETS} ${LIB_TARGETS} ${PICOQUANT_OBJ} ${LIBPQ_OBJ}
