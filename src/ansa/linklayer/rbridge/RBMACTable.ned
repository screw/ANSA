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

package ansa.linklayer.rbridge;

import inet.linklayer.contract.IMacAddressTable;

simple RBMACTable like IMacAddressTable
{
    parameters:
        @display("i=block/table2,lightblue");
        xml configData = default(xmldoc("config.xml"));
        xml configFile = default(xmldoc("config.xml"));
        string deviceId = default("");
        string deviceType = "RBridge";
        //TODO ANSAINET4.0 Utilize these value (at least load them in .cc)
        double agingTime @unit(s) = default(120s);
        string addressTableFile = default("");
        
}
