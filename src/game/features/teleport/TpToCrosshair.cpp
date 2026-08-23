#include "core/backend/ScriptMgr.hpp"
#include "core/commands/Command.hpp"
#include "core/frontend/Notifications.hpp"
#include "game/backend/Self.hpp"
#include "game/backend/TeleportService.hpp"
#include "game/gta/Natives.hpp"

#include <cmath>

namespace YimMenu::Features
{
	static Vector3 RotationToDirection(const Vector3& rotation)
	{
		constexpr float deg_to_rad = 0.017453292519943295769f;

		const float pitch = rotation.x * deg_to_rad;
		const float yaw = rotation.z * deg_to_rad;
		const float cos_pitch = std::cos(pitch);

		return {-std::sin(yaw) * cos_pitch, std::cos(yaw) * cos_pitch, std::sin(pitch)};
	}

	static bool GetRaycastHit(rage::fvector3& hit_position, rage::fvector3& surface_normal)
	{
		constexpr float max_distance = 5000.0f;

		const Vector3 camera_position = CAMERA::GET_GAMEPLAY_CAM_COORD();
		const Vector3 camera_rotation = CAMERA::GET_GAMEPLAY_CAM_ROT(2);
		const Vector3 direction = RotationToDirection(camera_rotation);

		const Vector3 end = {
		    camera_position.x + direction.x * max_distance,
		    camera_position.y + direction.y * max_distance,
		    camera_position.z + direction.z * max_distance};

		constexpr int trace_flags =
		    1 |   // world
		    16 |  // objects
		    64 |  // glass
		    128 | // rivers
		    256;  // foliage

		auto ped = Self::GetPed();

		const int shape_test = SHAPETEST::START_SHAPE_TEST_LOS_PROBE(
		    camera_position.x,
		    camera_position.y,
		    camera_position.z,
		    end.x,
		    end.y,
		    end.z,
		    trace_flags,
		    ped.GetHandle(),
		    7);

		BOOL hit = false;
		Vector3 native_hit_position{};
		Vector3 native_surface_normal{};
		::Entity hit_entity = 0; // We need to use a standard int for GET_SHAPE_TEST_RESULT instead of our YimMenu wrapper.

		int status = 1;

		while (status == 1)
		{
			status = SHAPETEST::GET_SHAPE_TEST_RESULT(
			    shape_test,
			    &hit,
			    &native_hit_position,
			    &native_surface_normal,
			    &hit_entity);

			if (status == 1)
				ScriptMgr::Yield();
		}

		if (status != 2 || !hit)
			return false;

		hit_position = {native_hit_position.x, native_hit_position.y, native_hit_position.z};

		surface_normal = {native_surface_normal.x, native_surface_normal.y, native_surface_normal.z};

		return true;
	}

	class TpToCrosshair : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			auto ped = Self::GetPed();

			if (!ped)
				return;

			rage::fvector3 hit_position{};
			rage::fvector3 surface_normal{};

			if (!GetRaycastHit(hit_position, surface_normal))
			{
				Notifications::Show("Teleport", "No valid surface was found at the crosshair.", NotificationType::Warning);
				return;
			}

			// Offset us slightly away from the hit surface so we don't end up getting stuck in anything.
			const float offset = Self::GetVehicle() ? 3.0f : 1.0f;

			hit_position.x += surface_normal.x * offset;
			hit_position.y += surface_normal.y * offset;
			hit_position.z += surface_normal.z * offset;

			TeleportService::TeleportTo(hit_position);
		}
	};

	static TpToCrosshair _TpToCrosshair{"tptocrosshair", "Teleport to Crosshair", "Teleports you to the surface you are aiming at"};
}
