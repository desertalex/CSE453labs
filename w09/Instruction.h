struct Instruction
/******************************************************************************
* Summary:
*    Stores the info for each of the instructions in the test instructions
*    file.  This struct is also used to store results gathered from testing a
*    program.
*******************************************************************************/
{
    Instruction(void)
    {
        wait = 0;
        points = -1;
        recv = NULL;
        send = NULL;
        start = false;
        stop = false;
        restart = false;
    }

    ~Instruction(void)
    {
        if (recv)
            delete[] recv;
        if (send)
            delete[] send;
    }

    void operator=(const Instruction &copyInst);

    // --- If greater than zero, the test program will sleep for that amount
    //     of time (in milliseconds).
    int wait;

    // --- If greater than -1, a new section in the test will be dillineated,
    // and points assigned to the new section.  If equal to 0, a new section
    // will be dillineated but no points assigned.
    int points;

    // --- If non-null, output is expected from the student's program
    char *recv;

    // --- If non-null, input will be provided to the student's program
    char *send;

    // --- If true, the student's program will be started
    bool start;

    // --- If true, the student's program is expected to self-terminate
    bool stop;

    // --- If true, the student's program was restarted due to a crash.
    bool restart;
};

void Instruction::operator=(const Instruction &copyInst)
/******************************************************************************
* Summary:
*    The copy constructor for the Instruction struct
* Inputs:
*    &copyInst - The struct to be copied
* Outputs:
*    none
*******************************************************************************/
{
    wait = copyInst.wait;
    points = copyInst.points;

    if (copyInst.recv)
    {
        if (recv)
            delete[] recv;
        if (!recv)
            recv = new char[strlen(copyInst.recv)];

        strcpy(recv, copyInst.recv);
    }

    if (copyInst.send)
    {
        if (send)
            delete[] send;
        if (!send)
            send = new char[strlen(copyInst.send)];

        strcpy(send, copyInst.send);
    }

    start = copyInst.start;
    stop = copyInst.stop;
    restart = copyInst.restart;
}

int operator==(const Instruction &leftCompare, const Instruction &rightCompare)
/******************************************************************************
* Summary:
*    Overloads the equivalence operator for Instruction structs
* Inputs:
*    &leftCompare, &rightCompare - The Instruction objects to be compared
* Outputs:
*    - True if Instructions are equal
*    - False otherwise
*******************************************************************************/
{
    if (leftCompare.wait != rightCompare.wait)
        return false;

    else if (leftCompare.points != rightCompare.points)
        return false;

    else if ((leftCompare.recv && !rightCompare.recv) ||
             (!leftCompare.recv && rightCompare.recv))
        return false;

    else if (leftCompare.recv && rightCompare.recv)
    {
        if (strcmp(leftCompare.recv, rightCompare.recv) != 0)
            return false;
    }
    else if ((leftCompare.send && !rightCompare.send) ||
             (!leftCompare.send && rightCompare.send))
        return false;

    else if (leftCompare.send && rightCompare.send)
    {
        if (strcmp(leftCompare.send, rightCompare.send) != 0)
            return false;
    }
    else if (leftCompare.start != rightCompare.start)
        return false;

    else if (leftCompare.stop != rightCompare.stop)
        return false;

    return true;
}