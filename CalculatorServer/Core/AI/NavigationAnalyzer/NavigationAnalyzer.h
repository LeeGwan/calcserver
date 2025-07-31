#pragma once
struct NavigationStatusInfo;
class Object;
class NavMesh;
class Vec3;
class NavigationAnalyzer {
public:
	NavigationAnalyzer();
	~NavigationAnalyzer();
	NavigationStatusInfo analyze_navigation_status(NavMesh* navmesh, Object* ai_object, float deltatime);
	bool is_navigation_path_valid(Object* ai_object);
	bool advance_navigation_waypoint(Object* ai_object);
	void cancel_navigation(Object* ai_object);
	bool has_reached_target(Object* ai_object);
private:
	bool check_path_blocked(NavMesh* navmesh,const Vec3& from, const Vec3& to, Object* ai_object, float search_radius);
};