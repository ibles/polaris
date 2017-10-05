#include "SerialDAQ.h"

#include <sstream>

extern "C" SerialDAQ* create_SerialDAQ( plrsController* c ){ return new SerialDAQ(c);}


extern "C" void destroy_SerialDAQ( SerialDAQ* p ){ delete p;}


SerialDAQ::SerialDAQ( plrsController* ctrl) : plrsModuleDAQ( ctrl){
    buff_size = 1000;
        // will create 100 buffer data
    sampling_period = 100;
        // sampling period in ms
}


SerialDAQ::~SerialDAQ(){}


void SerialDAQ::Configure(){
    Print("configuring serial port for DAQ\n", DETAIL);

    string fname = cparser->GetString("/module/daq/port");
    if( fname=="" ){
        fname = "ttyS0";
        Print( "Cannot find /module/daq/port, using ttyS0\n", ERR);
    }
    else
        Print( "opening "+fname+" for data acquisition\n", DETAIL);

    port.set_raw();
    port.set_baud( B9600 );

    if( port.serial_open( fname.c_str() )<0 ){
        Print( "Error: "+port.get_errmsg()+"\n", ERR);
        SetStatus( ERROR );
        return;
    }

    for( int i=0; i<buff_size; ++i ){
        int id = ctrl->GetIDByName( this->GetModuleName() );
        PushToBuffer( id, new int );
    }
    Print("serial DAQ module configured.\n", DETAIL);
}



void SerialDAQ::UnConfigure(){
    port.serial_close();
}



void SerialDAQ::CleanUp(){

    Print( "cleaning up...\n", DETAIL);

    port.serial_close();

    void* rdo = PullFromBuffer();
    while( rdo!=0 ){
        delete reinterpret_cast<int*>(rdo);
        rdo = PullFromBuffer();
        sched_yield();
    }
}



void SerialDAQ::Event(){
    char msb(0), lsb(0);

    void* rdo = 0;

    rdo = PullFromBuffer();
    while( rdo==0 ){
        if( GetState()!=RUN )
            break;
        rdo = PullFromBuffer();
        if( rdo==0 )
            sched_yield();
    }
    if( rdo==0 )
        return;

    while( port.serial_read( &lsb, 1)<0 ){
        if(GetState()!=RUN)
            break;
        sched_yield();
    }
    while( port.serial_read( &msb, 1)<0 ){
        if(GetState()!=RUN)
            break;
        sched_yield();
    }

    int* a = ( reinterpret_cast<int*> ( rdo ));
    *a = ( (lsb>>4)&0x0f ) + (int(msb)<<4);

    PushToBuffer( addr_nxt, rdo);
}


void SerialDAQ::StartDAQ(){
    Print( "DAQ starting\n", DETAIL);

    stringstream s;
    s << "DAQ will push data to " << ctrl->GetNameByID(addr_nxt) << endl;
    Print( s.str(), DETAIL);

    char c = 0x1;
    port.serial_write( &c, 1);
}


void SerialDAQ::StopDAQ(){
    char c = 0xff;
    port.serial_write( &c, 1);
    Print( "DAQ Stopped\n", DETAIL);
}


void SerialDAQ::PreEvent(){}


void SerialDAQ::PostEvent(){}