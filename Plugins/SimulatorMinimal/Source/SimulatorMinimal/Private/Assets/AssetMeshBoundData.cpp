#include "Assets/AssetMeshBoundData.h"
#include "Engine/StaticMesh.h"

UAssetMeshBoundData::UAssetMeshBoundData(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) { }
//-------------------------------------------------------------------------------
UAssetMeshBoundData* UAssetMeshBoundData::GetAssetMeshBoundUserData(UStaticMesh* staticMesh)
{
	if (auto userData = staticMesh->GetAssetUserDataOfClass(UAssetMeshBoundData::StaticClass()))
	{
		return Cast<UAssetMeshBoundData>(userData);
	}

	return nullptr;
}
//-------------------------------------------------------------------------------