#include "core/commands/Command.hpp"
#include "core/frontend/Notifications.hpp"
#include "game/backend/PersonalVehicles.hpp"
#include "game/backend/Self.hpp"
#include "game/backend/TeleportService.hpp"

namespace YimMenu::Features
{
	class TpToPersonalVehicle : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			auto ped = Self::GetPed();

			if (!ped)
				return;

			auto vehicle = PersonalVehicles::GetCurrentHandle();

			if (!vehicle)
			{
				Notifications::Show("Teleport", "No active personal vehicle was found.", NotificationType::Warning);
				return;
			}

			TeleportService::RecordCurrentLocation();

			ped.SetInVehicle(vehicle, -1);
		}
	};

	static TpToPersonalVehicle _TpToPersonalVehicle{"tptopersonalvehicle", "Teleport to Personal Vehicle", "Teleports you into your active personal vehicle"};
}