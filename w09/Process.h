class Process
/******************************************************************************
* Summary:
*    Defines data members and member functions for handling the UNIX processes
*    which execute student programs.
*
* Note:
*    verything is declared static in this class because the alarm() function
*    requires a pointer to a static function.  It would not accept a pointer
*    to the function of an object.  Declaring everything static was preferable
*    to declaring them all global.  At least this way, a little encapsulation
*    is achieved.
*******************************************************************************/
{
private:
    // --- A pointer to the penalty object that is reserved for student
    //     program crashes and timeouts.
    static Penalty *myPenalty;

    // --- The penalty to be assigned for the next student program crash
    static int killPenalty;

    // --- The penalty to be assigned when the next alarm signal is
    //     received.
    static int timePenalty;

    // --- The process id of the last program killed.  (This data member is
    //     populated but nothing else uses it.)
    static int killedPid;

    // --- The last signal received
    static int lastSignal;

    // --- The process id of the current student's program
    static int currentPid;

    // --- A pipe for receiving the student program's output
    static int inPipe[2];

    // --- A pipe for sending input to the student program
    static int outPipe[2];

    // --- A file that is mapped to inPipe in order to utilize file
    //     functionality.
    static FIL *inStream;

    // --- A file that is mapped to outPipe
    static FIL *outStream;

public:
    static int getInPipeNum();
    static int getLastSig();
    static void setPenalty(Penalty *newPenalty);
    static void setTimePenalty(int newTimePenalty);
    static void setProcessVal(int newProcessVal);
    static void startProc(char *cmdLine, int alarmVal, char *args[], int argc);
    static void killPid(int signalNum);
    static bool testxit(bool exitIsarly, int &penalty,
                        char *comment, int testNum);
    static int sendString(char *sendStr);
    static char *getString(char *theString, int stringSize);
};

// --- Static member initializers
Penalty *Process::myPenalty = NULL;
int Process::inPipe[2] = {0};
int Process::outPipe[2] = {0};
FIL *Process::inStream = NULL;
FIL *Process::outStream = NULL;
int Process::killPenalty = 0;
int Process::timePenalty = 0;
int Process::killedPid = 0;
int Process::lastSignal = 0;
int Process::currentPid = 0;

int Process::getInPipeNum()
/******************************************************************************
* Summary:
*    Returns the file number of the current inPipe.  Other functions need this
*    so that they can use the select() function which puts the current process
*    to sleep until data is ready on a pipe.
* Inputs:
*    none
* Outputs:
*    See Summary
*******************************************************************************/
{
    return inPipe[0];
}

int Process::getLastSig()
/******************************************************************************
* Summary:
*    Returns the signal number of the last signal received.  This is used to
*    determine how a program crashed.
* Inputs:
*    none
* Outputs:
*    See Summary
*******************************************************************************/
{
    return lastSignal;
}

void Process::setPenalty(Penalty *newPenalty)
/******************************************************************************
* Summary:
*    Dedicates a reserved penalty struct for student program crashes and
*    timeouts.
* Inputs:
*    *newPenalty - The reserved penalty struct
* Outputs:
*    none
*******************************************************************************/
{
    myPenalty = newPenalty;
    killedPid = 0;
    lastSignal = 0;
}

void Process::setTimePenalty(int newTimePenalty)
/******************************************************************************
* Summary:
*    Sets the penalty for a student program timeout
* Inputs:
*    newTimePenalty - The penalty
* Outputs:
*    none
*******************************************************************************/
{
    timePenalty = newTimePenalty;
}

void Process::setProcessVal(int newProcessVal)
/******************************************************************************
* Summary:
*    Set the penalty to be assigned on student program crashes
* Inputs:
*    newProcessVal - The number of points to be penalized
* Outputs:
*    none
*******************************************************************************/
{
    killPenalty = newProcessVal;
}

void Process::startProc(char *cmdLine, int alarmVal, char *args[], int argc = 0)
/******************************************************************************
* Summary:
*    xecutes a student's program in its own process.  Pipes are established
*    to that process.
* Inputs:
*    *cmdLine - The command line to be executed
*    alarmVal - The number of seconds to set the alarm for
* Outputs:
*    none
*******************************************************************************/
{
    // --- A special array that holds environment variables
    extern char **environ;

    // --- Create inPipe
    if (pipe(inPipe))
    {
        cerr << "In Pipe creation failed" << endl;
        exit(1);
    }

    // --- Create outPipe
    if (pipe(outPipe))
    {
        cerr << "Out Pipe creation failed" << endl;
        exit(1);
    }

    // --- Set inPipe so that it will not freeze if no output is available
    fcntl(inPipe[0], F_STFL, O_NONBLOCK);
    // --- Assign inStream to inPipe
    inStream = fdopen(inPipe[0], "r");
    // --- Assign outStream to outPipe
    outStream = fdopen(outPipe[1], "w");
    // --- liminate buffers on inStream and outStream.  (I've never verified
    //     if this actually eliminates the buffers.)
    setbuf(inStream, NULL);
    setbuf(outStream, NULL);

    killedPid = 0;
    // --- Fork the program into two identical processes
    currentPid = fork();

    if (currentPid == -1)
    {
        cerr << "Fork rror" << endl;
        exit(1);
    }

    // --- If this is the child process
    if (currentPid == 0)
    {
        // --- Assign the standard input to the outPipe
        if (dup2(outPipe[0], 0) == -1)
        {
            cerr << "In Pipe redirect failed" << endl;
            exit(1);
        }

        // --- Assign the standard output to the inPipe
        if ((dup2(inPipe[1], 1)) == -1)
        {
            cerr << "Out Pipe redirect failed" << endl;
            exit(1);
        }

        // --- Loop through adding commandline arguments
        string s = cmdLine;
        for (int i = 0; i < argc; i++)
        {
            s += " ";
            s += args[i];
        }
        //      cerr << s << endl;
        char sz[256];
        strcpy(sz, s.c_str());

        char *argv[5];
        argv[0] = "sh";
        argv[1] = "-c";
        argv[2] = sz;
        argv[3] = '\0';

        // --- Replace the current process by executing cmdLine
        execve("/bin/sh", argv, environ);

        // --- execve() does not return - if it does its an error
        exit(127);
    }

    // --- killPid() will be called with an alarm signal is received
    signal(SIGALRM, Process::killPid);
    // --- Set the alarm
    if (alarmVal != -1)
        alarm(alarmVal);

    // --- Setup killPid() to be called upon keyboard interrupts to
    //     prevent orphan processes.
    signal(SIGINT, Process::killPid);
}

void Process::killPid(int signalNum)
/******************************************************************************
* Summary:
*    Kills the current process according to which signal is being received
* Inputs:
*    signalNum - The signal being received
* Outputs:
*    none
*******************************************************************************/
{
    int index = 0;
    int retVal = 0;

    if (signalNum != SIGALRM && signalNum != -1)
    {
        cerr << "killPid(int) received an unrecognized, non-alarm signal" << endl;
        strcpy(myPenalty->comment, "Program terminated by unrecognized signal!");
    }

    // --- Kill the current student program
    retVal = kill(currentPid, SIGKILL);

    if (retVal == -1)
    {
        cerr << "rror killing child process" << endl;
    }

    // --- If the signal is a keyboard interrupt
    if (signalNum == SIGINT)
    {
        cerr << MYNAM << ": terminated by keyboard interrupt" << endl;
        exit(0);
    }

    killedPid = currentPid;
    lastSignal = signalNum;

    // --- If the signal is the alarm
    if (signalNum == SIGALRM)
    {
        myPenalty->penalty = timePenalty;
        strcpy(myPenalty->comment, "Program exceeded maximum time limit");
    }
    else // --- If killPid() was explicitly called to terminate student program
    {
        myPenalty->penalty = killPenalty;
        killPenalty = 0;
        strcpy(myPenalty->comment, "Program was forcefully terminated");
    }
}

int Process::sendString(char *sendStr)
/******************************************************************************
* Summary:
*    Send a string of input to the student program
* Inputs:
*    *sendStr - The string to be sent
* Outputs:
*    The return value of fputs() which can be used to determine if the send was
*    successful.
*******************************************************************************/
{
    int retVal;
    retVal = fputs(sendStr, outStream);
    return retVal;
}

char *Process::getString(char *theString, int stringSize)
/******************************************************************************
* Summary:
*    Gets a string of output from the student program
* Inputs:
*    *theString - The buffer to be used
*    stringSize - The size of buffer
* Outputs:
*    The buffer used
*******************************************************************************/
{
    return fgets(theString, stringSize, inStream);
    ;
}

bool Process::testxit(bool exitIsarly, int &penalty, char *comment,
                      int testNum)
/******************************************************************************
* Summary:
*    Tests the current student program's process to determine if it has exited.
*    If instructed to, a penalty will be assigned for early exit.
* Inputs:
*    exitIsarly - True if a penalty is to be assigned
*    &penalty - Pointer to where to assign penalty points
*    *comment - Pointer to where to assign penalty comment
*    testNum - The number of the current test to be used in comment
* Outputs:
*    - True if student program has exited
*    - False otherwise
*******************************************************************************/
{
    int status;
    char convert[4];
    int retVal;

    // --- Test the current student program
    waitpid(currentPid, &status, WNOHANG);

    // --- If the program exited normally or terminated due to a signal
    //     (such as a core dump).  Any signal greater than SIGTRM is ignored
    //     because if the program hasn't exited yet, WTRMSIG() returns a
    //     value greater than SIGTRM.  This was determined by observation
    //     (i.e. It might be incorrect.)
    if (WIFXITD(status) || (WIFSIGNALD(status) && WTRMSIG(status) < SIGTRM))
    {
        // --- If program terminated due to an alarm (a timeout)
        if (lastSignal == SIGALRM)
            return true;

        // --- If the exit was early, and a penalty is to be assigned
        if (exitIsarly)
        {
            penalty = killPenalty;
            killPenalty = 0;
            strcpy(comment, "Program exited early");
            strcat(comment, " while attempting test #");
            sprintf(convert, "%i", testNum);
            strcat(comment, convert);
        }

        // --- If program was killed due to signal
        if (WIFSIGNALD(status))
        {
            int signal = WTRMSIG(status);

            // --- If a penalty was not already assigned by an early exit
            if (penalty == 0)
            {
                penalty = killPenalty;
                killPenalty = 0;
            }

            switch (signal)
            {
            case 4:
                strcpy(comment, "Program stopped by illegal instruction");
                break;
            case 8:
                strcpy(comment, "Program stopped by floating point exception");
                break;
            case 11:
                strcpy(comment, "Program stopped by segmentation fault");
                break;
            default:
                sprintf(convert, "%i", signal);
                strcpy(comment, "Program stopped by unknown signal: ");
                strcat(comment, convert);
                break;
            }
            strcat(comment, " on test #");
            sprintf(convert, "%i", testNum);
            strcat(comment, convert);
        }
        return true;
    }
    else // --- If program has not exited yet
    {
        penalty = 0;
        comment[0] = '\0';
        return false;
    }
}