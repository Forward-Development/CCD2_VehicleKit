// Copyright Epic Games, Inc. All Rights Reserved.

#include "SimulatorMinimalEditor.h"
#include "Assets/ControlAssetDataHighlighter.h"

#define LOCTEXT_NAMESPACE "FSimulatorMinimalEditorModule"

void FSimulatorMinimalEditorModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	FControlAssetDataHighlighter::Initialize();
}

void FSimulatorMinimalEditorModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	FControlAssetDataHighlighter::Shutdown();
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FSimulatorMinimalEditorModule, SimulatorMinimalEditor)