#include "Network.hpp"

#include "core/backend/FiberPool.hpp"
#include "core/frontend/Notifications.hpp"
#include "game/frontend/items/Items.hpp"
#include "game/frontend/submenus/Network/RandomEvents.hpp"
#include "game/frontend/submenus/Network/SavedPlayers.hpp"
#include "game/gta/Network.hpp"

namespace YimMenu::Submenus
{
	namespace
	{
		enum class JoinMethod
		{
			SESSION_TYPE,
			USERNAME,
			ROCKSTAR_ID
		};

		std::shared_ptr<Group> BuildJoinSessionGroup()
		{
			auto joinGroup = std::make_shared<Group>("Join Session");

			joinGroup->AddItem(std::make_shared<ImGuiItem>([] {
				static JoinMethod joinMethod{JoinMethod::SESSION_TYPE};
				static std::uint64_t rockstarId{};
				static char username[24]{};

				constexpr const char* joinMethodNames[]{"Session Type", "Username", "Rockstar ID"};
				const int selectedMethod = static_cast<int>(joinMethod);

				ImGui::SetNextItemWidth(240.0f);
				if (ImGui::BeginCombo("Join Method", joinMethodNames[selectedMethod]))
				{
					for (int method = 0; method < 3; ++method)
					{
						const bool selected = selectedMethod == method;
						if (ImGui::Selectable(joinMethodNames[method], selected))
							joinMethod = static_cast<JoinMethod>(method);

						if (selected)
							ImGui::SetItemDefaultFocus();
					}

					ImGui::EndCombo();
				}

				ImGui::Spacing();

				switch (joinMethod)
				{
				case JoinMethod::SESSION_TYPE: ListCommandItem("joinsessiontype"_J, "Session Type").Draw(); break;
				case JoinMethod::USERNAME:
					ImGui::SetNextItemWidth(240.0f);
					ImGui::InputTextWithHint("Username", "Rockstar username", username, sizeof(username));
					break;
				case JoinMethod::ROCKSTAR_ID:
					ImGui::SetNextItemWidth(240.0f);
					ImGui::InputScalar("Rockstar ID", ImGuiDataType_U64, &rockstarId);
					break;
				}

				ImGui::Spacing();

				if (joinMethod == JoinMethod::SESSION_TYPE)
				{
					CommandItem("joinsession"_J, "Join Session").Draw();
				}
				else if (ImGui::Button("Join Session"))
				{
					if (joinMethod == JoinMethod::USERNAME)
					{
						FiberPool::Push([] {
							const auto resolvedId = YimMenu::Network::ResolveRockstarId(username);
							if (resolvedId)
							{
								YimMenu::Network::JoinRockstarId(*resolvedId);
							}
							else
							{
								Notifications::Show("Joiner", "Failed to get RID from username", NotificationType::Error);
							}
						});
					}
					else
					{
						FiberPool::Push([] {
							YimMenu::Network::JoinRockstarId(rockstarId);
						});
					}
				}
			}));

			return joinGroup;
		}

		std::shared_ptr<Group> BuildSessionToolsGroup()
		{
			// One row keeps the bounty controls and the remaining session-wide action aligned.
			auto sessionToolsGroup = std::make_shared<Group>("Session Tools", 1);
			sessionToolsGroup->AddItem(std::make_shared<IntCommandItem>("bountyamount"_J, "Bounty Amount"));
			sessionToolsGroup->AddItem(std::make_shared<BoolCommandItem>("anonymousbounty"_J, "Anonymous"));
			sessionToolsGroup->AddItem(std::make_shared<CommandItem>("setbountyall"_J, "Set Bounties"));
			sessionToolsGroup->AddItem(std::make_shared<BoolCommandItem>("forcethunder"_J));
			return sessionToolsGroup;
		}

		std::shared_ptr<Group> BuildTeleportGroup()
		{
			// Two rows create two balanced columns: destinations on the left,
			// coordinate and bring controls on the right.
			auto teleportGroup = std::make_shared<Group>("Teleport Players", 2);

			auto propertyGroup = std::make_shared<Group>("", 1);
			propertyGroup->AddItem(std::make_shared<ListCommandItem>("sendtopropertyindex"_J, "##selproperty"));
			propertyGroup->AddItem(std::make_shared<CommandItem>("sendtopropertyall"_J, "Send All to Property"));

			auto interiorGroup = std::make_shared<Group>("", 1);
			interiorGroup->AddItem(std::make_shared<ListCommandItem>("sendtointeriorindex"_J, "##selinterior"));
			interiorGroup->AddItem(std::make_shared<CommandItem>("sendtointeriorall"_J, "Send All to Interior"));

			auto coordinateGroup = std::make_shared<Group>("", 1);
			coordinateGroup->AddItem(std::make_shared<Vector3CommandItem>("playertpcoord"_J, ""));
			coordinateGroup->AddItem(std::make_shared<CommandItem>("tpplayertocoordall"_J, "Teleport Everyone"));

			teleportGroup->AddItem(std::move(propertyGroup));
			teleportGroup->AddItem(std::move(interiorGroup));
			teleportGroup->AddItem(std::move(coordinateGroup));
			teleportGroup->AddItem(std::make_shared<CommandItem>("bringall"_J, "Bring All"));
			return teleportGroup;
		}

		std::shared_ptr<Group> BuildPlayerActionsGroup()
		{
			// Three rows keep related actions together without a long vertical stack.
			auto playerActionsGroup = std::make_shared<Group>("Player Actions", 3);

			playerActionsGroup->AddItem(std::make_shared<CommandItem>("sendsextall"_J, "Send Sexts"));
			playerActionsGroup->AddItem(std::make_shared<BoolCommandItem>("harassplayers"_J));
			playerActionsGroup->AddItem(std::make_shared<BoolCommandItem>("spamkillfeed"_J));

			playerActionsGroup->AddItem(std::make_shared<CommandItem>("deletevehall"_J, "Delete Player Vehicles"));
			playerActionsGroup->AddItem(std::make_shared<CommandItem>("killall"_J, "Kill All"));
			playerActionsGroup->AddItem(std::make_shared<CommandItem>("explodeall"_J, "Explode All"));

			playerActionsGroup->AddItem(std::make_shared<CommandItem>("killexploitall"_J, "Permadeath All"));
			playerActionsGroup->AddItem(std::make_shared<CommandItem>("ceokickall"_J, "CEO Kick All"));
			playerActionsGroup->AddItem(std::make_shared<CommandItem>("hkickall"_J, "Host Kick All"));

			return playerActionsGroup;
		}

		std::shared_ptr<Group> BuildEnhancementsGroup()
		{
			// Four rows produce two evenly sized columns.
			auto enhancementsGroup = std::make_shared<Group>("Enhancements", 4);

			enhancementsGroup->AddItem(std::make_shared<BoolCommandItem>("notifyonplayerjoin"_J));
			enhancementsGroup->AddItem(std::make_shared<BoolCommandItem>("fastjoin"_J));
			enhancementsGroup->AddItem(std::make_shared<BoolCommandItem>("forcescripthost"_J));
			enhancementsGroup->AddItem(std::make_shared<BoolCommandItem>("pausegame"_J));

			enhancementsGroup->AddItem(std::make_shared<BoolCommandItem>("disabledeathbarriers"_J));
			enhancementsGroup->AddItem(std::make_shared<BoolCommandItem>("despawnbypass"_J));
			enhancementsGroup->AddItem(std::make_shared<BoolCommandItem>("bypasscasinogeoblock"_J));
			enhancementsGroup->AddItem(std::make_shared<BoolCommandItem>("nocalls"_J));

			return enhancementsGroup;
		}

		std::shared_ptr<Category> BuildSessionCategory()
		{
			auto session = std::make_shared<Category>("Session");
			session->AddItem(BuildJoinSessionGroup());
			session->AddItem(BuildSessionToolsGroup());
			session->AddItem(BuildTeleportGroup());
			session->AddItem(BuildPlayerActionsGroup());
			session->AddItem(BuildEnhancementsGroup());
			return session;
		}

		std::shared_ptr<Category> BuildSpoofingCategory()
		{
			auto spoofing = std::make_shared<Category>("Spoofing");

			auto clientGroup = std::make_shared<Group>("Matchmaking (Client)", 2);
			clientGroup->AddItem(std::make_shared<BoolCommandItem>("cheaterpool"_J));

			auto clientRegionGroup = std::make_shared<Group>("", 1);
			clientRegionGroup->AddItem(std::make_shared<BoolCommandItem>("spoofmmregion"_J, "Spoof Region"));
			clientRegionGroup->AddItem(std::make_shared<ConditionalItem>("spoofmmregion"_J, std::make_shared<ListCommandItem>("mmregion"_J, "##mmregion")));

			clientGroup->AddItem(std::make_shared<ConditionalItem>("cheaterpool"_J, std::move(clientRegionGroup), true));
			clientGroup->AddItem(std::make_shared<BoolCommandItem>("spoofdatahash"_J));
			spoofing->AddItem(std::move(clientGroup));

			auto serverGroup = std::make_shared<Group>("Matchmaking (Server)", 2);

			auto serverRegionGroup = std::make_shared<Group>("", 1);
			serverRegionGroup->AddItem(std::make_shared<BoolCommandItem>("mmspoofregiontype"_J));
			serverRegionGroup->AddItem(std::make_shared<ConditionalItem>("mmspoofregiontype"_J, std::make_shared<ListCommandItem>("mmregiontype"_J, "##mmregiontype")));

			auto serverLanguageGroup = std::make_shared<Group>("", 1);
			serverLanguageGroup->AddItem(std::make_shared<BoolCommandItem>("mmspooflanguage"_J));
			serverLanguageGroup->AddItem(std::make_shared<ConditionalItem>("mmspooflanguage"_J, std::make_shared<ListCommandItem>("mmlanguage"_J, "##mmlanguage")));

			auto serverPlayerCountGroup = std::make_shared<Group>("", 1);
			serverPlayerCountGroup->AddItem(std::make_shared<BoolCommandItem>("mmspoofplayercount"_J));
			serverPlayerCountGroup->AddItem(std::make_shared<ConditionalItem>("mmspoofplayercount"_J, std::make_shared<IntCommandItem>("mmplayercount"_J, "##mmplayercount")));

			auto multiplexGroup = std::make_shared<Group>("", 1);
			multiplexGroup->AddItem(std::make_shared<BoolCommandItem>("mmmultiplexsession"_J));
			multiplexGroup->AddItem(std::make_shared<ConditionalItem>("mmmultiplexsession"_J, std::make_shared<IntCommandItem>("mmmultiplexsessioncount"_J, "##mmmultiplexsessioncount")));

			serverGroup->AddItem(std::move(serverRegionGroup));
			serverGroup->AddItem(std::move(serverLanguageGroup));
			serverGroup->AddItem(std::move(serverPlayerCountGroup));
			serverGroup->AddItem(std::move(multiplexGroup));
			spoofing->AddItem(std::move(serverGroup));

			return spoofing;
		}
	}

	Network::Network() :
#define ICON_FA_ROUTE "\xef\x9b\xbf"
	    Submenu::Submenu("Network", ICON_FA_ROUTE)
	{
		AddCategory(BuildSessionCategory());
		AddCategory(BuildSpoofingCategory());
		AddCategory(BuildSavedPlayersMenu());
		AddCategory(BuildRandomEventsMenu());
	}
}