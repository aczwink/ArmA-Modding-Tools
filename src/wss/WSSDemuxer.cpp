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
#include "WSSDemuxer.hpp"
//Local
#include "WSS.hpp"
//Libs
#include <StdXXStreams.hpp>
//Namespaces
using namespace StdXX;
using namespace StdXX::Multimedia;
//Definitions
#define WSS_MAXPACKETSIZE 4096

//Public methods
void WSSDemuxer::ReadHeader()
{
	DataReader dataReader(false, this->inputStream);
	dataReader.Skip(4); //signature
	uint32 compressionType = dataReader.ReadUInt32();

	Stream* stream = new Stream(DataType::Audio);
	switch(compressionType)
	{
		case WSS_COMPRESSIONTYPE_UNCOMPRESSED:
			stream->codingParameters.codingFormat = FormatRegistry::Instance().FindCodingFormatById(CodingFormatId::PCM_S16LE);
			break;
		case WSS_COMPRESSIONTYPE_BYTECOMPRESSION:
			stream->codingParameters.codingFormat = &this->wssCompressedCodingFormat;
			break;
		default:
			NOT_IMPLEMENTED_ERROR; //TODO: implement me
			return;
	}

	uint16 formatTag = dataReader.ReadUInt16();
	uint16 nChannels = dataReader.ReadUInt16();
	uint32 sampleRate = dataReader.ReadUInt32();
	uint32 bytesPerSecond = dataReader.ReadUInt32();
	uint16 blockAlign = dataReader.ReadUInt16();
	uint16 bitsPerSample = dataReader.ReadUInt16();
	uint16 unknown = dataReader.ReadUInt16();

	ASSERT_EQUALS(1, formatTag); //wave pcm
	ASSERT_EQUALS(nChannels * sampleRate * (bitsPerSample / 8), bytesPerSecond);
	ASSERT_EQUALS(2, blockAlign);
	ASSERT_EQUALS(16, bitsPerSample);

	stream->codingParameters.audio.sampleRate = sampleRate;
	stream->codingParameters.audio.sampleFormat = {(uint8)nChannels, AudioSampleType::S16, false};

	this->AddStream(stream);
}

void WSSDemuxer::Seek(uint64 timestamp, const class TimeScale &timeScale)
{
	NOT_IMPLEMENTED_ERROR; //TODO: implement me
}

//Private methods
UniquePointer<IPacket> WSSDemuxer::ReadPacket()
{
	uint32 left = this->inputStream.QueryRemainingBytes();
	if(left == 0)
		return nullptr;

	uint32 size = Math::Min((uint32)WSS_MAXPACKETSIZE, left);

	UniquePointer<Packet> packet = new Packet;

	packet->streamIndex = 0;

	packet->Allocate(size);
	uint32 nBytesRead = this->inputStream.ReadBytes(packet->GetData(), packet->GetSize());
	packet->Allocate(nBytesRead);

	return packet;
}
