//------------------------------------------------------------------------------
// ControlAssetDataHighlighter.h
// Automatically highlights actor components in the Blueprint Editor Outliner(Components tree)
// with a green color if they have UControlAssetData attached.
//
// Copyright (c) 2026 Forward Development
// All rights reserved.
//
// Date:       29.10.26
// Author:     Кудрявцев Е.Б.
//------------------------------------------------------------------------------
#pragma once

#include "CoreMinimal.h"

#include "Containers/Ticker.h"

class SSubobjectEditor;
class FBlueprintEditor;
class SSCSEditor;
class FSubobjectEditorTreeNode;
class SWidget;
class STextBlock;
class SBorder;
class UBlueprint;
class UObject;

class SIMULATORMINIMALEDITOR_API FControlAssetDataHighlighter : public TSharedFromThis<FControlAssetDataHighlighter>
{
public:
	static void Initialize();
	static void Shutdown();
	static TSharedPtr<FControlAssetDataHighlighter> Get();

	/** Trigger a refresh of all open Blueprint editors */
	void Refresh();

	/** Checks whether a given object or component has UControlAssetData */
	static bool ObjectHasControlAssetData(UObject* InObject);

	~FControlAssetDataHighlighter();

private:

	FControlAssetDataHighlighter();

	void RegisterCallbacks();
	void UnregisterCallbacks();

	bool OnTick(float DeltaTime);
	void OnAssetEditorOpened(UObject* Asset);
	void OnObjectPropertyChanged(UObject* InObject, struct FPropertyChangedEvent& InPropertyChangedEvent);

	void ProcessBlueprintEditor(FBlueprintEditor* BlueprintEditor);
	void HighlightSCSEditor(TSharedPtr<SSubobjectEditor> SCSEditor, const TSet<FString>& GreenComponentNames);
	void TraverseAndHighlightWidgets(const TSharedRef<SWidget>& Widget, const TSet<FString>& GreenComponentNames, TOptional<bool> InRowHasAsset = TOptional<bool>());

	static bool DoesTextMatchComponentName(const FString& InText, const FString& ComponentName);

	void CollectGreenComponentNamesFromBlueprint(const UBlueprint* Blueprint, TSet<FString>& OutNames);
	void CollectGreenComponentNamesFromTree(const TSharedPtr<FSubobjectEditorTreeNode>& Node, TSet<FString>& OutNames);
	static UObject* GetObjectFromTreeNode(const TSharedPtr<FSubobjectEditorTreeNode>& Node);

	FTSTicker::FDelegateHandle TickerHandle;
	FDelegateHandle PropertyChangedHandle;
	FDelegateHandle AssetEditorOpenedHandle;

	// Track styled widgets so we can restore them if ControlAssetData is removed
	TSet<TWeakPtr<STextBlock>> HighlightedTextBlocks;
	TSet<TWeakPtr<SBorder>> HighlightedRowBorders;

	static TSharedPtr<FControlAssetDataHighlighter> Instance;
};