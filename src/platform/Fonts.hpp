#pragma once

#include <sstream>
#include <string>
#include <cstdio>
#include <cstdlib>

///Platform-specific code to guess font files.
namespace platform {

/**
 * Guess a default monospace-font for a given platform.
 */
std::string GuessMonoFont() {
	std::string res;
#if defined(_WIN32)
	char* winDir = getenv("WINDIR");
	if (winDir) {
		res = std::string(winDir) + "/" + "cour.ttf";
	}
#elif defined(_APPLE_) && defined(_MACH_)
	//Monaco is the default; not sure if SFML can handle "dfonts".
	res = "/System/Library/Fonts/Monaco.dfont";
#elif defined(linux) || defined(__linux)
    FILE* f = popen("fc-match -v mono | grep file: | sed 's/.*\"\\(.*\\)\".*/\\1/'", "r");
    if (f) {
		const int BUFSIZE = 1024; //TODO: This is problematic.
		char buf[BUFSIZE];
		while(fgets(buf, BUFSIZE,  f)) { //Read the first line only.
			res = std::string(buf);
			res.erase(res.find_last_not_of("\r\n")+1); //Trailing newline.
		}
		pclose(f);
    }
#endif
    return res;
}



}
