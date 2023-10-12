#ifndef _GODZILLA_PROCESS_PROCESS_H_
#define _GODZILLA_PROCESS_PROCESS_H_
#include <vector>
#include <string>
#include <memory>
#include <iostream>

#include "config.h"

#ifdef _WINDOWS_SYSTEM_
#include <Windows.h>
#include <Psapi.h>
#pragma comment(lib,"Psapi.lib")

#include <TlHelp32.h>
#endif

#include "godzilla.h"

#include "system/winsystemexp.h"
#include "memory/gprocmemory.h"

enum class ModuleType {
	ModuleList32Bit = 1,
	ModuleList64Bit = 2,
	ModuleListAll = (ModuleList32Bit | ModuleList64Bit)
};

struct ModuleInfo {
	void* dllBase;	//模块基地址
	Dword imageSize;	//模块大小
	void* entryPoint;	//模块入口地址
	std::wstring baseName;
	std::wstring path;
};

enum class ProcAccess {
#ifdef _WINDOWS_SYSTEM_
	Query = PROCESS_QUERY_INFORMATION,
	ReadMemory = PROCESS_VM_READ,
	AllAccess = PROCESS_ALL_ACCESS
#endif
};


std::vector<pid_t> EnumProcessId();

std::vector<GHandle> EnumProcessIns(ProcAccess access);

Gauge GetPebAddress(pins_t ins);


class GProcess {
public:

public:
	GProcess();
	GProcess(const GHandle& handle);
	GProcess(GHandle&& handle);
	GProcess(const GProcess& other);
	GProcess(GProcess&& other);
	~GProcess();
public:
	bool OpenProcess(pid_t pid, ProcAccess access);

	bool OpenProcess(const std::wstring& path, ProcAccess access);

	std::wstring GetPath();

	std::vector<ModuleInfo> GetAllModuleInfo(ModuleType type);

	ModuleInfo GetModuleInfo(
		const std::wstring& module_path,
		ModuleType type
	);

	Gauge GetFuncAddress(
		const std::wstring& module_path, 
		const std::string& func_name
	);
public:
	GProcMemory& Memory();
	GHandle& Handle();
private:
	GProcMemory memory_;
	GHandle handle_;
};
//#ifdef _WINDOWS_SYSTEM_
//vector<gpins_t> GetThreadList(gpins_t ins);
//#endif

#endif
