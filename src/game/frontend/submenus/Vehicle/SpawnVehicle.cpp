#include "SpawnVehicle.hpp"
#include "core/commands/BoolCommand.hpp"
#include "core/backend/ScriptMgr.hpp"
#include "core/backend/FiberPool.hpp"
#include "core/frontend/Notifications.hpp"
#include "game/frontend/Menu.hpp"
#include "game/backend/Self.hpp"
#include "game/backend/PersonalVehicles.hpp"
#include "game/backend/FavoritePersonalVehicles.hpp"
#include "game/gta/data/Vehicles.hpp"
#include "game/gta/Natives.hpp"

namespace YimMenu::Submenus
{
	static BoolCommand spawnInsideVehicle{"spawninsideveh", "Spawn Inside", "Spawn inside the vehicle."};
	static BoolCommand spawnVehicleMaxed{"spawnvehmaxed", "Spawn Maxed", "Spawn the vehicle maxed."};
	static BoolCommand spawnInsidePersonalVehicle{"spawninsidepv", "Spawn Inside", "Spawn inside the personal vehicle."};
	static BoolCommand spawnClonePersonalVehicle{"spawnclonepv", "Spawn Clone", "Spawn a clone of the personal vehicle."};

	std::shared_ptr<TabItem> RenderSpawnNewVehicle()
	{
		auto tab = std::make_shared<TabItem>("New Vehicle");

		auto spawn = std::make_shared<Group>("Spawn");
		auto settings = std::make_shared<Group>("Settings");

		static std::vector<std::string> vehicleNames{};
		static std::vector<int> vehicleClasses{};
		static int selectedClass{-1};

		spawn->AddItem(std::make_unique<ImGuiItem>([] {
			static bool init = [] {
				FiberPool::Push([] {
					std::unordered_map<std::string, int> nameCount;

					for (auto& veh : g_VehicleHashes)
					{
						std::string gxt = VEHICLE::GET_DISPLAY_NAME_FROM_VEHICLE_MODEL(veh);
						std::string display = HUD::GET_FILENAME_FOR_AUDIO_CONVERSATION(gxt.c_str());

						int& count = nameCount[display == "NULL" ? gxt : display];
						std::string finalName = display == "NULL" ? gxt : display;
						if (count > 0)
						{
							finalName += " " + std::to_string(count + 1);
						}
						++count;

						std::string maker = HUD::GET_FILENAME_FOR_AUDIO_CONVERSATION(VEHICLE::GET_MAKE_NAME_FROM_VEHICLE_MODEL(veh));
						if (maker != "NULL")
						{
							finalName = maker + " " + finalName;
						}

						vehicleNames.push_back(finalName);

						int id = VEHICLE::GET_VEHICLE_CLASS_FROM_NAME(veh);
						vehicleClasses.push_back(id);
					}
				});

				return true;
			}();

			static char search[64];
			ImGui::SetNextItemWidth(300.f);
			ImGui::InputTextWithHint("Name", "Search", search, sizeof(search));

			ImGui::SetNextItemWidth(300.f);
			if (ImGui::BeginCombo("Class", selectedClass == -1 ? "All" : g_VehicleClassNames[selectedClass]))
			{
				if (ImGui::Selectable("All", selectedClass == -1))
				{
					selectedClass = -1;
				}

				for (int i = 0; i < g_VehicleClassNames.size(); i++)
				{
					if (ImGui::Selectable(g_VehicleClassNames[i], selectedClass == i))
					{
						selectedClass = i;
					}
				}

				ImGui::EndCombo();
			}

			const int visible = std::min(20, static_cast<int>(vehicleNames.size()));
			const float height = visible * ImGui::GetTextLineHeightWithSpacing();
			if (ImGui::BeginListBox("##vehicles", {300.f, height}))
			{
				if (vehicleNames.empty())
				{
					ImGui::Text("Natives not cached yet.");
				}
				else
				{
					std::string lower = search;
					std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
					for (int veh = 0; veh < vehicleNames.size(); veh++)
					{
						auto hash = g_VehicleHashes[veh];
						auto name = vehicleNames[veh];
						auto lowerName = name;
						std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);

						bool matchesSearch = lowerName.find(lower) != std::string::npos;
						bool matchesClass = selectedClass == -1 || vehicleClasses[veh] == selectedClass;
						if (matchesSearch && matchesClass)
						{
							ImGui::PushID(hash);
							if (ImGui::Selectable(name.c_str()))
							{
								FiberPool::Push([hash] {
									auto handle = Vehicle::Create(hash, Vehicle::GetSpawnLocRelToPed(Self::GetPed().GetHandle(), hash), Self::GetPed().GetHeading());

									if (spawnInsideVehicle.GetState())
										Self::GetPed().SetInVehicle(handle);

									if (spawnVehicleMaxed.GetState())
										handle.Upgrade();
								});
							}
							ImGui::PopID();
						}
					}
				}

				ImGui::EndListBox();
			}
		}));

		settings->AddItem(std::make_shared<BoolCommandItem>("spawninsideveh"_J));
		settings->AddItem(std::make_shared<BoolCommandItem>("spawnvehmaxed"_J));

		tab->AddItem(spawn);
		tab->AddItem(settings);
		return tab;
	}

	namespace
	{
		constexpr const char* FAVORITE_STAR_ICON = "\xef\x80\x85";

		struct PersonalVehicleRowResult
		{
			bool SpawnClicked{};
			bool FavoriteClicked{};
		};

		PersonalVehicles::PersonalVehicle* FindPersonalVehicleById(int id)
		{
			for (auto& [label, vehicle] : PersonalVehicles::GetPersonalVehicles())
			{
				if (vehicle && vehicle->GetId() == id)
					return vehicle.get();
			}

			return nullptr;
		}

		void SpawnPersonalVehicle(int id)
		{
			FiberPool::Push([id] {
				auto personalVeh = FindPersonalVehicleById(id);

				if (!personalVeh)
				{
					Notifications::Show("Spawn Personal Vehicle", "The selected personal vehicle is no longer available.", NotificationType::Error);
					return;
				}

				if (spawnClonePersonalVehicle.GetState())
				{
					auto coords = Vehicle::GetSpawnLocRelToPed(Self::GetPed().GetHandle(), personalVeh->GetModel());

					auto heading = Self::GetPed().GetHeading();
					auto handle = personalVeh->Clone(coords, heading);

					if (handle && spawnInsidePersonalVehicle.GetState())
						Self::GetPed().SetInVehicle(handle);
				}
				else if (!personalVeh->Request(spawnInsidePersonalVehicle.GetState()))
				{
					Notifications::Show("Spawn Personal Vehicle", "Failed to spawn Personal Vehicle.", NotificationType::Error);
				}
			});
		}

		PersonalVehicleRowResult DrawPersonalVehicleRow(const std::string& label, int rowId, bool favorite, bool enabled = true)
		{
			PersonalVehicleRowResult result{};

			ImGui::PushID(rowId);

			const float starWidth = ImGui::GetFrameHeight();
			const float availableWidth = ImGui::GetContentRegionAvail().x;
			const float selectableWidth = std::max(1.0f, availableWidth - starWidth - ImGui::GetStyle().ItemSpacing.x);

			const ImGuiSelectableFlags flags = enabled ? ImGuiSelectableFlags_None : ImGuiSelectableFlags_Disabled;

			result.SpawnClicked = ImGui::Selectable(label.c_str(), false, flags, ImVec2(selectableWidth, 0.0f));

			const bool rowHovered = ImGui::IsItemHovered();
			const float rowHeight = ImGui::GetItemRectSize().y;

			ImGui::SameLine();

			result.FavoriteClicked = ImGui::InvisibleButton("##favorite", ImVec2(starWidth, rowHeight));

			const bool starHovered = ImGui::IsItemHovered();

			if (favorite || rowHovered || starHovered)
			{
				const ImVec2 min = ImGui::GetItemRectMin();
				const ImVec2 max = ImGui::GetItemRectMax();

				ImFont* iconFont = Menu::Font::g_AwesomeFont;
				const float iconSize = ImGui::GetFontSize();

				const ImVec2 textSize = iconFont->CalcTextSizeA(iconSize, FLT_MAX, 0.0f, FAVORITE_STAR_ICON);

				const ImVec2 textPos{min.x + ((max.x - min.x) - textSize.x) * 0.5f, min.y + ((max.y - min.y) - textSize.y) * 0.5f};

				const ImU32 color = favorite ? IM_COL32(255, 215, 0, 255) : ImGui::GetColorU32(ImGuiCol_TextDisabled);

				ImGui::GetWindowDrawList()->AddText(iconFont, iconSize, textPos, color, FAVORITE_STAR_ICON);
			}

			if (starHovered)
			{
				ImGui::SetTooltip(favorite ? "Remove from favorites" : "Add to favorites");
			}

			ImGui::PopID();
			return result;
		}
	}

	std::shared_ptr<TabItem> RenderSpawnPersonalVehicle()
	{
		auto tab = std::make_shared<TabItem>("Personal Vehicle");

		auto spawn = std::make_shared<Group>("Spawn");
		auto settings = std::make_shared<Group>("Settings");

		spawn->AddItem(std::make_unique<ImGuiItem>([] {
			if (!*Pointers.IsSessionStarted)
				return ImGui::TextDisabled("Join GTA Online.");

			PersonalVehicles::Update();
			FavoritePersonalVehicles::EnsureLoaded();

			static char search[64]{};
			static std::string selectedGarageStr{};
			static int selectedClass{-1};

			ImGui::SetNextItemWidth(300.f);
			ImGui::InputTextWithHint("Name", "Search", search, sizeof(search));

			ImGui::SetNextItemWidth(300.f);
			if (ImGui::BeginCombo("Garage", selectedGarageStr.empty() ? "All" : selectedGarageStr.c_str()))
			{
				if (ImGui::Selectable("All", selectedGarageStr.empty()))
					selectedGarageStr.clear();

				for (const auto& garage : PersonalVehicles::GetGarages())
				{
					if (ImGui::Selectable(garage.c_str(), garage == selectedGarageStr))
						selectedGarageStr = garage;
				}

				ImGui::EndCombo();
			}

			ImGui::SetNextItemWidth(300.f);
			if (ImGui::BeginCombo("Class", selectedClass == -1 ? "All" : g_VehicleClassNames[selectedClass]))
			{
				if (ImGui::Selectable("All", selectedClass == -1))
					selectedClass = -1;

				for (int i = 0; i < g_VehicleClassNames.size(); i++)
				{
					if (ImGui::Selectable(g_VehicleClassNames[i], selectedClass == i))
						selectedClass = i;
				}

				ImGui::EndCombo();
			}

			const int visible = std::min(20, static_cast<int>(PersonalVehicles::GetPersonalVehicles().size()));

			const float height = std::max(ImGui::GetTextLineHeightWithSpacing() * 5.0f, visible * ImGui::GetTextLineHeightWithSpacing());

			ImGui::BeginGroup();
			{
				ImGui::Text("Vehicles");

				if (ImGui::BeginListBox("##personalvehicles", {300.f, height}))
				{
					if (PersonalVehicles::GetPersonalVehicles().empty())
					{
						ImGui::Text("Stats not loaded yet.");
					}
					else
					{
						std::string lowerSearch = search;
						std::transform(lowerSearch.begin(), lowerSearch.end(), lowerSearch.begin(), ::tolower);

						for (const auto& [label, personalVeh] : PersonalVehicles::GetPersonalVehicles())
						{
							auto lowerName = label;
							std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);

							const bool matchesSearch = lowerName.find(lowerSearch) != std::string::npos;

							const bool matchesGarage = selectedGarageStr.empty() || personalVeh->GetGarage() == selectedGarageStr;

							const int vehicleClass = VEHICLE::GET_VEHICLE_CLASS_FROM_NAME(personalVeh->GetModel());

							const bool matchesClass = selectedClass == -1 || vehicleClass == selectedClass;

							if (!matchesSearch || !matchesGarage || !matchesClass)
								continue;

							const bool favorite = FavoritePersonalVehicles::IsFavorite(*personalVeh);
							const auto row = DrawPersonalVehicleRow(label, personalVeh->GetId(), favorite);

							if (row.FavoriteClicked)
								FavoritePersonalVehicles::Toggle(*personalVeh);
							else if (row.SpawnClicked)
								SpawnPersonalVehicle(personalVeh->GetId());
						}
					}

					ImGui::EndListBox();
				}
			}
			ImGui::EndGroup();

			ImGui::SameLine();

			ImGui::BeginGroup();
			{
				ImGui::Text("Favorites");

				if (ImGui::BeginListBox("##favoritepersonalvehicles", {300.f, height}))
				{
					const auto& favorites = FavoritePersonalVehicles::GetFavorites();
					std::optional<FavoritePersonalVehicles::Entry> favoriteToRemove;

					if (favorites.empty())
					{
						ImGui::TextDisabled("No favorites.");
					}
					else
					{
						for (int i = 0; i < static_cast<int>(favorites.size()); i++)
						{
							const auto& favorite = favorites[i];
							auto personalVeh = FavoritePersonalVehicles::Resolve(favorite);

							std::string label;
							bool available = personalVeh != nullptr;

							if (personalVeh)
								label = personalVeh->GetName();
							else if (!favorite.Name.empty())
								label = favorite.Name;
							else if (!favorite.Plate.empty())
								label = "Unavailable Vehicle (" + favorite.Plate + ")";
							else
								label = "Unavailable Vehicle";

							const auto row = DrawPersonalVehicleRow(label, i, true, available);

							if (row.FavoriteClicked)
							{
								favoriteToRemove = favorite;
								break;
							}

							if (row.SpawnClicked && personalVeh)
								SpawnPersonalVehicle(personalVeh->GetId());
						}
					}

					ImGui::EndListBox();

					if (favoriteToRemove)
						FavoritePersonalVehicles::Remove(*favoriteToRemove);
				}
			}
			ImGui::EndGroup();
		}));

		settings->AddItem(std::make_shared<BoolCommandItem>("spawninsidepv"_J));
		settings->AddItem(std::make_shared<BoolCommandItem>("spawnclonepv"_J));

		tab->AddItem(spawn);
		tab->AddItem(settings);

		return tab;
	}

	std::shared_ptr<Category> BuildSpawnVehicleMenu()
	{
		auto menu = std::make_shared<Category>("Spawn");

		auto tabBar = std::make_shared<TabBarItem>("Spawn");

		tabBar->AddItem(RenderSpawnNewVehicle());
		tabBar->AddItem(RenderSpawnPersonalVehicle());

		menu->AddItem(std::move(tabBar));

		return menu;
	}
}