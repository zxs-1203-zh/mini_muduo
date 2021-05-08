#ifndef PROCESS_INFO_H
#define PROCESS_INFO_H

#include <unistd.h>
#include <string>


namespace mini_muduo
{

using std::string;

namespace ProcessInfo
{

pid_t pid();
string pidString();

string hostname();

}//ProcessInfo

}//mini_muduo


#endif
