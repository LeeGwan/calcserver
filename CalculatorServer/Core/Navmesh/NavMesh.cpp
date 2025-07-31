#include "NavMesh.h"
#include "../Algorithm/NavigationService.h"
#include "Flags/Flags.h"
#include "recastnavigation/Include/DetourNavMeshQuery.h"
#include "recastnavigation/Include/DetourCommon.h"
#include "recastnavigation/Include/DetourNavMeshBuilder.h"
#include "../utils/Vector.h"
#include <fstream>
#include <filesystem>
NavMesh::NavMesh()
{
    LeagueNavMesh = dtAllocNavMesh();
    if (!LeagueNavMesh)
    {
        std::cerr << "Failed to allocate NavMesh" << std::endl;
        return;
    }

    // NavMesh 바이너리 파일 경로 설정
    std::filesystem::path currentPath = std::filesystem::current_path();
    std::string binPath = currentPath.string() + "\\Core\\Navmesh\\recastnavigation\\Bin\\LeagueNavmesh.bin";

    // NavMesh 로딩
    if (LoadNavMeshFromBin(binPath.c_str(), LeagueNavMesh))
    {
        Init_SetFlags(LeagueNavMesh);  // 폴리곤 플래그 설정

        // NavMeshQuery 초기화
        navQuery = dtAllocNavMeshQuery();
        if (!navQuery || dtStatusFailed(navQuery->init(LeagueNavMesh, 2048)))
        {
            std::cerr << "Failed to initialize NavMeshQuery" << std::endl;
            Release();
        }
    }
    else
    {
        std::cerr << "Failed to load NavMesh from: " << binPath << std::endl;
        Release();
    }
    m_NavService = std::make_unique<NavigationService>();
}

NavMesh::~NavMesh()
{
   Release(); 
   
}

bool NavMesh::LoadNavMeshFromBin(const char* filePath, dtNavMesh* navMesh)
{
    std::ifstream file(filePath, std::ios::binary);
    if (!file)
    {
        std::cerr << "Failed to open navmesh file: " << filePath << std::endl;
        return false;
    }

    // NavMesh 헤더 읽기
    NavMeshSetHeader header;
    file.read(reinterpret_cast<char*>(&header), sizeof(header));

    if (file.fail())
    {
        std::cerr << "Failed to read NavMesh header" << std::endl;
        return false;
    }

    // NavMesh 초기화
    if (dtStatusFailed(navMesh->init(&header.params)))
    {
        std::cerr << "Failed to initialize navmesh with header params" << std::endl;
        return false;
    }

    // 타일 데이터 로딩
    for (int i = 0; i < header.numTiles; ++i)
    {
        NavMeshTileHeader tileHeader;
        file.read(reinterpret_cast<char*>(&tileHeader), sizeof(tileHeader));

        if (file.fail())
        {
            std::cerr << "Failed to read tile header " << i << std::endl;
            return false;
        }

        // 빈 타일 스킵
        if (!tileHeader.dataSize) continue;

        // 타일 데이터 할당 및 읽기
        unsigned char* data = static_cast<unsigned char*>(dtAlloc(tileHeader.dataSize, DT_ALLOC_PERM));
        if (!data)
        {
            std::cerr << "Failed to allocate memory for tile " << i << std::endl;
            return false;
        }

        file.read(reinterpret_cast<char*>(data), tileHeader.dataSize);
        if (file.fail())
        {
            std::cerr << "Failed to read tile data " << i << std::endl;
            dtFree(data);
            return false;
        }

        // 타일 추가
        if (dtStatusFailed(navMesh->addTile(data, tileHeader.dataSize, DT_TILE_FREE_DATA, tileHeader.tileRef, nullptr)))
        {
            std::cerr << "Failed to add tile " << i << std::endl;
            dtFree(data);
            return false;
        }
    }


    return true;
}

void NavMesh::Init_SetFlags(dtNavMesh* navMesh)
{
    if (!navMesh) return;

    int maxTiles = navMesh->getMaxTiles();
    int walkablePolys = 0;
    int structurepoly = 0;
    int disabledPolys = 0;

    for (int i = 0; i < maxTiles; ++i)
    {
        dtMeshTile* tile = navMesh->getTile(i);
        if (!tile || !tile->header) continue;

        int polyCount = tile->header->polyCount;
        for (int j = 0; j < polyCount; ++j)
        {
        
            dtPoly& poly = tile->polys[j];
            PolyAreas areaType = static_cast<PolyAreas>(poly.getArea());
            poly.flags &= 0;
            switch (areaType)
            {
            case PolyAreas::SAMPLE_POLYAREA_GROUND:
                poly.flags |= PolyFlags::WALK;
                walkablePolys++;
                break;
            case PolyAreas::SAMPLE_POLYAREA_DOOR:
                poly.flags |= PolyFlags::Structure;
                structurepoly++;
                break;
            default:
                poly.flags |= PolyFlags::SAMPLE_POLYFLAGS_DISABLED;
                disabledPolys++;
                break;
            }
        }
    }

  //  std::cout << "걸어 갈수있는 타일 갯수 :" << walkablePolys << "건물 타일 갯수 :" << structurepoly << "이동불가 타일 갯수 :" << disabledPolys << std::endl;
}

void NavMesh::Release()
{
    std::unique_lock<std::shared_mutex> lock(NavMeshMutex);

    if (navQuery)
    {
        dtFreeNavMeshQuery(navQuery);
        navQuery = nullptr;
    }

    if (LeagueNavMesh)
    {
        dtFreeNavMesh(LeagueNavMesh);
        LeagueNavMesh = nullptr;
    }
}

dtNavMesh* NavMesh::Get_LeagueNavMesh()
{
    std::unique_lock<std::shared_mutex> lock(NavMeshMutex);
    return LeagueNavMesh;
}

dtNavMeshQuery* NavMesh::Get_navQuery()
{
    std::unique_lock<std::shared_mutex> lock(NavMeshMutex);
    return navQuery;
}

NavigationService* NavMesh::Get_NavigationService() const
{
    return m_NavService.get();
}