#include "world/terrain.h"
#include "stb_image.h"
#include "world/world_structures.h"

#include <iostream>

Terrain::Terrain(WorldScaleInfo &worldScaleInfo, HeightMapData &heightMapData) : m_worldScaleInfo(worldScaleInfo), m_heightMapData(heightMapData) { }

float Terrain::SampleHeight(float worldX, float worldZ) const
{
    glm::vec2 localCoord = worldToLocal(worldX, worldZ);
    // 좌표 보간 및 높이 샘플링
    return bilinearInterpolate(localCoord.x, localCoord.y) * m_worldScaleInfo.heightScale;
}

glm::vec3 Terrain::SampleNormal(float worldX, float worldZ) const
{
    glm::vec2 localCoord = worldToLocal(worldX, worldZ);
    float localX = localCoord.x;
    float localZ = localCoord.y;

    float step = 0.5f; // texel step
    float ws = m_worldScaleInfo.worldScale;
    float hs = m_worldScaleInfo.heightScale;

    float hC = bilinearInterpolate(localX, localZ) * hs;
    float hL = bilinearInterpolate(localX - step, localZ) * hs;
    float hR = bilinearInterpolate(localX + step, localZ) * hs;
    float hD = bilinearInterpolate(localX, localZ - step) * hs;
    float hU = bilinearInterpolate(localX, localZ + step) * hs;

    glm::vec3 center(worldX,           hC, worldZ);
    glm::vec3 left  (worldX - ws,      hL, worldZ);
    glm::vec3 right (worldX + ws,      hR, worldZ);
    glm::vec3 down  (worldX,           hD, worldZ - ws);
    glm::vec3 up    (worldX,           hU, worldZ + ws);

    glm::vec3 tangentX = right - left;
    glm::vec3 tangentZ = up - down;

    glm::vec3 normal = glm::normalize(glm::cross(tangentZ, tangentX));

    return normal;
}

float Terrain::bilinearInterpolate(float localX, float localZ) const
{
    int x0 = (int)floor(localX);
    int z0 = (int)floor(localZ);
    int x1 = x0 + 1;
    int z1 = z0 + 1;

    float h00 = getHeightFromHeightData(x0, z0);
    float h10 = getHeightFromHeightData(x1, z0);
    float h01 = getHeightFromHeightData(x0, z1);
    float h11 = getHeightFromHeightData(x1, z1);

    float fx = localX - x0;
    float fz = localZ - z0;

    float h0 = glm::mix(h00, h10, fx);
    float h1 = glm::mix(h01, h11, fx);

    return glm::mix(h0, h1, fz);
}

float Terrain::getHeightFromHeightData(int localX, int localZ) const
{
    int tx = glm::clamp(localX, 0, m_heightMapData.heightMapWidth  - 1);
    int tz = glm::clamp(localZ, 0, m_heightMapData.heightMapHeight - 1);

    int index = tz * m_heightMapData.heightMapWidth + tx;
    float height = m_heightMapData.heights[index] / 65535.0f;

    return height;
}

glm::vec2 Terrain::worldToLocal(float worldX, float worldZ) const
{
    // height map 좌표계로 변환
    float tx = worldX / m_worldScaleInfo.worldScale;
    float tz = worldZ / m_worldScaleInfo.worldScale;

    // 텍셀 중심이 되도록 보정
    tx -= 0.5f;
    tz -= 0.5f;

    return {tx, tz};
}