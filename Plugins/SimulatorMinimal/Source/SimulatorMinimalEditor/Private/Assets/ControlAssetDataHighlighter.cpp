#include "Assets/ControlAssetDataHighlighter.h"

#include "Vehicle/Prototype/ControlAssetData.h"

#include "Editor.h"
#include "Subsystems/AssetEditorSubsystem.h"
#include "BlueprintEditor.h"
#include "SSCSEditor.h"
#include "SSubobjectEditor.h"
#include "SubobjectDataSubsystem.h"

#include "Engine/Blueprint.h"
#include "Engine/SimpleConstructionScript.h"
#include "Engine/SCS_Node.h"
#include "Components/ActorComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkinnedMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine/SkinnedAsset.h"

#include "Widgets/SWidget.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Views/STableRow.h"
#include "Widgets/Layout/SBorder.h"

TSharedPtr<FControlAssetDataHighlighter> FControlAssetDataHighlighter::Instance = nullptr;

void FControlAssetDataHighlighter::Initialize()
{
	if (!Instance.IsValid())
	{
		Instance = MakeShareable(new FControlAssetDataHighlighter());
		Instance->RegisterCallbacks();
	}
}

void FControlAssetDataHighlighter::Shutdown()
{
	if (Instance.IsValid())
	{
		Instance->UnregisterCallbacks();
		Instance.Reset();
	}
}

TSharedPtr<FControlAssetDataHighlighter> FControlAssetDataHighlighter::Get()
{
	return Instance;
}

FControlAssetDataHighlighter::FControlAssetDataHighlighter()
{
}

FControlAssetDataHighlighter::~FControlAssetDataHighlighter()
{
	UnregisterCallbacks();
}

void FControlAssetDataHighlighter::RegisterCallbacks()
{
	// Periodic ticker to keep outliner highlighted during scrolling, tree expansion, and compilation
	TickerHandle = FTSTicker::GetCoreTicker().AddTicker(
		FTickerDelegate::CreateRaw(this, &FControlAssetDataHighlighter::OnTick),
		0.2f
	);

	// Listen for new asset editors being opened
	if (GEditor)
	{
		if (UAssetEditorSubsystem* AssetEditorSubsystem = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>())
		{
			AssetEditorOpenedHandle = AssetEditorSubsystem->OnAssetEditorOpened().AddRaw(
				this, &FControlAssetDataHighlighter::OnAssetEditorOpened
			);
		}
	}

	// Listen for property changes (e.g. adding or removing ControlAssetData on a component)
	PropertyChangedHandle = FCoreUObjectDelegates::OnObjectPropertyChanged.AddRaw(
		this, &FControlAssetDataHighlighter::OnObjectPropertyChanged
	);
}

void FControlAssetDataHighlighter::UnregisterCallbacks()
{
	if (TickerHandle.IsValid())
	{
		FTSTicker::GetCoreTicker().RemoveTicker(TickerHandle);
		TickerHandle.Reset();
	}

	if (GEditor)
	{
		if (UAssetEditorSubsystem* AssetEditorSubsystem = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>())
		{
			if (AssetEditorOpenedHandle.IsValid())
			{
				AssetEditorSubsystem->OnAssetEditorOpened().Remove(AssetEditorOpenedHandle);
				AssetEditorOpenedHandle.Reset();
			}
		}
	}

	if (PropertyChangedHandle.IsValid())
	{
		FCoreUObjectDelegates::OnObjectPropertyChanged.Remove(PropertyChangedHandle);
		PropertyChangedHandle.Reset();
	}
}

bool FControlAssetDataHighlighter::OnTick(float DeltaTime)
{
	Refresh();
	return true; // Keep ticking
}

void FControlAssetDataHighlighter::OnAssetEditorOpened(UObject* Asset)
{
	if (UBlueprint* Blueprint = Cast<UBlueprint>(Asset))
	{
		if (Blueprint->ParentClass && Blueprint->ParentClass->IsChildOf(AActor::StaticClass()))
		{
			Refresh();
		}
	}
}

void FControlAssetDataHighlighter::OnObjectPropertyChanged(UObject* InObject, FPropertyChangedEvent& InPropertyChangedEvent)
{
	if (InObject && (InObject->IsA<UActorComponent>() || InObject->IsA<AActor>() || InObject->IsA<UControlAssetData>()))
	{
		Refresh();
	}
}

void FControlAssetDataHighlighter::Refresh()
{
	if (!GEditor)
	{
		return;
	}

	UAssetEditorSubsystem* AssetEditorSubsystem = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>();
	if (!AssetEditorSubsystem)
	{
		return;
	}

	const TArray<UObject*> EditedAssets = AssetEditorSubsystem->GetAllEditedAssets();
	for (UObject* EditedAsset : EditedAssets)
	{
		UBlueprint* Blueprint = Cast<UBlueprint>(EditedAsset);
		if (!Blueprint || !Blueprint->ParentClass || !Blueprint->ParentClass->IsChildOf(AActor::StaticClass()))
		{
			continue;
		}

		IAssetEditorInstance* EditorInstance = AssetEditorSubsystem->FindEditorForAsset(Blueprint, false);
		if (!EditorInstance)
		{
			continue;
		}

		FBlueprintEditor* BlueprintEditor = static_cast<FBlueprintEditor*>(EditorInstance);
		if (BlueprintEditor)
		{
			ProcessBlueprintEditor(BlueprintEditor);
		}
	}
}

void FControlAssetDataHighlighter::ProcessBlueprintEditor(FBlueprintEditor* BlueprintEditor)
{
	if (!BlueprintEditor)
	{
		return;
	}

	TSharedPtr<SSubobjectEditor> SCSEditor = BlueprintEditor->GetSubobjectEditor();
	if (!SCSEditor.IsValid())
	{
		return;
	}

	UBlueprint* Blueprint = BlueprintEditor->GetBlueprintObj();
	if (!Blueprint)
	{
		return;
	}

	TSet<FString> GreenComponentNames;
	CollectGreenComponentNamesFromBlueprint(Blueprint, GreenComponentNames);

	const TArray<TSharedPtr<FSubobjectEditorTreeNode>>& RootNodes = SCSEditor->GetRootNodes();
	for (const TSharedPtr<FSubobjectEditorTreeNode>& RootNode : RootNodes)
	{
		CollectGreenComponentNamesFromTree(RootNode, GreenComponentNames);
	}

	HighlightSCSEditor(SCSEditor, GreenComponentNames);
}

void FControlAssetDataHighlighter::CollectGreenComponentNamesFromBlueprint(const UBlueprint* Blueprint, TSet<FString>& OutNames)
{
	if (!Blueprint)
	{
		return;
	}

	// 1. Check Simple Construction Script (SCS) nodes
	if (Blueprint->SimpleConstructionScript)
	{
		for (USCS_Node* Node : Blueprint->SimpleConstructionScript->GetAllNodes())
		{
			if (Node && Node->ComponentTemplate)
			{
				if (ObjectHasControlAssetData(Node->ComponentTemplate))
				{
					OutNames.Add(Node->GetVariableName().ToString());
					OutNames.Add(Node->ComponentTemplate->GetName());
				}
			}
		}
	}

	// 2. Check Blueprint ComponentTemplates
	for (UActorComponent* ComponentTemplate : Blueprint->ComponentTemplates)
	{
		if (ComponentTemplate && ObjectHasControlAssetData(ComponentTemplate))
		{
			OutNames.Add(ComponentTemplate->GetName());
		}
	}

	// 3. Check inherited components via GeneratedClass Default Object
	if (Blueprint->GeneratedClass)
	{
		if (AActor* CDO = Cast<AActor>(Blueprint->GeneratedClass->GetDefaultObject()))
		{
			TInlineComponentArray<UActorComponent*> Components(CDO);
			for (UActorComponent* Comp : Components)
			{
				if (Comp && ObjectHasControlAssetData(Comp))
				{
					OutNames.Add(Comp->GetName());
				}
			}
		}
	}
}

void FControlAssetDataHighlighter::CollectGreenComponentNamesFromTree(
	const TSharedPtr<FSubobjectEditorTreeNode>& Node,
	TSet<FString>& OutNames)
{
	if (!Node.IsValid())
	{
		return;
	}

	if (UObject* Object = GetObjectFromTreeNode(Node))
	{
		if (ObjectHasControlAssetData(Object))
		{
			OutNames.Add(Node->GetDisplayString());
			OutNames.Add(Object->GetName());
			if (UActorComponent* Comp = Cast<UActorComponent>(Object))
			{
				OutNames.Add(Comp->GetFName().ToString());
			}
		}
	}

	const FSubobjectDataHandle& Handle = Node->GetDataHandle();
	if (Handle.IsValid())
	{
		if (FSubobjectData* SubobjectData = Handle.GetData())
		{
			if (auto Object = (UObject*)SubobjectData->GetObject())
			{
				if (ObjectHasControlAssetData(Object))
				{
					OutNames.Add(Node->GetDisplayString());
					OutNames.Add(SubobjectData->GetVariableName().ToString());
					OutNames.Add(Object->GetName());
				}
			}
		}
	}

	for (const TSharedPtr<FSubobjectEditorTreeNode>& ChildNode : Node->GetChildren())
	{
		CollectGreenComponentNamesFromTree(ChildNode, OutNames);
	}
}

UObject* FControlAssetDataHighlighter::GetObjectFromTreeNode(const TSharedPtr<FSubobjectEditorTreeNode>& Node)
{
	if (!Node.IsValid())
	{
		return nullptr;
	}

	if (UActorComponent* CompTemplate = (UActorComponent*)Node->GetComponentTemplate())
	{
		return CompTemplate;
	}

	const FSubobjectDataHandle& Handle = Node->GetDataHandle();
	if (Handle.IsValid())
	{
		if (FSubobjectData* SubobjectData = Handle.GetData())
		{
			if (auto Object = (UObject*)SubobjectData->GetObject())
			{
				if (USCS_Node* SCSNode = Cast<USCS_Node>(Object))
				{
					return SCSNode->ComponentTemplate;
				}
				return Object;
			}
		}
	}

	return nullptr;
}

bool FControlAssetDataHighlighter::ObjectHasControlAssetData(UObject* InObject)
{
	if (!InObject || InObject->IsA<AActor>())
	{
		return false;
	}

	// If it's a USCS_Node, check its ComponentTemplate
	if (const USCS_Node* SCSNode = Cast<USCS_Node>(InObject))
	{
		return ObjectHasControlAssetData(SCSNode->ComponentTemplate);
	}

	// 1. Direct check using project's UControlAssetDataStatics
	if (UControlAssetData* Data = UControlAssetDataStatics::GetControlAssetData(const_cast<UObject*>(InObject)))
	{
		return true;
	}

	// 2. Direct check on IInterface_AssetUserData
	if (IInterface_AssetUserData* AssetUserData = Cast<IInterface_AssetUserData>(InObject))
	{
		if (AssetUserData->GetAssetUserDataOfClass(UControlAssetData::StaticClass()) != nullptr)
		{
			return true;
		}
	}

	// 3. If it's a UStaticMeshComponent, check the StaticMesh asset
	if (const UStaticMeshComponent* SMC = Cast<UStaticMeshComponent>(InObject))
	{
		if (UStaticMesh* Mesh = SMC->GetStaticMesh())
		{
			if (Mesh->GetAssetUserDataOfClass(UControlAssetData::StaticClass()) != nullptr)
			{
				return true;
			}
		}
	}

	// 4. If it's a USkinnedMeshComponent, check the SkinnedAsset
	if (const USkinnedMeshComponent* SkinnedComp = Cast<USkinnedMeshComponent>(InObject))
	{
		if (UObject* SkinnedAsset = SkinnedComp->GetSkinnedAsset())
		{
			if (IInterface_AssetUserData* MeshUserData = Cast<IInterface_AssetUserData>(SkinnedAsset))
			{
				if (MeshUserData->GetAssetUserDataOfClass(UControlAssetData::StaticClass()) != nullptr)
				{
					return true;
				}
			}
		}
	}

	return false;
}

bool FControlAssetDataHighlighter::DoesTextMatchComponentName(const FString& InText, const FString& ComponentName)
{
	if (ComponentName.IsEmpty() || InText.IsEmpty())
	{
		return false;
	}

	if (InText.Equals(ComponentName, ESearchCase::IgnoreCase))
	{
		return true;
	}

	// In the Components tree, component names may have decorators such as:
	// - "FuelTank (Inherited)"
	// - "FuelTank (StaticMeshComponent)"
	// - "FuelTank*" (unsaved changes indicator)
	// We MUST ensure the matching name is followed immediately by a decorator delimiter (' ', '(', or '*')
	// so that other components like "FuelTankArrow" or "FuelTank_01" are NEVER falsely matched!
	if (InText.StartsWith(ComponentName, ESearchCase::IgnoreCase))
	{
		const int32 PrefixLen = ComponentName.Len();
		if (InText.Len() > PrefixLen)
		{
			const TCHAR NextChar = InText[PrefixLen];
			if (NextChar == TEXT(' ') || NextChar == TEXT('(') || NextChar == TEXT('*'))
			{
				return true;
			}
		}
	}

	return false;
}

void FControlAssetDataHighlighter::HighlightSCSEditor(
	TSharedPtr<SSubobjectEditor> SCSEditor,
	const TSet<FString>& GreenComponentNames)
{
	if (!SCSEditor.IsValid())
	{
		return;
	}

	TraverseAndHighlightWidgets(SCSEditor.ToSharedRef(), GreenComponentNames);
}

void FControlAssetDataHighlighter::TraverseAndHighlightWidgets(
	const TSharedRef<SWidget>& Widget,
	const TSet<FString>& GreenComponentNames,
	TOptional<bool> InRowHasAsset)
{
	TOptional<bool> CurrentRowHasAsset = InRowHasAsset;

	// 1. Check if this widget is an SSubobject_RowWidget (UE5 Components tree row)
	if (Widget->GetType() == FName(TEXT("SSubobject_RowWidget")))
	{
		TSharedRef<SSubobject_RowWidget> RowWidget = StaticCastSharedRef<SSubobject_RowWidget>(Widget);
		FSubobjectEditorTreeNodePtrType Node = RowWidget->GetSubobjectPtr();

		bool bRowHasAsset = false;
		if (Node.IsValid())
		{
			// Direct check on the component or template belonging to this node
			if (UObject* Object = GetObjectFromTreeNode(Node))
			{
				bRowHasAsset = ObjectHasControlAssetData(Object);
			}

			if (!bRowHasAsset)
			{
				const FSubobjectDataHandle& Handle = Node->GetDataHandle();
				if (Handle.IsValid())
				{
					if (const FSubobjectData* SubobjectData = Handle.GetData())
					{
						if (UObject* DataObj = (UObject*)SubobjectData->GetObject())
						{
							if (const USCS_Node* SCSNode = Cast<USCS_Node>(DataObj))
							{
								bRowHasAsset = ObjectHasControlAssetData(SCSNode->ComponentTemplate);
							}
							else
							{
								bRowHasAsset = ObjectHasControlAssetData(DataObj);
							}
						}
					}
				}
			}

			// Fallback: exact match of node display string in GreenComponentNames
			if (!bRowHasAsset)
			{
				const FString DisplayStr = Node->GetDisplayString();
				if (GreenComponentNames.Contains(DisplayStr))
				{
					bRowHasAsset = true;
				}
			}
		}

		CurrentRowHasAsset = bRowHasAsset;

		if (bRowHasAsset)
		{
			// Soft green row background tint and helpful tooltip
			static const FSlateColor GreenRowBgColor(FLinearColor(0.06f, 0.35f, 0.12f, 0.35f));
			RowWidget->SetBorderBackgroundColor(GreenRowBgColor);
			RowWidget->SetToolTipText(FText::FromString(TEXT("ControlAssetData is attached to this component")));
			HighlightedRowBorders.Add(RowWidget);
		}
		else if (HighlightedRowBorders.Contains(RowWidget))
		{
			RowWidget->SetBorderBackgroundColor(FLinearColor::Transparent);
			RowWidget->SetToolTipText(FText::GetEmpty());
			HighlightedRowBorders.Remove(RowWidget);
		}
	}
	// 2. Legacy / fallback check for table rows
	else if (Widget->GetType() == FName(TEXT("SSCS_RowWidget")) ||
	         Widget->GetType() == FName(TEXT("STableRow")))
	{
		bool bRowContainsGreenComponent = false;
		TArray<TSharedRef<SWidget>> SubWidgets;
		SubWidgets.Add(Widget);

		while (SubWidgets.Num() > 0)
		{
			TSharedRef<SWidget> Current = SubWidgets.Pop();
			if (Current->GetType() == FName(TEXT("STextBlock")))
			{
				TSharedRef<STextBlock> SubTextBlock = StaticCastSharedRef<STextBlock>(Current);
				const FString SubText = SubTextBlock->GetText().ToString();
				for (const FString& GreenName : GreenComponentNames)
				{
					if (DoesTextMatchComponentName(SubText, GreenName))
					{
						bRowContainsGreenComponent = true;
						break;
					}
				}
				if (bRowContainsGreenComponent)
				{
					break;
				}
			}

			FChildren* SubChildren = Current->GetChildren();
			for (int32 i = 0; i < SubChildren->Num(); ++i)
			{
				SubWidgets.Add(SubChildren->GetChildAt(i));
			}
		}

		CurrentRowHasAsset = bRowContainsGreenComponent;

		TSharedRef<SBorder> RowBorder = StaticCastSharedRef<SBorder>(Widget);
		if (bRowContainsGreenComponent)
		{
			static const FSlateColor GreenRowBgColor(FLinearColor(0.06f, 0.35f, 0.12f, 0.35f));
			RowBorder->SetBorderBackgroundColor(GreenRowBgColor);
			RowBorder->SetToolTipText(FText::FromString(TEXT("ControlAssetData is attached to this component")));
			HighlightedRowBorders.Add(RowBorder);
		}
		else if (HighlightedRowBorders.Contains(RowBorder))
		{
			RowBorder->SetBorderBackgroundColor(FLinearColor::Transparent);
			RowBorder->SetToolTipText(FText::GetEmpty());
			HighlightedRowBorders.Remove(RowBorder);
		}
	}
	// 3. Check if this widget is an STextBlock (Component name display)
	else if (Widget->GetType() == FName(TEXT("STextBlock")))
	{
		TSharedRef<STextBlock> TextBlock = StaticCastSharedRef<STextBlock>(Widget);
		const FString Text = TextBlock->GetText().ToString();

		if (!Text.IsEmpty())
		{
			bool bShouldBeGreen = false;

			if (CurrentRowHasAsset.IsSet())
			{
				if (CurrentRowHasAsset.GetValue())
				{
					for (const FString& GreenName : GreenComponentNames)
					{
						if (DoesTextMatchComponentName(Text, GreenName))
						{
							bShouldBeGreen = true;
							break;
						}
					}
				}
				else
				{
					// If the containing row specifically does NOT have the asset, do not highlight
					bShouldBeGreen = false;
				}
			}
			else
			{
				for (const FString& GreenName : GreenComponentNames)
				{
					if (DoesTextMatchComponentName(Text, GreenName))
					{
						bShouldBeGreen = true;
						break;
					}
				}
			}

			if (bShouldBeGreen)
			{
				// Highlight with bright emerald green color
				static const FSlateColor GreenColor(FLinearColor(0.2f, 1.0f, 0.25f, 1.0f));
				TextBlock->SetColorAndOpacity(GreenColor);
				TextBlock->SetShadowColorAndOpacity(FLinearColor(0.0f, 0.25f, 0.05f, 0.9f));
				TextBlock->SetShadowOffset(FVector2D(1.0f, 1.0f));

				HighlightedTextBlocks.Add(TextBlock);
			}
			else if (HighlightedTextBlocks.Contains(TextBlock))
			{
				// Reset color if ControlAssetData was removed or this component shouldn't be highlighted
				TextBlock->SetColorAndOpacity(FSlateColor::UseForeground());
				TextBlock->SetShadowOffset(FVector2D::ZeroVector);
				HighlightedTextBlocks.Remove(TextBlock);
			}
		}
	}

	// Recurse into all children, propagating the row state down
	FChildren* Children = Widget->GetChildren();
	for (int32 ChildIdx = 0; ChildIdx < Children->Num(); ++ChildIdx)
	{
		TraverseAndHighlightWidgets(Children->GetChildAt(ChildIdx), GreenComponentNames, CurrentRowHasAsset);
	}
}