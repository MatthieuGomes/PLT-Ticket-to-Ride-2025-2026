#include "PlayerController.h"

namespace engine
{
  parser::CommandMessage PlayerController::nextCommand(std::shared_ptr<state::State> state,
                                                       std::shared_ptr<Engine> engine)
  {
    (void)state;
    (void)engine;
    parser::CommandMessage message;
    message.kind = "command";
    message.origin = "tui";
    message.version = 1;
    message.name = "confirm";
    return message;
  }

  void PlayerController::notify(const EngineResult& result)
  {
    (void)result;
  }
}
