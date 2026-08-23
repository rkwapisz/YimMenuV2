#include "core/commands/Command.hpp"
#include "core/frontend/Notifications.hpp"
#include "game/backend/Self.hpp"
#include "game/backend/TeleportService.hpp"

namespace YimMenu::Features
{
	class TpToLastVehicle : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			auto ped = Self::GetPed();

			if (!ped)
				return;

			auto vehicle = ped.GetLastVehicle();

			if (!vehicle)
			{
				Notifications::Show("Teleport", "No valid last vehicle was found.", NotificationType::Warning);
				return;
			}

			TeleportService::RecordCurrentLocation();

			ped.SetInVehicle(vehicle, -1);
		}
	};

	static TpToLastVehicle _TpToLastVehicle{"tptolastvehicle", "Teleport to Last Vehicle", "Teleports you into the last vehicle you used"};
}