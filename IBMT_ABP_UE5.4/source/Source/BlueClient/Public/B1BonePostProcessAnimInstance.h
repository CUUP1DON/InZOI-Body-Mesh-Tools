#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "B1BonePostProcessAnimInstance.generated.h"

/**
 * Stub matching inZOI's UB1BonePostProcessAnimInstance from /Script/BlueClient.
 *
 * Set this class as the "Parent Class" when creating a new Animation Blueprint
 * (File -> New Animation Blueprint, Parent Class dropdown) to produce an ABP
 * that inZOI recognises as a valid post-process animation instance. The game's
 * own B1AnimNode_BoneGroupTransform node reads data from this instance at
 * runtime and drives the body sliders/morphs.
 *
 * No runtime logic lives here; the real implementation is inside the game
 * binary. This stub only exists so UE's reflection system can resolve the
 * class name and let you assign it as an ABP parent from the editor.
 *
 * When this plugin is compiled for a BlueClient project the module is renamed
 * to "BlueClient", making the UClass path /Script/BlueClient.B1BonePostProcessAnimInstance
 * which is exactly what inZOI expects.
 */
UCLASS(Blueprintable, BlueprintType)
class BLUECLIENT_API UB1BonePostProcessAnimInstance : public UAnimInstance
{
    GENERATED_BODY()

public:
    /** Left-hand body-fit blend weight driven by inZOI's slider system. */
    UPROPERTY(BlueprintReadWrite, Category = "inZOI")
    float BodyFitAlpha_Hand_L = 1.0f;

    /** Right-hand body-fit blend weight driven by inZOI's slider system. */
    UPROPERTY(BlueprintReadWrite, Category = "inZOI")
    float BodyFitAlpha_Hand_R = 1.0f;
};
