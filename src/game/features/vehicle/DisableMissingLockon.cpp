#include "core/commands/LoopedCommand.hpp"

#include "game/backend/Self.hpp"
#include "game/gta/Natives.hpp"

namespace YimMenu::Features
{
	class DisableMissileLockOn : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;

		virtual void OnTick() override
		{
			auto vehicle = Self::GetVehicle();

			if (vehicle)
				VEHICLE::SET_VEHICLE_ALLOW_HOMING_MISSLE_LOCKON(vehicle.GetHandle(), false, false);
		}

		virtual void OnDisable() override
		{
			auto vehicle = Self::GetVehicle();

			if (vehicle)
				VEHICLE::SET_VEHICLE_ALLOW_HOMING_MISSLE_LOCKON(vehicle.GetHandle(), true, false);
		}
	};

	static DisableMissileLockOn _DisableMissileLockOn{"disablemissilelockon", "Disable Missile Lock-on", "Prevents homing missiles from locking onto your vehicle"};
}