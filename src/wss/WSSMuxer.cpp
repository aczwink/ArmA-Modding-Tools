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
#include "WSSMuxer.hpp"
//Local
#include "WSS.hpp"
//Namespaces
using namespace StdXX;
using namespace StdXX::Multimedia;

void WSSMuxer::Finalize()
{
}

void WSSMuxer::WriteHeader()
{
	const auto& audioParams = this->GetStream(0)->codingParameters.audio;

	uint16 blockAlign = audioParams.sampleFormat->nChannels * audioParams.sampleFormat->GetSampleSize();

	DataWriter dataWriter(false, this->outputStream);

	dataWriter.WriteBytes(WSS_SIGNATURE, 4);
	dataWriter.WriteUInt32(WSS_COMPRESSIONTYPE_UNCOMPRESSED);
	dataWriter.WriteUInt16(1); //PCM
	dataWriter.WriteUInt16(audioParams.sampleFormat->nChannels);
	dataWriter.WriteUInt32(audioParams.sampleRate);
	dataWriter.WriteUInt32(audioParams.sampleRate * blockAlign);
	dataWriter.WriteUInt16(blockAlign);
	dataWriter.WriteUInt16(audioParams.sampleFormat->GetSampleSize() * 8);
	dataWriter.WriteUInt16(0); //output size
}

void WSSMuxer::WritePacket(const IPacket &packet)
{
	this->outputStream.WriteBytes(packet.GetData(), packet.GetSize());
}
