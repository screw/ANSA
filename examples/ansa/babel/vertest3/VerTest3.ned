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

package inet.examples.ansa.babel.vertest3;

import ansa.node.ANSA_BabelRouter;
import ansa.node.ANSA_Host;
import inet.node.ethernet.Eth100M;
import inet.networklayer.configurator.ipv4.IPv4NetworkConfigurator;
import inet.networklayer.configurator.ipv6.FlatNetworkConfigurator6;
import inet.common.scenario.ScenarioManager;


network VerTest3
{
    @display("bgb=521,431,white");
    submodules:
        configurator: IPv4NetworkConfigurator {
            parameters:
                config = xml("<config></config>");
                assignAddresses = false;
                assignDisjunctSubnetAddresses = false;
                addStaticRoutes = false;
                addDefaultRoutes = false;
                addSubnetRoutes = false;
                optimizeRoutes = false;
                @display("p=450,290");
        }

//        channelControl: ChannelControl {
//            parameters:
//                @display("p=60,50");
//        }

        R1: ANSA_BabelRouter {
            parameters:
                @display("p=250,357");
            gates:
                ethg[3];
        }
        R2: ANSA_BabelRouter {
            parameters:
                @display("p=250,207");
            gates:
                ethg[4];
        }
        R3: ANSA_BabelRouter {
            parameters:
                @display("p=450,207");
            gates:
                ethg[3];
        }
        R4: ANSA_BabelRouter {
            parameters:
                @display("p=50,207");
            gates:
                ethg[3];
        }

        LAN1: ANSA_Host {
            @display("p=50,357;i=misc/cloud");
        }
        LAN2: ANSA_Host {
            @display("p=250,57;i=misc/cloud");
        }
        LAN3: ANSA_Host {
            @display("p=450,57;i=misc/cloud");
        }
        LAN4: ANSA_Host {
            @display("p=50,57;i=misc/cloud");
        }
        scenarioManager: ScenarioManager {
            @display("p=450,357");
        }

    connections allowunconnected:
        R1.ethg[0] <--> Eth100M <--> R2.ethg[0];
        R1.ethg[1] <--> Eth100M <--> R3.ethg[0];
        R2.ethg[1] <--> Eth100M <--> R3.ethg[1];
        R2.ethg[2] <--> Eth100M <--> R4.ethg[0];

        R1.ethg[2] <--> Eth100M <--> LAN1.ethg++;
        R2.ethg[3] <--> Eth100M <--> LAN2.ethg++;
        R3.ethg[2] <--> Eth100M <--> LAN3.ethg++;
        R4.ethg[1] <--> Eth100M <--> LAN4.ethg++;

}

