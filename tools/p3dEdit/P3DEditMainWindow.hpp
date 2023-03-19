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
#include <StdXX.hpp>
#include <libBISMod.hpp>
using namespace libBISMod;
using namespace StdXX;
using namespace StdXX::UI;

class P3DEditMainWindow : public MainAppWindow
{
public:
	//Constructor
	inline P3DEditMainWindow(EventHandling::EventQueue& eventQueue) : MainAppWindow(eventQueue)
	{
		this->SetTitle(u8"p3dEdit by Amir Czwink");
		this->BuildMenu();
	}

	//Methods
	void OpenFile(const FileSystem::Path& filePath);

private:
	//State
	UniquePointer<P3DData> p3dData;

	//Methods
	void BuildMenu();
};