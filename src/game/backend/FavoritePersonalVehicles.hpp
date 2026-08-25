#pragma once

#include "core/filemgr/FileMgr.hpp"
#include "game/backend/PersonalVehicles.hpp"

#include <string>
#include <vector>

namespace YimMenu
{
	class FavoritePersonalVehicles
	{
	public:
		struct Entry
		{
			int Id{-1};
			joaat_t Model{};
			std::string Plate{};
			std::string Name{};
			std::string Garage{};
		};

		static void EnsureLoaded();

		static const std::vector<Entry>& GetFavorites();

		static bool IsFavorite(PersonalVehicles::PersonalVehicle& vehicle);
		static void Toggle(PersonalVehicles::PersonalVehicle& vehicle);
		static void Remove(const Entry& favorite);

		static PersonalVehicles::PersonalVehicle* Resolve(const Entry& favorite);

	private:
		static Folder CheckFolder();
		static void Load();
		static void Save();

		static bool Matches(const Entry& favorite, PersonalVehicles::PersonalVehicle& vehicle);

		static inline bool m_Loaded = false;
		static inline std::vector<Entry> m_Favorites{};
	};
}
