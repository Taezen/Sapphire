#include <Script/NativeScriptApi.h>
#include <ScriptObject.h>
#include <Zone/InstanceContent.h>

class EngageEnemyReinforcements : public InstanceContentScript
{
public:
   EngageEnemyReinforcements() : InstanceContentScript( 15011 )
   { }

   void onInit( InstanceContentPtr instance ) override
   {
      
      instance->registerEObj( "Unknown0", 2007001, 0, 4, { -5.135936f, -0.986339f, -0.059807f }, 1.000000f );
      instance->registerEObj( "Unknown1", 2007004, 0, 4, { -6.501123f, -0.986346f, 2.121102f }, 1.000000f );
      instance->registerEObj( "Unknown2", 2007005, 0, 4, { -5.178086f, -0.986341f, -0.227772f }, 1.000000f );
      instance->registerEObj( "Unknown3", 2007006, 0, 4, { -7.272448f, -0.986346f, -2.204096f }, 1.000000f );
      instance->registerEObj( "Unknown4", 2007007, 6216275, 4, { -30.849630f, 0.697522f, -1.415699f }, 1.000000f );
      instance->registerEObj( "Gatelever", 2007008, 0, 4, { -31.199289f, 1.850766f, -7.087492f }, 1.000000f );
      instance->registerEObj( "Entrance", 2000182, 6215534, 5, { 44.100491f, 1.056062f, 1.068363f }, 1.000000f );
      instance->registerEObj( "Unknown6", 2001403, 3968682, 4, { 30.986059f, 0.993235f, 1.480142f }, 1.000000f );
      instance->registerEObj( "Unknown7", 2001404, 3968585, 4, { -31.243280f, 1.043213f, 1.580577f }, 1.000000f );
   }

   void onUpdate( InstanceContentPtr instance, uint32_t currTime ) override
   {

   }

   void onEnterTerritory( Entity::Player &player, uint32_t eventId, uint16_t param1, uint16_t param2 ) override
   {

   }

};