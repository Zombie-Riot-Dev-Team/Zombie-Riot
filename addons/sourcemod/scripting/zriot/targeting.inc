/**
 * ====================
 *     Zombie Riot
 *   File: targeting.inc
 *   Author: Greyscale
 * ====================
 */

int gTarget[MAXPLAYERS+1];

bool bTargeted[MAXPLAYERS+1][MAXPLAYERS+1];

void TargetPlayer(int attacker, int client)
{
    if (!IsClientInGame(attacker) || !IsClientInGame(client))
    {
        return;
    }
    
    gTarget[attacker] = client;
    
    bTargeted[client][attacker] = true;
}

int GetClientTarget(int client)
{
    if (gTarget[client] == -1 || !IsClientInGame(gTarget[client]))
    {
        return -1;
    }
    
    return gTarget[client];
}

int GetClientTargeters(int client, char[] clients, int maxClients)
{
    int count = 0;
    for (int x = 1; x <= maxClients; x++)
    {
        if (!IsClientInGame(x) || !bTargeted[client][x])
        {
            continue;
        }
        
        clients[count++] = x;
    }
    
    return count;
}

int FindClientNextTarget(int client)
{
    int maxplayers = MaxClients;
    for (int x = 1; x <= maxplayers; x++)
    {
        if (!IsClientInGame(x))
        {
            continue;
        }
        
        if (bTargeted[x][client])
        {
            return x;
        }
    }
    
    return -1;
}

void RemoveTargeters(int client)
{
    int maxplayers = MaxClients;
    for (int x = 1; x <= maxplayers; x++)
    {
        bTargeted[client][x] = false;
        
        if (gTarget[x] == client)
        {
            gTarget[x] = FindClientNextTarget(x);
        }
    }
}
