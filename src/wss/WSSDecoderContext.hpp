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
#include <StdXXMultimedia.hpp>
//Definitions
#define LOG2_E 1.4426950408889634070 //log2(e)
#define LN10 2.3025850929940456840 //ln(10)
#define MAGIC_NUMBER ( (LN10 * LOG2_E) / 28.12574042515172)
#define ROUND(f) (int32)((double)f+0.5)

class WSSDecoderContext : public StdXX::Multimedia::DecoderContext
{
public:
	//Constructor
	inline WSSDecoderContext(StdXX::Multimedia::Stream &stream) : DecoderContext(stream)
	{
	}

	void Decode(const StdXX::Multimedia::IPacket& packet) override
	{
		int8* input = (int8 *) packet.GetData();
		uint32 compressedSize = packet.GetSize();

		ASSERT_EQUALS(1, this->stream.codingParameters.audio.sampleFormat->nChannels);

		auto buffer = new StdXX::Multimedia::AudioBuffer(packet.GetSize(), *this->stream.codingParameters.audio.sampleFormat);
		int16* output = (int16*)buffer->GetPlane(0);

		int16 lastValue = 0;
		for(; compressedSize--; input++)
		{
			if(*input)
			{
				float64 asFloat = abs(*input) * MAGIC_NUMBER;
				float64 rounded = ROUND(asFloat);
				asFloat = pow(2.0, asFloat - rounded) * pow(2, rounded);

				if(*input < 0)
					asFloat *= -1;

				int32 asInt = ROUND(asFloat) + lastValue;

				if(asInt > SHRT_MAX)
					asInt = SHRT_MAX;
				if(asInt < SHRT_MIN)
					asInt = SHRT_MIN;
				lastValue = (int16)asInt;
			}
			*output++ = lastValue;
		}

		this->AddFrame(new StdXX::Multimedia::AudioFrame(buffer));
	}
};