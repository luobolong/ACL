#include "access_lib.h"

// 主函数，处理样例，返回是否成功
bool processTestCase(string testname) {

	whosLoggedIn = "admin"; // 管理员首先登录系统
	ifstream myInput (testname);
	string line;
	if (myInput.is_open()) {
		while (getline(myInput,line)){
			instructions.push_back(line); // 将指令集读入进vector容器
		}
		myInput.close();
	} else {
		log("Error: Cannot open file: " + testname + "\nExiting...\n");
		return false;
	}

	myAudit.open("audit.txt", ios_base::out | ios_base::in); // 检查文件是否存在然后删除它
	if (myAudit.is_open()) {
		remove("audit.txt");
	}
	myAudit.open("audit.txt", ios_base::app); // 新建audit.txt文件
	myAudit.close();

	if (checkSetup(instructions)!=0) return false; // 如果前四行不是预期中的，则退出并显示错误。

	for (size_t i = 0; i < instructions.size(); i++) {  // 遍历所有的指令
		istringstream in(instructions[i]);
		in >> arg1; // 读入第一个参数

		if (arg1 == "net") {
			in >> arg2; // 读入第二个参数（组或用户）
			if (arg2 == "group") {
				u = "";
				in >> g >> u; // 读入组名和用户名
				if(u=="") { // 如果用户名不存在，则创建一个组
					createGroup(g);
				} else { // 否则将该用户添加进组
					if (addToGroup(u, g)) {
						log("User " + u + " added to group " + g);
					}
				}
			}

			if (arg2 == "user") { // 创建用户
				in >> u >> p;
				createUser(u, p);
			}
		}

		if (arg1 == "del") {
			in >> arg2; // 读入第二个参数（组或用户）
			if (arg2 == "group") {
				in >> g;
				deleteGroup(g);
			}
			if (arg2 == "user") {
				in >> u;
				deleteUser(u);
			}
		}

		if (arg1 == "login") { // 登录
			in >> u >> p; // 得到用户名和密码
			if(isLoggedIn) { // 检查是否已经有用户登录，否则不允许登录并输出出错消息
				log("Login failed: simultaneous login not permitted");
			} else {
				login(u,p);
			}
		}
		if (arg1 == "logout") { // 退出
			logout();
		}

		if (arg1 == "create") { // 新建文件
			in >> arg2; // 导入文件名
			createFile(arg2);
		}
		if (arg1 == "write") { // 写文本
			in >> arg2; // 导入文件名
			string text;
			getline(in, text); // 得到一行要写入的文本
			text.erase(0,1); // 删除该行的第一个空格
			writeFile(arg2, text); // 写入文件
		}
		if (arg1 == "xcacls") { // 更改权限
			string filename, newPermissions;
			in >> filename >> arg2 >> newPermissions; // 导入文件名、参数和新权限
			if (isRestrictedName(filename)) {
				log("Error: Cannot change permission for " + filename + " as it is a restricted file");
			} else {
				if (arg2 == "/E") { // 编辑权限
					editPermissions(filename, newPermissions);
				} else if (arg2 == "/P") { // 替换权限
					replacePermissions(filename,newPermissions);
				} else if (arg2 == "/D") { // 拒绝权限
					denyPermissions(filename, newPermissions);
				} else {
					log("Error: Invalid argument specified");
				}
			}

		}

		if (arg1 == "read") { // 读文件
			string filename;
			in >> filename; // 导入文件名
			readFile(filename);
		}

		if (arg1 == "execute") { // 执行文件
			string filename;
			in >> filename;
			executeFile(filename);
		}

		if (arg1 == "uac") { // 改变UAC （用户帐户控制）
			in >> arg2;
			setUAC(whosLoggedIn,arg2);
		}

		if (arg1 == "program") { // 程序
			string filename;
			in >> arg2 >> filename; // 导入参数（执行、创建、读取、写入）和文件名
			if (arg2 == "execute") { // 执行
				if (getUAC(whosLoggedIn) == "Never") { // 检查UAC，如果是Never，则读取该文件
					programExecute(filename); // 读取文件
				} else { // 否则，检查下一行
					if (canContinue(instructions[i+1])) { // 判断哪个是下个指令，并执行
						programExecute(filename);
					}
				}
			}
			if (arg2 == "create") { // 创建
				if (getUAC(whosLoggedIn) == "Never") { // 检查UAC，如果是Never，则读取该文件
					createFile(filename); // 读取文件
				} else { // 否则，检查下一行
					if (canContinue(instructions[i+1])) { // 判断哪个是下个指令，并创建
						createFile(filename);
					}
				}
			}
			if (arg2 == "read") { // 读取
				if (getUAC(whosLoggedIn) == "Never") { // 检查UAC，如果是Never，则读取该文件
					readFile(filename); // 读取文件
				} else { // 否则，检查下一行
					if (canContinue(instructions[i+1])) { // 判断哪个是下个指令，并写入
						readFile(filename);
					}
				}
			}
			if (arg2 == "write") { // 写入
				string text;
				getline(in, text); // 得到一行要写入的文本
				text.erase(0,1); // 删除该行的第一个空格
				if (getUAC(whosLoggedIn) == "Never") { // 检查UAC，如果是Never，则读取该文件
					writeFile(filename,text); // 读取文件
				} else { // 否则，检查下一行
					if (canContinue(instructions[i+1])) {
						writeFile(filename,text);
					}
				}
			}
		}

		if (arg1 == "end") { // 结束，写所有文件
			fstream myOutput;
			myOutput.open("groups.txt", ios_base::out | ios_base::in);  // 打开，不会创建新文件

			if (myOutput.is_open()) {
				remove("groups.txt"); // 如果文件存在，则删除
			}
			myOutput.close(); // 关闭文件
			myOutput.open("groups.txt", ios_base::app); // 新建一个新文件

			for(auto it : usergroups) { // 遍历用户-用户组字典
				myOutput << it.first << ": "; // 保存组名数组另加一个冒号
				vector<string> itVector = it.second; // 循环用户数组
				for (size_t j = 0; j < itVector.size(); j++) {
					myOutput << itVector[j] << " "; // 保存用户名
				}
				myOutput << endl;
			}
			myOutput.close(); // 关闭文件

			myOutput.open("files.txt", ios_base::out | ios_base::in); // 检查文件是否存在然后删除它
			if (myOutput.is_open()) {
				remove("files.txt");
			}
			myOutput.close(); // 关闭文件
			myOutput.open("files.txt", ios_base::app); // 新建一个新文件
			for (auto it : permissions) { // 遍历权限字典
				myOutput << it.first << " ";
				vector<string> itVector = it.second;
				for (size_t j = 0; j < itVector.size(); j++) { // 遍历权限
					myOutput << itVector[j];
					if (j==0) { // 加左括号
						myOutput << " (";
					}
					if (j>0&&j<itVector.size()-1) { // 加逗号
						myOutput << ",";
					}
				}
				myOutput << ")"; // 加右括号
				myOutput << endl;
			}
			myOutput.close(); // 关闭文件

			myOutput.open("accounts.txt", ios_base::out | ios_base::in);  // 不创建accounts.txt文件

			if (myOutput.is_open()) {
				remove("accounts.txt"); // 如果accounts.txt存在，则删除
			}
			myOutput.close(); // 关闭文件
			myOutput.open("accounts.txt", ios_base::app); // 创建一个新文件
			for (auto map : userMap) {
				myOutput << map.first << " " << map.second << " " << getUACString(map.first) << endl;
			}
			myOutput.close(); // 关闭文件
		}

	}
	return true;
}

// 等待用户回答是否可以继续
bool canContinue(string answer) {

	answer.erase( std::remove(answer.begin(), answer.end(), '\r'), answer.end() ); // 从这一行中删除回车
	if (answer == "yes") { // 检查程序是否可以继续
		return true;
	} else {
		return false;
	}
}

// 执行程序
void programExecute(string filename) {
	if (isRestrictedName(filename)) { // 检查文件名是否是限制中的文件名
		log("Error: Cannot execute " + filename + " as it is a restricted file");
	} else {
		if (isLoggedIn) {
			if (fileExist(filename)) {
				string userPermissions = getPermissions(filename,whosLoggedIn); // 得到全部权限
				if (canExecute(userPermissions)) {
					log("Program executed " + filename);
					// 调用system()执行shell命令
					system(string("chmod +x "+filename).c_str()); // 设置文件的真实权限
					string scriptName = "./" + filename;
					system(scriptName.c_str()); // 执行脚本
				} else {
				log("program denied execute access to " + filename);
				}
			} else {
				log("Error: file " + filename + " does not exist");
			}
		} else {
			log("Error: no user logged in");
		}
	}

}

// 设置UAC权限
void setUAC(string username, string permissions) {
	if (isLoggedIn) {
		UAC[username] = permissions;
		log(getUACString(username));
	} else {
			log("Error: no user logged in");
	}
}

// 返回UAC权限
string getUAC(string username) {
	return UAC[username];
}

// 返回UAC中Never, Change, Always所代表的含义
string getUACString(string username) {
	if (getUAC(username) == "Always") {
		return "Always notify";
	} else if (getUAC(username) == "Change") {
		return "Notify only on change";
	} else {
		return "Never notify";
	}
}

// 添加新的权限
void editPermissions(string filename, string newPermissions) {
	if (isLoggedIn) {
		if (fileExist(filename)) { // 检查文件是否存在
			if (isOwner(filename,whosLoggedIn) || isAdmin(whosLoggedIn)) { // 检查用户是否是管理员或文件拥有者，否则不能改变权限
				permissions[filename].push_back(newPermissions); // 添加权限到字典中
				log("The ACL for " + filename + " appended by " + whosLoggedIn + " to include " + newPermissions);
			} else {
				log("Error with xcacls: Only file owner or member of Administrators group may run command");
			}
		} else {
			log("Error: File " + filename + " does not exist");
		}
	} else {
		log("Error: Please login to change permissions file");
	}
}

// 设置拒绝权限
void denyPermissions(string filename, string newPermissions) {
	if (isLoggedIn) {
		if (fileExist(filename)) {
			if (isOwner(filename, whosLoggedIn) || isAdmin(whosLoggedIn)) {
				permissions[filename].push_back(newPermissions+":D"); // 添加D到用户权限中

				// 这里对用户和用户组的输出不同
				// 因为用户和用户组都可以设置拒绝权限，但为了确定是那种情况
				// 我们需要检查新权限（newPermissions）的名字是一个组名还是用户名
				string temp = "user"; // 我们开始假设这是一个用户

				for (auto map : usergroups) { // 遍历用户-用户组字典
					if (!strncasecmp(map.first,newPermissions)) { // 如果组名对应usergroups中的一个组，则设置这个变量为group
						temp="group";
					}
				}
				log("ACL changed for " + filename +": " + temp + " " + newPermissions +" denied access"); // 打印输出
			} else {
				log("Error with xcacls: Only file owner or member of Administrators group may run command");
			}
		} else {
			log("Error: File " + filename + " does not exist");
		}
	} else {
		log("Error: Please login to change permissions file");
	}
}

// 替换已有的权限
void replacePermissions(string filename, string newPermissions) {
	bool found = false;
	if (isLoggedIn) {
		if (fileExist(filename)) { // 检查这个文件是否存在
			if (isOwner(filename,whosLoggedIn) || isAdmin(whosLoggedIn)) { // 检查用户是否是管理员或文件拥有者
				string tempUsername = newPermissions;
				tempUsername.resize(whosLoggedIn.length());
				for (size_t i = 1; i < permissions[filename].size(); i++) { // 遍历permission[filename]数组
					if (permissions[filename][i].substr(0,whosLoggedIn.length()) == tempUsername) { // 检查用户名是否与权限一样
						permissions[filename][i] = newPermissions; // 新权限替换
						log("User " + tempUsername + " access to file " + filename + " changed to " + newPermissions.substr(whosLoggedIn.length()+1) + " by " + whosLoggedIn);
						found=true;
					}
				}
				if(!found) {
					log("Error: Username does not exist");
				}
			} else {
				log("Error with xcacls: Only file owner or member of Administrators group may run command");
			}
		} else {
			log("Error: File " + filename + " does not exist");
		}
	} else {
		log("Error: Please login to change permissions file");
	}
}

// 通过遍历permissions字典来检查这个文件是否存在
bool fileExist(string filename) {
	return (permissions.find(filename) != permissions.end());
}

// 检查文件名是否是限制中的文件名
bool isRestrictedName(string filename) {
	vector<string> restrictedNames = {"accounts.txt", "audit.txt", "groups.txt", "files.txt"};
	for (auto it : restrictedNames) { // 循环文件名数组
    	if (!strncasecmp(it,filename)) { // 检查所有的文件名是否等于以上限制中的名称（不区分大小写）
    		return true;
    	}
	}
	return false;
}

// 新建文件
void createFile(string filename) {
	if (isRestrictedName(filename)) { // 检查文件名是否是限制中的文件名
		log("Error: Cannot use " + filename + " as filename");
	} else {
		if (isLoggedIn) { // 检查是否有用户登录
			fstream myFile;
			if (!fileExist(filename)) { // 检查文件是否在程序中存在
				// 删除文件（如果它是程序的先前执行后的遗留的文件）
				myFile.open(filename, ios_base::out | ios_base::in);  // 检查文件是否可以被打开，但不创建这个文件
				if (myFile.is_open()) { // 如果文件已存在，则删除它并新建一个文件
					remove(filename.c_str());
				}
				myFile.close();
				myFile.open(filename, ios_base::app); // 新建文件
				myFile.close(); // 关闭文件
				log("File " + filename + " with owner " + whosLoggedIn + " and default permissions created");
				setPermissions(filename); // 根据文件名设置权限
			} else {
				log("Error: file " + filename + " already exists");
			}
		} else {
			log("Error: Please login to create file");
		}
	}

}

// 根据文件名设置默认权限
void setPermissions(string filename) {
	permissions[filename].push_back(whosLoggedIn); // 在filename.txt中添加用户名
	permissions[filename].push_back(whosLoggedIn+":F"); // 添加用户权限
	permissions[filename].push_back("Administrators:F"); // 添加管理员权限

	if (isAdmin(whosLoggedIn)) { // 如果用户是管理员，则给予用户可读权限
		permissions[filename].push_back("Users:R"); // 添加可读权限
	}
}

// 根据文件名和用户名得到对应的最高权限
string getPermissions(string filename, string username) {
	vector<string> group;
	group = getUserGroup(username); // 得到用户所在的所有用户组数组
	string highestUserPermissionLevel = "";
	string highestGroupPermissionLevel = "";
	string tempUsername;
	for (size_t i = 1; i < permissions[filename].size(); i++) { // 遍历权限字典中的数组，并与当前用户比较
		tempUsername  = permissions[filename][i];
		tempUsername.resize(whosLoggedIn.length()); // 只取用户名字段
		if (tempUsername == whosLoggedIn) {
			highestUserPermissionLevel = permissions[filename][i].substr(tempUsername.length()+1); // 设置用户权限
		}
	}
	string s1,s2;

	for (size_t i = 1; i < permissions[filename].size(); i++) {
		for (size_t j = 0; j < group.size(); j++) {

			//tempUsername = permissions[filename][i];
			//tempUsername.resize(whosLoggedIn.length());
			size_t pos = permissions[filename][i].find(":");
			s1 = permissions[filename][i].substr(0,pos);
			s2 = group[j];
			if (strncasecmp(s1,s2)==0) {
				highestGroupPermissionLevel += permissions[filename][i].substr(s1.length()+1);
			}
		}
	}

	// 检查用户组或用户是否被拒绝
	if (contains(highestUserPermissionLevel,"D") || contains(highestGroupPermissionLevel,"D")) {
		return "D";
	} else { // 否则都将用户和用户组权限返回
		return highestUserPermissionLevel+highestGroupPermissionLevel;
	}

}

// 检查字符串s1是否包含字符串s2
bool contains(string s1, string s2) {
	if (s1.find(s2) != std::string::npos) {
    	return true;
	} else {
		return false;
	}
}

// 检查一个用户是否有权限写入
bool canWrite(string userPermissions) {
	//当权限是W或F时，用户可写入
	if (userPermissions.find("W") != std::string::npos) {
    	return true;
	} else if (userPermissions.find("F") != std::string::npos) {
		return true;
	} else {
		return false;
	}
}

// 写入文件
void writeFile(string filename, string text) {
	if (isRestrictedName(filename)) { // 检查文件名是否是限制中的文件名
			log("Error: Cannot write to " + filename + " as it is a restricted file");
	} else {
		if (isLoggedIn) { // 检查是否有人登录
			if (fileExist(filename)) { // 检查文件是否存在
				string userPermissions = getPermissions(filename, whosLoggedIn); // 得到用户权限
				fstream myFile;
				if (canWrite(userPermissions)) { // 检查用户是否可以写入
					myFile.open(filename, std::ios_base::app); // 追加内容到文件
					myFile << text << endl;
					myFile.close(); // 关闭文件
					log("User " + whosLoggedIn + " wrote to " + filename + ": " + text);
				} else {
					log("User " + whosLoggedIn + " denied write access to " + filename);
				}
			} else {
				log("Error: File " + filename + " does not exist");
			}
		} else {
			log("Error: no user logged in");
		}
	}
}

// 读取文件
void readFile(string filename) {
	if (isRestrictedName(filename)) { /// 检查文件名是否是限制中的文件名
		log("Error: Cannot read " + filename + " as it is restricted");
	} else {
		if (isLoggedIn) { // 检查是否有人登录
			if (fileExist(filename)) { // 检查文件是否存在
				string userPermissions = getPermissions(filename,whosLoggedIn); // 得到用户权限
				if (canRead(userPermissions)) { // 检查用户是否可以读取
					string line;
					fstream myFile;
					myFile.open(filename); // 打开文件
					log("User " + whosLoggedIn + " read " + filename + " as:");

					while (getline(myFile,line)) { // 从文件读取内容
						log(line);
					}
					myFile.close(); // 关闭文件
				} else {
					log("User " + whosLoggedIn + " denied read access to " + filename);
				}
			} else {
				log("Error: File " + filename + " does not exist");
			}
		} else {
			log("Error: no user logged in");
		}
	}
}

// 执行文件
void executeFile(string filename) {
	if (isRestrictedName(filename)) { // 检查文件名是否是限制中的文件名
		log("Error: Cannot execute " + filename + " as it is a restricted file");
	} else {
		if (isLoggedIn) {
			if (fileExist(filename)) {
				string userPermissions = getPermissions(filename,whosLoggedIn); // 得到当前用户的所有权限
				if (canExecute(userPermissions)) {
					log("File " + filename + " executed by " + whosLoggedIn);
				} else {
					log("User " + whosLoggedIn + " denied execute access to " + filename);
				}
			} else {
				log("Error: File " + filename + " does not exist");
			}
		} else {
			log("Error: no user logged in");
		}
	}
}

// 检查该用户是否有权限读取
bool canRead(string userPermissions) {
	// 如果权限包括X或F，则用户可以读取
	if (userPermissions.find("R") != std::string::npos) {
    	return true;
	} else if (userPermissions.find("F") != std::string::npos) {
		return true;
	} else {
		return false;
	}
}

// 检查该用户是否有权限执行
bool canExecute(string userPermissions) {
	// 如果权限包括X或F，则用户可以执行
	if (userPermissions.find("X") != std::string::npos) {
    	return true;
	} else if (userPermissions.find("F") != std::string::npos) {
		return true;
	} else {
		return false;
	}
}

// 添加用户到用户组中
bool addToGroup(string username, string groupname) {
	if (usergroups[groupname].empty() || isAdmin(whosLoggedIn)) { // 检查当前是否没有任何用户组，登录进的用户是否是管理员
		if (userMap.find(username) != userMap.end()) { // 检查用户是否已经存在
			if (usergroups.find(groupname) != usergroups.end()) {
				if (find(usergroups[groupname].begin(), usergroups[groupname].end(), username) != usergroups[groupname].end()) {
					log("User " + username + " is already member of " + groupname);
					return false;
				} else {
					usergroups[groupname].push_back(username); // 添加用户到用户组
					return true;
				}
			} else {
				log("Group " + groupname + " does not exist");
			}
		} else {
			log("User " + username + " does not exist");
		}
	} else {
		if (whosLoggedIn == "") {
			log("Error: no user logged in");
		} else {
			log("Error: only an Administrator may issue net group command");
		}
		return false;
	}
	return false;

}

// 新建用户组
bool createGroup(string groupname) {
	if (!usergroups.empty()) { // 检查当前是否没有任何用户组
		if (isAdmin(whosLoggedIn)) { // 检查登录进的用户是否是管理员
			if (usergroups.find(groupname) == usergroups.end()) { // 检查用户组是否已经存在
				usergroups[groupname]; // 建立用户组
				log("Group " + groupname + " created");
			} else {
				log("Error: Group " + g + " already exists");
			}
		} else {
			if (whosLoggedIn == "") {
				log("Error: no user logged in");
			} else {
				log("Error: only an Administrator may issue net group command");
			}
			return false;
		}
	} else {
		usergroups[groupname]; // 建立用户组
		log("Group " + g + " created");
	}
	return false;
}

// 检查用户是否是管理员
bool isAdmin(string username) {

	vector<string> userGroup = getUserGroup(whosLoggedIn); // 得到用户所在的所有用户组

	if (find(userGroup.begin(), userGroup.end(), "Administrators") != userGroup.end() || isFirstRun) {
		return true; // 检查这个用户是否是管理员中的一员
	} else {
		return false;
	}
}

// 检查用户是否是文件的拥有者
bool isOwner(string filename, string username) {
	return (permissions[filename][0] == username)?true:false;
}

// 创建一个新用户
bool createUser(string username, string password) {
	if (isAdmin(whosLoggedIn)) { // 检查是否是管理员登录
		if (userMap[username]=="") { // 检查用户是否存在
			if (!checkCommand(username)) { // 检查用户名是否符合命名规范
				if (username != "admin") { // 检查用户名是否是admin
					usergroups["Users"].push_back(username); // 如果不是，将user加入进用户-用户组字典中
				}
				userMap[username] = password; // 添加密码
				UAC[username] = "Change"; // 添加UAC,默认为Change
				log("User " + username + " created"); // 记录结果
				isFirstRun = false;

				log(getUACString(username)); // 记录UAC
			}
		} else {
			log("Error: user " + username + " already exists");
			return false;
		}
		return true;
	} else {
		if (whosLoggedIn == "") {
			log("Error: no user logged in");
		} else {
			log("Error: only an Administrator may issue net user command");
		}
		return false;
	}
}

// 删除一个用户
bool deleteUser(string username) {
	if (isAdmin(whosLoggedIn)) { // 检查是否是管理员登录
		if (userExists(username)) { // 检查用户是否存在
			if (username != whosLoggedIn) { // 检查要删除的用户是否是当前用户
				if (username != "admin") {
					bool isFound = false;
					for (auto it: usergroups) { // 遍历用户-用户组字典
						string itString = it.first;
						vector<string> itVector = it.second;
						for (vector<string>::iterator _it = itVector.begin(); _it != itVector.end();) {
							if(*_it == username) {
								itVector.erase(_it);
								isFound = true;
							} else {
								++_it;
							}
						}
						usergroups[itString].clear();
						usergroups[itString] = itVector;
					}
					if (!isFound) {
						log("Error: No user found");
						return false;
					}
					auto itUserMap = userMap.find(username); // 删除用户-密码中的用户
					userMap.erase(itUserMap);
					auto itUAC = UAC.find(username); // 删除用户-UAC中的用户
					UAC.erase(itUAC);
					
					log("User " + username + " deleted");
					return true;
				} else {
					log("Error: Cannot delete user \'admin\'");
					return false;
				}
			} else {
				log("Error: Cannot delete logged in user");
				return false;
			}
		} else {
			log("Error: user " + username + " not found");
			return false;
		}
	} else {
		if (whosLoggedIn == "") {
			log("Error: no user logged in");
		} else {
			log("Error: only an Administrator may issue del user command");
		}
		return false;
	}
}

// 删除一个用户组
bool deleteGroup(string groupname) {
	if (isAdmin(whosLoggedIn)) { // 检查是否是管理员登录
		if (groupExists(groupname)) { // 检查用户组是否存在
			if (groupname != "Administrators" && groupname != "Users") { // 检查是否要删除Administractors或者Users
				auto it = usergroups.find(groupname);
				usergroups.erase(it);
				log("Group " + groupname + " deleted");
				return true;
			} else {
				log("Error: Cannot delete group \'Administrators\' or \'Users\'");
				return false;
			}
		} else {
			log("Error: group " + groupname + " not found");
			return false;
		}
	} else {
		if (whosLoggedIn == "") {
			log("Error: no user logged in");
		} else {
			log("Error: only an Administrator may issue del group command");
		}
		return false;
	}
}

// 检查安装过程中的前四行
int checkSetup(vector<string> instructions) {
	if (instructions[0].substr(0, 14) != "net user admin") {
		log("Error: First line is not 'net user admin'\nExiting...\n");
		return 1;
	} else if (instructions[1].substr(0, 24) != "net group Administrators") {
		log("Error: Second line is not 'net group Administrators'\nExiting...\n");
		return 2;
	} else if (instructions[2].substr(0, 24) != "net group Administrators") {
		log("Error: Third line is not 'net group Administrators'\nExiting...\n");
		return 3;
	} else if (instructions[3].substr(0, 15)  != "net group Users") {
		log("Error: Fourth line is not 'net group Users'\nExiting...\n");
		return 4;
	} else {
		return 0;
	}
}

// 检查用户名是否包含合法的字符
int checkCommand(string username) {
	smatch m;
  	regex e ("\\s|\\/|\v|\\:"); // 使用C++11标准的正则表达式
	// \s 匹配任意的空格字符 [\r\n\t\f ]
	// \/ 匹配 /
	// \v 匹配任意的垂直空格字符
	// \: 匹配 :

	if (username.length()>30) { // 检查字符串长度
		log("Error: Username cannot be more than 30 characters");
		return 1;
	} else if (regex_search (username,m,e)) { // 检查用户名是否匹配
		log("Error: Username, group name, or filename  cannot contain forward slash ('/'), colon (':'), carriage return, form feed, horizontal tab, new line, vertical tab, and space.");
		return 1;
	} else {
		return 0;
	}
}

// 检查密码是否包含合法的字符
int checkPassword(string password) {
	smatch m;
  	regex e ("\\s|\v"); // 使用C++11标准的正则表达式
	// \s 匹配任意的空格字符 [\r\n\t\f ]
	// \v 匹配任意的垂直空格字符

	if (password.length()>30) { // 检查字符串长度
		log("Error: Password cannot be more than 30 characters");
		return 1;
	} else if (regex_search (password,m,e)) { // 检查密码是否匹配
		log("Error: Password cannot contain forward carriage return, form feed, horizontal tab, new line, vertical tab, and space.");
		return 1;
	} else {
		return 0;
	}
}

// 用户登录
void login(string username, string password) {
	wrongUsername = true;
	wrongPassword = true;
	for (auto map : userMap) { // 遍历用户-密码字典
		string u, p;
		u = map.first;
		p = map.second;
		if (u.compare(username) == 0) { // 比较用户名和文件中的用户名（区分大小写）
			if (p.compare(password) == 0) { // 密码同样
				log("User " + u + " logged in"); // 如果两个都正确则允许登录
				whosLoggedIn = u;
				isLoggedIn = true;
				wrongPassword = false;
			}
		}
	}
	if (!isLoggedIn && wrongUsername && wrongPassword) {
		log("Login failed: invalid username or password");
	}
}

// 得到用户所在的所有用户组
vector<string> getUserGroup(string username) {
	vector<string> userG; // 保存用户组的数组
	for (auto map : usergroups) { // 遍历用户-用户组字典
		vector<string> itVector = map.second; // 用户数组
		for (size_t i = 0; i < itVector.size(); i++) { // 遍历用户数组
			if (username == itVector[i]) { // 如果用户名在用户数组中
				userG.push_back(map.first); // 将组名加入到用户组中
			}
		}
	}
	return userG; // 返回
}

// 登出当前用户
void logout() {
	if(isLoggedIn) {
		log("User " + whosLoggedIn + " logged out");
		whosLoggedIn = "";
		isLoggedIn = false;
	} else {
		log("logout: No user logged in");
	}
}
// 用户组是否存在
bool groupExists(string groupname) {
	return (usergroups.find(groupname) != usergroups.end());
}

// 用户是否存在
bool userExists(string username) {
	return (userMap.find(username) != userMap.end());
}

// 将日志记录到audit.txt文件中
void log(string text) {
	myAudit.open("audit.txt", ios_base::app);
	myAudit << text << endl;
	myAudit.close();
}

// 比较两个字符串（不区分大小写）
bool strncasecmp(string s1, string s2) {
	for (size_t i = 0; i < s1.length(); i++) {
		s1[i] = tolower(s1[i]);
	}
	for (size_t i = 0; i < s2.length(); i++) {
		s2[i] = tolower(s2[i]);
	}
	return strcmp(s1.c_str(),s2.c_str());
}
