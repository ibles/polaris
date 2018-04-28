#ifndef RANDOMWALKDAQ_H
    #define RANDOMWALKDAQ_H 1

#include <fstream>

#include "ConfigParser.h"
#include "plrsModuleDAQ.h"
#include "plrsController.h"


class RandomWalkDAQ : public plrsModuleDAQ {

public:
    RandomWalkDAQ( plrsController* ctrl);

    ~RandomWalkDAQ();

protected:
    void Configure();

    void Deconfigure();

    void Deinitialize();

    void PreRun();

    void StopDAQ();

    void PreEvent();

    void Event();

    void PostEvent();

private:

    ifstream file;

    unsigned int start_time;

    int sample_intv;

    int buff_depth;

    int current_value;
};


extern "C" RandomWalkDAQ* create_RandomWalkDAQ( plrsController* c);

extern "C" void destroy_RandomWalkDAQ( RandomWalkDAQ* p );

#endif
