void getCommandLine(int argc, char *argv[], Assignment &myAssg)
/******************************************************************************
* Summary:
*    Process the user-provided command line
* Inputs:
*    argc - The lengthof argv[]
*    *argv[] - An array of command line arguments
*    &myAssg - The current Assignment object to work with
* Outputs:
*    none
*******************************************************************************/
{
    int argCount = 2;

    // --- Any help instructions should be placed here
    if (argc >= 2 && !strcmp(argv[1], "-help"))
    {
        cout << "These are HLP instructions" << endl;
        exit(1);
    }
    // --- This enables grading mode.  Grading mode creates extra files to be
    //     used for grading purposes.
    else if (argc >= 2 && !strcmp(argv[1], "-grade"))
    {
        myAssg.gradeMode = true;
        argCount++;
    }
    else
        myAssg.gradeMode = false;

    // --- If there are more command line arguments to be processed
    if (argc >= argCount)
    {
        char *tempLoc;

        // --- This reads in the assignment directory
        strcpy(myAssg.assgDir, ASSGHOM);
        strcat(myAssg.assgDir, argv[argCount - 1]);
        strcat(myAssg.assgDir, "/");

        // --- This sets the path and filename of the assignment file
        tempLoc = strstr(argv[argCount - 1], "/");
        if (tempLoc)
        {
            strcpy(myAssg.assgFile, myAssg.assgDir);
            strcat(myAssg.assgFile, tempLoc + 1);
            strcat(myAssg.assgFile, ".assg");
        }
        else
        {
            cerr << MYNAM << ": Invalid assignment name" << endl;
        }
    }
    else // --- If there were no more items to be read from command line
    {
        cerr << MYNAM << ": Invalid command line " << endl;
        exit(1);
    }

    argCount++;

    // --- If a custom test file was specified on the command line
    if (argc >= argCount && argv[argCount - 1][0] == '-')
    {
        strcpy(myAssg.testFile, (argv[argCount - 1] + 1));
        // --- Append a suffix to the test file name if necessary
        if (!strstr(myAssg.testFile, ".test"))
            strcat(myAssg.testFile, ".test");
        argCount++;
    }
    else
        myAssg.testFile[0] = '\0';

    // --- Determine the number of source files specified on the command line
    myAssg.sourceCount = argc - argCount + 1;
    myAssg.sourceFiles = new char *[myAssg.sourceCount];

    // --- If there are source files specified to be processed
    if (argc >= argCount)
        // --- Loop through each source file name on command line
        for (int counter = 0; counter + argCount <= argc; counter++)
        {
            myAssg.sourceFiles[counter] = new char[strlen(argv[counter + argCount - 1]) + 1];
            // --- Copy each source file name to the myAssg struct
            strcpy(myAssg.sourceFiles[counter], argv[counter + argCount - 1]);
        }
    else // --- If there are no source files specified
    {
        cerr << MYNAM << ": Invalid source file list" << endl;
        exit(1);
    }

    // --- Initialize the index to the source files array for the rest of the
    //     program to use.
    myAssg.sourceIndex = 0;
}