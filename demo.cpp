#include "demo.h"

// 打印日志文件
void printLog() {
    ifstream logreader;
    logreader.open("audit.txt", ios_base::in); // 打开日志文件
    if (!logreader.is_open()) {
        cout << "Error: audit.txt doesn't exit!" << endl;
    } else {
        string line;
        while (getline(logreader, line)) { // 读取行
            cout << line << endl;
        }
    }
    logreader.close(); // 关闭文件
}

int main(int argc, char const *argv[]) {
    if (argc < 2) { // 检查参数中是否有文件名，否则出错
		log("Error: Please specify a filename.\nUse the format './demo filename'");
        printLog();
		return 1;
	} else {
        string testname = string(argv[1]);
		bool isSuccessful = processTestCase(testname);
        printLog();
        if(isSuccessful) {
            return 0;
        } else {
            return 1;
        }
	}
}
