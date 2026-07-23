#include "resourceMenu.hpp"
#include <imgui.h>
#include <ImGuiFileDialog.h>
#include <iostream>

const auto ResourceMenu::draw() -> void const
{
	if (isHiddenByKeys)
		return;

	float screenW = ImGui::GetIO().DisplaySize.x;
	float screenH = ImGui::GetIO().DisplaySize.y;

	ImGui::SetNextWindowPos(ImVec2(screenW - menuWidth, menuHeight));
	ImGui::SetNextWindowSize(ImVec2(menuWidth, screenH - menuHeight));

	const auto flags = ImGuiWindowFlags_NoMove |
					   ImGuiWindowFlags_NoResize |
					   ImGuiWindowFlags_NoCollapse;
	
	static std::string updateID = "";
	static char idBuf[64] = "";
	static std::string pathBuf = "";
	if (ImGui::Begin("Resource Manager", nullptr, flags))
	{
		ImGui::SetWindowFontScale(1.5f);
		if (ImGui::Button("Load registry"))
		{
			IGFD::FileDialogConfig config;
			ImGuiFileDialog::Instance()->OpenDialog("LoadRegistryKey", "Select Registry JSON", ".json", config);
		}
		ImGui::SameLine();

		if (ImGui::Button("Save registry"))
		{
			manager.saveRegistry();
		}

		ImGui::Separator();

		ImGui::Text("Add new asset:");

		ImGui::InputText("ID", idBuf, IM_ARRAYSIZE(idBuf));
		ImGui::Text("Path: %s", pathBuf.empty() ? "No path selected" : pathBuf.c_str());
		ImGui::SameLine();

		if (ImGui::Button("Browse"))
		{
			IGFD::FileDialogConfig config;
			ImGuiFileDialog::Instance()->OpenDialog("AddFileKey", "Select Image", ".png,.jpg,.jpeg", config);
		}

		if (ImGui::Button("Add asset"))
		{
			if (strlen(idBuf) > 0 && !pathBuf.empty())
			{
				manager.addAsset(idBuf, pathBuf);
				memset(idBuf, 0, sizeof(idBuf));
				pathBuf = "";
			}
		}

		ImGui::Text("Assets:");
		std::vector<std::string> toRemove;

		if (ImGui::BeginTable("AssetTable", 4, ImGuiTableFlags_Borders 
												| ImGuiTableFlags_RowBg 
												| ImGuiTableFlags_Resizable
												| ImGuiTableFlags_ScrollY))
		{
			ImGui::TableSetupColumn("Preview");
			ImGui::TableSetupColumn("ID");
			ImGui::TableSetupColumn("Path");
			ImGui::TableSetupColumn("Actions", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoResize, 80.0f);
			ImGui::TableHeadersRow();


			for (auto const &[id, path] : manager.getRegistry())
			{
				ImGui::PushID(id.c_str());
				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				ImGui::AlignTextToFramePadding();
				auto texture = manager.getTexture(id);
				auto tex = texture.texID;
				if (tex)
				{
					auto avail = ImGui::GetContentRegionAvail().x;
					auto size = std::clamp(avail - 4.0f, 16.0f, 256.0f);
					auto aspect = texture.h / texture.w;
					ImGui::Image(tex, ImVec2(size, size * aspect));
					ImGui::SameLine();
				}
				ImGui::TableSetColumnIndex(1);
				ImGui::TextWrapped("%s", id.c_str());

				ImGui::TableSetColumnIndex(2);
				ImGui::TextWrapped("%s", path.c_str());
				

				ImGui::TableSetColumnIndex(3);
				if (ImGui::Button("Update"))
				{
					updateID = id;
					IGFD::FileDialogConfig config;
					ImGuiFileDialog::Instance()->OpenDialog("UpdateFileKey", "Update Image Path", ".png,.jpg,.jpeg", config);
				}
				if (ImGui::Button("Remove"))
				{
					toRemove.push_back(id);
				}
				ImGui::PopID();
			}
			ImGui::EndTable();
			for (const auto id : toRemove)
			{
				manager.removeAsset(id);
			}
		}
	}

	ImGui::End();

	if (ImGuiFileDialog::Instance()->IsOpened())
	{
		ImVec2 screenSize = ImGui::GetIO().DisplaySize;
		ImGui::SetNextWindowSize(ImVec2(screenSize.x * 0.8f, screenSize.y * 0.8f), ImGuiCond_Appearing);
		ImGui::SetNextWindowPos(ImVec2(screenSize.x * 0.5f, screenSize.y * 0.5f), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
	}
	if (ImGuiFileDialog::Instance()->Display("LoadRegistryKey"))
	{
		if (ImGuiFileDialog::Instance()->IsOk())
		{
			manager.loadRegistry(ImGuiFileDialog::Instance()->GetFilePathName());
		}
		ImGuiFileDialog::Instance()->Close();
	}

	if (ImGuiFileDialog::Instance()->Display("AddFileKey"))
	{
		if (ImGuiFileDialog::Instance()->IsOk())
		{
			pathBuf = ImGuiFileDialog::Instance()->GetFilePathName();
		}
		ImGuiFileDialog::Instance()->Close();
	}

	if (ImGuiFileDialog::Instance()->Display("UpdateFileKey"))
	{
		if (ImGuiFileDialog::Instance()->IsOk())
		{
			std::string newPath = ImGuiFileDialog::Instance()->GetFilePathName();

			if (!updateID.empty())
			{
				manager.updateAsset(updateID, newPath);
			}
		}
		updateID = "";
		ImGuiFileDialog::Instance()->Close();
	}
}

const auto ResourceMenu::update() -> void
{
	if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) && ImGui::IsKeyPressed(ImGuiKey_Y))
	{
		isHiddenByKeys = !isHiddenByKeys;
	}
}

auto ResourceMenu::setHiddenByKeys(bool isHidden) -> void
{
	isHiddenByKeys = isHidden;
}

const auto ResourceMenu::getHiddenByKeys() -> bool const
{
	return isHiddenByKeys;
}
