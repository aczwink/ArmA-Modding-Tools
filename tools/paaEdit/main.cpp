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
#include <StdXX.hpp>
//Namespaces
using namespace libBISMod;
using namespace StdXX;
using namespace StdXX::Multimedia;

static UniquePointer<Pixmap> decode_image(const MipMap& mipmap, const ImagePyramid& imagePyramid)
{
	UniquePointer<Stream> stream = new Stream(DataType::Video);
	stream->codingParameters.codingFormat = FormatRegistry::Instance().FindCodingFormatById(imagePyramid.QueryCodingFormatId());
	auto pixelFormat = imagePyramid.QueryPixelFormat();
	if(pixelFormat.HasValue())
		stream->codingParameters.video.pixelFormat = pixelFormat;
	stream->codingParameters.video.size = {mipmap.width, mipmap.height};

	Packet inputPacket;
	inputPacket.Allocate(mipmap.data.Size());
	MemCopy(inputPacket.GetData(), mipmap.data.Data(), inputPacket.GetSize());

	UniquePointer<DecoderContext> decoderContext = stream->codingParameters.codingFormat->GetBestMatchingDecoder()->CreateContext(*stream);
	decoderContext->Decode(inputPacket);

	UniquePointer<VideoFrame> frame = dynamic_cast<VideoFrame *>(decoderContext->GetNextFrame());

	auto result = frame->GetImage();
	frame->SetImage(nullptr);

	return result;
}

static void encode_image(const Pixmap& pixmap)
{
	DynamicByteBuffer outputBuffer;
	auto outputStream = outputBuffer.CreateOutputStream();

	const Format* bmpFormat = FormatRegistry::Instance().FindFormatByFileExtension(u8"bmp");
	UniquePointer<Muxer> muxer = bmpFormat->CreateMuxer(*outputStream);

	Stream* stream = new Stream(DataType::Video);
	stream->codingParameters.codingFormat = FormatRegistry::Instance().FindCodingFormatById(CodingFormatId::RawVideo);
	stream->codingParameters.video.pixelFormat = pixmap.GetPixelFormat();
	stream->codingParameters.video.size = pixmap.GetSize();

	muxer->AddStream(stream);

	Packet packet;
	packet.Allocate(pixmap.GetNumberOfLines(0) * pixmap.GetLineSize(0));
	MemCopy(packet.GetData(), pixmap.GetPlane(0), packet.GetSize());

	muxer->WriteHeader();
	muxer->WritePacket(packet);
	muxer->Finalize();

	outputBuffer.CreateInputStream()->FlushTo(stdOut);
}

static void to_dds(const ImagePyramid& imagePyramid)
{
	const auto& topLevelMipMap = imagePyramid.GetMipMap(0);

	DynamicByteBuffer outputBuffer;
	auto outputStream = outputBuffer.CreateOutputStream();

	const Format* ddsFormat = FormatRegistry::Instance().FindFormatByFileExtension(u8"dds");
	UniquePointer<Muxer> muxer = ddsFormat->CreateMuxer(*outputStream);

	Stream* stream = new Stream(DataType::Video);
	stream->codingParameters.codingFormat = FormatRegistry::Instance().FindCodingFormatById(imagePyramid.QueryCodingFormatId());
	stream->codingParameters.video.pixelFormat = imagePyramid.QueryPixelFormat();
	stream->codingParameters.video.size = {topLevelMipMap.width, topLevelMipMap.height};

	if(!stream->codingParameters.video.pixelFormat.HasValue())
	{
		//try to get pixel format from decoder
		UniquePointer<DecoderContext> decoderContext = stream->codingParameters.codingFormat->GetBestMatchingDecoder()->CreateContext(*stream);
	}

	muxer->AddStream(stream);

	muxer->WriteHeader();

	Packet packet;
	packet.Allocate(topLevelMipMap.data.Size());
	MemCopy(packet.GetData(), topLevelMipMap.data.Data(), packet.GetSize());

	muxer->WritePacket(packet);
	muxer->Finalize();

	outputBuffer.CreateInputStream()->FlushTo(stdOut);
}

int32 Main(const String &programName, const FixedArray<String> &args)
{
	auto inputFilePath = FileSystem::FileSystemsManager::Instance().OSFileSystem().FromNativePath(args[0]);

	FileInputStream fileInputStream(inputFilePath);

	ImagePyramid imagePyramid(fileInputStream);
	to_dds(imagePyramid);

	return EXIT_SUCCESS;
}