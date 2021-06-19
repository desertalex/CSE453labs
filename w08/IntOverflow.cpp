#include <iostream>
using namespace std;

/**********************************************
 * intVulnerability : Adds two numbers of type short.
 **********************************************/
short intVulnerability(short num1, short num2)
{
    // A short has 16 bits, the first one representing the sign (positive or negative).
    short answer = num1 + num2;
    return answer;
}

/**********************************************
 * intVulnerability : Adds two numbers of type short.
 **********************************************/
void intWorking()
{
    short num1 = 32766;
    short num2 = 1;
    cout << num1 << " + " << num2 << " = " << intVulnerability(num1, num2) << endl;
    // Expected: 32767
    // Actual:   32767
}

/**********************************************
 * intVulnerability : Adds two numbers of type short.
 **********************************************/
void intExploit()
{
    short num1 = 32766;
    short num2 = 2;
    cout << num1 << " + " << num2 << " = " << intVulnerability(num1, num2) << endl;
    // Exected: 32768
    // Actual  -32768
}

/**********************************************
 * MAIN : The top of the callstack.
 **********************************************/
int main()
{
    intWorking();
    intExploit();
    return 0;
}