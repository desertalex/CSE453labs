#include <string>
#include <iostream>
using namespace std;

string genQuery(string username, string password)
{
    string sqlOutput = "SELECT id FROM userAuth WHERE name='" + username + "' and password='" + password + "';";
    return sqlOutput;
}

/*******************************************************
 * weakMitigation
 * A function to check a string for bad characters.
 * It returns true if bad characters are present.
 * *****************************************************/
bool weakMitigation(string username, string password)
{
    //black list
    char badCharacters[] = "=-;'‘* ";
    //iterate through username
    for (int i = 0; i < username.length(); i++)
    {
        //iterate through black list for each character in username
        for (int j = 0; j < 4; j++)
        {
            if (username[i] == badCharacters[j])
            {
                return true;
            }
        }
    }
    for (int i = 0; i < password.length(); i++)
    {
        //iterate through black list for each character in password
        for (int j = 0; j < 4; j++)
        {
            if (password[i] == badCharacters[j])
            {
                return true;
            }
        }
    }
    return false;
}

int main()
{
    string username;
    string password;

    cout << "Enter username here: ";
    cin >> username;
    cout << "Enter password here: ";
    cin >> password;
    if (weakMitigation(username, password))
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