#include "Action.h"

namespace Spiceholic
{
	StringView GetActionName(Action action)
	{
		switch (action)
		{
		case Action::MoveUp: return U"MoveUp"_sv;
		case Action::MoveDown: return U"MoveDown"_sv;
		case Action::MoveLeft: return U"MoveLeft"_sv;
		case Action::MoveRight: return U"MoveRight"_sv;
		case Action::Attack: return U"Attack / Decide"_sv;
		case Action::Cancel: return U"Cancel"_sv;
		case Action::Pause: return U"Pause"_sv;
		}

		return U""_sv;
	}

	bool IsMoveAction(Action action)
	{
		return
			(action == Action::MoveUp) ||
			(action == Action::MoveDown) ||
			(action == Action::MoveLeft) ||
			(action == Action::MoveRight)
			;
	}
}
