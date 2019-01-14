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

package ansa.examples.ansa.rBridge.broadcast.a;


import inet.node.inet.StandardHost;
import inet.networklayer.configurator.ipv4.Ipv4NetworkConfigurator;
import ansa.node.ANSA_Host;
import inet.node.ethernet.Eth100M;


//
// TODO Auto-generated network
//
network ping extends a
{
    @display("bgb=509,354");
    submodules:
        host2: StandardHost {
            @display("p=243,214");
            gates:
                ethg[1];
        }
        host1: StandardHost {
            @display("p=87,214");
            gates:
                ethg[1];
        }
        
                configurator: Ipv4NetworkConfigurator {
            parameters:
                @display("p=100,100;is=s");
//                config = xml("<config><interface hosts='host*' address='192.168.1.x' netmask='255.255.255.x'/></config>");
        }

    connections:
        rBridge1.ethg++ <--> Eth100M <--> host1.ethg[0];
        rBridge2.ethg++ <--> Eth100M <--> host2.ethg[0];

}
