//
//  OctreeConfig.h
//  afstuderen_DO_NOT_DELETE
//
//  Created by Gerard Simons on 05/09/14.
//  Copyright (c) 2014 Gerard Simons. All rights reserved.
//

#ifndef __afstuderen_DO_NOT_DELETE__OctreeConfig__
#define __afstuderen_DO_NOT_DELETE__OctreeConfig__

#include <stdio.h>

class OctreeConfig {
private:
	const size_t MAX_DEPTH;
	const size_t MAX_NODE_CAPACITY;
	const float MIN_NODE_SIZE;
public:
	size_t MaxDepthAllowed() const {
		return MAX_DEPTH;
	}
	size_t MaxNodeCapacity() const {
		return MAX_NODE_CAPACITY;
	}
	size_t MinNodeSize() const {
		return MIN_NODE_SIZE;
	}
	OctreeConfig(size_t MAX_DEPTH_, size_t MAX_NODE_CAPACITY_, float MIN_NODE_SIZE_) : MAX_DEPTH(MAX_DEPTH_), MAX_NODE_CAPACITY(MAX_NODE_CAPACITY_), MIN_NODE_SIZE(MIN_NODE_SIZE_) {
			//Nothing else to do
	};
};

#endif /* defined(__afstuderen_DO_NOT_DELETE__OctreeConfig__) */
