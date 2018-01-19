#ifndef PLRSMODULEINPUT_H
    #define PLRSMODULEINPUT_H 1

#include <string>
#include <ostream>
#include <queue>

#include "plrsStateMachine.h"


//void* InputHandler( void* );   //!< Gets input and makes input available to different modules.


class plrsModuleInput : public plrsStateMachine{

public:

    plrsModuleInput( plrsController* c);
        //!< Constructor.

    virtual ~plrsModuleInput();
        //!< Destructor

    string GetModuleName(){ return "input";}

    void SendUserCommand( string );

protected:

    void Initialize();

    void Configure();

    void ConfigDataFlow();

	void UnConfigure();

    void CleanUp();

    void PreRun();

    void Run();

    void PostRun();

private:

    //pthread_t thread_input; //!< Launches a separate thread to get input

    //string input;       //!< Input string available to Input module.

    void IOHandler();   //!< Handles output. Output at different level: verbosity -1 (error), 0, 1, 2.

    //string getstr();
        //!< Non-blocking input. Internally this is handled with a static flag variable and a static FIFO buffer

};

#endif
