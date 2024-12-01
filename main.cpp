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

void clear() {
	json tasks = {
		{"maxId", -1}
	};

	ofstream outFile("tasks.json");
	outFile << tasks.dump(4); // Сохраняем с отступами для читаемости
    	outFile.close();
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




void mark(int& id){
	json tasks = get_tasks();
	if(tasks[to_string(id)]["status"] == "[ ]"){
		tasks[to_string(id)]["status"] = "[x]";
	} else{	
		tasks[to_string(id)]["status"] = "[ ]";
	}
	save_task(tasks);
}
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

void update_task(int& id, string& description){
	json tasks = get_tasks();
	tasks[to_string(id)]["description"] = description;
	tasks[to_string(id)]["updatedAt"] = get_time();
	save_task(tasks);
	cout << id << " "
	<< tasks[(to_string(id))]["status"].get<string>() << " "
	<< description << endl;
}

void remove(int& id){
	json tasks = get_tasks();
	tasks.erase(to_string(id));
	save_task(tasks);
	cout << "Task " << id << " removed" << endl;
}


void list () {
	const json& tasks = get_tasks();
	
	for(int i = 0; i < tasks["maxId"].get<int>() + 1; i++){
		if (tasks.contains(to_string(i))){
			cout << i << " " 
			<< tasks[to_string(i)]["status"].get<string>() 
			<< " " << tasks[to_string(i)]["description"].get<string>() << endl;
		}else {
			continue;
		}	
	}
}

void list_all () {
	const json& tasks = get_tasks();
	for(int i = 0; i < tasks["maxId"].get<int>() + 1; i++){
		if (tasks.contains(to_string(i))){
			string str_id = to_string(i);
			cout << i << " " 
			<< tasks[str_id]["status"].get<string>() 
			<< " " << tasks[str_id]["description"].get<string>() << endl
			<< "    : createdAt: " << tasks[str_id]["createdAt"].get<string>() << endl
			<< "    : updatedAt: " << tasks[str_id]["updatedAt"].get<string>() << endl;
		}else {
			continue;
		}	
	}
}

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
		cout << newId << " [ ] " << this->description << endl;
	};
	
	
};


bool isArg(int argc, char* argv[], const char* arg) {
    for (int i = 1; i < argc; ++i) {
        if (string(argv[i]) == arg) {  // Преобразуем argv[i] в строку и сравниваем
            return true;
        }
    }
    return false;
}

void help(){
	cout << "Use: task-cli [arg]\n"
        << "-l, --list                               - all tasks\n"
        << "    -la, --list-all                      - full list\n"
	<< "-a, --add [description]                         - add task\n"
        << "-r, --remove [task-id]                   - remove task\n"
        << "-u, --update [task-id] [new-description] - update task\n"
	<< "-c, --clear                              - clear all tasks\n"
	<< "-m, --mark [task-id]                     - mark task (change to the opposite)\n";
}


int main(int argc, char* argv[]) {
	if (argc < 2) {
		help();
	} else if(isArg(argc, argv, "-a") || isArg(argc, argv, "--add")) {
		if (argc < 2 || argc > 3){
			help();
		}else {
			string desc = *(argv + 2);
			Task task(desc);
			
		}
		
	} else if(isArg(argc, argv, "-c") || isArg(argc, argv, "--clear")){
		if (argc > 2){
			help();
		}else {
			clear();
			cout << "Cleared\n";
		}	
	} else if(isArg(argc, argv, "-l") || isArg(argc, argv, "--list")){
		if (argc > 2){
			help();
		}else {
			list();
		}	
	} else if(isArg(argc, argv, "-la") || isArg(argc, argv, "--list-all")){
		if (argc > 2){
			help();
		}else {
			list_all();
		}
	} else if(isArg(argc, argv, "-m") || isArg(argc, argv, "--mark")){
		if (argc < 3 || argc > 3){
			help();
		}else {
		        try {
				int id = stoi(*(argv + 2));
			        mark(id);
        		} catch (const std::invalid_argument& e) {
            			cerr << "Invalid ID: Not a number." << endl;
        		} catch (const std::out_of_range& e) {
            			cerr << "Invalid ID: Out of range." << endl;
        		}
		}	
	}   else if(isArg(argc, argv, "-r") || isArg(argc, argv, "--remove")){
		if (argc < 3 || argc > 3){
			help();
		}else {
		        try {
				int id = stoi(*(argv + 2));
			        remove(id);
        		} catch (const std::invalid_argument& e) {
            			cerr << "Invalid ID: Not a number." << endl;
        		} catch (const std::out_of_range& e) {
            			cerr << "Invalid ID: Out of range." << endl;
        		}
		}	
	} else if(isArg(argc, argv, "-u") || isArg(argc, argv, "--update")){
		if (argc < 4 || argc > 4){
			help();
		}else {
			 try {
				int id = stoi(*(argv + 2));
			        string desc = *(argv + 3);
				update_task(id, desc);
				
			
        		} catch (const std::invalid_argument& e) {
            			cerr << "Invalid ID: Not a number." << endl;
        		} catch (const std::out_of_range& e) {
            			cerr << "Invalid ID: Out of range." << endl;
        		}
		}	
	} 

	
	

	return 0;
}
