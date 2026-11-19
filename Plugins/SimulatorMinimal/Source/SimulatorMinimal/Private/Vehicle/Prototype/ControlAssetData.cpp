#include "Vehicle/Prototype/ControlAssetData.h"
#include "Components/SceneComponent.h"
#include "GameFramework/Actor.h"
#include "Interfaces/Interface_AssetUserData.h"

bool UControlBlock::GetProperty(FName Name, UPARAM(meta = (AllowAbstract = false)) TSubclassOf<UControlPropertyBase> propertyClass, UControlPropertyBase*& outProperty) const
{
	auto property = Properties.Find(Name);
	outProperty = property ? *property : nullptr;
	return property != nullptr;
}
//-------------------------------------------------------------------------------
UControlBlock* UControlAssetData::GetControl(FName name) const
{
	auto control = ControlBlocks.Find(name);
	return control ? *control : nullptr;
}
//-------------------------------------------------------------------------------
UControlAssetData* UControlAssetDataStatics::GetControlAssetData(UObject* object)
{
	if (AActor* Actor = Cast<AActor>(object))
	{
		object = Actor->GetRootComponent();
	}

	if (IInterface_AssetUserData* assetUserData = Cast<IInterface_AssetUserData>(object))
	{
		return Cast<UControlAssetData>(assetUserData->GetAssetUserDataOfClass(UControlAssetData::StaticClass()));
	}

	return nullptr;
}
//-------------------------------------------------------------------------------
UControlBlock* UControlAssetDataStatics::GetControl(UObject* object, FName name)
{
	if (auto controlAssetData = UControlAssetDataStatics::GetControlAssetData(object))
	{
		return controlAssetData->GetControl(name);
	}

	return nullptr;
}
//-------------------------------------------------------------------------------