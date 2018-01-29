![Xeekworx](http://xeekworx.com/images/github/xeekworx_logo.png) <br />
Logger
===========
**Logger is a single header/source class that you can easily include with your C/C++ project that gives you flexible logging capabilities with an iostream like syntax.**

Note: C++11 is required. If you're using Visual Studio 2015, make sure you're on at least Update 2. Some of what's used in C++11 is to convert UTF8/16 strings, but I may replace this later since those features may be deprecated in C++17.

If you want to enable colorized output for the logger then do "#define XWLOG_ENABLE_COLORS 1" just before the inclusion of logger.h. This also means that the rlutil third-party repository will be needed. I may replace that repo with my own code in the future since rlutil is not very well maintained for every platform.

HOW IT WORKS
------------
Preprocessor configurations (#define before including logger.h):
```cpp
// If you want colorized output, the rlutil third party repository will be required
#define XWLOG_ENALBE_COLORS

// If you want a global log object (xeekworx::log)
#define XWLOG_ENABLE_GLOBALOBJECT
```

First, you must include *"logger.h"* and put logger.c in your project, if your build environment hasa project; otherwise just make sure you're compiling with logger.c.

To be continued ...

