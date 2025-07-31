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

    // NavMesh ���̳ʸ� ���� ��� ����
    std::filesystem::path currentPath = std::filesystem::current_path();
    std::string binPath = currentPath.string() + "\\Core\\Navmesh\\recastnavigation\\Bin\\LeagueNavmesh.bin";

    // NavMesh �ε�
    if (LoadNavMeshFromBin(binPath.c_str(), LeagueNavMesh))
    {
        Init_SetFlags(LeagueNavMesh);  // ������ �÷��� ����

        // NavMeshQuery �ʱ�ȭ
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

    // NavMesh ��� �б�
    NavMeshSetHeader header;
    file.read(reinterpret_cast<char*>(&header), sizeof(header));

    if (file.fail())
    {
        std::cerr << "Failed to read NavMesh header" << std::endl;
        return false;
    }

    // NavMesh �ʱ�ȭ
    if (dtStatusFailed(navMesh->init(&header.params)))
    {
        std::cerr << "Failed to initialize navmesh with header params" << std::endl;
        return false;
    }

    // Ÿ�� ������ �ε�
    for (int i = 0; i < header.numTiles; ++i)
    {
        NavMeshTileHeader tileHeader;
        file.read(reinterpret_cast<char*>(&tileHeader), sizeof(tileHeader));

        if (file.fail())
        {
            std::cerr << "Failed to read tile header " << i << std::endl;
            return false;
        }

        // �� Ÿ�� ��ŵ
        if (!tileHeader.dataSize) continue;

        // Ÿ�� ������ �Ҵ� �� �б�
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

        // Ÿ�� �߰�
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

  //  std::cout << "�ɾ� �����ִ� Ÿ�� ���� :" << walkablePolys << "�ǹ� Ÿ�� ���� :" << structurepoly << "�̵��Ұ� Ÿ�� ���� :" << disabledPolys << std::endl;
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