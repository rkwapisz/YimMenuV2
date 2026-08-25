#include "core/commands/LoopedCommand.hpp"
#include "game/backend/Self.hpp"

namespace YimMenu::Features
{
	class KeepRepaired : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;

		virtual void OnTick() override
		{
			auto veh = Self::GetVehicle();
			if (veh)
				veh.Repair();
		}
	};

	static KeepRepaired _KeepRepaired{"keeprepaired", "Keep Vehicle Clean & Repaired", "Keeps your vehicle clean and repaired."};
}