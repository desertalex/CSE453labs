void getResults(Assignment &myAssg, Instruction **instructs,
                Instruction **results, Penalty penalties[])
/******************************************************************************
* Summary:
*    Compiles and tests the student program.  The results are stored for later
*    processing.
* Inputs:
*    &myAssg - The current Assignment object to work with
* Outputs:
*    none
*
* Note:
*    This function compiles the source code and then tests the resultant program.
*    The code for compiling source code could easily be moved into its own
*    function to reduce the number of tasks this function performs.
*******************************************************************************/
{
    int index = 0;
    int rindex = 0;
    int retVal = 0;
    int pid = 0;
    int pointVal = 0;
    int testNum = 0;
    int penaltyNum = 1;
    int argc = 0;
    char *args[MAXARGS];
    char command[100];
    bool secondChance = false;
    fd_set mySet;
    timeval tv;

    // --- Test the first instruction for a '+' char
    if (!instructs[index]->start)
    {
        cerr << MYNAM << ": First test instruction is not '+'" << endl;
        exit(1);
    }

    // --- Compiles the student source code if necessary
    if (myAssg.compileProgram[0])
    {
        // --- Create the command line that will be executed to compile the
        //     source code.
        strcpy(command, myAssg.compileProgram);
        strcat(command, " ");
        strcat(command, myAssg.sourceFiles[myAssg.sourceIndex]);

        // --- These lines of code could be used when the time comes to enable
        //     style checking.

        //      strcat(command, " > ");
        //      strcat(command, myAssg.sourceFiles[myAssg.sourceIndex]);
        //      strcat(command, ".grade");

        // --- xecute the compile program
        retVal = system(command);

        // --- If the compile program exit status was one, the source code
        //     compiled with warnings.  If it was two, the source code could not
        //     compile.
        switch (retVal >> 8)
        {
        case 1:
            penalties[penaltyNum].penalty = myAssg.compileWarnPenalty;
            strcpy(penalties[penaltyNum++].comment,
                   "Source compiled with warnings");
            break;
        case 2:
            penalties[penaltyNum].penalty = myAssg.compileFailPenalty;
            strcpy(penalties[penaltyNum++].comment, "Source could not compile");
            return;
        }

        // --- Setup the next command to execute the compiled program

        //Java files are interpereted and not compiled into a stand-alone
        //executable, therefore, the command line must be built differently
        if (strstr(myAssg.sourceFiles[myAssg.sourceIndex], ".java") != NULL)
        {
            strcpy(command, "java ");
            strncat(command, myAssg.sourceFiles[myAssg.sourceIndex], strlen(myAssg.sourceFiles[myAssg.sourceIndex]) - 5);
        }
        else
        {
            strcpy(command, myAssg.sourceFiles[myAssg.sourceIndex]);
            strcat(command, XSUFFIX);
        }
    }
    else // --- If no compile was necessary
         // --- Setup the next command to execute the source code file
        strcpy(command, myAssg.sourceFiles[myAssg.sourceIndex]);

    //parse and setup arguments
    if (MAXARGS) // make sure MAXARGS is not 0
    {
        char *ptr = strtok(instructs[index]->send, " ");
        //split on spaces and put chunks into args array
        while (ptr != NULL && argc < MAXARGS)
        {
            args[argc] = ptr;
            argc++;
            ptr = strtok(NULL, " ");
        }
    }

    // --- Initilaize the first object in the results array
    if (!results[rindex])
        results[rindex] = new Instruction;

    // --- Start the student program and set the time limit for completion
    Process::startProc(command, myAssg.programTimeLimit, args, argc);
    *results[rindex++] = *instructs[index++];

    // --- Loop through each test instruction
    while (instructs[index] != NULL)
    {
        // --- Sleep for the specified number of milliseconds before each test
        //     instruction.
        tv.tv_sec = 0;
        tv.tv_usec = DFAULTSLP * 1000;
        retVal = select(0, NULL, NULL, NULL, &tv);

        if (!results[rindex])
            results[rindex] = new Instruction;

        // --- If the program is to start again - do so without resetting the
        //     current time limit.
        if (instructs[index]->start)
        {
            Process::startProc(command, -1, args, argc);
            *results[rindex++] = *instructs[index++];
        }
        // --- If a new point value is to be assigned to future instructions
        else if (instructs[index]->points != -1)
        {
            pointVal = instructs[index]->points;
            Process::setProcessVal(pointVal);
            *results[rindex++] = *instructs[index++];
        }
        // --- If testing should pause to allow the student program to work
        else if (instructs[index]->wait > 0)
        {
            tv.tv_sec = 0;
            tv.tv_usec = instructs[index]->wait * 1000;
            retVal = select(0, NULL, NULL, NULL, &tv);
            *results[rindex++] = *instructs[index++];
        }
        // --- If text should be sent to the student program
        else if (instructs[index]->send)
        {
            Process::sendString(instructs[index]->send);
            *results[rindex++] = *instructs[index++];
        }
        // --- If the student program is expected to stop
        else if (instructs[index]->stop)
        {
            bool didxit;
            char *retChar;
            char inLine[256];

            // --- Test to see if there is any output from student program
            //     remaining.  This would indicate whether the student program
            //     terminated exactly when expected.
            retChar = Process::getString(inLine, 256);

            // --- If there was output available from student program
            if (retChar)
                didxit = false;
            else // --- If there was no ouput available
                 // --- Test to see if the program has really exited
                didxit = Process::testxit(false, penalties[penaltyNum].penalty,
                                          penalties[penaltyNum].comment, testNum);

            if (didxit)
            {
                // --- If the program  gracefully exited (i.e. no core dump)
                if (penalties[penaltyNum].penalty == 0 &&
                    !penalties[penaltyNum].comment[0])

                    *results[rindex] = *instructs[index];
                else
                    // --- I can't remember why this instruction is here
                    penaltyNum++;

                rindex++;
                index++;

                // --- If no instructions remain to be processed
                if (!instructs[index] || !instructs[index]->start)
                    return;
            }
            else // --- If the student program is still running
            {
                char convert[4];

                // --- Assign a penalty and eliminate future penalties for this
                //     specific test.
                penalties[penaltyNum].penalty = pointVal;
                pointVal = 0;
                strcpy(penalties[penaltyNum].comment,
                       "Program failed to exit when expected on test #");
                sprintf(convert, "%i", testNum);
                strcat(penalties[penaltyNum++].comment, convert);
                results[rindex++]->stop = false;

                // --- Test the program one more time
                didxit = Process::testxit(false, penalties[penaltyNum].penalty,
                                          penalties[penaltyNum].comment, testNum);
                if (!didxit)
                    Process::killPid(-1);

                return;
            }
        }
        // --- If output is expected from student program
        else if (instructs[index]->recv)
        {
            char *retChar;
            char inLine[256];
            bool didxit = false;
            int inPipe = Process::getInPipeNum();

            FD_ZRO(&mySet);
            FD_ST(inPipe, &mySet);

            // --- Sleep for the specified number of milliseconds
            tv.tv_sec = 0;
            tv.tv_usec = myAssg.defaultTimeLimit * 1000;
            select(inPipe + 1, &mySet, NULL, NULL, &tv);

            // --- Get output from student program
            retChar = Process::getString(inLine, 256);

            // --- If output was available
            if (retChar)
            {
                char *newLine;

                //This is to take care of a bug with Java.
                //Its ugly but this had to be done
                //when println is used in Java it prints the line
                //and then the \n. The \n would be taken on its own and below
                //it is removed. The output would then be interpereted as ""
                //and be incorrect of course.
                //This way the \n is replaced by "\n" and can be included in the
                //Instruction file.
                if ((int)inLine[0] == 10)
                    strcpy(inLine, "\\n");

                // --- If this is the instruction after points were assigned, and
                // we're not currently resuming from a crash then increment the
                // test number.  The test number is primarily used for outputting
                // the test results.
                if (!secondChance && instructs[index - 1]->points != -1)
                    testNum++;

                // --- Get rid of the trailing newline char if it exists
                newLine = strchr(inLine, '\n');
                if (newLine)
                    *newLine = '\0';

                // --- Copy the output into the current result struct
                results[rindex]->recv = new char[strlen(inLine) + 1];
                strcpy(results[rindex]->recv, inLine);

                // --- If the output does not match what was expected, assign a
                //     penalty.
                if (strcmp(instructs[index]->recv, results[rindex]->recv) != 0)
                {
                    char convert[4];

                    penalties[penaltyNum].penalty = pointVal;
                    pointVal = 0;
                    strcpy(penalties[penaltyNum].comment,
                           "Incorrect output on test #");
                    sprintf(convert, "%i", testNum);
                    strcat(penalties[penaltyNum++].comment, convert);
                }
                rindex++;
                index++;
            }
            else // --- If no output was available
            {
                // --- Test to see if the student program exited
                didxit = Process::testxit(true, penalties[penaltyNum].penalty,
                                          penalties[penaltyNum].comment, testNum);

                // --- If the student program terminated due to exceeding time
                //     limit.
                if (Process::getLastSig() == SIGALRM)
                    return;

                // --- If student program has not exited, assign a penalty
                if (!didxit)
                {
                    char convert[4];

                    penalties[penaltyNum].penalty = pointVal;
                    pointVal = 0;
                    strcpy(penalties[penaltyNum].comment,
                           "Program failed to output when expected on test #");
                    sprintf(convert, "%i", testNum);
                    strcat(penalties[penaltyNum].comment, convert);

                    Process::killPid(-1);
                }

                penaltyNum++;

                // --- If this test was a resumption from a crash
                if (secondChance)
                {
                    char convert[4];

                    strcpy(penalties[penaltyNum].comment,
                           "Program failed twice on test #");
                    sprintf(convert, "%i", testNum);
                    strcat(penalties[penaltyNum++].comment, convert);

                    results[rindex++]->restart = true;
                    index++;
                    secondChance = false;

                    // --- Fast forward instructions until the next test is found
                    while (instructs[index] &&
                           instructs[index]->points == -1 &&
                           !instructs[index - 1]->start)
                        index++;

                    // --- If the failed test was not the last test available
                    if (instructs[index])
                        Process::startProc(command, -1, args, argc);
                    else
                        return;
                }
                // --- If student programs are allowed to resume from crashes
                else if (myAssg.resumeFromCrash)
                {
                    // --- Rewind the instructions until the beginning of the
                    //     current test is found.
                    while (index > 0 &&
                           instructs[index - 1]->points == -1 &&
                           !instructs[index - 1]->start)
                        index--;

                    // --- If the test was found
                    if (instructs[index])
                        Process::startProc(command, -1, args, argc);
                    else
                        return;

                    secondChance = true;
                    results[rindex++]->restart = true;
                }
                else
                    return;
            }
        }
    }
}