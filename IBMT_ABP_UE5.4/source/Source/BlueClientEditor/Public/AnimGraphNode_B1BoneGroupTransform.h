#pragma once

#include "CoreMinimal.h"
#include "AnimGraphNode_Base.h"
#include "B1AnimNode_BoneGroupTransform.h"
#include "AnimGraphNode_B1BoneGroupTransform.generated.h"

/**
 * Editor graph-node wrapper for FB1AnimNode_BoneGroupTransform.
 *
 * After compiling this plugin into your BlueClient project you can search for
 * "B1 Bone Group Transform" in the AnimGraph node palette and place it between
 * the local-space input pose and the ConvertLocalToComponentSpace node.
 *
 * Typical wiring for combined morphs + jiggle:
 *   [Linked Input Pose]
 *        ↓  local space
 *   [B1 Bone Group Transform]
 *        ↓  local space  (morphs processed at runtime by the game binary)
 *   [Convert Local to Component Space]
 *        ↓  component space
 *   [AnimDynamics …]
 *        ↓  component space
 *   [Convert Component to Local Space]
 *        ↓
 *   [Output Pose]
 */
UCLASS(MinimalAPI)
class UAnimGraphNode_B1BoneGroupTransform : public UAnimGraphNode_Base
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, Category = Settings)
    FB1AnimNode_BoneGroupTransform Node;

    // UEdGraphNode / UAnimGraphNode_Base display overrides
    virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
    virtual FText GetTooltipText() const override;
    virtual FLinearColor GetNodeTitleColor() const override;
    virtual FString GetNodeCategory() const override;
};
