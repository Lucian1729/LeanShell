#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <dirent.h>
#include <sys/stat.h>
#include <iomanip>
#include <unistd.h>
#include <sys/wait.h>
#include <cstring>
#include <deque>
#include <filesystem>
#include <fstream>

std::string shellReadLine(){
	std::string input;
    char c;

    while (std::cin.get(c)) {
        if (c == '\n') {
            break;
        } else if (std::cin.fail()) {
            std::cerr << "Error reading input\n";
            input.clear();
            break;
        }
        input += c;
    }
    
    return input;
}

std::vector<std::string> shellSplitLine(std::string line){
	std::vector<std::string> args;
	
	int start,end;
	start=0;
	end=0;
	
	char delim = ' ';
	
	while ((start = line.find_first_not_of(delim, end))!= std::string::npos) {
		
        end = line.find(delim, start);
        args.push_back(line.substr(start, end - start));
    }
    
    return args;
}


int shell_cd(std::vector<std::string> args);
int shell_ls(std::vector<std::string> args);
int shell_touch(std::vector<std::string> args);
int shell_cp(std::vector<std::string> args);
int shell_mv(std::vector<std::string> args);
int shell_hs(std::vector<std::string> args);
int shell_clrhs(std::vector<std::string> args);
int shell_exit(std::vector<std::string> args);
int shell_help(std::vector<std::string> args);
int shell_echo(std::vector<std::string> args);
int shell_cat(std::vector<std::string> args);
int shell_df(std::vector<std::string> args);
int shell_clear(std::vector<std::string> args);

std::vector<std::string> builtin_str = {
    "cd", "ls", "touch", "cp", "mv", "hs", "clrhs", "exit",
    "help", "echo", "cat", "df", "clear"
};

std::vector<int (*)(std::vector<std::string>)> builtin_func = {
    &shell_cd, &shell_ls, &shell_touch, &shell_cp, &shell_mv, &shell_hs,
    &shell_clrhs, &shell_exit, &shell_help, &shell_echo, &shell_cat,
    &shell_df, &shell_clear
};

std::map<std::string, int (*)(std::vector<std::string>)> command_map;

std::deque<std::string> command_history;

int shell_exit(std::vector<std::string> args) {
    if (!args.empty()) {
        std::cerr << "exit: too many arguments" << std::endl;
        return 1;
    }
    
    exit(0);
}

int shell_cd(std::vector<std::string> args) {
    if (args.size() > 1) {
        std::cerr << "cd: too many arguments" << std::endl;
        return 1;
    }

    const char* dir = args.empty() ? getenv("HOME") : args[0].c_str();
    if (chdir(dir) != 0) {
        std::cerr << "cd: " << strerror(errno) << std::endl;
        return 1;
    }

    return 1;
}

int shell_ls(std::vector<std::string> args) {
    DIR* dir;
    struct dirent* entry;
    struct stat info;

    if (args.empty()) {
        dir = opendir(".");
    } else {
        dir = opendir(args[0].c_str());
    }

    if (dir == nullptr) {
        std::cerr << "ls: " << strerror(errno) << std::endl;
        return 1;
    }

    while ((entry = readdir(dir)) != nullptr) {
        std::string filename(entry->d_name);
        if (filename != "." && filename != "..") {
            std::string path = args.empty() ? filename : args[0] + "/" + filename;
            if (stat(path.c_str(), &info) == 0) {
                if (S_ISDIR(info.st_mode)) {
                    std::cout << "\033[1;34m" << std::left << std::setw(20) << filename << "\033[0m/  ";
                } else {
                    std::cout << std::left << std::setw(20) << filename << "  ";
                }
            } else {
                std::cerr << "ls: " << strerror(errno) << std::endl;
                closedir(dir);
                return 1;
            }
        }
    }

    std::cout << std::endl;
    closedir(dir);
    return 1;
}

int shell_touch(std::vector<std::string> args) {
    for (const std::string& filename : args) {
        FILE* file = fopen(filename.c_str(), "a");
        if (file == nullptr) {
            std::cerr << "touch: " << strerror(errno) << std::endl;
            return 1;
        }
        fclose(file);
    }
    return 1;
}

int shell_cp(std::vector<std::string> args) {
    if (args.size() != 2) {
        std::cerr << "cp: wrong number of arguments" << std::endl;
        return 1;
    }

    const char* src = args[0].c_str();
    const char* dst = args[1].c_str();

    FILE* src_file = fopen(src, "r");
    if (src_file == nullptr) {
        std::cerr << "cp: " << strerror(errno) << std::endl;
        return 1;
    }

    FILE* dst_file = fopen(dst, "w");
    if (dst_file == nullptr) {
        std::cerr << "cp: " << strerror(errno) << std::endl;
        fclose(src_file);
        return 1;
    }

    char buffer[4096];
    size_t bytes_read;
    while ((bytes_read = fread(buffer, 1, sizeof(buffer), src_file)) > 0) {
        fwrite(buffer, 1, bytes_read, dst_file);
    }

    fclose(src_file);
    fclose(dst_file);

    return 1;
}

int shell_hs(std::vector<std::string> args) {
    if (args.empty()) {
        for (const std::string& cmd : command_history) {
            std::cout << cmd << std::endl;
        }
    } else {
        std::cerr << "hs: too many arguments" << std::endl;
        return 1;
    }
    return 1;
}

int shell_clrhs(std::vector<std::string> args) {
    if (args.empty()) {
        command_history.clear();
    } else {
        std::cerr << "clrhs: too many arguments" << std::endl;
        return 1;
    }
    return 1;
}

int shell_mv(std::vector<std::string> args) {
    if (args.size() != 2) {
        std::cerr << "mv: wrong number of arguments" << std::endl;
        return 1;
    }

    const char* src = args[0].c_str();
    const char* dst = args[1].c_str();

    if (std::rename(src, dst) != 0) {
        std::cerr << "mv: " << strerror(errno) << std::endl;
        return 1;
    }

    return 1;
}

int shell_help(std::vector<std::string> args) {
    if (!args.empty()) {
        std::cerr << "help: too many arguments" << std::endl;
        return 1;
    }

    std::cout << "Available commands: " << std::endl;
    std::cout << "  cd [path]     - Change the current working directory" << std::endl;
    std::cout << "  ls [path]     - List files and directories in the specified path" << std::endl;
    std::cout << "  touch [files] - Create new empty files" << std::endl;
    std::cout << "  cp [src] [dst] - Copy a file from source to destination" << std::endl;
    std::cout << "  mv [src] [dst] - Move a file or directory from source to destination" << std::endl;
    std::cout << "  hs            - Show command history" << std::endl;
    std::cout << "  clrhs         - Clear command history" << std::endl;
    std::cout << "  exit          - Exit the shell" << std::endl;
    std::cout << "  help          - Display this help message" << std::endl;
    std::cout << "  echo [args]   - Print the arguments to the console" << std::endl;
    std::cout << "  cat [files]   - Print the contents of files to the console" << std::endl;
    std::cout << "  df            - Show available disk space" << std::endl;
    std::cout << "  clear         - Clear the console screen" << std::endl;
    std::cout << "Other built-in Unix commands can also be run if they exist in the PATH." << std::endl;

    return 1;
}

int shell_echo(std::vector<std::string> args) {
    for (const auto& arg : args) {
        std::cout << arg << " ";
    }
    std::cout << std::endl;

    return 1;
}

int shell_cat(std::vector<std::string> args) {
    for (const auto& filename : args) {
        std::ifstream file(filename);
        if (file.is_open()) {
            std::string line;
            while (std::getline(file, line)) {
                std::cout << line << std::endl;
            }
            file.close();
        } else {
            std::cerr << "cat: " << strerror(errno) << std::endl;
            return 1;
        }
    }

    return 1;
}

int shell_df(std::vector<std::string> args) {
    if (!args.empty()) {
        std::cerr << "df: too many arguments" << std::endl;
        return 1;
    }

    std::filesystem::space_info si = std::filesystem::space("/");
    std::cout << "Disk space available: " << si.available << " bytes" << std::endl;

    return 1;
}

int shell_clear(std::vector<std::string> args) {
    if (!args.empty()) {
        std::cerr << "clear: too many arguments" << std::endl;
        return 1;
    }

    std::cout << "\033[2J\033[1;1H";

    return 1;
}

int shellLaunch(std::vector<std::string> args) {
    pid_t pid = fork();
    if (pid < 0) {
        std::cerr << "fork error" << std::endl;
        return -1;
    } else if (pid == 0) {
        char* argv[args.size() + 1];
        int i = 0;
        for (const std::string& arg : args) {
            argv[i++] = const_cast<char*>(arg.c_str());
        }
        argv[i] = nullptr;

        execvp(argv[0], argv);
        //std::cerr << "execvp error: command probaly doesn't exist!" << std::endl;
        abort();
    } else {
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status)) {
            return WEXITSTATUS(status);
        } else {
            return -1;
        }
    }
}

int shellExecute(std::vector<std::string> args){
	if(args.empty()){
		std::cout<<"Please enter something :)\n";
		return 1;
	}

    std::string command = args[0];
    args.erase(args.begin());
	
    int status;
    std::string fullCommand=command;

	auto it = command_map.find(command);
    if (it != command_map.end()) {
        int (*func)(std::vector<std::string>) = it->second;
        status = func(args);
        for (const auto& arg : args) {
            fullCommand += " " + arg;
        }
        command_history.push_back(fullCommand);
        return status;
    } else {
		args.insert(args.begin(), command);
        status = shellLaunch(args);
        if (status == -1) {
            std::cout << "Command not found: " << command << std::endl;
            return 1;
        }
        for (const auto& arg : args) {
            fullCommand += " " + arg;
        }
        command_history.push_back(fullCommand);
        return 1;
    }
}

void shellLoop(void){
	std::string line;
	std::vector<std::string> args;
	int status;
	
	for (int i = 0; i < builtin_str.size(); ++i) {
        command_map[builtin_str[i]] = builtin_func[i];
    }
	
	do {
    printf("> ");
    line = shellReadLine();
    args = shellSplitLine(line);
    status = shellExecute(args);

  } while (status);
}


int main(int argc, char** argv){
	
	shellLoop();
	
	return 0;
}