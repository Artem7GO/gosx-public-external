//
//  GlowEsp.cpp
//  GO:SX v3
//
//  Created by Andre Kalisch on 25.01.17.
//  Copyright © 2017 Andre Kalisch. All rights reserved.
//

#include "GlowEsp.h"

cGlowEsp::cGlowEsp(cEngine* engineFactory)  {
    engine = engineFactory;
    bsp = engine->GetBspParser();
}

void cGlowEsp::applyGlow() {
    cEngineClient* engineClient = engine->GetEngineClient();
    cEntityManager* LocalPlayer = engine->GetLocalEntity();
    cSettingsManager* settings = engine->GetSettingsManager();
    cGlowManager* glowManager = engine->GetGlowManager();
    
    bool isGotvMode = true;
    if(LocalPlayer->isValidLivePlayer()) {
        isGotvMode = false;
    }
    
    for (int i = 1; i < engineClient->getMaxPlayers(); i++) {
        cEntityManager* entity = engine->GetEntity(i);
        if (entity->isValidLivePlayer()) {
            if(!settings->GetTeamglow() && LocalPlayer->GetTeam() == entity->GetTeam()) {
                continue;
            }
            
            sGlowEntity glow = entity->GetGlowObject();
            if(!glow.isValidGlowEntity()) {
                continue;
            }
            
            float alpha = settings->GetGlowalpha();
            std::string colorBase;
            if(!isGotvMode) {
                bool entityIsVisible = bsp->isVisible(LocalPlayer->GetPositionOffset(), entity->GetPositionOffset());
                if(entity->GetTeam() == TEAM_T) {
                    colorBase = settings->GetColorTvisible();
                    if (entityIsVisible) {
                        colorBase = settings->GetColorT();
                    }
                } else {
                    colorBase = settings->GetColorCTvisible();
                    if(entityIsVisible) {
                        colorBase = settings->GetColorCT();
                    }
                }
            } else {
                if(glow.RenderWhenOccluded) {
                    continue;
                }
                alpha = 0.65f;
                if (entity->GetTeam() == TEAM_CT) {
                    colorBase = "114,154,221";
                } else {
                    colorBase = "231,190,90";
                }
            }
            
            std::vector<std::string> colors = settings->split<std::string>(colorBase, ",");
            float red = atof(colors[0].c_str()) / 255;
            float green = atof(colors[1].c_str()) / 255;
            float blue = atof(colors[2].c_str()) / 255;
            
            glow.r = red;
            glow.g = green;
            glow.b = blue;
            glow.a = alpha;
            glow.RenderWhenOccluded = true;
            glow.RenderWhenUnoccluded = false;
            
            entity->setGlow(glow);
        }
        delete entity;
    }
    
    if(!isGotvMode) {
        for(int i = 0; i < glowManager->GetGlowCount(); i++) {
            sGlowEntity gEntity = glowManager->GetGlowEntity(i);
            if(gEntity.isValidGlowEntity()) {
                cEntityManager* entity = engine->DefineGlowEntity(gEntity.entityPointer);
                if(entity->isValidGlowEntity()) {
                    bool isWeapon = entity->isWeapon();
                    bool isChicken = entity->isChicken();
                    bool isBomb = entity->isBomb();
                    
                    if((isWeapon || isBomb) && gEntity.RenderWhenOccluded) {
                        continue;
                    }
                    
                    float alpha = settings->GetGlowalpha();
                    std::string colorBase;
                    if (isWeapon) {
                        colorBase = "255,0,0";
                    } else if (isBomb) {
                        colorBase = "0,255,0";
                    } else if (isChicken) {
                        colorBase = "255,255,255";
                    } else {
                        continue;
                    }
                    
                    std::vector<std::string> colors = settings->split<std::string>(colorBase, ",");
                    float red = atof(colors[0].c_str()) / 255;
                    float green = atof(colors[1].c_str()) / 255;
                    float blue = atof(colors[2].c_str()) / 255;
                    
                    gEntity.r = red;
                    gEntity.g = green;
                    gEntity.b = blue;
                    gEntity.a = alpha;
                    gEntity.RenderWhenOccluded = true;
                    gEntity.RenderWhenUnoccluded = false;
                    
                    entity->setGlow(gEntity, i);
                }
                delete entity;
            }
        }
    }
}
