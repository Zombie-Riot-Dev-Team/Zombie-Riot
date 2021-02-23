/**
 * ====================
 *     Zombie Riot
 *   File: zombiedata.inc
 *   Author: Greyscale
 * ==================== 
 */

#define MAXZOMBIES 25
char data_name[MAXZOMBIES][32];
char data_model[MAXZOMBIES][256];
char data_zvision[MAXZOMBIES][256];
bool data_override_required[MAXZOMBIES];
int data_health[MAXZOMBIES];
float data_speed[MAXZOMBIES];
float data_gravity[MAXZOMBIES];
float data_jump[MAXZOMBIES];
int data_fov[MAXZOMBIES];


char modelSuffix[8][16] = {".dx80.vtx", ".dx90.vtx", ".mdl", ".phy", ".sw.vtx", ".vvd", ".xbox", ".xbox.vtx"};

Handle kvZombies = INVALID_HANDLE;

Handle adtModels = INVALID_HANDLE;

Handle adtZombies = INVALID_HANDLE;
int zCount;

void FileLinesToArray(Handle array, const Handle file)
{
    ClearArray(array);
    
    char line[128];
    
    while(!IsEndOfFile(file) && ReadFileLine(file, line, sizeof(line)))
    {
        if (StrContains(line, ";") == -1)
        {
            if (StrContains(line, "//") > -1)
            {
                SplitString(line, "//", line, sizeof(line));
            }
            TrimString(line);
            
            if (!StrEqual(line, "", false))
            {
                PushArrayString(array, line);
            }
        }
    }
}
    
void LoadZombieData(bool defaultconfig)
{
    char path[PLATFORM_MAX_PATH];
    Format(path, sizeof(path), "%s/zombies.txt", gMapConfig);
    
    if (!defaultconfig && !FileExists(path))
    {
        return;
    }
    
    if (kvZombies != INVALID_HANDLE)
    {
        CloseHandle(kvZombies);
    }
    
    kvZombies = CreateKeyValues("zombies");
    
    if (!FileToKeyValues(kvZombies, path))
    {
        SetFailState("\"%s\" failed to load", path);
    }
    
    KvRewind(kvZombies);
    if (!KvGotoFirstSubKey(kvZombies))
    {
        SetFailState("No zombie data defined in \"%s\"", path);
    }
    
    char name[64];
    char type[32];
    char model[256];
    char zvision[256];
    
    zCount = 0;
    
    do
    {
        KvGetSectionName(kvZombies, name, sizeof(name));
        strcopy(data_name[zCount], 32, name);
        
        KvGetString(kvZombies, "type", type, sizeof(type));
        data_override_required[zCount] = (StrEqual(type, "override_required", false));
        
        KvGetString(kvZombies, "model", model, sizeof(model));
        strcopy(data_model[zCount], 256, model);
        
        KvGetString(kvZombies, "zvision", zvision, sizeof(zvision));
        strcopy(data_zvision[zCount], 256, zvision);
        
        data_health[zCount] = KvGetNum(kvZombies, "health", 500);
        
        data_speed[zCount] = KvGetFloat(kvZombies, "speed", 300.0);
        
        data_gravity[zCount] = KvGetFloat(kvZombies, "gravity", 1.0);
        
        data_jump[zCount] = KvGetFloat(kvZombies, "jump", 1.0);
        
        data_fov[zCount] = KvGetNum(kvZombies, "fov", 90);
        
        zCount++;
    } while (KvGotoNextKey(kvZombies));
}

void LoadModelData()
{
    char path[PLATFORM_MAX_PATH];
    BuildPath(Path_SM, path, sizeof(path), "configs/zriot/models.txt");
    
    Handle fileModels = OpenFile(path, "r");
    
    if (fileModels == INVALID_HANDLE)
    {
        SetFailState("\"%s\" missing from server", path);
    }
    
    if (adtModels != INVALID_HANDLE)
    {
        CloseHandle(adtModels);
    }
    
    adtModels = CreateArray(256, 0);
    
    FileLinesToArray(adtModels, fileModels);
    
    if (!GetArraySize(adtModels))
    {
        SetFailState("No models listed in models.txt, please add some models then restart");
    }
    
    char model[256];
    char modelpath[256];
    
    int modelsize = GetArraySize(adtModels);
    for (int x = 0; x < modelsize; x++)
    {
        for (int y = 0; y < 8; y++)
        {
            GetArrayString(adtModels, x, model, sizeof(model));
            Format(modelpath, sizeof(modelpath), "%s%s", model, modelSuffix[y]);
            
            if (FileExists(modelpath))
            {
                AddFileToDownloadsTable(modelpath);
            }
        }
    }
  
    CloseHandle(fileModels);
}

void LoadDownloadData()
{
    char path[PLATFORM_MAX_PATH];
    BuildPath(Path_SM, path, sizeof(path), "configs/zriot/downloads.txt");
    
    Handle fileDownloads = OpenFile(path, "r");
    
    if (fileDownloads == INVALID_HANDLE)
    {
        SetFailState("\"%s\" missing from server", path);
    }
    
    Handle arrayDownloads = CreateArray(256, 0);
    
    FileLinesToArray(arrayDownloads, fileDownloads);
    
    char file[256];
    
    int downloadsize = GetArraySize(arrayDownloads);
    for (int x = 0; x < downloadsize; x++)
    {
        GetArrayString(arrayDownloads, x, file, sizeof(file));
        if (FileExists(file))
        {
            AddFileToDownloadsTable(file);
        }
        else
        {
            ZRiot_LogMessage("File load failed", file);
        }
    }
  
    CloseHandle(fileDownloads);
    CloseHandle(arrayDownloads);
}

int FindZombieIDByName(const char[] name)
{
    for (int x = 0; x < zCount; x++)
    {
        if (StrEqual(name, data_name[x], false))
        {
            return x;
        }
    }
    
    return -1;
}

bool IsValidZombieID(int zombieid)
{
    if (zombieid > -1 && zombieid < zCount)
    {
        return true;
    }
    
    return false;
}

bool IsOverrideRequired(int zombieid)
{
    if (IsValidZombieID(zombieid))
    {
        return data_override_required[zombieid];
    }
    
    return false;
}

void ApplyZombieModel(int client, int zombieid)
{
    if (IsValidZombieID(zombieid))
    {
        char model[256];
        strcopy(model, sizeof(model), data_model[zombieid]);
        
        PrecacheModel(model);
        SetEntityModel(client, model);
    }
}

void GetZombieZVision(int zombieid, char[] zvision, int maxlen)
{
    if (IsValidZombieID(zombieid))
    {
        strcopy(zvision, maxlen, data_zvision[zombieid]);
    }
}

void ApplyZombieHealth(int client, int zombieid)
{
    if (IsValidZombieID(zombieid))
    {
        SetEntityHealth(client, data_health[zombieid]);
    }
}

void ApplyZombieSpeed(int client, int zombieid)
{
    if (IsValidZombieID(zombieid))
    {
        SetPlayerSpeed(client, data_speed[zombieid]);
    }
}

void ApplyZombieGravity(int client, int zombieid)
{
    if (IsValidZombieID(zombieid))
    {
        SetEntityGravity(client, data_gravity[zombieid]);
    }
}

float GetZombieJump(int zombieid)
{
    if (IsValidZombieID(zombieid))
    {
        return data_jump[zombieid];
    }
    
    return 0.0;
}

void ApplyZombieFOV(int client, int zombieid)
{
    if (IsValidZombieID(zombieid))
    {
        SetPlayerFOV(client, data_fov[zombieid]);
    }
}
