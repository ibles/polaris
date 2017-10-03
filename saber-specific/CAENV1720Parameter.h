#ifndef CAENV1720PARAMETER_H
    #define CAENV1720PARAMETER_H

#include "VMEBoardParameter.h"
#include "ConfigParser.h"

#include <cstdint>


/// Start mode of digitizer V1720. 
//
/// REG_CON: start run by register write access.
/// FIRST_TRIG_CON: run is started by the first input trigger. This trigger does not start recording of the event.
enum V1720_RUNMODE{
    REG_CON = 0x0,
    SIN_CON = 0x1,
    FIRST_TRIG_CON = 0x2,
    MULTI_SYNC = 0x3
};
// Note that forward declaration of enum is not allowed in C++


/// Important registers for using CAEN V1720 board.
enum V1720_REG {
    /*  local trigger */
    CHN_TRIG_THRESH = 0x1080,       // [11:0] trigger threshold for ch n n*0x100. LSB = input range / 12bit
    CHN_NSAMP_OVER_THRESH = 0x1084, // [11:0] remain cross-thresh for N samples (at least 4 samples)

    /*  channel n status */
    CHN_STATUS = 0x1088,            // [0] loc mem full, [1] mem empty, [2] 1 = DAC busy, 0 = DAC updated, [5] 1 = buffer free error
    CHN_BUFF_OCP = 0x1094,          // [10:0] number of occupied buffers

    /*  channel n DAC and ADC */
    CHN_DAC = 0x1098,               // [15:0] DAC offset in +/-1 V range. Wait for CHN_STATUS bit-2 to update
    CHN_ADC = 0x109C,               // T.B.D.

    /*  channel configuration */
    CH_CONFIG = 0x8000,             // [19:16] zero suppression, 0000 = no zero suppression, [11] 0 = pack2.5 disabled
                                    // [6] 0 = Trigger output over threshold, 1 = under threshold
                                    // [4] 0 = memory random access, 1 = memory sequential access (need sequential)
                                    // [3] 0 = disable test pattern generation
                                    // [1] 0 = trigger overlapping disabled
    CH_CONFIG_BIT_SET = 0x8004,     // [7:0] 
    CH_CONFIG_BIT_CLR = 0x8008,
    CH_ENABLE_MASK = 0x8120,        // [7:0] enabled channels store sample. Cannot be changed during acquisition.

    /*  Event organization */
    BUFFER_ORG = 0x800C,            // [3:0] buffer code, buff num = 2^code, SRAM size = 1024*1024/buff num.
    BUFFER_FREE = 0x8010,           // [11:0] free first N buffer blocks.
    CUSTOM_SIZE = 0x8020,           // [31:0] 0=disable, event is forced to be 4*N SAMPLES. (2*N 32-bit words)
    POST_TRIG_SETTING = 0x8114,     // [31:0] number of post trigger SAMPLE is 4*value of this register
    
    EVT_STORED = 0x812C,            // [31:0] Event stored
    EVT_SIZE = 0x814C,              // [31:0] No. of 32-bit word for next event.
    BLT_EVT_NUM = 0xEF1C,           // [7:0] No. of complete events which has to be transferred via BLT.

    /*  Acquisition control */
    ACQ_CON = 0x8100,               // [3] 0=count accepted trigger, 1=count all triggers;
                                    // [2] 0=acquisition STOP, 1=acquisition RUN
                                    // [1:0] 00=register controlled run mode, 01=S-IN controlled mode, 10=FIRST TRIG START MODE, 11=--
    ACQ_STATUS = 0x8104,            // [8] 1=board ready for acquisition, 0=not ready; [7] 0=PLL loss of lock, [6] 1=PLL bypass mode
                                    // [5] 0=internal clk source,
                                    // [4] 1=event full, [3] 1=event ready, [2] 0=run off
    SOFT_RESET = 0xEF24,
    SOFT_CLEAR = 0xEF28,
   
    /*  global trigger and output */
    SOFT_TRIGGER = 0x8108,          // write access generates software trigger
    TRIG_SRC_MASK = 0x810C,         // [31] 0=software trig disabled, [30] 0=ext trig disabled
                                    // [26:24] local trig coin level
                                    // [7:0] 0=channel n local trig disabled

    FRONT_PANEL_TRIGOUT = 0x8110,   // determine which source can output TRIG_OUT on the front panel
                                        // [31,30] soft and ext, [7:0] local trig, 0 for disable
    /*  Front panel */
    FRONT_PANEL_IODATA = 0x8118,    // readout and set IO levels of front panel LVDS
    FRONT_PANEL_IOCON = 0x811C,     // [15] 0=normal, 1=IO test mode [14] 1=TRIG-OUT test mode set to 1
                                    // [9] pattern in event header 1=latched with ext trigger, 0=internal trigger
                                    // [7:6] 00=general purpose IO
                                    // [5:2] 0=LVDS IO 15..12 (11..8, and so forth) are input, 1=output
                                    // [1] 0=panel output (TRIG_OUT, CLKOUT) enabled, 1=enabled in high impedance mode
                                    // [0] 0=TRIG/CLK are NIM level, 1=TTL levels

    /*  zero suppression time and threshold for channel n */
    CHN_ZS_THRESH = 0x1024,
    CHN_ZS_NSAMP = 0x1028,
    VME_CONTROL = 0xEF00,           // mainly interrupts on VME bus
    VME_STATUS = 0xEF04,            // BERR, output buffer, event ready, etc.
    FPGA_FIRMWARE_REV = 0x8124,     // 
    BOARD_INFO = 0x8140,            // [15:8] mem. size, [7:0] board type, 0x03=V1720
    BOARD_ID = 0xEF08,              // 
};


/// Parameters of CAEN V1720 digitizer.
//
class CAENV1720Parameter : public VMEBoardParameter{

public:

    CAENV1720Parameter();
        // handle by default 0.

    ~CAENV1720Parameter();

    void SetParamFromConfig( ConfigParser*, string s="/module/daq/board0");
        //!< It returns a V1720 parameter object from SVParamHandler using GetParameter<T> template function.
        //!< This method will be used to initialize the parameter object for the board from user-specified config file.

    void Print();
        //!< Prints out all parameter settings. Mainly for debug purpose.

    int GetNChanEnabled();
        //!< Returns number of enabled channels.

    uint32_t GetEvtSizeInSamp();
        //!< Get number of samples in one event. This does not include event header.

    uint32_t GetEvtSizeInWord();
        //!< Returns number of 32-bit words in one event. This includes event header.

    uint32_t GetEvtSizeInByte();
        //!< Returns event size in bytes. This includes event header.



    /* local channel setting */
    uint32_t threshold[8];
        //!< Local channel threshold.

    uint32_t tcrossthresh[8];
        //!< # of sample that signal must stay above/below threshold to issue a local trigger.
    
    uint32_t dac[8];
        //!< DAC register controls DC offset applied to the signal.


    /* channel global setting */
    uint32_t ch_enable_mask;
        //!< This mask controls which channels are enabled for data recording.
    
    bool trig_over_threshold;
        //!< If true, trigger issued when signal goes above threshold. Otherwise it is below threshold.
    
    bool trig_overlap;
        //!< If true, trigger overlap is enabled.



    /* event organization */
    bool enable_custom_size;
        //!< Custom size of number of samples of an event. (not power of 2).
    
    uint32_t pre_trig_sample;
        //!< Number of pre-trigger samples.
    
    uint32_t post_trig_sample;
        //!< Number of post-trigger samples.
    
    uint32_t buff_code;
        //!< Buffer code that divides the memory.



    /* acquisition control */
    V1720_RUNMODE runmode;



    /* front panel IO control */
    bool lvds_io_output;
        //!< If true, LVDS front panel will be set as output.
    
    bool logic_level_ttl;
        //!< If true, front panel uses TTL signal level.



    /* trigger source  and front panel output*/
    bool sw_trig_enable;
        //!< If enabled, write access to software trigger register will trigger data acquisition.
    
    bool sw_fp_trigout;
        //!< If true, software trigger is also propagated to the LEMO output on the front panel.

    bool ext_trig_enable;
        //!< If true, external trigger input will trigger DAQ.
    
    bool ext_fp_trigout;
        //!< If true, external trigger received will be propagated to LEMO output on front panel.

    uint32_t local_trig_enable;
        //!< Local trigger by threshold can generate local trigger for acquisition
    
    uint32_t local_fp_trigout;
        //!< Local trigger by threshold will output TTL/NIM signal on the LEMO output on the front panel.
        //!< This output is not the LVDS signal.

    int GetHeaderSize();

    void Serialize( char* p );

    void Deserialize( char* p, bool flip );
};


#endif
