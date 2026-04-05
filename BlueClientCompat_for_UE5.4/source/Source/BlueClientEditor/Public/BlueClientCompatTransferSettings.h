#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "BlueClientCompatTransferSettings.generated.h"

class USkeletalMesh;
class UAnimBlueprint;
class UMaterialInterface;

UCLASS(BlueprintType)
class UBlueClientCompatTransferSettings : public UObject
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, Category="Transfer")
    USkeletalMesh* TargetSkeletalMesh = nullptr;

    UPROPERTY(EditAnywhere, Category="Transfer")
    UAnimBlueprint* PostProcessAnimBlueprint = nullptr;

    UPROPERTY(EditAnywhere, Category="Materials")
    UMaterialInterface* UniformSectionMaterial = nullptr;

    UPROPERTY(EditAnywhere, Category="Materials", meta=(ToolTip="Applied to every section's material slot name for in-game masking."))
    FString UniformMaterialSlotName;
};
