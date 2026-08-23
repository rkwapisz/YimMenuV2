#include "core/commands/LoopedCommand.hpp"

#include "game/backend/Self.hpp"
#include "game/gta/Natives.hpp"

namespace YimMenu::Features
{
	class InvisibleVehicle : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;

		int m_LastVehicle = 0;

		void RestoreVehicle()
		{
			if (m_LastVehicle && ENTITY::DOES_ENTITY_EXIST(m_LastVehicle))
				ENTITY::SET_ENTITY_VISIBLE(m_LastVehicle, true, false);

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

			ENTITY::SET_ENTITY_VISIBLE(vehicle, false, false);
		}

		virtual void OnDisable() override
		{
			RestoreVehicle();
		}
	};

	static InvisibleVehicle _InvisibleVehicle{"invisiblevehicle", "Invisible Vehicle", "Makes your current vehicle invisible"};
}