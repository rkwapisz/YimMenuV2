#include "core/commands/LoopedCommand.hpp"

#include "game/backend/Self.hpp"
#include "game/gta/Natives.hpp"

namespace YimMenu::Features
{
	class AutoFlip : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;

		int m_LastVehicle = 0;
		bool m_IsTipped = false;
		std::chrono::steady_clock::time_point m_TippedSince;

		void Reset()
		{
			m_LastVehicle = 0;
			m_IsTipped = false;
		}

		virtual void OnTick() override
		{
			auto veh = Self::GetVehicle();

			if (!veh)
			{
				Reset();
				return;
			}

			const auto vehicle = veh.GetHandle();

			if (m_LastVehicle != vehicle)
			{
				Reset();
				m_LastVehicle = vehicle;
			}

			const float upright = ENTITY::GET_ENTITY_UPRIGHT_VALUE(vehicle);

			// Don't flip the vehicle while it's airborne.
			const bool tipped =
			    upright < 0.25f && !ENTITY::IS_ENTITY_IN_AIR(vehicle);

			if (!tipped)
			{
				m_IsTipped = false;
				return;
			}

			if (!m_IsTipped)
			{
				m_IsTipped = true;
				m_TippedSince = std::chrono::steady_clock::now();
				return;
			}

			// Require the vehicle to remain tipped for a short time so we
			// don't interfere with normal driving, jumps, or brief rolls.
			if (std::chrono::steady_clock::now() - m_TippedSince >= std::chrono::milliseconds(750))
			{
				VEHICLE::SET_VEHICLE_ON_GROUND_PROPERLY(vehicle, 5.0f);

				m_IsTipped = false;
			}
		}

		virtual void OnDisable() override
		{
			Reset();
		}
	};

	static AutoFlip _AutoFlip{"autoflip", "Auto Flip", "Automatically puts your vehicle back on its wheels when it tips over"};
}