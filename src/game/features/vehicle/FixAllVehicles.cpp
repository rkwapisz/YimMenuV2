#include "core/commands/Command.hpp"
#include "core/frontend/Notifications.hpp"
#include "game/backend/PersonalVehicles.hpp"
#include "game/pointers/Pointers.hpp"

namespace YimMenu::Features
{
	class FixAllVehicles : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			if (!*Pointers.IsSessionStarted)
				return;

			int count = 0;
			for (const auto& it : PersonalVehicles::GetPersonalVehicles())
			{
				const auto& personalVeh = it.second;
				if (personalVeh->RecoverVehicles())
					count++;
			}

			if (count > 0)
				Notifications::Show("Recover All Vehicles", std::format("{} vehicles recovered.", count), NotificationType::Success);
			else
				Notifications::Show("Recover All Vehicles", "No vehicles to recover.");
		}
	};

	static FixAllVehicles _FixAllVehicles{"recoverallvehicles", "Recover All Personal Vehicles", "Recovers all destroyed and impounded personal vehicles."};
}