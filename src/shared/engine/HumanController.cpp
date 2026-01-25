#include "HumanController.h"

namespace engine
{
  parser::CommandMessage HumanController::nextCommand(std::shared_ptr<state::State> state,
                                                      std::shared_ptr<Engine> engine)
  {
    return PlayerController::nextCommand(state, engine);
  }

  void HumanController::notify(const EngineResult& result)
  {
    (void)result;
  }
}
