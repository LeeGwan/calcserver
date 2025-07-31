#pragma once
#include <mutex>
#include <shared_mutex>
#include <iostream>
#include <memory>

#include "recastnavigation/Include/DetourNavMesh.h"

class Vec3;
class NavigationService;
class dtNavMeshQuery;
// NavMesh 파일 로딩을 위한 헤더 구조체
struct NavMeshSetHeader
{
    int magic;
    int version;
    int numTiles;
    dtNavMeshParams params;
};

struct NavMeshTileHeader
{
    dtTileRef tileRef;
    int dataSize;
};

class NavMesh final
{
public:
    NavMesh();
    ~NavMesh();

    // 복사 생성자와 대입 연산자 삭제 (RAII 패턴)
    NavMesh(const NavMesh&) = delete;
    NavMesh& operator=(const NavMesh&) = delete;

    void Release();
    dtNavMesh* Get_LeagueNavMesh();
    dtNavMeshQuery* Get_navQuery();
    NavigationService* Get_NavigationService() const;

private:
    dtNavMesh* LeagueNavMesh = nullptr;
    dtNavMeshQuery* navQuery = nullptr;
    std::unique_ptr<NavigationService> m_NavService;

    mutable std::shared_mutex NavMeshMutex;

    // 내부 함수들
    bool LoadNavMeshFromBin(const char* filePath, dtNavMesh* navMesh);
    void Init_SetFlags(dtNavMesh* navMesh);
};