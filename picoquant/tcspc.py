import re

class TCSPC(object):
    """Outline for a TCSPC box, like a HydraHarp or PicoHarp. This provides """
    """features like mode detection, hardware information inquiries, data """
    """allocation and collection, among other utilities."""
    def __init__(self, device_index=0):
        self.device_index = device_index

        self._comm_lib = None
        self._modes = tuple()
        self._mode_dict = dict()
        self._flags_dict = dict()
        self._warnings_dict = dict()

    def CHK(self, result):
        if result:
            raise(
                Exception("Error code {0}: {1}".format(
                    result,
                    self.error_string(result))))
        else:
            return(True)

    def modes(self):
        """
        Discover the valid modes for the device, and store them for future use.
        """
        if not self._mode_dict:
            mode_parser = re.compile("MODE_(?P<name>.+)")

            for name in filter(lambda x: mode_parser.search(x),
                               dir(self._comm_lib)):
                self._mode_dict[ \
                    mode_parser.search(name).group("name").lower()] = \
                    getattr(self._comm_lib, name)

        return(self._mode_dict)

    def valid_mode(self, mode):
        """
        Determines whether the given mode is valid. Handles both str and int
        forms of the mode specification.
        """

        try:
            self.mode_number(mode)
            return(True)
        except ValueError:
            return(False)
            
    def mode_number(self, mode):
        """
        Given a string description of the mode, determine its numerical value.
        """
        if isinstance(mode, str):
            try:
                return(self.modes()[mode.lower()])
            except KeyError:
                raise(ValueError("Unsupported mode: {0}.".format(mode)))
        elif isinstance(mode, int):
            return(mode)
        else:
            raise(ValueError("Invalid mode type {0} for {1} "
                             "(expected int or str)".format(
                                 type(mode), mode)))

    def flags(self):
        """
        Determine the status flags for the current device.
        """

        if not self._flags_dict:
            flag_parser = re.compile("FLAG_(?P<name>.+)")
            for flag in filter(lambda x: flag_parser.search(x),
                               dir(self._comm_lib)):
                self._flags_dict[\
                    flag_parser.search(flag).group("name").lower()] = \
                    getattr(self._comm_lib, flag)
                
        return(self._flags_dict)

    def warnings(self):
        """
        Determine the warning flags for the current device.
        """

        if not self._warnings_dict:
            warning_parser = re.compile("WARNING_(?P<name>.+)")
            for warning in filter(lambda x: flag_parser.search(x),
                               dir(self._comm_lib)):
                self._warning_dict[\
                    warning_parser.search(warning).group("name").lower()] = \
                    getattr(self._comm_lib, warning)
                
        return(self._warnings_dict)

# General library routines

    def error_string(self, errcode):
        return("")

    def library_version(self):
        pass
    
# Device opening, closing
    def open(self):
        pass

    def close(self):
        pass

    def initialize(self):
        pass
