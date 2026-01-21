#include "PlayerTurnState.h"

#include "Engine.h"

namespace engine
{
  void PlayerTurnState::onEnter(std::shared_ptr<Engine> engine)
  {
    if (!engine)
    {
      return;
    }
    engine->phase = Phase::PLAYER_TURN;
  }

  std::vector<EngineCommandType> PlayerTurnState::getAllowedCommands()
  {
    std::vector<EngineCommandType> commands;
    commands.push_back(EngineCommandType::CMD_EXIT);
    return commands;
  }
}
