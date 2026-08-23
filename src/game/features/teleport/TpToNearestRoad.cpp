#include "core/commands/Command.hpp"
#include "core/frontend/Notifications.hpp"
#include "game/backend/Self.hpp"
#include "game/backend/TeleportService.hpp"
#include "game/gta/Natives.hpp"

inline Vector3 ToScriptVector(const rage::fvector3& v)
{
	Vector3 out;
	out.x = v.x;
	out.y = v.y;
	out.z = v.z;
	return out;
}

inline rage::fvector3 FromScriptVector(const Vector3& v)
{
	return { v.x, v.y, v.z };
}

namespace YimMenu::Features
{
	class TpToNearestRoad : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			auto ped = Self::GetPed();

			if (!ped)
				return;

			Entity entity = ped;

			if (auto vehicle = Self::GetVehicle())
				entity = vehicle;

			const auto current_position = entity.GetPosition();

			Vector3 road_position_script{};
			float road_heading = 0.0f;

			const bool found_road = PATH::GET_CLOSEST_VEHICLE_NODE_WITH_HEADING(
			    current_position.x,
			    current_position.y,
			    current_position.z,
			    &road_position_script,
			    &road_heading,
			    0, // main road nodes
			    3.0f,
			    0.0f);

			if (!found_road)
			{
				Vector3 safe_position_script{};

				if (PATH::GET_SAFE_COORD_FOR_PED(
				        current_position.x,
				        current_position.y,
				        current_position.z,
				        true,
				        &safe_position_script,
				        16))
				{
					TeleportService::TeleportTo(FromScriptVector(safe_position_script));
					return;
				}

				Notifications::Show("Teleport", "Unable to find a nearby road or safe location.", NotificationType::Warning);
				return;
			}

			// Keep the entity just above the road and let vehicle grounding
			// resolve the final suspension/wheel position.
			road_position_script.z += 1.0f;

			TeleportService::TeleportTo(FromScriptVector(road_position_script), road_heading);

			if (auto vehicle = Self::GetVehicle())
				vehicle.SetOnGroundProperly();
		}
	};

	static TpToNearestRoad _TpToNearestRoad{"tptonearestroad", "Teleport to Nearest Road", "Teleports you to the nearest suitable road"};
}