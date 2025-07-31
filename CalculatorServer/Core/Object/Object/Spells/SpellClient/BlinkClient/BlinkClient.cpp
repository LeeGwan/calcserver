#include "BlinkClient.h"
#include "../../../ObjectComponent/Object.h"
#include "../../../Transform/Transform.h"
#include "../../../objectState/objectState.h"
#include "../../../../../utils/Vector.h"
#include"../../../Flags.h"

void BlinkClient::OnProcessSpellBlink(Object* caster, const Vec3& endPos)
{
	auto& castet_Transform = caster->Get_Transform_Component();
	float angle = castet_Transform.Get_Position().GetSignedAngleBetween(endPos);
	castet_Transform.Set_Position(endPos);
	castet_Transform.Set_Rotation(Vec3(0.0f, 0.0f, angle));
	caster->Get_objectState_Component().Set_CurrentOrderType(Enums::CurrentOrderType::Idle);

}
