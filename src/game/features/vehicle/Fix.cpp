#include "core/commands/Command.hpp"
#include "game/backend/Self.hpp"

namespace YimMenu::Features
{
	class RepairVehicle : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			if (Self::GetVehicle())
				Self::GetVehicle().Repair();
		}
	};

	static RepairVehicle _RepairVehicle{"repairvehicle", "Clean & Repair Vehicle", "Clean and repair your current vehicle"};
}