#include "menu.h"
#include <pspctrl.h>
#include <pspsdk.h>
#include <pspkernel.h>
#include <stdio.h>
#include "mh2.h"
#include "pspmem.h"
#include <pspkernel.h>

#define TRUE_FALSE(x) x ? "true" : "false"

struct Menu TheMenu;
SceCtrlData pad;

struct menu_wep {
	int id;
	char* name;
};

struct menu_wep weapons[] = {
	{CT_6SHOOTER},
	{CT_BAG},
	{CT_BOTTLE},
	{CT_BRICK_HALF},
	{CT_CAN},
	{CT_CIRCULAR_SAW},
	{CT_COLT_COMMANDO},
	{CT_CROSSBOW},
	{CT_CROWBAR},
	{CT_DESERT_EAGLE},
	{CT_FIRE_AXE},
	{CT_FLAREGUN},
	{CT_GLOCK},
	{CT_GOLFBALL},
	{CT_HACKSAW},
	{CT_HEDGETRIMMER},
	{CT_KATANA},
	{CT_NIGHTSTICK},
	{CT_PEN},
	{CT_PLIERS},
	{CT_SAWNOFF},
	{CT_SHARD},
	{CT_SHOTGUN},
	{CT_SHOTGUN_TORCH},
	{CT_SHOVEL},
	{CT_SICKLE},
	{CT_SLEDGEHAMMER},
	{CT_SNIPER_RIFLE},
	{CT_STUNPROD},
	{CT_SYRINGE},
	{CT_TORCH},
	{CT_TRANQ_RIFLE},
	{CT_UZI},
	{CT_W_BASEBALL_BAT},
	{CT_WIRE},
};


struct menu_entry {
	char* name;
	int* toggle;
	void (*func)();
	char* tip;
};


int m_nCurrentPos = 0;
int m_nCurrentMenuOpen = -1;
int m_nCurrentWeaponPage;

static char buffer[512] = {};
char* newPlayer;
// menu

int m_bDisplayCoords = 0;
int m_bGodMode = 0;
int m_bChangePlayerSkin = 0;
int m_bFreeCamera = 0;
int m_bLockCamera = 0;
int m_bFreezeWorld = 0;
int m_bInfiniteAmmo = 0;
int m_bDisplayBodyCount = 0;
int m_bEnableBodyCount = 0;
int m_nWantToSpawnWeapon = -1;
int m_bWantToTeleport = 0;

char* m_szMenuOptions[TOTAL_MENUS] = {
	"Weapons",
	"Player",
	"Set Player Model",
	"Weather",
	"HUD",
	"Misc.",
#ifdef DEBUG
	"Debug",
#endif //  DEBUG


};

void CameraTeleport()
{
	m_bWantToTeleport = 1;
}

char* menuSkins[] = {
	"Player_Bod-BROKEN",
	"Danny_Ingame-DAsy",
	"Danny_Ingame-DBkn",
	"Danny_Ingame-DSfe",
	"Leo_Ingame-LAsy",
	"Leo_Ingame-LCas",
	"Player_Bod-LeoC",
};

struct menu_entry PlayerOptions[] =
{
	{"Display Coordinates", &m_bDisplayCoords, NULL},
	{"God Mode", &m_bGodMode, NULL},
	{"Change Skin",&m_bChangePlayerSkin, NULL},
};

struct menu_entry WeatherOptions[] =
{
	{"Free",NULL, SetWeatherFree},
	{"Clear",NULL, SetWeatherClear},
	{"Cloudy",NULL, SetWeatherCloudy},
	{"Thunder",NULL, SetWeatherThunder},
	{"Rainy",NULL, SetWeatherRainy},
	{"Foggy",NULL, SetWeatherFoggy},
	{"Windy",NULL, SetWeatherWindy},
};


struct menu_entry MiscOptions[] =
{
	{"Free Camera", &m_bFreeCamera, NULL},
	{"Lock Camera", &m_bLockCamera, NULL},
	//{"Infinite Ammo",&m_bInfiniteAmmo, NULL},
};
struct menu_entry HudOptions[] =
{
	{"Body Count", &m_bDisplayBodyCount, NULL},

};

struct Menu GetMenu()
{
	return TheMenu;
}

void Menu_Init()
{
	for (int i = 0; i < sizeof(weapons) / sizeof(weapons[0]); i++)
	{
		weapons[i].name = GetWeapName(weapons[i].id);
	}
}

void Menu_Draw()
{
	if (m_bDisplayCoords)
	{

		int plr = CScene_FindPlayer();
		if (plr)
		{
			TheMenu.plrPos = *(CVector*)(**(int**)(plr + 228) + 112);

			sprintf(buffer, "X:%.3f Y:%.3f Z:%.3f", TheMenu.plrPos.x, TheMenu.plrPos.y, TheMenu.plrPos.z);
			CFrontend_SetDrawRGBA(0,0,0, 255);
			CFrontend_Print8(buffer, 1, 0.1 + 0.005, 0.01 + 0.005, 1.01, 1.03, 0);
			CFrontend_SetDrawRGBA(168, 168, 168, 255);
			CFrontend_Print8(buffer, 1, 0.1, 0.01, 1, 1, 0);
		}
	}

	if (TheMenu.m_bActive)
	{
		CRenderer_DrawQuad2d(0, 0.4, 0.5, 0.5, 0, 0, 0, 220, 0, 0, 0, 0);
		char* active = "";
		char isActive = 0;
		int total = TOTAL_MENUS;

		switch (m_nCurrentMenuOpen)
		{
		case MENU_PLAYER:
			total = sizeof(PlayerOptions) / sizeof(PlayerOptions[0]);
			break;
		case MENU_WEAPONS:
			total = sizeof(weapons) / sizeof(weapons[0]);
			break;
		case MENU_CHANGE_PLR_MODEL:
			total = sizeof(menuSkins) / sizeof(menuSkins[0]);
			break;
		case MENU_WEATHER:
			total = sizeof(WeatherOptions) / sizeof(WeatherOptions[0]);
			break;
		case MENU_HUD:
			total = sizeof(HudOptions) / sizeof(HudOptions[0]);
			break;
		case MENU_MISC:
			total = sizeof(MiscOptions) / sizeof(MiscOptions[0]);
			break;
		default:
			break;
		}

		if (!(m_nCurrentMenuOpen == -1))
		{
			CFrontend_SetDrawRGBA(0, 0, 0, 255);
			CFrontend_Print8(m_szMenuOptions[m_nCurrentMenuOpen], 1, 0.03 + 0.005, 0.05 + 0.005, 1, 1, 0);
			CFrontend_SetDrawRGBA(80, 80, 80, 255);
			CFrontend_Print8(m_szMenuOptions[m_nCurrentMenuOpen], 1, 0.03, 0.05, 1, 1, 0);
		}

		if (m_nCurrentMenuOpen == MENU_WEAPONS)
		{
			float x = 0.02f;
			float y = 0.15f;
			for (int i = 0; i < total; i++)
			{
				if (i >= 15)
				{
					if (i % 15 == 0)
					{
						x += 0.36f;
						y = 0.15f;
					}

				}

		
				sprintf(buffer, "%s", GetWeapName(weapons[i].id));



				CFrontend_SetDrawRGBA(0, 0, 0, 255);
				CFrontend_Print8(buffer, 1, x + 0.005 , y + 0.005 , 1, 1, 0);

				int available = CEntityManager_GetEntityTypeDataFromName(GetWeapName(weapons[i].id));

				if (available)
					CFrontend_SetDrawRGBA(80, 80, 80, 255);
				else
					CFrontend_SetDrawRGBA(65, 0, 0, 130);

				if (i == m_nCurrentPos && available)
					CFrontend_SetDrawRGBA(170, 64, 64, 255);
				else if (i == m_nCurrentPos && !available)
					CFrontend_SetDrawRGBA(65, 0, 0, 255);

				if (available && i == m_nCurrentPos)
					CFrontend_Print8(buffer, 1, x, y, 1.20, 1.20, 0);
				else
					CFrontend_Print8(buffer, 1, x, y, 1,1, 0);

				y += 0.05;


			}
		}
		else if (m_nCurrentMenuOpen == MENU_CHANGE_PLR_MODEL)
		{

			if (!m_bChangePlayerSkin)
			{
				sprintf(buffer, "Enable 'Change Skin' in player options to use this section!");
				CFrontend_SetDrawRGBA(0, 0, 0, 255);
				CFrontend_Print8(buffer, 1, 0.05 + 0.005, 0.65 + 0.005, 1, 1, 0);


				CFrontend_SetDrawRGBA(170, 120, 64, 255);

				CFrontend_Print8(buffer, 1, 0.05, 0.65, 1, 1, 0);
			}


			float x = 0.02f;
			float y = 0.15f;
			for (int i = 0; i < total; i++)
			{
				if (i >= 15)
				{
					if (i % 15 == 0)
					{
						x += 0.36f;
						y = 0.15f;
					}

				}


				sprintf(buffer, "%s", menuSkins[i]);



				CFrontend_SetDrawRGBA(0, 0, 0, 255);
				CFrontend_Print8(buffer, 1, x + 0.005, y + 0.005, 1, 1, 0);

				CFrontend_SetDrawRGBA(80, 80, 80, 255);

				if (i == m_nCurrentPos)
					CFrontend_SetDrawRGBA(170, 64, 64, 255);

				CFrontend_Print8(buffer, 1, x, y, 1, 1, 0);

				y += 0.05;


			}

			sprintf(buffer, "Restart level after selecting anything.");
			CFrontend_SetDrawRGBA(0, 0, 0, 255);
			CFrontend_Print8(buffer, 1, 0.05 + 0.005, 0.75 + 0.005, 1, 1, 0);


			CFrontend_SetDrawRGBA(170, 120, 64, 255);

			CFrontend_Print8(buffer, 1, 0.05, 0.75, 1, 1, 0);
		}
#ifdef DEBUG
		else if (m_nCurrentMenuOpen == MENU_DEBUG)
		{
			if (GetGameVersion() == GAME_USA)
			{
				float x = 0.02f;
				float y = 0.15f;


				struct CPadState padState = *(struct CPadState*)(GetBaseAddr() + 0x41E888);
				sprintf(buffer, "CInputManager::m_padState: %x OS: %x", GetBaseAddr() + 0x41E888, GetBaseAddr() + 0x330940);
				CFrontend_SetDrawRGBA(0, 0, 0, 255);
				CFrontend_Print8(buffer, 1, x + 0.005, y + 0.005, 1, 1, 0);
				CFrontend_SetDrawRGBA(80, 80, 80, 255);
				CFrontend_Print8(buffer, 1, x, y, 1, 1, 0);

				y += 0.075f;

				sprintf(buffer, "Left Stick X: %f Y: %f", padState.m_fStickX, padState.m_fStickY);
				CFrontend_SetDrawRGBA(0, 0, 0, 255);
				CFrontend_Print8(buffer, 1, x + 0.005, y + 0.005, 1, 1, 0);
				CFrontend_SetDrawRGBA(80, 80, 80, 255);
				CFrontend_Print8(buffer, 1, x, y, 1, 1, 0);

				y += 0.075f;

				sprintf(buffer, "Right Stick: X: %f Y: %f", padState.m_fRStickX, padState.m_fRStickY);
				CFrontend_SetDrawRGBA(0, 0, 0, 255);
				CFrontend_Print8(buffer, 1, x + 0.005, y + 0.005, 1, 1, 0);
				CFrontend_SetDrawRGBA(80, 80, 80, 255);
				CFrontend_Print8(buffer, 1, x, y, 1, 1, 0);

				y += 0.075f;

				sprintf(buffer, "Button: 0x%X", padState.m_padButton);
				CFrontend_SetDrawRGBA(0, 0, 0, 255);
				CFrontend_Print8(buffer, 1, x + 0.005, y + 0.005, 1, 1, 0);
				CFrontend_SetDrawRGBA(80, 80, 80, 255);
				CFrontend_Print8(buffer, 1, x, y, 1, 1, 0);

				y += 0.075f;
				struct SystemPad pad = *(struct SystemPad*)(GetBaseAddr() + 0x330940);
				sprintf(buffer, "Button: %x RS:  %d %d", pad.button, pad.rX, pad.rY);
				CFrontend_SetDrawRGBA(0, 0, 0, 255);
				CFrontend_Print8(buffer, 1, x + 0.005, y + 0.005, 1, 1, 0);
				CFrontend_SetDrawRGBA(80, 80, 80, 255);
				CFrontend_Print8(buffer, 1, x, y, 1, 1, 0);
			}		
		}
#endif
		else if (m_nCurrentMenuOpen == MENU_PLAYER)
		{
			for (int i = 0; i < total; i++)
			{

				if (PlayerOptions[i].toggle)
					sprintf(buffer, "%s = %s",PlayerOptions[i].name, TRUE_FALSE(*PlayerOptions[i].toggle));
				if (PlayerOptions[i].func)
					sprintf(buffer, "%s", PlayerOptions[i].name);
	
				CFrontend_SetDrawRGBA(0, 0, 0, 255);
				CFrontend_Print8(buffer, 1, 0.05 + 0.005, 0.15 + 0.005 + 0.0625 * i, 1.25, 1.25, 0);
				CFrontend_SetDrawRGBA(80, 80, 80, 255);
				if (i == m_nCurrentPos)
					CFrontend_SetDrawRGBA(170, 64, 64, 255);
				CFrontend_Print8(buffer, 1, 0.05, 0.15 + 0.0625 * i, 1.25, 1.25, 0);
			}
		}
		else if (m_nCurrentMenuOpen == MENU_MISC)
		{
			for (int i = 0; i < total; i++)
			{

				if (MiscOptions[i].toggle)
					sprintf(buffer, "%s = %s", MiscOptions[i].name, TRUE_FALSE(*MiscOptions[i].toggle));
				if (MiscOptions[i].func)
					sprintf(buffer, "%s", MiscOptions[i].name);

				CFrontend_SetDrawRGBA(0, 0, 0, 255);
				CFrontend_Print8(buffer, 1, 0.05 + 0.005, 0.15 + 0.005 + 0.0625 * i, 1.25, 1.25, 0);
				CFrontend_SetDrawRGBA(80, 80, 80, 255);
				if (i == m_nCurrentPos)
					CFrontend_SetDrawRGBA(170, 64, 64, 255);
				CFrontend_Print8(buffer, 1, 0.05, 0.15 + 0.0625 * i, 1.25, 1.25, 0);
			}
		}
		else if (m_nCurrentMenuOpen == MENU_WEATHER)
		{
			for (int i = 0; i < total; i++)
			{

				if (WeatherOptions[i].toggle)
					sprintf(buffer, "%s = %s", WeatherOptions[i].name, TRUE_FALSE(*WeatherOptions[i].toggle));
				if (WeatherOptions[i].func)
					sprintf(buffer, "%s", WeatherOptions[i].name);

				CFrontend_SetDrawRGBA(0, 0, 0, 255);
				CFrontend_Print8(buffer, 1, 0.05 + 0.005, 0.15 + 0.005 + 0.0625 * i, 1.25, 1.25, 0);
				CFrontend_SetDrawRGBA(80, 80, 80, 255);
				if (i == m_nCurrentPos)
					CFrontend_SetDrawRGBA(170, 64, 64, 255);
				CFrontend_Print8(buffer, 1, 0.05, 0.15 + 0.0625 * i, 1.25, 1.25, 0);
			}
		}
		else if (m_nCurrentMenuOpen == MENU_HUD)
		{
			for (int i = 0; i < total; i++)
			{

				if (HudOptions[i].toggle)
					sprintf(buffer, "%s = %s", HudOptions[i].name, TRUE_FALSE(*HudOptions[i].toggle));
				if (HudOptions[i].func)
					sprintf(buffer, "%s", HudOptions[i].name);

				CFrontend_SetDrawRGBA(0, 0, 0, 255);
				CFrontend_Print8(buffer, 1, 0.05 + 0.005, 0.15 + 0.005 + 0.0625 * i, 1.25, 1.25, 0);
				CFrontend_SetDrawRGBA(80, 80, 80, 255);
				if (i == m_nCurrentPos)
					CFrontend_SetDrawRGBA(170, 64, 64, 255);
				CFrontend_Print8(buffer, 1, 0.05, 0.15 + 0.0625 * i, 1.25, 1.25, 0);
			}
		}
		else
		{
			for (int i = 0; i < total; i++)
			{
				sprintf(buffer, "%s%s", active, m_szMenuOptions[i]);

				CFrontend_SetDrawRGBA(0, 0, 0, 255);
				CFrontend_Print8(buffer, 1, 0.05 + 0.005, 0.15 + 0.005 + 0.0625 * i, 1.25, 1.25, 0);
				CFrontend_SetDrawRGBA(80, 80, 80, 255);
				if (i == m_nCurrentPos)
					CFrontend_SetDrawRGBA(170, 64, 64, 255);
				CFrontend_Print8(buffer, 1, 0.05, 0.15 + 0.0625 * i, 1.25, 1.25, 0);
			}
		}
		

		// generic info
		if (m_nCurrentMenuOpen == -1)
		{
			char* curLvl = (char*)(GetBaseAddr() + 0x311194);
			sprintf(buffer, "Level: %s", curLvl);

			CFrontend_SetDrawRGBA(0, 0, 0, 255);
			CFrontend_Print8(buffer, 1, 0.5, 0.15 + 0.005, 1, 1, 0);
			CFrontend_SetDrawRGBA(80, 80, 80, 255);
			CFrontend_Print8(buffer, 1, 0.5, 0.15, 1, 1, 0);

			if (newPlayer)
			{
				sprintf(buffer, "Selected Skin: %s", newPlayer);

				CFrontend_SetDrawRGBA(0, 0, 0, 255);
				CFrontend_Print8(buffer, 1, 0.5, 0.18 + 0.005, 1, 1, 0);
				CFrontend_SetDrawRGBA(80, 80, 80, 255);
				CFrontend_Print8(buffer, 1, 0.5, 0.18, 1, 1, 0);
			}



			static char* controls[] = {
				"~right~ ~left~ ~down~ ~up~ - Navigate",
				"~cross~ - Accept/OK/Toggle",
				"~circle~/~triangle~ - Go back",
				"~r1~ + ~up~ - Toggle menu",
				"~l1~ + ~down~ + ~triangle~ - Toggle player input",
				"~l1~ + ~down~ + ~cross~ - Toggle free camera",
				"~l1~ + ~down~ + ~circle~ - Freeze/unfreeze world",
				"~r1~ - Teleport to free cam location"
			};

			for (int a = 0; a < sizeof(controls) / sizeof(controls[0]); a++)
			{
				CFrontend_SetDrawRGBA(0, 0, 0, 255);
				CFrontend_Print8(controls[a], 1, 0.5, 0.55 + 0.005 + 0.0475 * a , 0.95, 0.95, 0);
				CFrontend_SetDrawRGBA(80, 80, 80, 255);
				CFrontend_Print8(controls[a], 1, 0.5, 0.55 + 0.0475 * a , 0.95, 0.95, 0);
			}
		}



		CFrontend_SetDrawRGBA(0,0,0,255);
		CFrontend_Print8("MH2Menu " MH2MENU_VERSION " by ermaccer", 1, 0.05 + 0.005, 0.95 + 0.005, 1, 1, 0);
		CFrontend_SetDrawRGBA(90, 90, 90, 255);
		CFrontend_Print8("MH2Menu " MH2MENU_VERSION " by ermaccer", 1, 0.05, 0.95, 1, 1, 0);
#ifdef  DEBUG
		CFrontend_PrintInfo(23, "Pos: %d Cat: %d Page: %d\n", m_nCurrentPos, m_nCurrentMenuOpen, m_nCurrentWeaponPage);
#endif //  DEBUG


	}


}

void Menu_Process()
{
	Menu_Process_Toggles();
	Menu_Process_Teleport();
	// calling weapon spawner from menu crashes on hardware
	if (m_nWantToSpawnWeapon >= 0)
	{
		GiveWeapon(m_nWantToSpawnWeapon);
		m_nWantToSpawnWeapon = -1;
	}


	if (!TheMenu.m_bActive)
		return;


	int total = TOTAL_MENUS;

	if (m_nCurrentMenuOpen == MENU_PLAYER)
		total = sizeof(PlayerOptions) / sizeof(PlayerOptions[0]);
	else if (m_nCurrentMenuOpen == MENU_WEAPONS)
		total = sizeof(weapons) / sizeof(weapons[0]);
	else if (m_nCurrentMenuOpen == MENU_CHANGE_PLR_MODEL)
		total = sizeof(menuSkins) / sizeof(menuSkins[0]);
	else if (m_nCurrentMenuOpen == MENU_MISC)
		total = sizeof(MiscOptions) / sizeof(MiscOptions[0]);
	else if (m_nCurrentMenuOpen == MENU_WEATHER)
		total = sizeof(WeatherOptions) / sizeof(WeatherOptions[0]);
	else if (m_nCurrentMenuOpen == MENU_HUD)
		total = sizeof(HudOptions) / sizeof(HudOptions[0]);

	if (m_nCurrentPos + 1 > total) m_nCurrentPos = 0;
	if (m_nCurrentPos < 0) m_nCurrentPos = total - 1;
}

void Menu_Process_Toggles()
{
	if (CScene_FindPlayer())
		CEntity_SetFlag(CScene_FindPlayer(), 0x100, m_bGodMode);

	if (GetGameVersion() == GAME_EUROPE)
	{
		*(int*)(GetBaseAddr() + 0x3200A8) = m_bFreeCamera;
		*(int*)(GetBaseAddr() + 0x3200AC) = m_bLockCamera;
	}
	else
	{
		*(int*)(GetBaseAddr() + 0x31FFA8) = m_bFreeCamera;
		*(int*)(GetBaseAddr() + 0x31FFAC) = m_bLockCamera;
	}


	// TODO
	/*
		if (m_bInfiniteAmmo)
		{
			Nop(0x2789AC);
		}
		else
		{
			PatchInt(0x2789AC, 0x2484FFFF);
		}
	*/

	if (GetGameVersion() == GAME_EUROPE)
		*(int*)(GetBaseAddr() + 0x31D290) = m_bDisplayBodyCount;
	else
		*(int*)(GetBaseAddr() + 0x31D190) = m_bDisplayBodyCount;

	// TODO
	/*
	if (m_bDisplayBodyCount)
	{
		if (m_bEnableBodyCount)
		{
			int kills = *(int*)(GetBaseAddr() + 0x394398);
			int execs = *(int*)(GetBaseAddr() + 0x394378);
			*(int*)(GetBaseAddr() + 0x31D194) = kills + execs;
		}
	}
	*/
}

void Menu_OnKeyUp()
{
	m_nCurrentPos--;
}

void Menu_OnKeyLeft()
{
	m_nCurrentPos -= 15;
}

void Menu_OnKeyDown()
{
	m_nCurrentPos++;
}

void Menu_OnKeyRight()
{
	m_nCurrentPos += 15;
}

void Menu_OnKeyExecute()
{
	if (m_nCurrentMenuOpen == -1)
	{
		m_nCurrentMenuOpen = m_nCurrentPos;
		m_nCurrentPos = 0;
	}
	else
	{
		switch (m_nCurrentMenuOpen)
		{
		case MENU_PLAYER:
			if (PlayerOptions[m_nCurrentPos].toggle)
				*PlayerOptions[m_nCurrentPos].toggle ^= 1;
			if (PlayerOptions[m_nCurrentPos].func)
				PlayerOptions[m_nCurrentPos].func();
			break;
		case MENU_MISC:
			if (MiscOptions[m_nCurrentPos].toggle)
				*MiscOptions[m_nCurrentPos].toggle ^= 1;
			if (MiscOptions[m_nCurrentPos].func)
				MiscOptions[m_nCurrentPos].func();
			break;
		case MENU_WEATHER:
			if (WeatherOptions[m_nCurrentPos].toggle)
				*WeatherOptions[m_nCurrentPos].toggle ^= 1;
			if (WeatherOptions[m_nCurrentPos].func)
				WeatherOptions[m_nCurrentPos].func();
			break;
		case MENU_HUD:
			if (HudOptions[m_nCurrentPos].toggle)
				*HudOptions[m_nCurrentPos].toggle ^= 1;
			if (HudOptions[m_nCurrentPos].func)
				HudOptions[m_nCurrentPos].func();
			break;
		case MENU_WEAPONS:
			RegisterGiveWeapon(weapons[m_nCurrentPos].id);
			break;
		case MENU_CHANGE_PLR_MODEL:
			CFrontend_PrintInfo(1, "Selected %s", menuSkins[m_nCurrentPos]);
			newPlayer = menuSkins[m_nCurrentPos];
			break;
		default:
			break;
		}
	}

}

void Menu_OnKeyBack()
{
	m_nCurrentMenuOpen = -1;
	m_nCurrentPos = 0;
}

void Menu_Update()
{
	sceCtrlPeekBufferPositive(&pad, 1);
	if (pad.Buttons & PSP_CTRL_RTRIGGER)
	{
		if (pad.Buttons & PSP_CTRL_UP)
		{
			TheMenu.m_bActive ^= 1;
			int addr = 0x31DB60;
			if (GetGameVersion() == GAME_EUROPE)
				addr = 0x31DC60;
			*(int*)(GetBaseAddr() + addr) ^= 1;

			sceKernelDelayThread(125000);
		}
	}

	if (pad.Buttons & PSP_CTRL_LTRIGGER)
	{
		if (pad.Buttons & PSP_CTRL_DOWN)
		{
			if (pad.Buttons & PSP_CTRL_TRIANGLE)
			{
				int addr = 0x31DB60;
				if (GetGameVersion() == GAME_EUROPE)
					addr = 0x31DC60;
				*(int*)(GetBaseAddr() + addr) ^= 1;

				sceKernelDelayThread(125000);
				CFrontend_PrintInfo(4, "Input flipped (status %d)!", *(int*)(GetBaseAddr() + addr));
			}
			if (pad.Buttons & PSP_CTRL_CROSS)
			{
				// free cam
				m_bFreeCamera ^= 1;

				sceKernelDelayThread(125000);
			}
			if (pad.Buttons & PSP_CTRL_CIRCLE)
			{
				// freeze world
				if (GetGameVersion() == GAME_EUROPE)
					*(int*)(GetBaseAddr() + 0x3200E8) ^= 1;
				else
					*(int*)(GetBaseAddr() + 0x31FFE8) ^= 1;

				sceKernelDelayThread(125000);
			}

		}
	}
	int cam_addr = 0x31FFA8;
	if (GetGameVersion() == GAME_EUROPE)
		cam_addr = 0x3200A8;
	if (*(int*)(GetBaseAddr() + cam_addr))
	{
		if (pad.Buttons & PSP_CTRL_RTRIGGER)
		{
			if (!TheMenu.m_bActive)
				CameraTeleport();
		}

	}



	if (TheMenu.m_bActive)
	{
		if (pad.Buttons & PSP_CTRL_DOWN)
		{
			Menu_OnKeyDown();
			sceKernelDelayThread(125000);
		}
		if (pad.Buttons & PSP_CTRL_UP)
		{
			Menu_OnKeyUp();
			sceKernelDelayThread(125000);
		}
		if (pad.Buttons & PSP_CTRL_LEFT)
		{
			Menu_OnKeyLeft();
			sceKernelDelayThread(125000);
		}
		if (pad.Buttons & PSP_CTRL_RIGHT)
		{
			Menu_OnKeyRight();
			sceKernelDelayThread(125000);
		}
		if (pad.Buttons & PSP_CTRL_CROSS)
		{
			sceKernelDelayThread(125000);
			Menu_OnKeyExecute();
		}
		if (pad.Buttons & PSP_CTRL_CIRCLE || pad.Buttons & PSP_CTRL_TRIANGLE)
		{
			sceKernelDelayThread(125000);
			Menu_OnKeyBack();
			sceKernelDelayThread(125000);

		}
	}

	sceKernelDelayThread(10000);

}

void Menu_Update2()
{
	sceCtrlPeekBufferPositive(&pad, 1);
	if (pad.Buttons & PSP_CTRL_LTRIGGER)
	{
		if (pad.Buttons & PSP_CTRL_RTRIGGER)
		{
			if (CScene_FindPlayer())
			CCharacter_CreateInventoryItem(CScene_FindPlayer(), CT_CROWBAR, 0);
			sceKernelDelayThread(125000);
		}

	}

	sceKernelDelayThread(10000);

}

void Menu_Function()
{
	Menu_Process();
	Menu_Draw();

}

void Menu_Process_Teleport()
{
	if (m_bWantToTeleport)
	{
		int player = CScene_FindPlayer();
		if (player)
		{
			int camera = CScene_FindCamera();
			if (camera)
			{
				int cam_obj = *(int*)(camera + 256);
				if (cam_obj)
				{
					CVector camPos = *(CVector*)(cam_obj + 64 + 48);

					CPed_SetPos(player, &camPos, 0);
				}
			}
		}
		m_bWantToTeleport = 0;
	}
}

void RegisterGiveWeapon(int id)
{
	m_nWantToSpawnWeapon = id;
}

void GiveWeapon(int id)
{
	if (CEntityManager_GetEntityTypeDataFromName(GetWeapName(id)))
	{
		CFrontend_PrintInfo(2, "Weapon %s created.", GetWeapName(id));
		CCharacter_CreateInventoryItem(CScene_FindPlayer(), id, 1);

		int inventory = *(int*)(CScene_FindPlayer() + 516);
		if (inventory)
		{
			int collectable = CInventory_FindItemWithCollectableType(inventory, id);
			if (collectable)
			{
				int weapon = *(int*)(collectable + 596);
				int weapon_class = **(int**)weapon;
				if (weapon_class == WC_AMMO)
					CAmmoWeapon_SetAmmo(weapon, 1000);
			}
		}

	}
	else
		CFrontend_PrintInfo(2, "Entity %s does not exist on this level!", GetWeapName(id));

		
}

char * GetWeapName(int id)
{
	static char tmp[128] = {};
	CCollectable_GetNameStringFromType(id, tmp);

	return tmp;
}

void SetPlayer(CString * str)
{
	if (m_bChangePlayerSkin)
	{
		CString plr;
		char* newModel = newPlayer;
		plr.cap = strlen(newModel) + 1;
		plr.len = strlen(newModel);
		plr.str = newModel;
		CFileNames_setPlayer(&plr);
	}
	else
		CFileNames_setPlayer(str);
}

void PutBagOnHead()
{
	if (!CScene_FindPlayer())
		return;

}

void Menu_TestSpawn()
{
	GiveWeapon(CT_CROWBAR);
}

void SetWeatherFree()
{
	CWeather_ForceWeather(FREE);
}

void SetWeatherCloudy()
{
	CWeather_ForceWeather(CLOUDY);
}

void SetWeatherWindy()
{
	CWeather_ForceWeather(WINDY);
}

void SetWeatherRainy()
{
	CWeather_ForceWeather(RAINY);
}

void SetWeatherThunder()
{
	CWeather_ForceWeather(THUNDER);
}

void SetWeatherFoggy()
{
	CWeather_ForceWeather(FOGGY);
}

void SetWeatherClear()
{
	CWeather_ForceWeather(CLEAR);
}