#include "core/commands/LoopedCommand.hpp"

#include "game/backend/Self.hpp"
#include "game/gta/Natives.hpp"

namespace YimMenu::Features
{
	class InfiniteVehicleAmmo : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;

		static void SetVehicleAmmo(YimMenu::Vehicle vehicle, int ammo)
		{
			const auto handle = vehicle.GetHandle();

			if (!VEHICLE::DOES_VEHICLE_HAVE_WEAPONS(handle))
				return;

			for (int i = 0; i < 3; i++)
				VEHICLE::SET_VEHICLE_WEAPON_RESTRICTED_AMMO(handle, i, ammo);

			VEHICLE::SET_VEHICLE_BOMB_AMMO(handle, ammo);
			VEHICLE::SET_VEHICLE_COUNTERMEASURE_AMMO(handle, ammo);
		}

		virtual void OnTick() override
		{
			auto vehicle = Self::GetVehicle();

			if (!vehicle)
				return;

			SetVehicleAmmo(vehicle, -1);
		}

		virtual void OnDisable() override
		{
			auto vehicle = Self::GetVehicle();

			if (!vehicle)
				return;

			SetVehicleAmmo(vehicle, 20);
		}
	};

	static InfiniteVehicleAmmo _InfiniteVehicleAmmo{"infinitevehicleammo", "Infinite Ammo", "Provides unlimited ammo for vehicle weapons, bombs, and countermeasures"};
}