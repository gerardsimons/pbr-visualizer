//
//  Reporter.h
//  Afstuderen
//
//  Created by Gerard Simons on 04/06/14.
//  Copyright (c) 2014 Gerard Simons. All rights reserved.
//

#ifndef __Afstuderen__Reporter__
#define __Afstuderen__Reporter__

#include <string>
#include <map>

namespace reporter {
    class Task {
    private:
        bool isRunning = false;
        std::string name;
        clock_t startTime;
        float maxRounds;
    public:
        Task(std::string name_,float maxRounds_);
        void start();
        void update(float completed);
        //Undetermined update
        void update();
        void stop();
    };
    
    extern std::map<std::string,Task*> runningTasks;
	void startTask(char* taskname,float maxRounds_);
    void startTask(const std::string& taskName,float maxRounds_ = 100);
    void update(std::string taskName, float completed);
    void update(std::string taskName);
    void stop(std::string taskName);
}

#endif /* defined(__Afstuderen__Reporter__) */
