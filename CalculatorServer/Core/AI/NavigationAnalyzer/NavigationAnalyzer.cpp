#include "NavigationAnalyzer.h"
#include"../RLStructures/RLStructures.h"
#include "../../Object/Object/ObjectComponent/Object.h"
#include "../../Object/Object/Navgation/Navigation.h"
#include "../../Navmesh/NavMesh.h"
#include "../../Object/Object/objectStats/objectStats.h"
#include "../../utils/Vector.h"
#include "../../Algorithm/NavigationService.h"
#include "../../Object/Object/objectState/objectState.h"
#include "../../Object/Object/TargetSelector/TargetSelector.h"
#include "../../Object/Object/CharacterMesh/CharacterMesh.h"
NavigationAnalyzer::NavigationAnalyzer() = default;

NavigationAnalyzer::~NavigationAnalyzer() = default;
NavigationStatusInfo NavigationAnalyzer::analyze_navigation_status(NavMesh* navmesh, Object* ai_object, float deltatime)
{
	NavigationStatusInfo nav_info;
	float base_buffer = 0.05f;      // 최소 5cm 보장
	float ratio_buffer = 1.2f;      // 120% 비율
	float min_radius = 0.1f;        // 최소 반경
	float max_radius = 1.0f;        // 최대 반경
	float search_radius = 0.0f;
	float hit_box = ai_object->Get_CharacterMesh_Component().Get_CapsuleSIze().x;
	try {
		Navigation* navigation = ai_object->Get_Navigation_Component();
		if (!navigation|| !navmesh) {
			return nav_info; // 기본값 반환
		}
		std::vector<Vec3> nav_path = navigation->Get_NaviPath();
		int current_waypoint = navigation->Get_Current_NaviPath_Index();

		nav_info.has_navigation_path = !nav_path.empty();
		nav_info.total_waypoints = static_cast<int>(nav_path.size());
		nav_info.current_waypoint_index = current_waypoint;
		if (nav_info.has_navigation_path && current_waypoint < nav_path.size()) {
			Vec3 current_pos = ai_object->Get_Transform_Component().Get_Position();

			// 현재 목표점과 최종 목적지 설정
			nav_info.current_target = nav_path[current_waypoint];
			nav_info.final_destination = nav_path[nav_path.size() - 1];

			// 거리 계산
			nav_info.distance_to_target = current_pos.distance(nav_info.current_target);
			nav_info.distance_to_final = current_pos.distance(nav_info.final_destination);

			// 진행률 계산
			if (nav_info.total_waypoints > 1) {
				nav_info.progress_ratio = static_cast<float>(current_waypoint) /
					static_cast<float>(nav_info.total_waypoints - 1);
			}
			else {
				nav_info.progress_ratio = 0.0f;
			}
			float movement_speed = ai_object->Get_ObjectStats_Component().Get_total_MovementSpeed();
			float distance_this_frame_cm = movement_speed * deltatime;
			search_radius = (distance_this_frame_cm * ratio_buffer) + base_buffer;
			search_radius = std::max(search_radius, min_radius);
			search_radius = std::min(search_radius, max_radius);
			nav_info.estimated_travel_time = nav_info.distance_to_final / std::max(movement_speed, 1.0f);
			nav_info.is_path_blocked = check_path_blocked(navmesh,current_pos, nav_info.current_target, ai_object, search_radius+ hit_box);
		}
		else {
			// 경로가 없거나 인덱스가 잘못된 경우
			Vec3 current_pos = ai_object->Get_Transform_Component().Get_Position();
			nav_info.current_target = current_pos;
			nav_info.final_destination = current_pos;
			nav_info.distance_to_target = 0.0f;
			nav_info.distance_to_final = 0.0f;
			nav_info.progress_ratio = 1.0f;
			nav_info.is_path_blocked = false;
			nav_info.estimated_travel_time = 0.0f;
		}

	}
	catch (const std::exception& e) {
		std::cerr << "[RL] Error analyzing navigation: " << e.what() << std::endl;
	}
	return nav_info;
}

bool NavigationAnalyzer::is_navigation_path_valid(Object* ai_object)
{
	Navigation* navigation = ai_object->Get_Navigation_Component();
	if (!navigation) return false;

	std::vector<Vec3> nav_path = navigation->Get_NaviPath();
	int current_waypoint = navigation->Get_Current_NaviPath_Index();

	return !nav_path.empty() && current_waypoint < nav_path.size() && current_waypoint >= 0;
}

bool NavigationAnalyzer::advance_navigation_waypoint(Object* ai_object)
{
	Navigation* navigation = ai_object->Get_Navigation_Component();
	if (!navigation) return false;

	std::vector<Vec3> nav_path = navigation->Get_NaviPath();
	int current_waypoint = navigation->Get_Current_NaviPath_Index();

	if (current_waypoint + 1 < nav_path.size()) {
		navigation->Add_Current_NaviPath_Index(1);
		return true;
	}

	return false; // 마지막 웨이포인트에 도달
}

void NavigationAnalyzer::cancel_navigation(Object* ai_object)
{
	Navigation* navigation = ai_object->Get_Navigation_Component();
	if (navigation) {

		ai_object->Get_objectState_Component()
			.Set_CurrentOrderType(Enums::CurrentOrderType::Idle);
			ai_object->Get_objectState_Component().Set_AttackTime(0.0f);
		ai_object->Get_TargetSelector_Component()->clear();
		navigation->clear();

	}
}

bool NavigationAnalyzer::has_reached_target(Object* ai_object)
{
	Navigation* navigation = ai_object->Get_Navigation_Component();
	if (!navigation) return true;

	std::vector<Vec3> nav_path = navigation->Get_NaviPath();
	int current_waypoint = navigation->Get_Current_NaviPath_Index();

	if (nav_path.empty() || current_waypoint >= nav_path.size()) {
		return true;
	}
	Vec3 current_pos = ai_object->Get_Transform_Component().Get_Position();
	Vec3 target_pos = nav_path[current_waypoint];

	return current_pos.distance(target_pos) <= 0.1f;
}

bool NavigationAnalyzer::check_path_blocked(NavMesh* navmesh, const Vec3& from, const Vec3& to, Object* ai_object, float search_radius)
{
	Vec3 direction = (to - from).normalized();
	float total_distance = from.distance(to);
	float step_size = 50.0f; // 50 유닛씩 체크

	int team = ai_object->Get_Team();
	for (float dist = step_size; dist < total_distance; dist += step_size) {
		Vec3 check_pos = from + direction * dist;
		
		TileInfo tile_info = navmesh->Get_NavigationService()->analyze_tile_at_position(
			navmesh->Get_LeagueNavMesh(),navmesh->Get_navQuery(), check_pos, 1.0f, team);

		// 이동 불가하거나 매우 위험하면 차단된 것으로 판단
		if (!tile_info.is_walkable || tile_info.danger_level > 0.8f) {
			return true;
		}
	}
	return false;
}
