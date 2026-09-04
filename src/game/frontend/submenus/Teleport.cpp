#include "Teleport.hpp"

#include "core/frontend/Notifications.hpp"
#include "core/backend/FiberPool.hpp"
#include "game/backend/SavedLocations.hpp"
#include "game/backend/Self.hpp"
#include "game/frontend/items/Items.hpp"
#include "game/backend/TeleportService.hpp"

namespace YimMenu::Submenus
{
	namespace
	{
		constexpr auto ALL_LOCATIONS = "All Locations";

		enum class SavedLocationSort
		{
			NAME,
			DISTANCE,
			RECENTLY_USED
		};

		struct LocationListEntry
		{
			std::string Category;
			SavedLocation Location;
			float Distance;
			std::uint64_t LastUsed;
		};

		struct SavedTeleportUiState
		{
			std::string SaveCategory = "Default";
			std::string NewLocationName;
			std::string SelectedCategory = ALL_LOCATIONS;
			std::string SelectedLocationCategory;
			std::string SelectedLocationName;
			std::string Search;
			SavedLocationSort Sort = SavedLocationSort::NAME;

			std::string CategoryNameInput;
			std::string RenameLocationInput;
			std::string MoveLocationCategoryInput;

			std::string PendingCategory;
			std::string PendingLocationCategory;
			std::string PendingLocationName;

			std::unordered_map<std::string, std::uint64_t> LastUsed;
			std::uint64_t UsageCounter = 0;
		};

		SavedTeleportUiState& GetSavedTeleportUiState()
		{
			static SavedTeleportUiState state;
			return state;
		}

		std::string Trimmed(std::string value)
		{
			const auto isNotSpace = [](unsigned char character) {
				return !std::isspace(character);
			};

			value.erase(value.begin(), std::find_if(value.begin(), value.end(), isNotSpace));
			value.erase(std::find_if(value.rbegin(), value.rend(), isNotSpace).base(), value.end());
			return value;
		}

		bool IsValidSavedTeleportName(const std::string& value)
		{
			if (value.empty() || value.find("##") != std::string::npos)
				return false;

			return std::ranges::none_of(value, [](unsigned char character) {
				return std::iscntrl(character);
			});
		}

		std::string Lowercase(std::string value)
		{
			std::ranges::transform(value, value.begin(), [](unsigned char character) {
				return static_cast<char>(std::tolower(character));
			});
			return value;
		}

		std::string LocationKey(const std::string& category, const std::string& name)
		{
			return category + '\x1F' + name;
		}

		std::optional<SavedLocation> FindSavedLocation(const std::string& category, const std::string& name)
		{
			const auto& allLocations = SavedLocations::GetAllSavedLocations();
			const auto categoryIt = allLocations.find(category);
			if (categoryIt == allLocations.end())
				return std::nullopt;

			const auto locationIt = std::ranges::find(categoryIt->second, name, &SavedLocation::name);
			if (locationIt == categoryIt->second.end())
				return std::nullopt;

			return *locationIt;
		}

		bool SavedLocationExists(const std::string& category, const std::string& name)
		{
			return FindSavedLocation(category, name).has_value();
		}

		SavedLocation CaptureCurrentLocation(const std::string& name)
		{
			Entity teleportEntity = Self::GetPed();
			if (auto vehicle = Self::GetVehicle())
				teleportEntity = vehicle;

			const auto coords = teleportEntity.GetPosition();

			SavedLocation location;
			location.name = name;
			location.x = coords.x;
			location.y = coords.y;
			location.z = coords.z;
			location.yaw = teleportEntity.GetHeading();
			location.pitch = 0.0f;
			location.roll = 0.0f;
			return location;
		}

		void SaveCurrentLocation(const std::string& category, const std::string& name, bool overwrite)
		{
			FiberPool::Push([category, name, overwrite] {
				const auto existingLocation = FindSavedLocation(category, name);
				if (overwrite && existingLocation)
				{
					if (!SavedLocations::DeleteSavedLocation(category, name))
					{
						Notifications::Show("Saved Teleports", "Could not replace the existing location.", NotificationType::Error);
						return;
					}
				}
				else if (existingLocation)
				{
					Notifications::Show("Saved Teleports", "A location with that name already exists in this category.", NotificationType::Warning);
					return;
				}

				if (SavedLocations::SaveNewLocation(category, CaptureCurrentLocation(name)))
					Notifications::Show("Saved Teleports", std::format("Saved {} in {}.", name, category));
				else
				{
					if (existingLocation)
						SavedLocations::SaveNewLocation(category, *existingLocation);
					Notifications::Show("Saved Teleports", "Could not write saved_locations.json.", NotificationType::Error);
				}
			});
		}

		void TeleportToSavedLocation(const std::string& category, const SavedLocation& location)
		{
			auto& state = GetSavedTeleportUiState();
			state.LastUsed[LocationKey(category, location.name)] = ++state.UsageCounter;

			FiberPool::Push([location] {
				TeleportService::TeleportTo({location.x, location.y, location.z}, location.yaw, false);
			});
		}

		bool RenameOrMoveLocation(const std::string& oldCategory, const std::string& oldName, const std::string& newCategory, const std::string& newName)
		{
			if (oldCategory == newCategory && oldName == newName)
				return true;

			auto location = FindSavedLocation(oldCategory, oldName);
			if (!location || SavedLocationExists(newCategory, newName))
				return false;

			location->name = newName;
			if (!SavedLocations::SaveNewLocation(newCategory, *location))
				return false;

			if (!SavedLocations::DeleteSavedLocation(oldCategory, oldName))
			{
				SavedLocations::DeleteSavedLocation(newCategory, newName);
				return false;
			}

			return true;
		}

		bool RenameCategory(const std::string& oldCategory, const std::string& newCategory)
		{
			if (oldCategory == newCategory)
				return true;

			const auto& allLocations = SavedLocations::GetAllSavedLocations();
			const auto oldCategoryIt = allLocations.find(oldCategory);
			if (oldCategoryIt == allLocations.end() || allLocations.contains(newCategory))
				return false;

			const auto locations = oldCategoryIt->second;
			std::vector<std::string> copiedNames;
			copiedNames.reserve(locations.size());

			for (const auto& location : locations)
			{
				if (!SavedLocations::SaveNewLocation(newCategory, location))
				{
					for (const auto& copiedName : copiedNames)
						SavedLocations::DeleteSavedLocation(newCategory, copiedName);
					return false;
				}
				copiedNames.push_back(location.name);
			}

			for (const auto& location : locations)
			{
				if (!SavedLocations::DeleteSavedLocation(oldCategory, location.name))
					return false;
			}

			return true;
		}

		bool DeleteCategory(const std::string& category)
		{
			const auto& allLocations = SavedLocations::GetAllSavedLocations();
			const auto categoryIt = allLocations.find(category);
			if (categoryIt == allLocations.end())
				return false;

			const auto locations = categoryIt->second;
			for (const auto& location : locations)
			{
				if (!SavedLocations::DeleteSavedLocation(category, location.name))
					return false;
			}

			return true;
		}

		std::string FormatDistance(float distance)
		{
			if (distance >= 1000.0f)
				return std::format("{:.1f} km", distance / 1000.0f);
			return std::format("{:.0f} m", distance);
		}
	}

	static float GetDistanceFromLocation(const SavedLocation& t)
	{
		return rage::fvector3(t.x, t.y, t.z).GetDistance(Self::GetPed().GetPosition());
	}

	void RenderCustomTeleport()
	{
		auto& state = GetSavedTeleportUiState();
		const auto& allLocations = SavedLocations::GetAllSavedLocations();
		const char* popupToOpen = nullptr;

		if (state.SelectedCategory != ALL_LOCATIONS && !allLocations.contains(state.SelectedCategory))
			state.SelectedCategory = ALL_LOCATIONS;

		ImGui::TextUnformatted("Save Current Position");
		if (ImGui::BeginTable("##save_location_layout", 3, ImGuiTableFlags_SizingStretchProp))
		{
			ImGui::TableSetupColumn("##save_category_column", ImGuiTableColumnFlags_WidthStretch, 0.35f);
			ImGui::TableSetupColumn("##save_name_column", ImGuiTableColumnFlags_WidthStretch, 0.40f);
			ImGui::TableSetupColumn("##save_button_column", ImGuiTableColumnFlags_WidthStretch, 0.25f);
			ImGui::TableNextRow();

			ImGui::TableSetColumnIndex(0);
			ImGui::SetNextItemWidth(-1.0f);
			InputTextWithHint("##save_location_category", "Category", &state.SaveCategory).Draw();

			ImGui::TableSetColumnIndex(1);
			ImGui::SetNextItemWidth(-1.0f);
			InputTextWithHint("##save_location_name", "Location name", &state.NewLocationName).Draw();

			ImGui::TableSetColumnIndex(2);
			if (ImGui::Button("Save Location", {-1.0f, 0.0f}))
			{
				state.SaveCategory = Trimmed(state.SaveCategory);
				state.NewLocationName = Trimmed(state.NewLocationName);

				if (!IsValidSavedTeleportName(state.SaveCategory) || !IsValidSavedTeleportName(state.NewLocationName))
				{
					Notifications::Show("Saved Teleports", "Enter a category and location name. The sequence ## and control characters are not allowed.", NotificationType::Warning);
				}
				else if (SavedLocationExists(state.SaveCategory, state.NewLocationName))
				{
					state.PendingLocationCategory = state.SaveCategory;
					state.PendingLocationName = state.NewLocationName;
					popupToOpen = "Replace Saved Location";
				}
				else
				{
					SaveCurrentLocation(state.SaveCategory, state.NewLocationName, false);
					state.SelectedCategory = state.SaveCategory;
					state.SelectedLocationCategory = state.SaveCategory;
					state.SelectedLocationName = state.NewLocationName;
					state.NewLocationName.clear();
				}
			}

			ImGui::EndTable();
		}

		ImGui::Separator();
		ImGui::TextUnformatted("Saved Locations");

		if (ImGui::BeginTable("##saved_location_toolbar", 2, ImGuiTableFlags_SizingStretchProp))
		{
			ImGui::TableSetupColumn("##search_column", ImGuiTableColumnFlags_WidthStretch, 0.72f);
			ImGui::TableSetupColumn("##sort_column", ImGuiTableColumnFlags_WidthStretch, 0.28f);
			ImGui::TableNextRow();

			ImGui::TableSetColumnIndex(0);
			ImGui::SetNextItemWidth(-1.0f);
			InputTextWithHint("##saved_location_search", "Search this category", &state.Search).Draw();

			ImGui::TableSetColumnIndex(1);
			const char* sortLabel = state.Sort == SavedLocationSort::NAME ? "Name" : state.Sort == SavedLocationSort::DISTANCE ? "Distance" : "Recently Used";
			ImGui::SetNextItemWidth(-1.0f);
			if (ImGui::BeginCombo("##saved_location_sort", sortLabel))
			{
				if (ImGui::Selectable("Name", state.Sort == SavedLocationSort::NAME))
					state.Sort = SavedLocationSort::NAME;
				if (ImGui::Selectable("Distance", state.Sort == SavedLocationSort::DISTANCE))
					state.Sort = SavedLocationSort::DISTANCE;
				if (ImGui::Selectable("Recently Used", state.Sort == SavedLocationSort::RECENTLY_USED))
					state.Sort = SavedLocationSort::RECENTLY_USED;
				ImGui::EndCombo();
			}

			ImGui::EndTable();
		}

		std::vector<LocationListEntry> visibleLocations;
		const auto lowercaseSearch = Lowercase(state.Search);
		for (const auto& [category, locations] : allLocations)
		{
			if (state.SelectedCategory != ALL_LOCATIONS && category != state.SelectedCategory)
				continue;

			for (const auto& location : locations)
			{
				if (!lowercaseSearch.empty() && Lowercase(location.name).find(lowercaseSearch) == std::string::npos)
					continue;

				const auto usageIt = state.LastUsed.find(LocationKey(category, location.name));
				visibleLocations.push_back({category, location, GetDistanceFromLocation(location), usageIt == state.LastUsed.end() ? 0 : usageIt->second});
			}
		}

		std::ranges::sort(visibleLocations, [&state](const LocationListEntry& left, const LocationListEntry& right) {
			if (state.Sort == SavedLocationSort::DISTANCE && left.Distance != right.Distance)
				return left.Distance < right.Distance;
			if (state.Sort == SavedLocationSort::RECENTLY_USED && left.LastUsed != right.LastUsed)
				return left.LastUsed > right.LastUsed;

			const auto leftName = Lowercase(left.Location.name);
			const auto rightName = Lowercase(right.Location.name);
			if (leftName != rightName)
				return leftName < rightName;
			return Lowercase(left.Category) < Lowercase(right.Category);
		});

		if (ImGui::BeginTable("##saved_location_browser", 2, ImGuiTableFlags_SizingStretchProp | ImGuiTableFlags_BordersInnerV))
		{
			ImGui::TableSetupColumn("Categories", ImGuiTableColumnFlags_WidthStretch, 0.28f);
			ImGui::TableSetupColumn("Locations", ImGuiTableColumnFlags_WidthStretch, 0.72f);
			ImGui::TableHeadersRow();
			ImGui::TableNextRow();

			ImGui::TableSetColumnIndex(0);
			if (ImGui::BeginChild("##saved_location_categories", {0.0f, 300.0f}, false))
			{
				std::size_t totalLocationCount = 0;
				for (const auto& locations : allLocations | std::views::values)
					totalLocationCount += locations.size();

				const auto allLabel = std::format("{} ({})", ALL_LOCATIONS, totalLocationCount);
				if (ImGui::Selectable(allLabel.c_str(), state.SelectedCategory == ALL_LOCATIONS))
					state.SelectedCategory = ALL_LOCATIONS;

				for (const auto& [category, locations] : allLocations)
				{
					ImGui::PushID(category.c_str());
					const auto label = std::format("{} ({})", category, locations.size());
					if (ImGui::Selectable(label.c_str(), state.SelectedCategory == category))
					{
						state.SelectedCategory = category;
						state.SaveCategory = category;
					}
					ImGui::PopID();
				}
			}
			ImGui::EndChild();

			if (ImGui::Button("New", {-1.0f, 0.0f}))
			{
				state.CategoryNameInput.clear();
				popupToOpen = "New Saved Location Category";
			}

			const bool categorySelected = state.SelectedCategory != ALL_LOCATIONS && allLocations.contains(state.SelectedCategory);
			if (!categorySelected)
				ImGui::BeginDisabled();
			if (ImGui::Button("Rename", {ImGui::GetContentRegionAvail().x * 0.5f, 0.0f}))
			{
				state.PendingCategory = state.SelectedCategory;
				state.CategoryNameInput = state.SelectedCategory;
				popupToOpen = "Rename Saved Location Category";
			}
			ImGui::SameLine();
			if (ImGui::Button("Delete", {-1.0f, 0.0f}))
			{
				state.PendingCategory = state.SelectedCategory;
				popupToOpen = "Delete Saved Location Category";
			}
			if (!categorySelected)
				ImGui::EndDisabled();

			ImGui::TableSetColumnIndex(1);
			if (ImGui::BeginChild("##saved_location_entries", {0.0f, 300.0f}, false))
			{
				if (visibleLocations.empty())
				{
					ImGui::TextDisabled(state.Search.empty() ? "No saved locations in this category." : "No locations match your search.");
				}
				else
				{
					for (const auto& entry : visibleLocations)
					{
						ImGui::PushID(entry.Category.c_str());
						ImGui::PushID(entry.Location.name.c_str());
						const bool isSelected = state.SelectedLocationCategory == entry.Category && state.SelectedLocationName == entry.Location.name;
						const auto rowLabel = state.SelectedCategory == ALL_LOCATIONS ?
						    std::format("{}  [{}]    {}", entry.Location.name, entry.Category, FormatDistance(entry.Distance)) :
						    std::format("{}    {}", entry.Location.name, FormatDistance(entry.Distance));

						if (ImGui::Selectable(rowLabel.c_str(), isSelected, ImGuiSelectableFlags_AllowDoubleClick))
						{
							state.SelectedLocationCategory = entry.Category;
							state.SelectedLocationName = entry.Location.name;
							if (ImGui::IsMouseDoubleClicked(0))
								TeleportToSavedLocation(entry.Category, entry.Location);
						}

						if (ImGui::IsItemHovered())
						{
							ImGui::BeginTooltip();
							ImGui::TextUnformatted(entry.Location.name.c_str());
							ImGui::Text("Category: %s", entry.Category.c_str());
							ImGui::Text("Distance: %s", FormatDistance(entry.Distance).c_str());
							ImGui::Text("X: %.2f  Y: %.2f  Z: %.2f", entry.Location.x, entry.Location.y, entry.Location.z);
							ImGui::EndTooltip();
						}

						ImGui::PopID();
						ImGui::PopID();
					}
				}
			}
			ImGui::EndChild();

			const auto selectedLocation = FindSavedLocation(state.SelectedLocationCategory, state.SelectedLocationName);
			if (!selectedLocation)
				ImGui::BeginDisabled();

			if (ImGui::Button("Teleport", {ImGui::GetContentRegionAvail().x * 0.25f, 0.0f}) && selectedLocation)
				TeleportToSavedLocation(state.SelectedLocationCategory, *selectedLocation);
			ImGui::SameLine();
			if (ImGui::Button("Rename##location", {ImGui::GetContentRegionAvail().x / 3.0f, 0.0f}) && selectedLocation)
			{
				state.PendingLocationCategory = state.SelectedLocationCategory;
				state.PendingLocationName = state.SelectedLocationName;
				state.RenameLocationInput = state.SelectedLocationName;
				popupToOpen = "Rename Saved Location";
			}
			ImGui::SameLine();
			if (ImGui::Button("Move", {ImGui::GetContentRegionAvail().x * 0.5f, 0.0f}) && selectedLocation)
			{
				state.PendingLocationCategory = state.SelectedLocationCategory;
				state.PendingLocationName = state.SelectedLocationName;
				state.MoveLocationCategoryInput = state.SelectedLocationCategory;
				popupToOpen = "Move Saved Location";
			}
			ImGui::SameLine();
			if (ImGui::Button("Delete##location", {-1.0f, 0.0f}) && selectedLocation)
			{
				state.PendingLocationCategory = state.SelectedLocationCategory;
				state.PendingLocationName = state.SelectedLocationName;
				popupToOpen = "Delete Saved Location";
			}

			if (!selectedLocation)
				ImGui::EndDisabled();

			ImGui::EndTable();
		}

		// Open modals outside the table's ID scope so BeginPopupModal resolves the
		// same popup ID below. Opening them from inside either table silently fails.
		if (popupToOpen)
			ImGui::OpenPopup(popupToOpen);

		if (ImGui::BeginPopupModal("Replace Saved Location", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text("Replace '%s' in '%s' with your current position?", state.PendingLocationName.c_str(), state.PendingLocationCategory.c_str());
			if (ImGui::Button("Replace"))
			{
				SaveCurrentLocation(state.PendingLocationCategory, state.PendingLocationName, true);
				state.SelectedCategory = state.PendingLocationCategory;
				state.SelectedLocationCategory = state.PendingLocationCategory;
				state.SelectedLocationName = state.PendingLocationName;
				state.NewLocationName.clear();
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button("Cancel##replace"))
				ImGui::CloseCurrentPopup();
			ImGui::EndPopup();
		}

		if (ImGui::BeginPopupModal("New Saved Location Category", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
		{
			InputTextWithHint("##new_saved_location_category", "Category name", &state.CategoryNameInput).Draw();
			ImGui::TextDisabled("The category will be created when its first location is saved.");
			if (ImGui::Button("Use Category"))
			{
				state.CategoryNameInput = Trimmed(state.CategoryNameInput);
				if (!IsValidSavedTeleportName(state.CategoryNameInput))
					Notifications::Show("Saved Teleports", "Enter a valid category name.", NotificationType::Warning);
				else
				{
					state.SaveCategory = state.CategoryNameInput;
					ImGui::CloseCurrentPopup();
				}
			}
			ImGui::SameLine();
			if (ImGui::Button("Cancel##new_category"))
				ImGui::CloseCurrentPopup();
			ImGui::EndPopup();
		}

		if (ImGui::BeginPopupModal("Rename Saved Location Category", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text("Rename category '%s'", state.PendingCategory.c_str());
			InputTextWithHint("##renamed_saved_location_category", "New category name", &state.CategoryNameInput).Draw();
			if (ImGui::Button("Rename##category_confirm"))
			{
				state.CategoryNameInput = Trimmed(state.CategoryNameInput);
				if (!IsValidSavedTeleportName(state.CategoryNameInput))
					Notifications::Show("Saved Teleports", "Enter a valid category name.", NotificationType::Warning);
				else if (allLocations.contains(state.CategoryNameInput) && state.CategoryNameInput != state.PendingCategory)
					Notifications::Show("Saved Teleports", "That category already exists.", NotificationType::Warning);
				else if (RenameCategory(state.PendingCategory, state.CategoryNameInput))
				{
					if (state.SelectedLocationCategory == state.PendingCategory)
						state.SelectedLocationCategory = state.CategoryNameInput;
					state.SelectedCategory = state.CategoryNameInput;
					state.SaveCategory = state.CategoryNameInput;
					Notifications::Show("Saved Teleports", "Category renamed.");
					ImGui::CloseCurrentPopup();
				}
				else
					Notifications::Show("Saved Teleports", "Could not rename the category.", NotificationType::Error);
			}
			ImGui::SameLine();
			if (ImGui::Button("Cancel##rename_category"))
				ImGui::CloseCurrentPopup();
			ImGui::EndPopup();
		}

		if (ImGui::BeginPopupModal("Delete Saved Location Category", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
		{
			const auto categoryIt = allLocations.find(state.PendingCategory);
			const auto count = categoryIt == allLocations.end() ? 0U : categoryIt->second.size();
			ImGui::Text("Delete '%s' and its %zu saved location(s)?", state.PendingCategory.c_str(), count);
			if (ImGui::Button("Delete Category"))
			{
				if (DeleteCategory(state.PendingCategory))
				{
					if (state.SelectedLocationCategory == state.PendingCategory)
					{
						state.SelectedLocationCategory.clear();
						state.SelectedLocationName.clear();
					}
					state.SelectedCategory = ALL_LOCATIONS;
					Notifications::Show("Saved Teleports", "Category deleted.");
					ImGui::CloseCurrentPopup();
				}
				else
					Notifications::Show("Saved Teleports", "Could not delete the category.", NotificationType::Error);
			}
			ImGui::SameLine();
			if (ImGui::Button("Cancel##delete_category"))
				ImGui::CloseCurrentPopup();
			ImGui::EndPopup();
		}

		if (ImGui::BeginPopupModal("Rename Saved Location", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text("Rename '%s'", state.PendingLocationName.c_str());
			InputTextWithHint("##renamed_saved_location", "New location name", &state.RenameLocationInput).Draw();
			if (ImGui::Button("Rename##location_confirm"))
			{
				state.RenameLocationInput = Trimmed(state.RenameLocationInput);
				if (!IsValidSavedTeleportName(state.RenameLocationInput))
					Notifications::Show("Saved Teleports", "Enter a valid location name.", NotificationType::Warning);
				else if (SavedLocationExists(state.PendingLocationCategory, state.RenameLocationInput) && state.RenameLocationInput != state.PendingLocationName)
					Notifications::Show("Saved Teleports", "That name is already used in this category.", NotificationType::Warning);
				else if (RenameOrMoveLocation(state.PendingLocationCategory, state.PendingLocationName, state.PendingLocationCategory, state.RenameLocationInput))
				{
					state.SelectedLocationName = state.RenameLocationInput;
					Notifications::Show("Saved Teleports", "Location renamed.");
					ImGui::CloseCurrentPopup();
				}
				else
					Notifications::Show("Saved Teleports", "Could not rename the location.", NotificationType::Error);
			}
			ImGui::SameLine();
			if (ImGui::Button("Cancel##rename_location"))
				ImGui::CloseCurrentPopup();
			ImGui::EndPopup();
		}

		if (ImGui::BeginPopupModal("Move Saved Location", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text("Move '%s' to another category", state.PendingLocationName.c_str());
			InputTextWithHint("##move_saved_location_category", "Destination category", &state.MoveLocationCategoryInput).Draw();
			if (ImGui::Button("Move##location_confirm"))
			{
				state.MoveLocationCategoryInput = Trimmed(state.MoveLocationCategoryInput);
				if (!IsValidSavedTeleportName(state.MoveLocationCategoryInput))
					Notifications::Show("Saved Teleports", "Enter a valid destination category.", NotificationType::Warning);
				else if (SavedLocationExists(state.MoveLocationCategoryInput, state.PendingLocationName) && state.MoveLocationCategoryInput != state.PendingLocationCategory)
					Notifications::Show("Saved Teleports", "That category already contains a location with this name.", NotificationType::Warning);
				else if (RenameOrMoveLocation(state.PendingLocationCategory, state.PendingLocationName, state.MoveLocationCategoryInput, state.PendingLocationName))
				{
					state.SelectedCategory = state.MoveLocationCategoryInput;
					state.SelectedLocationCategory = state.MoveLocationCategoryInput;
					state.SaveCategory = state.MoveLocationCategoryInput;
					Notifications::Show("Saved Teleports", "Location moved.");
					ImGui::CloseCurrentPopup();
				}
				else
					Notifications::Show("Saved Teleports", "Could not move the location.", NotificationType::Error);
			}
			ImGui::SameLine();
			if (ImGui::Button("Cancel##move_location"))
				ImGui::CloseCurrentPopup();
			ImGui::EndPopup();
		}

		if (ImGui::BeginPopupModal("Delete Saved Location", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text("Delete '%s' from '%s'?", state.PendingLocationName.c_str(), state.PendingLocationCategory.c_str());
			if (ImGui::Button("Delete Location"))
			{
				if (SavedLocations::DeleteSavedLocation(state.PendingLocationCategory, state.PendingLocationName))
				{
					state.SelectedLocationCategory.clear();
					state.SelectedLocationName.clear();
					Notifications::Show("Saved Teleports", "Location deleted.");
					ImGui::CloseCurrentPopup();
				}
				else
					Notifications::Show("Saved Teleports", "Could not delete the location.", NotificationType::Error);
			}
			ImGui::SameLine();
			if (ImGui::Button("Cancel##delete_location"))
				ImGui::CloseCurrentPopup();
			ImGui::EndPopup();
		}
	}

	void RenderDirectionalTp()
	{
		FloatCommandItem("directionaltpdistance"_J).Draw();

		ImGui::BeginGroup();
		CommandItem("directionaltpforward"_J).Draw();
		CommandItem("directionaltpbackward"_J).Draw();
		ImGui::EndGroup();

		ImGui::SameLine();

		ImGui::BeginGroup();
		CommandItem("directionaltpright"_J).Draw();
		CommandItem("directionaltpleft"_J).Draw();
		ImGui::EndGroup();

		ImGui::SameLine();

		ImGui::BeginGroup();
		CommandItem("directionaltpup"_J).Draw();
		CommandItem("directionaltpdown"_J).Draw();
		ImGui::EndGroup();
	}

	void RenderPreviousLocations()
	{
		const auto history = TeleportService::GetHistory();

		if (history.empty())
		{
			ImGui::TextDisabled("No previous locations.");
			return;
		}

		// Keep the panel compact and scroll when the history grows.
		if (ImGui::BeginChild("##previous_teleport_locations", ImVec2(0.0f, 220.0f), true))
		{
			// History is stored oldest -> newest, so render it in reverse.
			for (std::size_t i = history.size(); i-- > 0;)
			{
				const auto& location = history[i];

				const auto label = std::format("{:.2f}, {:.2f}, {:.2f}##previous_location_{}", location.Position.x, location.Position.y, location.Position.z, i);

				if (ImGui::Selectable(label.c_str()))
				{
					// Navigating history should not itself create another
					// history entry.
					FiberPool::Push([location] {
						TeleportService::TeleportTo(location.Position, location.Heading, false);
					});
				}

				if (ImGui::IsItemHovered())
				{
					ImGui::BeginTooltip();
					ImGui::Text("X: %.3f\nY: %.3f\nZ: %.3f\nHeading: %.1f", location.Position.x, location.Position.y, location.Position.z, location.Heading);
					ImGui::EndTooltip();
				}
			}
		}

		ImGui::EndChild();
	}

	Teleport::Teleport() :
#define ICON_FA_TELEPORT "\xef\x8f\x85"
	    Submenu::Submenu("Teleport", ICON_FA_TELEPORT)
	{
		auto main = std::make_shared<Category>("Main");

		auto quickTeleport = std::make_shared<Group>("Quick Teleport", 8);

		quickTeleport->AddItem(std::make_shared<ConditionalItem>("autotptowaypoint"_J, std::make_shared<CommandItem>("tptowaypoint"_J), true));
		quickTeleport->AddItem(std::make_shared<BoolCommandItem>("autotptowaypoint"_J));
		quickTeleport->AddItem(std::make_shared<CommandItem>("tptoobjective"_J));
		quickTeleport->AddItem(std::make_shared<CommandItem>("tpback"_J));
		quickTeleport->AddItem(std::make_shared<CommandItem>("tptonearestroad"_J));
		quickTeleport->AddItem(std::make_shared<CommandItem>("tptocrosshair"_J));
		quickTeleport->AddItem(std::make_shared<CommandItem>("tptopersonalvehicle"_J));
		quickTeleport->AddItem(std::make_shared<CommandItem>("tptolastvehicle"_J));

		auto previousLocations = std::make_shared<Group>("Previous Locations");
		previousLocations->AddItem(std::make_shared<ImGuiItem>([] {
			RenderPreviousLocations();
		}));

		// Quick Teleport on the left, Previous Locations on the right.
		main->AddItem(std::make_shared<ImGuiItem>([quickTeleport, previousLocations] {
			if (ImGui::BeginTable("##quick_teleport_history_layout", 2, ImGuiTableFlags_SizingStretchProp))
			{
				ImGui::TableSetupColumn("##quick_teleport_column", ImGuiTableColumnFlags_WidthStretch, 0.55f);
				ImGui::TableSetupColumn("##previous_locations_column", ImGuiTableColumnFlags_WidthStretch, 0.45f);

				ImGui::TableNextRow();

				ImGui::TableSetColumnIndex(0);
				quickTeleport->Draw();

				ImGui::TableSetColumnIndex(1);
				previousLocations->Draw();

				ImGui::EndTable();
			}
		}));

		auto coordinates = std::make_shared<Group>("Coordinates");

		coordinates->AddItem(std::make_shared<ImGuiItem>([] {
			ImGui::BeginGroup();

			FloatCommandItem("tpcoordx"_J, "X", false).Draw();
			FloatCommandItem("tpcoordy"_J, "Y", false).Draw();
			FloatCommandItem("tpcoordz"_J, "Z", false).Draw();

			CommandItem("usecurrenttpcoords"_J).Draw();
			CommandItem("tptocoordinates"_J).Draw();

			ImGui::EndGroup();
		}));

		main->AddItem(coordinates);

		auto directional = std::make_shared<Group>("Directional");

		directional->AddItem(std::make_shared<ImGuiItem>([] {
			RenderDirectionalTp();
		}));

		main->AddItem(directional);

		auto customteleport = std::make_shared<Category>("Saved");
		customteleport->AddItem(std::make_shared<ImGuiItem>([] {
			RenderCustomTeleport();
		}));

		AddCategory(std::move(main));
		AddCategory(std::move(customteleport));
	}

}