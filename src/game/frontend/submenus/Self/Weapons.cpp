#include "Weapons.hpp"
#include "WeaponEditor.hpp"
#include "core/backend/FiberPool.hpp"
#include "core/backend/ScriptMgr.hpp"
#include "game/backend/Self.hpp"
#include "game/gta/data/Weapons.hpp"
#include "game/gta/Natives.hpp"
#include "game/gta/Scripts.hpp"
#include "game/gta/ScriptFunction.hpp"
#include "types/script/scrThread.hpp"
#include "core/commands/Commands.hpp"
#include "game/features/self/CustomWeapon.hpp"
#include "game/features/self/SavedWeaponLoadouts.hpp"

#include <algorithm>
#include <atomic>

namespace YimMenu::Submenus
{
	static std::vector<WeaponDisplay> g_WeaponDisplayCache;
	static bool g_WeaponDisplayCacheStarted = false;
	static bool g_WeaponDisplayCacheReady = false;
	static WeaponManagementPanel g_OpenWeaponManagementPanel = WeaponManagementPanel::None;

	bool BeginWeaponManagementPanel(WeaponManagementPanel panel, const char* label)
	{
		const bool wasOpen = g_OpenWeaponManagementPanel == panel;

		ImGui::SetNextItemOpen(wasOpen, ImGuiCond_Always);
		const bool isOpen = ImGui::CollapsingHeader(label);

		if (isOpen && !wasOpen)
			g_OpenWeaponManagementPanel = panel;
		else if (!isOpen && wasOpen)
			g_OpenWeaponManagementPanel = WeaponManagementPanel::None;

		return isOpen;
	}

	void EnsureWeaponDisplayCache()
	{
		if (g_WeaponDisplayCacheStarted || g_WeaponDisplayCacheReady)
			return;

		g_WeaponDisplayCacheStarted = true;

		FiberPool::Push([] {
			while (Scripts::IsScriptActive("startup"_J))
				ScriptMgr::Yield();

			uint64_t garbage[4]{};

			if (auto id = Scripts::StartScript("mp_weapons"_J, eStackSizes::PAUSE_MENU_SCRIPT, &garbage, 4))
			{
				if (auto thread = Scripts::FindScriptThreadByID(id))
				{
					thread->m_Context.m_State = rage::scrThread::State::PAUSED;

					static ScriptFunction getWeaponNameLabel("mp_weapons"_J, ScriptPointer("GetWeaponNameLabel", "2D 02 2B 00 00"));

					static ScriptFunction getWeaponDescLabel("mp_weapons"_J, ScriptPointer("GetWeaponDescLabel", "2D 02 A0 00 00"));

					std::vector<WeaponDisplay> weaponDisplays;
					weaponDisplays.reserve(g_WeaponHashes.size());

					for (const auto weaponHash : g_WeaponHashes)
					{
						std::string nameGxt = getWeaponNameLabel.Call<const char*>(weaponHash, false);

						std::string descGxt = getWeaponDescLabel.Call<const char*>(weaponHash, false);

						std::string nameDisplay = HUD::GET_FILENAME_FOR_AUDIO_CONVERSATION(nameGxt.c_str());

						std::string descDisplay = HUD::GET_FILENAME_FOR_AUDIO_CONVERSATION(descGxt.c_str());

						if (nameDisplay.empty() || nameDisplay == "NULL" || nameDisplay == "Invalid")
						{
							nameDisplay.clear();
						}

						if (descDisplay.empty() || descDisplay == "NULL" || descDisplay == "Invalid")
						{
							descDisplay.clear();
						}

						weaponDisplays.push_back({std::move(nameDisplay), std::move(descDisplay), weaponHash});
					}

					thread->Kill();
					thread->m_Context.m_State = rage::scrThread::State::KILLED;

					/*
					 * Publish the completed cache all at once instead
					 * of exposing a partially populated vector.
					 */
					g_WeaponDisplayCache = std::move(weaponDisplays);

					g_WeaponDisplayCacheReady = true;

					return;
				}
			}

			/*
			 * Initialization failed. Allow a future call to retry.
			 */
			g_WeaponDisplayCacheStarted = false;
		});
	}

	const std::vector<WeaponDisplay>& GetWeaponDisplayCache()
	{
		EnsureWeaponDisplayCache();

		return g_WeaponDisplayCache;
	}

	const WeaponDisplay* FindWeaponDisplay(joaat_t weaponHash)
	{
		EnsureWeaponDisplayCache();

		const auto it = std::ranges::find_if(g_WeaponDisplayCache, [weaponHash](const WeaponDisplay& weapon) {
			return weapon.hash == weaponHash;
		});

		if (it == g_WeaponDisplayCache.end())
			return nullptr;

		return &*it;
	}

	std::string GetWeaponDisplayName(joaat_t weaponHash)
	{
		if (const auto weapon = FindWeaponDisplay(weaponHash))
		{
			if (!weapon->name.empty())
				return weapon->name;
		}

		return std::format("0x{:08X}", weaponHash);
	}

	static void FetchWeaponStats(joaat_t weaponHash, int& kills, int& deaths, float& kd, int& headshots, int& accuracy)
	{
		uint64_t garbage[4]{};
		if (auto id = Scripts::StartScript("mp_weapons"_J, eStackSizes::PAUSE_MENU_SCRIPT, &garbage, 4))
		{
			if (auto thread = Scripts::FindScriptThreadByID(id))
			{
				thread->m_Context.m_State = rage::scrThread::State::PAUSED;

				static ScriptFunction getWeaponKills("mp_weapons"_J, ScriptPointer("GetWeaponKills", "5D ? ? ? 39 0F 38 00").Add(1).Rip());
				static ScriptFunction getWeaponDeaths("mp_weapons"_J, ScriptPointer("GetWeaponDeaths", "5D ? ? ? 39 10").Add(1).Rip());
				static ScriptFunction getWeaponKDRatio("mp_weapons"_J, ScriptPointer("GetWeaponKDRatio", "5D ? ? ? 39 12").Add(1).Rip());
				static ScriptFunction getWeaponHeadshots("mp_weapons"_J, ScriptPointer("GetWeaponHeadshots", "5D ? ? ? 39 11").Add(1).Rip());
				static ScriptFunction getWeaponAccuracy("mp_weapons"_J, ScriptPointer("GetWeaponAccuracy", "2D 01 09 00 00"));

				kills = getWeaponKills.Call<int>(weaponHash, -1);
				deaths = getWeaponDeaths.Call<int>(weaponHash, -1);
				kd = getWeaponKDRatio.Call<float>(weaponHash, -1);
				headshots = getWeaponHeadshots.Call<int>(weaponHash, -1);
				accuracy = static_cast<int>(getWeaponAccuracy.Call<float>(weaponHash));

				thread->Kill();
				thread->m_Context.m_State = rage::scrThread::State::KILLED;
			}
		}
	}

	static void RenderWeaponManagerMenu()
	{
		static std::string selectedWeapon{"Select"};
		static joaat_t selectedWeaponHash{};
		static char searchWeapon[64];

		static int kills{};
		static int deaths{};
		static float kdRatio{};
		static int headshots{};
		static int accuracy{};

		EnsureWeaponDisplayCache();

		const auto& weaponDisplays = GetWeaponDisplayCache();

		ImGui::BeginCombo("Weapons", selectedWeapon.c_str());
		if (ImGui::IsItemActive() && !ImGui::IsPopupOpen("##weaponspopup"))
		{
			ImGui::OpenPopup("##weaponspopup");
			memset(searchWeapon, 0, sizeof(searchWeapon));
		}
		if (ImGui::BeginPopup("##weaponspopup", ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove))
		{
			ImGui::Text("Search:");
			ImGui::SameLine();
			ImGui::SetNextItemWidth(250.f);
			ImGui::InputText("##searchweapon", searchWeapon, sizeof(searchWeapon));

			std::string searchLower = searchWeapon;
			std::transform(searchLower.begin(), searchLower.end(), searchLower.begin(), ::tolower);
			for (const auto& weap : weaponDisplays)
			{
				if (weap.name.empty())
					continue;

				std::string weaponLower = weap.name;
				std::transform(weaponLower.begin(), weaponLower.end(), weaponLower.begin(), ::tolower);

				if (weaponLower.find(searchLower) != std::string::npos)
				{
					ImGui::PushID(weap.hash);
					if (ImGui::Selectable(weap.name.c_str()))
					{
						FiberPool::Push([weap] {
							selectedWeapon = weap.name;
							selectedWeaponHash = weap.hash;
							FetchWeaponStats(selectedWeaponHash, kills, deaths, kdRatio, headshots, accuracy);
						});
					}
					ImGui::PopID();
					if (ImGui::IsItemHovered() && !weap.desc.empty())
					{
						ImGui::BeginTooltip();
						ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35);
						ImGui::TextUnformatted(weap.desc.c_str());
						ImGui::PopTextWrapPos();
						ImGui::EndTooltip();
					}
				}
			}
			ImGui::EndPopup();
		}

		if (ImGui::Button("Give Weapon"))
		{
			FiberPool::Push([] {
				Self::GetPed().GiveWeapon(selectedWeaponHash, true);
			});
		}
		ImGui::SameLine();
		if (ImGui::Button("Remove Weapon"))
		{
			FiberPool::Push([] {
				Self::GetPed().RemoveWeapon(selectedWeaponHash);
			});
		}

		if (*Pointers.IsSessionStarted && selectedWeaponHash != 0)
		{
			ImGui::Text("Kills With: %d", kills);
			ImGui::Text("Deaths By: %d", deaths);
			ImGui::Text("K/D Ratio: %.2f", kdRatio);
			ImGui::Text("Headshots: %d", headshots);
			ImGui::Text("Accuracy: %d%%", accuracy);
		}
	}

	static void RenderWeaponLoadoutsMenu()
	{
		static char loadoutName[64]{};
		static std::vector<std::string> loadoutFiles{};
		static std::string selectedLoadout{};
		static std::atomic_bool refreshRequested{true};

		if (refreshRequested.exchange(false))
		{
			SavedWeaponLoadouts::RefreshList(loadoutFiles);

			if (!selectedLoadout.empty() && std::ranges::find(loadoutFiles, selectedLoadout) == loadoutFiles.end())
			{
				selectedLoadout.clear();
			}
		}

		ImGui::SetNextItemWidth(250.0f);
		ImGui::InputTextWithHint("##weaponloadoutname", "Loadout name", loadoutName, sizeof(loadoutName));

		ImGui::SameLine();

		if (ImGui::Button("Save Current"))
		{
			const std::string name = loadoutName;

			FiberPool::Push([name] {
				if (SavedWeaponLoadouts::Save(name))
					refreshRequested = true;
			});
		}

		ImGui::Spacing();
		ImGui::Text("Saved Loadouts");

		if (ImGui::BeginListBox("##weaponloadouts", ImVec2(350.0f, 160.0f)))
		{
			for (const auto& file : loadoutFiles)
			{
				const bool selected = selectedLoadout == file;

				std::string displayName = file;

				if (displayName.ends_with(".json"))
					displayName.resize(displayName.size() - 5);

				if (ImGui::Selectable(displayName.c_str(), selected))
					selectedLoadout = file;

				if (selected)
					ImGui::SetItemDefaultFocus();
			}

			ImGui::EndListBox();
		}

		const bool hasSelection = !selectedLoadout.empty();

		if (!hasSelection)
			ImGui::BeginDisabled();

		if (ImGui::Button("Apply"))
		{
			const auto file = selectedLoadout;

			FiberPool::Push([file] {
				SavedWeaponLoadouts::Load(file);
			});
		}

		ImGui::SameLine();

		if (ImGui::Button("Delete"))
		{
			const auto file = selectedLoadout;

			FiberPool::Push([file] {
				if (SavedWeaponLoadouts::Delete(file))
					refreshRequested = true;
			});
		}

		if (!hasSelection)
			ImGui::EndDisabled();

		ImGui::SameLine();

		if (ImGui::Button("Refresh"))
			refreshRequested = true;

		ImGui::Spacing();
		ImGui::TextDisabled("Applying a loadout replaces all current weapons.");
	}

	static std::shared_ptr<Group> RenderCustomWeaponsMenu()
	{
		static bool customWeaponsExpanded = false;

		auto customWeaponsGroup = std::make_shared<Group>("", 1);
		auto customWeaponsContents = std::make_shared<Group>("");

		auto cutomWeaponTypes = std::make_shared<Group>("", 1);
		auto customWeapons = std::make_shared<Group>("");
		auto paintGunGroup = std::make_shared<Group>("");

		auto cmd = Commands::GetCommand<ListCommand>("customweapontype"_J);

		auto isGravityGunEnabled = [cmd] {
			return static_cast<Features::CustomWeapons>(cmd->GetState()) == Features::CustomWeapons::GRAVITY_GUN;
		};

		auto isVehicleGunEnabled = [cmd] {
			return static_cast<Features::CustomWeapons>(cmd->GetState()) == Features::CustomWeapons::VEHICLE_GUN;
		};

		auto isPaintGunEnabled = [cmd] {
			return static_cast<Features::CustomWeapons>(cmd->GetState()) == Features::CustomWeapons::PAINT_GUN;
		};

		auto isCustomWeaponsExpanded = [] {
			return customWeaponsExpanded;
		};

		cutomWeaponTypes->AddItem(std::make_shared<ListCommandItem>("customweapontype"_J));
		cutomWeaponTypes->AddItem(std::make_shared<ConditionalItem>(isGravityGunEnabled, std::make_shared<BoolCommandItem>("gravitygunlaunchonrelease"_J)));
		cutomWeaponTypes->AddItem(std::make_shared<ConditionalItem>(isVehicleGunEnabled, std::make_shared<StringCommandItem>("vehiclegunmodel"_J)));
		cutomWeaponTypes->AddItem(std::make_shared<ConditionalItem>(isPaintGunEnabled, std::make_shared<ConditionalItem>("paintgunrainbowcolorenabled"_J, std::make_shared<ColorCommandItem>("paintguncolor"_J), true)));

		paintGunGroup->AddItem(std::make_shared<BoolCommandItem>("paintgunrainbowcolorenabled"_J));
		paintGunGroup->AddItem(std::make_shared<ConditionalItem>("paintgunrainbowcolorenabled"_J, std::make_shared<ListCommandItem>("paintgunrainbowcolorstyle"_J)));
		paintGunGroup->AddItem(std::make_shared<ConditionalItem>("paintgunrainbowcolorenabled"_J, std::make_shared<IntCommandItem>("paintgunrainbowcolorspeed"_J)));

		customWeapons->AddItem(std::make_shared<BoolCommandItem>("customweaponenabledonweaponout"_J));
		customWeapons->AddItem(std::move(cutomWeaponTypes));
		customWeapons->AddItem(std::make_shared<ConditionalItem>(isPaintGunEnabled, std::move(paintGunGroup)));

		customWeaponsContents->AddItem(std::make_shared<BoolCommandItem>("customweapon"_J));
		customWeaponsContents->AddItem(std::make_shared<ConditionalItem>("customweapon"_J, std::move(customWeapons)));

		customWeaponsGroup->AddItem(std::make_shared<ImGuiItem>([] {
			customWeaponsExpanded = ImGui::CollapsingHeader("Custom Weapons");
		}));
		customWeaponsGroup->AddItem(std::make_shared<ConditionalItem>(isCustomWeaponsExpanded, std::move(customWeaponsContents)));

		return customWeaponsGroup;
	}

	std::shared_ptr<Category> BuildWeaponsMenu()
	{
		EnsureWeaponDisplayCache(); // Build our weapon name cache so we don't have to open Weapon Manager first

		auto weapons = std::make_shared<Category>("Weapons");

		auto weaponsGlobalsGroup = std::make_shared<Group>("Globals", 4);
		auto weaponsDamageGroup = std::make_shared<Group>("Damage", 4);
		auto weaponsToolsGroup = std::make_shared<Group>("Tools", 2);
		auto weaponsEditorGroup = std::make_shared<Group>("", 1);
		auto weaponsManagementGroup = std::make_shared<Group>("Weapon Management", 1);
		auto weaponsAimbotGroup = std::make_shared<Group>("Aimbot", 1);

		// Globals
		weaponsGlobalsGroup->AddItem(std::make_shared<BoolCommandItem>("infiniteammo"_J));
		weaponsGlobalsGroup->AddItem(std::make_shared<BoolCommandItem>("infiniteclip"_J));
		weaponsGlobalsGroup->AddItem(std::make_shared<BoolCommandItem>("rapidfire"_J));
		weaponsGlobalsGroup->AddItem(std::make_shared<BoolCommandItem>("infiniteparachutes"_J));

		// Damage
		weaponsDamageGroup->AddItem(std::make_shared<BoolCommandItem>("ExplosiveAmmo"_J));
		weaponsDamageGroup->AddItem(std::make_shared<ConditionalItem>("ExplosiveAmmo"_J, std::make_shared<ListCommandItem>("selectedexplosion"_J)));

		weaponsDamageGroup->AddItem(std::make_shared<ConditionalItem>("ExplosiveAmmo"_J, std::make_shared<FloatCommandItem>("explosiondamage"_J, std::nullopt, false)));

		weaponsDamageGroup->AddItem(std::make_shared<ConditionalItem>("ExplosiveAmmo"_J, std::make_shared<FloatCommandItem>("explosioncamerashake"_J, std::nullopt, false)));

		weaponsDamageGroup->AddItem(std::make_shared<BoolCommandItem>("weapondamage"_J));
		weaponsDamageGroup->AddItem(std::make_shared<ConditionalItem>("weapondamage"_J, std::make_shared<FloatCommandItem>("weapondamagescale"_J, std::nullopt, false)));

		weaponsDamageGroup->AddItem(std::make_shared<BoolCommandItem>("meleedamage"_J));
		weaponsDamageGroup->AddItem(std::make_shared<ConditionalItem>("meleedamage"_J, std::make_shared<FloatCommandItem>("meleedamagescale"_J, std::nullopt, false)));

		weaponsDamageGroup->AddItem(std::make_shared<BoolCommandItem>("explosionradius"_J));
		weaponsDamageGroup->AddItem(std::make_shared<ConditionalItem>("explosionradius"_J, std::make_shared<FloatCommandItem>("explosionradiusscale"_J, std::nullopt, false)));

		// Tools
		weaponsToolsGroup->AddItem(std::make_shared<CommandItem>("giveallweapons"_J));
		weaponsToolsGroup->AddItem(std::make_shared<CommandItem>("givemaxammo"_J));

		weaponsManagementGroup->AddItem(std::make_shared<ImGuiItem>([] {
			CommandItem("opengunlocker"_J).Draw();

			ImGui::Spacing();

			if (BeginWeaponManagementPanel(WeaponManagementPanel::Editor, "Weapon Editor"))
			{
				RenderWeaponEditorMenu();
			}

			if (BeginWeaponManagementPanel(WeaponManagementPanel::AddRemoveWeapons, "Add/Remove Weapons"))
			{
				RenderWeaponManagerMenu();
			}

			if (BeginWeaponManagementPanel(WeaponManagementPanel::Loadouts, "Loadouts"))
			{
				RenderWeaponLoadoutsMenu();
			}
		}));

		// Aimbot
		weaponsAimbotGroup->AddItem(std::make_shared<BoolCommandItem>("aimbot"_J));
		weaponsAimbotGroup->AddItem(std::make_shared<ConditionalItem>("aimbot"_J, std::make_shared<BoolCommandItem>("aimbotaimforhead"_J)));
		weaponsAimbotGroup->AddItem(std::make_shared<ConditionalItem>("aimbot"_J, std::make_shared<BoolCommandItem>("aimbottargetdrivers"_J)));
		weaponsAimbotGroup->AddItem(std::make_shared<ConditionalItem>("aimbot"_J, std::make_shared<BoolCommandItem>("aimbotreleasedeadped"_J)));

		// Layout
		weapons->AddItem(weaponsGlobalsGroup);
		weapons->AddItem(weaponsDamageGroup);
		weapons->AddItem(weaponsToolsGroup);
		weapons->AddItem(weaponsEditorGroup);
		weapons->AddItem(weaponsManagementGroup);
		weapons->AddItem(weaponsAimbotGroup);
		weapons->AddItem(RenderCustomWeaponsMenu());

		return weapons;
	}
}
