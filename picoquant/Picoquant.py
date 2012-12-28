import configparser
import subprocess

class FakeIniSection(object):
    def __init__(self, fp):
        self.fp = fp
        self.sechead = '[asection]\n'
    def readline(self):
        if self.sechead:
            try:
                return(self.sechead)
            finally:
                self.sechead = None
        else:
            return(self.fp.readline())

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

    def header(self):
        if not self._header:
            header_raw = subprocess.Popen(
                ["picoquant",
                 "--file-in", self._filename,
                 "--header-only"],
                stdout=subprocess.PIPE).communicate()[0]

            self._header = configparser.ConfigParser(FakeIniSection(header_raw))

        return(self._header)

if __name__ == "__main__":
    p = Picoquant("../sample_data/hydraharp/v20.ht2")
    print(p.header())
