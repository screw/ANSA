//
// Copyright (C) 2016 OpenSim Ltd.
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, see http://www.gnu.org/licenses/.
//

#include "inet/linklayer/ieee80211/mac/fragmentation/BasicFragmentationPolicy.h"
#include "inet/linklayer/ieee80211/mac/Ieee80211Frame_m.h"

namespace inet {
namespace ieee80211 {

Define_Module(BasicFragmentationPolicy);

void BasicFragmentationPolicy::initialize()
{
    fragmentationThreshold = par("fragmentationThreshold");
}

std::vector<int> BasicFragmentationPolicy::computeFragmentSizes(Packet *frame)
{
    if (fragmentationThreshold < frame->getByteLength()) {
        std::vector<int> sizes;
        int payloadLength = 0;
        int headerLength = 0;
        // Mgmt frames don't have payload
        const auto& header = frame->peekHeader<Ieee80211Frame>();
        const auto& trailer = frame->peekTrailer<Ieee80211MacTrailer>();
        int trailerLength = byte(trailer->getChunkLength()).get();
        if (std::dynamic_pointer_cast<Ieee80211DataFrame>(header)) {
            headerLength = byte(header->getChunkLength()).get();
            payloadLength = frame->getByteLength() - headerLength - trailerLength;
        }
        else
            headerLength = frame->getByteLength();
        int maxFragmentPayload = fragmentationThreshold - headerLength - trailerLength;
        if (payloadLength >= maxFragmentPayload * MAX_NUM_FRAGMENTS)
            throw cRuntimeError("Fragmentation: frame \"%s\" too large, won't fit into %d fragments", frame->getName(), MAX_NUM_FRAGMENTS);
        for(int i = 0; headerLength + trailerLength + payloadLength > fragmentationThreshold; i++) {
            sizes.push_back(fragmentationThreshold - headerLength - trailerLength);
            payloadLength -= maxFragmentPayload;
        }
        sizes.push_back(payloadLength);
        return sizes;
    }
    return std::vector<int>();
}

} // namespace ieee80211
} // namespace inet