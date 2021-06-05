#include <string>
#include <iostream>
using namespace std;

/*******************************************************
* whiteList
 * This is a function to check if a character is on the
 * approved list
 * *****************************************************/
bool isApproved(char checkChar)
{
    char goodCharacters[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890_";
        for (int l = 0; l < 63; l++)
        {
            if (checkChar == goodCharacters[l])
            {
                //as soon as it's found return true
                return true;
            }
        }
    // if it's never found, it's not approved
    return false;
}
 
/********************************************
 * strongMitigation
 * This function returns true if any unapproved 
 * characters are used
 * *********************************************/
bool strongMitigation(string username, string password)
{
    for (int m = 0; m < username.length(); m++)
    {
        if (!isApproved(username[m]))
        {
            //found an unapproved character
            return true;
        }
    }
    for (int m = 0; m < password.length(); m++)
    {
        if (!isApproved(password[m]))
        {
            //found an unapproved character
            return true;
        }
    }
    //if control reaches here, all characters are approved
    return false;
}
 
 /********************************************
 * genQuery
 * This function takes a username and password 
 * and returns the SQL query generated
 * *********************************************/
string genQuery(string username, string password) {
    string sqlOutput = "SELECT authenticate FROM passwordList WHERE name='" + username + "' and password='" + password + "';";
    return sqlOutput;
}

int main()
{
    string username;
    string password;

    cout << "Enter username here: ";
    cin >> username;
    cout << "Enter password here: ";
    cin >> password;
    if (strongMitigation(username, password))
    {
        cout << "System detected possible SQL injection" << endl;
    }
    else
    {
        string sqlOutput = genQuery(username, password);
        cout << sqlOutput << endl;
    }

    return 0;
}