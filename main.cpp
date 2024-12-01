#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <ctime> 
#include <iomanip> 
#include <sstream>
#include <nlohmann/json.hpp>
#include <fstream>


using namespace std;
using json = nlohmann::json;



class Task {
private:
	string description;
	string status;
	string createdAt;
	string updatedAt;
public:
	Task(string& description) {
		this->description = description;
		this->status = "[ ]";
		this->createdAt = get_time();
		this->updatedAt = get_time();

		add_task();
	}


	json get_tasks(){
		ifstream taskFile("tasks.json");
		if (taskFile){
			json tasks;
			taskFile >> tasks;
			taskFile.close();
			return tasks; 
		} else {
			clear();
			ifstream taskFile("tasks.json");	
			json tasks;
			taskFile >> tasks;
			taskFile.close();
			return tasks;
		}	 		
	}
	
	void save_task(const json& tasks){
		ofstream outFile("tasks.json");
		outFile << tasks.dump(4); // Сохраняем с отступами для читаемости
    		outFile.close();
	}

	void add_task() {	
		json tasks = get_tasks();
				
		int newId = tasks["maxId"].get<int>() + 1;
		
		tasks[to_string(newId)] = {
			{"description", this->description},
			{"status", this->status},
			{"createdAt", this->createdAt},
			{"updatedAt", this->updatedAt} 
		};
		
		tasks["maxId"] = newId;
		save_task(tasks);
	};
	

	string get_time(){
		auto now = std::chrono::system_clock::now();

    		// Преобразование в time_t
    		std::time_t now_time = std::chrono::system_clock::to_time_t(now);
    		std::tm local_time = *std::localtime(&now_time);

    		// Сохранение даты и времени в строку
    		std::ostringstream datetime_stream;
    		datetime_stream << std::put_time(&local_time, "%Y-%m-%d %H:%M");
    		return datetime_stream.str();	
	}
	
};

bool isArg(char* argv[], const char arg[]){
	if(*(argv + 1) == arg);
		return true;
	return false;
}

void help(){
	cout << "Use: task-cli [arg]\n"
        << "-l, --list                               - all tasks\n"
        << "-a, --add [description]                         - add task\n"
        << "-r, --remove [task-id]                   - remove task\n"
        << "-u, --update [task-id] [new-description] - update task";
}


int main(int argc, char* argv[]) {
	if (argc < 2) {
		help();
	} else if(isArg(argv, "-a") || isArg(argv, "--add")) {
		if (argc == 2){
			help();
		}else {
			string desc = *(argv + 2);
			Task task(desc);
			cout << "[ ] " << *(argv + 2) << endl;
		}
		
	} else if(isArg(argv, "-c") || isArg(argv, "--clear")){
		if (argc > 2){
			help();
		}else {
			Task::clear();
			cout << "Cleared\n";
		}	
	} 

	
	

	return 0;
}
