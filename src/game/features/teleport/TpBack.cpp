#include "core/commands/Command.hpp"
#include "core/frontend/Notifications.hpp"
#include "game/backend/TeleportService.hpp"

namespace YimMenu::Features
{
	class TpBack : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			if (!TeleportService::TeleportBack())
			{
				Notifications::Show("Teleport", "There is no previous teleport location.", NotificationType::Warning);
			}
		}
	};

	static TpBack _TpBack{"tpback", "Teleport Back", "Returns you to your previous teleport location"};
}