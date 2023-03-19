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
#include "P3DEditMainWindow.hpp"

//Public methods
void P3DEditMainWindow::OpenFile(const FileSystem::Path &filePath)
{
	FileInputStream fileInputStream(filePath);
	this->p3dData = ReadP3DFile(fileInputStream);
}


//Private methods
void P3DEditMainWindow::BuildMenu()
{
	Menu* menu = new Menu(u8"File");

	Action* openAction = new Action(u8"Open", [this](){
		DynamicArray<Tuple<String, DynamicArray<String>>> filters;

		DynamicArray<String> extensions;
		extensions.Push(u8"*.p3d");
		filters.Push({u8"P3D files", extensions});

		this->OpenFile(this->SelectExistingFile(u8"Open P3D file", filters, FileSystem::FileSystemsManager::Instance().OSFileSystem().GetWorkingDirectory()));
	});
	menu->AppendEntry(openAction);

	this->GetMenuBar()->AppendMenu(menu);
}