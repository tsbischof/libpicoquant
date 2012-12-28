## To do: work out how to make an unsigned int array for data transfer

from picoquant import TCSPC
from picoquant.timeharp import timeharp_comm as thlib

class TimeHarp(TCSPC):
    """
    This object is designed to interact with a physical TimeHarp, with the
    intent of controlling experiments. It does not decode the data natively,
    but instead relies on other libraries to do so.
    """
    def __init__(self, device_index=0):
        super(TimeHarp, self).__init__()

        self._comm_lib = thlib

        self.serial = ""

        self._mode_dict = {"interactive": 0,
                           "t3": 1,
                           "t3r": 1,
                           "tttr": 1}

        self._histogram = None
        self._flags = None
##        self._fifo_buffer = thlib.uint_array(2**12)
##        self._continuous_buffer = thlib.uint_array(2**12)

        self._error_buff = bytearray(40)
        self._serial_buff = bytearray(8)
        self._warnings_buff = bytearray(16384)

# General library routines
    def error_string(self, errcode):
        """
        Return the error string associated with the error code.
        """
        result, errstring = thlib.TH_GetErrorString("", errcode)
        if result == 0:
            return(errstring)
        else:
            return("Error {0} could not be decoded.".format(errcode))

    def library_version(self):
        """
        Return the version number of the shared library.
        """
        result, version = thlib.TH_GetLibraryVersion("")

        if self.CHK(result):
            major, minor = version.split(".")
            return(int(major), int(minor))
    
# Device opening, closing
    def open(self):
        """
        Open the device, and record its serial number.
        """
        pass
##        result, serial =  thlib.TH_OpenDevice(self.device_index, "")
##        self.serial = serial
##        
##        return(self.CHK(result))

    def close(self):
        """
        Close the device and release it for further use.
        """
        return(self.CHK(thlib.TH_Shutdown(self.device_index)))

    def initialize(self, mode):
        """
        Prepare the device for a measurement in the specified mode,
        using the specified reference source.
        """
        result = thlib.TH_Initialize(self.mode_number(mode))

        return(self.CHK(result))
                
# Hardware information
    def hardware_version(self):
        """
        Determine the hardware version number.
        """
        result, version = thlib.TH_GetHardwareVersion("")
        
        if self.CHK(result):
            return(version)

    def serial_number(self):
        """
        Determine the serial number of the current device.
        """
        result, serial = thlib.TH_GetSerialNumber("")

        if self.CHK(result):
            return(serial)
    
    def base_resolution(self):
        """
        Detrmine the base resolution of the current device.
        """
        result = thlib.TH_GetBaseResolution()
        
        if self.CHK(result):
            return(result)

    def calibrate(self):
        """
        Calibrate the current device.
        """

        return(self.CHK(thlib.TH_Calibrate(self.device_index)))

    def set_CFD(self, level, zero_cross):
        """
        Set the CFD discriminator and zero cross level, in mV.
        
        Valid discriminator range: {0} to {1}
        Valid zero cross range: {2} to {3}
        """.format(thlib.DISCRMIN, thlib.DISCRMAX,
                   thlib.ZCMIN, thlib.ZCMAX)

        result = self.CHK(thlib.TH_SetCFDDiscrMin(level))
        if self.CHK(result):
            return(self.CHK(thlib.TH_SetCFDZeroCross(zero_cross)))

    def set_sync_level(self, level):
        """
        Set the sync discriminator level, in mV.

        Valid range: {0} to {1}
        """.format(thlib.SYNCMIN, thlib.SYNCMAX)

        return(self.CHK(thlib.TH_SetSyncLevel(level)))
    
    def set_stop_overflow(self, stop_on_overflow):
        """
        Set the device to stop on an overflow (True or False).
        """
        
        if stop_on_overflow:
            stop_on_overflow = 1
        else:
            stop_on_overflow = 0
        
        result = thlib.TH_SetStopOverflow(stop_on_overflow)
        return(self.CHK(result))

    def set_range(self, measurement_range):
        """
        Set the measurement range.

        Valid range: {0} to {1}
        """.format(0, thlib.RANGES-1)
        
        result =  thlib.TH_SetRange(measurement_range)
        return(self.CHK(result))

    def set_offset(self, offset):
        """
        Set the histogram time offset, in ps.

        Valid range: {0} to {1}
        """.format(thlib.OFFSETMIN, thlib.OFFSETMAX)
        
        result = thlib.TH_SetOffset(offset)
        if self.CHK(result):
            return(result)

    def set_next_offset(self, direction):
        """
        Set the direction of the next offset change:
        -1: down
        +1: up
        """

        result = thlib.TH_NextOffset(direction)

        if self.CHK(result):
            return(result)
            
    def clear_histogram(self):
        """
        Clear the current histogram in memory.
        """
        
        result = thlib.TH_ClearHistMem(0)
        return(self.CHK(result))

    def set_measurement_mode(self, mode, acquisition_time):
        """
        Set the measurement mode, and the acquisition time (in ms).

        Valid measurement modes:
        0: interactive/tttr
        1: continuous

        Valid acquisition times: {0} to {1}
        (set to 0 for continuous mode and external clock)
        """.format(thlib.ACQTMIN, thlib.ACQTMAX)

        result = thlib.TH_SetMMode(mode, acquisition_time)
        return(self.CHK(result))

    def start_measurement(self):
        """
        Start the current measurement.
        """
        
        result = thlib.TH_StartMeas(s)
        return(self.CHK(result))

    def stop_measurement(self):
        """
        Stop the currently-running measurement.
        """

        result = thlib.TH_StopMeas()
        return(self.CHK(result))

    def ctc_status(self):
        """
        Determine the current status of the experiment. Returns True is the
        experiment is complete, and False otherwise.
        """

        result = thlib.TH_CTCStatus()

        return(result == 0)

    def set_sync_mode(self):
        """
        Call this function to enable the device to report accurate count
        rates.
        """
        
        result = thlib.TH_SetSyncMode()

        return(self.CHK(result))

    def get_histogram(self, block=0):
        """
        Return the current histogram. Use the block number only if routing.
        """

        # First, make sure we have space allocated.
        if not self._histogram:
            raise(ValueError("No acquisition buffer found, did you call "
                             "set_histogram_length()?"))

        result = thlib.TH_GetHistogram(self._histogram,
                                       block)

        if self.CHK(result):
            return(self._histogram, result)

    def get_resolution(self):
        """
        Determine the resolution at the current binning settings, in ps.
        """

        result = thlib.TH_GetResolution()

        if self.CHK(result):
            return(result)

    def get_count_rate(self, channel):
        """
        Determine the current count rate on the specified channel.
        """

        result = thlib.TH_GetCountRate()

        if self.CHK(result):
            return(result)

    def get_flags(self):
        """
        Determine the current status flags.
        """

        result = thlib.TH_GetFlags()
    
        if self.CHK(result):
            # Extract the status value from the flag
            found = list()

            for name, value in self.flags():
                if flags.value() & result:
                    found.append(name)

            return(tuple(found))

    def get_elapsed_measurement_time(self):
        """
        Determine the elapsed time in the current measurement, in ms.
        """

        result = thlib.TH_GetElapsedMeasTime()

        if self.CHK(result):
            return(result)

# TTTR
# Continuous
