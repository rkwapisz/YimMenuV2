#include "core/commands/LoopedCommand.hpp"

#include "game/backend/Self.hpp"
#include "game/gta/Natives.hpp"
#include "types/pad/ControllerInputs.hpp"

namespace YimMenu::Features
{
	class InstantBrake : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;

		static constexpr float min_speed = 0.5f;

		virtual void OnTick() override
		{
			auto veh = Self::GetVehicle();

			if (!veh)
				return;

			const auto vehicle = veh.GetHandle();
			const auto speed = ENTITY::GET_ENTITY_SPEED_VECTOR(vehicle, true);

			// Moving forward: brake input stops immediately.
			if (speed.y > min_speed
			    && PAD::IS_CONTROL_PRESSED(0, (int)ControllerInputs::INPUT_VEH_BRAKE))
			{
				VEHICLE::SET_VEHICLE_FORWARD_SPEED(vehicle, 0.0f);
			}

			// Moving backward: accelerator acts as the brake.
			else if (speed.y < -min_speed
			    && PAD::IS_CONTROL_PRESSED(0, (int)ControllerInputs::INPUT_VEH_ACCELERATE))
			{
				VEHICLE::SET_VEHICLE_FORWARD_SPEED(vehicle, 0.0f);
			}
		}
	};

	static InstantBrake _InstantBrake{"instantbrake", "Instant Brake", "Immediately stops your vehicle when braking"};
}