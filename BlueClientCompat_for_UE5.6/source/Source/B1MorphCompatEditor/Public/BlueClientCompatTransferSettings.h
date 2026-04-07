#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "BlueClientCompatTransferSettings.generated.h"

class USkeletalMesh;
class UAnimBlueprint;
class UMaterialInterface;

UCLASS(BlueprintType)
class B1MORPHCOMPATEDITOR_API UBlueClientCompatTransferSettings : public UObject
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, Category="Transfer")
    USkeletalMesh* TargetSkeletalMesh = nullptr;

    UPROPERTY(EditAnywhere, Category="Transfer")
    UAnimBlueprint* PostProcessAnimBlueprint = nullptr;

    /** Applied to every skeletal mesh material slot when using "Apply uniform materials (all sections)". */
    UPROPERTY(EditAnywhere, Category="Materials")
    UMaterialInterface* UniformSectionMaterial = nullptr;

    /** Written to MaterialSlotName (and imported slot name in editor) for every section. Independent of the asset name. */
    UPROPERTY(EditAnywhere, Category="Materials", meta=(ToolTip="Applied to every section's material slot name for in-game masking."))
    FString UniformMaterialSlotName;
};

