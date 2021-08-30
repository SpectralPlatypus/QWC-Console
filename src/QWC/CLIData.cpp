#include "CLIData.h"
#include <string.h>
#include <algorithm>

CmdNode* CLI::GetCmdNode(const char* cmdStr)
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

CmdNode* CmdNode::GetChildNode(const char* cmdStr)
{
    auto* ptr = childNode;
    while (ptr)
    {
        if (_strnicmp(ptr->cmdName, cmdStr, std::max(strlen(ptr->cmdName), strlen(cmdStr))) == 0)
            return ptr;
        ptr = ptr->next;
    }

    return nullptr;
}

template <typename T, size_t N>
constexpr size_t ARRAY_SIZE(T(&)[N]) { return N; }

bool CLI::IsValidCommand(const char* cmdStr)
{
    CliLexer lexer{ cmdStr };
    CmdNode* ptr = NULL;
    char tokBuf[128] = { 0 };

    CliLexer::TokenType tType;
    ptr = commandTable->head;
    tType = lexer.GetNextToken(tokBuf, ARRAY_SIZE(tokBuf));
    ptr = GetCmdNode(tokBuf);
    if (!ptr)
        return false;

    //Commands with single level entry -- only 2 of those
    if (tType == CliLexer::EOL)
    {
        return (ptr && ptr->childNode == nullptr);
    }

    while ((tType = lexer.GetNextToken(tokBuf, ARRAY_SIZE(tokBuf))) == CliLexer::CmdNode)
    {
        ptr = ptr->GetChildNode(tokBuf);
        if (!ptr) return false;
    }

    if (tType == CliLexer::EOL)
    {
        //Check if there is a valid final node with 0 no-arg func call
        if (!ptr || (!ptr->func && !ptr->unk1c)) return false;
        // Or if all args are optional
        for (auto* args = ptr->args; args != nullptr; args = args->next)
        {
            if (args->optional == false) return false;
        }
        return true;
    }
    
    // Must be first arg, start parsing those
    CliRegParam* params = ptr->args;

    uint32_t dummy[2];
    do {
        char* tp = &tokBuf[0];
        if (!params || !params->IsValidParam(&tp, dummy))
            return false;
        params = params->next;
    } while ((tType = lexer.GetNextToken(tokBuf, ARRAY_SIZE(tokBuf))) == CliLexer::Param);

    if (tType != CliLexer::EOL || params != nullptr)
        return false;

    return true;
}