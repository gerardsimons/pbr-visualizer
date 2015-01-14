//
//  EventManager.h
//  embree
//
//  Created by Gerard Simons on 13/01/15.
//
//

#ifndef __embree__EventManager__
#define __embree__EventManager__

#include <map>

class Event {
private:
public:
	
	void static CreateUIEvent();
};

class EventManager {
private:

public:
	void registerEventListener();
	void processEvent(Event* event);
	
};

#endif /* defined(__embree__EventManager__) */
