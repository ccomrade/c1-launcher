#ifndef CRYMODULEDEFS_H__
#define CRYMODULEDEFS_H__


enum ECryModule
{
  eCryM_3DEngine = 0,
  eCryM_Action,
  eCryM_AISystem,
  eCryM_Animation,
  eCryM_EntitySystem,
  eCryM_Font,
  eCryM_Input,
  eCryM_Movie,
  eCryM_Network,
  eCryM_Physics,
  eCryM_ScriptSystem,
  eCryM_SoundSystem,
  eCryM_System,
  eCryM_Game,
  eCryM_Render,
  eCryM_Launcher,

  eCryM_Num,
};

extern const char *CM_Name[eCryM_Num];

#endif //CRYMODULEDEFS_H__
