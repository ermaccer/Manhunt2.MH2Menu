#pragma once
#include "mh2.h"

#define MH2MENU_VERSION "1.1"

enum eMenuItems {
	MENU_WEAPONS,
	MENU_PLAYER,
	MENU_CHANGE_PLR_MODEL,
	MENU_WEATHER,
	MENU_HUD,
	MENU_MISC,
#ifdef DEBUG
	MENU_DEBUG,
#endif
	TOTAL_MENUS
};



struct Menu {
	char m_bActive;
	CVector plrPos;
};

struct Menu GetMenu();

void Menu_Init();
void Menu_Draw();
void Menu_Process();
void Menu_Process_Toggles();
void Menu_OnKeyUp();
void Menu_OnKeyLeft();
void Menu_OnKeyDown();
void Menu_OnKeyRight();
void Menu_OnKeyExecute();
void Menu_OnKeyBack();
void Menu_Update();
void Menu_Update2();
void Menu_Function();
void Menu_Process_Teleport();



void RegisterGiveWeapon(int id);
void GiveWeapon(int id);
char* GetWeapName(int id);

void SetPlayer(CString* str);
void PutBagOnHead();

void Menu_TestSpawn();

void SetWeatherFree();
void SetWeatherCloudy();
void SetWeatherWindy();
void SetWeatherRainy();
void SetWeatherThunder();
void SetWeatherFoggy();
void SetWeatherClear();

void CameraTeleport();