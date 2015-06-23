try:
    import configparser
    import io
except:
    import StringIO as io
    import ConfigParser as configparser

import subprocess
import csv

from picoquant import FakeIniSection

class Picoquant(object):
    """
    Base class for Picoquant data. This includes:
    1. Common header
    2. Hardware header
    3. Mode header.
    4. Data

    Additionally, various helper routines are used to decode information
    about the files, including resolution and mode.
    """
    def __init__(self, filename):
        self._filename = filename
        self._header = None
        self._resolution = None
        self._data = None
        self._mode = None

    def header(self):
        if not self._header:
            header_raw = subprocess.Popen(
                ["picoquant",
                 "--file-in", self._filename,
                 "--header-only"],
                stdout=subprocess.PIPE).communicate()[0].decode()

            self._header = configparser.ConfigParser()
            self._header.readfp(FakeIniSection(io.StringIO(header_raw)))

        return(self._header)

    def resolution(self):
        if not self._resolution:
            resolution_raw = subprocess.Popen(
                ["picoquant",
                 "--file-in", self._filename,
                 "--resolution-only"],
                stdout=subprocess.PIPE).communicate()[0]

            if "," in resolution_raw:
                # curves
                self._resolution = list()
                for curve, resolution in csv.reader(\
                    io.StringIO(resolution_raw)):
                    self._resolution.append((int(curve),
                                             float(resolution)))
                self._resolution = tuple(self._resolution)
            else:
                # single result
                self._resolution = float(resolution_raw)

        return(self._resolution)

    def mode(self):
        if not self._mode:
            self._mode = subprocess.Popen(
                ["picoquant",
                 "--file-in", self._filename,
                 "--mode-only"],
                stdout=subprocess.PIPE).communicate()[0].strip()

        return(self._mode)


    def __iter__(self):
        data = subprocess.Popen(
            ["picoquant", 
             "--file-in", self._filename],
            stdout=subprocess.PIPE)
        return(csv.reader(data.stdout))
