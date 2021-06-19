/******************************************************************************
 *
 *  PROGRAM: bed.cpp
 *
 *  AUTHOR:  J. Matt Pontius
 *
 * SUMMARY: Compiles and tests user specified source code files according to the
 *          user's predefined test instructions file.  Results of the test are
 *          output to the screen and, optionally, to files if this program is
 *          run in * "grade mode".
 *
 ******************************************************************************/

#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <math.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <time.h>

// Included files
#include "Assignment.h"
#include "Instruction.h"
#include "Penalty.h"
#include "Process.h"
#include "getCommandLine.h"
#include "getAssgFile.h"
#include "getInstructions.h"
#include "getResults.h"
#include "findDifference.h"
#include "writeResults.h"

using namespace std;

#define MYNAM "bed"

#define ASSGHOM "/mnt/local/testbed/"

// --- The file that stores all testing output
#define CLASSLOGFIL "zclass.log"

// --- The suffix to append to the end of each student's log file
#define LOGSUFFIX ".log"

// --- The suffix that the compile program specified in the assignment file
//     appends to each students student's executable file.
#define XSUFFIX ".exe"

// --- The suffix to append to each student's penalty/grade file
#define GRADSUFFIX ".grade"

// --- The suffix that the compile program specified in the assignment file
//     appends to each student's compiler output file.
#define COMPILSUFFIX ".compile"

// --- The maximum number of instructions allowed
#define MAXINSTRUCTS 1000

// --- The maximum number of penalties allowed (Theoretically, there could be
//     more penalties than instructions - a possible source for segmentation
//     fault!)
#define MAXPNALTIS 1500

// --- The number of milliseconds to sleep between each test instruction
#define DFAULTSLP 35
// ^ Changed from 100 by Seth Grover in order to fulfill Bro. Twitchell's
//   tyranical desires to make the test bed go faster. If the program is
//   too fast, increase this value..
#define MAXARGS 5
// the maximum number of command line arguments allowed to a student's program.
// feature added by Dallin Wilcox to make Bro. Helfrich happy.

// --- These are screen codes taken from the UNIX tput command.  See the
//     man page on terminfo and tput for more info.  They are used for
//     formatting output to the screen.  If weird output is observed, this is
//     probably where it's happening.
#define BOLD "\[0;1m" //[1m"
#define UL "\[0;4m"   //[4m"
#define RVRS "\[0;7m" //[7m"
#define BP ""
#define RST "\[0m" //[m"

// Assignment was here

// Instruction was here

// Pentalty was here

// class Process goes here

// getCommandLine() here

// getAssgFile() here

// getInstructions() here

// getResults() here

// findDifference() here

// writeResults() here

main(int argc, char *argv[])
/*******************************************************************************
*
*******************************************************************************/
{
    // --- Holds the assignment definition data
    Assignment myAssg;
    // --- Holds the test instructions
    Instruction **myInstructs = new Instruction *[MAXINSTRUCTS];
    // --- Holds the testing results
    Instruction **myResults = new Instruction *[MAXINSTRUCTS];
    // --- Holds the testing penalties
    Penalty *myPenalties = new Penalty[MAXPNALTIS];
    char fileName[100];
    int index;

    // --- rase the class log file if it exists.  I can't remember why this
    //     was necessary.
    unlink(CLASSLOGFIL);

    getCommandLine(argc, argv, myAssg);
    getAssgFile(myAssg);
    getInstructions(myAssg, myInstructs);

    // --- Set the penalty for a time out
    Process::setTimePenalty(myAssg.programTimePenalty);

    // --- Loop through every source file listed in the command line
    for (int i = 0; i < myAssg.sourceCount; i++)
    {
        // --- Reset the penalty for a program crash
        Process::setPenalty(&myPenalties[0]);

        index = 0;
        myAssg.sourceIndex = i;

        getResults(myAssg, myInstructs, myResults, myPenalties);

        writeResults(myAssg, myInstructs, myResults, myPenalties);

        // --- If the testing is not occuring in grade mode erase the compiler
        //     output (including the executable file.)
        if (!myAssg.gradeMode)
        {
            strcpy(fileName, myAssg.sourceFiles[myAssg.sourceIndex]);
            strcat(fileName, XSUFFIX);
            unlink(fileName);
            strcpy(fileName, myAssg.sourceFiles[myAssg.sourceIndex]);
            strcat(fileName, COMPILSUFFIX);
            unlink(fileName);
        }

        // --- Reset the penalty array for the next program test
        delete[] myPenalties;
        myPenalties = new Penalty[MAXPNALTIS];

        // --- Reset the results array for the next program test
        while (myResults[index])
        {
            delete myResults[index];
            myResults[index++] = NULL;
        }
    }
}