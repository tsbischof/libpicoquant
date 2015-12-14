class FakeIniSection(object):
    """
    Wrapper to enable configparser to parse a file without sections.
    """
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

def fake_ini_section(text, section="header"):
    """
    Add the fake section "{}" to an ini-like file.
    """.format(section)
    return("[{}]".format(section) + text)

from .PHD import PHD
from .HHD import HHD
