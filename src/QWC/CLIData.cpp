#include "CLIData.h"
#include <string.h>
#include <algorithm>

SubCmd* TopCmd::GetSubCmd(const char* cmdStr)
    {
    auto* ptr = head;
    while (ptr)
    {
        if (_strnicmp(ptr->cmdName, cmdStr, std::max(strlen(ptr->cmdName), strlen(cmdStr))) == 0)
            return ptr;
        ptr = ptr->next;
    }

    return nullptr;
    }

TopCmd* CLI::GetTopCmd(const char* cmdStr)
{
    auto* ptr = commandTable->head;
    while (ptr)
    {
        if (_strnicmp(ptr->cmdName, cmdStr, std::max(strlen(ptr->cmdName), strlen(cmdStr))) == 0)
            return ptr;
        ptr = ptr->next;
    }

    return nullptr;
}

bool CLI::IsValidCommand(const char* cmdStr)
{
    TopCmd* tptr = NULL;
    SubCmd* sptr = NULL;
    char* next_token = NULL;

    const char* loc = strchr(cmdStr, '.');
    if (!loc || cmdStr == loc) return false;
    char tStr[128];
    strncpy_s(tStr, cmdStr, loc - cmdStr);

    tptr = GetTopCmd(tStr);
    ++loc;
    if (tptr && loc)
    {
       strcpy_s(tStr, loc);
       const char* spc = strtok_s(tStr, " ", &next_token);
       if (!spc)
       {
           spc = &tStr[0];
       }

       //If subCmd is valid but doesn't have args, return early
       sptr = tptr->GetSubCmd(spc);
       if (!sptr)
           return false;
       else if (sptr->args == nullptr)
           return true;

       const char* arg = strtok_s(NULL, " ", &next_token);
       auto* params = sptr->args;
       std::uint32_t dummy[2];
       while (arg != nullptr && params != nullptr)
       {
           if (!params->IsValidParam(&arg, dummy)) 
              return false;
           params = params->next;
           arg = strtok_s(NULL, " ", &next_token);
       }

       if (!arg && !params) return true;
    }

    return false;
}