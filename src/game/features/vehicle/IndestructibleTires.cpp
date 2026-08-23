#include "core/commands/LoopedCommand.hpp"

#include "game/backend/Self.hpp"
#include "game/gta/Natives.hpp"

namespace YimMenu::Features
{
	class IndestructibleTires : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;

		int m_LastVehicle = 0;
		bool m_TyresCouldBurst = true;

		void RestoreVehicle()
		{
			if (m_LastVehicle && ENTITY::DOES_ENTITY_EXIST(m_LastVehicle))
			{
				VEHICLE::SET_VEHICLE_TYRES_CAN_BURST(m_LastVehicle, m_TyresCouldBurst);
				VEHICLE::SET_VEHICLE_WHEELS_CAN_BREAK(m_LastVehicle, true);
			}

			m_LastVehicle = 0;
			m_TyresCouldBurst = true;
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
				m_TyresCouldBurst = VEHICLE::GET_VEHICLE_TYRES_CAN_BURST(vehicle);
			}

			VEHICLE::SET_VEHICLE_TYRES_CAN_BURST(vehicle, false);
			VEHICLE::SET_VEHICLE_WHEELS_CAN_BREAK(vehicle, false);
		}

		virtual void OnDisable() override
		{
			RestoreVehicle();
		}
	};

	static IndestructibleTires _IndestructibleTires{"indestructibletires", "Indestructible Tires", "Prevents your vehicle's tires from bursting and wheels from breaking"};
}