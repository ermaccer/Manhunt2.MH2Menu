#include "mh2.h"
#include "pspmem.h"
#include "menu.h"


int CScene_FindPlayer()
{
	return ReadInt(0x31FF9C);
}

int CScene_FindCamera()
{
	return ReadInt(0x31FF94);
}

int GetRwSkel()
{
	return ReadInt(0x31FF9C);
}

void CEntity_SetFlag(int ptr, int flag, int status)
{
	int newFlags = 0;
	if (status)
		newFlags = *(int*)(ptr + 160) | flag;
	else
		newFlags = *(int*)(ptr + 160) & ~flag;

	*(int*)(ptr + 160) = newFlags;
}

void MH2_Init(int addr)
{
	CFrontend_PrintInfo = (void *)(addr + 0x195BD0);
	CFrontend_Print8 = (void *)(addr + 0x195C40);
	CFrontend_SetDrawRGBA = (void *)(addr + 0x1957E8);
	CRenderer_SetIngameInfoRenderStates = (void *)(addr + 0x284794);
	CGameInfo_Render = (void*)(addr + 0x1A08E0);
	CCharacter_CreateInventoryItem = (void*)(addr + 0x1382D8);
	CCollectable_GetNameStringFromType = (void*)(addr + 0x1784A4);
	CEntityManager_GetEntityTypeDataFromName = (void*)(addr + 0x12B41C);
	CRenderer_DrawQuad2d = (void*)(addr + 0x1A1010);
	CPlayer_IsExecuting = (void*)(addr + 0x1F4BF0);
	CInventory_FindItemWithCollectableType = (void*)(addr + 0x1C7384);
	CAmmoWeapon_SetAmmo = (void*)(addr + 0x2776F4);
	CInputManager_EventHandler = (void*)(addr + 0x1C3DC0);
	RwSkel_NormaliseStickValues= (void*)(addr + 0x11BEB0);
	RwSkel_PadButtonDown = (void*)(addr + 0x11B994);
	CFileNames_setPlayer = (void*)(addr + 0x192C1C);
	CPed_ChangePedHead = (void*)(addr + 0x13B9FC);
	CWeather_ForceWeather = (void*)(addr + 0x266320);
	CPed_SetPos = (void*)(addr + 0x169440);
}


