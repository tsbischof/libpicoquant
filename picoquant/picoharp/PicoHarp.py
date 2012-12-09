## To do: work out how to make an unsigned int array for data transfer

from picoquant.tcspc import TCSPC
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
        self._flags = None
        self._fifo_buffer = None
        self._continuous_buffer = None

# General library routines
    def error_string(self, errcode):
        """
        Return the error string associated with the error code.
        """
        result, errstring = phlib.ph_GetErrorString("", errcode)
        if result == 0:
            return(errstring)
        else:
            return("Error {0} could not be decoded.".format(errcode))

    def library_version(self):
        """
        Return the version number of the shared library.
        """
        result, version = phlib.ph_GetLibraryVersion("")

        if self.CHK(result):
            major, minor = version.split(".")
            return(major, minor)
    
# Device opening, closing
    def open(self):
        """
        Open the device, and record its serial number.
        """
        result, serial =  phlib.ph_OpenDevice(self.device_index, "")
        self.serial = serial
        
        return(self.CHK(result))

    def close(self):
        """
        Close the device and release it for further use.
        """
        return(self.CHK(phlib.ph_CloseDevice(self.device_index)))

    def initialize(self, mode, reference_source=0):
        """
        Prepare the device for a measurement in the specified mode,
        using the specified reference source.
        Reference source: 0 = internal, 1 = external
        """
        if not reference_source in self.reference_sources:
            raise(ValueError("Unsupported reference source: "
                                 "{0})".format(reference_source)))
        else:
            self.CHK(phlib.ph_Initialize(self.device_index,
                                         self.mode_number(mode),
                                         reference_source))
                
# Hardware information
    def hardware_info(self):
        """
        Determine the model and part number of the current device.
        """
        result, model, part_number = phlib.ph_GetHardwareInfo(self.device_index,
                                                             "", "")
        if self.CHK(result):
            return(model, part_number)

    def serial_number(self):
        """
        Determine the serial number of the current device.
        """
        result, serial = phlib.ph_GetSerialNumber(self.device_index, "")

        if self.CHK(result):
            return(serial)       
    
    def base_resolution(self):
        """
        Detrmine the base resolution and maximum allowed binning steps
        for the current device.
        """
        resolution = phlib.doublep()
        binning_steps = phlib.intp()

        result = phlib.ph_GetBaseResolution(self.device_index,
                                            resolution,
                                            binning_steps)
        
        if self.CHK(result):
            return(resolution.value(), binning_steps.value())

    def number_of_input_channels(self):
        """
        Determine the number of installed input channels in the current device.
        """
        input_channels = phlib.intp()

        result = phlib.ph_GetNumOfInputChannels(self.device_index,
                                                input_channels)

        if self.CHK(result):
            return(input_channels.value())

    def number_of_modules(self):
        """
        Determine the number of installed modules in the current device.
        """
        modules = phlib.intp()

        result = phlib.ph_GetNumOfModules(self.device_index,
                                          modules)

        if self.CHK(result):
            return(modules.value())

    def module_info(self, module):
        """
        Determine the model and version code for the specified module.
        """
        model_code = phlib.intp()
        version_code = phlib.intp()

        result = phlib.ph_GetModuleInfo(self.device_index,
                                        module,
                                        model_code,
                                        version_code)

        if self.CHK(result):
            return(model_code.value(), version_code.value())

    def module_index(self, channel):
        """
        Determine the index of the module where a given input channel resides.
        """
        module_index = phlib.intp()

        result = phlib.ph_GetModuleIndex(self.device_index,
                                         channel,
                                         module_index)

        if self.CHK(result):
            return(module_index.value())

    def calibrate(self):
        """
        Calibrate the current device.
        """

        return(self.CHK(phlib.ph_Calibrate(self.device_index)))

    def set_sync_divider(self, divider):
        """
        Set the synx divider (valid values: 1, 2, 4, ... {0}
        """.format(phlib.SYNCDIVMAX)

        return(self.CHK(phlib.ph_SetSyncDiv(self.device_index,
                                            divider)))

    def set_sync_CFD(self, level, zero_cross):
        """
        Set the sync CFD discriminator and zero cross level, in mV.
        
        Valid discriminator range: {0} to {1}
        Valid zero cross range: {2} to {3}
        """.format(phlib.DISCRMIN, phlib.DISCRMAX,
                   phlib.ZCMIN, phlib.ZCMAX)
               
        major, minor = self.library_version()
        if major == 1:
            # v1 has two separate calls
            result = \
                   self.CHK(phlib.ph_SetSyncCFDLevel(self.device_index, \
                                                     level)) and \
                   self.CHK(phlib.ph_SetSyncCFDZeroCross(self.device_index, \
                                                         zero_cross))
                                            
        elif major >= 2:
            result = self.CHK(phlib.ph_SetSyncCFD(self.device_index,
                                                  level,
                                                  zero_cross))

        return(result)

    def set_sync_channel_offset(self, offset):
        """
        Set the offset of the sync channel, in ps.
        Valid range: {0} to {1}
        """.format(phlib.CHANOFFSMIN, phlib.CHANOFFSMAX)
        
        return(self.CHK(phlib.ph_SetSyncChannelOffset(self.device_index,
                                                      offset)))

    def set_input_cfd(self, channel, level, zero_cross):
        """
        Set the input CFD discriminator and zero cross level for the
        given channel.
        
        Valid disciminator range: {0} to {1}
        Valid zero cross range: {2} to {3}
        """.format(phlib.DISCRMIN, phlib.DISCRMAX,
                   phlib.ZCMIN, phlib.ZCMAX)
        
        return(self.CHK(phlib.ph_SetInputCFDLevel(self.device_index,
                                                  channel,
                                                  level)) and \
               self.CHK(phlib.ph_SetInputCFDZeroCross(self.device_index,
                                                      channel,
                                                      zero_cross)))

    def set_input_channel_offset(self, channel, offset):
        """
        Set the channel timing offset, in ps.
        
        Valid range: {0} to {1}
        """.format(phlib.CHANOFFSMIN, phlib.CHANOFFSMAX)
        
        result =  phlib.ph_SetInputChannelOffset(self.device_index,
                                                 channel,
                                                 offset)
        return(self.CHK(result))

    def set_stop_overflow(self, stop_on_overflow, stop_on_count):
        """
        Set the device to stop on an overflow (True or False) and the count
        at which the device should stop.

        Valid count level range: {0} to {1}
        """.format(phlib.STOPCNTMIN, phlib.STOPCNTMAX)
        
        if stop_on_overflow:
            stop_on_overflow = 1
        else:
            stop_on_overflow = 0
        
        result = phlib.ph_SetStopOverflow(self.device_index,
                                          stop_on_overflow,
                                          stop_on_count)
        return(self.CHK(result))

    def set_binning(self, binning):
        """
        Set the measurement binning code.

        Valid range: {0} to {1}
        """.format(0, phlib.MAXBINSTEPS-1)
        
        result =  phlib.ph_SetBinnin(self.device_index,
                                     binning)
        return(self.CHK(result))

    def set_offset(self, offset):
        """
        Set the histogram time offset, in ps.

        Valid range: {0} to {1}
        """.format(phlib.OFFSETMIN, phlib.OFFSETMAX)
        
        result = phlib.ph_SetOffset(self.device_index,
                                    offset)
        return(self.CHK(result))

    def set_histogram_length(self, length):
        """
        Set the histogram length, and return the actual length of the
        histogram, in the number of time bins.

        Valid range: {0} to {1}
        """.format(0, phlib.MAXLENCODE)
        
        actual_length = phlib.intp()
        result = phlib.ph_SetHistoLen(self.device_index,
                                      length,
                                      actual_length)
        if self.CHK(result):
            # Create the acquisition buffer.
            return(1024*actual_length.value()**2)
                                      
    def clear_histogram(self):
        """
        Clear the current histogram in memory.
        """
        
        result = phlib.ph_ClearHistMem(self.device_index)
        return(self.CHK(result))

    def start_measurement(self, acquisition_time):
        """
        Start the current measurement, acquiring for the
        specified time (in milliseconds).

        Valid range: {0} to {1}
        """.format(phlib.ACQTMIN, phlib.ACQTMAX)
        
        result = phlib.ph_StartMeas(self.device_index,
                                    acquisition_time)
        return(self.CHK(result))

    def stop_measurement(self):
        """
        Stop the currently-running measurement.
        """

        result = phlib.ph_StopMeas(self.device_index)
        return(self.CHK(result))

    def ctc_status(self):
        """
        Determine the current status of the experiment. Returns True is the
        experiment is complete, and False otherwise.
        """

        status = phlib.intp()
        result = phlib.ph_CTCStatus(self.device_index,
                                    status)

        if self.CHK(result):
            # 0 is running, 1 is complete.
            return(not status.value())

    def get_histogram(self, channel, clear=False):
        """
        Return the current histogram from the specified channe, and optionally
        clear it from the acquisition buffer.
        """

        # First, make sure we have space allocated.
        if not self._histogram:
            raise(ValueError("No acquisition buffer found, did you call "
                             "set_histogram_length()?"))

        if clear:
            clear = 1
        else:
            clear = 0

        result = phlib.ph_GetHistogram(self.device_index,
                                       self._histogram,
                                       channel,
                                       clear)

        if self.CHK(result):
            return(self._histogram)

    def get_resolution(self):
        """
        Determine the resolution at the current binning settings, in ps.
        """

        resolution = phlib.doublep()
        result = phlib.ph_GetResolution(self.device_index,
                                        resolution)

        if self.CHK(result):
            return(resolution.value())

    def get_sync_rate(self):
        """
        Determine the current sync rate.
        """

        sync_rate = phlib.intp()
        result = phlib.ph_GetSyncRate(self.device_index,
                                      sync_rate)

        if self.CHK(result):
            return(sync_rate.value())

    def get_count_rate(self, channel):
        """
        Determine the current count rate on the specified channel.
        """

        count_rate = phlib.intp()
        result = phlib.ph_GetCountRate(self.device_index,
                                       channel,
                                       count_rate)

        if self.CHK(result):
            return(count_rate.value())

    def get_flags(self):
        """
        Determine the current status flags.
        """

        flags = phlib.intp()
        result = phlib.ph_GetFlags(self.device_index,
                                   flags)
    
        if self.CHK(result):
            # Extract the status value from the flag
            found = list()

            for name, value in self.flags():
                if flags.value() & value:
                    found.append(name)

            return(tuple(found))

    def get_elapsed_measurement_time(self):
        """
        Determine the elapsed time in the current measurement, in ms.
        """

        elapsed = phlib.doublep()
        result = phlib.ph_GetElapsedMeasTime(self.device_index,
                                             elapsed)

        if self.CHK(result):
            return(elapsed.value())

    def get_warnings(self):
        """
        Determine the current warnings for the device.
        """

        # Call ph_GetCountRate and ph_GetSyncRate for all channels first.
        self.get_sync_rate()
        
        for channel in range(self.number_of_input_channels()):
            self.get_count_rate(chanel)

        # Now get and interpret the warnings
        warnings = phlib.ph_GetWarnings(self.device_index)

        if self.CHK(warnings):
            return(warnings.value())

    def warnings_text(self, warnings):
        """
        Determine the long-form warning text for the current warnings.
        """

        result, text = phlib.ph_GetWarningsText(self.device_index,
                                                "",
                                                warnings)

        if self.CHK(result):
            return(text.value())

# TTTR
    def read_tttr(self):
        """
        Read TTTR records from the current measurement.
        """

        if not self._fifo_buffer:
            raise(ValueError("No fifo buffer."))

        actual_read = phlib.intp()
        result = phlib.ph_ReadFiFo(self.device_index,
                                   self._fifo_buffer,
                                   len(self._fifo_buffer),
                                   actual_read)

        if self.CHK(result):
            return(self._fifo_buffer[:actual_read.value()])        

    def set_marker_edges(self, edges):
        """
        Set the marker edges (0=falling, 1=rising) for the 4 marker channels.
        Edges should be passed as a list or tuple of integers.
        """

        if len(edges) != 4:
            raise(ValueError("Must specify all four edges."))
        
        edges = tuple(edges)
        result = phlib.ph_SetMarkerEdges(self.device_index,
                                         *edges)

        return(self.CHK(result))

    def enable_markers(self, markers):
        """
        Enable or disable each marker signal (True=disable, False=enable).
        Markers should be passed as a list or tuple of booleans, and must
        specify all 4 marker states.
        """

        if len(markers) != 4:
            raise(ValueError("Must specify all 4 marker channel states."))

        my_markers = list()

        for marker in markers:
            if marker:
                my_markers.append(1)
            else:
                my_markers.append(0)

        markers = tuple(my_markers)
        result = phlib.ph_SetMarkerEnable(self.device_index,
                                          *markers)

        return(self.CHK(result))

# Continuous
    def get_continuous_mode_block(self):
        """
        Get the current continuous mode data.
        """

        if not self._continuous_buffer:
            raise(ValueError("No continuous mode buffer found."))

        bytes_received = phlib.intp()

        result = phlib.ph_GetContModeBlock(self.device_index,
                                           self._continuous_buffer,
                                           bytes_received)

        if self.CHK(result):
            return(self._continuous_buffer[0:bytes_received.value()])

if __name__ == "__main__":
    ph = PicoHarp()
##    ph.open()
    print(ph.valid_mode("t4"))
    print(ph.modes())
    print(ph.flags())
