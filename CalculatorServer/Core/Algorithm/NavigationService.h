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
    // ��� ã�� ���� �Լ�
    bool FindPath(dtNavMeshQuery* navQuery, const Vec3& startPos, const Vec3& endPos,
        const Vec3 characterSize, std::vector<Vec3>& output, unsigned short flags = 0);
    Vec3 GetCheckEndPosition(dtNavMeshQuery* navQuery,
        const Vec3& navStartPos, const Vec3& navEndPos, 
        const Vec3& characterSize,
        unsigned short flags = 0);

  
    // �� ������ ������ ���
    void calculate_direction_safety(dtNavMesh* nav, dtNavMeshQuery* query, 
        const Vec3& current_pos, float search_radius, float Hitbox, int player_team,
        float safety_scores[8], bool blocked[8]);
    bool Find_Safe_Direction_Recursively(dtNavMesh* nav, dtNavMeshQuery* query, const Vec3& current_pos, float min_range, float max_range, float Hitbox, int player_team, std::vector<safeposINFO>& check);
    // Ư�� ��ġ�� Ÿ�� ���� �м�
    TileInfo analyze_tile_at_position(dtNavMesh* nav , dtNavMeshQuery* query,
        const Vec3& position, float search_radius, int player_team);
    // 8���� �ֺ� Ÿ�� �м�
    void analyze_surrounding_tiles(dtNavMesh* nav, dtNavMeshQuery* query, const Vec3& center_pos, 
        float radius,float hitbox, int player_team, TileInfo surrounding_tiles[8]);
    Vec3 GetTilePosition(dtNavMesh* nav, dtNavMeshQuery* query, const Vec3& position, float radius) const;
    // ���� ������ ���� ã��
    int find_safest_direction(float safety_scores[8], bool blocked[8]);
    // ������ ��������鼭 ������ ���� ã��
    int find_approach_direction(const Vec3& current_pos, const Vec3& target_pos,
        float safety_scores[8], bool blocked[8], float min_safety = 0.3f);
    Vec3 FindsafePos(dtNavMesh* nav, dtNavMeshQuery* navQuery, const Vec3& currentPos, const Vec3& range, float search_radius, int player_team);
    bool CheckMissileCollisionWithObject(dtNavMesh* navMesh, dtNavMeshQuery* navQuery,
        const float* objectPos, float canmove_and_radius);
private:
    mutable std::mutex nav_mutex;
    TileInfo parse_poly_flags(unsigned short flags, int player_team);
};
