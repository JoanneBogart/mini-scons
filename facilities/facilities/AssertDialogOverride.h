#include <crtdbg.h>
#include <iostream>

/*
 * In order to use this function the following lines of code have to be placed
 * in the file containing main() for the test or program.
 *
// Place the following in the includes section:

#ifdef WIN32
#include "facilities/AssertDialogOverride.h"
#endif

// Place the following at the beginning of the main() function:

#ifdef _DEBUG
   _CrtSetReportHook( AssertDialogOverride );
   _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);
   _CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_FILE);
   _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE);
#endif

 */

/* 
 * Define our own reporting function.
 * We hook it into the debug reporting
 * process later using _CrtSetReportHook.
 */
int AssertDialogOverride( int reportType, char *userMessage, int *retVal )
{
   /*
    * Tell the user our reporting function is being called.
    * In other words - verify that the hook routine worked.
    */
	std::cout << "Overriding assertion dialog box." << std::endl;
	
   /*
    * By setting retVal to zero, we are instructing _CrtDbgReport
    * to continue with normal execution after generating the report.
    * If we wanted _CrtDbgReport to start the debugger, we would set
    * retVal to one.
    */
   retVal = 0;
	
   /*
    * When the report type is for an ASSERT,
    * we'll redirect to std::cout instead of having a dialog appear
    * so we'll return false and have processing stop here.
    *
    * When the report type is a WARNing or ERROR,
    * we'll just pass it on to the system so return true.
    */
   if (reportType == _CRT_ASSERT)
   {
	std::cout << "Assertion failure:" << std::endl;
	std::cout << userMessage << std::endl;
	std::cout << "Shutting down the test." << std::endl;
	exit(10);
   } else {
	std::cout << "Warning or Error:" << std::endl;
	std::cout << userMessage << std::endl;
	return(true);
   }
}
