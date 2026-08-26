#include "core/commands/Command.hpp"
#include "core/commands/IntCommand.hpp"
#include "core/commands/LoopedCommand.hpp"
#include "game/gta/Natives.hpp"

#include <algorithm>

namespace YimMenu::Features
{
	class SanitizedIntCommand : public IntCommand
	{
	public:
		using IntCommand::IntCommand;

	protected:
		void OnChange() override
		{
			if (m_Min)
				m_State = std::max(m_State, *m_Min);

			if (m_Max)
				m_State = std::min(m_State, *m_Max);
		}
	};

	static SanitizedIntCommand _NetworkTimeHour{"networktimehour", "Hour", "Set hour (0-23)", 0, 23, 12};

	static SanitizedIntCommand _NetworkTimeMinute{"networktimeminute", "Minute", "Set minute (0-59)", 0, 59, 0};

	// Not shown to the user
	static SanitizedIntCommand _NetworkTimeSecond{"networktimesecond", "Second", "Set second (0-59)", 0, 59, 0};

	static void ApplyNetworkTime()
	{
		// Input sanitation
		const int hour = std::clamp(_NetworkTimeHour.GetState(), 0, 23);
		const int minute = std::clamp(_NetworkTimeMinute.GetState(), 0, 59);
		const int second = std::clamp(_NetworkTimeSecond.GetState(), 0, 59);

		if (hour != _NetworkTimeHour.GetState())
			_NetworkTimeHour.SetState(hour);

		if (minute != _NetworkTimeMinute.GetState())
			_NetworkTimeMinute.SetState(minute);

		if (second != _NetworkTimeSecond.GetState())
			_NetworkTimeSecond.SetState(second);

		NETWORK::NETWORK_OVERRIDE_CLOCK_TIME(hour, minute, second);
	}

	static void SetAndApplyNetworkTime(int hour, int minute, int second = 0)
	{
		_NetworkTimeHour.SetState(hour);
		_NetworkTimeMinute.SetState(minute);
		_NetworkTimeSecond.SetState(second);

		ApplyNetworkTime();
	}

	class SetNetworkTime : public Command
	{
		using Command::Command;

		void OnCall() override
		{
			ApplyNetworkTime();
		}
	};

	class SetNetworkTimePreset : public Command
	{
	public:
		SetNetworkTimePreset(const char* name, const char* label, const char* description, int hour, int minute) :
		    Command(name, label, description),
		    m_Hour(hour),
		    m_Minute(minute)
		{
		}

	private:
		void OnCall() override
		{
			SetAndApplyNetworkTime(m_Hour, m_Minute);
		}

		int m_Hour;
		int m_Minute;
	};

	class FreezeNetworkTime : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;

		void OnTick() override
		{
			ApplyNetworkTime();
		}

		void OnDisable() override
		{
			NETWORK::NETWORK_CLEAR_CLOCK_TIME_OVERRIDE();
		}
	};

	static SetNetworkTime _SetTime{"setnetworktime", "Apply Time", "Applies the selected online time."};

	static FreezeNetworkTime _FreezeTime{"freezenetworktime", "Freeze", "Freezes the online clock at the selected time."};

	static SetNetworkTimePreset _SetTimeDawn{"networktimedawn", "Dawn", "Sets the online time to 06:00.", 6, 0};

	static SetNetworkTimePreset _SetTimeNoon{"networktimenoon", "Noon", "Sets the online time to 12:00.", 12, 0};

	static SetNetworkTimePreset _SetTimeSunset{"networktimesunset", "Sunset", "Sets the online time to 19:00.", 19, 0};

	static SetNetworkTimePreset _SetTimeMidnight{"networktimemidnight", "Midnight", "Sets the online time to 00:00.", 0, 0};
}