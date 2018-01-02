/*
    Plugin-SDK (Grand Theft Auto) header file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once
#include "scripting\ScriptCommandNames.h"
#include "game_vc\CRunningScript.h"
#include <vector>
#include <string.h>

class CPed;
class CVehicle;
class CObject;

namespace plugin {

class scripting {

public:

enum ScriptCommandEndParameter {
    END_PARAMETER
};

private:

enum ScriptResultVarType {
    SCRIPT_RESULT_VAR_NUMBER,
    SCRIPT_RESULT_VAR_STRING,
    SCRIPT_RESULT_VAR_PED,
    SCRIPT_RESULT_VAR_VEHICLE,
    SCRIPT_RESULT_VAR_OBJECT
};

class ScriptCode {
    unsigned char *data;
    unsigned int capacity;
    unsigned int size;

    struct VarToSet {
        unsigned int varIndex;
        void *pVar;
        ScriptResultVarType varType;

        VarToSet(unsigned int _varIndex, void *_pVar, ScriptResultVarType _varType);
    };

    std::vector<VarToSet> varsToSet;
    unsigned short varIndexCounter;

public:
    ScriptCode(short commandId = -1);
    ~ScriptCode();
    void AddParameterDescription(unsigned char paramType);
    void AddBytes(unsigned char *bytes, unsigned int count);
    unsigned char *GetData();
    void SaveResultVariables(CRunningScript *script);
    void operator<<(char n);
    void operator<<(unsigned char n);
    void operator<<(short n);
    void operator<<(unsigned short n);
    void operator<<(int n);
    void operator<<(unsigned int n);
    void operator<<(float n);
    void operator<<(double n);
    void operator<<(ScriptCommandEndParameter);
    void operator<<(char *str);
    void operator<<(const char *str);
    void operator<<(float *p);
    void operator<<(int *p);
    void operator<<(unsigned int *p);
    void operator<<(CPed *n);
    void operator<<(CVehicle *n);
    void operator<<(CObject *n);
    void operator<<(CPed **p);
    void operator<<(CVehicle **p);
    void operator<<(CObject **p);

    template <typename T>
    void Pack(T value) {
        operator<<(value);
    }

    template <typename First, typename... Rest>
    void Pack(First firstValue, Rest... rest) {
        Pack(firstValue);
        Pack(rest...);
    }

    void Pack() {}
};

public:

template<typename... ArgTypes>
static bool CallCommandById(unsigned int commandId, ArgTypes... arguments) {
    // create our 'script' object
    static CRunningScript script;
    memset(&script, 0, sizeof(CRunningScript));
    strcpy(script.m_szName, "plg-sdk");
    script.m_bWastedBustedCheck = true;
    script.m_bIsMission = false;
    script.m_bUseMissionCleanup = false;
    script.m_bNotFlag = (commandId >> 15) & 1;
    // our script code
    ScriptCode code(commandId);
    // for all arguments: add them to script code
    code.Pack(arguments...);
    script.m_nIp = reinterpret_cast<int>(code.GetData()) - reinterpret_cast<int>(CRunningScript::GetScriptSpaceBase());
    script.ProcessOneCommand();
    code.SaveResultVariables(&script);
    return script.m_bCondResult ? true : false;
}

template<typename... ArgTypes>
static bool CallCommandById(plugin::Commands commandId, ArgTypes... arguments) {
    return CallCommandById(static_cast<unsigned int>(commandId) - 0x10000, arguments...);
}

};

template <plugin::Commands CommandId, typename... ArgTypes>
bool Command(ArgTypes... arguments) {
    return scripting::CallCommandById(static_cast<unsigned int>(CommandId) - 0x10000, arguments...);
}

template <int CommandId, typename... ArgTypes>
bool Command(ArgTypes... arguments) {
    return scripting::CallCommandById(CommandId, arguments...);
}

};
