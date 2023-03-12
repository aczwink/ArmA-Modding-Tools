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
#include <libBISMod.hpp>
using namespace StdXX;
using namespace StdXX::FileSystem;
using namespace StdXX::Multimedia;
using namespace libBISMod;

//Prototypes
void PrintManual();
void Transcode(const Path& input, const Path& output);

int32 Main(const String &programName, const FixedArray<String> &args)
{
	FormatRegistry::Instance().Register(new WSSFormat);

	stdOut << u8"wssEdit" << endl << endl;

	if(args.GetNumberOfElements() != 2)
	{
		PrintManual();
		return EXIT_FAILURE;
	}

	OSFileSystem& osFileSystem = FileSystemsManager::Instance().OSFileSystem();
	Path input = osFileSystem.FromNativePath(args[0]);
	Path output = osFileSystem.FromNativePath(args[1]);

	Transcode(input, output);
	return EXIT_SUCCESS;
}

void PrintManual()
{
	stdOut << "Note:" << endl
		   << " WSS' can be compressed. This tool does support decoding but not encoding." << endl << endl
		   << "Usage: " << endl
		   << "  wssEdit input output" << endl << endl
		   << "   input      either a Wave PCM or a WSS file" << endl
		   << "   output     file to be created" << endl;
}

void Transcode(const Path& input, const Path& output)
{
	FileInputStream inputStream(input);

	auto* inputFormat = FormatRegistry::Instance().ProbeFormat(inputStream);
	if(inputFormat == nullptr)
	{
		stdErr << u8"Could not find a format for the input" << endl;
		return;
	}

	auto* outputFormat = FormatRegistry::Instance().FindFormatByFileExtension(output.GetFileExtension());
	if(outputFormat == nullptr)
	{
		stdErr << u8"Could not find a format for the output" << endl;
		return;
	}

	UniquePointer<Multimedia::Demuxer> demuxer = inputFormat->CreateDemuxer(inputStream);
	if(demuxer.IsNull())
	{
		stdErr << u8"Input file can't be demuxed" << endl;
		return;
	}
	demuxer->ReadHeader();
	demuxer->FindStreamInfo();

	Stream* inputAudioStream = demuxer->GetStream(0);
	DecoderContext* decoder = inputAudioStream->GetDecoderContext();

	FileOutputStream outputStream(output);
	UniquePointer<Multimedia::Muxer> muxer = outputFormat->CreateMuxer(outputStream);

	Stream* targetStream = new Stream(DataType::Audio);
	muxer->AddStream(targetStream);

	targetStream->codingParameters = inputAudioStream->codingParameters;
	targetStream->codingParameters.codingFormat = outputFormat->GetPreferredCodingFormat(DataType::Audio);

	const Encoder* encoder = targetStream->codingParameters.codingFormat->GetBestMatchingEncoder();
	EncoderContext* encoderContext = encoder->CreateContext(*targetStream);
	targetStream->SetEncoderContext(encoderContext);

	muxer->WriteHeader();

	while(true)
	{
		auto packet = demuxer->ReadFrame();
		if(packet.IsNull())
			break;

		decoder->Decode(*packet);

		while(decoder->IsFrameReady())
		{
			UniquePointer<Frame> frame = decoder->GetNextFrame();
			encoderContext->Encode(*frame);

			while(encoderContext->IsPacketReady())
			{
				UniquePointer<IPacket> encodedPacket = encoderContext->GetNextPacket();
				muxer->WritePacket(*encodedPacket);
			}
		}
	}

	muxer->Finalize();
}