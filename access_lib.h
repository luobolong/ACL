#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <regex>
#include <sstream>
#include <map>
#include <stdlib.h>

using namespace std;

string arg1, arg2, arg3, arg4; // 命令参数
string u, p, g; // 用户名 密码 组名
fstream myAudit; // 日志文件 记录系统中各种动作和事件
string whosLoggedIn; // 当前已登录用户
vector<string> instructions; // 指令数组
map<string, vector<string> > permissions; // 文件-用户权限字典
map<string, vector<string> > usergroups; // 用户-用户组字典
map<string, string> UAC; // 用户-UAC状态字典
map<string, string> userMap; // 用户-密码字典
bool isLoggedIn = false; // 是否有用户登录
bool wrongUsername, wrongPassword;
bool isFirstRun = true;

bool processTestCase(string testname);
int checkSetup(vector<string> instructions);
int checkCommand(string username);
int checkPassword(string password);
void login(string username, string password);
void logout();
bool groupExists(string groupname);
bool userExists(string username);
bool createUser(string username, string password);
bool createGroup(string groupname);
bool deleteUser(string username);
bool deleteGroup(string groupname);
vector<string> getUserGroup(string username);
bool isAdmin(string username);
bool isOwner(string filename, string username);
bool addToGroup(string username, string groupname);
bool fileExist(string filename);
void createFile(string filename);
bool strncasecmp(string s1, string s2);
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
void editPermissions(string filename, string newPermissions);
void replacePermissions(string filename, string newPermissions);
void denyPermissions(string filename, string newPermissions);
void log(string text);
void programExecute(string filename);
bool isRestrictedName(string filename);
