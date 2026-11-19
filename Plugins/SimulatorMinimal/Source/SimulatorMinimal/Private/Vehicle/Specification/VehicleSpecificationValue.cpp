#include "Vehicle/Specification/VehicleSpecificationValue.h"


FVehicleSpecificationEngineType::FVehicleSpecificationEngineType()
{
	DisplayName = NSLOCTEXT("VehicleSpecifications", "GasolineEngine", "Gasoline");
}
//-----------------------------------------------------------------------------------------------------------
TArray<FString> UVehicleSpecificationLibrary::GetVehicleClassOptions()
{
	return {"Economy", "Comfort", "Business", "Sports", "Supercar"};
}
//-----------------------------------------------------------------------------------------------------------
TArray<FString> UVehicleSpecificationLibrary::GetFuelOptions()
{
	return {"Gasoline", "Diesel"};
}
//-----------------------------------------------------------------------------------------------------------
TArray<FString> UVehicleSpecificationLibrary::GetTransmissionOptions()
{
	return {"Auto", "Manual"};
}
//-----------------------------------------------------------------------------------------------------------
FVehicleSpecificationEngineType UVehicleSpecificationLibrary::GetDefaultEngineType()
{
	return FVehicleSpecificationEngineType();
}
//-----------------------------------------------------------------------------------------------------------
FVehicleSpecificationValue UVehicleSpecificationLibrary::GetDefaultTransmissionType()
{
	return { NSLOCTEXT("VehicleSpecifications", "AutomaticTransmission", "Automatic") };
}
//-----------------------------------------------------------------------------------------------------------
TArray<FCustomVehicleSpecification> UVehicleSpecificationLibrary::CollectSingleSpecifications(UVehicleSpecifications* specifications)
{
	TArray<FCustomVehicleSpecification> result;
	for (TFieldIterator<FStructProperty> structPropertyIt(specifications->GetClass()); structPropertyIt; ++structPropertyIt)
	{
		if (structPropertyIt->Struct == FCustomVehicleSpecification::StaticStruct())
		{
			FCustomVehicleSpecification valueContainer;
			structPropertyIt->GetValue_InContainer(specifications, &valueContainer);

			result.Add(valueContainer);

		}
	}
	
	return result;
}
//-----------------------------------------------------------------------------------------------------------
TArray<FCustomVehicleMultiSpecification> UVehicleSpecificationLibrary::CollectMultiSpecifications(UVehicleSpecifications* specifications)
{
	TArray<FCustomVehicleMultiSpecification> result;
	for (TFieldIterator<FStructProperty> structPropertyIt(specifications->GetClass()); structPropertyIt; ++structPropertyIt)
	{
		if (structPropertyIt->Struct == FCustomVehicleMultiSpecification::StaticStruct())
		{
			FCustomVehicleMultiSpecification valueContainer;
			structPropertyIt->GetValue_InContainer(specifications, &valueContainer);

			result.Add(valueContainer);

		}
	}
	
	return result;
}
//-----------------------------------------------------------------------------------------------------------
TArray<FText> UVehicleSpecificationLibrary::CollectOptions(const TArray<FVehicleSpecificationValue>& specifications)
{
	TArray<FText> result;
	for (const auto& spec : specifications)
	{
		if (spec.BoundToOption)
		{
			result.Add(spec.DisplayName);
		}
	}
	
	return result;
}
//-----------------------------------------------------------------------------------------------------------
TArray<FText> UVehicleSpecificationLibrary::CollectEngineOptions(const TArray<FVehicleSpecificationEngineType>& specifications)
{
	TArray<FText> result;
	for (const auto& spec : specifications)
	{
		if (spec.BoundToOption)
		{
			result.Add(spec.DisplayName);
		}
	}
	
	return result;
}
//-----------------------------------------------------------------------------------------------------------
UVehicleSpecifications::UVehicleSpecifications()
{
	EngineTypes = { UVehicleSpecificationLibrary::GetDefaultEngineType() };
	TransmissionTypes = { UVehicleSpecificationLibrary::GetDefaultTransmissionType() };
}
