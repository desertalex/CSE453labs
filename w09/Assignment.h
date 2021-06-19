struct Assignment
/******************************************************************************
* Summary:
*    Stores all the info gathered from the command line arguments and the
*    assignment file.
*
* Note:
*    If a lengthy path is used as the home directory for this program, some of
*    the following arrays may have to be enlarged to accomodate.
*******************************************************************************/
{
    Assignment(void)
    {
        compileProgram[0] = '\0';
    }

    // --- The directory where the assignment file is stored
    char assgDir[70];

    // --- The directory and file name of the assignment file
    char assgFile[70];

    // --- Is true if in grade mode, false otherwise
    bool gradeMode;

    // --- An array of source code file names that will be tested
    char **sourceFiles;

    // --- The length of the sourceFiles array
    int sourceCount;

    // --- The current location in the sourceFiles array
    int sourceIndex;

    // --- The program to be executed to check each student's source code for
    //     style compliance.  (This is not currently implemented.)
    char styleProgram[70];

    // --- The program to be executed to compile each student's source code
    char compileProgram[70];

    // --- The directory and file name of the test instructions file
    char testFile[70];

    // --- The maximum time limit a program is allowed to run before
    //     termination (in seconds).
    int programTimeLimit;

    // --- The penalty given if a program exceeds programTimeLimit
    int programTimePenalty;

    // --- The amount of time allowed for each line of output to appear from
    //     student programs (in milliseconds).
    int defaultTimeLimit;

    // --- The penalty given if a program produces warnings during compile.
    int compileWarnPenalty;

    // --- The penalty given if a program cannot compile.
    int compileFailPenalty;

    // --- If true, when a program crashes, it will be restarted and the
    //     current test will be resumed.  Only one resume per crash is
    //     allowed.
    bool resumeFromCrash;
};