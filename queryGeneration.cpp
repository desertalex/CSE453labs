#include <string>
#include <iostream>
using namespace std;

string genQuery(string username, string password)
{
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
    string sqlOutput = genQuery(username, password);
    cout << sqlOutput << endl;
    return 0;
}