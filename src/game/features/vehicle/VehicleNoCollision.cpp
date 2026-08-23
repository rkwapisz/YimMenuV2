#include "core/commands/LoopedCommand.hpp"

#include "game/backend/Self.hpp"
#include "game/gta/Natives.hpp"

namespace YimMenu::Features
{
	class VehicleNoCollision : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;

		int m_LastVehicle = 0;

		void RestoreVehicle()
		{
			if (m_LastVehicle && ENTITY::DOES_ENTITY_EXIST(m_LastVehicle))
				ENTITY::SET_ENTITY_COMPLETELY_DISABLE_COLLISION(m_LastVehicle, true, true);

			m_LastVehicle = 0;
		}

		virtual void OnTick() override
		{
			auto veh = Self::GetVehicle();

			if (!veh)
			{
				RestoreVehicle();
				return;
			}

			const auto vehicle = veh.GetHandle();

			if (m_LastVehicle != vehicle)
			{
				RestoreVehicle();
				m_LastVehicle = vehicle;
			}

			ENTITY::SET_ENTITY_COMPLETELY_DISABLE_COLLISION(vehicle, false, true);
		}

		virtual void OnDisable() override
		{
			RestoreVehicle();
		}
	};

	static VehicleNoCollision _VehicleNoCollision{"vehiclenocollision", "No Collision", "Disables collision for your current vehicle. Use with care!"};
}