#pragma once
#include <cstdint>
#include <string.h>

constexpr std::uintptr_t CLISingleton = 0x1060114;
constexpr std::uintptr_t SingletonInit = 0x1060120;
constexpr std::uintptr_t CircularBufferPtr = 0x1068D4C;

struct CmdNode;
class CliRegParam;

struct CircularBuffer
{
    virtual ~CircularBuffer();     // 00

    char buffer[0xFF][0x80];       // 04
    std::uint32_t unk_7f84;        // 7F84
    size_t bufIdx;                 // 7F88
};
//static_assert(sizeof(CircularBuffer) == 0x7F8C);

struct CommandTable
{
    uint32_t pad[3];    // 00
    CmdNode* head;       // 0C
};

struct CmdNode
{
    char* cmdName;          // 00
    CliRegParam* args;      // 04
    void* parent;           // 08 - SubCmd or Command Table
    CmdNode* childNode;     // 0C
    CmdNode* next;          // 10
    void* optVar;           // 14
    void* func;             // 18
    int32_t unk1c;          // 1C

    CmdNode* GetChildNode(const char* cmdStr);
};
// static_assert(sizeof(CmdNode) == 0x20);

class CliRegParam  // Has children types
{
public:
    enum ParamType
    {
        Hex = 0,
        Decimal = 1,
        Boolean = 2,
        AlphaNum = 3,   //?
        ObjName = 4,    //?  Optionally quoted
        String = 5      //?
    };


    virtual ~CliRegParam();                                                  // 00
    virtual bool IsValidParam(char** paramStr, void* unk);             // 01

    CmdNode* parent;    // 04
    char* argName;      // 08
    ParamType type;     // 0C
    CliRegParam* next;  // 10
    bool  optional;     // 14
    void* unk18;        // 18
    int32_t unk1c;      // 1c

    const char* GetType() const
    {
        switch (type)
        {
        case Hex:   return "Hex";
        case Decimal:   return "Decimal";
        case Boolean: return "Bool";
        case AlphaNum: return "AlphaNum";
        case ObjName: return "Object";
        case String: return "String";
        default:      return "Unk";
        }
    }
};
// static_assert(sizeof(CliRegParam) == 0x20);

class CLI
{
public:
    std::uint32_t unk04;         // 04
    CommandTable* commandTable;     // 08

    virtual void Unk00 (std::uintptr_t param1);                                                                                         // 00
    virtual void AddTopLevelCmd (char* cmdName, std::uint32_t arg2, std::uint32_t arg3);                                                // 01
    virtual void AddSubCmd (CmdNode* top, char* cmdName, std::uint32_t arg2, std::uint32_t arg3);                                        // 02
    virtual void AddSubCmdWithVar(CmdNode* top, char* cmdName, std::uintptr_t val, std::uint32_t arg3, std::uint32_t arg4);              // 03
    virtual void AddCliRegParam(CmdNode* sub, char* cmdName, CliRegParam::ParamType type, std::uint32_t arg3);                           // 04
    virtual void Unk05(std::uintptr_t param1, std::uintptr_t param2);                                                                   // 05
    virtual void Unk06(std::uintptr_t param1);                                                                                          // 06
    virtual bool ExecCmd(const char* cmdStr, std::uintptr_t param2 = 0x5b880c);                                                         // 07

    CmdNode* GetCmdNode(const char* cmdStr);
    bool IsValidCommand(const char* cmdStr);
};
//static_assert(sizeof(CLI) == 0x20);

class CliLexer
{
    const char* input;
    bool parseArgs;

public:
    enum TokenType
    {
        CmdNode = 0,
        Param,
        EOL
    };

    CliLexer(const char* inText) :
        input(inText),
        parseArgs(false)
    {}

    TokenType GetNextToken(char* buf, size_t bufLen)
    {
        const char* ptr = input;
        TokenType retVal = EOL;

        if (*input == '\0')
        {
            return EOL;
        }

        while (ptr)
        {
            if (*ptr == '.' && !parseArgs)
            {
                strncpy_s(buf, bufLen, input, ptr - input);
                input = ++ptr;
                retVal = CmdNode;
                break;
            }
            else if (*ptr == ' ')
            {
                strncpy_s(buf, bufLen, input, ptr - input);
                input = ++ptr;
                if (!parseArgs)
                {
                    parseArgs = true;
                    retVal = CmdNode;
                }
                else
                {
                    retVal = Param;
                }
                break;
            }
            else if (*ptr == '\0' && input != ptr)
            {
                strncpy_s(buf, bufLen, input, ptr - input);
                input = ptr;
                retVal = parseArgs ? Param : CmdNode;
                break;
            }
            ++ptr;
        }

        return retVal;
    }
};