#pragma once
#include <vector>
#include <unordered_map>
#include <string>
#include <mutex>
#include <shared_mutex>
class dtNavMeshQuery;
class dtNavMesh;
struct Box;
class Vec3;
class Object;
struct TileInfo;
struct safeposINFO;
class NavigationService
{
public:
    // 경로 찾기 관련 함수
    bool FindPath(dtNavMeshQuery* navQuery, const Vec3& startPos, const Vec3& endPos,
        const Vec3 characterSize, std::vector<Vec3>& output, unsigned short flags = 0);
    Vec3 GetCheckEndPosition(dtNavMeshQuery* navQuery,
        const Vec3& navStartPos, const Vec3& navEndPos, 
        const Vec3& characterSize,
        unsigned short flags = 0);

  
    // 각 방향의 안전도 계산
    void calculate_direction_safety(dtNavMesh* nav, dtNavMeshQuery* query, 
        const Vec3& current_pos, float search_radius, float Hitbox, int player_team,
        float safety_scores[8], bool blocked[8]);
    bool Find_Safe_Direction_Recursively(dtNavMesh* nav, dtNavMeshQuery* query, const Vec3& current_pos, float min_range, float max_range, float Hitbox, int player_team, std::vector<safeposINFO>& check);
    // 특정 위치의 타일 정보 분석
    TileInfo analyze_tile_at_position(dtNavMesh* nav , dtNavMeshQuery* query,
        const Vec3& position, float search_radius, int player_team);
    // 8방향 주변 타일 분석
    void analyze_surrounding_tiles(dtNavMesh* nav, dtNavMeshQuery* query, const Vec3& center_pos, 
        float radius,float hitbox, int player_team, TileInfo surrounding_tiles[8]);
    Vec3 GetTilePosition(dtNavMesh* nav, dtNavMeshQuery* query, const Vec3& position, float radius) const;
    // 가장 안전한 방향 찾기
    int find_safest_direction(float safety_scores[8], bool blocked[8]);
    // 적에게 가까워지면서 안전한 방향 찾기
    int find_approach_direction(const Vec3& current_pos, const Vec3& target_pos,
        float safety_scores[8], bool blocked[8], float min_safety = 0.3f);
    Vec3 FindsafePos(dtNavMesh* nav, dtNavMeshQuery* navQuery, const Vec3& currentPos, const Vec3& range, float search_radius, int player_team);
    bool CheckMissileCollisionWithObject(dtNavMesh* navMesh, dtNavMeshQuery* navQuery,
        const float* objectPos, float canmove_and_radius);
private:
    mutable std::mutex nav_mutex;
    TileInfo parse_poly_flags(unsigned short flags, int player_team);
};
