#include "Level_InputComponent.h"
#include "../../../Object/Object/ObjectComponent/Object.h"

#include "../../../Object/Object/objectState/objectState.h"
#include "../../../utils/Find_OBJ/Find_OBJ.h"
/*
ErrorCode::Level_Input_ErrorType Level_InputComponent::Routine_Level_InputComponent_init(const std::unordered_map<std::string, Object*>& OBJ, const To_Server_LevelUpSpell& task)
{
	ErrorCode::Level_Input_ErrorType Return_Code = ErrorCode::Level_Input_ErrorType::Level_Input_Successed;
	std::string This_Hash=task.ObjectHash;
	Object* this_hero = nullptr;
	if (This_Hash.empty())return ErrorCode::Level_Input_ErrorType::Level_Input_Error_Dont_have_hash;

	if (Find_OBJ::Get_OBJ_To_Use_Hash(OBJ, This_Hash, this_hero))
	{
		Return_Code = ErrorCode::Level_Input_ErrorType::Level_Input_Successed;
	}
	else
	{
		Return_Code = ErrorCode::Level_Input_ErrorType::Level_Input_Error_Dont_have_OBJ;
	}
	if (Return_Code != ErrorCode::Level_Input_ErrorType::Level_Input_Successed)return Return_Code;
	if (!this_hero->Get_objectState_Component().Has_Anti_Check_Level())return  ErrorCode::Level_Input_ErrorType::Anti_Hack_Check_Level_Input;
	this_hero->Get_objectState_Component().Set_Anti_Check_Level(false);


}
*/