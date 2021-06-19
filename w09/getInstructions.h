void getInstructions(Assignment &myAssg, Instruction **instructs)
/******************************************************************************
* Summary:
*    Reads in the instructions from the test instruction file into **intructs
* Inputs:
*    &myAssg - The current assignment file.
*    **instructs - The array of pointers to instruction structs
* Outputs:
*    none
*******************************************************************************/
{
    ifstream testFile;
    ofstream inputFile;
    char inLine[256];
    int lineCount = 0;
    int instructIndex = 0;

    // --- Reset the pointers in the array
    for (int i = 0; i < MAXINSTRUCTS; i++)
        instructs[i] = NULL;

    testFile.open(myAssg.testFile);

    // This "if statement" added by Conrad Kennington: Feb 1, 2005.
    // The testFile.bad() below checks stream corruption,
    // it was hanging if the testfile didn't exist.
    if (!testFile)
    {
        cerr << "rror: Test instructions file could not be found/opened." << endl
             << endl;
        exit(1);
    }

    if (testFile.bad() || !strlen(myAssg.testFile))
    {
        cerr << MYNAM << ": Test instructions file could not be opened" << endl;
        cerr << "\tPlease specify one (e.g., \"" << MYNAM
             << " cs235/hw01 -myTest.test myCode.cpp\" or " << endl
             << "\t\"" << MYNAM << " cs165/hw16 myCode.cpp\")" << endl;
        exit(1);
    }

    // --- Loop through each line of the instruction file
    while (!testFile.eof())
    {
        char *ptr;

        testFile.getline(inLine, sizeof(inLine));

        // --- A '+' char means start the program
        // anything following the '+' is used as arguments to the program
        if (inLine[0] == '+')
        {
            instructs[instructIndex] = new Instruction;
            instructs[instructIndex]->start = true;
            if (*(ptr = inLine + 1))
            {
                instructs[instructIndex]->send = strcpy(new char[strlen(ptr) + 2],
                                                        ptr);
                strcat(instructs[instructIndex]->send, "\n");
            }
            instructIndex++;
        }
        // --- A '$' char deliniates a new test.  Digits following '$' on the
        //     same line are the number of points assigned to that test.
        else if (inLine[0] == '$')
        {
            instructs[instructIndex] = new Instruction;
            if (*(ptr = inLine + 1))
                if (atoi(ptr) == 0)
                    instructs[instructIndex]->points = -1;
                else
                    instructs[instructIndex]->points = atoi(ptr);
            else
                instructs[instructIndex]->points = 0;

            instructIndex++;
        }
        // --- A '~' char followed by digits means to wait for that number of
        //     milliseconds.
        else if (inLine[0] == '~')
        {
            instructs[instructIndex] = new Instruction;
            if (*(ptr = inLine + 1))
                instructs[instructIndex]->wait = atoi(ptr);
            instructIndex++;
        }
        // --- A '=' char followed by text means that the text is the expected
        //     output.
        else if (inLine[0] == '=')
        {
            instructs[instructIndex] = new Instruction;
            if (*(ptr = inLine + 1))
                instructs[instructIndex]->recv = strcpy(new char[strlen(ptr) + 1],
                                                        ptr);
            instructIndex++;
        }
        // --- A '<' char followed by text means that the text is given to the
        //     program as input.
        else if (inLine[0] == '<')
        {
            instructs[instructIndex] = new Instruction;
            if (*(ptr = inLine + 1))
            {
                instructs[instructIndex]->send = strcpy(new char[strlen(ptr) + 2],
                                                        ptr);
                strcat(instructs[instructIndex]->send, "\n");
            }
            instructIndex++;
        }
        // --- A '-' char means that the program is expected to terminate
        else if (inLine[0] == '-')
        {
            instructs[instructIndex] = new Instruction;
            instructs[instructIndex]->stop = true;
            instructIndex++;
        }
    }

    testFile.close();
}