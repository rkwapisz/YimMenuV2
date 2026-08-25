#include "FavoritePersonalVehicles.hpp"

#include "core/frontend/Notifications.hpp"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>

namespace
{
	std::string StripImGuiId(std::string name)
	{
		if (const auto pos = name.find("##"); pos != std::string::npos)
			name.resize(pos);

		return name;
	}
}

namespace YimMenu
{
	Folder FavoritePersonalVehicles::CheckFolder()
	{
		return FileMgr::GetProjectFolder("./saved_json_fav_vehicles/");
	}

	void FavoritePersonalVehicles::EnsureLoaded()
	{
		if (m_Loaded)
			return;

		m_Loaded = true;
		Load();
	}

	const std::vector<FavoritePersonalVehicles::Entry>& FavoritePersonalVehicles::GetFavorites()
	{
		EnsureLoaded();
		return m_Favorites;
	}

	bool FavoritePersonalVehicles::Matches(const Entry& favorite, PersonalVehicles::PersonalVehicle& vehicle)
	{
		// Prefer the internal personal-vehicle ID when it still points to the same model.
		if (favorite.Id == vehicle.GetId() && favorite.Model == vehicle.GetModel())
			return true;

		// Fall back to model + plate in case the internal ID changes between sessions.
		return favorite.Model == vehicle.GetModel() && !favorite.Plate.empty() && favorite.Plate == vehicle.GetPlate();
	}

	bool FavoritePersonalVehicles::IsFavorite(PersonalVehicles::PersonalVehicle& vehicle)
	{
		EnsureLoaded();

		return std::ranges::any_of(m_Favorites, [&vehicle](const Entry& favorite) {
			return Matches(favorite, vehicle);
		});
	}

	void FavoritePersonalVehicles::Toggle(PersonalVehicles::PersonalVehicle& vehicle)
	{
		EnsureLoaded();

		const auto it = std::ranges::find_if(m_Favorites, [&vehicle](const Entry& favorite) {
			return Matches(favorite, vehicle);
		});

		if (it != m_Favorites.end())
		{
			m_Favorites.erase(it);
		}
		else
		{
			m_Favorites.push_back({vehicle.GetId(), vehicle.GetModel(), vehicle.GetPlate(), StripImGuiId(vehicle.GetName()), vehicle.GetGarage()});
		}

		Save();
	}

	void FavoritePersonalVehicles::Remove(const Entry& favorite)
	{
		EnsureLoaded();

		const auto it = std::ranges::find_if(m_Favorites, [&favorite](const Entry& entry) {
			return entry.Id == favorite.Id && entry.Model == favorite.Model && entry.Plate == favorite.Plate;
		});

		if (it == m_Favorites.end())
			return;

		m_Favorites.erase(it);
		Save();
	}

	PersonalVehicles::PersonalVehicle* FavoritePersonalVehicles::Resolve(const Entry& favorite)
	{
		for (auto& [label, vehicle] : PersonalVehicles::GetPersonalVehicles())
		{
			if (vehicle && Matches(favorite, *vehicle))
				return vehicle.get();
		}

		return nullptr;
	}

	void FavoritePersonalVehicles::Load()
	{
		m_Favorites.clear();

		try
		{
			const auto path = CheckFolder().GetFile("favorites.json").Path();

			if (!std::filesystem::exists(path))
				return;

			std::ifstream stream(path);
			if (!stream.is_open())
				return;

			nlohmann::json json;
			stream >> json;

			if (!json.contains("favorites") || !json["favorites"].is_array())
				return;

			for (const auto& item : json["favorites"])
			{
				Entry favorite;
				favorite.Id = item.value("id", -1);
				favorite.Model = static_cast<joaat_t>(item.value("model", 0u));
				favorite.Plate = item.value("plate", std::string{});
				favorite.Name = item.value("name", std::string{});
				favorite.Garage = item.value("garage", std::string{});

				if (favorite.Model == 0)
					continue;

				m_Favorites.push_back(std::move(favorite));
			}
		}
		catch (const std::exception&)
		{
			m_Favorites.clear();
			Notifications::Show("Favorite Vehicles", "Failed to load saved favorite vehicles.", NotificationType::Warning);
		}
	}

	void FavoritePersonalVehicles::Save()
	{
		try
		{
			nlohmann::json json;
			json["favorites"] = nlohmann::json::array();

			for (const auto& favorite : m_Favorites)
			{
				json["favorites"].push_back(
				    {{"id", favorite.Id}, {"model", favorite.Model}, {"plate", favorite.Plate}, {"name", favorite.Name}, {"garage", favorite.Garage}});
			}

			const auto file = CheckFolder().GetFile("favorites.json");
			std::ofstream stream(file.Path(), std::ios::out | std::ios::trunc);

			if (!stream.is_open())
			{
				Notifications::Show("Favorite Vehicles", "Failed to save favorite vehicles.", NotificationType::Warning);
				return;
			}

			stream << json.dump(4);
		}
		catch (const std::exception&)
		{
			Notifications::Show("Favorite Vehicles", "Failed to save favorite vehicles.", NotificationType::Warning);
		}
	}
}
