#include <string>
#include <iostream>
using namespace std;

/*******************************
* genQuery
* Function takes a username and password both as strings and 
* combines them and returns a SQL query
*******************************/
string genQuery(string username, string password)
{
    string sqlOutput = "SELECT id FROM userAuth WHERE name='" + username + "' and password='" + password + "';";
    return sqlOutput;
}

/*******************************
* main
* Asks the user for username as password
* Calls genQuery() and prints result
*******************************/
int main()
{
    string username;
    string password;

    cout << "Enter username here: ";
    cin >> username;
    cout << "Enter password here: ";
    cin >> password;
    string sqlOutput = genQuery(username, password);
    cout << sqlOutput << endl;

    return 0;
}