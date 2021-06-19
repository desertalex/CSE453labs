void writeResults(Assignment &myAssg, Instruction **instructs,
                  Instruction **results, Penalty penalties[])
/******************************************************************************
* Summary:
*    Outputs the results of testing the student program to the screen and to
*    various files.
* Inputs:
*    &myAssg - The current assignment definition struct
*    **instructs - The instructions that were used to test the student program
*    **results - The results obtained from testing the student program
*    penalties[] - The penalties that were assigned during testing
* Outputs:
*    none
*******************************************************************************/
{
    int index;
    int rindex = 0;
    int testNum = 0;
    bool secondChance = false;
    char fileName[100];
    time_t timeStamp;
    char *timeLabel;
    ofstream outFile;
    ofstream gradeFile;
    ofstream logFile;

    // --- If the student program was tested in grade mode (grading
    //     files are output.)
    if (myAssg.gradeMode)
    {
        strcpy(fileName, myAssg.sourceFiles[myAssg.sourceIndex]);
        strcat(fileName, LOGSUFFIX);
        // --- Open the file for student program testing output
        outFile.open(fileName, ios::out);
        // --- Open the file for the whole class's testing output
        logFile.open(CLASSLOGFIL, ios::out | ios::app);
    }

    // --- Output the date and time
    timeStamp = time(NULL);
    timeLabel = ctime(&timeStamp);
    timeLabel[strlen(timeLabel) - 1] = '\0';
    cout << BOLD << UL << "\n\n"
         << myAssg.sourceFiles[myAssg.sourceIndex]
         << "  --  ";
    cout << timeLabel << RST << endl;
    outFile << BOLD << UL << "\n\n"
            << myAssg.sourceFiles[myAssg.sourceIndex]
            << "  --  ";
    outFile << timeLabel << RST << endl;
    logFile << BOLD << UL << "\n\n"
            << myAssg.sourceFiles[myAssg.sourceIndex]
            << "  --  ";
    logFile << timeLabel << RST << endl;

    // --- Loop through as many instructions or results as possible
    index = 0;
    while (instructs[index] && results[rindex])
    {
        // --- If the student program was restarted (due to a crash)
        if (results[rindex]->restart)
        {
            // --- If the program crashed even after it was restarted
            if (secondChance)
            {
                // --- Fast-forward the instructions to the next test
                while (instructs[index] &&
                       instructs[index]->points == -1 &&
                       !instructs[index - 1]->start)
                    index++;

                secondChance = false;

                // --- If there are no more instructions left
                if (!instructs[index])
                    break;
            }
            else // --- If this is the first time the program crashed
            {
                // --- Rewind the instructions and restart the current test
                while (index > 0 &&
                       instructs[index - 1]->points == -1 &&
                       !instructs[index - 1]->start)
                    index--;

                secondChance = true;
            }

            cout << BOLD << endl
                 << "<Program Restarted>" << RST;
            outFile << BOLD << endl
                    << "<Program Restarted>" << RST;
            logFile << BOLD << endl
                    << "<Program Restarted>" << RST;

            // --- This is here to compensate for the index being incremented at
            //     the bottom of the loop.
            index--;
        }
        // --- If the program was expected to start executing
        else if (instructs[index]->start)
        {
            cout << endl
                 << BOLD << "<Program Started>" << RST;
            outFile << endl
                    << BOLD << "<Program Started>" << RST;
            logFile << endl
                    << BOLD << "<Program Started>" << RST;
        }
        // --- If the program was expected to stop executing
        else if (instructs[index]->stop)
            // --- If the program actually stopped
            if (results[rindex]->stop)
            {
                cout << endl
                     << BOLD << "<Program nded>" << RST << endl;
                outFile << endl
                        << BOLD << "<Program nded>" << RST << endl;
                logFile << endl
                        << BOLD << "<Program nded>" << RST << endl;
            }
            else // --- If the program did not stop
                break;
        // --- If the program was expected to receive input
        else if (instructs[index]->send)
            // --- If the program actually received input
            if (results[rindex]->send)
            {
                char *newLine;

                // --- If the actual input matches expected input.  This should
                // never be false.  No error checking is performed on input sent
                // to a program.
                if (strcmp(instructs[index]->send, results[rindex]->send) == 0)
                {
                    newLine = strchr(results[rindex]->send, '\n');
                    if (newLine)
                        *newLine = '\0';

                    cout << UL << results[rindex]->send << RST;
                    outFile << UL << results[rindex]->send << RST;
                    logFile << UL << results[rindex]->send << RST;
                }
            }
            // --- If the program did not accept input.  This should never be
            // true.  No error checking is performed on input sent to a
            // program.
            else
            {
                cout << endl
                     << RVRS << "Could not send: "
                     << instructs[index]->send << RST << endl;
                outFile << endl
                        << RVRS << "Could not send: "
                        << instructs[index]->send << RST << endl;
                logFile << endl
                        << RVRS << "Could not send: "
                        << instructs[index]->send << RST << endl;
            }
        // --- If the program was expected to provide output
        else if (instructs[index]->recv)
            // --- If the program actually provided output
            if (results[rindex]->recv)
            {
                // --- If this is the first instruction after the beginning of a
                //     new test, increment the test number.
                if (index > 0 && instructs[index - 1]->points != -1 && !secondChance)
                    testNum++;

                //Whether or not the expected and actual outputs are equal
                bool equal = (strcmp(instructs[index]->recv,
                                     results[rindex]->recv) == 0);

                //Prints the arrow at the location of the error found
                if (!equal)
                {
                    int location = 6 +
                                   //Where the error is.
                                   findDifference(results[rindex]->recv, instructs[index]->recv);

                    cout << endl
                         << setw(location) << " "
                         << "v";
                    outFile << endl
                            << setw(location) << " "
                            << "v";
                    logFile << endl
                            << setw(location) << " "
                            << "v";
                }

                cout << endl
                     << BOLD << setw(3) << testNum << "> " << RST
                     << "|" << results[rindex]->recv << "|" << flush;
                outFile << endl
                        << BOLD << testNum << setw(3) << "> " << RST
                        << "|" << results[rindex]->recv << "|" << flush;
                logFile << endl
                        << BOLD << testNum << setw(3) << "> " << RST
                        << "|" << results[rindex]->recv << "|" << flush;

                // --- If the actual output did not match the expected output
                if (!equal)
                {

                    cout << endl
                         << RVRS << "xp: "
                         << "|" << instructs[index]->recv << "|" << RST;
                    outFile << endl
                            << RVRS << "xp: "
                            << "|" << instructs[index]->recv << "|" << RST;
                    logFile << endl
                            << RVRS << "xp: "
                            << "|" << instructs[index]->recv << "|" << RST;
                }
            }

        // --- This is the bottom of the while loop
        index++;
        rindex++;
    }

    // --- If either the instruction or the result array was not completely
    //     used, the program must have terminated early.
    if (instructs[index] || results[rindex])
    {
        cout << BOLD << endl
             << "<Program Terminated>" << RST << endl;
        outFile << BOLD << endl
                << "<Program Terminated>" << RST << endl;
        logFile << BOLD << endl
                << "<Program Terminated>" << RST << endl;
    }

    // --- Output a list of all penalties accumulated during testing
    cout << '\n'
         << UL << "RROR SUMMARY:" << RST << '\n'
         << endl;
    outFile << '\n'
            << UL << "RROR SUMMARY:" << RST << '\n'
            << endl;
    logFile << '\n'
            << UL << "RROR SUMMARY:" << RST << '\n'
            << endl;

    // --- If the testing occured in grade mode then output the penalties to
    //     their own files for easy retrieval by a teacher.
    if (myAssg.gradeMode)
    {
        strcpy(fileName, myAssg.sourceFiles[myAssg.sourceIndex]);
        strcat(fileName, GRADSUFFIX);
        gradeFile.open(fileName, ios::out | ios::app);
    }

    // --- The first penalty in the array is reserved for use by the Process
    //     class.  There may or may not be such a penalty.
    if (penalties[0].comment[0])
        index = 0;
    else
        index = 1;

    // --- Loop through every penalty in the array and output them.
    while (penalties[index].comment[0])
    {
        cout << penalties[index].comment << endl;
        gradeFile << penalties[index].penalty << '\t'
                  << penalties[index].comment << endl;
        outFile << penalties[index].penalty << '\t'
                << penalties[index].comment << endl;
        logFile << penalties[index].comment << endl;
        index++;
    }

    // --- If the first to penalties in the array are empty, the program ran
    //     flawlessly (without any penalties.)
    if (!penalties[0].comment[0] && !penalties[1].comment[0])
    {
        cout << "No rrors" << endl;
        outFile << "No rrors" << endl;
        logFile << "No rrors" << endl;
    }

    cout << endl;
    outFile << endl;
    logFile << endl;

    gradeFile.close();
    outFile.close();
    logFile.close();
}