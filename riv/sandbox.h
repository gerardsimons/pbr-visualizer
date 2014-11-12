//
//  sandbox.h
//  pbrt
//
//  Created by Gerard Simons on 17/09/14.
//
//

#ifndef pbrt_sandbox_h
#define pbrt_sandbox_h

#include "pbrt-v2/src/core/parser.h"
#include "pbrt-v2/src/core/api.h"

#include <QCoreApplication>

#include <string>

void testPBRTParser(const std::string& filename) {
	
	Options options;
	options.nCores = 1;
	
	pbrtInit(options);
	ParseFile(filename);
}

void testQT() {
	
}

#endif
