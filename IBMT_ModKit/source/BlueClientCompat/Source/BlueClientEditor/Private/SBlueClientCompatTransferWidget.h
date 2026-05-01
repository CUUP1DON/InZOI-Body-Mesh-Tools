#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

class UBlueClientCompatTransferSettings;
class USkeletalMesh;
class SEditableTextBox;

class SBlueClientCompatTransferWidget : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SBlueClientCompatTransferWidget) {}
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs);

private:
    FReply OnTransferClicked();
    FReply OnBrowseJsonClicked();
    FReply OnBrowseBlenderJsonClicked();
    FReply OnApplyUniformMaterialsClicked();
    FReply OnSaveClicked();

    FText GetStatusText() const;

    FText StatusText;
    bool bLastTransferOk = false;

    TObjectPtr<UBlueClientCompatTransferSettings> Settings = nullptr;
    TSharedPtr<SEditableTextBox> JsonPathEdit;
    TSharedPtr<SEditableTextBox> BlenderJsonPathEdit;

    USkeletalMesh* GetTargetMesh() const;
};
