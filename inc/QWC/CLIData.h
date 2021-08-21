#pragma once
#include <cstdint>

constexpr std::uintptr_t CLISingleton = 0x1060114;
constexpr std::uintptr_t SingletonInit = 0x1060120;

struct CommandTable;
struct TopCmd;
struct SubCmd;
class CliRegParam;

struct CommandTable
{
    uint32_t pad[3];    // 00
    TopCmd* head;       // 0C
};

struct TopCmd
{
    char* cmdName;      // 00
    int32_t unk04;      // 04
    void* parentPtr;    // 08
    SubCmd* head;       // 0C
    TopCmd* next;       // 10
    int32_t unk14;      // 14
    int32_t unk18;      // 18
    int32_t unk1c;      // 1c

    SubCmd* GetSubCmd(const char* cmdStr);

};
//static_assert(sizeof(TopCmd) == 0x20);

struct SubCmd
{
    char* cmdName;      // 00
    CliRegParam* args;  // 04
    TopCmd* parent;     // 08
    int32_t unk0c;      // 0C
    SubCmd* next;       // 10
    void* optVar;     // 14
    void* func;       // 18
    int32_t unk1c;      // 1C
};
//static_assert(sizeof(SubCmd) == 0x20);

class CliRegParam  // Has children types
{
public:
    enum ParamType
    {
        Hex = 0,
        Decimal = 1,
        Boolean = 2,
        AlphaNum = 3,   //?
        ObjName = 4,    //?  Needs to be quoted
        String = 5      //?
    };


    virtual ~CliRegParam();                                                  // 00
    virtual bool IsValidParam(const char** paramStr, void* unk);             // 01

    SubCmd* parent;    // 04
    char* argName;      // 08
    ParamType type;     // 0C
    CliRegParam* next;  // 10
    int32_t param3;     // some boolean?
    void* unk18;      // 18
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
//static_assert(sizeof(CliRegParam) == 0x20);

class CLI
{
public:
    std::uint32_t unk04;         // 04
    CommandTable* commandTable;     // 08

    virtual void Unk00 (std::uintptr_t param1);                                                                                         // 00
    virtual void AddTopLevelCmd (char* cmdName, std::uint32_t arg2, std::uint32_t arg3);                                                // 01
    virtual void AddSubCmd (TopCmd* top, char* cmdName, std::uint32_t arg2, std::uint32_t arg3);                                        // 02
    virtual void AddSubCmdWithVar(TopCmd* top, char* cmdName, std::uintptr_t val, std::uint32_t arg3, std::uint32_t arg4);              // 03
    virtual void AddCliRegParam(SubCmd* sub, char* cmdName, CliRegParam::ParamType type, std::uint32_t arg3);                           // 04
    virtual void Unk05(std::uintptr_t param1, std::uintptr_t param2);                                                                   // 05
    virtual void Unk06(std::uintptr_t param1);                                                                                          // 06
    virtual bool ExecCmd(const char* cmdStr, std::uintptr_t param2 = 0x5b880c);                                                         // 07

    TopCmd* GetTopCmd(const char* cmdStr);
    bool IsValidCommand(const char* cmdStr);
};
//static_assert(sizeof(CLI) == 0x20);
