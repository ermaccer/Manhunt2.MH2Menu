#include <pspsdk.h>
#include <pspkernel.h>

#include <stdio.h>
#include <string.h>

#include <systemctrl.h>

#include "pspmem.h"

PSP_MODULE_INFO("MH2Menu", 0x1007, 1, 0);


void Init(unsigned int addr)
{
    
}


int module_start(SceSize args, void *argp) {
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

				if (strcmp(info.name, "MAN2") == 0)
				{
					//_sw(0, info.text_addr + );

					Init(info.text_addr);
				}
			}
		}
	}
	return 0;
}