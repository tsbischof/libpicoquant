import subprocess
import re
import configparser
import math
import io
import csv
import os

from photon_correlation import *

from picoquant import fake_ini_section

fake_section = "section"

class HHD(G1):
    def __init__(self, filename):
        super(HHD, self).__init__()

        self._header = None
        self.filename = filename

        if os.path.exists(self.filename):
            stream_in = subprocess.Popen(
                ["picoquant",
                 "--file-in", self.filename],
                stdout=subprocess.PIPE)
            
            self.from_stream(csv.reader(map(lambda _: _.decode(),
                                            stream_in.stdout)))
        else:
            raise(ValueError("Must specify a valid filename: {} does not exist"
                             "".format(self.filename)))

    def __getitem__(self, curves):
        """
        Return the lifetime represented by the given curve,
        or the g1 represented by the given curves.
        """
        if type(curves) == int:
            return(self._counts[curves])
        elif type(curves) == slice:
            g1 = G1()

            for curve in list(range(len(self)))[curves]:
                g1[curve] = self[curve]

            return(g1)
        else:
            raise(KeyError("Cannot interpret index: {}".format(curves)))
        
    def channels(self):
        """
        Return the number of channels installed in the device.
        """
        return(int(self.get("NumberOfModules")))

    def curve_val(self, attr, curve, outtype=lambda x: x):
        """
        If no curve is specified, return a dict with the attr for each curve.
        Otherwise, just return a single value.
        """ 
        def get(val):
            return(outtype(self.get("Curve[{}].{}".format(curve, attr))))
    
        if curve is None:
            vals = dict()

            for curve in range(self.n_curves()):
                vals[curve] = get(curve)
        else:
            vals = get(curve)

        return(vals)
        
    def header(self):
        """
        Read the header from the file, for parsing purposes.
        """
        if not self._header:
            raw_header = subprocess.Popen(
                ["picoquant",
                 "--file-in", self.filename,
                 "--header-only"],
                stdout=subprocess.PIPE).communicate()[0].decode()

            self._header = configparser.ConfigParser()
            self._header.readfp(io.StringIO(fake_ini_section(raw_header,
                                                             fake_section)))
            
        return(self._header)

    def get(self, param):
        """
        Return the associated parameter from the header.
        """
        return(self.header().get(fake_section, param))

    def resolution(self, curve=None):
        """
        Determine the time resolution for each curve, in ps.
        """
        return(self.curve_val("Resolution", curve,
                              lambda x: math.floor(float(x))))

    def sync_rate(self, curve=None):
        """
        Return the repetition of the laser, in counts per second.
        """
        return(self.curve_val("SyncRate", curve, int))

    def integration_time(self, curve=None):
        """
        Return the integration time for the curve, in milliseconds.
        """
        return(self.curve_val("StopAfter", curve, int))
