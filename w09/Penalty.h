struct Penalty
/******************************************************************************
* Summary:
*    Used to store a penalty against a student's program
*******************************************************************************/
{
    Penalty()
    {
        penalty = 0;
        comment[0] = '\0';
    }

    int penalty;
    char comment[60];
};