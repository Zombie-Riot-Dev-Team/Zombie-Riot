/**
 * ====================
 *     Zombie Riot
 *   File: daydata.inc
 *   Author: Greyscale
 * ====================
 */

#define MAXDAYS 25
char data_display[MAXDAYS][32];
char data_zombieoverride[MAXDAYS][32*MAXZOMBIES];
char data_storyline[MAXDAYS][192];
int data_count[MAXDAYS];
int data_healthboost[MAXDAYS];
int data_respawn[MAXDAYS];
int data_deaths_before_zombie[MAXDAYS];
float data_fademin[MAXDAYS];
float data_fademax[MAXDAYS];

Handle kvDays = INVALID_HANDLE;

int dCount;

void LoadDayData(bool defaultconfig)
{
    char path[PLATFORM_MAX_PATH];
    Format(path, sizeof(path), "%s/days.txt", gMapConfig);
    
    if (!defaultconfig && !FileExists(path))
    {
        return;
    }
    
    if (kvDays != INVALID_HANDLE)
    {
        CloseHandle(kvDays);
    }
    
    kvDays = CreateKeyValues("days");
    KvSetEscapeSequences(kvDays, true);
    
    if (!FileToKeyValues(kvDays, path))
    {
        SetFailState("\"%s\" failed to load", path);
    }
    
    KvRewind(kvDays);
    if (!KvGotoFirstSubKey(kvDays))
    {
        SetFailState("No day data defined in \"%s\"", path);
    }
    
    char display[32];
    char zombieoverride[32*MAXZOMBIES];
    char storyline[192];
    
    dCount = 0;
    do
    {
        KvGetSectionName(kvDays, display, sizeof(display));
        strcopy(data_display[dCount], 32, display);
        
        KvGetString(kvDays, "zombieoverride", zombieoverride, sizeof(zombieoverride));
        strcopy(data_zombieoverride[dCount], 32*MAXZOMBIES, zombieoverride);
        
        KvGetString(kvDays, "storyline", storyline, sizeof(storyline));
        strcopy(data_storyline[dCount], 192, storyline);
        
        data_count[dCount] = KvGetNum(kvDays, "count", 25);
        data_healthboost[dCount] = KvGetNum(kvDays, "healthboost");
        //[dCount][data_respawn] = bool:KvGetNum(kvDays, "respawn");
        data_respawn[dCount] = KvGetNum(kvDays, "respawn");
        data_deaths_before_zombie[dCount] = KvGetNum(kvDays, "deaths_before_zombie");
        data_fademin[dCount] = KvGetFloat(kvDays, "fademin");
        data_fademax[dCount] = KvGetFloat(kvDays, "fademax");
        
        dCount++;
    } while (KvGotoNextKey(kvDays));
}

void GetDayDisplay(int day, char[] display, int len)
{
    strcopy(display, len, data_display[day]);
}

bool ExplodeZombieOverrides(int day)
{
    if (adtZombies != INVALID_HANDLE)
    {
        CloseHandle(adtZombies);
        adtZombies = INVALID_HANDLE;
    }
    
    char zombieoverride[32*MAXZOMBIES];
    GetDayZombieOverride(day, zombieoverride, sizeof(zombieoverride));
    
    if (zombieoverride[0])
    {
        adtZombies = CreateArray();
        
        char sZombies[MAXZOMBIES][64];
        ExplodeString(zombieoverride, ",", sZombies, MAXZOMBIES, 64);
        
        for (int x = 0; x < MAXZOMBIES; x++)
        {
            if (!sZombies[x][0])
                continue;
            
            TrimString(sZombies[x]);
            int zombieid = FindZombieIDByName(sZombies[x]);
            
            if (zombieid == -1)
                continue;
            
            PushArrayCell(adtZombies, zombieid);
        }
        
        return true;
    }
    
    return false;
}

void GetDayZombieOverride(int day, char[] zombieoverride, int len)
{
    strcopy(zombieoverride, len, data_zombieoverride[day]);
}

void GetDayStoryLine(int day, char[] storyline, int len)
{
    strcopy(storyline, len, data_storyline[day]);
}

int GetDayCount(int day)
{
    return data_count[day];
}

int GetDayHealthBoost(int day)
{
    return data_healthboost[day];
}

int GetDayRespawn(int day)
{
    return data_respawn[day];
}

int GetDayDeathsBeforeZombie(int day)
{
    return data_deaths_before_zombie[day];
}

float GetDayMinFade(int day)
{
    return data_fademin[day];
}

float GetDayMaxFade(int day)
{
    return data_fademax[day];
}

void BeginDay()
{
    gZombiesKilled = 0;
    
    int zombiecount = GetDayCount(gDay);
    int zombiemax = GetConVarInt(CVAR_ZOMBIEMAX);
    
    int spawncount;
    int respawn = GetDayRespawn(gDay);
    
    if (respawn)
    {
        spawncount = zombiemax;
    }
    else
    {
        spawncount = (zombiecount < zombiemax) ? zombiecount : zombiemax;
    }
    
    ServerCommand("bot_quota %d", spawncount);
    
    char display[32];
    GetDayDisplay(gDay, display, sizeof(display));
    
    bool override = ExplodeZombieOverrides(gDay);
    
    int maxplayers = MaxClients;
    for (int x = 1; x <= maxplayers; x++)
    {
        if (!IsClientInGame(x))
        {
            continue;
        }
        
        ChangeClientDeathCount(x, 0);
        
        if (!IsPlayerZombie(x))
        {
            continue;
        }
        
        if (override)
        {
            int size = GetArraySize(adtZombies);
            if (size)
            {
                int zombieid = GetRandomInt(0, size - 1);
                            
                Zombify(x, GetArrayCell(adtZombies, zombieid));
            }
        }
    }
}
