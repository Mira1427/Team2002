#include "Collision.h"

#include <algorithm>

#include "../Graphics/MeshRenderer.h"


// --- ãÈå`ìØémÇÃåç∑îªíË ---
bool Collision::IntersectRectangles(
    const Vector2& centerA, const Vector2& sizeA,
    const Vector2& centerB, const Vector2& sizeB)
{
    Vector2 leftTopA = centerA - sizeA * 0.5f;
    Vector2 rightBottomA = centerA + sizeA * 0.5f;

    Vector2 leftTopB = centerB - sizeB * 0.5f;
    Vector2 rightBottomB = centerB + sizeB * 0.5f;


    if (leftTopA.y     < rightBottomB.y   &&
        rightBottomA.y > leftTopB.y       &&
        leftTopA.x     < rightBottomB.x   &&
        rightBottomA.x > leftTopB.x)
        return true;

    return false;
}


// --- â~ìØémÇÃåç∑îªíË ---
bool Collision::IntersectCircles(const Vector2& centerA, float radiusA, const Vector2& centerB, float radiusB)
{
    float length = Vector2(centerA - centerB).Length();

    if (length > radiusA + radiusB)
        return false;

    return true;
}


// --- óßï˚ëÃìØémÇÃåç∑îªíË ---
bool Collision::IntersectBoxes(const Vector3& positionA, const Vector3& sizeA, const Vector3& positionB, const Vector3& sizeB)
{
    Vector3 rightTopBackA       = positionA + sizeA * 0.5f;
    Vector3 leftBottomFrontA    = positionA - sizeA * 0.5f;

    Vector3 rightTopBackB       = positionB + sizeB * 0.5f;
    Vector3 leftBottomFrontB    = positionB - sizeB * 0.5f;

    if (leftBottomFrontA.x < rightTopBackB.x      &&
        rightTopBackA.x    > leftBottomFrontB.x   &&
        leftBottomFrontA.y < rightTopBackB.y      &&
        rightTopBackA.y    > leftBottomFrontB.y   &&
        leftBottomFrontA.z < rightTopBackB.z      &&
        rightTopBackA.z    > leftBottomFrontB.z)
        return true;

    return false;
}


// --- ãÖìØémÇÃåç∑îªíË ---
bool Collision::IntersectSpheres(
    const Vector3& positionA, float radiusA,
    const Vector3& positionB, float radiusB)
{
    Vector3 vec = positionB - positionA;    // AÇ©ÇÁBÇ÷ÇÃÉxÉNÉgÉãÇéZèo

    // --- ãóó£îªíË ---
    float lengthSq = vec.Length();
    float range = radiusA + radiusB;

    if (range < lengthSq)
        return false;

    return true;
}


// --- â~íåìØémÇÃåç∑îªíË ---
bool Collision::IntersectCapsules(
    const Vector3& positionA, float radiusA, float heightA,
    const Vector3& positionB, float radiusB, float heightB)
{
    // --- çÇÇ≥îªíË ---
    if (positionA.y > positionB.y + heightB)
        return false;

    if (positionA.y + heightA < positionB.y)
        return false;

    // --- ãóó£îªíË ---
    Vector2 vec = positionB.xz() - positionA.xz();
    float length = vec.Length();
    float range = radiusA + radiusB;

    if (range < length)
        return false;

    return true;
}


// --- ãÖÇ∆AABBÇÃè’ìÀîªíË ---
bool Collision::IntersectSphereAndAABB(Vector3 positionA, float radius, Vector3 positionB, Vector3 sizeB)
{
    Vector3 nearPoint;

    nearPoint.x = (std::max)(positionB.x - sizeB.x * 0.5f, (std::min)(positionA.x, positionB.x + sizeB.x * 0.5f));
    nearPoint.y = (std::max)(positionB.y - sizeB.y * 0.5f, (std::min)(positionA.y, positionB.y + sizeB.y * 0.5f));
    nearPoint.z = (std::max)(positionB.z - sizeB.z * 0.5f, (std::min)(positionA.z, positionB.z + sizeB.z * 0.5f));

    const float distance = sqrtf(
        (nearPoint.x - positionA.x) * (nearPoint.x - positionA.x) +
        (nearPoint.y - positionA.y) * (nearPoint.y - positionA.y) +
        (nearPoint.z - positionA.z) * (nearPoint.z - positionA.z)
    );

    return distance < radius;
}



// --- ÉåÉCÇ∆ÉÇÉfÉãÇÃè’ìÀîªíË ---
bool Collision::IntersectRayModel(
    const Vector3& start,
    const Vector3& end,
    MeshRenderer* model, 
    HitResult& result
)
{
    Vector3 wRayVec = end - start;
    result.distance_ = wRayVec.Length();    // ÉåÉCÇÃí∑Ç≥

    bool hit = false;

    for (const MeshRenderer::Mesh& mesh : model->meshes)
    {
        const NodeTree::Node& node = model->sceneView.nodes.at(mesh.nodeIndex);

        // --- ÉçÅ[ÉJÉããÛä‘Ç…ïœä∑ ---
        Matrix WT = mesh.defaultGlobalTransform;
        Matrix InvWT = Matrix::Inverse(WT);

        Vector3 S = start;
        Vector3 E = end;
        S.TransformCoord(start, InvWT);
        E.TransformCoord(end, InvWT);
        
        Vector3 SE = E - S;
        float length = SE.Length();
        Vector3 V = Vector3::Normalize(SE);

        const std::vector<MeshRenderer::Vertex>& vertices = mesh.vertices;
        const std::vector<UINT>& indices = mesh.indices;

        int materialIndex = -1;
        Vector3 hitPosition;
        Vector3 hitNormal;
        for (const MeshRenderer::Mesh::Subset& subset : mesh.subsets)
        {
            for (UINT i = 0; i < subset.indexCount; i += 3)
            {
                UINT index = subset.startIndexLocation + i;
                const MeshRenderer::Vertex& a = vertices.at(indices.at(index));
                const MeshRenderer::Vertex& b = vertices.at(indices.at(index + 1));
                const MeshRenderer::Vertex& c = vertices.at(indices.at(index + 2));

                Vector3 A = a.position;
                Vector3 B = b.position;
                Vector3 C = c.position;

                Vector3 AB = B - A;
                Vector3 BC = C - B;
                Vector3 CA = A - C;

                Vector3 N = AB.Cross(BC);
                N.Normalize();

                float dot = V.Dot(N);
                if (dot >= 0.0f) continue;

                Vector3 SA = A - S;
                float x = SA.Dot(N);
                x /= dot;
                if (x < 0.0f || x > length) continue;

                Vector3 P = S + (V * x);

                Vector3 PA = A - P;
                Vector3 cross1 = PA.Cross(AB);
                //Vector3 cross1 = AB.Cross(PA);
                float dot1 = N.Dot(cross1);
                if (dot1 < 0.0f) continue;

                Vector3 PB = B - P;
                Vector3 cross2 = PB.Cross(BC);
                //Vector3 cross2 = BC.Cross(PB);
                float dot2 = N.Dot(cross2);
                if (dot2 < 0.0f) continue;

                Vector3 PC = C - P;
                Vector3 cross3 = PC.Cross(CA);
                //Vector3 cross3 = CA.Cross(PC);
                float dot3 = N.Dot(cross3);
                if (dot3 < 0.0f) continue;

                length = x;

                hitPosition = P;
                hitNormal = N;
                materialIndex = static_cast<int>(subset.materialUniqueID);
            }
        }


        if (materialIndex >= 0)
        {
            Vector3 wPos;
            wPos.TransformCoord(hitPosition, WT);
            Vector3 wCrossVec = wPos - start;
            float distance = wCrossVec.Length();

            if (result.distance_ > distance)
            {
                Vector3 wNormal;
               wNormal.TransformNormal(hitNormal, WT);

                result.distance_ = distance;
                result.materialIndex_ = materialIndex;
                result.position_ = wPos;
                result.normal_ = Vector3::Normalize(wNormal);
                
                hit = true;
            }
        }
    }

    return hit;
}
