#include <pspsdk.h>
#include <pspkernel.h>
#include <pspctrl.h>
#include <pspdisplay.h>
#include <systemctrl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define EMULATOR_DEVCTL__IS_EMULATOR 0x00000003

PSP_MODULE_INFO("MAN2M", 0, 1, 0);
static STMOD_HANDLER previous;

u32 mod_text_addr;
//register int gp asm("gp");

char buffer[256]; //string buffer




#define MAKE_CALL(a, f) _sw(0x0C000000 | (((u32)(f) >> 2) & 0x03FFFFFF), a);

#define MAKE_JUMP(a, f) _sw(0x08000000 | (((u32)(f) & 0x0FFFFFFC) >> 2), a);

#define MAKE_DUMMY_FUNCTION(a, r) { \
  u32 _func_ = a; \
  if (r == 0) { \
    _sw(0x03E00008, _func_); \
    _sw(0x00001021, _func_ + 4); \
  } else { \
    _sw(0x03E00008, _func_); \
    _sw(0x24020000 | r, _func_ + 4); \
  } \
}

#define REDIRECT_FUNCTION(a, f) { \
  u32 _func_ = a; \
  _sw(0x08000000 | (((u32)(f) >> 2) & 0x03FFFFFF), _func_); \
  _sw(0, _func_ + 4); \
}

#define HIJACK_FUNCTION(a, f, ptr) { \
  u32 _func_ = a; \
  static u32 patch_buffer[3]; \
  _sw(_lw(_func_), (u32)patch_buffer); \
  _sw(_lw(_func_ + 4), (u32)patch_buffer + 8);\
  MAKE_JUMP((u32)patch_buffer + 4, _func_ + 8); \
  _sw(0x08000000 | (((u32)(f) >> 2) & 0x03FFFFFF), _func_); \
  _sw(0, _func_ + 4); \
  ptr = (void *)patch_buffer; \
}

void(*CFrontend_PrintInfo)(int lineId, char* format, ...);


void Menu_Function()
{
	CFrontend_PrintInfo(4, "test");
	CFrontend_PrintInfo(4, "testv2");
}


int patch(u32 text_addr) {
	CFrontend_PrintInfo = (void*)(text_addr + 0x195BD0);
	MAKE_CALL(text_addr + 0x00197498, Menu_Function); //

	return 0;
}


static void CheckModules() { // PPSSPP only
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

				int ret = patch(mod_text_addr);
				if (ret != 0) //patching returned error
					return;

				sceKernelDcacheWritebackAll();
				return;
			}
		}
	}
}

int OnModuleStart(SceModule2 *mod) {
	char *modname = mod->modname;

	if (strcmp(modname, "MAN2") == 0) {
		mod_text_addr = mod->text_addr;

		int ret = patch(mod_text_addr);
		if (ret != 0)
			return -1;

		sceKernelDcacheWritebackAll();
	}
	if (!previous)
		return 0;

	return previous(mod);
}


int module_start(SceSize argc, void* argp) {

	if (sceIoDevctl("kemulator:", EMULATOR_DEVCTL__IS_EMULATOR, NULL, 0, NULL, 0) == 0) { // PPSSPP
		CheckModules(); // scan the modules using normal/official syscalls. 
	}
	else {
		previous = sctrlHENSetStartModuleHandler(OnModuleStart); // PSP
	}

	return 0;
}
