#include "BlueClientModule.h"

#include "B1MorphTargetUserData.h"
#include "Engine/SkeletalMesh.h"
#include "HAL/IConsoleManager.h"
#include "Modules/ModuleManager.h"
#include "Misc/PackageName.h"

static bool IsStripQuoteChar(TCHAR C)
{
    const uint32 U = static_cast<uint32>(static_cast<uint16>(C));
    switch (U)
    {
    case 0x0022: // "
    case 0x0027: // '
    case 0x201C:
    case 0x201D:
    case 0x2018:
    case 0x2019:
        return true;
    default:
        return false;
    }
}

static FString StripOuterQuotesAndTrim(FString Path)
{
    Path.TrimStartAndEndInline();
    for (int32 Guard = 0; Guard < 16 && Path.Len() > 0; ++Guard)
    {
        bool bChanged = false;
        while (Path.Len() > 0 && IsStripQuoteChar(Path[0]))
        {
            Path.RemoveAt(0);
            bChanged = true;
        }
        while (Path.Len() > 0 && IsStripQuoteChar(Path[Path.Len() - 1]))
        {
            Path.RemoveAt(Path.Len() - 1);
            bChanged = true;
        }
        Path.TrimStartAndEndInline();
        if (!bChanged)
        {
            break;
        }
    }
    return Path;
}

static FString NormalizeMeshPathForLoad(const FString& InPath)
{
    FString Path = StripOuterQuotesAndTrim(InPath);

    const int32 GameSlash = Path.Find(TEXT("/Game/"), ESearchCase::CaseSensitive);
    if (GameSlash != INDEX_NONE && GameSlash > 0)
    {
        Path = Path.Mid(GameSlash);
    }
    Path = StripOuterQuotesAndTrim(Path);

    for (int32 i = 0; i < Path.Len(); ++i)
    {
        const TCHAR C = Path[i];
        if (FChar::IsWhitespace(C) || IsStripQuoteChar(C))
        {
            Path.LeftInline(i, EAllowShrinking::No);
            break;
        }
    }
    Path = StripOuterQuotesAndTrim(Path);

    if (Path.StartsWith(TEXT("/Game/")) && !Path.Contains(TEXT(".")))
    {
        const FString AssetName = FPackageName::GetShortName(Path);
        Path = FString::Printf(TEXT("%s.%s"), *Path, *AssetName);
    }

    return Path;
}

IMPLEMENT_MODULE(FBlueClientModule, BlueClient)

void FBlueClientModule::StartupModule()
{
    ImportConsoleCommand = IConsoleManager::Get().RegisterConsoleCommand(
        TEXT("BlueClientImport"),
        TEXT("Imports B1MorphTargetUserData from an FModel JSON file into a skeletal mesh. "
             "Usage: BlueClientImport /Game/Path/To/SKM_Female_E E:/path/to/SKM_Female_Base_Original.json"),
        FConsoleCommandWithArgsDelegate::CreateLambda([](const TArray<FString>& Args)
        {
            if (Args.Num() < 2)
            {
                UE_LOG(LogTemp, Warning, TEXT("[BlueClientMorphTools] Usage: BlueClientImport <AssetPath> <JsonFilePath>"));
                UE_LOG(LogTemp, Warning, TEXT("[BlueClientMorphTools] Example: BlueClientImport /Game/Characters/SKM_Female_E E:/path/to/SKM_Female_Base_Original.json"));
                return;
            }

            FString AssetPath = NormalizeMeshPathForLoad(Args[0]);
            FString JsonPath = Args[1];
            for (int32 i = 2; i < Args.Num(); ++i)
            {
                JsonPath += TEXT(" ") + Args[i];
            }
            JsonPath = StripOuterQuotesAndTrim(JsonPath);

            UE_LOG(LogTemp, Display, TEXT("[BlueClientMorphTools] BlueClientImport: Loading mesh '%s'"), *AssetPath);

            USkeletalMesh* Mesh = LoadObject<USkeletalMesh>(nullptr, *AssetPath);
            if (!Mesh)
            {
                UE_LOG(LogTemp, Error, TEXT("[BlueClientMorphTools] Failed to load skeletal mesh at path: %s"), *AssetPath);
                UE_LOG(LogTemp, Error, TEXT("[BlueClientMorphTools] Make sure the path is like: /Game/Characters/Meshes/SKM_Female_E"));
                return;
            }

            FString OutMessage;
            const bool bSuccess = UBlueClientMorphTools::ImportB1MorphUserDataFromFModelJsonFile(Mesh, JsonPath, OutMessage);
            if (bSuccess)
            {
                UE_LOG(LogTemp, Display, TEXT("[BlueClientMorphTools] SUCCESS: %s"), *OutMessage);
                UE_LOG(LogTemp, Display, TEXT("[BlueClientMorphTools] SAVE YOUR MESH NOW."));
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("[BlueClientMorphTools] FAILED: %s"), *OutMessage);
            }
        }),
        ECVF_Default
    );

    SetABPConsoleCommand = IConsoleManager::Get().RegisterConsoleCommand(
        TEXT("BlueClientSetABP"),
        TEXT("Sets the PostProcessAnimBlueprint on a skeletal mesh. "
             "Usage: BlueClientSetABP /Game/Path/To/SKM /Game/Path/To/ABP.ABP_C"),
        FConsoleCommandWithArgsDelegate::CreateLambda([](const TArray<FString>& Args)
        {
            if (Args.Num() < 2)
            {
                UE_LOG(LogTemp, Warning, TEXT("[BlueClientMorphTools] Usage: BlueClientSetABP <MeshAssetPath> <ABPClassPath>"));
                UE_LOG(LogTemp, Warning, TEXT("[BlueClientMorphTools] Example: BlueClientSetABP /Game/Characters/AvatarParts/Body/Female/Mesh/SKM_Female_Base /Game/Characters/Anims/ABP_BonePostProcess.ABP_BonePostProcess_C"));
                return;
            }

            FString MeshPath = NormalizeMeshPathForLoad(Args[0]);
            FString ABPPath = StripOuterQuotesAndTrim(Args[1]);

            UE_LOG(LogTemp, Display, TEXT("[BlueClientMorphTools] BlueClientSetABP: Loading mesh '%s'"), *MeshPath);

            USkeletalMesh* Mesh = LoadObject<USkeletalMesh>(nullptr, *MeshPath);
            if (!Mesh)
            {
                UE_LOG(LogTemp, Error, TEXT("[BlueClientMorphTools] Failed to load skeletal mesh: %s"), *MeshPath);
                return;
            }

            FString OutMessage;
            const bool bSuccess = UBlueClientMorphTools::SetPostProcessAnimBlueprintByClassPath(Mesh, ABPPath, OutMessage);
            if (bSuccess)
            {
                UE_LOG(LogTemp, Display, TEXT("[BlueClientMorphTools] SUCCESS: %s"), *OutMessage);
                UE_LOG(LogTemp, Display, TEXT("[BlueClientMorphTools] SAVE YOUR MESH NOW."));
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("[BlueClientMorphTools] FAILED: %s"), *OutMessage);
            }
        }),
        ECVF_Default
    );

    UE_LOG(LogTemp, Display, TEXT("[BlueClientMorphTools] Plugin loaded. Commands ready: BlueClientImport, BlueClientSetABP"));
}

void FBlueClientModule::ShutdownModule()
{
    if (ImportConsoleCommand)
    {
        IConsoleManager::Get().UnregisterConsoleObject(ImportConsoleCommand);
        ImportConsoleCommand = nullptr;
    }
    if (SetABPConsoleCommand)
    {
        IConsoleManager::Get().UnregisterConsoleObject(SetABPConsoleCommand);
        SetABPConsoleCommand = nullptr;
    }
}
