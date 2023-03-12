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
#include <libBISMod/wss/WSSFormat.hpp>
//Local
#include "WSS.hpp"
#include "WSSDemuxer.hpp"
#include "WSSMuxer.hpp"
//Namespaces
using namespace StdXX;
using namespace StdXX::Multimedia;
using namespace libBISMod;

//Public methods
Demuxer *WSSFormat::CreateDemuxer(SeekableInputStream& inputStream) const
{
	return new WSSDemuxer(*this, inputStream);
}

Muxer *WSSFormat::CreateMuxer(SeekableOutputStream& outputStream) const
{
	return new WSSMuxer(*this, outputStream);
}

String WSSFormat::GetExtension() const
{
	return "wss";
}

void WSSFormat::GetFormatInfo(FormatInfo& formatInfo) const
{
	formatInfo.supportsByteSeeking = true;
}

String WSSFormat::GetName() const
{
	return "WSS";
}

DynamicArray<const CodingFormat *> WSSFormat::GetSupportedCodingFormats(DataType dataType) const
{
	DynamicArray<const CodingFormat*> codingFormats;

	if(dataType != DataType::Audio)
		return codingFormats;

	codingFormats.Push(FormatRegistry::GetCodingFormatById(CodingFormatId::PCM_S16LE));

	return codingFormats;
}

float32 WSSFormat::Probe(BufferInputStream& buffer) const
{
	byte readSignature[4];
	uint32 nBytesRead = buffer.ReadBytes(readSignature, sizeof(readSignature));
	if(nBytesRead < sizeof(readSignature))
		return Format::FORMAT_MATCH_BUFFER_TOO_SMALL; //buffer too small

	if(MemCmp(readSignature, WSS_SIGNATURE, 4) == 0)
		return 1;
	return 0;
}
