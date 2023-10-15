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
//Local
#include "MLOD_Lod.hpp"
#include "P3DModelInfo.hpp"

namespace libBISMod
{
	class MLODModelInfo : public P3DModelInfo
	{
	public:
		//State
		StdXX::String defaultPath;

		//Constructors
		inline MLODModelInfo()
		{
		}

		inline MLODModelInfo(StdXX::InputStream& inputStream)
		{
			uint8 defaultPath[P3D_MLOD_SP3X_PATHLENGTH];
			uint32 nBytesRead = inputStream.ReadBytes(defaultPath, sizeof(defaultPath));
			if(nBytesRead == sizeof(defaultPath))
			{
				StdXX::BufferInputStream bufferInputStream(defaultPath, P3D_MLOD_SP3X_PATHLENGTH);
				StdXX::TextReader textReader(bufferInputStream, StdXX::TextCodecType::ASCII);
				this->defaultPath = textReader.ReadZeroTerminatedString();
			}
		}

		//Methods
		void Write(StdXX::OutputStream &outputStream) const override
		{
			if(!this->defaultPath.IsEmpty())
			{
				StdXX::TextWriter textWriter(outputStream, StdXX::TextCodecType::ASCII);
				textWriter.WriteFixedLengthString(this->defaultPath, P3D_MLOD_SP3X_PATHLENGTH);
			}
		}
	};
}