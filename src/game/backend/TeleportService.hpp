#pragma once

#include "game/backend/Self.hpp"

#include <mutex>
#include <optional>
#include <vector>

namespace YimMenu
{
	class TeleportService
	{
	public:
		struct TeleportLocation
		{
			rage::fvector3 Position;
			float Heading;
		};

	private:
		static inline std::vector<TeleportLocation> m_History{};
		static inline std::mutex m_HistoryMutex{}; // Don't forget we're now accessing the history from two threads (rendering and script)

		static constexpr std::size_t MAX_HISTORY = 20; // Resize if necessary but I think 20 is way more than enough

	public:
		static void RecordCurrentLocation()
		{
			auto ped = Self::GetPed();

			if (!ped)
				return;

			TeleportLocation location{
			    ped.GetPosition(),
			    ped.GetHeading()};

			if (auto vehicle = Self::GetVehicle())
			{
				location.Position = vehicle.GetPosition();
				location.Heading = vehicle.GetHeading();
			}

			std::scoped_lock lock(m_HistoryMutex);

			if (!m_History.empty()) // Sanity check for duplicates
			{
				const auto& last = m_History.back();

				const float dx = last.Position.x - location.Position.x;
				const float dy = last.Position.y - location.Position.y;
				const float dz = last.Position.z - location.Position.z;

				if ((dx * dx + dy * dy + dz * dz) < 0.01f)
				{
					m_History.back().Heading = location.Heading;
					return;
				}
			}

			if (m_History.size() >= MAX_HISTORY)
				m_History.erase(m_History.begin());

			m_History.push_back(location);
		}

		static bool TeleportTo(
		    const rage::fvector3& position,
		    std::optional<float> heading = std::nullopt,
		    bool record_history = true)
		{
			auto ped = Self::GetPed();

			if (!ped)
				return false;

			if (record_history)
				RecordCurrentLocation();

			ped.TeleportTo(position);

			if (heading.has_value())
			{
				if (auto vehicle = Self::GetVehicle())
					vehicle.SetHeading(*heading);
				else
					ped.SetHeading(*heading);
			}

			return true;
		}

		static bool TeleportBack()
		{
			auto ped = Self::GetPed();

			if (!ped)
				return false;

			TeleportLocation location{};

			{
				std::scoped_lock lock(m_HistoryMutex);

				if (m_History.empty())
					return false;

				location = m_History.back();
				m_History.pop_back();
			}

			// Don't record history teleports as a new quick teleport.
			ped.TeleportTo(location.Position);

			if (auto vehicle = Self::GetVehicle())
				vehicle.SetHeading(location.Heading);
			else
				ped.SetHeading(location.Heading);

			return true;
		}

		static std::vector<TeleportLocation> GetHistory()
		{
			std::scoped_lock lock(m_HistoryMutex);
			return m_History;
		}

		static void ClearHistory()
		{
			std::scoped_lock lock(m_HistoryMutex);
			m_History.clear();
		}

		static std::size_t GetHistorySize()
		{
			std::scoped_lock lock(m_HistoryMutex);
			return m_History.size();
		}
	};
}
