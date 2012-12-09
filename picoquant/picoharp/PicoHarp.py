## To do: work out how to make an unsigned int array for data transfer

from picoquant import TCSPC
from picoquant.picoharp import picoharp_comm as phlib

class PicoHarp(TCSPC):
    """
    This object is designed to interact with a physical PicoHarp, with the
    intent of controlling experiments. It does not decode the data natively,
    but instead relies on other libraries to do so.
    """
    def __init__(self, device_index=0):
        super(PicoHarp, self).__init__()

        self._comm_lib = phlib

        self.device_index = device_index
        self.serial = ""

        self.reference_sources = [0, # internal
                                  1] # external

        self._histogram = None
        self._tttr_buffer = None
        self._continuous_buffer = None

# General library routines
    def error_string(self, errcode):
        """
        Return the error string associated with the error code.
        """
        result, errstring = phlib.PH_GetErrorString("", errcode)
        if result == 0:
            return(errstring)
        else:
            return("Error {0} could not be decoded.".format(errcode))

    def library_version(self):
        """
        Return the version number of the shared library.
        """
        result, version = phlib.PH_GetLibraryVersion("")

        if self.CHK(result):
            major, minor = version.split(".")
            return(int(major), int(minor))
    
# Device opening, closing
    def open(self):
        """
        Open the device, and record its serial number.
        """
        result, serial =  phlib.PH_OpenDevice(self.device_index, "")
        self.serial = serial
        
        return(self.CHK(result))

    def close(self):
        """
        Close the device and release it for further use.
        """
        return(self.CHK(phlib.PH_CloseDevice(self.device_index)))

    def initialize(self, mode):
        """
        Prepare the device for a measurement in the specified mode.
        """
 
        result = phlib.PH_Initialize(self.device_index,
                                     self.mode_number(mode))

        return(self.CHK(result))
            
# Hardware information
    def number_of_input_channels(self):
        """
        Determine the number of input channels in the current device.
        """
        return(2)
    
    def hardware_info(self):
        """
        Determine the model and part number of the current device.
        """
        result, model, part_number = phlib.PH_GetHardwareInfo(self.device_index,
                                                             "", "")
        if self.CHK(result):
            return(model, part_number)

    def serial_number(self):
        """
        Determine the serial number of the current device.
        """
        result, serial = phlib.PH_GetSerialNumber(self.device_index, "")

        if self.CHK(result):
            return(serial)       
    
    def base_resolution(self):
        """
        Detrmine the base resolution for the current device.
        """
        
        result = phlib.PH_GetBaseResolution(self.device_index)
        
        if self.CHK(result):
            return(result)

    def calibrate(self):
        """
        Calibrate the current device.
        """

        return(self.CHK(phlib.PH_Calibrate(self.device_index)))


    def set_input_cfd(self, channel, level, zero_cross):
        """
        Set the input CFD discriminator and zero cross level for the
        given channel.
        
        Valid disciminator range: {0} to {1}
        Valid zero cross range: {2} to {3}
        """.format(phlib.DISCRMIN, phlib.DISCRMAX,
                   phlib.ZCMIN, phlib.ZCMAX)
        
        return(self.CHK(phlib.PH_SetInputCFDLevel(self.device_index,
                                                  channel,
                                                  level)) and \
               self.CHK(phlib.PH_SetInputCFDZeroCross(self.device_index,
                                                      channel,
                                                      zero_cross)))

    def set_sync_divider(self, divider):
        """
        Set the synx divider (valid values: 1, 2, 4, ... {0}
        """.format(phlib.SYNCDIVMAX)

        return(self.CHK(phlib.PH_SetSyncDiv(self.device_index,
                                            divider)))

    def set_stop_overflow(self, stop_on_overflow, stop_on_count):
        """
        Set the device to stop on an overflow (True or False) and the count
        at which the device should stop.

        Valid count level range: {0} to {1}
        """.format(0, 65535)
        
        if stop_on_overflow:
            stop_on_overflow = 1
        else:
            stop_on_overflow = 0
        
        result = phlib.PH_SetStopOverflow(self.device_index,
                                          stop_on_overflow,
                                          stop_on_count)
        return(self.CHK(result))

    def set_range(self, measurement_range):
        """
        Set the measurement range.

        Valid range: {0} to {1}
        """.format(0, phlib.RANGES)

        result = phlib.PH_SetRange(self.device_index,
                                   measurement_range)

        return(self.CHK(result))

    def set_offset(self, offset):
        """
        Set the input timing offset, in ps.
        
        Valid range: {0} to {1}
        """.format(phlib.OFFSETMIN, phlib.OFFSETMAX)
        
        result =  phlib.PH_SetOffset(self.device_index,
                                     offset)
        if self.CHK(result):
            # result is the new offset
            return(result)

    def clear_histogram(self, block=0):
        """
        Clear the specified block of the histogram.
        """
        
        result = phlib.PH_ClearHistMem(self.device_index, block)
        return(self.CHK(result))

    def start_measurement(self, acquisition_time):
        """
        Start the current measurement, acquiring for the
        specified time (in milliseconds).

        Valid range: {0} to {1}
        """.format(phlib.ACQTMIN, phlib.ACQTMAX)
        
        result = phlib.PH_StartMeas(self.device_index,
                                    acquisition_time)
        return(self.CHK(result))

    def stop_measurement(self):
        """
        Stop the currently-running measurement.
        """

        result = phlib.PH_StopMeas(self.device_index)
        return(self.CHK(result))

    def ctc_status(self):
        """
        Determine the current status of the experiment. Returns True is the
        experiment is complete, and False otherwise.
        """

        result = phlib.PH_CTCStatus(self.device_index)

        if self.CHK(result):
            # 0 is running, >0 is finished.
            return(result)

    def get_block(self, block=0):
        """
        Get the specified block of the current histogram.
        """

        if not self._histogram:
            raise(ValueError("No histogram buffer found."))

        result = phlib.PH_GetHistogram(self.device_index,
                                       self._histogram,
                                       block)

        if self.CHK(result):
            return(self._histogram)

    def get_resolution(self):
        """
        Determine the resolution at the current binning settings, in ps.
        """
        
        result = phlib.PH_GetResolution(self.device_index,)

        if self.CHK(result):
            # result is an integer resolution, in ps
            return(result)

    def get_count_rate(self, channel):
        """
        Determine the current count rate on the specified channel.
        """
        
        result = phlib.PH_GetCountRate(self.device_index,
                                       channel)

        if self.CHK(result):
            # result is counts per second
            return(result)

    def get_flags(self):
        """
        Determine the current status flags.
        """

        flags = phlib.PH_GetFlags(self.device_index)
    
        # Extract the status value from the flag
        found = list()

        for name, value in self.flags():
            if flags & value:
                found.append(name)

        return(tuple(found))

    def get_elapsed_measurement_time(self):
        """
        Determine the elapsed time in the current measurement, in ms.
        """

        elapsed = phlib.PH_GetElapsedMeasTime(self.device_index)
        return(elapsed)

    def get_warnings(self):
        """
        Determine the current warnings for the device.
        """
        
        for channel in range(self.number_of_input_channels()):
            self.get_count_rate(chanel)

        # Now get and interpret the warnings
        warnings = phlib.PH_GetWarnings(self.device_index)

        if self.CHK(warnings):
            return(warnings)

    def warnings_text(self, warnings):
        """
        Determine the long-form warning text for the current warnings.
        """

        result, text = phlib.PH_GetWarningsText(self.device_index,
                                                warnings,
                                                "")

        if self.CHK(result):
            return(text)

# TTTR
    def read_tttr(self):
        """
        Read TTTR records from the current measurement.
        """

        if not self._fifo_buffer:
            raise(ValueError("No fifo buffer."))

        actual_read = phlib.intp()
        result = phlib.PH_TTReadData(self.device_index,
                                     self._tttr_buffer,
                                     len(self._tttr_buffer))

        if self.CHK(result):
            return(self._tttr_buffer[:result])        

    def set_marker_edges(self, edges):
        """
        Set the marker edges (0=falling, 1=rising) for the 4 marker channels.
        Edges should be passed as a list or tuple of integers.
        """

        if len(edges) != 4:
            raise(ValueError("Must specify all four edges."))
        
        edges = tuple(edges)
        result = phlib.PH_TTSetMarkerEdges(self.device_index,
                                           *edges)

        return(self.CHK(result))

# Routing
    def get_routing_channels(self):
        """7
        Determine the number of available routing channels.
        """

        result = phlib.PH_GetRoutingChannels(self.device_index)

        if self.CHK(result):
            return(result)

    def enable_routing(self, state):
        """
        Enable or disable (True or False) routing. An error may indicate the
        lack of a router.
        """

        if state:
            state = 1
        else:
            state = 0
            
        result = phlib.PH_EnableRouting(self.device_index,
                                        state)

        return(self.CHK(result))

    def get_router_version(self):
        """
        Determine the router model and version for the router attached
        the to current device, as well as the number of routing channels
        available.
        """

        result, model, version = phlib.PH_GetRouterVersion(self.device_index,
                                                           "",
                                                           "")

        if self.CHK(result):
            return(result, model, version)

    def set_router_input(self, channel, level, edge):
        """
        Set the trigger voltage and edge for the specified channel of the
        PHR800 router.

        Valid level (mV): {0} to {1}
        Edges: 0=falling, 1=rising
        """.format(-1600, 2400)

        result = phlib.PH_SetPHR800Input(self.device_index,
                                         channel,
                                         level,
                                         edge)

        if self.CHK(result):
            # return value is the number of routing channels
            return(result)

    def set_router_cfd(self, channel, level, zero_cross):
        """
        Set the discriminator and zero cross levels for the specified
        router channel.
        """

        result = phlib.PH_SetPHR800CFD(self.device_index,
                                       channel,
                                       level,
                                       zero_cross)

        if self.CHK(result):
            return(result)
        
if __name__ == "__main__":
    ph = PicoHarp()
##    ph.open()
    print(ph.valid_mode("t2"), ph.valid_mode("t3"))
    print(ph.modes())
    print(ph.flags())
    print(ph.set_router_input(0, 100, 1))
