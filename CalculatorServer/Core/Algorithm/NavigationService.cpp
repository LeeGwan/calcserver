#include "NavigationService.h"
#include "Shape/Box/Box.h"
#include "Shape/Circle/Circle.h"
#include "../Object/Object/ObjectComponent/Object.h"
#include "../Object/Object/VMT/VMT.h"
#include "../Object/Object/struct_pos/struct_pos.h"
#include "../Navmesh/NavMesh.h"
#include "../Navmesh/Flags/Flags.h"
#include "../Navmesh/recastnavigation/Include/DetourNavMesh.h"
#include "../Navmesh/recastnavigation/Include/DetourNavMeshQuery.h"
#include "../Navmesh/recastnavigation/Include/DetourCommon.h"
#include "../Navmesh/recastnavigation/Include/DetourNavMeshBuilder.h"
#include "../AI/RLStructures/RLStructures.h"
#include <algorithm>
bool NavigationService::FindPath(dtNavMeshQuery* navQuery, const Vec3& startPos, const Vec3& endPos,
	const Vec3 characterSize, std::vector<Vec3>& output, unsigned short flags)
{
	std::lock_guard<std::mutex> lock(nav_mutex);
	if (!navQuery)
	{
		std::cerr << "NavQuery is null" << std::endl;
		return false;
	}

	// ��ǥ ��ȯ (Unreal -> Navigation)
	Vec3 navStartPos = startPos.to_Nav_POS();
	Vec3 navEndPos = endPos.to_Nav_POS();
	Vec3 navCharSize = { characterSize.x * 0.01f, characterSize.z * 0.01f, characterSize.y * 0.01f };

	// �� �浹 üũ �� ������ ���� ��ġ�� ����
	navEndPos = GetCheckEndPosition(navQuery, navStartPos, navEndPos, navCharSize, flags);

	// �׺���̼� ������
	dtQueryFilter filter;
	dtPolyRef startRef = 0, endRef = 0;
	dtPolyRef pathPolys[256];
	dtPolyRef straightPathPolys[256];
	int pathCount = 0;
	float straightPath[256 * 3];
	unsigned char straightPathFlags[256];
	int straightPathCount = 0;
	int straightPathOptions = 2;

	// ��ǥ �迭 ��ȯ
	float startPosArray[3] = { navStartPos.x, navStartPos.y, navStartPos.z };
	float endPosArray[3] = { navEndPos.x, navEndPos.y, navEndPos.z };
	float halfExtents[3] = { navCharSize.x, navCharSize.y, navCharSize.z };
	float nearestPt[3];
	float endPosAdjusted[3];

	// ���� ����
	filter.setIncludeFlags(PolyFlags::SAMPLE_POLYFLAGS_ALL);
	filter.setExcludeFlags(flags);

	// �������� ������ ������ ã��
	dtStatus startStatus = navQuery->findNearestPoly(startPosArray, halfExtents, &filter, &startRef, nearestPt);
	dtStatus endStatus = navQuery->findNearestPoly(endPosArray, halfExtents, &filter, &endRef, nearestPt);

	if (!startRef)
	{
		//std::cerr << "Cannot find start polygon at position: "
		 //   << startPos.x << ", " << startPos.y << ", " << startPos.z << std::endl;
		return false;
	}

	if (!endRef)
	{
		std::cerr << "Cannot find end polygon at adjusted position" << std::endl;
		return false;
	}

	// ��� ã��
	dtStatus pathStatus = navQuery->findPath(startRef, endRef, startPosArray, endPosArray,
		&filter, pathPolys, &pathCount, 256);

	if (dtStatusFailed(pathStatus) || pathCount == 0)
	{
		std::cerr << "Path finding failed" << std::endl;
		return false;
	}

	// ���� ����
	dtVcopy(endPosAdjusted, endPosArray);
	if (pathPolys[pathCount - 1] != endRef)
	{
		navQuery->closestPointOnPoly(pathPolys[pathCount - 1], endPosArray, endPosAdjusted, 0);
	}

	// ���� ��� ����
	dtStatus straightPathStatus = navQuery->findStraightPath(startPosArray, endPosAdjusted, pathPolys, pathCount,
		straightPath, straightPathFlags, straightPathPolys,
		&straightPathCount, 256, straightPathOptions);

	if (dtStatusFailed(straightPathStatus) || straightPathCount == 0)
	{

		return false;
	}

	// ����� Unreal ��ǥ��� ��ȯ
	output.clear();
	output.reserve(straightPathCount);

	for (int i = 0; i < straightPathCount; ++i)
	{
		Vec3 point(straightPath[i * 3], straightPath[i * 3 + 1], straightPath[i * 3 + 2]);
		output.push_back(point.to_Unreal_POS());
	}


	return true;
}

Vec3 NavigationService::GetCheckEndPosition(dtNavMeshQuery* navQuery, const Vec3& navStartPos,
	const Vec3& navEndPos, const Vec3& characterSize, unsigned short flags)
{
	dtQueryFilter filter;
	filter.setIncludeFlags(PolyFlags::SAMPLE_POLYFLAGS_ALL);
	filter.setExcludeFlags(PolyFlags::SAMPLE_POLYFLAGS_DISABLED | PolyFlags::Structure);

	dtPolyRef startRef = 0;
	dtPolyRef endRef = 0;
	float endPosArray[3] = { navEndPos.x, navEndPos.y, navEndPos.z };
	float halfExtents[3] = { characterSize.x, characterSize.y, characterSize.z };
	float nearestPt[3];

	// endPos �ֺ� ������ üũ
	navQuery->findNearestPoly(endPosArray, halfExtents, &filter, &endRef, nearestPt);

	if (!endRef) // �� �� ���� ���̸� raycast�� �浹 ���� ��ǥ ã��
	{
		float startPosArray[3] = { navStartPos.x, navStartPos.y, navStartPos.z };
		navQuery->findNearestPoly(startPosArray, halfExtents, &filter, &startRef, nearestPt);

		dtRaycastHit hit;
		dtStatus rayStatus = navQuery->raycast(startRef, startPosArray, endPosArray, &filter, 0, &hit);

		if (dtStatusSucceed(rayStatus) && hit.t < 1.0f)
		{
			// �浹 ���� ��ġ ��ȯ
			Vec3 direction = navEndPos - navStartPos;
			return navStartPos + direction * hit.t;
		}
	}

	return navEndPos;
}

void NavigationService::calculate_direction_safety(dtNavMesh* nav, dtNavMeshQuery* query, const Vec3& current_pos, float radius, float Hitbox, int player_team, float safety_scores[8], bool blocked[8])
{
	Vec3 currentPos = current_pos;
	for (int i = 0; i < 8; ++i) {
		Vec3 target_pos = currentPos.get_direction_offset(i, radius);
		TileInfo tile_info = analyze_tile_at_position(nav, query, target_pos, Hitbox, player_team);

		// �̵� ���� ����
		blocked[i] = !tile_info.is_walkable || tile_info.is_structure;

		// ������ ��� (1.0 = �ſ� ����, 0.0 = �ſ� ����)
		float safety = 1.0f - tile_info.danger_level;

		// ��� ���� (�̻��� + ���� ����)�̸� ������ 0
		if (tile_info.has_missile && tile_info.is_enemy_spell_area) {
			safety = 0.0f;
		}
		// ���� ���� �����̸� ������ ����
		else if (tile_info.is_enemy_spell_area) {
			safety *= 0.3f;
		}
		// �̻��ϸ� �־ ����
		else if (tile_info.has_missile) {
			safety *= 0.1f;
		}
		// �Ʊ� ���� �����̸� ��������� ����
		else if (tile_info.is_ally_spell_area) {
			safety = std::max(safety, 0.7f);
		}

		safety_scores[i] = safety;
	}
}
bool NavigationService::Find_Safe_Direction_Recursively(dtNavMesh* nav, dtNavMeshQuery* query, const Vec3& current_pos, float min_range,float max_range, float Hitbox, int player_team, std::vector<safeposINFO>& check)
{
	// ���� �ʰ��� ����
	if (min_range >= max_range)
	{
		return !check.empty();
	}

	Vec3 CurrentPos = current_pos;
	bool found_good_position = false;
	float best_safety_this_level = 0.0f;

	// ���� �Ÿ����� 8���� Ž��
	for (int i = 0; i < 8; ++i) {
		Vec3 target_pos = CurrentPos.get_direction_offset(i, min_range);
		TileInfo tile_info = analyze_tile_at_position(nav, query, target_pos, Hitbox, player_team);

		// �̵� �Ұ����� ���� ��ŵ
		bool blocked = !tile_info.is_walkable || tile_info.is_structure;
		if (blocked) continue;

		// ������ ���
		float safety = 1.0f - tile_info.danger_level;

		// ��� ���� (�̻��� + ���� ����)
		if (tile_info.has_missile && tile_info.is_enemy_spell_area) {
			safety = 0.0f;
		}
		// ���� ���� ����
		else if (tile_info.is_enemy_spell_area) {
			safety *= 0.3f;
		}
		// �̻��ϸ� �ִ� ���
		else if (tile_info.has_missile) {
			safety *= 0.1f;
		}
		// �Ʊ� ���� ����
		else if (tile_info.is_ally_spell_area) {
			safety = std::max(safety, 0.7f);
		}

		// ������ ������ ��ġ �߰�
		if (safety >= 1.0f) {
			check.push_back({ target_pos, safety, blocked });
			return true; // ���� ������ ���� ã�����Ƿ� ��� ��ȯ
		}
		// ������ ������ ��ġ
		else if (safety > 0.6f) {
			check.push_back({ target_pos, safety, blocked });
			found_good_position = true;
			best_safety_this_level = std::max(best_safety_this_level, safety);
		}
	}

	// ���� �������� ����� ���� ��ġ�� ã�Ҵٸ� (safety > 0.8) �� �ָ� ���� ����
	if (best_safety_this_level > 0.8f) {
		return true;
	}

	// ��� ȣ���Ͽ� �� �� �Ÿ� Ž��
	bool recursive_result = Find_Safe_Direction_Recursively(nav, query, current_pos, min_range+ Hitbox, max_range, Hitbox, player_team, check);

	// ���� �������� ã�Ұų� ��Ϳ��� ã������ true
	return found_good_position || recursive_result;
}
TileInfo NavigationService::analyze_tile_at_position(dtNavMesh* nav, dtNavMeshQuery* query, const Vec3& position, float hitbox, int player_team)
{
	TileInfo info;
	if (!nav || !query) return info;
	Vec3 Nav_position = position.to_Nav_POS();
	float pos[3] = { Nav_position.x, Nav_position.y, Nav_position.z };
	float halfExtents[3] = { hitbox * 0.01f, 10.0f, hitbox * 0.01f }; // �˻� �ݰ�
	dtQueryFilter filter;
	filter.setIncludeFlags(PolyFlags::SAMPLE_POLYFLAGS_ALL);
	dtPolyRef polyRef;
	float nearestPt[3];

	dtStatus status = query->findNearestPoly(pos, halfExtents, &filter, &polyRef, nearestPt);
	
	if (dtStatusSucceed(status) && polyRef != 0) {
		unsigned short flags;
		if (dtStatusSucceed(nav->getPolyFlags(polyRef, &flags))) {

			info = parse_poly_flags(flags, player_team);
			info.tilePos = position;
		}
	}

	return info;
}
// 8���� �ֺ� Ÿ�� �м�
void NavigationService::analyze_surrounding_tiles(dtNavMesh* nav, dtNavMeshQuery* query, const Vec3& center_pos, float radius, float hitbox, int player_team, TileInfo surrounding_tiles[8])
{
	Vec3 currentPos = center_pos;
	for (int i = 0; i < 8; ++i) {
		Vec3 check_pos = currentPos.get_direction_offset(i, radius);//�׿� �ش��� ��ǥ
		surrounding_tiles[i] = analyze_tile_at_position(nav, query, check_pos, hitbox, player_team);
		

	}
}
Vec3 NavigationService::GetTilePosition(dtNavMesh* nav, dtNavMeshQuery* query, const Vec3& position, float radius)const
{
	
	if (!nav || !query) return Vec3(0,0,0);
	Vec3 Nav_position = position.to_Nav_POS();
	float pos[3] = { Nav_position.x, Nav_position.y, Nav_position.z };
	float halfExtents[3] = { radius * 0.01f, 10.0f, radius * 0.01f }; // �˻� �ݰ�
	dtQueryFilter filter;
	filter.setIncludeFlags(PolyFlags::SAMPLE_POLYFLAGS_ALL);
	dtPolyRef polyRef;
	float nearestPt[3];
	Vec3 TilePos;
	dtStatus status = query->findNearestPoly(pos, halfExtents, &filter, &polyRef, nearestPt);
	
	if (dtStatusSucceed(status) && polyRef != 0) {
		return Vec3({ nearestPt[0], 0.0f, nearestPt[2] });
	}

	return Vec3(0, 0, 0);
}

int NavigationService::find_safest_direction(float safety_scores[8], bool blocked[8])
{
	int best_direction = -1;
	float best_safety = -1.0f;

	for (int i = 0; i < 8; ++i) {
		if (!blocked[i] && safety_scores[i] > best_safety) {
			best_safety = safety_scores[i];
			best_direction = i;
		}
	}

	return best_direction;
}

int NavigationService::find_approach_direction(const Vec3& current_pos, const Vec3& target_pos, float safety_scores[8], bool blocked[8], float min_safety)
{
	Vec3 to_target = (target_pos - current_pos).normalized();
	Vec3 currentPos = current_pos;
	int best_direction = -1;
	float best_score = -1.0f;

	for (int i = 0; i < 8; ++i) {
		if (blocked[i] || safety_scores[i] < min_safety) continue;

		Vec3 direction_vec = currentPos.get_direction_offset(i, min_safety);
		direction_vec = direction_vec.normalized();

		// ��ǥ ������� ���絵 ��� (����)
		float dot_product = direction_vec.x * to_target.x + direction_vec.y * to_target.y;

		// �������� ���� ������ ����
		float combined_score = safety_scores[i] * 0.6f + (dot_product + 1.0f) * 0.2f; // 0~1 ����ȭ

		if (combined_score > best_score) {
			best_score = combined_score;
			best_direction = i;
		}
	}

	return best_direction;
}

Vec3 NavigationService::FindsafePos(dtNavMesh* nav, dtNavMeshQuery* navQuery, const Vec3& currentPos, const Vec3& range,float search_radius, int player_team)//canmove_and_radius �� �̵��Ҽ��ִ°Ÿ��� ��Ʈ�ڽ�����
{
	TileInfo tileinfo;
	struct safePos_listInfo {
		TileInfo info;
		float distance;
		float safety;
	};
	std::vector<safePos_listInfo> safe_tile_list;
	const int MAX_POLYS = 256;
	dtPolyRef polys[MAX_POLYS];
	int polyCount = 0;
	float halfExtents[3] = { range.x * 0.01f, 10.0f, range.x * 0.01f };

	Vec3 Nav_currentPos = currentPos.to_Nav_POS();
	float navpos[3] = { Nav_currentPos.x, Nav_currentPos.y, Nav_currentPos.z };
	dtQueryFilter filter;
	filter.setIncludeFlags(SAMPLE_POLYFLAGS_ALL);
	dtStatus status = navQuery->queryPolygons(navpos, halfExtents, &filter, polys, &polyCount, MAX_POLYS);

	if (dtStatusSucceed(status)) {
		for (int i = 0; i < polyCount; ++i) {
			unsigned short flags;
			if (dtStatusSucceed(nav->getPolyFlags(polys[i], &flags))) {
				tileinfo = parse_poly_flags(flags, player_team);
				bool blocked = !tileinfo.is_walkable || tileinfo.is_structure;
				if (blocked)continue;
				float safety = 1.0f - tileinfo.danger_level;
				if (safety >= 0.8f) {
					const dtMeshTile* tile;
					const dtPoly* poly;
					if (dtStatusSucceed(nav->getTileAndPolyByRef(polys[i], &tile, &poly)) && poly->vertCount > 0) {
						float centerX = 0, centerY = 0.0f, centerZ = 0;
						for (int j = 0; j < poly->vertCount; ++j) {
							float* vertex = &tile->verts[poly->verts[j] * 3];
							centerX += vertex[0];
							centerZ += vertex[2];
						}
						centerX /= poly->vertCount;
						centerZ /= poly->vertCount;

						
						tileinfo.tilePos = Vec3(centerX, centerY, centerZ).to_Unreal_POS();
						float dist = tileinfo.tilePos.distance(currentPos);
						safe_tile_list.push_back({ tileinfo, dist,safety });
					}
				}
			}
		}
	}

	if (safe_tile_list.empty()) {
		return Vec3(0.0f, 0.0f, 0.0f);
	}

	// ������ ����: ������ ���� �� -> ������ �Ÿ� ����� ��
	std::sort(safe_tile_list.begin(), safe_tile_list.end(),
		[](const safePos_listInfo& a, const safePos_listInfo& b) {
			if (std::abs(a.safety - b.safety) < 0.01f) { // �������� ���� ������
				return a.distance < b.distance; // �Ÿ��� ����� �� �켱
			}
			return a.safety > b.safety; // �������� ���� �� �켱
		});

	return safe_tile_list[0].info.tilePos;
}

TileInfo NavigationService::parse_poly_flags(unsigned short flags, int player_team)
{
	TileInfo info;
	info.raw_flags = flags;

	// �⺻ �̵� ���� ����
	info.is_walkable = (flags & PolyFlags::WALK) != 0;

	// �̵� �Ұ� ����
	if (flags & PolyFlags::SAMPLE_POLYFLAGS_DISABLED) {
		info.is_walkable = false;
		info.danger_level = 1.0f;
		return info;
	}

	// ������
	info.is_structure = (flags & PolyFlags::Structure) != 0;
	if (info.is_structure) {
		info.is_walkable = false;
		info.danger_level = 1.0f;
		return info;
	}

	// ���� ���� ���� �Ǵ�
	if (player_team == 1) {
		info.is_ally_spell_area = (flags & PolyFlags::Team1) != 0;
		info.is_enemy_spell_area = (flags & PolyFlags::Team2) != 0;
	}
	else {
		info.is_ally_spell_area = (flags & PolyFlags::Team2) != 0;
		info.is_enemy_spell_area = (flags & PolyFlags::Team1) != 0;
	}

	// �̻��� ��ġ
	info.has_missile = (flags & PolyFlags::Missile_pos) != 0;

	// ���赵 ���
	float danger = 0.0f;

	// ��� ����: �̻��� + ���� ���� = �浹 ���� �Ǵ� �̹� �浹
	if (info.has_missile && info.is_enemy_spell_area) {
		danger = 1.0f; // �ִ� ����
	}
	// �̻��ϸ� �ִ� ���
	else if (info.has_missile) {
		danger = 0.8f; // ���� ����
	}
	// ���� ���� ����
	else if (info.is_enemy_spell_area) {
		danger = 0.6f; // �߰� ����
	}

	// ���赵 ���� �߰�
	if (flags & PolyFlags::DANGEROUS_Medium) {
		danger += 0.4f;
	}
	if (flags & PolyFlags::DANGEROUS_Low) {
		danger += 0.2f;
	}

	// �Ʊ� �����̸� ���赵 ����
	if (info.is_ally_spell_area) {
		danger *= 0.5f;
	}

	info.danger_level = (std::min)(danger, 1.0f);
	info.is_safe = (danger < 0.3f && info.is_walkable);

	return info;
}


