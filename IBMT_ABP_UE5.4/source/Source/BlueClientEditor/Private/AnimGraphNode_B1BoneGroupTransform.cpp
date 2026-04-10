#include "AnimGraphNode_B1BoneGroupTransform.h"

FText UAnimGraphNode_B1BoneGroupTransform::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
    return FText::FromString(TEXT("B1 Bone Group Transform"));
}

FText UAnimGraphNode_B1BoneGroupTransform::GetTooltipText() const
{
    return FText::FromString(TEXT(
        "Applies inZOI per-bone-group transforms driven by the body slider/morph parameters "
        "stored in the owning UB1BonePostProcessAnimInstance. "
        "Must be placed in the local-space section of the graph (before Convert Local to Component Space). "
        "The ABP parent class must be set to B1BonePostProcessAnimInstance for this node "
        "to carry morph data at runtime inside the game."));
}

FLinearColor UAnimGraphNode_B1BoneGroupTransform::GetNodeTitleColor() const
{
    // Warm yellow - visually distinct from standard anim nodes
    return FLinearColor(0.75f, 0.65f, 0.1f);
}

FString UAnimGraphNode_B1BoneGroupTransform::GetNodeCategory() const
{
    return TEXT("inZOI");
}
