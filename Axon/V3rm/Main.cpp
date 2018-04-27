
#include "globals.h"
#include "Bridge.h"
#include <string>
#include <iostream>
#include <stdlib.h>
#include <vector>
#include <iterator>
#include <fstream>
#include <intrin.h>
#include <Tlhelp32.h>
#include <CommCtrl.h>
#include <Wininet.h>
#pragma comment(lib, "wininet.lib")
using namespace std;


DWORD ScriptContext;
DWORD ScriptContextVFTable = x(0x110D470);

DWORD grabGlobalStateIndex(DWORD ScriptContext, int idx)
{
	DWORD* context = reinterpret_cast<DWORD*>(ScriptContext);
	return context[idx] + (int)&context[idx];
}

using Bridge::m_rL;
using Bridge::m_L;
#define r_setidentity(rL, i) DWORD* identity  = (DWORD*)(rL - 8); *identity ^= (i ^ (unsigned __int8)*identity) & 0x1F;





void PushGlobal(DWORD rL, lua_State* L, const char* s)
{
	r_lua_getglobal(rL, s);
	Bridge::push(rL, L, -1);
	lua_setglobal(L, s);
	r_lua_pop(rL, 1);
}


int collectmemory(lua_State* L)
{
cout << LUA_GCCOUNT;
return 0;
}


DWORD WINAPI input(PVOID lvpParameter)
{
	string WholeScript = "";
	HANDLE hPipe;
	char buffer[999999];
	DWORD dwRead;
	hPipe = CreateNamedPipe(TEXT("\\\\.\\pipe\\Axon"),
		PIPE_ACCESS_DUPLEX | PIPE_TYPE_BYTE | PIPE_READMODE_BYTE,
		PIPE_WAIT,
		1,
		999999,
		999999,
		NMPWAIT_USE_DEFAULT_WAIT,
		NULL);
	while (hPipe != INVALID_HANDLE_VALUE)
	{
		if (ConnectNamedPipe(hPipe, NULL) != FALSE)
		{
			while (ReadFile(hPipe, buffer, sizeof(buffer) - 1, &dwRead, NULL) != FALSE)
			{
				buffer[dwRead] = '\0';
				try {
					try {
						WholeScript = WholeScript + buffer;
					}
					catch (...) {
					}
				}
				catch (std::exception e) {

				}
				catch (...) {

				}
			}
			if (luaL_loadstring(m_L, WholeScript.c_str()))
				printf("Error: %s\n", lua_tostring(m_L, -1));
			else
				lua_pcall(m_L, 0, 0, 0);

			WholeScript = "";
		}
		DisconnectNamedPipe(hPipe);
	}
}


bool CompareData(const char* Data, const char* Pattern, const char* Mask) {
	while (*Mask) {
		if (*Mask != '?') {
			if (*Data != *Pattern) {
				return false;
			};
		};
		++Mask;
		++Data;
		++Pattern;
	};
	return true;
};


DWORD ScanForScriptContext(const char* SCVFT_Offsetted) {
	MEMORY_BASIC_INFORMATION BasicMemoryInformation = {};
	SYSTEM_INFO SystemInformation = {};
	GetSystemInfo(&SystemInformation);
	DWORD StartingMemorySearchPosition = (DWORD)SystemInformation.lpMinimumApplicationAddress;
	DWORD MaximumSearchBoundary = (DWORD)SystemInformation.lpMaximumApplicationAddress;
	do {
		while (VirtualQuery((void*)StartingMemorySearchPosition, &BasicMemoryInformation, sizeof(BasicMemoryInformation))) {
			if ((BasicMemoryInformation.Protect & PAGE_READWRITE) && !(BasicMemoryInformation.Protect & PAGE_GUARD)) {
				for (DWORD Key = (DWORD)(BasicMemoryInformation.BaseAddress); ((Key - (DWORD)(BasicMemoryInformation.BaseAddress) < BasicMemoryInformation.RegionSize)); ++Key) {
					if (CompareData((const char*)Key, SCVFT_Offsetted, "xxxx")) {
						return Key;
					};
				};
			};
			StartingMemorySearchPosition += BasicMemoryInformation.RegionSize;
		};
	} while (StartingMemorySearchPosition < MaximumSearchBoundary);
	return 0x0;
};
#include <signal.h>







//----------------------------------------\\
//credits to louka for consolebypass
BOOL WINAPI CONSOLEBYPASS()
{
	DWORD nOldProtect;
	if (!VirtualProtect(FreeConsole, 1, PAGE_EXECUTE_READWRITE, &nOldProtect))
		return FALSE;
	*(BYTE*)(FreeConsole) = 0xC3;
	if (!VirtualProtect(FreeConsole, 1, nOldProtect, &nOldProtect))
		return FALSE;	
	AllocConsole();
}


using namespace std;
std::string replace_all(std::string subject, const std::string& search, const std::string& replace) {
	size_t pos = 0;
	while ((pos = subject.find(search, pos)) != std::string::npos) {
		subject.replace(pos, search.length(), replace);
		pos += replace.length();
	}
	return subject;
}

inline std::string download_url(const std::string &url) {
	HINTERNET interwebs = InternetOpenA("Mozilla/5.0", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, NULL);
	std::string rtn;

	if (interwebs) {
		HINTERNET url_file = InternetOpenUrlA(interwebs, url.c_str(), NULL, NULL, NULL, NULL);

		if (url_file) {
			char buffer[2000];
			DWORD bytes_read;

			do {
				InternetReadFile(url_file, buffer, 2000, &bytes_read);
				rtn.append(buffer, bytes_read);
				memset(buffer, 0, 2000);
			} while (bytes_read);

			InternetCloseHandle(interwebs);
			InternetCloseHandle(url_file);
			std::string p = replace_all(rtn, "|n", "\r\n");
			return p;
		}
	}

	InternetCloseHandle(interwebs);
	std::string p = replace_all(rtn, "|n", "\r\n");
	return p;
}

static bool auth_success = false;


DWORD authorize() {



	std::string rvxhost = "http://openholo.net/trial.php";


	std::string trial_response = "AEROISGAY";
	std::string wlurl = rvxhost;
	std::string content = download_url(wlurl);



	if (content == trial_response) 
	{
		cout << "Welcome, to RVX-4 trial\n";
		auth_success = true;
	}
	else {
		cout << "hi owner\n";
		auth_success = true;
		//Sleep(2500);
		//exit(0);
	}
	return 1;


	
	
}

int getRawMetaTable(lua_State *L) {
	Bridge::push(L, m_rL, 1);

	if (r_lua_getmetatable(m_rL, -1) == 0) {
		lua_pushnil(L);
		return 0;
	}
	Bridge::push(m_rL, L, -1);

	return 1;
}

static int UserDataGC(lua_State *Thread) {
	void *UD = lua_touserdata(Thread, 1);
	if (m_rL) {
		MessageBoxA(NULL, "GET REFERENCE", "Error", MB_OK);

		r_lua_rawgeti(m_rL, LUA_REGISTRYINDEX, (int)UD);
		if (r_lua_type(m_rL, -1) <= R_LUA_TNIL) {
			lua_pushnil(Thread);
			lua_rawseti(Thread, LUA_REGISTRYINDEX, (int)UD);
			MessageBoxA(NULL, "GOTz THREAD SET", "Error", MB_OK);

		}
	}
	return 0;
}
void main()
{
	/*CONSOLEBYPASS();
	freopen("CONOUT$", "w", stdout);
	freopen("CONIN$", "r", stdin);
	HWND ConsoleHandle = GetConsoleWindow();
	SetWindowPos(ConsoleHandle, HWND_TOPMOST, 50, 20, 0, 0, SWP_DRAWFRAME | SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
	ShowWindow(ConsoleHandle, 1);
	SetConsoleTitle("RVX-4 - LEVEL 6 DEBUG CONSOLE.");*/
	/*MessageBoxA(NULL, "hostthingie", "hostthingie", NULL);
	char HostsPath[260];
	GetSystemDirectory(HostsPath, MAX_PATH);
	lstrcat(HostsPath, "\\drivers\\etc\\HOSTS");
	ifstream inputt;

	size_t pos;
	string line;

	inputt.open(HostsPath);
	if (inputt.is_open())
	{
		while (getline(inputt, line))
		{
			pos = line.find("openholo.net");
			if (pos != string::npos)
			{
				cout << "NICE TRY LMAO????\n";
				Sleep(2500);
				exit(0);
				break;
			}
		}
	}

	MessageBoxA(NULL, "hostthingie passed", "hostthingie", NULL);*/

	//MessageBoxA(NULL, "ScriptContext", "ScriptContext", NULL);
	printf("ScriptContext\r\n");
	ScriptContext = ScanForScriptContext((char*)&ScriptContextVFTable);
	if (!ScriptContext)
	{
		MessageBoxA(NULL, "Scan failed ScriptContextVirtualTable", "Error", MB_OK);
	}
	//MessageBoxA(NULL, "ScriptContext passed", "ScriptContext passed", NULL);
	printf("ScriptContext passed\r\n");
	printf("globalindex\r\n");
	//MessageBoxA(NULL, "globalindex", "globalindex", NULL);
	m_rL = grabGlobalStateIndex(ScriptContext, 41);

	if (!m_rL)
	{
		MessageBoxA(NULL, "Scan failed Globalstateindex", "Error", MB_OK);
	}
	//MessageBoxA(NULL, "globalindex passed", "globalindex passed", NULL);
	printf("globalindex passed\r\n");
	printf("newstate\r\n");
	//MessageBoxA(NULL, "newstate", "newstate", NULL);
	m_L = luaL_newstate();
	//MessageBoxA(NULL, "newstate passed", "newstate passed", NULL);
	printf("newstate passed\r\n");
	printf("identity\r\n");
	//MessageBoxA(NULL, "identity", "identity", NULL);
	r_setidentity(m_rL, 6);
	//MessageBoxA(NULL, "identity passed", "identity passed", NULL);
	printf("identity passed\r\n");
	printf("vehhand\r\n");
	//MessageBoxA(NULL, "vehhand", "vehhand", NULL);
	Bridge::VehHandlerpush();
	//MessageBoxA(NULL, "vehhand passed", "vehhand passed", NULL);
	printf("vehhand passed\r\n");
	printf("openlibs\r\n");
	//MessageBoxA(NULL, "openlibs", "openlibs", NULL);

	luaL_openlibs(m_L);
	//MessageBoxA(NULL, "openlibs passed", "openlibs passed", NULL);
	printf("openlibs passed\r\n");
	
	/*if (!ADDRESS_GETFIELD)
	{
		MessageBoxA(NULL, "Scan failed lua getfield", "Error", MB_OK);
	}
	if (!ADDRESS_SETFIELD)
	{
		MessageBoxA(NULL, "Scan failed lua setfield", "Error", MB_OK);
	}
	if (!ADDRESS_TYPE)
	{
		MessageBoxA(NULL, "Scan failed lua type", "Error", MB_OK);
	}
	if (!ADDRESS_PCALL)
	{
		MessageBoxA(NULL, "Scan failed lua pcall", "Error", MB_OK);
	}
	if (!ADDRESS_TONUMBER)
	{
		MessageBoxA(NULL, "Scan failed lua tonumber", "Error", MB_OK);
	}
	if (!ADDRESS_TOUSERDATA)
	{
		MessageBoxA(NULL, "Scan failed lua touserdata", "Error", MB_OK);
	}

	if (!ADDRESS_NEWUSERDATA)
	{
		MessageBoxA(NULL, "Scan failed lua newuserdata", "Error", MB_OK);
	}
	if (!ADDRESS_GETMETATABLE)
	{
		MessageBoxA(NULL, "Scan failed lua getmetatable", "Error", MB_OK);
	}
	if (!ADDRESS_PUSHNIL)
	{
		MessageBoxA(NULL, "Scan failed lua pushnil", "Error", MB_OK);
	}
	if (!ADDRESS_PUSHLIGHTUSERDATA)
	{
		MessageBoxA(NULL, "Scan failed lua pushlightuserdata", "Error", MB_OK);
	}
	if (!ADDRESS_PUSHCCLOSURE)
	{
		MessageBoxA(NULL, "Scan failed lua pushcclosure", "Error", MB_OK);
	}
	if (!ADDRESS_SETTOP)
	{
		MessageBoxA(NULL, "Scan failed lua settop", "Error", MB_OK);
	}*/
	
	printf("initialziing shit\r\n");
	//MessageBoxA(NULL, "initialziing shit", "initialziing shit", NULL);
	luaL_newmetatable(m_L, "garbagecollector");  
	printf("success 1\r\n");
	//MessageBoxA(NULL, "success 1", "success 1", NULL);
	lua_pushcfunction(m_L, UserDataGC); 
	printf("success 2\r\n");
	//MessageBoxA(NULL, "success 2", "success 2", NULL);
	lua_setfield(m_L, -2, "__gc");    
	printf("success 3\r\n");
	//MessageBoxA(NULL, "success 3", "success 4", NULL);

	lua_pushvalue(m_L, -1);
	printf("success 4\r\n");
	//MessageBoxA(NULL, "success 4", "success 4", NULL);
	lua_setfield(m_L, -2, "__index");
	printf("success 5\r\n");
	//MessageBoxA(NULL, "success 5", "success 5", NULL);


	printf("does pushglobal work?\r\n");
	//MessageBoxA(NULL, "does pushglobal work?", "question  of the day", NULL);
	PushGlobal(m_rL, m_L, "printidentity");
	PushGlobal(m_rL, m_L, "game");
	PushGlobal(m_rL, m_L, "Game");
	PushGlobal(m_rL, m_L, "workspace");
	PushGlobal(m_rL, m_L, "Workspace");
	printf("yep they do\r\n");
	//MessageBoxA(NULL, "yep they do", "yes indeed", NULL);

	PushGlobal(m_rL, m_L, "Axes");
	PushGlobal(m_rL, m_L, "BrickColor");
	PushGlobal(m_rL, m_L, "CFrame");
	PushGlobal(m_rL, m_L, "Color3");
	PushGlobal(m_rL, m_L, "ColorSequence");
	PushGlobal(m_rL, m_L, "ColorSequenceKeypoint");
	PushGlobal(m_rL, m_L, "NumberRange");
	PushGlobal(m_rL, m_L, "NumberSequence");
	PushGlobal(m_rL, m_L, "NumberSequenceKeypoint");
	PushGlobal(m_rL, m_L, "PhysicalProperties");
	PushGlobal(m_rL, m_L, "Ray");
	PushGlobal(m_rL, m_L, "Rect");
	PushGlobal(m_rL, m_L, "Region3");
	PushGlobal(m_rL, m_L, "Region3int16");
	PushGlobal(m_rL, m_L, "TweenInfo");
	PushGlobal(m_rL, m_L, "UDim");
	PushGlobal(m_rL, m_L, "UDim2");
	PushGlobal(m_rL, m_L, "Vector2");
	PushGlobal(m_rL, m_L, "Vector2int16");
	PushGlobal(m_rL, m_L, "Vector3");
	PushGlobal(m_rL, m_L, "Vector3int16");
	PushGlobal(m_rL, m_L, "Enum");
	PushGlobal(m_rL, m_L, "Faces");
	PushGlobal(m_rL, m_L, "Instance");
	PushGlobal(m_rL, m_L, "math");
	PushGlobal(m_rL, m_L, "warn");
	PushGlobal(m_rL, m_L, "typeof");
	PushGlobal(m_rL, m_L, "type");
	PushGlobal(m_rL, m_L, "spawn");
	PushGlobal(m_rL, m_L, "Spawn");
	PushGlobal(m_rL, m_L, "print");
	PushGlobal(m_rL, m_L, "printidentity");
	PushGlobal(m_rL, m_L, "ypcall");
	PushGlobal(m_rL, m_L, "Wait");
	PushGlobal(m_rL, m_L, "wait");
	PushGlobal(m_rL, m_L, "delay");
	PushGlobal(m_rL, m_L, "Delay");
	PushGlobal(m_rL, m_L, "tick");
	PushGlobal(m_rL, m_L, "LoadLibrary");
	lua_register(m_L, "getrawmetatable", getRawMetaTable);



	lua_newtable(m_L);
	lua_setglobal(m_L, "_G");
	//MessageBoxA(NULL, "DONE SHIT", "DONE SHIT", NULL);
	printf("DONE SHIT\r\n");


	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)input, NULL, NULL, NULL);


	printf("RVX INJECTED!\n");
	}



BOOL APIENTRY DllMain(HMODULE Module, DWORD Reason, void* Reserved)
{
	switch (Reason)
	{
	case DLL_PROCESS_ATTACH:
		DisableThreadLibraryCalls(Module);
		CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)main, NULL, NULL, NULL);
		break;
	case DLL_PROCESS_DETACH:
		break;
	default: break;
	}

	return TRUE;
}