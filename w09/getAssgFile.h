void getAssgFile(Assignment &myAssg)
/******************************************************************************
* Summary:
*    Reads in the assignment file and stores its information in the myAssg struct
* Inputs:
*    &myAssg - The struct to store info in
* Outputs:
*    none
*******************************************************************************/
{
    ifstream assgFile;
    char inLine[256];
    int lineCount = 1;

    // --- Open the assignment file
    assgFile.open(myAssg.assgFile);

    if (assgFile.bad())
    {
        cerr << MYNAM << ": Assignment definition file could not be opened"
             << endl;
        cerr << "\tMake sure you specified the course name and "
             << "assignment correctly" << endl;
        exit(1);
    }

    // --- Loop through each line of the assignment file

    //TMPTRV
    //   while (! assgFile.eof())

    while (assgFile.peek() != OF)
    {
        char *tempPtr;
        char *tempPtr2;

        assgFile.getline(inLine, sizeof(inLine));

        tempPtr2 = inLine;
        // --- Look for the '=' char as the variable/data delimeter
        tempPtr = strsep(&tempPtr2, "=");
        // --- If no '=' char was found
        if (!tempPtr)
        {
            cerr << MYNAM << ": In assg file - tag expected on line " << lineCount
                 << endl;
            exit(1);
        }

        // --- Look for the ';' char which marks then end of each line
        tempPtr = strsep(&tempPtr2, ";");
        // --- If no ';' char was found
        if (!tempPtr)
        {
            cerr << MYNAM << ": In assg file - Semicolon missing on line "
                 << lineCount << endl;
            exit(1);
        }

        // --- The program to be called that will check the style of source
        //     code files.  * This functionality is currently not implemented *
        if (!strcmp(inLine, "StyleCheckProgram"))
        {
            strcpy(myAssg.styleProgram, myAssg.assgDir);
            strcat(myAssg.styleProgram, tempPtr);
        }
        // --- The program to be executed that compiles each source code file
        //     in preparation for testing.
        else if (!strcmp(inLine, "CompileProgram"))
        {
            if (tempPtr[0])
            {
                //This line commented out to improve path problems.
                //strcpy(myAssg.compileProgram, myAssg.assgDir);
                strcat(myAssg.compileProgram, tempPtr);
            }
        }
        // --- The test instructions file
        else if (!strcmp(inLine, "TestInstructions"))
        {
            // --- If no test instructions file is specified then the
            //     student is expected to proved his/her own.
            if (!myAssg.testFile[0] && tempPtr[0])
            {
                strcpy(myAssg.testFile, myAssg.assgDir);
                strcat(myAssg.testFile, tempPtr);
            }
        }
        // --- The penalty to be assigned when a source code file compiles with
        //     warnings.
        else if (!strcmp(inLine, "CompilerWarningPenalty"))
            myAssg.compileWarnPenalty = atoi(tempPtr);
        // --- The penalty to be assigned when a source code fails to compile
        else if (!strcmp(inLine, "CompilerFailPenalty"))
            myAssg.compileFailPenalty = atoi(tempPtr);
        // --- The overall time limit given for each test to complete
        else if (!strcmp(inLine, "OverallTimeLimit"))
            myAssg.programTimeLimit = atoi(tempPtr);
        // --- The penalty assigned if a test exceeds the overall time limit
        else if (!strcmp(inLine, "OverallTimePenalty"))
            myAssg.programTimePenalty = atoi(tempPtr);
        // --- The default amount of time to wait for each line of output from
        //     the program being tested.
        else if (!strcmp(inLine, "DefaultLineOutputTimeLimit"))
            myAssg.defaultTimeLimit = atoi(tempPtr);
        // --- Specifies whether a program should be restarted and the current
        //     test resumed upon a program crash.
        else if (!strcmp(inLine, "ResumeFromCrash"))
        {
            if (tempPtr[0] == 'T')
                myAssg.resumeFromCrash = true;
            else
                myAssg.resumeFromCrash = false;
        }
        else
            cerr << MYNAM << ": Unknown tag in assignment file on line "
                 << lineCount << endl;

        lineCount++;
    }

    assgFile.close();
}