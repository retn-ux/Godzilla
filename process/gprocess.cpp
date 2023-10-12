#include "gprocess.h"


std::vector<pid_t> EnumProcessId() {
	const uint32_t ENTRY_SIZE = 0x1000;
	std::vector<pid_t> result;
	result.resize(ENTRY_SIZE);
	Dword returnSize = 0;
#ifdef _WINDOWS_SYSTEM_
	if (!EnumProcesses(
		result.data(),
		result.size() * sizeof(DWORD),
		&returnSize
	)) {
		result.clear();
		return result;
	}
#endif
	result.resize(returnSize);
	return std::move(result);
}

std::vector<GHandle> EnumProcessIns(ProcAccess access) {
	std::vector<GHandle> result;
	auto pid_list = EnumProcessId();
	if (pid_list.empty())
		return std::move(result);
#ifdef _WINDOWS_SYSTEM_
	for (auto& entry : pid_list) {
		GHandle ins = OpenProcess(
			static_cast<Dword>(access),
			false,
			entry
		);
		result.push_back(std::move(ins));
	}
#endif
	return std::move(result);
}


GProcess::GProcess() {}

GProcess::GProcess(const GHandle& handle):
	memory_(handle),handle_(handle) {}

GProcess::GProcess(GHandle&& handle):
	memory_(std::forward<GHandle>(handle)),
	handle_(std::forward<GHandle>(handle)) 
{}

GProcess::GProcess(const GProcess& other):
	memory_(other.memory_),
	handle_(other.handle_)
{}

GProcess::GProcess(GProcess&& other) :
	memory_(std::forward<GProcMemory>(other.memory_)),
	handle_(std::forward<GHandle>(other.handle_))
{}

GProcess::~GProcess() {}



GProcMemory& GProcess::Memory() {
	return memory_;
}

GHandle& GProcess::Handle() {
	return handle_;
}


bool GProcess::OpenProcess(pid_t pid, ProcAccess access) {
#ifdef _WINDOWS_SYSTEM_
	handle_ = ::OpenProcess(
		static_cast<DWORD>(access),
		false,
		pid);
#endif
	if (handle_.IsInvalid())return false;
	return true;
}

std::wstring GProcess::GetPath() {
	std::wstring result;
	if (handle_.IsInvalid())
		return result;
#ifdef _WINDOWS_SYSTEM_
	wchar_t path[MAX_PATH];
	memset(path, 0, MAX_PATH);
	if (!GetModuleFileNameExW(handle_.GetElement(), nullptr, path, MAX_PATH))
	{
		auto a = GetLastError();
		a = 0;
	}
	result = path;
#endif
	return std::move(result);
}


bool GProcess::OpenProcess(const std::wstring& path, ProcAccess access) {
	if (path.empty()) {
		return false;
	}
	auto ids = EnumProcessId();
	if (ids.empty())
		return false;
	bool IsInit = false;
	for (auto& id : ids) {
#ifdef _WINDOWS_SYSTEM_
		handle_=::OpenProcess(
			static_cast<Dword>(access), 
			false, 
			id)
		;
		if (handle_.IsInvalid())continue;
		auto proc_path = GetPath();
		if (path == proc_path) {
			IsInit = true;
			break;
		}
		else {
			handle_ = nullptr;
		}
		
#endif
	}
	if (!IsInit)handle_ = nullptr;
	if (handle_.IsInvalid()) return false;
	return true;
}

std::vector<ModuleInfo> GProcess::GetAllModuleInfo(ModuleType type)
{
	std::vector<ModuleInfo> result;
	ModuleInfo module_info;

	if (handle_.IsInvalid())
		return std::move(result);

#ifdef _WINDOWS_SYSTEM_
	DWORD lpcbNeeded = 0;
	if (!EnumProcessModulesEx(
		handle_.GetElement(),
		nullptr,
		0,
		&lpcbNeeded,
		static_cast<DWORD>(type)
	))
		return result;

	std::vector<HMODULE> module_list(lpcbNeeded / sizeof(HMODULE));
	if (!EnumProcessModulesEx(
		handle_.GetElement(),
		module_list.data(),
		module_list.size() * sizeof(HMODULE),
		&lpcbNeeded,
		static_cast<DWORD>(type)
	))
		return result;

	MODULEINFO mi;
	wchar_t name[MAX_PATH];
	for (auto& item : module_list) {
		if (item == 0)
			continue;

		if (GetModuleInformation(handle_.GetElement(), item, &mi, sizeof(mi))) {
			module_info.dllBase = mi.lpBaseOfDll;
			module_info.imageSize = mi.SizeOfImage;
			module_info.entryPoint = mi.EntryPoint;
		}

		if (GetModuleBaseNameW(handle_.GetElement(), item, name, MAX_PATH)) {
			module_info.baseName = name;
		}

		if (GetModuleFileNameExW(handle_.GetElement(), item, name, MAX_PATH)) {
			module_info.path = name;
		}
		result.push_back(module_info);
	}
#endif

	return std::move(result);

}

ModuleInfo GProcess::GetModuleInfo(
	const std::wstring& module_path,
	ModuleType type
) {
	ModuleInfo result;
	memset(&result, 0, sizeof(result));
	if (handle_.IsInvalid())
		return result;

	auto module_list = GetAllModuleInfo(type);

	auto it = std::find_if(
		module_list.begin(),
		module_list.end(),
		[module_path](const ModuleInfo& info) {
			return info.path == module_path;
		}
	);

	if (it == module_list.end())
		return result;

	result = *it;
	return result;

}

Gauge GProcess::GetFuncAddress(
	const std::wstring& module_path,
	const std::string& func_name
) {
	if (handle_.IsInvalid()) return 0;
	Gauge addr = 0;
#ifdef _WINDOWS_SYSTEM_
	HMODULE module = GetModuleHandleW(module_path.c_str());
	if (module == 0) {
		module = LoadLibraryW(module_path.c_str());
		if (module == 0)return 0;
	}
	if (handle_.GetElement() == GetCurrentProcess()) {
		addr = (Gauge)::GetProcAddress(
			module,
			func_name.c_str()
		);
		return addr;
	}
	addr = (Gauge)::GetProcAddress(
		module,
		func_name.c_str()
	);
	Gauge relav_addr = addr - (Gauge)module;
	ModuleInfo info = GetModuleInfo(
		module_path,
		ModuleType::ModuleListAll);
	if (info.dllBase == nullptr)return 0;

	return GaugeCast(info.dllBase) + relav_addr;
#endif
}

//Gauge GetPebAddress(GProcessIns ins) {
//#ifdef _WINDOWS_SYSTEM_
//
//	if (ins == GetCurrentProcess()) {
//		return GAUGE_CAST(NtCurrentTeb()->teb.ProcessEnvironmentBlock);
//	}
//	PROCESS_BASIC_INFORMATION pbi;
//	ulong read_len;
//	if (NtQueryInformationProcess(
//		ins,
//		ProcessBasicInformation,
//		&pbi,
//		sizeof(PROCESS_BASIC_INFORMATION),
//		&read_len
//	)
//		)
//		return 0;
//
//	return pbi.PebBaseAddress;
//#endif
//}
