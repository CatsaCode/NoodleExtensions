#include "beatsaber-hook/shared/utils/il2cpp-utils.hpp"
#include "beatsaber-hook/shared/utils/hooking.hpp"

#include "GlobalNamespace/GameplayCoreInstaller.hpp"
#include "GlobalNamespace/GameplayCoreSceneSetupData.hpp"
#include "GlobalNamespace/BeatmapDifficulty.hpp"
#include "GlobalNamespace/BeatmapDifficultyMethods.hpp"
#include "GlobalNamespace/GameplayModifiers.hpp"
#include "GlobalNamespace/PlayerSpecificSettings.hpp"
#include "GlobalNamespace/BeatmapObjectSpawnControllerHelpers.hpp"
#include "GlobalNamespace/IReadonlyBeatmapData.hpp"
#include "GlobalNamespace/BeatmapLevel.hpp"
#include "GlobalNamespace/BeatmapBasicData.hpp"

#include "Zenject/DiContainer.hpp"

#include "NECaches.h"
#include "NEHooks.h"
#include "GlobalNamespace/BeatmapObjectSpawnMovementData.hpp"

using namespace GlobalNamespace;

MAKE_HOOK_MATCH(InstallBindings, &GameplayCoreInstaller::InstallBindings, void, GameplayCoreInstaller* self) {
  if (!Hooks::isNoodleHookEnabled()) return InstallBindings(self);

  auto* difficultyBeatmap = self->_sceneSetupData->beatmapBasicData;
  GameplayModifiers* gameplayModifiers = self->_sceneSetupData->gameplayModifiers;

  BeatmapObjectSpawnControllerHelpers::GetNoteJumpValues(
      self->_sceneSetupData->playerSpecificSettings, difficultyBeatmap->noteJumpStartBeatOffset,
      ByRef(NECaches::noteJumpValueType), ByRef(NECaches::noteJumpValue));

  NECaches::noteJumpStartBeatOffset = difficultyBeatmap->noteJumpStartBeatOffset +
                                      self->_sceneSetupData->playerSpecificSettings->noteJumpStartBeatOffset;
  NECaches::beatsPerMinute = self->_sceneSetupData->___beatmapLevel->beatsPerMinute;
  NECaches::numberOfLines = self->_sceneSetupData->transformedBeatmapData->numberOfLines;

  InstallBindings(self);

  NECaches::GameplayCoreContainer = self->Container;

  NECaches::JumpOffsetYProvider = self->Container->Resolve<GlobalNamespace::IJumpOffsetYProvider*>();
  NECaches::VariableMovementDataProvider = self->Container->Resolve<GlobalNamespace::VariableMovementDataProvider*>();
  NECaches::InitData = self->Container->Resolve<GlobalNamespace::BeatmapObjectSpawnController::InitData*>();
}

void InstallGameplayCoreInstallerHooks() {
  INSTALL_HOOK(NELogger::Logger, InstallBindings);
}

NEInstallHooks(InstallGameplayCoreInstallerHooks);