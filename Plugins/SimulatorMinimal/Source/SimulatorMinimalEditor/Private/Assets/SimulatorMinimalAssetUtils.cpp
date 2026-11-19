#include "Assets/SimulatorMinimalAssetUtils.h"

#include "Assets/AssetMeshBoundData.h"
#include "MeshDescription.h"

//-------------------------------------------------------------------------------
void USimulatorMinimalAssetUtils::AddUVChannelTo(UStaticMesh* mesh, int uvChannel)
{
	for (int i = 0; i < mesh->GetNumLODs(); ++i)
	{
		for (auto channel = mesh->GetNumUVChannels(i); channel < uvChannel + 1; ++channel)
		{
			mesh->AddUVChannel(i);
		}
	}
}
//-------------------------------------------------------------------------------
void USimulatorMinimalAssetUtils::GenerateUVsByVertexLocation(UStaticMesh* mesh, int uvChannel)
{
	FVector2D min{ 0, 0 };
	FVector2D max{ 0, 0 };

	auto mainDesc = mesh->GetMeshDescription(0);
	for (auto vertexInstanceId : mainDesc->VertexInstances().GetElementIDs())
	{
		auto pos = mainDesc->GetVertexPosition(mainDesc->GetVertexInstanceVertex(vertexInstanceId));

		min.X = pos.X < min.X ? pos.X : min.X;
		min.Y = pos.Y < min.Y ? pos.Y : min.Y;

		max.X = pos.X > max.X ? pos.X : max.X;
		max.Y = pos.Y > max.Y ? pos.Y : max.Y;
	}

	float width = max.X - min.X;
	float height = max.Y - min.Y;

	TArray<const FMeshDescription*> descriptions;
	for (int i = 0; i < mesh->GetNumLODs(); ++i)
	{
		auto desc = mesh->GetMeshDescription(i);
		descriptions.Add(desc);

		TMap<FVertexInstanceID, FVector2D> texCoords;
		for (auto vertexInstanceId : desc->VertexInstances().GetElementIDs())
		{
			auto pos = desc->GetVertexPosition(desc->GetVertexInstanceVertex(vertexInstanceId));
			texCoords.Add(vertexInstanceId, FVector2D((pos.X - min.X) / width, (pos.Y - min.Y) / height));
		}

		mesh->SetUVChannel(i, uvChannel, texCoords);
	}

	UAssetMeshBoundData* boundData = Cast<UAssetMeshBoundData>(mesh->GetAssetUserDataOfClass(UAssetMeshBoundData::StaticClass()));
	if (boundData == nullptr)
	{
		boundData = NewObject<UAssetMeshBoundData>(mesh);
		mesh->AddAssetUserData(boundData);
	}

	boundData->MinBound = min;
	boundData->MaxBound = max;

	mesh->bAllowCPUAccess = true;

	UStaticMesh::FBuildMeshDescriptionsParams params;
	mesh->BuildFromMeshDescriptions(descriptions, params);
}