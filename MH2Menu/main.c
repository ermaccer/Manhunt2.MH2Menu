#include <pspsdk.h>
#include <pspkernel.h>
#include <pspctrl.h>
#include <pspdisplay.h>
#include <systemctrl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "pspmem.h"
#include "mh2.h"
#include "menu.h"

PSP_MODULE_INFO("MAN2M", 0, 1, 0);
PSP_MAIN_THREAD_ATTR(0);
static STMOD_HANDLER previous;
SceUID thid;
u32 mod_text_addr;

int mh2_thread()
{
	while (1)
	{
		Menu_Update();
	}
	sceKernelExitDeleteThread(0);
	return 0;
}



int Init(u32 addr) 
{
	PSPPatcher_Init(addr);
	MH2_Init(addr);
	sceKernelStartThread(thid, 0, 0);
	Menu_Init();
	if (GetGameVersion() == GAME_EUROPE)
	{
		MakeCall(0x197498, (int)Menu_Function);
		MakeCall(0x22E8B8, (int)SetPlayer);
		MakeCall(0x22ECAC, (int)SetPlayer);
		Nop(0x27ED0C);
	}
	else
	{
		MakeCall(0x197498, (int)Menu_Function);
		MakeCall(0x22E7F0, (int)SetPlayer);
		MakeCall(0x22EBE4, (int)SetPlayer);
		Nop(0x27EBF8);
	}



	return 0;
}

int OnModuleStart(SceModule2 *mod) {
	char *modname = mod->modname;

	if (strcmp(modname, "MAN2") == 0) {
		mod_text_addr = mod->text_addr;

		int ret = Init(mod_text_addr);
		if (ret != 0)
			return -1;

		sceKernelDcacheWritebackAll();
	}
	if (!previous)
		return 0;

	return previous(mod);
}


int module_start(SceSize argc, void* argp) {

	thid = sceKernelCreateThread("MAN2MENU", mh2_thread, 0x18, 2048, 0, NULL);
	if (sceIoDevctl("kemulator:", 0x00000003, NULL, 0, NULL, 0) == 0) { 
		SceUID modules[10];
		int count = 0;
		if (sceKernelGetModuleIdList(modules, sizeof(modules), &count) >= 0) {
			int i;
			SceKernelModuleInfo info;
			for (i = 0; i < count; ++i) {
				info.size = sizeof(SceKernelModuleInfo);
				if (sceKernelQueryModuleInfo(modules[i], &info) < 0) {
					continue;
				}
				if (strcmp(info.name, "MAN2") == 0) {

					mod_text_addr = info.text_addr;

					int ret = Init(mod_text_addr);
					if (ret != 0)
						return;

					sceKernelDcacheWritebackAll();
					return;
				}
			}
		}
	}
	else {
		previous = sctrlHENSetStartModuleHandler(OnModuleStart); // PSP
	}


	return 0;
}
