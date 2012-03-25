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

#ifndef ANSAOSPFINTERFACESTATEWAITING6_H_
#define ANSAOSPFINTERFACESTATEWAITING6_H_

#include "ansaOspfInterfaceState6.h"

namespace AnsaOspf6 {

class InterfaceStateWaiting : public InterfaceState {
   public:
      virtual void ProcessEvent(Interface* intf, Interface::InterfaceEventType event);
      virtual Interface::InterfaceStateType GetState(void) const { return Interface::WaitingState; }
};

}

#endif /* ANSAOSPFINTERFACESTATEWAITING6_H_ */
