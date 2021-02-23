/**
 * ====================
 *     Zombie Riot
 *   File: overlays.inc
 *   Author: Greyscale
 * ==================== 
 */

void ShowOverlays(float time, int winner)
{
    char overlay[64];
    if (winner == gZombieTeam)
    {
        GetConVarString(CVAR_OVERLAYS_ZOMBIE, overlay, sizeof(overlay));
    }
    else if (winner == gHumanTeam)
    {
        GetConVarString(CVAR_OVERLAYS_HUMAN, overlay, sizeof(overlay));
    }
        
    int maxplayers = MaxClients;
    for (int x = 1; x <= maxplayers; x++)
    {
        if (!IsClientInGame(x))
            continue;
        
        DisplayClientOverlay(x, overlay);
    }
    
    CreateTimer(time, KillOverlays);
}

public Action KillOverlays(Handle timer)
{
    int maxplayers = MaxClients;
    for (int x = 1; x <= maxplayers; x++)
    {
        if (!IsClientInGame(x))
            continue;
        
        ClientCommand(x, "r_screenoverlay \"\"");
    }
}
