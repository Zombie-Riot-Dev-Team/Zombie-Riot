/**
 * ====================
 *     Zombie Riot
 *   File: commands.inc
 *   Author: Greyscale
 * ====================
 */
 
Handle hAdminMenu = INVALID_HANDLE;
TopMenuObject obj_admin_menu_zombie_riot = INVALID_TOPMENUOBJECT;
 
void CreateCommands()
{
	RegAdminCmd("sm_zradmin", Command_MenuAdminZombieRiot, ADMFLAG_GENERIC, "Zombie Riot Admin Menu");
	RegAdminCmd("zriot_restrict", Command_Restrict, ADMFLAG_GENERIC, "Restrict a specified weapon");
	RegAdminCmd("zriot_unrestrict", Command_UnRestrict, ADMFLAG_GENERIC, "Unrestrict a specified weapon");
	
	RegAdminCmd("sm_setday", Command_SetDay, ADMFLAG_GENERIC, "Sets the game to a certain day");
	
	RegAdminCmd("sm_zombie", Command_Zombie, ADMFLAG_GENERIC, "Turns player into zombie");
	RegAdminCmd("sm_human", Command_Human, ADMFLAG_GENERIC, "Turns player into human");
}

public void OnAdminMenuCreated(Handle topmenu)
{
	if (topmenu == hAdminMenu && obj_admin_menu_zombie_riot != INVALID_TOPMENUOBJECT)
	{
		return;
	}

	obj_admin_menu_zombie_riot = AddToTopMenu(topmenu, "admin_menu_zombie_riot", TopMenuObject_Category, Handle_Category, INVALID_TOPMENUOBJECT);
}

public void Handle_Category(Handle topmenu, TopMenuAction action, TopMenuObject object_id, int param, char[] buffer, int maxlength)
{
	switch(action)
	{
		case TopMenuAction_DisplayTitle:
			Format(buffer, maxlength, "%T", "MenuAdminZombieRiot_TITLE", param);
		case TopMenuAction_DisplayOption:
			Format(buffer, maxlength, "%T", "MenuAdminZombieRiot_TITLE", param);
	}
}

public void OnAdminMenuReady(Handle topmenu)
{

	if(obj_admin_menu_zombie_riot == INVALID_TOPMENUOBJECT)
	{
		OnAdminMenuCreated(topmenu);
	}
	
	if (topmenu == hAdminMenu)
	{
		return;
	}
	
	hAdminMenu = topmenu;
	
	TopMenuObject menu = FindTopMenuCategory(hAdminMenu, "admin_menu_zombie_riot");
	
	AddToTopMenu(hAdminMenu, "sm_human", 		TopMenuObject_Item, AdminMenu_ZombieRiotHuman, 		menu, "sm_zrhuman", 	ADMFLAG_GENERIC);
	AddToTopMenu(hAdminMenu, "sm_zombie", 	TopMenuObject_Item, AdminMenu_ZombieRiotZombie, 	menu, "sm_zrzombie", 	ADMFLAG_GENERIC);
}

public void AdminMenu_ZombieRiotHuman(Handle topmenu, TopMenuAction action, TopMenuObject object_id, int param, char[] buffer, int maxlength)
{
	switch(action)
	{
		case TopMenuAction_DisplayOption:
			Format(buffer, maxlength, "%T", "MenuAdminZombieRiot_HUMANIFY_MENU_TITLE", param);
		case TopMenuAction_SelectOption:
		{
			BuildMenuAdminZombieRiot_HUMANIFY(param);
		}
	}
}

public void AdminMenu_ZombieRiotZombie(Handle topmenu, TopMenuAction action, TopMenuObject object_id, int param, char[] buffer, int maxlength)
{
	switch(action)
	{
		case TopMenuAction_DisplayOption:
			Format(buffer, maxlength, "%T", "MenuAdminZombieRiot_ZOMBIFY_MENU_TITLE", param);
		case TopMenuAction_SelectOption:
		{
			BuildMenuAdminZombieRiot_ZOMBIFY(param);
		}
	}
}

public Action Command_MenuAdminZombieRiot(int client, int args) 
{
	BuildMenuAdminZombieRiot(client);
	return Plugin_Handled;
}

void BuildMenuAdminZombieRiot(int client)
{
	char title[40]; 
	char humanify[40];
	char zombify[40];
	Menu menu = CreateMenu(MenuAdminZombieRiotAction);
	
	Format(humanify, sizeof(humanify), "%T", "MenuAdminZombieRiot_HUMANIFY_MENU_TITLE", client);
	AddMenuItem(menu, "M_humanify", humanify);
	
	Format(zombify, sizeof(zombify), "%T", "MenuAdminZombieRiot_ZOMBIFY_MENU_TITLE", client);
	AddMenuItem(menu, "M_zombify", zombify);
	
	
	Format(title, sizeof(title), "%T", "MenuAdminZombieRiot_TITLE", client);
	menu.SetTitle(title);
	SetMenuExitBackButton(menu, true);
	menu.Display(client, MENU_TIME_FOREVER);
}

public int MenuAdminZombieRiotAction(Menu menu, MenuAction action, int param1, int param2)
{
	switch(action)
	{
		case MenuAction_End:
		{
			CloseHandle(menu);	
		}
		case MenuAction_Cancel:
		{
			if (param2 == MenuCancel_ExitBack && hAdminMenu != INVALID_HANDLE)
			{
				DisplayTopMenu(hAdminMenu, param1, TopMenuPosition_LastCategory);
			}		
		}
		case MenuAction_Select:
		{
			char menu1[56];
			menu.GetItem(param2, menu1, sizeof(menu1));
			
			if(StrEqual(menu1, "M_humanify"))
			{
				BuildMenuAdminZombieRiot_HUMANIFY(param1);
			}
			else if(StrEqual(menu1, "M_zombify"))
			{
				BuildMenuAdminZombieRiot_ZOMBIFY(param1);
			}
		}
	}
}

void BuildMenuAdminZombieRiot_HUMANIFY(int client)
{
	Menu menu = CreateMenu(BuildMenuAdminZombieRiot_HUMANIFY_ACTION);

	char title[100];

	Format(title, sizeof(title), "%T", "MenuAdminZombieRiot_HUMANIFY_MENU_TITLE", client);

	menu.SetTitle(title);
	menu.ExitBackButton = true;

	AddTargetsToMenu(menu, client, true, true);

	menu.Display(client, MENU_TIME_FOREVER);

}

public int BuildMenuAdminZombieRiot_HUMANIFY_ACTION(Menu menu, MenuAction action, int param1, int param2)
{
	if (action == MenuAction_End)
	{
		delete menu;
	}
	else if (action == MenuAction_Cancel)
	{
		if (param2 == MenuCancel_ExitBack)
		{
			BuildMenuAdminZombieRiot(param1);
		}
	}
	else if (action == MenuAction_Select)
	{
		char info[32];
		int userid, target;
		menu.GetItem(param2, info, sizeof(info));
		userid = StringToInt(info);
		
		if ((target = GetClientOfUserId(userid)) == 0)
		{
			ZRiot_PrintToChat(param1, "%t", "Player no longer available");
		}
		else if (!CanUserTarget(param1, target))
		{
			ZRiot_PrintToChat(param1, "%t", "Unable to target");
		}
		else
		{
			char name[32];
			GetClientName(target, name, sizeof(name));
			ZRiot_Human(target);
			ChangeClientDeathCount(target, 0);
		}
		/* Re-draw the menu if they're still valid */
		if (IsClientInGame(param1) && !IsClientInKickQueue(param1))
		{
			BuildMenuAdminZombieRiot_HUMANIFY(param1);
		}
	}
}

void BuildMenuAdminZombieRiot_ZOMBIFY(int client)
{
	Menu menu = CreateMenu(BuildMenuAdminZombieRiot_ZOMBIFY_ACTION);

	char title[100];

	Format(title, sizeof(title), "%T", "MenuAdminZombieRiot_ZOMBIFY_MENU_TITLE", client);

	menu.SetTitle(title);
	menu.ExitBackButton = true;

	AddTargetsToMenu(menu, client, true, true);

	menu.Display(client, MENU_TIME_FOREVER);

}

public int BuildMenuAdminZombieRiot_ZOMBIFY_ACTION(Menu menu, MenuAction action, int param1, int param2)
{
	if (action == MenuAction_End)
	{
		delete menu;
	}
	else if (action == MenuAction_Cancel)
	{
		if (param2 == MenuCancel_ExitBack)
		{
			BuildMenuAdminZombieRiot(param1);
		}
	}
	else if (action == MenuAction_Select)
	{
		char info[32];
		int userid, target;
		menu.GetItem(param2, info, sizeof(info));
		userid = StringToInt(info);
		
		if ((target = GetClientOfUserId(userid)) == 0)
		{
			ZRiot_PrintToChat(param1, "%t", "Player no longer available");
		}
		else if (!CanUserTarget(param1, target))
		{
			ZRiot_PrintToChat(param1, "%t", "Unable to target");
		}
		else
		{
			char name[32];
			GetClientName(target, name, sizeof(name));
			ZRiot_Human(target);
		}
		/* Re-draw the menu if they're still valid */
		if (IsClientInGame(param1) && !IsClientInKickQueue(param1))
		{
			BuildMenuAdminZombieRiot_ZOMBIFY(param1);
		}
	}
}

public Action Command_Restrict(int client, int argc)
{
    bool enabled = GetConVarBool(CVAR_ENABLE);
    if (argc < 1 || !enabled)
    {
        return Plugin_Handled;
    }
    
    char arg1[32];
    GetCmdArg(1, arg1, sizeof(arg1));
    
    WepRestrictQuery output = RestrictWeapon(arg1);
    
    if (output == Existing)
    {
        ZRiot_ReplyToCommand(client, "Weapon already restricted", arg1);
    }
    
    return Plugin_Handled;
}

public Action Command_UnRestrict(int client, int argc)
{
    bool enabled = GetConVarBool(CVAR_ENABLE);
    if (argc < 1 || !enabled)
    {
        return Plugin_Handled;
    }
    
    char arg1[32];
    GetCmdArg(1, arg1, sizeof(arg1));
    
    WepRestrictQuery output = UnRestrictWeapon(arg1);
    
    if (output == Invalid)
    {
        ZRiot_ReplyToCommand(client, "Weapon invalid", arg1);
    }
    
    return Plugin_Handled;
}

public Action Command_SetDay(int client, int argc)
{
    bool enabled = GetConVarBool(CVAR_ENABLE);
    if (argc < 1 || !enabled)
    {
        return Plugin_Handled;
    }
    
    char arg1[32];
    GetCmdArg(1, arg1, sizeof(arg1));
    
    int day = StringToInt(arg1) - 1;
    day = (day >= 0) ? day : 0;
    
    gDay = day;
    gDay = (gDay + 1 > dCount) ? dCount - 1 : gDay;
    
    ZRiot_PrintToChat(0, "Skip to day", gDay + 1);
    
    if (tHUD == INVALID_HANDLE)
    {
        return Plugin_Handled;
    }
    
    TerminateRound(3.0, CSRoundEnd_Draw);
    
    return Plugin_Handled;
}

public Action Command_Zombie(int client, int argc)
{
    bool enabled = GetConVarBool(CVAR_ENABLE);
    if (argc < 1 || !enabled)
    {
        return Plugin_Handled;
    }
    
    char arg1[32];
    GetCmdArg(1, arg1, sizeof(arg1));
    
    char target_name[MAX_TARGET_LENGTH];
    int targets[MAXPLAYERS];
    bool tn_is_ml;
    
    int tcount = ProcessTargetString(arg1, client, targets, MAXPLAYERS, COMMAND_FILTER_NO_BOTS, target_name, sizeof(target_name), tn_is_ml);
    if (tcount <= 0)
    {
        ReplyToTargetError(client, tcount);
        return Plugin_Handled;
    }
    
    for (int x = 0; x < tcount; x++)
    {
        ZRiot_Zombie(targets[x]);
    }
    
    if (GetLiveHumanCount() <= 0)
    {
        TerminateRound(5.0, CSRoundEnd_TerroristWin);
    }
    
    return Plugin_Handled;
}

public Action Command_Human(int client, int argc)
{
    bool enabled = GetConVarBool(CVAR_ENABLE);
    if (argc < 1 || !enabled)
    {
        return Plugin_Handled;
    }
    
    char arg1[32];
    GetCmdArg(1, arg1, sizeof(arg1));
    
    char target_name[MAX_TARGET_LENGTH];
    int targets[MAXPLAYERS];
    bool tn_is_ml;
    
    int tcount = ProcessTargetString(arg1, client, targets, MAXPLAYERS, COMMAND_FILTER_NO_BOTS, target_name, sizeof(target_name), tn_is_ml);
    if (tcount <= 0)
    {
        ReplyToTargetError(client, tcount);
        return Plugin_Handled;
    }
    
    for (int x = 0; x < tcount; x++)
    {
        ZRiot_Human(targets[x]);
    }
    
    return Plugin_Handled;
}
