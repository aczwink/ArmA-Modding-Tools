/*
 * Copyright (c) 2023 Amir Czwink (amir130@hotmail.de)
 *
 * This file is part of ArmA-Modding-Tools.
 *
 * ArmA-Modding-Tools is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * ArmA-Modding-Tools is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with ArmA-Modding-Tools.  If not, see <http://www.gnu.org/licenses/>.
 */
//Class header
#include "WRPEditMainWindow.hpp"

//Public methods
void WRPEditMainWindow::OpenFile(const FileSystem::Path& path)
{
	FileInputStream fileInputStream(path);
	this->world = LoadWorld(fileInputStream);
}

//Private methods
void WRPEditMainWindow::BuildMenu()
{
	Menu* menu = new Menu(u8"File");

	//File -> Open
	Action* openAction = new Action(u8"Open", [this]()
	{
		DynamicArray<Tuple<String, DynamicArray<String>>> filters;

		DynamicArray<String> extensions;
		extensions.Push(u8"*.wrp");
		filters.Push({u8"WRP files", extensions});

		this->OpenFile(this->SelectExistingFile(u8"Open WRP file", filters, FileSystem::FileSystemsManager::Instance().OSFileSystem().GetWorkingDirectory()));
	});
	menu->AppendEntry(openAction);

	this->GetMenuBar()->AppendMenu(menu);
}