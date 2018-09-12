//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#ifndef __ANSAINET_ANSA_ETHERENCAP_H_
#define __ANSAINET_ANSA_ETHERENCAP_H_

#include "inet/linklayer/ethernet/EtherEncap.h"


namespace inet {

/**
 * To be moved to INET
 */
class INET_API ANSA_EtherEncap : public EtherEncap
{
  protected:
    virtual int numInitStages() const override { return NUM_INIT_STAGES; }
    virtual void initialize(int stage) override;

};

} //namespace

#endif
