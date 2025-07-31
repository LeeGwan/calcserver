#include "SkillDetected.h"
#include <algorithm>
#include <cstring>
#include"../../Error/is_valid.h"
#include "../../utils/Vector.h"
#include"../../Navmesh/Flags/Flags.h"
#include "../../Navmesh/recastnavigation/Include/DetourNavMeshQuery.h"
#include "../../Navmesh/recastnavigation/Include/DetourCommon.h"
#include "../../Navmesh/recastnavigation/Include/DetourNavMeshBuilder.h"
#include "../../Object/Object/ObjectComponent/Object.h"
#include "../../Object/Object/VMT/VMT.h"
#include "../../Object/Object/Transform/Transform.h"
#include"../../Object/Object/CharacterMesh/CharacterMesh.h"
#include <Windows.h>
PolyFlagInfo::PolyFlagInfo()
{
}
PolyFlagInfo::~PolyFlagInfo() = default;

// PolyFlagInfo 구현
dtPolyRef PolyFlagInfo::GetPolyRef() const {
	std::shared_lock lock(m_PolyFlagInfoMutex);
	return polyRef;
}

unsigned short PolyFlagInfo::GetOriginalFlags() const {
	std::shared_lock lock(m_PolyFlagInfoMutex);
	return originalFlags;
}

size_t PolyFlagInfo::GetActiveSkillsCount() const {
	std::shared_lock lock(m_PolyFlagInfoMutex);
	return activeSkills.size();
}

bool PolyFlagInfo::HasActiveSkills() const {
	std::shared_lock lock(m_PolyFlagInfoMutex);
	return !activeSkills.empty();
}

void PolyFlagInfo::SetPolyRef(dtPolyRef ref) {
	std::unique_lock lock(m_PolyFlagInfoMutex);
	polyRef = ref;
}

void PolyFlagInfo::SetOriginalFlags(unsigned short flags) {
	std::unique_lock lock(m_PolyFlagInfoMutex);
	originalFlags = flags;
}

void PolyFlagInfo::AddOrUpdateSkill(const SkillEffect& skill) {
	std::unique_lock lock(m_PolyFlagInfoMutex);
	activeSkills[skill.skillId] = skill;
}

bool PolyFlagInfo::RemoveSkill(unsigned int skillId) {
	std::unique_lock lock(m_PolyFlagInfoMutex);
	return activeSkills.erase(skillId) > 0;
}

void PolyFlagInfo::ClearSkills() {
	std::unique_lock lock(m_PolyFlagInfoMutex);
	activeSkills.clear();
}

bool PolyFlagInfo::UpdateSkillTimes(float deltaTime) {
	std::unique_lock lock(m_PolyFlagInfoMutex);
	bool wasModified = false;

	for (auto it = activeSkills.begin(); it != activeSkills.end();) {
		it->second.remainingTime -= deltaTime;
		if (it->second.remainingTime <= 0.0f) {
			it = activeSkills.erase(it);
			wasModified = true;
		}
		else {
			++it;
		}
	}
	return wasModified;
}

unsigned short PolyFlagInfo::GetCombinedDangerLevel() const {
	std::shared_lock lock(m_PolyFlagInfoMutex);
	unsigned short combinedDanger = 0;
	for (const auto& pair : activeSkills) {
		combinedDanger |= pair.second.dangerousLevel;
	}
	return combinedDanger;
}

MovingSkillInfo::MovingSkillInfo()
{
}

MovingSkillInfo::~MovingSkillInfo() = default;


// MovingSkillInfo 구현
unsigned int MovingSkillInfo::GetSkillId() const {
	std::shared_lock lock(m_MovingSkillInfoMutex);
	return skillId;
}

void MovingSkillInfo::GetStartPos(float pos[3]) const {
	std::shared_lock lock(m_MovingSkillInfoMutex);
	memcpy(pos, startPos, sizeof(float) * 3);
}

void MovingSkillInfo::GetEndPos(float pos[3]) const {
	std::shared_lock lock(m_MovingSkillInfoMutex);
	memcpy(pos, endPos, sizeof(float) * 3);
}

float MovingSkillInfo::GetRadius() const {
	std::shared_lock lock(m_MovingSkillInfoMutex);
	return radius;
}

unsigned short MovingSkillInfo::GetDangerousLevel() const {
	std::shared_lock lock(m_MovingSkillInfoMutex);
	return dangerousLevel;
}

float MovingSkillInfo::GetDuration() const {
	std::shared_lock lock(m_MovingSkillInfoMutex);
	return duration;
}

std::vector<dtPolyRef> MovingSkillInfo::GetCurrentPolys() const {
	std::shared_lock lock(m_MovingSkillInfoMutex);
	return std::vector<dtPolyRef>(currentPolys.begin(), currentPolys.end());
}

size_t MovingSkillInfo::GetCurrentPolysCount() const {
	std::shared_lock lock(m_MovingSkillInfoMutex);
	return currentPolys.size();
}

unsigned long MovingSkillInfo::GetcollisionObj() const
{
	std::shared_lock lock(m_MovingSkillInfoMutex);
	return collisionobject;
}

void MovingSkillInfo::SetSkillId(unsigned int id) {
	std::unique_lock lock(m_MovingSkillInfoMutex);
	skillId = id;
}

void MovingSkillInfo::SetStartPos(const float pos[3]) {
	std::unique_lock lock(m_MovingSkillInfoMutex);
	memcpy(startPos, pos, sizeof(float) * 3);
}

void MovingSkillInfo::SetEndPos(const float pos[3]) {
	std::unique_lock lock(m_MovingSkillInfoMutex);
	memcpy(endPos, pos, sizeof(float) * 3);
}

void MovingSkillInfo::SetRadius(float r) {
	std::unique_lock lock(m_MovingSkillInfoMutex);
	radius = r;
}

void MovingSkillInfo::SetDangerousLevel(unsigned short level) {
	std::unique_lock lock(m_MovingSkillInfoMutex);
	dangerousLevel = level;
}

void MovingSkillInfo::SetDuration(float dur) {
	std::unique_lock lock(m_MovingSkillInfoMutex);
	duration = dur;
}

void MovingSkillInfo::SetcollisionObj(const unsigned long& in_collisionobject)
{
	std::unique_lock lock(m_MovingSkillInfoMutex);
	collisionobject &= 0;
	collisionobject = in_collisionobject;
}

bool MovingSkillInfo::AddCurrentPoly(dtPolyRef polyRef) {
	std::unique_lock lock(m_MovingSkillInfoMutex);
	return currentPolys.insert(polyRef).second;
}

bool MovingSkillInfo::RemoveCurrentPoly(dtPolyRef polyRef) {
	std::unique_lock lock(m_MovingSkillInfoMutex);
	return currentPolys.erase(polyRef) > 0;
}

void MovingSkillInfo::ClearCurrentPolys() {
	std::unique_lock lock(m_MovingSkillInfoMutex);
	currentPolys.clear();
}

void MovingSkillInfo::SetAllData(unsigned int id, const float start[3], const float end[3],
	float r, unsigned short level, float dur, unsigned long incollisionobject) {
	std::unique_lock lock(m_MovingSkillInfoMutex);
	skillId = id;
	memcpy(startPos, start, sizeof(float) * 3);
	memcpy(endPos, end, sizeof(float) * 3);
	radius = r;
	dangerousLevel = level;
	duration = dur;
	collisionobject = incollisionobject;
	currentPolys.clear();
}

// SkillDetected 구현
SkillDetected::SkillDetected()
{
}

unsigned int SkillDetected::ApplySkillDangerousArea(dtNavMesh* navMesh, dtNavMeshQuery* navQuery,
	const float* startPos, const float* endPos, float radius,
	unsigned short dangerousLevel, float duration)
{
	if (!navMesh) return 0;

	// 구현 필요 시 추가
	return 0;
}

bool SkillDetected::CreateMovingSkill(dtNavMesh* navMesh, dtNavMeshQuery* navQuery,
	const float* startPos, const float* endPos, float radius,
	unsigned short dangerousLevel, float duration, unsigned int in_skillid, unsigned long incollisionobject)
{
	if (!navMesh) return false;

	unsigned int skillId = in_skillid;

	// 이동 스킬 정보 저장
	std::unique_ptr<MovingSkillInfo> skillInfo = std::make_unique<MovingSkillInfo>();

	skillInfo->SetAllData(skillId, startPos, endPos, radius, dangerousLevel, duration, incollisionobject);

	// 초기 위치에서 폴리곤들 찾기 및 적용
	std::vector<dtPolyRef> affectedPolys;
	FindPolysInSkillRange(navQuery, startPos, endPos, radius, affectedPolys);

	for (dtPolyRef polyRef : affectedPolys)
	{
		{
			std::shared_lock lock(m_modifiedPolysmutex);
		
			if (ApplyDangerousFlagToPoly(navMesh, polyRef, skillId, dangerousLevel, duration))
			{
				skillInfo->AddCurrentPoly(polyRef);
			}
		}
	}

	m_movingSkills.insert({ skillId, std::move(skillInfo) });

	return true;
}
void SkillDetected::FindPolysInCircularRange_NoFilter(dtNavMeshQuery* navQuery, const float* center, float radius,
	std::vector<dtPolyRef>& outPolys)
{
	if (!navQuery) {

		return;
	}

	outPolys.clear();

	const int MAX_POLYS = 256;
	dtPolyRef polys[MAX_POLYS];
	int polyCount = 0;




	// 원형 범위로 폴리곤 검색
	float halfExtents[3] = { radius, 10.0f, radius };

	dtQueryFilter filter;
	filter.setIncludeFlags(SAMPLE_POLYFLAGS_ALL);
	filter.setExcludeFlags(SAMPLE_POLYFLAGS_DISABLED);

	dtStatus status = navQuery->queryPolygons(center, halfExtents, &filter, polys, &polyCount, MAX_POLYS);

	if (dtStatusSucceed(status)) {
		outPolys.reserve(polyCount);

	

		// === circular check 비활성화 - 모든 폴리곤 유지 ===
		for (int i = 0; i < polyCount; ++i) {
			outPolys.push_back(polys[i]);
		}

	
	}

}
float SkillDetected::CheckMissileCollisionWithObject(dtNavMesh* navMesh, dtNavMeshQuery* navQuery,
	const float* objectPos, float radius)
{
	if (!navQuery) return -1.0f;

	std::vector<dtPolyRef> nearbyPolys;
	// === 필터 없는 버전 사용 ===
	FindPolysInCircularRange_NoFilter(navQuery, objectPos, radius, nearbyPolys);

	if (nearbyPolys.empty()) return -1.0f;

	for (dtPolyRef polyRef : nearbyPolys) {
		const dtMeshTile* tile = nullptr;
		const dtPoly* poly = nullptr;

		if (dtStatusFailed(navMesh->getTileAndPolyByRef(polyRef, &tile, &poly)))
			continue;

		unsigned short currentFlags = 0;
		if (dtStatusSucceed(navMesh->getPolyFlags(polyRef, &currentFlags)))
		{
			if (currentFlags & PolyFlags::Missile_pos)
			{
				const float* verts = tile->verts;
				Vec3 objPos(objectPos[0], objectPos[1], objectPos[2]);
				Vec3 polyCenter = CalculatePolygonCenter(poly, verts);
				return polyCenter.Nav_distance(objPos);
			}
		}
	}
	return -1.0f;
	
}
bool SkillDetected::CheckMultipleObjectsMissileCollision(dtNavMesh* navMesh,dtNavMeshQuery* navQuery,
	const std::unordered_map<std::string, Object*>& objects, Object* caster, Object*& target,unsigned long collisionObj)
{

	//시간 남으면 스펠 충돌 검사 후 삭제 여부를 처리할 예정()Set_Collision_Object(CollisionObject::YasuoWall | CollisionObject::Heroes | CollisionObject::Minions)
	struct HitObjectInfo {
		Object* object;
		float distance;
	};
	std::vector<HitObjectInfo> hitObjects;
	Vec3 Nav_objPos;
	Vec3 Nav_Radius;
	for (const auto& pair : objects)
	{
		Object* obj = pair.second;
		if (!MemoryCheck::is_valid<Object>(obj) || obj->Get_VMT_Component()->is_dead()||
			obj->Get_VMT_Component()->is_structure() ||
			!caster->Get_VMT_Component()->is_enemy(obj)) continue;
		Nav_objPos = obj->Get_Transform_Component().Get_Position().to_Nav_POS();
		float objPos[3] = { Nav_objPos.x,Nav_objPos.y,Nav_objPos.z };
		Nav_Radius = obj->Get_CharacterMesh_Component().Get_CapsuleSIze();
		float distance = CheckMissileCollisionWithObject(navMesh, navQuery, objPos, Nav_Radius.x * 0.01f);
		if (distance!=-1.0f)
		{
			hitObjects.push_back({ obj ,distance });
		
		}
	}
	if (hitObjects.empty())return false;

	std::sort(hitObjects.begin(), hitObjects.end(),
		[](const HitObjectInfo& a, const HitObjectInfo& b) {
			return a.distance < b.distance;
		});

	target = hitObjects[0].object;

	return true;
}
bool SkillDetected::UpdateMovingSkillPosition(Object* caster,Object*& target, const std::unordered_map<std::string, Object*>& objects, dtNavMesh* navMesh, dtNavMeshQuery* navQuery,
	unsigned int skillId, const float* newStartPos, float radius)
{
	auto it = m_movingSkills.find(skillId);
	if (it == m_movingSkills.end()) return false;

	MovingSkillInfo* skillInfo = it->second.get();
	unsigned long collisionObj = skillInfo->GetcollisionObj();
	// 이전 폴리곤 수 기록
	int oldPolyCount = skillInfo->GetCurrentPolysCount();
	
	// 기존 위치의 폴리곤들에서 이 스킬 제거
	std::vector<dtPolyRef> currentPolys = skillInfo->GetCurrentPolys();
	for (dtPolyRef polyRef : currentPolys)
	{
		RemoveSkillFromPoly(navMesh, polyRef, skillId);
	}
	skillInfo->ClearCurrentPolys();

	// 새 위치 업데이트
	skillInfo->SetStartPos(newStartPos);
	PolyFlags Team;
	if (caster->Get_Team() == 1)
	{
		Team = PolyFlags::Team1| PolyFlags::Missile_pos;
	}
	else
	{
		Team = PolyFlags::Team2 | PolyFlags::Missile_pos;
	}

	// 미사일 위치 플래그 업데이트
	UpdateMissilePositionFlag(navMesh, navQuery, skillId, newStartPos, radius, Team);
	//CheckAllMissileFlags(navMesh);

	if (CheckMultipleObjectsMissileCollision(navMesh, navQuery, objects, caster, target,collisionObj))
	{
		
		return true;
	}
	// 새 위치에서 폴리곤들 찾기
	float startPos[3], endPos[3];
	skillInfo->GetStartPos(startPos);
	skillInfo->GetEndPos(endPos);

	unsigned short dangerousLevel = skillInfo->GetDangerousLevel();
	float duration = skillInfo->GetDuration();

	std::vector<dtPolyRef> newAffectedPolys;
	FindPolysInSkillRange(navQuery, startPos, endPos, radius, newAffectedPolys);

	int appliedCount = 0;
	for (dtPolyRef polyRef : newAffectedPolys)
	{
		if (ApplyDangerousFlagToPoly(navMesh, polyRef, skillId, dangerousLevel, duration))
		{

			skillInfo->AddCurrentPoly(polyRef);
			appliedCount++;
		}
	}
	return false;

}

void SkillDetected::UpdateMissilePositionFlag(dtNavMesh* navMesh, dtNavMeshQuery* navQuery,
	unsigned int skillId, const float* currentPos, float radius, PolyFlags flag)
{
	if (!navMesh || !navQuery) return;

	// 이전 위치의 Missile_pos 플래그 제거
	auto prevIt = m_missilePositionPolys.find(skillId);
	if (prevIt != m_missilePositionPolys.end()) {
		ApplyMissilePositionFlag(navMesh, prevIt->second, false, PolyFlags::Team1);
		m_missilePositionPolys.erase(prevIt);
	}

	// 현재 위치에서 겹치는 폴리곤 찾기
	std::vector<dtPolyRef> currentPolys;
	FindPolysInCircularRange(navQuery, currentPos, radius, currentPolys);

	// === 디버그 추가 ===


	if (!currentPolys.empty()) {
		dtPolyRef bestPoly = FindClosestPolyToPoint(navMesh, navQuery, currentPos, currentPolys);
		if (bestPoly != 0) {
			if (ApplyMissilePositionFlag(navMesh, bestPoly, true, flag)) {
				m_missilePositionPolys[skillId] = bestPoly;

			
			}
		}
	}

}



void SkillDetected::FindPolysInCircularRange(dtNavMeshQuery* navQuery, const float* center, float radius,
	std::vector<dtPolyRef>& outPolys)
{
	if (!navQuery) {

		return;
	}

	outPolys.clear();

	const int MAX_POLYS = 256;
	dtPolyRef polys[MAX_POLYS];
	int polyCount = 0;



	// 원형 범위로 폴리곤 검색
	float halfExtents[3] = { radius, 10.0f, radius }; // Y를 10.0f로 증가!

	dtQueryFilter filter;
	filter.setIncludeFlags(SAMPLE_POLYFLAGS_ALL);
	filter.setExcludeFlags(SAMPLE_POLYFLAGS_DISABLED);

	dtStatus status = navQuery->queryPolygons(center, halfExtents, &filter, polys, &polyCount, MAX_POLYS);




	if (dtStatusSucceed(status)) {
		outPolys.reserve(polyCount);



		// 실제로 원형 범위 내에 있는지 확인
		for (int i = 0; i < polyCount; ++i) {
			if (IsPolyInCircularRange(navQuery->getAttachedNavMesh(), polys[i], center, radius)) {
				outPolys.push_back(polys[i]);
			}
		}

	}
}



dtPolyRef SkillDetected::FindClosestPolyToPoint(const dtNavMesh* navMesh, dtNavMeshQuery* navQuery,
	const float* point, const std::vector<dtPolyRef>& candidates)
{
	if (candidates.empty()) return 0;

	dtPolyRef closestPoly = 0;
	float closestDist = FLT_MAX;

	for (dtPolyRef polyRef : candidates) {
		const dtMeshTile* tile = nullptr;
		const dtPoly* poly = nullptr;

		if (dtStatusFailed(navMesh->getTileAndPolyByRef(polyRef, &tile, &poly)))
			continue;

		Vec3 polyCenter = CalculatePolygonCenter(poly, tile->verts);

		float dx = polyCenter.x - point[0];
		float dz = polyCenter.z - point[2];
		float distance = dx * dx + dz * dz; // 제곱근 계산 생략 (비교만 하므로)

		if (distance < closestDist) {
			closestDist = distance;
			closestPoly = polyRef;
		}
	}

	return closestPoly;
}

bool SkillDetected::ApplyMissilePositionFlag(dtNavMesh* navMesh, dtPolyRef polyRef, bool apply , PolyFlags flag)
{
	if (!navMesh || !navMesh->isValidPolyRef(polyRef))
		return false;

	unsigned short currentFlags = 0;
	unsigned short removeFlags = 0;
	if (!dtStatusSucceed(navMesh->getPolyFlags(polyRef, &currentFlags))) {
		return false;
	}

	unsigned short newFlags;
	if (apply) {
		// Missile_pos 플래그 추가
		newFlags = currentFlags | flag;
	}
	else {
		// Missile_pos 플래그 제거
		if (currentFlags & flag)
		{
			removeFlags = PolyFlags::Missile_pos | flag;
			newFlags = currentFlags & ~removeFlags;

		}
		else
		{
			removeFlags = PolyFlags::Missile_pos | PolyFlags::Team2;
			newFlags = currentFlags & ~removeFlags;
		}
		
	}

	return dtStatusSucceed(navMesh->setPolyFlags(polyRef, newFlags));
}

bool SkillDetected::CheckObjectCollision(const float* startPos, const float* endPos, float skillradius,
	const float* objectPos, float objectRadius)
{
	return IsCircleIntersectCapsule(objectPos, objectRadius, startPos, endPos, skillradius);
}

std::vector<int> SkillDetected::CheckMultipleObjectsCollision(const float* startPos, const float* endPos, float skillradius,
	const std::vector<std::pair<float[3], float>>& objects)
{
	std::vector<int> hitObjects;

	for (size_t i = 0; i < objects.size(); ++i)
	{
		if (IsCircleIntersectCapsule(objects[i].first, objects[i].second, startPos, endPos, skillradius))
		{
			hitObjects.push_back(static_cast<int>(i));
		}
	}

	return hitObjects;
}

void SkillDetected::RemoveSkillEffect(dtNavMesh* navMesh, unsigned int skillId)
{
	if (!navMesh) return;

	// *** 1. 미사일 위치 플래그 강제 제거 - 원본으로 복원 ***
	auto missileIt = m_missilePositionPolys.find(skillId);
	if (missileIt != m_missilePositionPolys.end()) {
		dtPolyRef missilePolyRef = missileIt->second;

		// 해당 폴리곤의 원본 플래그 찾기
		auto modifiedIt = m_modifiedPolys.find(missilePolyRef);
		if (modifiedIt != m_modifiedPolys.end()) {
			// 원본 플래그로 복원
			unsigned short originalFlags = modifiedIt->second->GetOriginalFlags();
			navMesh->setPolyFlags(missilePolyRef, originalFlags);
		}
		else {
			// m_modifiedPolys에 없다면 그냥 Missile_pos 플래그만 제거
			unsigned short currentFlags = 0;
			if (dtStatusSucceed(navMesh->getPolyFlags(missilePolyRef, &currentFlags))) {
				unsigned short newFlags = currentFlags & ~PolyFlags::Missile_pos;
				navMesh->setPolyFlags(missilePolyRef, newFlags);
			}
		}

		// 미사일 위치 추적에서 제거
		m_missilePositionPolys.erase(missileIt);
	}

	// *** 2. 모든 관련 폴리곤을 원본 플래그로 강제 복원 ***
	std::vector<dtPolyRef> polysToRestore;

	for (auto& pair : m_modifiedPolys) {
		auto& polyInfo = pair.second;
		if (polyInfo->RemoveSkill(skillId)) {
			dtPolyRef polyRef = pair.first;

			// 해당 폴리곤에 다른 활성 스킬이 있는지 확인
			if (!polyInfo->HasActiveSkills()) {
				// 다른 스킬이 없으면 완전히 원본으로 복원
				unsigned short originalFlags = polyInfo->GetOriginalFlags();
				navMesh->setPolyFlags(polyRef, originalFlags);
				polysToRestore.push_back(polyRef);
			}
			else {
				// 다른 스킬이 있으면 해당 스킬들만 적용
				unsigned short originalFlags = polyInfo->GetOriginalFlags();
				unsigned short combinedDanger = polyInfo->GetCombinedDangerLevel();
				unsigned short newFlags = originalFlags | combinedDanger;
				navMesh->setPolyFlags(polyRef, newFlags);
			}
		}
	}

	// 완전히 복원된 폴리곤들을 m_modifiedPolys에서 제거
	for (dtPolyRef polyRef : polysToRestore) {
		std::unique_lock lock(m_modifiedPolysmutex);
		{
			m_modifiedPolys.erase(polyRef);
		}
	}

	// *** 3. 이동 스킬 목록에서 제거 ***
	auto movingIt = m_movingSkills.find(skillId);
	if (movingIt != m_movingSkills.end()) {
		m_movingSkills.erase(movingIt);
	}
}

void SkillDetected::RemoveAllSkillEffects(dtNavMesh* navMesh)
{
	// 모든 미사일 위치 플래그 제거
	for (auto& pair : m_missilePositionPolys) {
		ApplyMissilePositionFlag(navMesh, pair.second, false,PolyFlags::Team1);
	}
	m_missilePositionPolys.clear();

	// 모든 데이터 초기화
	m_modifiedPolys.clear();
	m_movingSkills.clear();
}

void SkillDetected::Update(dtNavMesh* navMesh, dtNavMeshQuery* navQuery, float deltaTime)
{
	// 1. 입력 검증
	if (!navMesh || !navQuery)
		return;

	// 2. 빈 컨테이너 확인
	if (m_modifiedPolys.empty())
		return;

	std::vector<dtPolyRef> polysToUpdate;
	std::vector<dtPolyRef> emptyPolys; // 스킬이 모두 제거된 폴리곤들

	// 3. 각 폴리곤의 스킬들 처리
	for (auto& pair : m_modifiedPolys)
	{
		dtPolyRef polyRef = pair.first;
		auto& polyInfo = pair.second;

		// 활성 스킬이 없으면 스킵
		if (!polyInfo->HasActiveSkills())
		{
			emptyPolys.push_back(polyRef);
			continue;
		}

		// 시간 업데이트 및 만료된 스킬 제거
		if (polyInfo->UpdateSkillTimes(deltaTime))
		{
			if (!polyInfo->HasActiveSkills())
			{
				emptyPolys.push_back(polyRef);
			}
			else
			{
				polysToUpdate.push_back(polyRef);
			}
		}
	}

	// 4. 업데이트 처리
	for (dtPolyRef polyRef : polysToUpdate)
	{
		UpdatePolyFlags(navMesh, polyRef);
	}

	// 5. 빈 폴리곤들 정리
	for (dtPolyRef polyRef : emptyPolys)
	{
		RestoreOriginalFlag(navMesh, polyRef);
	}
}

void SkillDetected::FindPolysInSkillRange(dtNavMeshQuery* navQuery, const float* startPos, const float* endPos, float radius,
	std::vector<dtPolyRef>& outPolys)
{
	if (!navQuery) return;
	outPolys.clear();

	// 사각형의 4개 꼭짓점 계산
	Vec3 rectCorners[4];
	CalculateRectangleCorners(startPos, endPos, radius, rectCorners);

	// 사각형을 포함하는 바운딩 박스 계산
	float bounds[6]; // minX, minY, minZ, maxX, maxY, maxZ
	CalculateRectangleBounds(rectCorners, bounds);

	// NavMesh 쿼리로 후보 폴리곤들 수집
	std::vector<dtPolyRef> candidatePolys;
	QueryCandidatePolygons(navQuery, bounds, candidatePolys);

	// 각 후보 폴리곤이 사각형과 겹치는지 체크
	int validCount = 0;
	for (dtPolyRef polyRef : candidatePolys)
	{
		if (IsPolygonInRectangle(navQuery->getAttachedNavMesh(), polyRef, rectCorners))
		{
			outPolys.push_back(polyRef);
			validCount++;
		}
	}
}

void SkillDetected::CalculateRectangleCorners(const float* startPos, const float* endPos, float radius, Vec3* corners)
{
	Vec3 start(startPos[0], 0.0f, startPos[2]);
	Vec3 end(endPos[0], 0.0f, endPos[2]);

	// 미사일 방향 벡터
	Vec3 forward = end - start;
	float forwardLength = forward.length();
	if (forwardLength < 0.001f) {
		// 기본값으로 설정
		for (int i = 0; i < 4; i++) {
			corners[i] = start;
		}
		return;
	}
	forward = forward.normalized();

	// XZ 평면에서 수직 벡터 계산 (Y=0 평면에서)
	Vec3 right(-forward.z, 0.0f, forward.x);
	right = right.normalized();

	// 4개 꼭짓점 계산
	corners[0] = start + right * radius;   // 시작점 오른쪽
	corners[1] = start - right * radius;   // 시작점 왼쪽  
	corners[2] = end - right * radius;     // 끝점 왼쪽
	corners[3] = end + right * radius;     // 끝점 오른쪽
}

void SkillDetected::CalculateRectangleBounds(const Vec3* corners, float* bounds)
{
	bounds[0] = bounds[3] = corners[0].x; // minX, maxX
	bounds[1] = bounds[4] = 0.0f;         // minY, maxY (항상 0.0 고정)
	bounds[2] = bounds[5] = corners[0].z; // minZ, maxZ

	// 4개 꼭짓점을 확인해서 X, Z의 min/max 찾기
	for (int i = 1; i < 4; i++) {
		bounds[0] = (std::min)(bounds[0], corners[i].x); // minX
		bounds[2] = (std::min)(bounds[2], corners[i].z); // minZ
		bounds[3] = (std::max)(bounds[3], corners[i].x); // maxX
		bounds[5] = (std::max)(bounds[5], corners[i].z); // maxZ
	}

	// Y 범위는 적은 값으로 고정
	bounds[1] = -1.0f;   // minY
	bounds[4] = 1.0f;    // maxY
}

void SkillDetected::QueryCandidatePolygons(dtNavMeshQuery* navQuery, const float* bounds, std::vector<dtPolyRef>& candidates)
{
	candidates.clear();

	// 바운딩 박스 중심과 크기 계산
	float center[3] = {
		(bounds[0] + bounds[3]) * 0.5f,
		(bounds[1] + bounds[4]) * 0.5f,
		(bounds[2] + bounds[5]) * 0.5f
	};

	float halfExtents[3] = {
		(bounds[3] - bounds[0]) * 0.5f,
		(bounds[4] - bounds[1]) * 0.5f,
		(bounds[5] - bounds[2]) * 0.5f
	};

	// NavMesh 쿼리 실행
	static const int MAX_POLYS = 1024;
	dtPolyRef polys[MAX_POLYS];
	int polyCount = 0;

	dtQueryFilter filter;
	filter.setIncludeFlags(SAMPLE_POLYFLAGS_ALL);
	filter.setExcludeFlags(SAMPLE_POLYFLAGS_DISABLED);

	dtStatus status = navQuery->queryPolygons(center, halfExtents, &filter, polys, &polyCount, MAX_POLYS);

	if (dtStatusSucceed(status)) {
		candidates.reserve(polyCount);
		for (int i = 0; i < polyCount; ++i) {
			candidates.push_back(polys[i]);
		}
	}
}

bool SkillDetected::IsPolygonInRectangle(const dtNavMesh* navMesh, dtPolyRef polyRef, const Vec3* rectCorners)
{
	const dtMeshTile* tile = nullptr;
	const dtPoly* poly = nullptr;

	if (dtStatusFailed(navMesh->getTileAndPolyByRef(polyRef, &tile, &poly)))
		return false;

	const float* verts = tile->verts;

	// 방법 1: 폴리곤의 정점 중 하나라도 사각형 안에 있으면 OK
	for (int i = 0; i < poly->vertCount; ++i) {
		const float* vertex = &verts[poly->verts[i] * 3];
		Vec3 point(vertex[0], vertex[1], vertex[2]);

		if (IsPointInRectangle(point, rectCorners)) {
			return true;
		}
	}

	// 방법 2: 폴리곤 중심점이 사각형 안에 있으면 OK
	Vec3 polyCenter = CalculatePolygonCenter(poly, verts);
	if (IsPointInRectangle(polyCenter, rectCorners)) {
		return true;
	}

	return false;
}

bool SkillDetected::IsPointInRectangle(const Vec3& point, const Vec3* rectCorners)
{
	// 2D 계산만 수행 (Y값 무시, X-Z 평면에서만)
	Vec2 point2D(point.x, point.z);

	for (int i = 0; i < 4; i++) {
		int nextIndex = (i + 1) % 4;

		Vec2 corner1(rectCorners[i].x, rectCorners[i].z);
		Vec2 corner2(rectCorners[nextIndex].x, rectCorners[nextIndex].z);

		Vec2 edge = corner2 - corner1;
		Vec2 toPoint = point2D - corner1;

		// 2D Cross Product
		float cross = edge.cross_product(toPoint);

		// 모든 모서리에 대해 같은 방향이어야 내부에 있음
		if (i == 0) {
			// 첫 번째 모서리의 방향을 기준으로 설정
			if (cross < 0) {
				return false;
			}
		}
		else {
			if (cross < 0) {
				return false;
			}
		}
	}

	return true;
}



bool SkillDetected::IsCircleIntersectCapsule(const float* circleCenter, float circleRadius,
	const float* capsuleStart, const float* capsuleEnd, float capsuleradius)
{
	float capsuleRadius = capsuleradius;

	// 캡슐의 중심선(선분)과 원의 중심 사이의 최단거리 계산
	float lineVec[3] = {
		capsuleEnd[0] - capsuleStart[0],
		capsuleEnd[1] - capsuleStart[1],
		capsuleEnd[2] - capsuleStart[2]
	};

	float pointVec[3] = {
		circleCenter[0] - capsuleStart[0],
		circleCenter[1] - capsuleStart[1],
		circleCenter[2] - capsuleStart[2]
	};

	float lineLength = sqrt(lineVec[0] * lineVec[0] + lineVec[1] * lineVec[1] + lineVec[2] * lineVec[2]);

	// 캡슐이 점인 경우 (start == end)
	if (lineLength < 0.001f) {
		float distToStart = sqrt(
			(circleCenter[0] - capsuleStart[0]) * (circleCenter[0] - capsuleStart[0]) +
			(circleCenter[1] - capsuleStart[1]) * (circleCenter[1] - capsuleStart[1]) +
			(circleCenter[2] - capsuleStart[2]) * (circleCenter[2] - capsuleStart[2])
		);
		return distToStart <= (circleRadius + capsuleRadius);
	}

	// 선분 위의 가장 가까운 점 찾기
	float t = (pointVec[0] * lineVec[0] + pointVec[1] * lineVec[1] + pointVec[2] * lineVec[2]) / (lineLength * lineLength);
	t = (std::max)(0.0f, (std::min)(1.0f, t)); // 선분 범위로 제한

	float closestPoint[3] = {
		capsuleStart[0] + t * lineVec[0],
		capsuleStart[1] + t * lineVec[1],
		capsuleStart[2] + t * lineVec[2]
	};

	// 원의 중심과 가장 가까운 점 사이의 거리
	float distToClosest = sqrt(
		(circleCenter[0] - closestPoint[0]) * (circleCenter[0] - closestPoint[0]) +
		(circleCenter[1] - closestPoint[1]) * (circleCenter[1] - closestPoint[1]) +
		(circleCenter[2] - closestPoint[2]) * (circleCenter[2] - closestPoint[2])
	);

	// 원과 캡슐이 충돌하는지 체크 (두 반지름의 합보다 거리가 작거나 같으면 충돌)
	return distToClosest <= (circleRadius + capsuleRadius);
}

void SkillDetected::RemoveSkillFromPoly(dtNavMesh* navMesh, dtPolyRef polyRef, unsigned int skillId)
{
	auto it = m_modifiedPolys.find(polyRef);
	if (it == m_modifiedPolys.end() || !it->second) return;

	if (it->second->RemoveSkill(skillId)) {
		UpdatePolyFlags(navMesh, polyRef);
	}
}

bool SkillDetected::ApplyDangerousFlagToPoly(dtNavMesh* navMesh, dtPolyRef polyRef,
	unsigned int skillId, unsigned short dangerousLevel, float duration)
{
	if (!navMesh || !navMesh->isValidPolyRef(polyRef))
		return false;

	// find() 한 번만 호출하고 iterator 재사용

	auto it = m_modifiedPolys.find(polyRef);

	// 처음 수정하는 폴리곤인 경우
	if (it == m_modifiedPolys.end()) {
		auto info = std::make_unique<PolyFlagInfo>();
		info->SetPolyRef(polyRef);

		unsigned short currentFlags = 0;
		if (!dtStatusSucceed(navMesh->getPolyFlags(polyRef, &currentFlags))) {
			return false;
		}

		if (currentFlags & PolyFlags::SAMPLE_POLYFLAGS_DISABLED) {
			return false;
		}

		info->SetOriginalFlags(currentFlags);

		// insert 후 iterator 받기
		auto insertResult = m_modifiedPolys.insert({ polyRef, std::move(info) });
		it = insertResult.first;  // 새로 삽입된 요소의 iterator
	}

	// 안전성 확인
	if (!MemoryCheck::is_valid<PolyFlagInfo>(it->second.get())) {
		return false;
	}

	// 스킬 효과 추가
	SkillEffect effect{ skillId, duration, dangerousLevel };
	it->second->AddOrUpdateSkill(effect);

	UpdatePolyFlags(navMesh, polyRef);
	return true;
}

void SkillDetected::UpdatePolyFlags(dtNavMesh* navMesh, dtPolyRef polyRef)
{
	auto it = m_modifiedPolys.find(polyRef);
	if (it == m_modifiedPolys.end() || !it->second) return;

	auto& polyInfo = it->second;

	// 활성 스킬이 없으면 원본으로 복원
	if (!polyInfo->HasActiveSkills()) {
		navMesh->setPolyFlags(polyRef, polyInfo->GetOriginalFlags());
		std::unique_lock lock(m_modifiedPolysmutex);
		{
			m_modifiedPolys.erase(it);
		}
		return;
	}

	// 모든 활성 스킬의 위험도를 OR 연산으로 합치기
	unsigned short combinedDanger = polyInfo->GetCombinedDangerLevel();

	// 새로운 플래그 적용 (원본 + 합쳐진 모든 위험도)
	unsigned short newFlags = polyInfo->GetOriginalFlags();
	newFlags &= 0;  // 기존 위험 플래그 제거
	newFlags |= combinedDanger;   // 합쳐진 모든 위험도 추가

	navMesh->setPolyFlags(polyRef, newFlags);
}

void SkillDetected::RestoreOriginalFlag(dtNavMesh* navMesh, dtPolyRef polyRef)
{
	auto it = m_modifiedPolys.find(polyRef);
	if (it != m_modifiedPolys.end())
	{
		navMesh->setPolyFlags(polyRef, it->second->GetOriginalFlags());
		std::unique_lock lock(m_modifiedPolysmutex);
		{
			m_modifiedPolys.erase(it);
		}
	}
}
bool SkillDetected::IsPolyInCircularRange(const dtNavMesh* navMesh, dtPolyRef polyRef,
	const float* center, float radius)
{
	const dtMeshTile* tile = nullptr;
	const dtPoly* poly = nullptr;

	if (dtStatusFailed(navMesh->getTileAndPolyByRef(polyRef, &tile, &poly)))
		return false;

	const float* verts = tile->verts;

	// 폴리곤 중심점 계산
	Vec3 polyCenter = CalculatePolygonCenter(poly, verts);

	// 중심점과의 거리 계산 (2D, Y축 무시)
	float dx = polyCenter.x - center[0];
	float dz = polyCenter.z - center[2];
	float distance = sqrt(dx * dx + dz * dz);

	return distance <= radius;
}
Vec3 SkillDetected::CalculatePolygonCenter(const dtPoly* poly, const float* verts)
{
	Vec3 center(0.0f, 0.0f, 0.0f);

	for (int i = 0; i < poly->vertCount; ++i) {
		const float* vertex = &verts[poly->verts[i] * 3];
		// Y값은 항상 0.0으로 고정
		center.x += vertex[0];
		center.z += vertex[2];
	
	}

	center.x /= static_cast<float>(poly->vertCount);
	center.z /= static_cast<float>(poly->vertCount);
	// 혹시 모를 Y값 보정
	center.y = 0.0f;

	return center;
}
void SkillDetected::CheckAllMissileFlags(dtNavMesh* navMesh)
{
	
	for (const auto& pair : m_missilePositionPolys)
	{
		unsigned int skillId = pair.first;
		dtPolyRef polyRef = pair.second;

		unsigned short flags = 0;
		if (dtStatusSucceed(navMesh->getPolyFlags(polyRef, &flags)))
		{
			bool hasMissileFlag = (flags & PolyFlags::Missile_pos) != 0;
			OutputDebugStringA(("Skill " + std::to_string(skillId) +
				" -> Poly " + std::to_string(polyRef) +
				" flags: " + std::to_string(flags) +
				" hasMissile: " + (hasMissileFlag ? "YES" : "NO") + "\n").c_str());
		}
	}
	OutputDebugStringA("================================\n");
}