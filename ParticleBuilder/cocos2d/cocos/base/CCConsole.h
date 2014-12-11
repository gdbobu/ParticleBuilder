/****************************************************************************
 Copyright (c) 2013-2014 Chukong Technologies Inc.

 http://www.cocos2d-x.org

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/


#ifndef __CCCONSOLE_H__
#define __CCCONSOLE_H__

#if defined(_MSC_VER) || defined(__MINGW32__)
#include <BaseTsd.h>
#include <WinSock2.h>

#ifndef __SSIZE_T
#define __SSIZE_T
typedef SSIZE_T ssize_t;
#endif // __SSIZE_T

	#else
	#include <sys/select.h>
	#endif

#include <thread>
#include <vector>
#include <map>
#include <functional>
#include <string>
#include <mutex>
#include <stdarg.h>

#include "base/CCRef.h"
#include "base/ccMacros.h"
#include "platform/CCPlatformMacros.h"


NS_CC_BEGIN

/// The max length of CCLog message.
static const int MAX_LOG_LENGTH = 16*1024;

/**
 @brief Output Debug message.
 */
void CC_DLL log(const char * format, ...) CC_FORMAT_PRINTF(1, 2);

/** Console is helper class that lets the developer control the game from TCP connection.
 Console will spawn a new thread that will listen to a specified TCP port.
 Console has a basic token parser. Each token is associated with an std::function<void(int)>.
 If the std::function<> needs to use the cocos2d API, it needs to call
 
 ```
 scheduler->performFunctionInCocosThread( ... );
 ```
 */

class CC_DLL Console
    : public Ref
{
public:
    struct Command {
        std::string name;
        std::string help;
        std::function<void(int, const std::string&)> callback;
    };

    /** Constructor */
    Console();

    /** Destructor */
    virtual ~Console();

    /** starts listening to specifed TCP port */
    bool listenOnTCP(int port);

    /** starts listening to specifed file descriptor */
    bool listenOnFileDescriptor(int fd);

    /** stops the Console. 'stop' will be called at destruction time as well */
    void stop();

    /** add custom command */
    void addCommand(const Command& cmd);
    /** log something in the console */
    void log(const char *buf);
 
protected:
    void loop();
    ssize_t readline(int fd, char *buf, size_t maxlen);
    ssize_t readBytes(int fd, char* buffer, size_t maxlen, bool* more);
    bool parseCommand(int fd);
    
    void addClient();

    // Add commands here
    void commandHelp(int fd, const std::string &args);
    void commandExit(int fd, const std::string &args);
    void commandSceneGraph(int fd, const std::string &args);
    void commandFileUtils(int fd, const std::string &args);
    void commandConfig(int fd, const std::string &args);
    void commandTextures(int fd, const std::string &args);
    void commandResolution(int fd, const std::string &args);
    void commandProjection(int fd, const std::string &args);
    void commandDirector(int fd, const std::string &args);
    void commandTouch(int fd, const std::string &args);
    void commandUpload(int fd);
    // file descriptor: socket, console, etc.
    int _listenfd;
    int _maxfd;
    std::vector<int> _fds;
    std::thread _thread;

    fd_set _read_set;

    bool _running;
    bool _endThread;

    std::string _writablePath;

    std::map<std::string, Command> _commands;

    // strings generated by cocos2d sent to the remote console
    bool _sendDebugStrings;
    std::mutex _DebugStringsMutex;
    std::vector<std::string> _DebugStrings;

    intptr_t _touchId;
private:
    CC_DISALLOW_COPY_AND_ASSIGN(Console);
};

NS_CC_END

#endif /* defined(__CCCONSOLE_H__) */
