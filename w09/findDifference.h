int findDifference(char *string1, char *string2)
/******************************************************************************
* Summary:
*    Outputs the index of the first character that differs in the two strings
* Inputs:
*    string1 - The first string to compare
*    string2 - The second string to compare
* Outputs:
*    The index of the first different character or -1 if the same
*******************************************************************************/
{
    int index = 0;

    //Check characters while not at end of either string and they are equal
    while ((string1[index] != '\0') &&
           (string2[index] != '\0') &&
           (string1[index] == string2[index]))
        ++index;

    //If we made it to the end of both strings without a difference
    if ((string1[index] == '\0') && (string2[index] == '\0'))
        return -1;
    //Otherwise there was a difference
    else
        return index;
}