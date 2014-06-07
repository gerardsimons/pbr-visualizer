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
#include <string>
#include <iostream>

namespace reporter {
    Task::Task(std::string name_,float maxRounds_) {
        name = name_;
        maxRounds = maxRounds_;
    }
    void Task::start() {
        if(isRunning) {
            throw "Task already running.";
        }
        printf("Task \"%s\" started.",name.c_str());
        isRunning = true;
        startTime = clock();
    }
    void Task::update(float completed) {
        if(isRunning) {
            printf("Task %s is %f%% completed.\n",name.c_str(),completed / maxRounds * 100.F);
        }
        else throw std::string("No task is running.");
    }
    //Undetermined update
    void Task::update() {
        printf(".");
    }
    void Task::stop() {
        if(isRunning) {
            double duration = ( clock() - startTime ) / (double) CLOCKS_PER_SEC;
            isRunning = false;
            std::cout << "Task " << name << " took " << duration << " seconds.\n";
        }
        else throw("Task is not running!\n");
    }
}

std::map<std::string,reporter::Task*> runningTasks;

void startTask(std::string taskName,float maxRounds_ = 100) {
    if(!runningTasks[taskName]) {
        reporter::Task* newTask = new reporter::Task(taskName,maxRounds_);
        runningTasks[taskName] = newTask;
        newTask->start();
    }
    else {
        throw("Some task with this name is already running.\n");
    }
}

void update(std::string taskName, float completed) {
    reporter::Task* runningTask = runningTasks[taskName];
    if(runningTask) {
        runningTask->update(completed);
    }
    else throw std::string("No such task is running.");
}
void update(std::string taskName) {
    reporter::Task* runningTask = runningTasks[taskName];
    if(runningTask) {
        runningTask->update();
    }
    else throw std::string("No such task is running.");
}
void stop(std::string taskName) {
    reporter::Task* runningTask = runningTasks[taskName];
    if(runningTask) {
        runningTask->stop();
        runningTasks.erase(taskName);
    }
    else throw std::string("No such task is running.");
}



