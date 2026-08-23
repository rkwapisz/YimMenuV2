#include "core/commands/Command.hpp"
#include "core/commands/FloatCommand.hpp"
#include "game/backend/Self.hpp"
#include "game/backend/TeleportService.hpp"

namespace YimMenu::Features
{
	static FloatCommand _TpCoordX{"tpcoordx", "X", "Teleport X coordinate"};

	static FloatCommand _TpCoordY{"tpcoordy", "Y", "Teleport Y coordinate"};

	static FloatCommand _TpCoordZ{"tpcoordz", "Z", "Teleport Z coordinate"};

	class UseCurrentCoordinates : public Command
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

			const auto position = entity.GetPosition();

			_TpCoordX.SetState(position.x);
			_TpCoordY.SetState(position.y);
			_TpCoordZ.SetState(position.z);
		}
	};

	class TpToCoordinates : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			TeleportService::TeleportTo({_TpCoordX.GetState(), _TpCoordY.GetState(), _TpCoordZ.GetState()});
		}
	};

	static UseCurrentCoordinates _UseCurrentCoordinates{"usecurrenttpcoords", "Use Current Coordinates", "Copies your current position into the coordinate fields"};

	static TpToCoordinates _TpToCoordinates{"tptocoordinates", "Teleport to Coordinates", "Teleports you to the specified coordinates"};
}