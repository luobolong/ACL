#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <regex>
#include <sstream>
#include <map>
#include <stdlib.h> //for fun bash command.
using namespace std;

bool processTestCase(string testname);
int checkSetup(vector<string> instructions);
int checkCommand(string username);
int checkPassword(string password);
void login(string username, string password);
void logout();
bool createUser(string username, string password);
bool createGroup(string groupname);
bool isAdmin(string username);
bool isOwner(string filename, string username);
bool addToGroup(string username, string groupname);
bool groupExist(string groupname);
bool userExist(string username);
bool fileExist(string filename);
void createFile(string filename);
bool strncasecmp(string s1, string s2);
vector<string> getUserGroup(string username);
bool canContinue(string answer);
bool contains(string s1, string s2);
void setUAC(string username, string permissions);
string getUAC(string username);
string getUACString(string username);
void setPermissions(string filename);
void writeFile(string filename, string text);
void readFile(string filename);
void executeFile(string filename);
bool canWrite(string userPermissions);
bool canRead(string userPermissions);
bool canExecute(string userPermissions);
string getPermissions(string filename, string username);
void editPermissions(string filename, string newPermissions, string uORg);
void replacePermissions(string filename, string newPermissions, string uORg);
void denyPermissions(string filename, string newPermissions);
void log(string text);
void programExecute(string filename);
void programCreate(string filename);
void programRead(string filename);
void programWrite(string filename, string text);
bool isRestrictedName(string filename);
