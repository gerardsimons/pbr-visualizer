//
//  reporter.cpp
//  Afstuderen
//
//  Created by Gerard Simons on 07/06/14.
//  Copyright (c) 2014 Gerard Simons. All rights reserved.
//

//
//  Reporter.h
//  Afstuderen
//
//  Created by Gerard Simons on 04/06/14.
//  Copyright (c) 2014 Gerard Simons. All rights reserved.
//


#include "reporter.h"
#include <iostream>

namespace reporter {
    
    std::map<std::string,Task*> runningTasks;
	int currentRound;
	int updateRound;
    
	Task::Task(std::string name,float maxRounds) : name(name), maxRounds(maxRounds) {

		updateRound = maxRounds / 10.F;
    }
    void Task::start() {
        if(isRunning) {
            throw std::runtime_error("Task already running.");
        }
        printf("Task \"%s\" started .........\n ",name.c_str());
        isRunning = true;
        startTime = clock();
    }
    void Task::update(float completed) {
        if(isRunning) {
            printf("Task %s is %f%% completed.\n",name.c_str(),completed / maxRounds * 100.F);
//			printf(".");
        }
        else throw std::runtime_error("No task is running.");
    }
    //Undetermined update
    void Task::update() {
		++currentRound;
		if(currentRound++ % updateRound == 0) {
			printf(".");
		}
    }
    void Task::stop() {
        if(isRunning) {
            double duration = ( clock() - startTime ) / (double) CLOCKS_PER_SEC;
            isRunning = false;
            std::cout << "Task " << name << " took " << duration << " seconds.\n";
        }
        else throw std::runtime_error("Task is not running!\n");
    }
}

void reporter::startTask(char* taskname,float maxRounds_) {
	startTask(std::string(taskname),maxRounds_);
}

void reporter::startTask(const std::string& taskName,float maxRounds_) {
    if(!reporter::runningTasks[taskName]) {
        reporter::Task* newTask = new reporter::Task(taskName,maxRounds_);
        reporter::runningTasks[taskName] = newTask;
        newTask->start();
    }
    else {
		throw std::runtime_error("Some task with this name is already running.\n");
    }
}

void reporter::update(std::string taskName, float completed) {
    reporter::Task* runningTask = reporter::runningTasks[taskName];
    if(runningTask) {
        runningTask->update(completed);
    }
    else throw std::runtime_error("No such task is running.");
}
void reporter::update(std::string taskName) {
    reporter::Task* runningTask = reporter::runningTasks[taskName];
    if(runningTask) {
        runningTask->update();
    }
    else throw std::runtime_error("No such task is running.");
}
void reporter::stop(std::string taskName) {
    reporter::Task* runningTask = reporter::runningTasks[taskName];
    if(runningTask) {
        runningTask->stop();
        reporter::runningTasks.erase(taskName);
//		delete runningTask;
    }
	else throw std::runtime_error("No such task is running.");
}