#include "Dispatch.h"

namespace Spiceholic
{
	namespace GlobalDispatch
	{
		namespace
		{
			Optional<Dispatch> gDispatch;
		}

		void Init()
		{
			gDispatch = Dispatch{};
		}
	}

	Dispatch& GetDispatch()
	{
		return *GlobalDispatch::gDispatch;
	}
}
