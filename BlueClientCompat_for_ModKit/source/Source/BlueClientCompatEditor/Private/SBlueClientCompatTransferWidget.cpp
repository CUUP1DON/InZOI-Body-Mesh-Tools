#include "SBlueClientCompatTransferWidget.h"

#include "BCCE_SkmMatUniformizer.h"
#include "BlueClientCompatTransferSettings.h"

#include "Animation/AnimBlueprint.h"
#include "AssetRegistry/AssetData.h"
#include "B1MorphTargetUserData.h"
#include "Engine/SkeletalMesh.h"
#include "FileHelpers.h"
#include "HAL/FileManager.h"
#include "Materials/MaterialInterface.h"
#include "PropertyCustomizationHelpers.h"
#include "ThumbnailRendering/ThumbnailManager.h"

#include "Editor.h"
#include "Misc/Paths.h"
#include "DesktopPlatformModule.h"
#include "IDesktopPlatform.h"
#include "Styling/AppStyle.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Text/STextBlock.h"

namespace
{
    FSlateFontInfo HintFont()
    {
        return FAppStyle::GetFontStyle(TEXT("SmallFont"));
    }

    FSlateFontInfo LabelFont()
    {
        return FAppStyle::GetFontStyle(TEXT("PropertyWindow.NormalFont"));
    }

    /** Details-style category header + body without collapse (always open). */
    TSharedRef<SWidget> MakeStaticSection(const FText& Title, TSharedRef<SWidget> Body)
    {
        return SNew(SVerticalBox)
            + SVerticalBox::Slot()
                  .AutoHeight()
            [
                SNew(SBorder)
                    .BorderImage(FAppStyle::Get().GetBrush(TEXT("DetailsView.CategoryTop")))
                    .BorderBackgroundColor(FAppStyle::GetSlateColor(TEXT("Colors.Panel")))
                    .Padding(FMargin(8.f, 6.f, 8.f, 6.f))
                [
                    SNew(STextBlock)
                        .Text(Title)
                        .TextStyle(FAppStyle::Get(), TEXT("DetailsView.CategoryTextStyle"))
                ]
            ]
            + SVerticalBox::Slot()
                  .AutoHeight()
            [
                SNew(SBorder)
                    .BorderImage(FAppStyle::Get().GetBrush(TEXT("DetailsView.CategoryMiddle")))
                    .BorderBackgroundColor(FAppStyle::GetSlateColor(TEXT("Colors.Panel")))
                    .Padding(FMargin(6.f, 8.f, 6.f, 8.f))
                [
                    Body
                ]
            ];
    }

    TSharedRef<SWidget> MakeLabeledAssetPicker(
        const FText& Label,
        UClass* AllowedClass,
        const TSharedPtr<FAssetThumbnailPool>& ThumbnailPool,
        TAttribute<FString> ObjectPath,
        FOnSetObject OnObjectChanged)
    {
        return SNew(SHorizontalBox)
            + SHorizontalBox::Slot()
                  .AutoWidth()
                  .VAlign(VAlign_Center)
                  .Padding(0.f, 6.f, 14.f, 6.f)
            [
                SNew(SBox)
                    .MinDesiredWidth(196.f)
                    .VAlign(VAlign_Center)
                [
                    SNew(STextBlock)
                        .Text(Label)
                        .Font(LabelFont())
                ]
            ]
            + SHorizontalBox::Slot()
                  .FillWidth(1.f)
                  .VAlign(VAlign_Center)
                  .Padding(0.f, 4.f)
            [
                SNew(SObjectPropertyEntryBox)
                    .ObjectPath(ObjectPath)
                    .AllowedClass(AllowedClass)
                    .ThumbnailPool(ThumbnailPool)
                    .AllowClear(true)
                    .DisplayUseSelected(true)
                    .DisplayBrowse(true)
                    .DisplayThumbnail(true)
                    .OnObjectChanged(OnObjectChanged)
            ];
    }

    TSharedRef<SWidget> MakeLabeledTextBox(const FText& Label, const TSharedRef<SEditableTextBox>& TextBox)
    {
        return SNew(SHorizontalBox)
            + SHorizontalBox::Slot()
                  .AutoWidth()
                  .VAlign(VAlign_Center)
                  .Padding(0.f, 6.f, 14.f, 6.f)
            [
                SNew(SBox)
                    .MinDesiredWidth(196.f)
                    .VAlign(VAlign_Center)
                [
                    SNew(STextBlock)
                        .Text(Label)
                        .Font(LabelFont())
                ]
            ]
            + SHorizontalBox::Slot()
                  .FillWidth(1.f)
                  .VAlign(VAlign_Center)
                  .Padding(0.f, 4.f)
            [
                TextBox
            ];
    }

    TSharedRef<SWidget> MakeFullWidthButton(const TSharedRef<SButton>& Button)
    {
        return SNew(SHorizontalBox)
            + SHorizontalBox::Slot()
                  .FillWidth(1.f)
            [
                Button
            ];
    }
}

void SBlueClientCompatTransferWidget::Construct(const FArguments& InArgs)
{
    StatusText = FText::GetEmpty();

    Settings = NewObject<UBlueClientCompatTransferSettings>(GetTransientPackage());
    Settings->AddToRoot();

    const TSharedPtr<FAssetThumbnailPool> ThumbnailPool = UThumbnailManager::Get().GetSharedThumbnailPool();

    const TSharedRef<SVerticalBox> TransferBody = SNew(SVerticalBox)
        + SVerticalBox::Slot()
              .AutoHeight()
              .Padding(0.f, 0.f, 0.f, 8.f)
        [
            SNew(STextBlock)
                .Text(FText::FromString(TEXT("Select assets and click Transfer.")))
                .Font(HintFont())
                .ColorAndOpacity(FSlateColor::UseSubduedForeground())
                .AutoWrapText(true)
        ]
        + SVerticalBox::Slot()
              .AutoHeight()
              .Padding(0.f, 0.f, 0.f, 12.f)
        [
            SNew(SHorizontalBox)
            + SHorizontalBox::Slot()
                  .FillWidth(1.f)
                  .VAlign(VAlign_Center)
            [
                SAssignNew(JsonPathEdit, SEditableTextBox)
                    .HintText(FText::FromString(TEXT("Original InZOI body JSON file path")))
                    .Font(LabelFont())
            ]
            + SHorizontalBox::Slot()
                  .AutoWidth()
                  .Padding(8.f, 0.f, 0.f, 0.f)
                  .VAlign(VAlign_Center)
            [
                SNew(SButton)
                    .Text(FText::FromString(TEXT("browse inZOI Body JSON")))
                    .OnClicked(this, &SBlueClientCompatTransferWidget::OnBrowseJsonClicked)
            ]
        ]
        + SVerticalBox::Slot()
              .AutoHeight()
        [
            MakeLabeledAssetPicker(
                FText::FromString(TEXT("Target Skeletal Mesh")),
                USkeletalMesh::StaticClass(),
                ThumbnailPool,
                TAttribute<FString>::CreateLambda([this]()
                {
                    return (Settings && Settings->TargetSkeletalMesh) ? Settings->TargetSkeletalMesh->GetPathName() : FString();
                }),
                FOnSetObject::CreateLambda([this](const FAssetData& AssetData)
                {
                    if (!Settings)
                    {
                        return;
                    }
                    Settings->Modify();
                    Settings->TargetSkeletalMesh =
                        AssetData.IsValid() ? Cast<USkeletalMesh>(AssetData.GetAsset()) : nullptr;
                }))
        ]
        + SVerticalBox::Slot()
              .AutoHeight()
        [
            MakeLabeledAssetPicker(
                FText::FromString(TEXT("Post Process Anim Blueprint")),
                UAnimBlueprint::StaticClass(),
                ThumbnailPool,
                TAttribute<FString>::CreateLambda([this]()
                {
                    return (Settings && Settings->PostProcessAnimBlueprint) ? Settings->PostProcessAnimBlueprint->GetPathName()
                                                                          : FString();
                }),
                FOnSetObject::CreateLambda([this](const FAssetData& AssetData)
                {
                    if (!Settings)
                    {
                        return;
                    }
                    Settings->Modify();
                    Settings->PostProcessAnimBlueprint =
                        AssetData.IsValid() ? Cast<UAnimBlueprint>(AssetData.GetAsset()) : nullptr;
                }))
        ]
        + SVerticalBox::Slot()
              .AutoHeight()
              .Padding(0.f, 10.f, 0.f, 2.f)
        [
            MakeFullWidthButton(
                SNew(SButton)
                    .Text(FText::FromString(TEXT("Transfer")))
                    .HAlign(HAlign_Center)
                    .OnClicked(this, &SBlueClientCompatTransferWidget::OnTransferClicked))
        ];

    const TSharedRef<SVerticalBox> MaterialsBody = SNew(SVerticalBox)
        + SVerticalBox::Slot()
              .AutoHeight()
              .Padding(0.f, 0.f, 0.f, 8.f)
        [
            SNew(STextBlock)
                .Text(FText::FromString(TEXT("Select assets and click apply")))
                .Font(HintFont())
                .ColorAndOpacity(FSlateColor::UseSubduedForeground())
                .AutoWrapText(true)
        ]
        + SVerticalBox::Slot()
              .AutoHeight()
        [
            MakeLabeledAssetPicker(
                FText::FromString(TEXT("Uniform Section Material")),
                UMaterialInterface::StaticClass(),
                ThumbnailPool,
                TAttribute<FString>::CreateLambda([this]()
                {
                    return (Settings && Settings->UniformSectionMaterial) ? Settings->UniformSectionMaterial->GetPathName() : FString();
                }),
                FOnSetObject::CreateLambda([this](const FAssetData& AssetData)
                {
                    if (!Settings)
                    {
                        return;
                    }
                    Settings->Modify();
                    Settings->UniformSectionMaterial =
                        AssetData.IsValid() ? Cast<UMaterialInterface>(AssetData.GetAsset()) : nullptr;
                }))
        ]
        + SVerticalBox::Slot()
              .AutoHeight()
        [
            MakeLabeledTextBox(
                FText::FromString(TEXT("Uniform Material Slot Name")),
                SNew(SEditableTextBox)
                    .HintText(FText::FromString(TEXT("i.e. MI_Female_Body_001")))
                    .Text(FText::FromString(Settings->UniformMaterialSlotName))
                    .Font(LabelFont())
                    .OnTextChanged_Lambda([this](const FText& NewText)
                    {
                        if (Settings)
                        {
                            Settings->UniformMaterialSlotName = NewText.ToString();
                        }
                    }))
        ]
        + SVerticalBox::Slot()
              .AutoHeight()
              .Padding(0.f, 10.f, 0.f, 2.f)
        [
            MakeFullWidthButton(
                SNew(SButton)
                    .Text(FText::FromString(TEXT("Apply materials")))
                    .ToolTipText(FText::FromString(TEXT(
                        "Sets every material slot on the target skeletal mesh to Uniform Section Material and renames each slot to Uniform Material Slot Name. Does not merge sections — only updates the materials list. Save the skeletal mesh after.")))
                    .HAlign(HAlign_Center)
                    .OnClicked(this, &SBlueClientCompatTransferWidget::OnApplyUniformMaterialsClicked))
        ];

    ChildSlot
    [
        SNew(SScrollBox)
        + SScrollBox::Slot()
              .Padding(8.f)
        [
            SNew(SBorder)
                .BorderImage(FAppStyle::GetBrush(TEXT("ToolPanel.GroupBorder")))
                .Padding(FMargin(12.f, 14.f))
            [
                SNew(SVerticalBox)
                + SVerticalBox::Slot()
                      .AutoHeight()
                      .Padding(0.f, 0.f, 0.f, 8.f)
                [
                    MakeStaticSection(FText::FromString(TEXT("Transfer")), TransferBody)
                ]
                + SVerticalBox::Slot()
                      .AutoHeight()
                      .Padding(0.f, 4.f, 0.f, 8.f)
                [
                    MakeStaticSection(FText::FromString(TEXT("Materials")), MaterialsBody)
                ]
                + SVerticalBox::Slot()
                      .AutoHeight()
                      .Padding(0.f, 12.f, 0.f, 8.f)
                      .HAlign(HAlign_Left)
                [
                    MakeFullWidthButton(
                        SNew(SButton)
                            .Text(FText::FromString(TEXT("Save Packages")))
                            .HAlign(HAlign_Center)
                            .OnClicked(this, &SBlueClientCompatTransferWidget::OnSaveClicked))
                ]
                + SVerticalBox::Slot()
                      .AutoHeight()
                      .Padding(0.f, 2.f, 0.f, 0.f)
                [
                    SNew(STextBlock)
                        .Text(this, &SBlueClientCompatTransferWidget::GetStatusText)
                        .Font(LabelFont())
                        .ColorAndOpacity_Lambda([this]()
                        {
                            return bLastTransferOk ? FSlateColor(FLinearColor(0.35f, 0.85f, 0.45f))
                                                   : FSlateColor::UseForeground();
                        })
                        .AutoWrapText(true)
                ]
            ]
        ]
    ];
}

FText SBlueClientCompatTransferWidget::GetStatusText() const
{
    return StatusText;
}

USkeletalMesh* SBlueClientCompatTransferWidget::GetTargetMesh() const
{
    return Settings ? Settings->TargetSkeletalMesh : nullptr;
}

FReply SBlueClientCompatTransferWidget::OnTransferClicked()
{
    bLastTransferOk = false;
    StatusText = FText::FromString(TEXT("Running import..."));

    USkeletalMesh* TargetMesh = GetTargetMesh();
    if (!TargetMesh)
    {
        StatusText = FText::FromString(TEXT("No target SkeletalMesh selected."));
        return FReply::Handled();
    }

    const FString JsonFilePath = JsonPathEdit.IsValid() ? JsonPathEdit->GetText().ToString() : FString();
    if (JsonFilePath.IsEmpty())
    {
        StatusText = FText::FromString(TEXT("No Original InZOI body JSON path set."));
        return FReply::Handled();
    }

    if (!FPaths::FileExists(JsonFilePath))
    {
        StatusText = FText::FromString(FString::Printf(TEXT("JSON file does not exist: %s"), *JsonFilePath));
        return FReply::Handled();
    }

    FString AnimBlueprintGeneratedClassPath;
    if (Settings && Settings->PostProcessAnimBlueprint)
    {
        if (Settings->PostProcessAnimBlueprint->GeneratedClass)
        {
            AnimBlueprintGeneratedClassPath = Settings->PostProcessAnimBlueprint->GeneratedClass->GetPathName();
        }
        else
        {
            AnimBlueprintGeneratedClassPath = Settings->PostProcessAnimBlueprint->GetPathName();
        }
    }

    if (AnimBlueprintGeneratedClassPath.IsEmpty())
    {
        StatusText = FText::FromString(TEXT("No PostProcess AnimBP selected."));
        return FReply::Handled();
    }

    FString OutMessage;
    const bool bImportOk = UBlueClientMorphTools::ImportB1MorphUserDataFromFModelJsonFile(TargetMesh, JsonFilePath, OutMessage);
    if (!bImportOk)
    {
        StatusText = FText::FromString(OutMessage);
        return FReply::Handled();
    }

    const bool bSetOk = UBlueClientMorphTools::SetPostProcessAnimBlueprintByClassPath(TargetMesh, AnimBlueprintGeneratedClassPath, OutMessage);
    if (!bSetOk)
    {
        StatusText = FText::FromString(OutMessage);
        return FReply::Handled();
    }

    bLastTransferOk = true;
    StatusText = FText::FromString(FString::Printf(TEXT("Transfer complete. %s Save the mesh asset."), *OutMessage));
    return FReply::Handled();
}

FReply SBlueClientCompatTransferWidget::OnBrowseJsonClicked()
{
    bLastTransferOk = false;
    IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();
    if (!DesktopPlatform)
    {
        StatusText = FText::FromString(TEXT("DesktopPlatform module not available."));
        return FReply::Handled();
    }

    const void* ParentWindowHandle = nullptr;
    TArray<FString> OutFilenames;
    const FString DefaultPath = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir());
    const bool bOpened = DesktopPlatform->OpenFileDialog(
        const_cast<void*>(ParentWindowHandle),
        TEXT("Select inZOI body JSON file"),
        DefaultPath,
        TEXT(""),
        TEXT("JSON files (*.json)|*.json"),
        EFileDialogFlags::None,
        OutFilenames);

    if (bOpened && OutFilenames.Num() > 0)
    {
        const FString& OutFilePath = OutFilenames[0];
        if (JsonPathEdit.IsValid())
        {
            JsonPathEdit->SetText(FText::FromString(OutFilePath));
        }
        StatusText = FText::FromString(FString::Printf(TEXT("Selected JSON: %s"), *OutFilePath));
    }
    else
    {
        StatusText = FText::FromString(TEXT("JSON selection cancelled."));
    }

    return FReply::Handled();
}

FReply SBlueClientCompatTransferWidget::OnApplyUniformMaterialsClicked()
{
    USkeletalMesh* TargetMesh = GetTargetMesh();
    if (!TargetMesh)
    {
        StatusText = FText::FromString(TEXT("No target SkeletalMesh selected."));
        return FReply::Handled();
    }

    if (!Settings)
    {
        StatusText = FText::FromString(TEXT("Internal error: settings are null."));
        return FReply::Handled();
    }

    FString OutMessage;
    const bool bOk = BlueClientCompatSkmMaterialUniformizer::ApplyUniformMaterialsToAllSlots(
        TargetMesh,
        Settings->UniformSectionMaterial,
        Settings->UniformMaterialSlotName,
        OutMessage);

    if (bOk)
    {
        bLastTransferOk = true;
        StatusText = FText::FromString(FString::Printf(TEXT("%s Save the skeletal mesh asset."), *OutMessage));
    }
    else
    {
        bLastTransferOk = false;
        StatusText = FText::FromString(OutMessage);
    }
    return FReply::Handled();
}

FReply SBlueClientCompatTransferWidget::OnSaveClicked()
{
    bLastTransferOk = false;
    StatusText = FText::FromString(TEXT("Saving dirty packages..."));
    FEditorFileUtils::SaveDirtyPackages(true, false, true);
    StatusText = FText::FromString(TEXT("Save requested. If prompted by source control / file dialogs, complete them."));
    return FReply::Handled();
}
