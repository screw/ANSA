// Copyright (C) 2012 - 2013 Brno University of Technology (http://nes.fit.vutbr.cz/ansa)
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

/**
 * @file ISIS.cc
 * @author Matej Hrncirik, Marcel Marek (mailto:imarek@fit.vutbr.cz), Vladimir Vesely (mailto:ivesely@fit.vutbr.cz)
 * @date 7.3.2012
 * @brief Base class for the IS-IS module.
 * @detail Base class for the IS-IS module.
 * @todo TODO B3 Multicast MAC adresses 01:80:c2:00:00:14 and :15 works so replace the ff:ff:...
 *       TODO A1 SEVERE call trillDIS() -> such method would appoint forwarder and handle other TRILL-DIS related duties
 *       TODO B3 Add SimTime isisStarted; and compute initial wait period (for LSP generating and SPF calculation} from such variable
 */

#include "ansa/networklayer/isis/ISISMain.h"

#include "inet/common/packet/Packet.h"
#include "inet/common/ModuleAccess.h"
#include "inet/linklayer/common/MacAddressTag_m.h"
#include "inet/linklayer/common/InterfaceTag_m.h"

#include "inet/common/IProtocolRegistrationListener.h"
#include "inet/linklayer/common/Ieee802SapTag_m.h"
#include "inet/common/ProtocolTag_m.h"

#include "ansa/networklayer/isis/ISISDeviceConfigurator.h"





//#include "TRILL.h"

//TODO A! Create ISISDeviceConfigurator class and include it
//#include "deviceConfigurator.h"

//TODO A! Add TRILL source files
//#include "TrillInterfaceData.h"

namespace inet {

Define_Module(ISISMain);

ISISMain::ISISMain() {
    this->L1LSPDb = new std::vector<LSPRecord*>;
    this->L2LSPDb = new std::vector<LSPRecord*>;

    this->L1SRMBQueue = new std::vector<std::vector<FlagRecord *> *>;
    this->L2SRMBQueue = new std::vector<std::vector<FlagRecord *> *>;

    this->L1SRMPTPQueue = new std::vector<std::vector<FlagRecord *> *>;
    this->L2SRMPTPQueue = new std::vector<std::vector<FlagRecord *> *>;

    this->L1SSNBQueue = new std::vector<std::vector<FlagRecord *> *>;
    this->L2SSNBQueue = new std::vector<std::vector<FlagRecord *> *>;

    this->L1SSNPTPQueue = new std::vector<std::vector<FlagRecord *> *>;
    this->L2SSNPTPQueue = new std::vector<std::vector<FlagRecord *> *>;

    this->attIS = new ISISNeighbours_t;

    att = false;
}

/*
 * This method deallocate dynamically created objects and free their memory.
 */
ISISMain::~ISISMain() {
    if (this->L1LSPDb != NULL) {
        std::vector<LSPRecord *>::iterator it = this->L1LSPDb->begin();
        for (; it != this->L1LSPDb->end(); ++it) {
            //delete (*it)->LSP;
            //delete (*it)->deadTimer;
            delete (*it);
        }
        this->L1LSPDb->clear();
        delete this->L1LSPDb;
    }

    if (this->L2LSPDb != NULL) {
        std::vector<LSPRecord *>::iterator it = this->L2LSPDb->begin();
        for (; it != this->L2LSPDb->end(); ++it) {
            //delete (*it)->LSP;
            //delete (*it)->deadTimer;
            delete (*it);
        }
        this->L2LSPDb->clear();
        delete this->L2LSPDb;
    }

    /* delete SRM */
    if (this->L1SRMBQueue != NULL) {
        std::vector<std::vector<FlagRecord*> *>::iterator qIt =
                this->L1SRMBQueue->begin();
        for (; qIt != this->L1SRMBQueue->end(); ++qIt) {
            std::vector<FlagRecord*>::iterator it = (*qIt)->begin();
            for (; it != (*qIt)->end(); ++it) {
                delete (*it);
            }
            (*qIt)->clear();
            delete (*qIt);
        }
        this->L1SRMBQueue->clear();
        delete this->L1SRMBQueue;
    }

    if (this->L1SRMPTPQueue != NULL) {
        std::vector<std::vector<FlagRecord*> *>::iterator qIt =
                this->L1SRMPTPQueue->begin();
        for (; qIt != this->L1SRMPTPQueue->end(); ++qIt) {
            std::vector<FlagRecord*>::iterator it = (*qIt)->begin();
            for (; it != (*qIt)->end(); ++it) {
                delete (*it);
            }
            (*qIt)->clear();
            delete (*qIt);
        }
        this->L1SRMPTPQueue->clear();
        delete this->L1SRMPTPQueue;
    }

    if (this->L2SRMBQueue != NULL) {
        std::vector<std::vector<FlagRecord*> *>::iterator qIt =
                this->L2SRMBQueue->begin();
        for (; qIt != this->L2SRMBQueue->end(); ++qIt) {
            std::vector<FlagRecord*>::iterator it = (*qIt)->begin();
            for (; it != (*qIt)->end(); ++it) {
                delete (*it);
            }
            (*qIt)->clear();
            delete (*qIt);
        }
        this->L2SRMBQueue->clear();
        delete this->L2SRMBQueue;
    }

    if (this->L2SRMPTPQueue != NULL) {
        std::vector<std::vector<FlagRecord*> *>::iterator qIt =
                this->L2SRMPTPQueue->begin();
        for (; qIt != this->L2SRMPTPQueue->end(); ++qIt) {
            std::vector<FlagRecord*>::iterator it = (*qIt)->begin();
            for (; it != (*qIt)->end(); ++it) {
                delete (*it);
            }
            (*qIt)->clear();
            delete (*qIt);
        }
        this->L2SRMPTPQueue->clear();
        delete this->L2SRMPTPQueue;
    }
    /* end of delete SRM */

    /* delete SSN */
    if (this->L1SSNBQueue != NULL) {
        std::vector<std::vector<FlagRecord*> *>::iterator qIt =
                this->L1SSNBQueue->begin();
        for (; qIt != this->L1SSNBQueue->end(); ++qIt) {
            std::vector<FlagRecord*>::iterator it = (*qIt)->begin();
            for (; it != (*qIt)->end(); ++it) {

                delete (*it);

            }
            (*qIt)->clear();
            delete (*qIt);
        }
        this->L1SSNBQueue->clear();
        delete this->L1SSNBQueue;
    }

    if (this->L1SSNPTPQueue != NULL) {
        std::vector<std::vector<FlagRecord*> *>::iterator qIt =
                this->L1SSNPTPQueue->begin();
        for (; qIt != this->L1SSNPTPQueue->end(); ++qIt) {
            std::vector<FlagRecord*>::iterator it = (*qIt)->begin();
            for (; it != (*qIt)->end(); ++it) {
                delete (*it);
            }
            (*qIt)->clear();
            delete (*qIt);
        }
        this->L1SSNPTPQueue->clear();
        delete this->L1SSNPTPQueue;
    }

    if (this->L2SSNBQueue != NULL) {
        std::vector<std::vector<FlagRecord*> *>::iterator qIt =
                this->L2SSNBQueue->begin();
        for (; qIt != this->L2SSNBQueue->end(); ++qIt) {
            std::vector<FlagRecord*>::iterator it = (*qIt)->begin();
            for (; it != (*qIt)->end(); ++it) {
                delete (*it);
            }
            (*qIt)->clear();
            delete (*qIt);
        }
        this->L2SSNBQueue->clear();
        delete this->L2SSNBQueue;
    }

    if (this->L2SSNPTPQueue != NULL) {
        std::vector<std::vector<FlagRecord*> *>::iterator qIt =
                this->L2SSNPTPQueue->begin();
        for (; qIt != this->L2SSNPTPQueue->end(); ++qIt) {
            std::vector<FlagRecord*>::iterator it = (*qIt)->begin();
            for (; it != (*qIt)->end(); ++it) {
                delete (*it);
            }
            (*qIt)->clear();
            delete (*qIt);
        }
        this->L2SSNPTPQueue->clear();
        delete this->L2SSNPTPQueue;
    }
    /* end of delete SSN */

}

/**
 * Handles notifications received through notification board.
 * @param is fired notification
 * @param detail is user defined pointer
 */
void ISISMain::receiveChangeNotification(cComponent *source, simsignal_t signalID, cObject *details) {
    //TODO A! Make it work again.
    // ignore notifications during initialization
//    if (simulation.getContextType() == CTX_INITIALIZE){
//        return;
//    }
    Enter_Method_Silent();
    if(signalID == isisAdjChangedSignal) {
        /*TODO B3 Make new timer Type and schedule it when you receive ADJ_CHANGED for short period of time (TBD)
         * and increment some internal counter. When the counter hit certain threshold, stop pushing the timer and wait for
         * timeout.
         * Achievement of this is that we bridge the short period of time when all adjacencies are coming up, so we don't
         * have to generate LSP, calculate SPF and such for every ajd go UP.
         * OR maybe set the timer only once so we meet the criterium of minimum time between LSP generation and SPF calculation as specified.
         *
         *
         */
//        ISISadj *adj = check_and_cast<ISISadj*>(details);
        ISISTimer *timer = check_and_cast<ISISTimer*>(details);

        generateLSP(timer);

        if(timer->getIsType() == L1_TYPE) {
            cancelEvent(this->spfL1Timer);
            scheduleAt(simTime(), this->spfL1Timer);
        } else {
            cancelEvent(this->spfL2Timer);
            scheduleAt(simTime(), this->spfL2Timer);

        }

    }
}

/**
 * Initialization function called at the start of simulation. This method provides initial
 * parsing of XML config file and configuration of whole module including network interfaces.
 * NED address is validated and after it's loaded. Initial timers for hello and LSP packets
 * are also set.
 * @see insertIft(InterfaceEntry *entry, cXMLElement *intElement)
 * @see parseNetAddr()
 * @param stage defines stage(step) of global initialization
 */

void ISISMain::initialize(int stage) {
    //interface init at stage 2
    if (stage == INITSTAGE_LOCAL) {

      attIS = nullptr;
        //TODO A! Notification board
//         nb = NotificationBoardAccess().get();
////         nb->subscribe(this, interfaceStateChangedSignal);
//         nb->subscribe(this, NF_ISIS_ADJ_CHANGED);

        ift = getModuleFromPar<IInterfaceTable>(par("interfaceTableModule"), this);
        clnsrt = check_and_cast<CLNSRoutingTable*>(getModuleByPath(par("routingTableModule")));


        deviceType = std::string((const char *) par("deviceType"));
        deviceId = std::string((const char *) par("deviceId"));
//        configFile = std::string((const char *) par("configFile"));
        if (deviceType == "Router") {
            this->mode = L3_ISIS_MODE;
            //register service and protocol
            registerService(Protocol::isis, nullptr, gate("lowerLayerIn"));
            registerProtocol(Protocol::isis, gate("lowerLayerOut"), nullptr);
        } else if (deviceType == "RBridge") {
            this->mode = L2_ISIS_MODE;
            //register service and protocol
            registerService(Protocol::l2isis, nullptr, gate("lowerLayerIn"));
            registerProtocol(Protocol::l2isis, gate("lowerLayerOut"), nullptr);
        } else {
            throw cRuntimeError("Unknown device type for IS-IS module");
        }

    }else if (stage == INITSTAGE_LINK_LAYER)
    {


//        registerService(Protocol::isis, nullptr, gate("trillIn"));
//        registerProtocol(Protocol::isis, gate("trillOut"), nullptr);

    }else if (stage == INITSTAGE_ROUTING_PROTOCOLS) {
        //TODO A! Create ISISDeviceConfigurator class
//        DeviceConfigurator *devConf = ModuleAccess<DeviceConfigurator>("deviceConfigurator").get();
//        devConf->loadISISConfig(this, this->mode);

        ISISDeviceConfigurator* devConf = new ISISDeviceConfigurator(
                par("deviceId"), par("deviceType"), par("configData"), ift);

        devConf->prepareAddress(mode);
        systemId.setSystemId(devConf->getSystemId());

        nickname.set(systemId.toInt());
        areaID.setAreaId(devConf->getAreaId());

        devConf->loadISISConfig(this, this->mode);

        //TODO passive-interface
        //create SRMQueue for each interface (even though it would be used only for broadcast interfaces)
        for (std::vector<ISISinterface>::iterator it = this->ISISIft.begin();
                it != this->ISISIft.end(); ++it) {
            it->l1srmBQueue = new FlagRecQ_t;
            this->L1SRMBQueue->push_back(it->l1srmBQueue);
        }

        for (std::vector<ISISinterface>::iterator it = this->ISISIft.begin();
                it != this->ISISIft.end(); ++it) {
            it->l1srmPTPQueue = new FlagRecQ_t;
            this->L1SRMPTPQueue->push_back(it->l1srmPTPQueue);
        }

        //SSNflags
        for (std::vector<ISISinterface>::iterator it = this->ISISIft.begin();
                it != this->ISISIft.end(); ++it) {
            it->l1ssnBQueue = new FlagRecQ_t;
            this->L1SSNBQueue->push_back(it->l1ssnBQueue);
        }

        for (std::vector<ISISinterface>::iterator it = this->ISISIft.begin();
                it != this->ISISIft.end(); ++it) {
            it->l1ssnPTPQueue = new FlagRecQ_t;
            this->L1SSNPTPQueue->push_back(it->l1ssnPTPQueue);
        }

        //create SRMQueue for each interface (even though it would be used only for broadcast interfaces)
        for (std::vector<ISISinterface>::iterator it = this->ISISIft.begin();
                it != this->ISISIft.end(); ++it) {
            it->l2srmBQueue = new FlagRecQ_t;
            this->L2SRMBQueue->push_back(it->l2srmBQueue);
        }

        for (std::vector<ISISinterface>::iterator it = this->ISISIft.begin();
                it != this->ISISIft.end(); ++it) {
            it->l2srmPTPQueue = new FlagRecQ_t;
            this->L2SRMPTPQueue->push_back(it->l2srmPTPQueue);
        }

        //SSNflags
        for (std::vector<ISISinterface>::iterator it = this->ISISIft.begin();
                it != this->ISISIft.end(); ++it) {
            it->l2ssnBQueue = new FlagRecQ_t;
            this->L2SSNBQueue->push_back(it->l2ssnBQueue);
        }

        for (std::vector<ISISinterface>::iterator it = this->ISISIft.begin();
                it != this->ISISIft.end(); ++it) {
            it->l2ssnPTPQueue = new FlagRecQ_t;
            this->L2SSNPTPQueue->push_back(it->l2ssnPTPQueue);
        }

//        //TODO A3
//        this->L1SPFFullInterval = ISIS_SPF_FULL_INTERVAL;
//        this->L2SPFFullInterval = ISIS_SPF_FULL_INTERVAL;



//        RegisterTransportProtocolCommand *message =   new RegisterTransportProtocolCommand();
//        message->setProtocol(1234);
//        send(message, "lowerLayerOut");

        //        this->initISIS();
    }else if (stage == INITSTAGE_LAST){
        ISISTimer *timerMsg = new ISISTimer("ISIS Start", ISIS_START_TIMER);
        timerMsg->setTimerKind(ISIS_START_TIMER);
        this->schedule(timerMsg);

    }

}
//
///**
// * Set initial parameters of network interfaces.
// * @param entry Pointer to interface record in interfaceTable
// * @param intElement XML element of current interface in XML config file
// */
//void ISIS::insertIft(InterfaceEntry *entry, cXMLElement *intElement)
//{
//
//    if (intElement == NULL)
//    {
//        return;
//    }
//    ISISinterface newIftEntry;
//    newIftEntry.interfaceId = entry->getInterfaceId();
//
//    newIftEntry.interfaceId = entry->getNetworkLayerGateIndex();
//    EV<< "deviceId: " << this->deviceId << "ISIS: adding interface, interfaceId: " << newIftEntry.interfaceId << endl;
//
//    //set interface priority
//    newIftEntry.priority = ISIS_DIS_PRIORITY; //default value
//
//    /* Interface is NOT enabled by default. If ANY IS-IS related property is configured on interface then it's enabled. */
//    newIftEntry.ISISenabled = false;
//
//    cXMLElement *priority = intElement->getFirstChildWithTag("ISIS-Priority");
//    if (priority != NULL && priority->getNodeValue() != NULL)
//    {
//        newIftEntry.priority = (unsigned char) atoi(priority->getNodeValue());
//        newIftEntry.ISISenabled = true;
//    }
//
//    //set network type (point-to-point vs. broadcast)
//
//    newIftEntry.network = true; //default value
//
//    cXMLElement *network = intElement->getFirstChildWithTag("ISIS-Network");
//    if (network != NULL && network->getNodeValue() != NULL)
//    {
//        if (!strcmp(network->getNodeValue(), "point-to-point"))
//        {
//            newIftEntry.network = false;
//            EV<< "Interface network type is point-to-point " << network->getNodeValue() << endl;
//        }
//        else if (!strcmp(network->getNodeValue(), "broadcast"))
//        {
//            EV << "Interface network type is broadcast " << network->getNodeValue() << endl;
//        }
//        else
//        {
//            EV << "ERORR: Unrecognized interface's network type: " << network->getNodeValue() << endl;
//
//        }
//        newIftEntry.ISISenabled = true;
//
//    }
//
//            //set interface metric
//
//    newIftEntry.metric = ISIS_METRIC; //default value
//
//    cXMLElement *metric = intElement->getFirstChildWithTag("ISIS-Metric");
//    if (metric != NULL && metric->getNodeValue() != NULL)
//    {
//        newIftEntry.metric = (unsigned char) atoi(metric->getNodeValue());
//        newIftEntry.ISISenabled = true;
//    }
//
//    //set interface type according to global router configuration
//    switch (this->isType)
//        {
//        case (L1_TYPE):
//            newIftEntry.circuitType = L1_TYPE;
//            break;
//        case (L2_TYPE):
//            newIftEntry.circuitType = L2_TYPE;
//            break;
//            //if router is type is equal L1L2, then interface configuration sets the type
//        default: {
//
//            newIftEntry.circuitType = L1L2_TYPE;
//
//            cXMLElement *circuitType = intElement->getFirstChildWithTag("ISIS-Circuit-Type");
//            if (circuitType != NULL && circuitType->getNodeValue() != NULL)
//            {
//                if (strcmp(circuitType->getNodeValue(), "L2") == 0)
//                {
//                    newIftEntry.circuitType = L2_TYPE;
//                }
//                else
//                {
//                    if (strcmp(circuitType->getNodeValue(), "L1") == 0)
//                        newIftEntry.circuitType = L1_TYPE;
//                }
//                newIftEntry.ISISenabled = true;
//            }
//            else
//            {
//                newIftEntry.circuitType = L1L2_TYPE;
//            }
//
//            break;
//        }
//        }
//
//    //set L1 hello interval in seconds
//    cXMLElement *L1HelloInt = intElement->getFirstChildWithTag("ISIS-L1-Hello-Interval");
//    if (L1HelloInt == NULL || L1HelloInt->getNodeValue() == NULL)
//    {
//        newIftEntry.L1HelloInterval = this->L1HelloInterval;
//    }
//    else
//    {
//        newIftEntry.L1HelloInterval = atoi(L1HelloInt->getNodeValue());
//    }
//
//    //set L1 hello multiplier
//    cXMLElement *L1HelloMult = intElement->getFirstChildWithTag("ISIS-L1-Hello-Multiplier");
//    if (L1HelloMult == NULL || L1HelloMult->getNodeValue() == NULL)
//    {
//        newIftEntry.L1HelloMultiplier = this->L1HelloMultiplier;
//    }
//    else
//    {
//        newIftEntry.L1HelloMultiplier = atoi(L1HelloMult->getNodeValue());
//    }
//
//    //set L2 hello interval in seconds
//    cXMLElement *L2HelloInt = intElement->getFirstChildWithTag("ISIS-L2-Hello-Interval");
//    if (L2HelloInt == NULL || L2HelloInt->getNodeValue() == NULL)
//    {
//        newIftEntry.L2HelloInterval = this->L2HelloInterval;
//    }
//    else
//    {
//        newIftEntry.L2HelloInterval = atoi(L2HelloInt->getNodeValue());
//    }
//
//    //set L2 hello multiplier
//    cXMLElement *L2HelloMult = intElement->getFirstChildWithTag("ISIS-L2-Hello-Multiplier");
//    if (L2HelloMult == NULL || L2HelloMult->getNodeValue() == NULL)
//    {
//        newIftEntry.L2HelloMultiplier = this->L2HelloMultiplier;
//    }
//    else
//    {
//        newIftEntry.L2HelloMultiplier = atoi(L2HelloMult->getNodeValue());
//    }
//
//    //set lspInterval
//    cXMLElement *cxlspInt = intElement->getFirstChildWithTag("ISIS-LSP-Interval");
//    if (cxlspInt == NULL || cxlspInt->getNodeValue() == NULL)
//    {
//        newIftEntry.lspInterval = ISIS_LSP_INTERVAL;
//    }
//    else
//    {
//        newIftEntry.lspInterval = atoi(cxlspInt->getNodeValue());
//    }
//
//    //set L1CsnpInterval
//    cXMLElement *cxL1CsnpInt = intElement->getFirstChildWithTag("ISIS-L1-CSNP-Interval");
//    if (cxL1CsnpInt == NULL || cxL1CsnpInt->getNodeValue() == NULL)
//    {
//        newIftEntry.L1CsnpInterval = ISIS_CSNP_INTERVAL;
//    }
//    else
//    {
//        newIftEntry.L1CsnpInterval = atoi(cxL1CsnpInt->getNodeValue());
//    }
//
//    //set L2CsnpInterval
//    cXMLElement *cxL2CsnpInt = intElement->getFirstChildWithTag("ISIS-L2-CSNP-Interval");
//    if (cxL2CsnpInt == NULL || cxL2CsnpInt->getNodeValue() == NULL)
//    {
//        newIftEntry.L2CsnpInterval = ISIS_CSNP_INTERVAL;
//    }
//    else
//    {
//        newIftEntry.L2CsnpInterval = atoi(cxL2CsnpInt->getNodeValue());
//    }
//
//    //set L1PsnpInterval
//    cXMLElement *cxL1PsnpInt = intElement->getFirstChildWithTag("ISIS-L1-PSNP-Interval");
//    if (cxL1PsnpInt == NULL || cxL1PsnpInt->getNodeValue() == NULL)
//    {
//        newIftEntry.L1PsnpInterval = ISIS_CSNP_INTERVAL;
//    }
//    else
//    {
//        newIftEntry.L1PsnpInterval = atoi(cxL1PsnpInt->getNodeValue());
//    }
//
//    //set L2PsnpInterval
//    cXMLElement *cxL2PsnpInt = intElement->getFirstChildWithTag("ISIS-L2-PSNP-Interval");
//    if (cxL2PsnpInt == NULL || cxL2PsnpInt->getNodeValue() == NULL)
//    {
//        newIftEntry.L2PsnpInterval = ISIS_CSNP_INTERVAL;
//    }
//    else
//    {
//        newIftEntry.L2PsnpInterval = atoi(cxL2PsnpInt->getNodeValue());
//    }
//
//    // priority is not needed for point-to-point, but it won't hurt
//    // set priority of current DIS = me at start
//    newIftEntry.L1DISpriority = newIftEntry.priority;
//    newIftEntry.L2DISpriority = newIftEntry.priority;
//
//    //set initial designated IS as himself
//    newIftEntry.L1DIS.set(systemId, newIftEntry.interfaceId + 1);
////    this->copyArrayContent((unsigned char*) this->sysId, newIftEntry.L1DIS, ISIS_SYSTEM_ID, 0, 0);
////    //set LAN identifier; -99 is because, OMNeT starts numbering interfaces from 100 -> interfaceID 100 means LAN ID 0; and we want to start numbering from 1
////    //newIftEntry.L1DIS[6] = entry->getInterfaceId() - 99;
////    newIftEntry.L1DIS[ISIS_SYSTEM_ID] = newIftEntry.interfaceId + 1;
//    //do the same for L2 DIS
//    newIftEntry.L2DIS.set(systemId, newIftEntry.interfaceId + 1);
////    this->copyArrayContent((unsigned char*) this->sysId, newIftEntry.L2DIS, ISIS_SYSTEM_ID, 0, 0);
////    //newIftEntry.L2DIS[6] = entry->getInterfaceId() - 99;
////    newIftEntry.L2DIS[ISIS_SYSTEM_ID] = newIftEntry.interfaceId + 1;
//
//    newIftEntry.passive = false;
//    newIftEntry.entry = entry;
//    this->ISISIft.push_back(newIftEntry);
//}

/**
 * Initiate scheduling timers.
 */
void ISISMain::initISIS() {



    if (this->mode == ISISMain::L3_ISIS_MODE) {
        //TODO ANSAINET4.0 Uncomment; replaced by message dispatcher?
//      RegisterTransportProtocolCommand *message =   new RegisterTransportProtocolCommand();
//      message->setProtocol(1234);
//      send(message, "lowerLayerOut");
        this->initHello();

    } else {
        this->initTRILLHello();
    }
    this->initGenerate();
    this->initRefresh(); //this could be called after at least one adjcency becomes UP
    this->initPeriodicSend();
    this->initCsnp(); //this could be called within initRefresh();
    this->initPsnp(); //see above
    this->initSPF();

}

/*
 * Initiate scheduling Hello timers.
 */
void ISISMain::initHello() {
    ISISTimer *timerMsg;
    ISISinterface *iface;
    EV << "ISIS: initHello()" << endl;
    for (unsigned int k = 0; k < this->ISISIft.size(); k++) {
        //don't schedule Hello message on Loopback interfaces
        if (this->ISISIft.at(k).entry->isLoopback()) {
            continue;
        }

        //schedule Hello timer per level => check if L1L2 on broadcast => schedule two timers
        //on PTP is L1L2 Hello valid timer
        iface = &(this->ISISIft.at(k));

        //don't schedule sending hello PDU on passive or not ISIS-enabled interface
        if (!iface->ISISenabled || iface->passive) {
            continue;
        }

        if (iface->network && iface->circuitType == L1L2_TYPE) {
            timerMsg = new ISISTimer("Hello_L1_timer");
            timerMsg->setTimerKind(HELLO_TIMER);
            timerMsg->setIsType(L1_TYPE);
            timerMsg->setInterfaceId(iface->entry->getInterfaceId());
//            timerMsg->setGateIndex(iface->e);
            this->schedule(timerMsg);

            timerMsg = new ISISTimer("Hello_L2_timer");
            timerMsg->setTimerKind(HELLO_TIMER);
            timerMsg->setIsType(L2_TYPE);
            timerMsg->setInterfaceId(iface->entry->getInterfaceId());
//            timerMsg->setGateIndex(iface->gateIndex);
            this->schedule(timerMsg);

        } else {
            timerMsg = new ISISTimer("Hello_timer");
            timerMsg->setTimerKind(HELLO_TIMER);
            timerMsg->setIsType(iface->circuitType);
            timerMsg->setInterfaceId(iface->entry->getInterfaceId());
//            timerMsg->setGateIndex(iface->gateIndex);
            this->schedule(timerMsg);

        }
    }
}

/*
 * Initiate scheduling TRILL Hello timers.
 */
void ISISMain::initTRILLHello() {
    ISISTimer *timerMsg;
    ISISinterface *iface;
    EV << "ISIS: initTRILLHello()" << endl;
    for (unsigned int k = 0; k < this->ISISIft.size(); k++) {
        //don't schedule Hello message on Loopback interfaces
        if (this->ISISIft.at(k).entry->isLoopback()) {
            continue;
        }

        //schedule Hello timer per level => check if L1L2 on broadcast => schedule two timers
        //on PTP is L1L2 Hello valid timer
        iface = &(this->ISISIft.at(k));

        //don't schedule sending hello PDU on passive or not ISIS-enabled interface
        if (!iface->ISISenabled || iface->passive) {
            continue;
        }

        timerMsg = new ISISTimer("TRILL_HELLO_timer");
        timerMsg->setTimerKind(TRILL_HELLO_TIMER);
        timerMsg->setIsType(iface->circuitType);
        timerMsg->setInterfaceId(iface->entry->getInterfaceId());
//        timerMsg->setGateIndex(iface->gateIndex);
        scheduleAt(simTime(), timerMsg);
//        this->schedule(timerMsg);

    }
}

/*
 * Initial schedule of timer for generating LSPs
 */
void ISISMain::initGenerate() {
    ISISTimer *timerMsg;

    if (this->isType == L1L2_TYPE) {
        timerMsg = new ISISTimer("Generate LSPs timer");
        timerMsg->setTimerKind(GENERATE_LSP_TIMER);
        timerMsg->setIsType(L1_TYPE);
        this->genL1LspTimer = timerMsg;
        this->schedule(timerMsg);

        timerMsg = new ISISTimer("Generate LSPs timer");
        timerMsg->setTimerKind(GENERATE_LSP_TIMER);
        timerMsg->setIsType(L2_TYPE);
        this->genL2LspTimer = timerMsg;
        this->schedule(timerMsg);

    } else {
        timerMsg = new ISISTimer("Generate LSPs timer");
        timerMsg->setTimerKind(GENERATE_LSP_TIMER);
        timerMsg->setIsType(this->isType);
        if (this->isType == L1_TYPE) {
            this->genL1LspTimer = timerMsg;
        } else {
            this->genL2LspTimer = timerMsg;
        }
        this->schedule(timerMsg);

    }
    EV << "ISIS: initGenerate()" << endl;

}
/*
 * Initial schedule of timer for refreshing LSPs
 */
void ISISMain::initRefresh() {
    ISISTimer *timerMsg;

    timerMsg = new ISISTimer("Refresh LSPs timer");
    timerMsg->setTimerKind(LSP_REFRESH_TIMER);
    timerMsg->setIsType(this->isType);
    this->schedule(timerMsg);
    EV << "ISIS: initRefresh()" << endl;

}

/*
 * Initial schedule of timer for periodic sending LSPs.
 */
void ISISMain::initPeriodicSend() {
    ISISTimer *timerMsg;

    if (this->isType == L1L2_TYPE) {
        timerMsg = new ISISTimer("Periodic send");
        timerMsg->setTimerKind(PERIODIC_SEND_TIMER);
        timerMsg->setIsType(L1_TYPE);
        this->periodicL1Timer = timerMsg;
        this->schedule(timerMsg);

        timerMsg = new ISISTimer("Periodic send");
        timerMsg->setTimerKind(PERIODIC_SEND_TIMER);
        timerMsg->setIsType(L2_TYPE);
        this->periodicL2Timer = timerMsg;
        this->schedule(timerMsg);
    } else {
        timerMsg = new ISISTimer("Periodic send");
        timerMsg->setTimerKind(PERIODIC_SEND_TIMER);
        timerMsg->setIsType(this->isType);
        if (this->isType == L1_TYPE) {
            this->periodicL1Timer = timerMsg;
        } else {
            this->periodicL2Timer = timerMsg;
        }
        this->schedule(timerMsg);
    }
}

/*
 * Initial schedule of timer for sending CSNP.
 */
void ISISMain::initCsnp() {

    ISISTimer *timerMsg;
    ISISinterface *iface;
    for (unsigned int k = 0; k < this->ISISIft.size(); k++) {
        iface = &(this->ISISIft.at(k));
        //don't schedule Hello message on Loopback interfaces
        if (this->ISISIft.at(k).entry->isLoopback()) {
            continue;
        }

        //don't schedule sending CSNP PDU on passive or not ISIS-enabled interface
        if (!iface->ISISenabled || iface->passive) {
            continue;
        }

        if (iface->network && iface->circuitType == L1L2_TYPE) {
            timerMsg = new ISISTimer("CSNP L1");
            timerMsg->setTimerKind(CSNP_TIMER);
            timerMsg->setIsType(L1_TYPE);
            timerMsg->setInterfaceId(iface->entry->getInterfaceId());
//            timerMsg->setGateIndex(iface->gateIndex);
            this->schedule(timerMsg);

            timerMsg = new ISISTimer("CSNP L2");
            timerMsg->setTimerKind(CSNP_TIMER);
            timerMsg->setIsType(L2_TYPE);
            timerMsg->setInterfaceId(iface->entry->getInterfaceId());
//            timerMsg->setGateIndex(iface->gateIndex);
            this->schedule(timerMsg);

        } else {
            timerMsg = new ISISTimer("CSNP");
            timerMsg->setTimerKind(CSNP_TIMER);
            timerMsg->setIsType(iface->circuitType);
            timerMsg->setInterfaceId(iface->entry->getInterfaceId());
//            timerMsg->setGateIndex(iface->gateIndex);
            this->schedule(timerMsg);

        }
    }
}

/*
 * Initial schedule of timer for sending PSNP.
 */
void ISISMain::initPsnp() {

    ISISTimer *timerMsg;
    ISISinterface *iface;
    for (unsigned int k = 0; k < this->ISISIft.size(); k++) {
        iface = &(this->ISISIft.at(k));

        //don't schedule Hello message on Loopback interfaces
        if (this->ISISIft.at(k).entry->isLoopback()) {
            continue;
        }

        //don't schedule sending PSNP on passive or not ISIS-enabled interface
        if (!iface->ISISenabled || iface->passive) {
            continue;
        }

        if (iface->network && iface->circuitType == L1L2_TYPE) {
            timerMsg = new ISISTimer("PSNP L1");
            timerMsg->setTimerKind(PSNP_TIMER);
            timerMsg->setIsType(L1_TYPE);
            timerMsg->setInterfaceId(iface->entry->getInterfaceId());
//            timerMsg->setGateIndex(iface->gateIndex);
            this->schedule(timerMsg);

            timerMsg = new ISISTimer("PSNP L2");
            timerMsg->setTimerKind(PSNP_TIMER);
            timerMsg->setIsType(L2_TYPE);
            timerMsg->setInterfaceId(iface->entry->getInterfaceId());
//            timerMsg->setGateIndex(iface->gateIndex);
            this->schedule(timerMsg);

        } else {
            timerMsg = new ISISTimer("PSNP");
            timerMsg->setTimerKind(PSNP_TIMER);
            timerMsg->setIsType(iface->circuitType);
            timerMsg->setInterfaceId(iface->entry->getInterfaceId());
//            timerMsg->setGateIndex(iface->gateIndex);
            this->schedule(timerMsg);

        }
    }
}

/*
 * Initial schedule of timer for computing shortest paths.
 */
void ISISMain::initSPF() {
    ISISTimer *timerMsg;

    if (this->isType == L1L2_TYPE) {
        timerMsg = new ISISTimer("L1 SPF Full");
        timerMsg->setTimerKind(SPF_FULL_TIMER);
        timerMsg->setIsType(L1_TYPE);
        this->spfL1Timer = timerMsg;
        this->schedule(timerMsg);

        timerMsg = new ISISTimer("L2 SPF Full");
        timerMsg->setTimerKind(SPF_FULL_TIMER);
        timerMsg->setIsType(L2_TYPE);
        this->spfL2Timer = timerMsg;
        this->schedule(timerMsg);
    } else {
        timerMsg = new ISISTimer("SPF Full");
        timerMsg->setTimerKind(SPF_FULL_TIMER);
        timerMsg->setIsType(this->isType);
        if (this->isType == L1_TYPE) {
            this->spfL1Timer = timerMsg;
        } else {
            this->spfL2Timer = timerMsg;
        }
        this->schedule(timerMsg);
    }
}

/**
 * Handle incoming messages: Method differs between self messages and external messages
 * and executes appropriate function.
 * @param msg incoming message
 */
void ISISMain::handleMessage(cMessage* msg) {

    if (msg->isSelfMessage()) {
        ISISTimer *timer = (ISISTimer *)(msg);
        switch (timer->getTimerKind()) {
        case (ISIS_START_TIMER):
            this->initISIS();
            delete timer;
            break;

        case (HELLO_TIMER):
            this->sendHelloMsg(timer);
            break;

        case (TRILL_HELLO_TIMER):
            this->sendTRILLHelloMsg(timer);
            break;

        case (NEIGHBOUR_DEAD_TIMER):
            this->removeDeadNeighbour(timer);
            delete timer;
            break;

        case (GENERATE_LSP_TIMER):
            this->generateLSP(timer);
            break;

        case (LSP_REFRESH_TIMER):
            this->refreshLSP(timer);
            break;

        case (LSP_DEAD_TIMER):
            this->purgeLSP(timer->getLSPid(), timer->getIsType());
            // don't delete timer, it's re-used for LSP_DELETE_TIMER
            //delete timer;
            break;

        case (LSP_DELETE_TIMER):
            this->deleteLSP(timer);
            this->drop(timer);
            delete timer;
            break;

        case (CSNP_TIMER):
            if (timer->getIsType() == L1L2_TYPE) {
                EV << "ISIS: Warning: Discarding CSNP_TIMER for L1L2." << endl;
                delete timer;
            } else {
                this->sendCsnp(timer);
            }
            break;

        case (PSNP_TIMER):
            if (timer->getIsType() == L1L2_TYPE) {
                EV << "ISIS: Warning: Discarding PSNP_TIMER for L1L2." << endl;
                delete timer;
            } else {
                this->sendPsnp(timer);
            }
            break;

        case (PERIODIC_SEND_TIMER):
            this->periodicSend(timer, timer->getIsType());
            break;

        case (SPF_FULL_TIMER):
            this->fullSPF(timer);
            break;

        default:
            EV
                      << "ISIS: Warning: Received unsupported Timer type in handleMessage"
                      << endl;
            delete timer;
            break;
        }
    } else {

        //TODO externalDomain check
        //TODO every (at least all Hello) message should be checked for matching system-ID length
        cPacket *cpacket = check_and_cast<cPacket *>(msg);
        Packet *packet = check_and_cast<Packet*>(cpacket);

        packet->trimFront(); // why though?
        const Ptr<const ISISMessage> inMsg = packet->peekAtFront<ISISMessage>();

        if (!this->isMessageOK(inMsg)) {
            EV << "ISIS: Warning: discarding message" << endl;
            //TODO schedule event discarding message
            delete msg;
            return;
        }

        //get arrival interface
        int interfaceId = packet->findTag<InterfaceInd>()->getInterfaceId();

        ISISinterface * tmpIntf = this->getIfaceById(interfaceId);
        if (tmpIntf == NULL) {
            EV
                      << "ISIS: ERROR: Couldn't find interface by gate index when ISIS::handleMessage"
                      << endl;
            delete msg;
            return;
        }

        //get circuit type for arrival interface
        short circuitType = tmpIntf->circuitType;

        /* Usually we shouldn't need to check matching circuit type with arrived message
         * and these messages should be filtered based on destination MAC address.
         * Since we use broadcast MAC address, IS(router) cannot determine if it's
         * for ALL L1 or ALL L2 systems. Therefore we need to check manually.
         * If appropriate Level isn't enabled on interface, then the message is discarded.
         */

        switch (inMsg->getType()) {
        case (LAN_L1_HELLO):
            if (circuitType == L1_TYPE || circuitType == L1L2_TYPE) {
                this->handleL1HelloMsg(packet);

                //comment if printing of the adjacency table is too disturbing
                this->printAdjTable();

            } else {
                EV << "deviceId " << deviceId
                          << ": ISIS: WARNING: Discarding LAN_L1_HELLO message on unsupported circuit type interface "
                          << packet->getId() << endl;

            }

        delete msg;
            break;

        case (LAN_L2_HELLO):
            if (circuitType == L2_TYPE || circuitType == L1L2_TYPE) {
                this->handleL2HelloMsg(packet);
                this->printAdjTable();

            } else {
                EV << "deviceId " << deviceId
                          << ": ISIS: WARNING: Discarding LAN_L2_HELLO message on unsupported circuit type interface "
                          << packet->getId() << endl;
            }

        delete msg;
            break;

        case (PTP_HELLO):
            //On PTP link process all Hellos
            if (circuitType != RESERVED_TYPE) {
                this->handlePTPHelloMsg(packet);
                this->printAdjTable();
            } else {
                EV << "deviceId " << deviceId
                          << ": ISIS: WARNING: Discarding PTP_HELLO message. Received RESERVED_TYPE circuit type "
                          << packet->getId() << endl;
            }

            delete msg;
            break;

        case (TRILL_HELLO):

            if (circuitType != RESERVED_TYPE) {
                this->handleTRILLHelloMsg(packet);
                this->printAdjTable();
            } else {
                EV << "deviceId " << deviceId
                          << ": ISIS: WARNING: Discarding PTP_HELLO message. Received RESERVED_TYPE circuit type "
                          << packet->getId() << endl;
            }

        delete msg;
            break;

        case (L1_LSP):
            if (circuitType == L1_TYPE || circuitType == L1L2_TYPE) {

                this->handleLsp(packet);

                //comment if printing of the link-state database is too disturbing
                this->printLSPDB();
            } else {
                EV << "deviceId " << deviceId
                          << ": ISIS: WARNING: Discarding L1_LSP message on unsupported circuit type interface "
                          << packet->getId() << endl;
            }
            //delete inMsg; //TODO don't delete inMsg in new version

            break;

        case (L2_LSP):
            if (circuitType == L2_TYPE || circuitType == L1L2_TYPE) {

                this->handleLsp(packet);

                //comment if printing of the link-state database is too disturbing
                this->printLSPDB();
            } else {
                EV << "deviceId " << deviceId
                          << ": ISIS: WARNING: Discarding L1_LSP message on unsupported circuit type interface "
                          << packet->getId() << endl;
            }
            //delete inMsg; //TODO don't delete inMsg in new version

            break;

        case (L1_CSNP):
            if (circuitType == L1_TYPE || circuitType == L1L2_TYPE) {
                //this->handleL1CSNP(inMsg);
                this->handleCsnp(packet);
                this->printLSPDB();
            } else {
                EV << "deviceId " << deviceId
                          << ": ISIS: WARNING: Discarding L1_CSNP message on unsupported circuit type interface\n"
                          << packet->getId() << endl;
            }
            //delete inMsg;
            break;

        case (L2_CSNP):
            if (circuitType == L2_TYPE || circuitType == L1L2_TYPE) {
                //this->handleL1CSNP(inMsg);
                this->handleCsnp(packet);
                this->printLSPDB();
            } else {
                EV << "deviceId " << deviceId
                          << ": ISIS: WARNING: Discarding L2_CSNP message on unsupported circuit type interface\n"
                          << packet->getId() << endl;
            }
            //delete inMsg;
            break;

        case (L1_PSNP):
            if (circuitType == L1_TYPE || circuitType == L1L2_TYPE) {
                this->handlePsnp(packet);
                this->printLSPDB();
                //delete inMsg;
            }
            break;

        case (L2_PSNP):
            if (circuitType == L2_TYPE || circuitType == L1L2_TYPE) {
                this->handlePsnp(packet);
                this->printLSPDB();
                //delete inMsg;
            }
            break;

        default:
            EV << "deviceId " << deviceId
                      << ": ISIS: WARNING: Discarding unknown message type. Msg id: "
                      << packet->getId() << endl;
            //TODO ANSAINET4.0 Uncomment delete?
//            delete inMsg;
            break;

        }
    }
}

/**
 * Create hello packet and send it out to specified interface. This method handle
 * LAN hello and PTP hello packets. Destination MAC address is broadcast (ff:ff:ff:ff:ff:ff).
 * @param timer is timer that triggered this action
 */
void ISISMain::sendHelloMsg(ISISTimer* timer) {
    if (this->getIfaceById(timer->getInterfaceId())->network) {
//        EV<< "ISIS: sendingBroadcastHello: " << endl;
        this->sendBroadcastHelloMsg(timer->getInterfaceId(), timer->getIsType());

    } else {
//        EV << "ISIS: sendingPTPHello: " << endl;
        this->sendPTPHelloMsg(timer->getInterfaceId(), timer->getIsType());
    }
    //re-schedule timer
    this->schedule(timer);

}

/*
 * Send hello message on specified broadcast interface.
 * Packets contain IS_NEIGHBOURS_HELLO and AREA_ADDRESS TLVs.
 * @param k is interface index (index to ISISIft)
 * @param circuitType is circuit type of specified interface.
 */
void ISISMain::sendBroadcastHelloMsg(int interfaceId, short circuitType) {

    /*
     * TODO B1
     * Hellos are scheduled per interface per level, so only one Hello Packet
     * should be created and then sent.
     * Appropriate level should be based on circuitType from timer and not from
     * interface's circuitType.
     */
//    unsigned int tlvSize;
    PseudonodeID disID;
    ISISinterface *iface = this->getIfaceById(interfaceId);

    if (iface->passive || !iface->ISISenabled)
    {
        return;
    }

    // create L1 and L2 hello packets

    /*ISISL1HelloPacket *helloL1 = new ISISL1HelloPacket("L1 Hello");
     ISISL2HelloPacket *helloL2 = new ISISL2HelloPacket("L2 Hello");
     */
    Packet *packet = new Packet("ISIS LAN Hello");

    auto hello = makeShared<ISISLANHelloPacket>();
    //set appropriate destination MAC addresses
    MacAddress ma;

    if (circuitType == L1_TYPE) {
        hello->setType(LAN_L1_HELLO);
        disID = PseudonodeID(iface->L1DIS);
        ma.setAddress(ISIS_ALL_L1_IS);
    } else if (circuitType == L2_TYPE) {
        hello->setType(LAN_L2_HELLO);
        disID = PseudonodeID(iface->L2DIS);
        ma.setAddress(ISIS_ALL_L2_IS);
    } else {
        EV << "ISIS: Warning: Sending LAN Hello" << endl;
        return;
    }

    hello->setCircuitType(circuitType);


    hello->setSourceID(systemId);

//    // set DSAP & NSAP fields
    auto sapTag = packet->addTag<Ieee802SapReq>();
    sapTag->setDsap(SAP_CLNS);
    sapTag->setSsap(SAP_CLNS);

    packet->addTag<PacketProtocolTag>()->setProtocol(&Protocol::isis);

    auto macTag = packet->addTag<MacAddressReq>();
    macTag->setDestAddress(ma);

    //set interfaceID
    auto interfaceTag = packet->addTag<InterfaceReq>();
    interfaceTag->setInterfaceId(iface->entry->getInterfaceId());


    //set TLVs
//    TLV_t myTLV;
    //helloL1->setTLVArraySize(0);

    //set area address
    this->addTLV(hello, AREA_ADDRESS, circuitType);

    //set NEIGHBOURS
    this->addTLV(hello, IS_NEIGHBOURS_HELLO, circuitType, interfaceId);

    //TODO PADDING TLV is omitted
    //TODO Authentication TLV
    //TODO add eventually another TLVs (eg. from RFC 1195)
    //don't send hello packets from passive interfaces
    if (!iface->passive && iface->ISISenabled) {
        // if this interface is DIS for LAN, hellos are sent 3-times faster (3.33sec instead of 10.0)
        // decision is made according to global hello counter (dirty hax - don't blame me pls, but i don't have time to code it nice way :)

        //set LAN ID field (DIS-ID)
        hello->setLanID(disID);

        hello->setPriority(iface->priority);

        packet->insertAtFront(hello);
        sendDown(packet);

//        EV<< "'devideId :" << deviceId << " ISIS: L1 Hello packet was sent from " << iface->entry->getName() << "\n";
        EV << "ISIS::sendLANHello: Source-ID: " << systemId;
        EV << " DIS: " << disID;

        EV << endl;

    }
}

/*
 * Sends hello message to specified PtP interface.
 * Packets contain IS_NEIGHBOURS_HELLO and AREA_ADDRESS TLVs.
 * @param interfaceId is interface index (index to ISISIft)
 * @param circuitType is circuit type of specified interface.
 */
void ISISMain::sendPTPHelloMsg(int interfaceId, short circuitType) {

//    unsigned int tlvSize;
    ISISinterface *iface = getIfaceById(interfaceId);
    //don't send hello packets from passive interfaces
    if (iface->passive || !iface->ISISenabled) {
        return;
    }

    Packet *packet = new Packet("PTP Hello");
    auto ptpHello = makeShared<ISISPTPHelloPacket>();

    //TODO change to appropriate layer-2 protocol
//    // set DSAP & NSAP fields
    auto sapTag = packet->addTag<Ieee802SapReq>();
    sapTag->setDsap(SAP_CLNS);
    sapTag->setSsap(SAP_CLNS);

    packet->addTag<PacketProtocolTag>()->setProtocol(&Protocol::isis);

    //set appropriate destination MAC addresses
    MacAddress ma;
//
    if (iface->circuitType == L1_TYPE)
    {
        ma.setAddress(ISIS_ALL_L1_IS);
    }
    else
    {
        ma.setAddress(ISIS_ALL_L2_IS);
    }


    auto macTag = packet->addTagIfAbsent<MacAddressReq>();
    macTag->setDestAddress(ma);

    auto interfaceTag = packet->addTagIfAbsent<InterfaceReq>();
    interfaceTag->setInterfaceId(iface->entry->getInterfaceId());

    //type


    //circuitType
    ptpHello->setCircuitType(iface->circuitType);

    //sourceID
    //set source id
    ptpHello->setSourceID(systemId);


    //holdTime
    //set holdTime
    ptpHello->setHoldTime(
            this->getHoldTime(interfaceId, iface->circuitType));

    //pduLength

    //localCircuitID
    ptpHello->setLocalCircuitID(iface->interfaceId);

    //TLV[]
    //set TLVs
//    TLV_t myTLV;

    //set area address
    this->addTLV(ptpHello, AREA_ADDRESS, circuitType);

    //ptp adjacency state TLV #240
    this->addTLV(ptpHello, PTP_HELLO_STATE, circuitType, interfaceId);

    //TODO TLV #129 Protocols supported

//    this->send(ptpHello, "lowerLayerOut", iface->interfaceId);

    packet->insertAtFront(ptpHello);
        this->sendDown(packet);

}

/******************/
/*   TRILL Hellos */
/******************/
/**
 * Generates TRILL hellos and places them in TrillInterfaceData
 * @param interfaceId is interface id in interfacetable
 * @param circuitType is circuit type of interface with @param interfaceId
 */
void ISISMain::genTRILLHello(int interfaceId, ISISCircuitType circuitType) {

    PseudonodeID disId;
    InterfaceEntry *ie = ift->getInterfaceById(interfaceId);

    IsisInterfaceData *d = ie->getProtocolData<IsisInterfaceData>();
    ISISinterface *iface = this->getIfaceById(interfaceId);

    if (d->isHelloValid()) { //TODO A1 remove true
                             //nothing has changed since last Hello generating
        return;
    }
    //clear previously generated Hellos
    d->clearHello();

    std::vector<TLV_t *>* tlvTable = new std::vector<TLV_t *>;

    /* TODO MUST appear in every TRILL-Hello
     * 1. VLAN ID of the Designated VLAN
     * 2. A copy of the Outer.VLAN ID with which the Hello was tagged on sending.
     * 3. 16-bit port-ID (interfaceId)
     * 4. Nickname of the sending RBridge
     * 5. Two flags
     *    detected VLAN mapping
     *    believe it is appointed forwarder for the VLAN and port on which the TRILL-Hello was sent.
     *
     * ** MAY appear **
     * 1. set of VLANs for which end-station is enabled on the port
     * 2. Several flags
     *    sender's port is configured as access port
     *    sender's port is configured as trunk port
     *    bypass pseudonode
     * 3. If the sender is DRB, the RBridges (excluding etself) that it appoints as forwarder for that link and the VLANs
     *    for which it appoints them. Not all the appointment information need to be included in all Hellos.
     *    Its absence means continue as previously configured.
     * 4. TRILL neighbor list (TLV_TRILL_NEIGHBOR)
     */

    //TLV AREA ADDRESS (I'm not ever sure it MUST be present)
    this->addTLV(tlvTable, AREA_ADDRESS, circuitType);

    //TLV TLV_MT_PORT_CAP
    this->addTLV(tlvTable, TLV_MT_PORT_CAP, circuitType, ie);

    //TLV TLV_TRILL_NEIGHBOR
    this->addTLV(tlvTable, TLV_TRILL_NEIGHBOR, circuitType, ie);

    TLV_t *tmpTlv;
//    unsigned tlvSize;
    unsigned int availableSpace = ISIS_TRILL_MAX_HELLO_SIZE;

    for (unsigned int fragment = 0; !tlvTable->empty(); fragment++) {
        Packet *packet = new Packet("TRILL Hello");
        auto hello = makeShared<TRILLHelloPacket>();
        //set source id
        hello->setSourceID(systemId);


        //DIS (TRILL currently supports only L1)
        disId = d->getL1Dis();
        hello->setLanID(disId);

        //TODO A1 change to define
        hello->setMaxAreas(1);
        hello->setPriority(d->getPriority());
        hello->setHoldTime(
                this->getHoldTime(iface->interfaceId, circuitType));

        for (; !tlvTable->empty();) {
            tmpTlv = tlvTable->front();

            if (tmpTlv->length > ISIS_TRILL_MAX_HELLO_SIZE) {
                //tlv won't fit into single message (this shouldn't happen)
                EV
                          << "ISIS: Warning: When generating TRILL Hello, TLV won't fit into single message (this shouldn't happen)"
                          << endl;
                tlvTable->erase(tlvTable->begin());
            } else if (tmpTlv->length > availableSpace) {
                //tlv won't fit into this LSP, so break cycle and create new LSP
                //                tmpLSPDb->push_back(LSP);
                EV << "ISIS: This TLV is full." << endl;
                break;                //ends inner cycle
            } else {

                this->addTLV(hello, tmpTlv);
                //update availableSpace
                //2Bytes are Type and Length fields
                availableSpace = availableSpace - (2 + tmpTlv->length);

                //clean up
//                    delete tmpTlv->value;
//                    delete tmpTlv;
                tlvTable->erase(tlvTable->begin());
            }
        }
        packet->insertAtFront(hello);
        d->addHello(packet);

    }

    d->setHelloValid(true);

}

/**
 * Create hello packet and send it out to specified interface. This method handle
 * LAN hello and PTP hello packets. Destination MAC address is broadcast (ff:ff:ff:ff:ff:ff).
 * @param timer is timer that triggered this action
 */
void ISISMain::sendTRILLHelloMsg(ISISTimer* timer) {
    if (getIfaceById(timer->getInterfaceId())->network) {
        EV << "ISIS: sendingTRILLBroadcastHello: " << endl;
        this->sendTRILLBroadcastHelloMsg(timer->getInterfaceId(), timer->getIsType());

    } else {
        EV << "ISIS: sendingPTPHello: " << endl;
        this->sendTRILLPTPHelloMsg(timer->getInterfaceId(), timer->getIsType());
    }
    //re-schedule timer
    this->schedule(timer);

}

/*
 * Send hello message on specified broadcast interface.
 * Packets contain IS_NEIGHBOURS_HELLO and AREA_ADDRESS TLVs.
 * @param k is interface index (index to ISISIft)
 * @param circuitType is circuit type of specified interface.
 */
//TODO the last parameter is not necessary
void ISISMain::sendTRILLBroadcastHelloMsg(int interfaceId, short circuitType) {
    if (circuitType != L1_TYPE) {
        throw cRuntimeError(
                "sendTRILLBroadcastHelloMsg: Wrong circuitType for TRILL (only L1_TYPE is supported");
    }

    ISISinterface *iface = getIfaceById(interfaceId);
    if (iface->passive || !iface->ISISenabled) {
        return;
    }

    InterfaceEntry *ie = getIfaceById(interfaceId)->entry;
    this->genTRILLHello(ie->getInterfaceId(), (ISISCircuitType) circuitType);

    IsisInterfaceData *d = ie->getProtocolData<IsisInterfaceData>();
    std::vector<Packet* > hellos = d->getHellos();
    for (std::vector<Packet * >::iterator it = hellos.begin();
            it != hellos.end(); ++it) {

        Packet * packet = ((*it))->dup();
//
        //TODO Fix encapsulation
//        // set DSAP & NSAP fields
        auto sapTag = packet->addTag<Ieee802SapReq>();
        sapTag->setDsap(SAP_CLNS);
        sapTag->setSsap(SAP_CLNS);

        packet->addTag<PacketProtocolTag>()->setProtocol(&Protocol::isis);

        //set appropriate destination MAC addresses
        MacAddress ma;
        ma.setAddress(ALL_IS_IS_RBRIDGES);

        auto macTag = packet->addTag<MacAddressReq>();
        macTag->setDestAddress(ma);

        auto interfaceTag = packet->addTag<InterfaceReq>();
        interfaceTag->setInterfaceId(ie->getInterfaceId());

        this->sendDown(packet);

    }

}

/* TODO transform it to TRILL versions
 * Sends hello message to specified PtP interface.
 * Packets contain IS_NEIGHBOURS_HELLO and AREA_ADDRESS TLVs.
 * @param interfaceId is interface index (index to ISISIft)
 * @param circuitType is circuit type of specified interface.
 */
void ISISMain::sendTRILLPTPHelloMsg(int interfaceId,
        short circuitType) {

//    unsigned int tlvSize;
    ISISinterface *iface = getIfaceById(interfaceId);
    //don't send hello packets from passive interfaces
    if (iface->passive || !iface->ISISenabled) {
        return;
    }
    Packet *packet = new Packet("PTP Hello");
    //TODO change to appropriate layer-2 protocol
//    // set DSAP & NSAP fields
    auto sapTag = packet->addTag<Ieee802SapReq>();
    sapTag->setDsap(SAP_CLNS);
    sapTag->setSsap(SAP_CLNS);

    packet->addTag<PacketProtocolTag>()->setProtocol(&Protocol::isis);

    //set appropriate destination MAC addresses
    MacAddress ma;
    /*
     * I assume that since there is only one level, destination is All ISIS Systems.
     */



    //type

    auto ptpHello = makeShared<ISISPTPHelloPacket>();

    auto macTag = packet->addTag<MacAddressReq>();
    macTag->setDestAddress(ma);

    auto interfaceTag = packet->addTag<InterfaceReq>();
    interfaceTag->setInterfaceId(iface->entry->getInterfaceId());


    //circuitType
    ptpHello->setCircuitType(iface->circuitType);

    //sourceID
    //set source id
    ptpHello->setSourceID(systemId);


    //holdTime
    //set holdTime
    ptpHello->setHoldTime(
            this->getHoldTime(interfaceId, iface->circuitType));

    //pduLength

    //localCircuitID
    ptpHello->setLocalCircuitID(iface->interfaceId);

    //TLV[]
    //set TLVs
//    TLV_t myTLV;

    this->addTLV(ptpHello, AREA_ADDRESS, circuitType);

    this->addTLV(ptpHello, PTP_HELLO_STATE, circuitType, interfaceId);

    //TODO TLV #129 Protocols supported


    packet->insertAtFront(ptpHello);
    this->sendDown(packet);

}

/*
 * Schedule specified timer according to it's type.
 * @param timer is timer to be scheduled
 * @param timee additional time information
 */
void ISISMain::schedule(ISISTimer *timer, double timee) {

    double timeAt;
    double randomTime;

    switch (timer->getTimerKind()) {
    case (ISIS_START_TIMER): {
        this->scheduleAt(0, timer);
        break;
    }
    case (TRILL_HELLO_TIMER): {
        //no brak here is on purpose
    }

    case (HELLO_TIMER): {
        timeAt = this->getHelloInterval(timer->getInterfaceId(),
                timer->getIsType());
        randomTime = uniform(0, 0.25 * timeAt);
        this->scheduleAt(simTime() + timeAt - randomTime, timer);
//            EV<< "ISIS::schedule: timeAt: " << timeAt << " randomTime: " << randomTime << endl;
        break;
    }
    case (NEIGHBOUR_DEAD_TIMER): {
        if (timee < 0) {
            EV
                      << "ISIS: Warning: You forgot provide additional time to schedule for NEIGHBOUR_DEAD_TIMER"
                      << endl;
            timee = 0;
        }
        timeAt = timee;
        randomTime = uniform(0, 0.25 * timeAt);
//            randomTime = 0;
        this->scheduleAt(simTime() + timeAt - randomTime, timer);
        break;
    }
    case (PERIODIC_SEND_TIMER): {
        cancelEvent(timer);
        if (timer->getIsType() == L1_TYPE) {
            timeAt = (double) this->L1LspSendInterval;
        } else if (timer->getIsType() == L2_TYPE) {
            timeAt = (double) this->L2LspSendInterval;
        } else {
            throw omnetpp::cRuntimeError("ISIS: Warning: Unsupported IS-Type in schedule for CSNP_TIMER");
            EV
                      << "ISIS: Warning: Unsupported IS-Type in schedule for PERIODIC_SEND_TIMER"
                      << endl;
        }

//            randomTime = uniform(0, 0.25 * timeAt);
        randomTime = 0;
        this->scheduleAt(simTime() + timeAt - randomTime, timer);
        break;
    }
    case (LSP_REFRESH_TIMER): {
        timeAt = this->lspRefreshInterval;
        randomTime = uniform(0, 0.25 * timeAt);
//            randomTime = 0;
        this->scheduleAt(simTime() + timeAt - randomTime, timer);
        break;
    }
    case (LSP_DEAD_TIMER): {
        if (timee < 0) {
            EV
                      << "ISIS: Warning: You forgot provide additional time to schedule for LSP_DEAD_TIMER"
                      << endl;
            timee = 0;
        }
        timeAt = timee;
        //randomTime = uniform(0, 0.25 * timeAt);
        this->scheduleAt(simTime() + timeAt, timer);

        /*dirty hack */
        this->runSPF(0, timer);

        break;
    }
    case (LSP_DELETE_TIMER): {
        timeAt = this->lspMaxLifetime * 2;
        this->scheduleAt(simTime() + timeAt, timer);
        break;
    }
    case (CSNP_TIMER): {
        if (timer->getIsType() == L1_TYPE) {
            timeAt =
                    getIfaceById(timer->getInterfaceId())->L1CsnpInterval;
        } else if (timer->getIsType() == L2_TYPE) {
            timeAt =
                    getIfaceById(timer->getInterfaceId())->L2CsnpInterval;
        } else {
            throw omnetpp::cRuntimeError("ISIS: Warning: Unsupported IS-Type in schedule for CSNP_TIMER");
            EV
                      << "ISIS: Warning: Unsupported IS-Type in schedule for CSNP_TIMER"
                      << endl;
        }

        randomTime = uniform(0, 0.25 * timeAt);
//            randomTime = 0;
        this->scheduleAt(simTime() + timeAt - randomTime, timer);
        break;
    }
    case (PSNP_TIMER): {
        if (timer->getIsType() == L1_TYPE) {
            timeAt =
                    getIfaceById(timer->getInterfaceId())->L1PsnpInterval;
        } else if (timer->getIsType() == L2_TYPE) {
            timeAt =
                    getIfaceById(timer->getInterfaceId())->L2PsnpInterval;
        } else {
            throw omnetpp::cRuntimeError("ISIS: Warning: Unsupported IS-Type in schedule for PSNP_TIMER");
            EV
                      << "ISIS: Warning: Unsupported IS-Type in schedule for PSNP_TIMER"
                      << endl;
        }

        randomTime = uniform(0, 0.25 * timeAt);
//            randomTime = 0;
        this->scheduleAt(simTime() + timeAt - randomTime, timer);
        break;
    }
    case (L1_CSNP): {
        timeAt = getIfaceById(timer->getInterfaceId())->L1CsnpInterval;
        randomTime = uniform(0, 0.25 * timeAt);
//            randomTime = 0;
        this->scheduleAt(simTime() + timeAt - randomTime, timer);

        break;
    }
    case (L1_PSNP): {
        timeAt = getIfaceById(timer->getInterfaceId())->L1PsnpInterval;
        randomTime = uniform(0, 0.25 * timeAt);
        randomTime = 0;
        this->scheduleAt(simTime() + timeAt - randomTime, timer);

        break;
    }
    case (GENERATE_LSP_TIMER): {
        if (timer->getIsType() == L1_TYPE) {
            if ((simTime().dbl() + this->L1LspGenInterval) * 1000
                    < this->L1LspInitWait) {
                timeAt = this->L1LspInitWait / 1000;
            } else {
                timeAt = this->L1LspGenInterval;
            }
        } else if (timer->getIsType() == L2_TYPE) {
            if ((simTime().dbl() + this->L2LspGenInterval) * 1000
                    < this->L2LspInitWait) {
                timeAt = this->L2LspInitWait / 1000;
            } else {
                timeAt = this->L2LspGenInterval;
            }
        } else {
            throw omnetpp::cRuntimeError("ISIS: ERROR: Unsupported IS-Type in PERIODIC_SEND_TIMER timer");
            EV
                      << "ISIS: ERROR: Unsupported IS-Type in PERIODIC_SEND_TIMER timer"
                      << endl;
        }
        randomTime = uniform(0, 0.25 * timeAt);
//            randomTime = 0;
        this->scheduleAt(simTime() + timeAt - randomTime, timer);

        break;
    }
    case (SPF_FULL_TIMER): {
        if (timer->getIsType() == L1_TYPE) {
            timeAt = this->L1SPFFullInterval;
        } else if (timer->getIsType() == L2_TYPE) {
            timeAt = this->L2SPFFullInterval;
        } else {
            throw omnetpp::cRuntimeError("ISIS: Error Unsupported IS-Type in SPF_FULL timer");
            EV << "ISIS: Error Unsupported IS-Type in SPF_FULL timer" << endl;
        }
        randomTime = uniform(0, 0.25 * timeAt);
//            randomTime = 0;
        this->scheduleAt(simTime() + timeAt - randomTime, timer);
        break;
    }
    default:
        throw omnetpp::cRuntimeError("ISIS: ERROR: Unsupported timer type in schedule");
        EV << "ISIS: ERROR: Unsupported timer type in schedule" << endl;
        break;
    }

}

///**
// * Parse NET address stored in this->netAddr into areaId, sysId and NSEL.
// * Method is used in initialization.
// * @see initialize(int stage)
// * @return Return true if NET address loaded from XML file is valid. Otherwise return false.
// */
//bool ISIS::parseNetAddr()
//{
//    std::string net = netAddr;
//    unsigned int dots = 0;
//    size_t found;
//
//    //net address (in this module - not according to standard O:-) MUST have this format:
//    //49.0001.1921.6801.2003.00
//
//    found = net.find_first_of(".");
//    if (found != 2 || net.length() != 25)
//    {
//        return false;
//    }
//
//    unsigned char *area = new unsigned char[ISIS_AREA_ID];
//    unsigned char *systemId = new unsigned char[ISIS_SYSTEM_ID];
//    unsigned char *nsel = new unsigned char[1];
//
//    while (found != std::string::npos)
//    {
//
//        switch (found)
//            {
//            case 2:
//                dots++;
//                area[0] = (unsigned char) (atoi(net.substr(0, 2).c_str()));
//                std::cout << "BEZ ATOI" << net.substr(0, 2).c_str() << endl;
//                break;
//            case 7:
//                dots++;
//                area[1] = (unsigned char) (atoi(net.substr(3, 2).c_str()));
//                area[2] = (unsigned char) (atoi(net.substr(5, 2).c_str()));
//                break;
//            case 12:
//                dots++;
//                systemId[0] = (unsigned char) (strtol(net.substr(8, 2).c_str(), NULL, 16));
//                systemId[1] = (unsigned char) (strtol(net.substr(10, 2).c_str(), NULL, 16));
//                break;
//            case 17:
//                dots++;
//                systemId[2] = (unsigned char) (strtol(net.substr(13, 2).c_str(), NULL, 16));
//                systemId[3] = (unsigned char) (strtol(net.substr(15, 2).c_str(), NULL, 16));
//                break;
//            case 22:
//                dots++;
//                systemId[4] = (unsigned char) (strtol(net.substr(18, 2).c_str(), NULL, 16));
//                systemId[5] = (unsigned char) (strtol(net.substr(20, 2).c_str(), NULL, 16));
//                break;
//            default:
//                return false;
//                break;
//
//            }
//
//        found = net.find_first_of(".", found + 1);
//    }
//
//    if (dots != 5)
//    {
//        return false;
//    }
//
//    nsel[0] = (unsigned char) (atoi(net.substr(23, 2).c_str()));
//
//    //49.0001.1921.6801.2003.00
//
//    areaId = area;
////    std::cout << "ISIS: AreaId: " << areaId[0] << endl;
////    std::cout << "ISIS: AreaId: " << areaId[1] << endl;
////    std::cout << "ISIS: AreaId: " << areaId[2] << endl;
//    sysId = systemId;
////    std::cout << "ISIS: SystemID: " << sysId << endl;
//    NSEL = nsel;
////    std::cout << "ISIS: NSEL: " << NSEL << endl;
//
//    this->nickname = this->sysId[ISIS_SYSTEM_ID - 1] + this->sysId[ISIS_SYSTEM_ID - 2] * 0xFF;
//
//
//    return true;
//}

/**
 * Handle L1 hello messages. Insert new neighbours into L1 adjacency table (this->adjL1Table) and
 * update status of existing neighbours.
 * @param inMsg incoming L1 hello packet
 */
void ISISMain::handleL1HelloMsg(Packet* packet) {

//    auto inMsg = packet->removeAtFront<ISISMessage>();
    auto msg = packet->removeAtFront<ISISLANHelloPacket>();

    TLV_t* tmpTLV;

      int interfaceId = packet->getTag<InterfaceInd>()->getInterfaceId();// = ift->getInterfaceById(ctrl->getInterfaceId())->getNetworkLayerGateIndex();
//    int interfaceId = inMsg->getArrivalGate()->getIndex();
    ISISinterface *tmpIntf = this->getIfaceById(interfaceId);
//    Ptr<ISISLANHelloPacket> msg = static_cast<Ptr<ISISLANHelloPacket> >(inMsg);

//    short circuitType = L1_TYPE;

    //duplicate system ID check
    if (this->checkDuplicateSysID(msg.get())) {
        //TODO schedule event duplicitSystemID
        return;
    }

    /* 8.4.2.2 */
    //check if at least one areaId matches our areaId (don't do this for L2)
    bool areaOK = false;

    for (int i = 0; (tmpTLV = this->getTLVByType(msg.get(), AREA_ADDRESS, i)) != NULL;
            i++) {
        areaOK = areaOK || this->isAreaIDOK(tmpTLV, AreaId());
    }

    if (!areaOK) {
        //TODO schedule event AreaIDMismatch
        EV << "ISIS: Warning: L1_LAN_HELLO doesn't contain Area address TLV."
                  << endl;
        return;
    }

    //if remote system ID is contained in adjL1Table
    ISISadj *tmpAdj;
    if ((tmpAdj = this->getAdj(packet, msg)) != NULL) {

        //reset timer
        cancelEvent(tmpAdj->timer);

        //TODO use this->schedule()
        //scheduleAt(simTime() + msg->getHoldTime(), tmpAdj->timer);
        this->schedule(tmpAdj->timer, msg->getHoldTime());

        //TODO DONT update the interface, if the interfaceId has changed, then declare the adjacency as down or at least init and then change interfaceId
        // I think this is covered in getAdj() -> there is performed check to ensure the adjacency match even interfaceId
        //update interface
        //tmpAdj->interfaceId = msg->getArrivalGate()->getIndex();

        //find neighbours TLV
        if ((tmpTLV = this->getTLVByType(msg.get(), IS_NEIGHBOURS_HELLO)) != NULL) {
            //TODO check length of IS_NEIGHBOURS_HELLO value
            unsigned char *tmpRecord = new unsigned char[MAC_ADDRESS_SIZE]; //size of MAC address
            //walk through all neighbour identifiers contained in TLV
            for (unsigned int r = 0; r < (tmpTLV->length / 6); r++) {
                //check if my system id is contained in neighbour's adjL1Table
                this->copyArrayContent(tmpTLV->value, tmpRecord, MAC_ADDRESS_SIZE, r * MAC_ADDRESS_SIZE, 0);

                MacAddress tmpMAC = tmpIntf->entry->getMacAddress();
                unsigned char *tmpMACAddress = new unsigned char[MAC_ADDRESS_SIZE];
                tmpMAC.getAddressBytes(tmpMACAddress);

                if (compareArrays(tmpMACAddress, tmpRecord, MAC_ADDRESS_SIZE)) {
                    //store previous state
                    ISISAdjState changed = tmpAdj->state;
                    tmpAdj->state = ISIS_ADJ_REPORT;

                    //if state changed, flood new LSP
                    if (changed != tmpAdj->state) {
                        //this->sendMyL1LSPs();
                        //TODO generate event adjacencyStateChanged
                        //TODO A! Transform to signals
//                        nb->fireChangeNotification(NF_ISIS_ADJ_CHANGED, this->genL1LspTimer);
                    }
                    break;
                }

            }
            delete[] tmpRecord;

        } else {
            //TODO Delete after debugging

            EV
                      << "ISIS: Warning: Didn't find IS_NEIGHBOURS_HELLO TLV in LAN_L1_HELLO"
                      << endl;
            return;
        }

        //check for DIS priority and eventually set new DIS if needed; do it only if exists adjacency with state "UP"
        if (tmpAdj->state == ISIS_ADJ_REPORT) {
            msg->addTag<InterfaceInd>()->setInterfaceId(packet->getTag<InterfaceInd>()->getInterfaceId());
            electDIS(msg);
        }

    }

    //else create new record in adjL1Table
    else {
        //EV << "CREATING NEW ADJ RECORD\n";

        //find area ID TLV

        if ((tmpTLV = this->getTLVByType(msg.get(), AREA_ADDRESS)) != NULL) {

            //create new neighbour record and set parameters
            ISISadj neighbour;
            neighbour.state = ISIS_ADJ_DETECT; //set state to initial

            //set timeout of neighbour
            neighbour.timer = new ISISTimer("Neighbour_timeout");
            neighbour.timer->setTimerKind(NEIGHBOUR_DEAD_TIMER);
            neighbour.timer->setIsType(L1_TYPE);
            neighbour.timer->setInterfaceId(interfaceId);
            //set source system ID in neighbour record & in timer to identify it
            //set also lspId for easier purging LSPs
            neighbour.sysID = msg->getSourceID();
            neighbour.timer->setSysID(msg->getSourceID());
            neighbour.timer->setLSPid(LspID(msg->getSourceID()));

            //TODO A1 should be from message's TLV Area Addresses

            AreaId tmpAreaID;
            tmpAreaID.fromTLV(tmpTLV->value);
            neighbour.timer->setAreaID(tmpAreaID);


            //get source MAC address of received frame
            neighbour.mac = packet->getTag<MacAddressInd>()->getSrcAddress();

            //set gate index, which is neighbour connected to
            neighbour.interfaceId = interfaceId;

            //set network type
            neighbour.network = tmpIntf->network;

            neighbour.priority = msg->getPriority();

            this->schedule(neighbour.timer, msg->getHoldTime());
            //scheduleAt(simTime() + msg->getHoldTime(), neighbour.timer);

            //insert neighbour into adjL1Table
            adjL1Table.push_back(neighbour);
            std::sort(this->adjL1Table.begin(), this->adjL1Table.end());

            //EV << "deviceId " << deviceId << ": new adjacency\n";
        }
    }
}

/**
 * Handle L2 hello messages. Insert new neighbours into L2 adjacency table (this->adjL2Table) and
 * update status of existing neighbours.
 * @param inMsg incoming L2 hello packet
 */
void ISISMain::handleL2HelloMsg(Packet* packet) {

//    auto inMsg = packet->peekAtFront<ISISMessage>();
    auto msg = packet->removeAtFront<ISISLANHelloPacket>();
    TLV_t* tmpTLV;

      int interfaceId = packet->getTag<InterfaceInd>()->getInterfaceId();// = ift->getInterfaceById(ctrl->getInterfaceId())->getNetworkLayerGateIndex();
//    int interfaceId = inMsg->getArrivalGate()->getIndex();
    ISISinterface *iface = this->getIfaceById(interfaceId);
//    Ptr<ISISLANHelloPacket> msg = static_cast<Ptr<ISISLANHelloPacket> >(inMsg);

    short circuitType = L2_TYPE;

    //duplicate system ID check
    if (this->checkDuplicateSysID(msg.get())) {
        //TODO schedule event duplicitSystemID
        return;
    }

    //if remote system ID is contained in adjL2Table
    ISISadj *tmpAdj;
    if ((tmpAdj = this->getAdj(packet, msg)) != NULL) {

        //cancel timer
        cancelEvent(tmpAdj->timer);
        //re-schedule timer
        this->schedule(tmpAdj->timer, msg->getHoldTime());

        //check for DIS priority and eventually set new DIS if needed; do it only if exists adjacency with state "UP"
        if (tmpAdj->state == ISIS_ADJ_REPORT)
            electDIS(msg);

        //find neighbours TLV
        if ((tmpTLV = this->getTLVByType(msg.get(), IS_NEIGHBOURS_HELLO)) != NULL) {
            unsigned char *tmpRecord = new unsigned char[MAC_ADDRESS_SIZE];
            //walk through all neighbour identifiers contained in TLV

            for (unsigned int r = 0; r < (tmpTLV->length / MAC_ADDRESS_SIZE);
                    r++) {
                //check if my system id is contained in neighbour's adjL1Table
                this->copyArrayContent(tmpTLV->value, tmpRecord, MAC_ADDRESS_SIZE,
                        r * MAC_ADDRESS_SIZE, 0);

                MacAddress tmpMAC = iface->entry->getMacAddress();
                unsigned char *tmpMACAddress = new unsigned char[MAC_ADDRESS_SIZE];
                tmpMAC.getAddressBytes(tmpMACAddress);

                if (compareArrays(tmpMACAddress, tmpRecord, MAC_ADDRESS_SIZE)) {
                    //store previous state
                    bool changed = tmpAdj->state;
                    tmpAdj->state = ISIS_ADJ_REPORT;

                    //if state changed, flood new LSP
                    if (changed != tmpAdj->state) {
                        //this->sendMyL1LSPs();
                        //TODO B1 generate event adjacencyStateChanged (i suppose it's done)
                        //TODO A! Signals ...
//                        nb->fireChangeNotification(NF_ISIS_ADJ_CHANGED, this->genL1LspTimer);
                        //TODO support multiple area addresses
                        if (areaID != tmpAdj->areaID
                                && this->isType == L1L2_TYPE) {
//                            this->att = true;
                            this->updateAtt(true);
                        }
                    }
                    break;
                }

            }
            delete tmpRecord;
        } else {
            //TODO Delete after debugging

            EV
                      << "ISIS: Warning: Didn't find IS_NEIGHBOURS_HELLO TLV in LAN_L2_HELLO"
                      << endl;
            return;
        }

    }

    //else create new record in adjL2Table
    else {
        //EV << "CREATING NEW ADJ RECORD\n";

        //find area ID TLV

        if ((tmpTLV = this->getTLVByType(msg.get(), AREA_ADDRESS)) != NULL) {
            //TODO add loop thru tmpTLV to add support for multiple Area Addresses

            //create new neighbour record and set parameters
            ISISadj neighbour;
            neighbour.state = ISIS_ADJ_DETECT;            //set state to initial

            //set timeout of neighbour
            neighbour.timer = new ISISTimer("Neighbour_timeout");
            neighbour.timer->setTimerKind(NEIGHBOUR_DEAD_TIMER);
            neighbour.timer->setInterfaceId(interfaceId);
            neighbour.timer->setIsType(circuitType);
            //set source system ID in neighbour record & in timer to identify it

            //set neighbours system ID
            //set also lspId for easier purging LSPs
            neighbour.sysID = msg->getSourceID();
            neighbour.timer->setSysID(msg->getSourceID());
            neighbour.timer->setLSPid(LspID(msg->getSourceID()));


            //TODO check that area address length match with ISIS_AREA_ID
            // tmpTLV->value[0] == ISIS_AREA_ID
            //set neighbours area ID
            AreaId tmpAreaID;
            tmpAreaID.fromTLV(tmpTLV->value);
            neighbour.timer->setAreaID(tmpAreaID);


            //get source MAC address of received frame
            neighbour.mac = packet->getTag<MacAddressInd>()->getSrcAddress();

            //set gate index, which is neighbour connected to
            neighbour.interfaceId = interfaceId;

            //set network type
            neighbour.network = iface->network;

            this->schedule(neighbour.timer, msg->getHoldTime());
            //scheduleAt(simTime() + msg->getHoldTime(), neighbour.timer);

            //insert neighbour into adjL1Table
            adjL2Table.push_back(neighbour);
            std::sort(this->adjL2Table.begin(), this->adjL2Table.end());

            //EV << "deviceId " << deviceId << ": new adjacency\n";

        }

    }
}
/**
 * Handle TRILL Hello messages.
 * I necessary creates new adjacency.
 * Handles only L1.
 * @param inMsg is incoming TRILL Hello message
 */
void ISISMain::handleTRILLHelloMsg(Packet* packet) {

    //RFC 6327 7.2 Receiving TRILL Hellos

//    auto inMsg = packet->rAtFront<ISISMessage>();
    auto msg = packet->removeAtFront<ISISLANHelloPacket>();

    TLV_t* tmpTLV;

    //duplicate system ID check
    if (this->checkDuplicateSysID(msg.get())) {
        //TODO schedule event duplicitSystemID
        return;
    }

//    Ptr<ISISLANHelloPacket> msg = static_cast<Ptr<ISISLANHelloPacket> >(inMsg);

    /* 8.4.2.2 */
    //check if at least one areaId matches our areaId (don't do this for L2)
    bool areaOK = false;

    for (int i = 0; (tmpTLV = this->getTLVByType(msg.get(), AREA_ADDRESS, i)) != NULL;
            i++) {
        areaOK = areaOK || this->isAreaIDOK(tmpTLV, AreaId());
    }

    if (!areaOK) {
        //TODO schedule event AreaIDMismatch
        EV << "ISIS: Warning: TRILL_HELLO doesn't match Area address." << endl;
        return;
    }

    if (msg->getMaxAreas() != 1) {
        EV
                  << "ISIS: Warning: Received TRILL Hello with wrong MaximumAreaAddress field."
                  << endl;
        return;
    }

    tmpTLV = this->getTLVByType(msg.get(), TLV_MT_PORT_CAP);

    if (tmpTLV == NULL) {
        EV << "ISIS: Warning: Received TRILL Hello without TLV TLV_MT_PORT_CAP."
                  << endl;
        return;
    }
    unsigned char *subTLV;
    if ((subTLV = this->getSubTLVByType(tmpTLV, TLV_MT_PORT_CAP_VLAN_FLAG))
            == NULL) {
        EV
                  << "ISIS: Warning: Received TRILL Hello without subTLV TLV_MT_PORT_CAP_VLAN_FLAG."
                  << endl;
        return;
    }

    int tmpDesigVlanId, tmpOuterVlanId, tmpPortId;
//    int senderNickname;
//    bool af, ac, vm, by, tr;
    //parsing TLV_MT_PORT_CAP
//    tmpPortId = subTLV[2] + subTLV[3] * 0xFF;
//    senderNickname = subTLV[4] + subTLV[5] * 0xFF;
    tmpDesigVlanId = subTLV[6] + (subTLV[7] & 0x0F) * 0xFF;
//    af = (subTLV[7] >> 7) & 0x01;
//    ac = (subTLV[7] >> 6) & 0x01;
//    vm = (subTLV[7] >> 5) & 0x01;
//    by = (subTLV[7] >> 4) & 0x01;
//    tmpOuterVlanId = subTLV[8] + (subTLV[8] & 0x0F) * 0xFF;
//    tr = (subTLV[9] >> 7) & 0x01;



    int interfaceId = packet->getTag<InterfaceInd>()->getInterfaceId();
    msg->addTag<InterfaceInd>()->setInterfaceId(interfaceId);

    ISISinterface *tmpIntf = this->getIfaceById(interfaceId);
    MacAddress tmpMAC = tmpIntf->entry->getMacAddress();
    unsigned char *tmpMACChars = new unsigned char[MAC_ADDRESS_SIZE];
    tmpMAC.getAddressBytes(tmpMACChars);
    //if sender is DIS
    if (msg->getSourceID() == tmpIntf->L1DIS) {

        TrillInterfaceData *trillD = tmpIntf->entry->getProtocolData<TrillInterfaceData>();
        trillD->setDesigVlan(tmpDesigVlanId);
        if ((subTLV = this->getSubTLVByType(tmpTLV, TLV_MT_PORT_CAP_APP_FWD))
                != NULL) {
            //THIS SHOULD BE HANDLED IN electDIS. based on result act accordingly
            //handle Appointed Forwarders subTLV
            //TODO A2 appointed Fwd should be touples <nickname, vlanIdRange> or simply <nickname, vlanId>


            trillD->clearAppointedForwarder();
            for (int subLen = 0; subLen < subTLV[1];) {
                TRILLNickname appointeeNickname;
                appointeeNickname.set(subTLV[subLen + 2] + subTLV[subLen + 1 + 2] * 0xFF);
                int startVlan = subTLV[subLen + 2 + 2]
                        + (subTLV[subLen + 3 + 2] & 0x0F) * 0xFF;
                int endVlan = subTLV[subLen + 4 + 2]
                        + (subTLV[subLen + 5 + 2] & 0x0F) * 0xFF;
                subLen += 6;

                for(int appVlan = startVlan; appVlan <= endVlan; appVlan++){
                  trillD->addAppointedForwarder(appVlan, appointeeNickname);
                }
            }

        }
    }

    //check for presence of TLV Protocols Supported and if so it must list TRILL NLPID (0xC0)
    //if such TLV is not present then continue as it would list TRILL NLPID
    //(that's what we will do for now)

    //TODO We're gonna need a bigger boat erm I mean more complex Adjacency State Machinery. Preferably new method to handle it.
    //TODO RFC6327 3.3. event A0 (received Hello with SNPA matching SNPA of port this Hello was received on.
    //                   event A2 received on non desig VLAN -> ignore TLV TRILL_NEIGHBOR
    //                         A4, A5, A6, A7, A8
    ISISadj *tmpAdj;
    if ((tmpAdj = this->getAdj(packet, msg)) != NULL) {
        //reset timer
        cancelEvent(tmpAdj->timer);

        this->schedule(tmpAdj->timer, msg->getHoldTime());

        //TODO A3? updating interface and gate index (see handleL1HelloMsg())

//        if (tmpAdj->state == ISIS_ADJ_REPORT)
//        {
//            electDIS(msg);
//        }
        //TODO handle MT
        for (int offset = 0;
                (tmpTLV = this->getTLVByType(msg.get(), TLV_TRILL_NEIGHBOR, offset))
                        != NULL; offset++) {
            unsigned char *tmpRecord = new unsigned char[MAC_ADDRESS_SIZE];
//            bool s, l;
//            s = tmpTLV->value[0] >> 7 & 0x01;
//            l = tmpTLV->value[0] >> 6 & 0x01;

            for (unsigned int r = 0; r < ((tmpTLV->length - 1) / 9); r++) {
                //reported SNPA (MAC Address)
                memcpy(tmpRecord, &tmpTLV->value[1 + (r * 9) + 3],
                        MAC_ADDRESS_SIZE);
                bool failedMTU = tmpTLV->value[1 + r * 9] >> 7 && 0x01;
                if (memcmp(tmpMACChars, tmpRecord, MAC_ADDRESS_SIZE) == 0) {
                    //RFC 6327 3.3. A1 (sender explicitly list this IS's SNPA)
                    //store previous state
                    ISISAdjState changed = tmpAdj->state;
                    switch (tmpAdj->state) {
                    case ISIS_ADJ_DOWN: //not sure if this could occur (but the chosen behavior is correct)
                    case ISIS_ADJ_DETECT:
                    case ISIS_ADJ_2WAY:
                        tmpAdj->state = ISIS_ADJ_2WAY;
                        break;
                    case ISIS_ADJ_REPORT:
                        break;
                    }

                    if (!failedMTU) {
                        //RFC 6327 3.3 A6
                        switch (tmpAdj->state) {
                        case ISIS_ADJ_DOWN: //not sure if this could occur (but the chosen behavior is correct)
                        case ISIS_ADJ_DETECT:
                            EV
                                      << "ISIS: ERROR: This Adjacency state shoudn't occur"
                                      << endl;
                            break;
                        case ISIS_ADJ_2WAY:
                        case ISIS_ADJ_REPORT:
                            tmpAdj->state = ISIS_ADJ_REPORT;
                            break;
                        }

                    }

                    if (changed != tmpAdj->state) {
                        //TODO generate event adjacencyStateChanged
                        //TODO A! Signals...
//                        nb->fireChangeNotification(NF_ISIS_ADJ_CHANGED, this->genL1LspTimer);

                        int interfaceId = packet->getTag<InterfaceInd>()->getInterfaceId();

                        InterfaceEntry *entry = ift->getInterfaceById(interfaceId);
                        IsisInterfaceData *isisData = entry->getProtocolData<IsisInterfaceData>();
                        isisData->setHelloValid(false);

                    }

                } else {
                    //reported neighbor's SNPA is not this port's SNPA
                    //TODO dunno if I shoudn't add this reported neighbor anyway
                }
            }

        }

        //TODO A!
        if (tmpAdj->state >= ISIS_ADJ_2WAY) {
            electDIS(msg);
        }

    }
    //else create new record in adjL1Table
    else {

        ISISadj neighbour;
        neighbour.state = ISIS_ADJ_DETECT;

        //set timeout timer of neighbour
        neighbour.timer = new ISISTimer("Neighbour_timeout");
        neighbour.timer->setTimerKind(NEIGHBOUR_DEAD_TIMER);
        neighbour.timer->setIsType(L1_TYPE);
        neighbour.timer->setInterfaceId(interfaceId);
//        neighbour.timer->setGateIndex(interfaceId);
        //set source system ID in neighbour record & in timer to identify it
        //set also lspId for easier purging LSPs
        neighbour.sysID = msg->getSourceID();
        neighbour.timer->setSysID(msg->getSourceID());
        neighbour.timer->setLSPid(LspID(msg->getSourceID()));


        //TODO should be from message's TLV Area Addresses
        neighbour.areaID = areaID;
        neighbour.timer->setAreaID(areaID);


        //get source MAC address of received frame
        neighbour.mac = packet->getTag<MacAddressInd>()->getSrcAddress();

        //set gate index, which is neighbour connected to
        neighbour.interfaceId = interfaceId;

        //set network type
        neighbour.network = tmpIntf->network;

        neighbour.priority = msg->getPriority();

        //TODO B check that tmpDesigVlanId is DESIRED and not currently used designated VLAN
        neighbour.desiredDesVLAN = tmpDesigVlanId;
        this->schedule(neighbour.timer, msg->getHoldTime());
        //scheduleAt(simTime() + msg->getHoldTime(), neighbour.timer);

        //insert neighbour into adjL1Table
        adjL1Table.push_back(neighbour);
        std::sort(this->adjL1Table.begin(), this->adjL1Table.end());

//        nb->fireChangeNotification(NF_ISIS_ADJ_CHANGED, this->genL1LspTimer);

        int interfaceId = packet->getTag<InterfaceInd>()->getInterfaceId();

        InterfaceEntry *entry = ift->getInterfaceById(interfaceId);
        IsisInterfaceData *isisData = entry->getProtocolData<IsisInterfaceData>();
        isisData->setHelloValid(false);

    }

}
/**
 * Handle PTP hello messages.
 * If necessary creates new adjacency.
 * Update status of existing neighbours.
 * @param inMsg incoming PtP hello packet
 */
void ISISMain::handlePTPHelloMsg(Packet* packet)
{
//    auto inMsg = packet->peekAtFront<ISISMessage>();
    auto msg = packet->removeAtFront<ISISPTPHelloPacket>();

  int interfaceId = packet->getTag<InterfaceInd>()->getInterfaceId();// = ift->getInterfaceById(ctrl->getInterfaceId())->getNetworkLayerGateIndex();
  ISISinterface *iface = this->getIfaceById(interfaceId);


  //duplicate system ID check
  if (this->checkDuplicateSysID(msg.get()))
  {
    //TODO schedule event duplicitSystemID
    return;
  }

//  Ptr<ISISPTPHelloPacket> msg = static_cast<Ptr<ISISPTPHelloPacket>>(inMsg);

  //check if at least one areaId matches our areaId (don't do this for L2)
  TLV_t* tmpTLV;
  if (msg->getCircuitType() != L2_TYPE)
  {
    bool areaOK = false;
    tmpTLV = this->getTLVByType(msg.get(), AREA_ADDRESS, 0);
    for (int i = 0; (tmpTLV) != NULL; i++)
    {
      areaOK = areaOK || this->isAreaIDOK(tmpTLV, AreaId());
      tmpTLV = this->getTLVByType(msg.get(), AREA_ADDRESS, i);
    }

    if (!areaOK)
    {
      //TODO schedule event AreaIDMismatch
      EV << "ISIS: Warning: PTP_HELLO doesn't contain Area address TLV." << endl;
      return;
    }
  }
  //if remote system ID is contained in adjL1Table
  ISISadj *tmpAdj;
  if (msg->getCircuitType() == L1_TYPE || msg->getCircuitType() == L1L2_TYPE)
  {

    if ((tmpAdj = this->getAdj(packet, msg, L1_TYPE)) != NULL)
    {

      //reset timer
      cancelEvent(tmpAdj->timer);

      //TODO use this->schedule()
      scheduleAt(simTime() + msg->getHoldTime(), tmpAdj->timer);

      //find neighbours TLV
      if ((tmpTLV = this->getTLVByType(msg.get(), PTP_HELLO_STATE)) != NULL)
      {
        if (tmpTLV->length == 1)
        {

          if (tmpAdj->state == ISIS_ADJ_REPORT) //UP
          {

            if (tmpTLV->value[0] == PTP_UP)
            {
              //OK do nothing
            }
            else if (tmpTLV->value[0] == PTP_DOWN)
            {
              //state init
              //TODO B2
              tmpAdj->state = ISIS_ADJ_DETECT;

            }
            else
            {
              //state accept
            }

          }
          else if (tmpAdj->state == ISIS_ADJ_DETECT) // INIT
          {
            if (tmpTLV->value[0] == PTP_UP || tmpTLV->value[0] == PTP_INIT)
            {
              //OK
              tmpAdj->state = ISIS_ADJ_REPORT;
//                            if (simTime() > 35.0){ //TODO use at least some kind of ISIS_INITIAL_TIMER
              //this->sendMyL1LSPs();
//                            }
              //TODO B2
              //schedule adjacencyStateChange(up);
              //TODO A! Signals...
//                            nb->fireChangeNotification(NF_ISIS_ADJ_CHANGED, this->genL1LspTimer);

            }
            else
            {
              //stay init
            }
          }
        }

      }
      else
      {
        //TODO Delete after debugging
        EV << "ISIS: Warning: Didn't find PTP_HELLO_STATE TLV in PTP_HELLO L2" << endl;
        return;
      }

    }

    //else create new record in adjL1Table
    else
    {
      //EV << "CREATING NEW ADJ RECORD\n";

      //find area ID TLV

      //create new neighbour record and set parameters
      ISISadj neighbour;
      neighbour.state = ISIS_ADJ_DETECT;            //set state to initial

      //set timeout of neighbour
      neighbour.timer = new ISISTimer("Neighbour_timeout");
      neighbour.timer->setTimerKind(NEIGHBOUR_DEAD_TIMER);
      neighbour.timer->setIsType(L1_TYPE);
      neighbour.timer->setInterfaceId(interfaceId);

      //set source system ID in neighbour record & in timer to identify it

      neighbour.sysID = msg->getSourceID();
      neighbour.timer->setSysID(msg->getSourceID());
      neighbour.timer->setLSPid(LspID(msg->getSourceID()));

      //TODO should be from message's TLV Area Addresses
      neighbour.areaID = areaID;


      //get source MAC address of received frame
      neighbour.mac = packet->getTag<MacAddressInd>()->getSrcAddress();

      //set gate index, which is neighbour connected to
      neighbour.interfaceId = interfaceId;

      //set network type
      neighbour.network = this->getIfaceById(neighbour.interfaceId)->network;

      this->schedule(neighbour.timer, msg->getHoldTime());
      //scheduleAt(simTime() + msg->getHoldTime(), neighbour.timer);

      //insert neighbour into adjL1Table
      adjL1Table.push_back(neighbour);
      std::sort(this->adjL1Table.begin(), this->adjL1Table.end());

      //EV << "deviceId " << deviceId << ": new adjacency\n";

    }
  }

  if (msg->getCircuitType() == L2_TYPE || msg->getCircuitType() == L1L2_TYPE)
  {
    if ((tmpAdj = this->getAdj(packet, msg, L2_TYPE)) != NULL)
    {
      //reset timer
      cancelEvent(tmpAdj->timer);

      //TODO use this->schedule()
      scheduleAt(simTime() + msg->getHoldTime(), tmpAdj->timer);

      //find neighbours TLV
      if ((tmpTLV = this->getTLVByType(msg.get(), PTP_HELLO_STATE)) != NULL)
      {
        if (tmpTLV->length == 1)
        {
          if (tmpAdj->state == ISIS_ADJ_REPORT) //UP
          {
            if (tmpTLV->value[0] == PTP_UP)
            {
              //OK do nothing
            }
            else if (tmpTLV->value[0] == PTP_DOWN)
            {
              //state init
              //TODO B2
              tmpAdj->state = ISIS_ADJ_DETECT;

            }
            else
            {
              //state accept
            }

          }
          else // INIT
          {
            if (tmpTLV->value[0] == PTP_UP || tmpTLV->value[0] == PTP_INIT)
            {
              //OK
              tmpAdj->state = ISIS_ADJ_REPORT;
//                            if (simTime() > 35.0)
//                            { //TODO use at least some kind of ISIS_INITIAL_TIMER
              //this->sendMyL2LSPs();
//                            }
              //TODO B2
              //schedule adjacencyStateChange(up);
              //TODO A! Signals
//                            nb->fireChangeNotification(NF_ISIS_ADJ_CHANGED, this->genL2LspTimer);
              //TODO support multiple area addresses
              if (areaID != tmpAdj->areaID && this->isType == L1L2_TYPE)
              {
                this->updateAtt(true);
              }
            }
            else
            {
              //stay init
            }
          }
        }
      }
      else
      {
        //TODO Delete after debugging
        EV << "ISIS: Warning: Didn't find PTP_HELLO_STATE TLV in PTP_HELLO" << endl;
        return;
      }

    }

    //else create new record in adjL2Table
    else
    {
      //EV << "CREATING NEW ADJ RECORD\n";

      //find area ID TLV

      //create new neighbour record and set parameters
      ISISadj neighbour;
      neighbour.state = ISIS_ADJ_DETECT;            //set state to initial

      //set timeout of neighbour
      neighbour.timer = new ISISTimer("Neighbour_timeout");
      neighbour.timer->setTimerKind(NEIGHBOUR_DEAD_TIMER);
      neighbour.timer->setIsType(L2_TYPE);
      neighbour.timer->setInterfaceId(interfaceId);


      //set source system ID in neighbour record & in timer to identify it
      neighbour.sysID = msg->getSourceID();
      neighbour.timer->setSysID(msg->getSourceID());
      neighbour.timer->setLSPid(LspID(msg->getSourceID()));

      //set neighbours area ID
      tmpTLV = this->getTLVByType(msg.get(), AREA_ADDRESS);

      //TODO compare tmpTLV->value[0] and ISIS_AREA_ID
      AreaId tmpAreaID;
      tmpAreaID.fromTLV(tmpTLV->value);
      neighbour.timer->setAreaID(tmpAreaID);


      //get source MAC address of received frame

      neighbour.mac = packet->getTag<MacAddressInd>()->getSrcAddress();

      //set gate index, which is neighbour connected to

      neighbour.interfaceId = packet->getTag<InterfaceInd>()->getInterfaceId();

      //set network type
      neighbour.network = this->getIfaceById(neighbour.interfaceId)->network;

      this->schedule(neighbour.timer, msg->getHoldTime());
      //scheduleAt(simTime() + msg->getHoldTime(), neighbour.timer);

      //insert neighbour into adjL2Table
      adjL2Table.push_back(neighbour);
      std::sort(this->adjL2Table.begin(), this->adjL2Table.end());

      //EV << "deviceId " << deviceId << ": new adjacency\n";

    }
  }
}

void ISISMain::updateAtt(bool action) {
    //TODO IS should regenerate zero-th LSP when attached flag changes
    if (action) {
        if (!this->att) {
            this->att = true;
            this->setClosestAtt();
        }

    } else if (action != this->att) {
        for (AdjTab_t::iterator it = this->adjL2Table.begin();
                it != this->adjL2Table.end(); ++it) {
            if (areaID != (*it).areaID && this->isType == L1L2_TYPE) {
                //at least one L2 adjacency with another area exists so don't clear Attached flag
                this->setClosestAtt();
                return;
            }
        }

        this->att = false;
    }
}

bool ISISMain::isAdjBySystemID(SystemID systemID, short ciruitType) {
    if (ciruitType == L1_TYPE) {
        //walk through adjacency table and look for existing neighbours
        for (std::vector<ISISadj>::iterator it = this->adjL1Table.begin();
                it != adjL1Table.end(); ++it) {
            if (systemId == (*it).sysID) {
                return true;
            }
        }
    } else if (ciruitType == L2_TYPE) {
        //walk through adjacency table and look for existing neighbours
        for (std::vector<ISISadj>::iterator it = this->adjL2Table.begin();
                it != adjL2Table.end(); ++it) {
            if (systemId == (*it).sysID) {
                return true;
            }

        }
    }

    return false;
}

/**
 * This method check adjacency in table based on inMsg type and compares System-ID, MAC address and interfaceId.
 * If any of these parameters don't match returns NULL, otherwise relevant adjacency;
 * @param inMsg is incoming message that is being parsed
 */
ISISadj* ISISMain::getAdj(Packet* packet, Ptr<ISISMessage> inMsg, short circuitType) {
//    short circuitType;
    SystemID sysID;
    std::vector<ISISadj> *adjTable;

    if (inMsg->getType() == LAN_L1_HELLO) {
        ISISLANHelloPacket* msg = static_cast<ISISLANHelloPacket*>(inMsg.get());
        sysID = msg->getSourceID();
        circuitType = L1_TYPE;
        adjTable = &(this->adjL1Table);
    } else if (inMsg->getType() == LAN_L2_HELLO) {
        ISISLANHelloPacket* msg = static_cast<ISISLANHelloPacket* >(inMsg.get());
        sysID = msg->getSourceID();
        circuitType = L2_TYPE;
        adjTable = &(this->adjL2Table);
    } else if (inMsg->getType() == TRILL_HELLO) {
        TRILLHelloPacket* msg = static_cast<TRILLHelloPacket* >(inMsg.get());
        sysID = msg->getSourceID();
        circuitType = L1_TYPE;
        adjTable = &(this->adjL1Table);
    } else if (inMsg->getType() == PTP_HELLO) {
        ISISPTPHelloPacket* msg = static_cast<ISISPTPHelloPacket* >(inMsg.get());
        sysID = msg->getSourceID();
//        circuitType = msg->getCircuitType();

        adjTable = &(this->adjL1Table);
        if (circuitType == L1_TYPE) {
            adjTable = &(this->adjL1Table);
        } else if (circuitType == L2_TYPE) {
            adjTable = &(this->adjL2Table);
        }
    }else {
        throw omnetpp::cRuntimeError("ISIS: Unsupported message type in getAdj()");
    }


      int interfaceId = packet->getTag<InterfaceInd>()->getInterfaceId();

      //TODO for truly point-to-point link there would not be MAC address
      MacAddress tmpMac = packet->getTag<MacAddressInd>()->getSrcAddress();// = ctrl->getSrc();

    for (std::vector<ISISadj>::iterator it = adjTable->begin(); it != adjTable->end(); ++it) {
        //System-ID match?
        if (sysID == (*it).sysID) {

            //MAC Address and interfaceId
            //we need to check source (tmpMac) and destination interface thru we received this hello

            if (tmpMac.compareTo((*it).mac) == 0
                    && interfaceId == (*it).interfaceId) {
                if (circuitType != L2_TYPE) { //TODO shoudn't it be "!="
                                              //check if at least one areaId matches our areaId (don't do this for L2)
                    bool areaOK = false;
                    TLV_t* tmpTLV;
                    tmpTLV = this->getTLVByType(inMsg.get(), AREA_ADDRESS, 0);
                    for (int i = 0; tmpTLV != NULL; i++) {
                        areaOK = areaOK
                                || this->isAreaIDOK(tmpTLV, (*it).areaID);
                        tmpTLV = this->getTLVByType(inMsg.get(), AREA_ADDRESS, i);
                    }

                    if (!areaOK) {
                        //TODO schedule event AreaIDMismatch
//                        EV << "ISIS: Warning: L1_LAN_HELLO doesn't contain Area address TLV." << endl;
                        break;
                    }
                }
//                delete[] sysID;
                return &(*it);
            }
        }
    }
//    delete[] sysID;
    return NULL;

}
/**
 * Don't use this method, but rather ISIS::getAdj(Ptr<ISISMessage> ) which can handle multiple adjacencies between two ISes.
 *
 * @param systemID is system-ID for which we want to find adjacency
 * @param specify level
 * @param interfaceId specify interface
 */
ISISadj *ISISMain::getAdjBySystemID(SystemID systemID, short circuitType, int interfaceId) {

    /* For redundant links there could be more than one adjacency for the same System-ID.
     * We should return std::vector<ISISadj> with all adjacencies */

  if (circuitType == L1_TYPE)
  {

    for (std::vector<ISISadj>::iterator it = this->adjL1Table.begin(); it != this->adjL1Table.end(); ++it)
    {
      if (systemID == (*it).sysID)
      {
        if (interfaceId > -1 && (*it).interfaceId != interfaceId)
        {
          continue;
        }
        return &(*it);
      }

    }
  }
  else if (circuitType == L2_TYPE)
  {

        for (std::vector<ISISadj>::iterator it = this->adjL2Table.begin(); it != this->adjL2Table.end(); ++it) {
            if (systemID == (*it).sysID) {
                if (interfaceId > -1 && (*it).interfaceId != interfaceId) {
                    continue;
                }
                return &(*it);
            }

        }
    } else if (circuitType == L1L2_TYPE) {
        EV
                  << "ISIS: ERROR: getAdjBySystemID for L1L2_TYPE is not implemented (yet)"
                  << endl;
        //TODO B3
        /* For point-to-point link there should be only ONE adjacency in both tables*/
        for (std::vector<ISISadj>::iterator it = this->adjL1Table.begin();
                it != this->adjL1Table.end(); ++it) {
            if (systemID == (*it).sysID) {
                return &(*it);
            }

        }

    }
    return NULL;
}

ISISadj *ISISMain::getAdjByMAC(const MacAddress &address, short circuitType,
        int interfaceId) {

    std::vector<ISISadj> *adjTab = this->getAdjTab(circuitType);

    for (std::vector<ISISadj>::iterator it = adjTab->begin();
            it != adjTab->end(); ++it) {
        if ((*it).mac.compareTo(address) == 0) {
            if (interfaceId == -1 || interfaceId == (*it).interfaceId) {
                return &(*it);
            }
        }
    }

    return NULL;
}

/*
 * Extract System-ID from message.
 * @param msg incomming msg
 * @return newly allocated system-id
 */
SystemID ISISMain::getSysID(const ISISMessage *msg) {

    //TODO How about pure virtual method 'getSysID' in ISISMessage which forces all the subclassed classes
    // to implment it and then we can call it on 'ISISMessage' object without this hassle!
    if (msg->getType() == LAN_L1_HELLO) {
        const ISISLANHelloPacket *l1hello = check_and_cast<const ISISLANHelloPacket *>(msg);
        return l1hello->getSourceID();

    } else if (msg->getType() == LAN_L2_HELLO) {
        const ISISLANHelloPacket *l2hello = check_and_cast<const ISISLANHelloPacket *>(msg);
        return l2hello->getSourceID();

    } else if (msg->getType() == TRILL_HELLO) {
        const TRILLHelloPacket *trillHello = check_and_cast<const TRILLHelloPacket *>(msg);
        return trillHello->getSourceID();

    } else if (msg->getType() == PTP_HELLO) {
        const ISISPTPHelloPacket *ptphello = check_and_cast<const ISISPTPHelloPacket *>(
                msg);
        return ptphello->getSourceID();


    } else if (msg->getType() == L1_PSNP) {
        const ISISPSNPPacket *psnp = check_and_cast<const ISISPSNPPacket *>(msg);
        return psnp->getSourceID();


    } else if (msg->getType() == L2_PSNP) {
        const ISISPSNPPacket *psnp = check_and_cast<const ISISPSNPPacket *>(msg);
        return psnp->getSourceID();


    } else if (msg->getType() == L1_CSNP) {
        const ISISCSNPPacket *csnp = check_and_cast<const ISISCSNPPacket *>(msg);
        return csnp->getSourceID();


    } else if (msg->getType() == L2_CSNP) {
        const ISISCSNPPacket *csnp = check_and_cast<const ISISCSNPPacket *>(msg);
        return csnp->getSourceID();


    } else if (msg->getType() == L1_LSP || msg->getType() == L2_LSP) {
        const ISISLSPPacket *lsp = check_and_cast<const ISISLSPPacket *>(msg);
        return lsp->getLspID().getSystemId();


    } else {
        EV
                  << "ISIS: ERROR: getSysID for this message type is not implemented (yet?): "
                  << msg->getType() << endl;
    }

    throw cRuntimeError("ISIS: ERROR: getSysID for this message type is not implemented (yet?):");

}

///*
// * Extract System-ID from timer.
// * @param msg incomming msg
// * @return newly allocated system-id
// */
//unsigned char* ISIS::getSysID(ISISTimer *timer)
//{
//    unsigned char *systemID = new unsigned char[ISIS_SYSTEM_ID];
//
//    for (int i = 0; i < ISIS_SYSTEM_ID; i++)
//    {
//        systemID[i] = timer->getSysID(i);
//    }
//
//    return systemID;
//}
//
///*
// * Extract LSP-ID from timer.
// * @param timer incomming timer
// * @return newly allocated system-id
// */
//unsigned char* ISIS::getLspID(ISISTimer *timer)
//{
//    unsigned char *lspID = new unsigned char[ISIS_SYSTEM_ID + 2];
//
//    for (int i = 0; i < ISIS_SYSTEM_ID + 2; i++)
//    {
//        lspID[i] = timer->getLSPid(i);
//    }
//
//    return lspID;
//}
//
//
///*
// * Extract LSP-ID from message.
// * @param msg incomming msg
// * @return newly allocated system-id
// */
//unsigned char* ISIS::getLspID(ISISLSPPacket *msg)
//{
//
//    unsigned char *lspId = new unsigned char[8]; //TODO change back to ISIS_SYSTEM_ID + 2
//
//    for (int i = 0; i < ISIS_SYSTEM_ID + 2; i++)
//    {
//        lspId[i] = msg->getLspID(i);
//    }
//
//    return lspId;
//
//}
//
//void ISIS::setLspID(ISISLSPPacket *msg, unsigned char * lspId)
//{
//
//    for (int i = 0; i < ISIS_SYSTEM_ID + 2; i++)
//    {
//        msg->setLspID(i, lspId[i]);
//    }
//
//}
//
//unsigned char* ISIS::getLanID(Ptr<ISISLANHelloPacket> msg)
//{
//
//    unsigned char *lanID = new unsigned char(ISIS_SYSTEM_ID +  2);
//
//    for (int i = 0; i < ISIS_SYSTEM_ID + 1; ++i)
//    {
//        lanID[i] = msg->getLanID(i); //XXX Invalid write? How?
//    }
//
//    return lanID;
//}

/**
 * Print L1 and L2 adjacency tables to EV.
 * This function is currently called every time hello packet is received and processed.
 * @see handleMessage(cMessage* msg)
 */
void ISISMain::printAdjTable() {
    std::sort(this->adjL1Table.begin(), this->adjL1Table.end());
    EV << "L1 adjacency table of IS ";

    //print area id
    EV << areaID << ".";


    //print system id
    EV << systemId;


    //print NSEL
    EV << ".00"; //It's always zero (for ISIS)
    EV << "\tNo. of records in Table: " << adjL1Table.size() << endl;
//    EV << std::setfill('0') << std::setw(2) << std::dec << (unsigned int) NSEL[0] << "\tNo. of records in Table: "
//    << adjL1Table.size() << endl;

    //print neighbour records
    for (unsigned int j = 0; j < adjL1Table.size(); j++) {
        EV << "\t";
        //print neighbour system id
        EV << adjL1Table.at(j).sysID;
        EV << "\t";

        //print neighbour MAC address
        for (unsigned int i = 0; i < 6; i++) {
            EV << std::setfill('0') << std::setw(2) << std::hex
                      << (unsigned int) adjL1Table.at(j).mac.getAddressByte(i);
            if (i < 5)
                EV << ":";
        }
        EV << "\t";

        if (adjL1Table.at(j).state == ISIS_ADJ_DETECT)
            EV << "Init (Detect)\n";
        else if (adjL1Table.at(j).state == ISIS_ADJ_2WAY)
            EV << "Up (2Way)\n";
        else if (adjL1Table.at(j).state == ISIS_ADJ_REPORT)
            EV << "Up (Report)\n";
    }

    EV
              << "--------------------------------------------------------------------\n";

    EV << "L2 adjacency table of IS ";

    //print area id
    EV << areaID << ".";


    //print system id
    EV << systemId << ".";


    //print NSEL
    EV << "00\tNo. of records in Table: " << adjL2Table.size() << endl;
//    EV<< std::setfill('0') << std::setw(2) << std::dec << (unsigned int) NSEL[0] << "\tNo. of records in Table: "
//    << adjL2Table.size() << endl;

    //print neighbour records
    for (unsigned int j = 0; j < adjL2Table.size(); j++) {
        EV << "\t";
        //print neighbour area id and system id
        EV << adjL2Table.at(j).areaID << ".";

        EV << adjL2Table.at(j).sysID;

        EV << "\t";

        //print neighbour MAC address
        for (unsigned int i = 0; i < 6; i++) {
            EV << std::setfill('0') << std::setw(2) << std::hex
                      << (unsigned int) adjL2Table.at(j).mac.getAddressByte(i);
            if (i < 5)
                EV << ":";
        }
        EV << "\t";

        if (adjL2Table.at(j).state == ISIS_ADJ_DETECT)
            EV << "Init (Detect)\n";
        else if (adjL2Table.at(j).state == ISIS_ADJ_2WAY)
            EV << "Up (2Way)\n";
        else if (adjL2Table.at(j).state == ISIS_ADJ_REPORT)
            EV << "Up (Report)\n";
    }
}

//void ISIS::setClnsTable(CLNSTable *clnsTable)
//{
//    this->clnsTable = clnsTable;
//}

//void ISIS::setNb(NotificationBoard *nb)
//{
//    this->nb = nb;
//}
/* Sets and then parses NET address.
 * @param netAddr specify NET address
 */
//void ISIS::setNetAddr(std::string netAddr)
//{
//    this->netAddr = netAddr;
//
//    if (!this->parseNetAddr())
//    {
//        throw cRuntimeError("Unable to parse NET address.");
//    }
//}
void ISISMain::setMode(ISIS_MODE mode) {
    this->mode = mode;
}

//TODO A! Signals
//void ISIS::subscribeNb(void)
//{
//    nb->subscribe(this, interfaceStateChangedSignal);
//    nb->subscribe(this, NF_CLNS_ROUTE_DELETED);
//    nb->subscribe(this, NF_ISIS_ADJ_CHANGED);
//
//}

void ISISMain::setIft(IInterfaceTable *ift) {
    this->ift = ift;
}

void ISISMain::setTrill(TRILL *trill)
{
    if (trill == NULL)
    {
        throw cRuntimeError("Got NULL pointer instead of TRILL reference");
    }
    this->trill = trill;
}

/**
 * Print content of L1 LSP database to EV.
 */
void ISISMain::printLSPDB() {
    short circuitType = L1_TYPE;
    std::vector<LSPRecord *> *lspDb = this->getLSPDb(circuitType);
    std::sort(lspDb->begin(), lspDb->end(), cmpLSPRecord());
    EV << "L1 LSP database of IS ";

    //print area id
    EV << areaID << ".";


    //print system id
    EV << systemId << ".";


    //print NSEL
    EV << "00\tNo. of records in database: " << lspDb->size() << endl;
//    EV<< std::setfill('0') <<   std::setw(2) << std::dec << (unsigned int) NSEL[0] << "\tNo. of records in database: "
//    << lspDb->size() << endl;
//    unsigned char *lspId;
    std::vector<LSPRecord *>::iterator it = lspDb->begin();
    for (; it != lspDb->end(); ++it) {

        //Update the remaining lifetime
        (*it)->LSP->setRemLifeTime(floor((*it)->simLifetime - simTime().dbl()));

        EV << "\t";
        //print LSP ID
        EV << (*it)->LSP->getLspID();

        EV << "\t0x";

        //print sequence number
        EV << std::setfill('0') << std::setw(8) << std::hex
                  << (*it)->LSP->getSeqNumber();
        EV << "\t" << std::setfill('0') << std::setw(5) << std::dec
                  << (*it)->LSP->getRemLifeTime() << endl;
        //EV <<"SeqNum: " << (*it)->LSP->getSeqNumber()<<endl;

        TLV_t *tmpTlv;

        //print neighbours
        for (unsigned int k = 0;
                (tmpTlv = this->getTLVByType((*it)->LSP.get(), IS_NEIGHBOURS_LSP, k))
                        != NULL; k++) {
            for (unsigned int m = 1; m + 11 <= tmpTlv->length; m += 11) {
                EV << "\t\t";
                PseudonodeID tmpPseudoID;
                tmpPseudoID.fromTLV(&(tmpTlv->value[m + 4]));
                EV << tmpPseudoID;


                EV << "\tmetric: " << std::setfill('0') << std::setw(2)
                          << std::dec << (unsigned int) tmpTlv->value[m + 0]
                          << endl;
            }

        }

    }

    if (this->attIS != NULL) {
        EV << "Closest L1L2 IS:" << endl;
        for (ISISNeighbours_t::iterator attIt = this->attIS->begin();
                attIt != this->attIS->end(); ++attIt) {
            EV << (*attIt)->id;

            EV << endl;
        }
    }

    circuitType = L2_TYPE;
    lspDb = this->getLSPDb(circuitType);
    EV << "L2 LSP database of IS ";

    //print area id
    EV << areaID << ".";


    //print system id
    EV << systemId << ".";


    //print NSEL
    EV << "00\tNo. of records in database: " << lspDb->size() << endl;
//    EV<< std::setfill('0') <<  std::setw(2) << std::dec << (unsigned int) NSEL[0] << "\tNo. of records in database: "
//    << lspDb->size() << endl;
//        unsigned char *lspId;
    it = lspDb->begin();
    for (; it != lspDb->end(); ++it) {
        EV << "\t";
        //print LSP ID
        EV << (*it)->LSP->getLspID();

        EV << "\t0x";

        //print sequence number
        EV << std::setfill('0') << std::setw(8) << std::hex
                  << (*it)->LSP->getSeqNumber();
        EV << "\t" << std::setfill('0') << std::setw(5) << std::dec
                  << (*it)->LSP->getRemLifeTime() << endl;
        //EV <<"SeqNum: " << (*it)->LSP->getSeqNumber()<<endl;

        TLV_t *tmpTlv;

        //print neighbours
        for (unsigned int k = 0;
                (tmpTlv = this->getTLVByType((*it)->LSP.get(), IS_NEIGHBOURS_LSP, k))
                        != NULL; k++) {
            for (unsigned int m = 1; m + 11 <= tmpTlv->length; m += 11) {
                EV << "\t\t";
                for (unsigned int l = 0; l < 7; l++) {
                    //1 = virtual flag, m = current neighbour record, 4 is offset in current neigh. record(start LAN-ID)
                    EV << std::setfill('0') << std::setw(2) << std::dec
                              << (unsigned int) tmpTlv->value[m + 4 + l];
                    if (l % 2 == 1)
                        EV << ".";
                }

                EV << "\tmetric: " << std::setfill('0') << std::setw(2)
                          << std::dec << (unsigned int) tmpTlv->value[m + 0]
                          << endl;
            }

        }

    }

    //TODO print L2 LSP DB
}
/*
 * Print contents of LSP to std::cout
 * @param lsp is LSP packet
 * @param from is description where from this print has been called.
 */
void ISISMain::printLSP(Ptr<ISISLSPPacket>lsp, char *from) {
    std::cout << "PrintLSP called from: " << from << endl;
    LspID lspId = lsp->getLspID();
    std::cout << "Printing LSP: " << lspId;
//    this->printLspId(lspId);
    std::cout << "Print LSP->test:";
    std::cout << "seqNum: " << lsp->getSeqNumber() << endl;
    std::cout << "Length of TLVarray: " << lsp->getTLVArraySize() << endl;
    std::cout << "TLV: " << endl;
    for (unsigned int i = 0; i < lsp->getTLVArraySize(); i++) {
        std::cout << "Type: " << (unsigned short) lsp->getTLV(i).type << endl;
        std::cout << "Length: " << (unsigned short) lsp->getTLV(i).length
                << endl;
    }

    TLV_t *tmpTlv;
    for (unsigned int k = 0;
            (tmpTlv = this->getTLVByType(lsp.get(), IS_NEIGHBOURS_LSP, k)) != NULL;
            k++) {
        std::cout << "Start printing TLV of length: "
                << (unsigned int) tmpTlv->length << endl;
        for (unsigned int m = 0; (m + 11) < tmpTlv->length; m += 11) {
            std::cout << "LAN-ID:";
            for (unsigned int l = 0; l < 7; l++) {
                //1 = virtual flag, m = current neighbour record, 4 is offset in current neigh. record(start LAN-ID)
                std::cout << (unsigned short) tmpTlv->value[1 + m + 4 + l];
                if (l % 2 == 1)
                    std::cout << ".";
            }

            std::cout << "\t metric: "
                    << (unsigned short) tmpTlv->value[1 + m + 0] << endl;
        }
    }
}

void ISISMain::printLspId(LspID lspId) {

    std::cout << lspId;
//    std::cout << std::setfill('0') <<  std::setw(2) << std::dec << (unsigned int) lspId[ISIS_SYSTEM_ID];
//    std::cout << "-";
//    std::cout << std::setfill('0') <<  std::setw(2) << std::dec << (unsigned int) lspId[ISIS_SYSTEM_ID + 1] << endl;
}

void ISISMain::printSysId(SystemID sysId) {

    //print system id
    std::cout << sysId;

}

/**
 * Compare two unsigned char arrays and return result of comparison.
 * @param first First unsigned char array to be compared
 * @param second First unsigned char array to be compared
 * @param size Size of two arrays
 * @return True if arrays match, false otherwise.
 */
bool ISISMain::compareArrays(unsigned char * first, unsigned char * second,
        unsigned int size) {
    bool result = true;
    for (unsigned int i = 0; i < size; ++i) {
        if (first[i] != second[i])
            result = false;
    }

    return result;
}

/**
 * Copy content of one unsigned char array to another.
 * @param src Source array to be copied from.
 * @param dst Destination array to be copied to.
 * @param size Size in bytes co be copied.
 * @param startSrc Position in first array, where copying has to be started (usually 0).
 * @param startDst Position in second, where data are saved.
 */
void ISISMain::copyArrayContent(unsigned char * src, unsigned char * dst,
        unsigned int size, unsigned int startSrc, unsigned int startDst) {
    for (unsigned int i = 0; i < size; i++) {
        dst[i + startDst] = src[i + startSrc];
    }
}

/**
 * Remove dead neighbour, when timer expires. Remove neighbour from adjacency table (L1 or L2)
 * and start new DIS election process.
 * @param msg Timer associated with dead neighbour.
 */
void ISISMain::removeDeadNeighbour(ISISTimer *msg) {
    //TODO timer should also have an interface index and only adjacency with matching interface should be deleted
    //we can have multiple adjacencies with neighbour and only one link failed
    /* Check it and rewrite it */
    EV
              << "ISIS: Warning: RemoveDeadNeighbour: If everything is working correctly, this method shoudn't be called"
              << endl;

    //L1 neighbour dead
    // * remove from adjL1Table
    // * reset DIS on Ift

    if (msg->getIsType() == L1_TYPE) {
        for (unsigned int i = 0; i < adjL1Table.size();) {
            if (msg->getInterfaceId() == adjL1Table.at(i).interfaceId) {


                if (msg->getSysID() == adjL1Table.at(i).sysID) {
                    adjL1Table.erase(adjL1Table.begin() + i);
                } else {
                    i++;
                }
            } else {
                i++;
            }
        }
        std::sort(this->adjL1Table.begin(), this->adjL1Table.end());

        LspID lspId = msg->getLSPid();
        this->purgeRemainLSP(lspId, msg->getIsType());

        if (getIfaceById(msg->getInterfaceId())->network) {
            //lspId has two bytes more than we need (that does no harm)
            this->resetDIS(lspId.getPseudonodeID(), msg->getInterfaceId(),
                    msg->getIsType());
        }
//        delete lspId;

    }

    //else it's L2 dead neighbour
    // * remove from adjL2Table
    else {
        for (unsigned int i = 0; i < adjL2Table.size(); i++) {
            if (msg->getInterfaceId() == adjL1Table.at(i).interfaceId) {
                bool found = true;
                found = msg->getSysID() == adjL2Table.at(i).sysID;


                //XXX WHY must area ID also match??

                //area ID must also match
                found &= msg->getAreaID() == adjL2Table.at(i).areaID;


                if (found) {
                    adjL2Table.erase(adjL2Table.begin() + i);
                }
            }
        }
        std::sort(this->adjL2Table.begin(), this->adjL2Table.end());

        LspID lspId = msg->getLSPid();
        this->purgeRemainLSP(lspId, msg->getIsType());

        if (getIfaceById(msg->getInterfaceId())->network) {
            //lspId has two bytes more than we need (that does no harm)
            this->resetDIS(lspId, msg->getInterfaceId(), msg->getIsType());
        }

//        delete lspId;

        //update att flag, since we removing neighbour, try to clear it -> sending false as action
        this->updateAtt(false);
    }

}
/**
 * Performs DIS election
 * @param msg is received LAN Hello PDU
 */
void ISISMain::electDIS(Ptr<ISISLANHelloPacket> msg) {

    short circuitType;
    PseudonodeID disID;
    unsigned char disPriority;
    int interfaceId;
    ISISinterface* iface;

    //TODO ANSAINET4.0 Uncomment
      interfaceId = msg->getTag<InterfaceInd>()->getInterfaceId();// = ift->getInterfaceById(ctrl->getInterfaceId())->getNetworkLayerGateIndex();


    iface = this->getIfaceById(interfaceId);

    //set circuiType according to LAN Hello Packet type
    if (msg->getType() == LAN_L1_HELLO || msg->getType() == TRILL_HELLO) {
        circuitType = L1_TYPE;
        disID = iface->L1DIS;
        disPriority = iface->L1DISpriority;
    } else if (msg->getType() == LAN_L2_HELLO) {
        circuitType = L2_TYPE;
        disID = iface->L2DIS;
        disPriority = iface->L2DISpriority;
    }

    PseudonodeID msgLanID;
    msgLanID = msg->getLanID();

    if (msgLanID == disID) {
        //LAN DIS in received message is the same as the one that THIS Intermediate System has
        return;
    }


    PseudonodeID lastDIS;
    lastDIS = disID;
//    this->copyArrayContent(disID, lastDIS, ISIS_SYSTEM_ID + 1, 0, 0);

    MacAddress localDIS, receivedDIS;
    ISISadj *tmpAdj;

    //first old/local DIS
    //if DIS == me then use my MAC for specified interface
    if (this->amIL1DIS(interfaceId)) {
        localDIS = iface->entry->getMacAddress();
    }
    //else find adjacency and from that use MAC
    else {
        if ((tmpAdj = this->getAdjBySystemID(lastDIS, circuitType, interfaceId))
                != NULL) {
            localDIS = tmpAdj->mac;
        } else {
            EV << "deviceId: " << deviceId
                      << " ISIS: Warning: Didn't find adjacency for local MAC comparison in electL1DesignatedIS "
                      << endl;
            localDIS = MacAddress("000000000000");
        }
    }

    //find out MAC address for IS with LAN-ID from received message
    if ((tmpAdj = this->getAdjBySystemID(msgLanID, circuitType, interfaceId))
            != NULL) {
        receivedDIS = tmpAdj->mac;
    } else {
        EV << "deviceId: " << deviceId
                  << " ISIS: Warning: Didn't find adjacency for received MAC comparison in electL1DesignatedIS "
                  << endl;
        receivedDIS = MacAddress("000000000000");
    }

    //if announced DIS priority is higher then actual one or if they are equal and src MAC is higher than mine, then it's time to update DIS
    if (((msg->getPriority() > disPriority)
            || (msg->getPriority() == disPriority
                    && (receivedDIS.compareTo(localDIS) > 0)))) {
        LspID disLspID;
        disLspID.set(lastDIS);

        //purge lastDIS's LSPs
        this->purgeRemainLSP(disLspID, circuitType);



        if (msg->getType() == LAN_L1_HELLO || msg->getType() == TRILL_HELLO) {

            iface->L1DIS = msgLanID;
            iface->L1DISpriority = msg->getPriority();;
        } else if (msg->getType() == LAN_L2_HELLO) {

            iface->L2DIS = msgLanID;
            iface->L2DISpriority = msg->getPriority();;
        }



        if (mode == L2_ISIS_MODE) {
            //TODO B1 SEVERE call trillDIS() -> such method would appoint forwarder and handled other TRILL-DIS related duties
//            TrillInterfaceData *trillD = ift->getInterfaceByNetworkLayerGateIndex(interfaceId)->trillData();
//            trillD->clearAppointedFWDs();
            //when appointed forwarder observes that DRB on link has changet, it NO LONGER considers itselft as app fwd

        }
//        delete[] disLspID;
    }

    //clean
//    delete[] msgLanID;
//    delete[] lastDIS;
}

/**
 * Reset DIS on all interfaces. New election occurs after reset.
 * @see electL1DesignatedIS(ISISL1HelloPacket *msg)
 * @param IStype Defines IS type - L1 or L2
 */
void ISISMain::resetDIS(PseudonodeID pseudoID, int interfaceId,
        short circuitType) {

    ISISinterface *iface = getIfaceById(interfaceId);
    if (circuitType == L1_TYPE || circuitType == L1L2_TYPE) {
        //if the systemID was DIS, then set DIS = me. if dead neighbour wasn't DIS do nothing.
        if (pseudoID == iface->L1DIS) {
            //set myself as DIS
            iface->L1DISpriority = iface->priority;
            //set initial designated IS as himself
            iface->L1DIS.set(systemId, interfaceId);
//            this->copyArrayContent((unsigned char*) this->sysId, iface->L1DIS, ISIS_SYSTEM_ID, 0, 0);
            //set LAN identifier; -99 is because, OMNeT starts numbering interfaces from 100 -> interfaceID 100 means LAN ID 0; and we want to start numbering from 1
//            iface->L1DIS[ISIS_SYSTEM_ID] = interfaceIndex + 1;
        }
    }

    if (circuitType == L2_TYPE || circuitType == L1L2_TYPE) {
        //if the systemID was DIS, then set DIS = me. if dead neighbour wasn't DIS do nothing.
        if (pseudoID == iface->L2DIS) {
            //set myself as DIS
            iface->L2DISpriority = iface->priority;
            //set initial designated IS as himself
            iface->L2DIS.set(systemId, interfaceId);
//            this->copyArrayContent((unsigned char*) this->sysId, iface->L2DIS, ISIS_SYSTEM_ID, 0, 0);
//            //set LAN identifier; -99 is because, OMNeT starts numbering interfaces from 100 -> interfaceID 100 means LAN ID 0; and we want to start numbering from 1
//            iface->L2DIS[ISIS_SYSTEM_ID] = interfaceIndex + 1;
        }
    }
}

short ISISMain::getLevel(const ISISMessage* msg) {
    switch (msg->getType()) {
    case (LAN_L1_HELLO):
    case (L1_LSP):
    case (L1_CSNP):
    case (L1_PSNP):
        return L1_TYPE;
        break;

    case (LAN_L2_HELLO):
    case (L2_LSP):
    case (L2_CSNP):
    case (L2_PSNP):
        return L2_TYPE;
        break;

    default:
        EV << "deviceId " << deviceId
                  << ": ISIS: WARNING: Unsupported Message Type in getLevel"
                  << endl;

        break;

    }
    return L1_TYPE;
}

/*
 * Handles L1 LSP according to ISO 10589 7.3.15.1
 * @param lsp is received LSP
 */
void ISISMain::handleLsp(Packet *packet) {

    /*
     * Verify that we have adjacency UP for Source-ID in lsp
     * if the lsp is received on broadcast circuit compare senders MAC address with MAC address in adjacency
     * We don't need to check Area address, it was done by verifying adjacency
     * if remainingLifetime == 0 -> purgeLSP
     * if lsp->getLspId() == this->sysID //if it's my LSP
     *  and i don't have it anymore in my lspDb, then init network wide purge ... purgeLSP()
     *  if it's my LSP and I have it in DB perform 7.3.16.1 -> they have old version so send new version to that link
     *  source is somebody else:
     * received lsp is newer or i don't have it in DB => installLSP
     *   set SRMflags and clear it for incomming interface
     *   if received on PTP set SSNflag for that interface and clear other
     * received lsp is equal(same lsp-ID, seqNumber, remLifetime both zero or both non-zero)
     *   clear SRM flag for C
     *   if C is PTP set SSN
     * received is older than in DB
     *   set SRM flag for C
     *   clear SSN flag for C
     *

     * Process PATT.. flag (IS Type, overload, etc.)

     *
     * this->updateLSP(lsp, L1_TYPE);
     */

    auto lsp = packet->peekAtFront<ISISLSPPacket>();
    /* 7.3.15.1. */
    LspID lspID;
    int circuitType = this->getLevel(lsp.get());

    int interfaceId = packet->getTag<InterfaceInd>()->getInterfaceId();// = ift->getInterfaceById(ctrl->getInterfaceId())->getNetworkLayerGateIndex();

//    int interfaceId = lsp->getArrivalGate()->getIndex();
    ISISinterface *iface = this->getIfaceById(interfaceId);

    /* 7.3.15.1. a) 6) */
    //returns true if for source-id in LSP there is adjacency with matching MAC address
    if (!this->isAdjUp(packet, circuitType)) {
        //no adjacency for source-id => discard lsp
        //generate event?

        EV
                  << "ISIS: Warning: Discarding LSP: didn't find adjacency in state UP for this LSP"
                  << endl;

        delete packet;
        return;
    }
//    this->schedulePeriodicSend(circuitType);

    lspID = lsp->getLspID();

    /* 7.3.15.1. b */
    if (lsp->getRemLifeTime() == 0) {

        this->purgeLSP(packet, circuitType);
        /* lsp is already deleted in purgeLSP */
        //delete lsp;
//        delete[] lspID;
        return;

    }
    LSPRecord *lspRec;
    //is it my LSP?
    if (lspID.getSystemId().getSystemId() == systemId.getSystemId()) {

        //if i don't have it anymore
        /* 7.3.15.1 c) i don't have it in DB */
        if ((lspRec = this->getLSPFromDbByID(lspID, circuitType)) == NULL) {
            //init network wide purge
            this->purgeLSP(packet, circuitType);
            //delete lsp;
//            delete lspID;
            return;
        } else {
            /* 7.3.15.1 c) OR no longer in the set of LSPs generated by this system */
            if (lspRec->deadTimer->getTimerKind() == LSP_DELETE_TIMER) //TODO improve this
                    {/*if the deadTimer is set to xxLSP_DELETE, then it's already purged
                     * and in database is kept just header.
                     */
                /* 7.3.15.1 c) */
                this->purgeLSP(packet, circuitType);
//                delete lspID;
                return;
            }
            /* 7.3.15.1 d) */
            //if we have it
            /* 7.3.16.1 sequence numbers */
            if (lsp->getSeqNumber() > lspRec->LSP->getSeqNumber()) {
//                std::cout<<"handle seqNum: "<< lspRec->LSP->getSeqNumber() <<endl;
                lspRec->LSP->setSeqNumber(lsp->getSeqNumber() + 1); //TODO handle overflow of seqNumer
//                std::cout<<"handle seqNum: "<< lspRec->LSP->getSeqNumber() <<endl;
                this->setSRMflags(lspRec, circuitType);
                //TODO set new remaining lifetime
                //TODO reschedule deadTimer

            } else {


                delete packet;
                return;
            }

        }
    } else {
        /* 7.3.15.1 e) 1) */
        lspRec = this->getLSPFromDbByID(lspID, circuitType);
        if (lspRec == NULL) {
            /* 7.3.15.1 e) 1) i. */
            auto tmplsp = packet->removeAtFront<ISISLSPPacket>();
//            tmplsp->addTagIfAbsent<InterfaceInd>()->setInterfaceId(packet->getTag<InterfaceInd>()->getInterfaceId());
            this->installLSP(tmplsp, circuitType);
            this->schedulePeriodicSend(circuitType);
//            delete[]   lspID;
            delete packet;
            return;

        } else {
            if (lsp->getSeqNumber() > lspRec->LSP->getSeqNumber()) {
                /* 7.3.15.1 e) 1) i. */
                auto tmplsp = packet->removeAtFront<ISISLSPPacket>();
//                tmplsp->addTag<InterfaceInd>()->setInterfaceId(packet->getTag<InterfaceInd>()->getInterfaceId());
                this->replaceLSP(tmplsp, lspRec, circuitType);
                this->schedulePeriodicSend(circuitType);
//                delete[] lspID;
                delete packet;
                return;

            }
            /* 7.3.15.1 e) 2) */
            /* should check also lsp->getRemLifetime != 0  OR both zero, but this is handled in purgeLSP*/
            else if (lsp->getSeqNumber() == lspRec->LSP->getSeqNumber()
                    && lspRec->LSP->getRemLifeTime() != 0) {

                /* 7.3.15.1 e) 2) i. */
                this->clearSRMflag(lspRec, interfaceId, circuitType);

                if (!getIfaceById(interfaceId)->network) {
                    /* 7.3.15.1 e) 2) ii. */
                    this->setSSNflag(lspRec, interfaceId, circuitType);
                }

            }
            /* 7.3.15.1 e) 3) */
            else if (lsp->getSeqNumber() < lspRec->LSP->getSeqNumber()) {
                /* 7.3.15.1 e) 3) i. */
                this->setSRMflag(lspRec, interfaceId, circuitType);
                /* 7.3.15.1 e) 3) ii. */
                this->clearSSNflag(lspRec, interfaceId, circuitType);
            }
        }

    }

    delete packet;
}

/*
 * Create and send CSNP message to DIS interface.
 * @param timer is specific CSNP timer
 */
void ISISMain::sendCsnp(ISISTimer *timer) {
    //TODO don't know how to handle csnp over PtP yet (there is no periodic sending, but initial csnp is sent)
    /* Maybe send CSNP during some initial interval (or number of times, or just once) and then just don't re-schedule timer for this interface */
//    if (this->ISISIft.at(timer->getInterfaceIndex()).network
//            && (false && !this->ISISIft.at(timer->getInterfaceIndex()).network
//                    || !this->amIDIS(timer->getInterfaceIndex(),
//                            timer->getIsType()))) {
//        this->schedule(timer);
//        return;
//    }

    ISISinterface *iface = getIfaceById(timer->getInterfaceId());
//    unsigned char * disID;
    short circuitType = timer->getIsType();

    std::vector<LSPRecord *> *lspDb = this->getLSPDb(circuitType);
    std::vector<LSPRecord *>::iterator it = lspDb->begin();
    for (int fragment = 0; it != lspDb->end(); fragment++)

    {
        Packet *packet = new Packet("CSNP");
        auto header = makeShared<ISISCSNPPacket>();
        if (circuitType == L1_TYPE) {
            header->setType(L1_CSNP);
//            disID = iface->L1DIS;
        } else if (circuitType == L2_TYPE) {
            header->setType(L2_CSNP);
//            disID = iface->L2DIS;
        } else {
            header->setType(L1_CSNP);
//            disID = iface->L1DIS;
            EV << "deviceId " << deviceId
                      << ": ISIS: WARNING: Setting unknown CSNP packet type "
                      << endl;
        }
        header->setLength(0); //TODO set to length of header


//        // set DSAP & NSAP fields
        auto sapTag = packet->addTag<Ieee802SapReq>();
        sapTag->setDsap(SAP_CLNS);
        sapTag->setSsap(SAP_CLNS);

        packet->addTag<PacketProtocolTag>()->setProtocol(&Protocol::isis);

        //set destination broadcast address
        //It should be multicast 01-80-C2-00-00-14 MAC address, but it doesn't work in OMNeT
        MacAddress ma;
        if (iface->circuitType == L1_TYPE)
        {
            ma.setAddress(ISIS_ALL_L1_IS);
        }
        else
        {
            ma.setAddress(ISIS_ALL_L2_IS);
        }


        auto macTag = packet->addTag<MacAddressReq>();
        macTag->setDestAddress(ma);

        auto interfaceTag = packet->addTag<InterfaceReq>();
        interfaceTag->setInterfaceId(iface->entry->getInterfaceId());


        //set system ID field which consists of my system id + zero circuit id inc this case
        //last octet has to be 0 see 9.11 at ISO 10589:2002(E)
        header->setSourceID(PseudonodeID(systemId, 0));

        int lspCount = lspDb->end() - it;

        //TODO set start LSP-ID
        if (fragment != 0) {
            header->setStartLspID((*it)->LSP->getLspID());
        } else {
            header->setStartLspID(LspID());
        }

        for (; it != lspDb->end() && header->getLength() < ISIS_LSP_MAX_SIZE;) {

            TLV_t myTLV;
            myTLV.type = LSP_ENTRIES;

            if (lspCount * 16 > 255) { //TODO replace "16" with something more appropriate
                myTLV.length = 255 - (255 % 16); //TODO minus size of header
            } else {
                myTLV.length = lspCount * 16;
            }
            myTLV.value = new unsigned char[myTLV.length];
            myTLV.length = 0;

            for (int i = 0;
                    (myTLV.length + 16) < 255 && it != lspDb->end()
                            && (header->getLength() + (myTLV.length + 16) + 2)
                                    < ISIS_LSP_MAX_SIZE; ++it, i++) { //255 is maximum that can fit into Length field of TLV
                                                                      //add entry from lspDb
                                                                      //convert unsigned short to unsigned char array and insert to TLV
                myTLV.value[(i * 16)] = (((*it)->LSP->getRemLifeTime() >> 8)
                        & 0xFF);
                myTLV.value[(i * 16) + 1] =
                        ((*it)->LSP->getRemLifeTime() & 0xFF);

                //copy LSP ID to TLV
                LspID lspId = (*it)->LSP->getLspID();
                this->copyArrayContent(lspId.toTLV(), myTLV.value,
                        ISIS_SYSTEM_ID + 2, 0, (i * 16) + 2);
//                delete[] lspId;
                //convert unsigned long seq number to unsigned char array[4] and insert into TLV
                for (unsigned int j = 0; j < 4; j++) {
                    myTLV.value[(i * 16) + 10 + j] = ((*it)->LSP->getSeqNumber()
                            >> (24 - (8 * j))) & 0xFF;
                }

                //set end LSP-ID

                myTLV.length += 16;
                //packet->setLength(packet->getLength() + 16);
            }
            //int tlvSize = packet->getTLVArraySize();
            this->addTLV(header, &myTLV);
            header->setLength(header->getLength() + myTLV.length + 2);

            delete[] myTLV.value;

        }
        if (it != lspDb->end()) //if there is still another lsp in DB
                {
            for (unsigned int i = 0; i < ISIS_SYSTEM_ID + 2; i++) {
                header->setEndLspID((*it)->LSP->getLspID());
//                packet->setEndLspID(i, (*it)->LSP->getLspID(i));
            }
        } else
        //this was last lsp, so mark it as last with LSP-ID FFFFF...
        {
            for (unsigned int i = 0; i < ISIS_SYSTEM_ID + 2; i++) {
                LspID endLsp;
                endLsp.setMax();
                header->setEndLspID(endLsp);
//                packet->setEndLspID(i, 255);
            }

        }

        //send only on interface specified in timer

        packet->insertAtFront(header);
        sendDown(packet);

        EV << "ISIS::sendCSNP: Source-ID: " << systemId;
        EV << endl;

    }

    //reschedule timer
    this->schedule(timer);

}

/*
 * Create and send PSNP message to DIS interface.
 * @param timer is specific PSNP timer
 */
void ISISMain::sendPsnp(ISISTimer *timer) {


    int interfaceId;
    short circuitType;
//    unsigned char * disID;
    ISISinterface * iface;
    FlagRecQ_t *SSNQueue;

    interfaceId = timer->getInterfaceId();
    circuitType = timer->getIsType();
    iface = getIfaceById(interfaceId);
    SSNQueue = this->getSSNQ(iface->network, interfaceId, circuitType);

    if (this->amIDIS(interfaceId, circuitType)) {
        //reschedule OR make sure that during DIS election/resignation this timer is handled correctly
        //this is a safer, but dumber, solution
        //TODO see above
        this->schedule(timer);
        return;

    }

    //if queue is empty reschedule timer
    if (SSNQueue == nullptr || SSNQueue->empty()) {
        this->schedule(timer);
        return;
    }

    Packet *packet = new Packet("PSNP");
    auto header = makeShared<ISISPSNPPacket>();
    if (circuitType == L1_TYPE) {
        header->setType(L1_PSNP);
//        disID = iface->L1DIS;
    } else if (circuitType == L2_TYPE) {
        header->setType(L2_PSNP);
//        disID = iface->L2DIS;
    } else {
        header->setType(L1_PSNP);
//        disID = iface->L1DIS;
        EV << "deviceId " << deviceId
                  << ": ISIS: WARNING: Setting unknown PSNP packet type "
                  << endl;
    }
    header->setLength(0); //TODO set to length of header

    //TODO Proper L2 encap

//    // set DSAP & NSAP fields
    auto sapTag = packet->addTag<Ieee802SapReq>();
    sapTag->setDsap(SAP_CLNS);
    sapTag->setSsap(SAP_CLNS);

    packet->addTag<PacketProtocolTag>()->setProtocol(&Protocol::isis);

    //set destination broadcast address
    //It should be multicast 01-80-C2-00-00-14 MAC address, but it doesn't work in OMNeT
    MacAddress ma;
    ma.setAddress("ff:ff:ff:ff:ff:ff");
    if (iface->circuitType == L1_TYPE)
    {
        ma.setAddress(ISIS_ALL_L1_IS);
    }
    else
    {
        ma.setAddress(ISIS_ALL_L2_IS);
    }

    auto macTag = packet->addTag<MacAddressReq>();
     macTag->setDestAddress(ma);


     //set interfaceID
     auto interfaceTag = packet->addTag<InterfaceReq>();
     interfaceTag->setInterfaceId(iface->entry->getInterfaceId());

    //set system ID field which consists of my system id + zero circuit id in this case
    header->setSourceID(PseudonodeID(systemId, 0));


    //TODO check that all fields of the packet are filled correctly

    unsigned int lspCount = SSNQueue->size();
    for (FlagRecQ_t::iterator it = SSNQueue->begin(); it != SSNQueue->end();) {

        TLV_t myTLV;
        myTLV.type = LSP_ENTRIES;

        if (lspCount * 16 > 255) { //TODO replace "16" with something more appropriate
            myTLV.length = 255 - (255 % 16); //TODO minus size of header
        } else {
            myTLV.length = lspCount * 16;
        }
        myTLV.value = new unsigned char[myTLV.length];
        myTLV.length = 0;

        for (int i = 0;
                (myTLV.length + 16) < 255 && it != SSNQueue->end()
                        && (header->getLength() + (myTLV.length + 16) + 2)
                                < ISIS_LSP_MAX_SIZE;
                it = SSNQueue->begin(), i++) { //255 is maximum that can fit into Length field of TLV
                                               //add entry from lspDb

            //convert unsigned short to unsigned char array and insert to TLV
            myTLV.value[(i * 16)] = (((*it)->lspRec->LSP->getRemLifeTime() >> 8)
                    & 0xFF);
            myTLV.value[(i * 16) + 1] = ((*it)->lspRec->LSP->getRemLifeTime()
                    & 0xFF);

            //copy LSP ID to TLV
            LspID lspId = (*it)->lspRec->LSP->getLspID();
            this->copyArrayContent(lspId.toTLV(), myTLV.value,
                    ISIS_SYSTEM_ID + 2, 0, (i * 16) + 2);
//            delete[] lspId;
            //convert unsigned long seq number to unsigned char array[4] and insert into TLV
            for (unsigned int j = 0; j < 4; j++) {
                myTLV.value[(i * 16) + 10 + j] =
                        ((*it)->lspRec->LSP->getSeqNumber() >> (24 - (8 * j)))
                                & 0xFF;
            }

            //set end LSP-ID

            myTLV.length += 16;
            //packet->setLength(packet->getLength() + 16);

            //it should wait after sending the lsp, but let's just assume it will work fine
            //clear SSNflag, this should also remove the flagRecord from correct queue(vector
            this->clearSSNflag((*it)->lspRec, (*it)->interfaceId, circuitType);
        }
        //int tlvSize = packet->getTLVArraySize();
        this->addTLV(header, &myTLV);
        header->setLength(header->getLength() + myTLV.length + 2);
        delete[] myTLV.value;

    }

    packet->insertAtFront(header);
    sendDown(packet);

    EV << "ISIS::sendPSNP: Source-ID: " << systemId;
    EV << endl;

    this->schedule(timer);
}

/*
 * Handle incomming psnp message according to ISO 10589 7.3.15.2
 * @param psnp is incomming PSNP message
 */
void ISISMain::handlePsnp(Packet* packet) {

    auto psnp = packet->peekAtFront<ISISPSNPPacket>();
    short circuitType = this->getLevel(psnp.get());

    int interfaceId = packet->getTag<InterfaceInd>()->getInterfaceId();
    ISISinterface* iface = this->getIfaceById(interfaceId);


    /* 7.3.15.2. a) If circuit C is a broadcast .. */
    if (iface->network && !this->amIDIS(interfaceId, circuitType)) {
        EV << "ISIS: Warning: Discarding PSNP. Received on nonDIS interface."
                  << endl;

        delete packet;
        return;
    }

    /* 7.3.15.2 a) 6) */
    /* we handle broadcast and non-broadcast adjacencies same way */
    if (!this->isAdjUp(packet, circuitType)) {
        EV << "ISIS: Warning: Discarding PSNP. Didn't find matching adjacency."
                  << endl;

        delete packet;
        return;
    }

    /* 7.3.15.2 a) 7) */
    /* 7.3.15.2 a) 8) */
    /* Authentication is omitted */

    /* 7.3.15.2 b) */
    TLV_t * tmpTlv;
    LspID tmpLspID;
    for (int offset = 0;
            (tmpTlv = this->getTLVByType(psnp.get(), LSP_ENTRIES, offset)) != NULL;
            offset++) {

        for (int i = 0; i < tmpTlv->length; i += 16) //TODO change 16 to something
                {
//            tmpLspID = new unsigned char[ISIS_SYSTEM_ID + 2];
            tmpLspID.fromTLV(&(tmpTlv->value[i + 2]));
//            this->copyArrayContent(tmpTlv->value, tmpLspID, (ISIS_SYSTEM_ID + 2), i + 2, 0);
            unsigned short remLife = tmpTlv->value[i] * 255
                    + tmpTlv->value[i + 1];
            /* this just makes me laugh */
            unsigned long seqNum = tmpTlv->value[i + 10] * 255 * 255 * 255
                    + tmpTlv->value[i + 11] * 255 * 255
                    + tmpTlv->value[i + 12] * 255 + tmpTlv->value[i + 13];
            //getLspBySysID
            LSPRecord *lspRec;
            lspRec = this->getLSPFromDbByID(tmpLspID, circuitType);
            if (lspRec != NULL) {
                /* 7.3.15.2 b) 2) */
                //if values are same
                if (seqNum == lspRec->LSP->getSeqNumber()
                        && (remLife == lspRec->LSP->getRemLifeTime()
                                || (remLife != 0
                                        && lspRec->LSP->getRemLifeTime() != 0))) {
                    //if non-broadcast -> clear SRMflag for C
                    if (!iface->network) {
                        this->clearSRMflag(lspRec, interfaceId, circuitType);
                    }
                }
                /* 7.3.15.2 b) 3) */
                //else if received is older
                else if (seqNum < lspRec->LSP->getSeqNumber()) {
                    //clean SSN and set SRM
                    this->clearSSNflag(lspRec, interfaceId, circuitType);
                    this->setSRMflag(lspRec, interfaceId, circuitType);
                }
                /* 7.3.15.2 b) 4) */
                //else if newer
                else if (seqNum > lspRec->LSP->getSeqNumber()
                        || (seqNum == lspRec->LSP->getSeqNumber()
                                && lspRec->LSP->getRemLifeTime() != 0)) {
                    //setSSNflag AND if C is non-broadcast clearSRM
                    this->setSSNflag(lspRec, interfaceId, circuitType);
                    if (!iface->network) {
                        this->clearSRMflag(lspRec, interfaceId, circuitType);
                    }
                }
            }
            /* 7.3.15.2 b) 5) */
            else {
                //if remLifetime, checksum, seqNum are all non-zero
                if (remLife != 0 && seqNum != 0) {
                    this->printSysId(systemId);
//                    std::cout<<"Received new LSP in PSNP";
//                    this->printLspId(tmpLspID);
                    //create LSP with seqNum 0 and set SSNflag for C
                    //DO NOT SET SRMflag!!!!!!!
                    Ptr<ISISLSPPacket> lsp;
                    if (circuitType == L1_TYPE) {
                        lsp = makeShared<ISISLSPPacket>();
                        lsp->setType(L1_LSP);
                        ///set PATTLSPDBOLIS
                        lsp->setPATTLSPDBOLIS(0x02 + (this->att * 0x10)); //only setting IS type = L1 (TODO A2)
                    } else if (circuitType == L2_TYPE) {
                        lsp = makeShared<ISISLSPPacket>();
                        lsp->setType(L2_LSP);
                        //set PATTLSPDBOLIS
                        lsp->setPATTLSPDBOLIS(0x0A); //bits 1(2) and 3(8) => L2
                    }

                    //remLifeTime
                    lsp->setRemLifeTime(remLife);
                    //lspID[8]
                    lsp->setLspID(tmpLspID);
//                    this->setLspID(lsp, tmpLspID);

                    //set seqNum
                    lsp->setSeqNumber(0);

                    //install new "empty" LSP and set SSNflag
                    this->setSSNflag(this->installLSP(lsp, circuitType), interfaceId, circuitType);

                }
            }
//            delete[] tmpLspID;
        }

    }
    //if lsp-entry equals

    delete packet;
}

/*
 * Handles incomming CSNP message according to ISO 10589 7.3.15.2
 * @param csnp is incomming CSNP message
 */
void ISISMain::handleCsnp(Packet* packet) {

    auto csnp = packet->peekAtFront<ISISCSNPPacket>();
    short circuitType = this->getLevel(csnp.get()); //L1_TYPE; //TODO get circuitType from csnp

    int interfaceId = packet->getTag<InterfaceInd>()->getInterfaceId();

    ISISinterface* iface = this->getIfaceById(interfaceId);

    /* 7.3.15.2 a) 6) */
    /* we handle broadcast and non-broadcast adjacencies same way */
    if (!this->isAdjUp(packet, circuitType)) {
        EV << "ISIS: Warning: Discarding CSNP. Didn't find matching adjacency."
                  << endl;

        delete packet;
        return;
    }

    /* 7.3.15.2 a) 7) */
    /* 7.3.15.2 a) 8) */
    /* Authentication is omitted */

    std::vector<LspID>* lspRange;
    lspRange = this->getLspRange(csnp->getStartLspID(), csnp->getEndLspID(),
            circuitType);

    //for -> iterate over tlvArraySize and then iterate over TLV
//    for(int i = 0; i < csnp->getTLVArraySize(); i++)
    /* 7.3.15.2 b) */
    TLV_t * tmpTlv;
    LspID tmpLspID;
    for (int offset = 0;
            (tmpTlv = this->getTLVByType(csnp.get(), LSP_ENTRIES, offset)) != NULL;
            offset++) {

        for (int i = 0; i + 16 <= tmpTlv->length; i += 16) //TODO change 16 to something
                {
            tmpLspID.fromTLV(&(tmpTlv->value[i + 2]));
//            tmpLspID = new unsigned char[ISIS_SYSTEM_ID + 2];
//            this->copyArrayContent(tmpTlv->value, tmpLspID, (ISIS_SYSTEM_ID + 2), i + 2, 0);
            unsigned short remLife = tmpTlv->value[i] * 255
                    + tmpTlv->value[i + 1];
            /* this just makes me laugh */
            unsigned long seqNum = tmpTlv->value[i + 10] * 255 * 255 * 255
                    + tmpTlv->value[i + 11] * 255 * 255
                    + tmpTlv->value[i + 12] * 255 + tmpTlv->value[i + 13];

            /* 7.3.15.2 c) */
            if (!lspRange->empty()) {
                while (lspRange->front() < tmpLspID) {
                    this->setSRMflag(
                            this->getLSPFromDbByID(lspRange->front(),
                                    circuitType), interfaceId, circuitType);
//                    delete lspRange->front();
                    lspRange->erase(lspRange->begin());
                }

                if (lspRange->front() == tmpLspID) {
//                    delete lspRange->front();
                    lspRange->erase(lspRange->begin());
                }
            }

            //getLspBySysID
            LSPRecord *lspRec;
            lspRec = this->getLSPFromDbByID(tmpLspID, circuitType);
            if (lspRec != NULL) {
                //if values are same
                if (seqNum == lspRec->LSP->getSeqNumber()
                        && (remLife == lspRec->LSP->getRemLifeTime()
                                || (remLife != 0
                                        && lspRec->LSP->getRemLifeTime() != 0))) {
                    //if non-broadcast -> clear SRMflag for C
                    if (!iface->network) {
                        this->clearSRMflag(lspRec, interfaceId, circuitType);
                    }
                }
                //else if received is older
                else if (seqNum < lspRec->LSP->getSeqNumber()) {
                    //clean SSN and set SRM
                    this->clearSSNflag(lspRec, interfaceId, circuitType);
                    this->setSRMflag(lspRec, interfaceId, circuitType);
                }
                //else if newer
                else if (seqNum > lspRec->LSP->getSeqNumber()
                        || (seqNum == lspRec->LSP->getSeqNumber()
                                && lspRec->LSP->getRemLifeTime() != 0)) {
                    //setSSNflag AND if C is non-broadcast clearSRM
                    this->setSSNflag(lspRec, interfaceId, circuitType);
                    if (!iface->network) {
                        this->clearSRMflag(lspRec, interfaceId, circuitType);
                    }
                }
            }
            /* 7.3.15.2 b) 5) */
            else {
                //if remLifetime, checksum, seqNum are all non-zero
                if (remLife != 0 && seqNum != 0) {
                    this->printSysId(systemId);
//                    std::cout << "Received new LSP in CSNP";
//                    this->printLspId(tmpLspID);
                    //create LSP with seqNum 0 and set SSNflag for C
                    //DO NOT SET SRMflag!!!!!!!
                    Ptr<ISISLSPPacket> lsp;
                    if (circuitType == L1_TYPE) {
                        lsp = makeShared<ISISLSPPacket>();
                        lsp->setType(L1_LSP);
                        ///set PATTLSPDBOLIS
                        lsp->setPATTLSPDBOLIS(0x02 + (this->att * 0x10)); //only setting IS type = L1 (TODO A2)
                    } else if (circuitType == L2_TYPE) {
                        lsp = makeShared<ISISLSPPacket>();
                        lsp->setType(L2_LSP);
                        //set PATTLSPDBOLIS
                        lsp->setPATTLSPDBOLIS(0x0A); //bits 1(2) and 3(8) => L2
                    }
                    //remLifeTime
                    lsp->setRemLifeTime(remLife);
                    //lspID[8]
                    lsp->setLspID(tmpLspID);

                    //set seqNum
                    lsp->setSeqNumber(0);

                    //install new "empty" LSP and set SSNflag
                    this->setSSNflag(this->installLSP(lsp, circuitType),
                            interfaceId, circuitType);

                }
            }
            //TODO A! delete[] tmpLspID;
        }

    }

    while (!lspRange->empty()) {
        LSPRecord *tmpRec = this->getLSPFromDbByID(lspRange->front(),
                circuitType);
        if (tmpRec != NULL) {
            this->setSRMflag(
                    this->getLSPFromDbByID(lspRange->front(), circuitType),
                    interfaceId, circuitType);
        } else {
            EV << "ISIS: ERROR: run for your Life!!!!!" << endl;
        }
//TODO A!        delete lspRange->front();
        lspRange->erase(lspRange->begin());

    }
//TODO A!    delete lspRange;
    //if lsp-entry equals

    delete packet;
}

/*
 * Returns set of LSP-IDs in range from startLspID to endLspID
 * @param startLspID beginning of range
 * @param endLspID end of range
 * @return range of LSP-IDs
 */
std::vector<LspID>* ISISMain::getLspRange(LspID startLspID, LspID endLspID,
        short circuitType) {

    LspID lspID;
    std::vector<LSPRecord *> *lspDb = this->getLSPDb(circuitType);
    std::vector<LspID> *lspRange = new std::vector<LspID>;
    std::sort(lspDb->begin(), lspDb->end(), cmpLSPRecord());
    //TODO we can end the search before hitting lspDb->end when DB is sorted
    for (std::vector<LSPRecord *>::iterator it = lspDb->begin();
            it != lspDb->end(); ++it) {
        lspID = (*it)->LSP->getLspID();

        if (startLspID <= lspID && lspID <= endLspID) {
            lspRange->push_back(lspID);
        }
//        delete[] lspID;
    }

    return lspRange;

}

///*
// * Extracts Start LSP-ID from CSNP message.
// * @param csnp incoming CSNP message.
// * @return start LSP-ID.
// */
//unsigned char * ISIS::getStartLspID(ISISCSNPPacket *csnp)
//{
//
//    unsigned char *lspId = new unsigned char[ISIS_SYSTEM_ID + 2];
//
//    for (int i = 0; i < ISIS_SYSTEM_ID + 2; i++)
//    {
//        lspId[i] = csnp->getStartLspID(i);
//    }
//
//    return lspId;
//}
//
///*
// * Extracts End LSP-ID from CSNP message.
// * @param csnp incoming CSNP message.
// * @return end LSP-ID.
// */
//unsigned char * ISIS::getEndLspID(ISISCSNPPacket *csnp)
//{
//
//    unsigned char *lspId = new unsigned char[ISIS_SYSTEM_ID + 2];
//
//    for (int i = 0; i < ISIS_SYSTEM_ID + 2; i++)
//    {
//        lspId[i] = csnp->getEndLspID(i);
//    }
//
//    return lspId;
//}

/**
 * Check sysId from received hello packet for duplicity.
 * @return True if neighbour's sysId is the same as mine, false otherwise.
 */
bool ISISMain::checkDuplicateSysID(const ISISMessage*  msg) {

    bool test = false;

    if (msg->getType() == LAN_L1_HELLO || msg->getType() == LAN_L2_HELLO
            || msg->getType() == TRILL_HELLO) {

        // typecast for L1 hello; L1 and L2 hellos differ only in "type" field
        const ISISLANHelloPacket* hello = static_cast<const ISISLANHelloPacket* >(msg);

        //check for area address tlv and compare with my area id
        for (unsigned int j = 0; j < hello->getTLVArraySize(); j++) {
            if (hello->getTLV(j).type == AREA_ADDRESS) {
                AreaId tmpAreaId;
                tmpAreaId.fromTLV(hello->getTLV(j).value);

                test = (areaID == tmpAreaId)
                        && (hello->getSourceID() == systemId);
                if (test) {
                    break;
                }
            }
        }
    } else if (msg->getType() == PTP_HELLO) {
        test = true;
        const ISISPTPHelloPacket* hello = static_cast<const ISISPTPHelloPacket* >(msg);
        test = hello->getSourceID() == systemId;
    }

    if (test) {
        EV << this->deviceId
                  << ": IS-IS WARNING: possible duplicate system ID: "
                  << systemId;

        EV << " detected" << endl;
    }

    return test;
}

void ISISMain::schedulePeriodicSend(short circuitType) {
    ISISTimer *timer;
    if (circuitType == L1_TYPE) {
        timer = this->periodicL1Timer;
    } else {
        timer = this->periodicL2Timer;
    }

    cancelEvent(timer);
    scheduleAt(simTime() + 0.33, timer);
}

/*
 * Sends LSPs that have set SRM flag.
 * For PtP interfaces sends all messages in queue.
 * For broadcast sends only one random LSP.
 * Method is called per interface.
 * @param timer incomming timer
 * @param circuiType specify level for which send should be performed.
 */
void ISISMain::periodicSend(ISISTimer* timer, short circuitType) {
    /*
     * TODO improvement: don't scan whole database, but instead
     *  create queue "toBeSend" with lspRec* and index to SRMflags vector
     *  and when setting SRMflag (in any method) put a record in mentioned queue
     *  When doing periodicSend we don't have to check whole database (but should
     *   at least in the beginning to check that it works correctly), but just this queue
     *  We could have two queues (for PTP and broadcast).
     *  PTP queue would be simply send out.
     *  Broadcast queue would be subject of random picking.
     *
     */
    /*
     std::vector<LSPRecord *> *lspDb = this->getLSPDb(circuitType);
     std::vector<ISISadj> * adjTable = this->getAdjTab(circuitType);
     std::vector<ISISinterface>::iterator itIface = this->ISISIft.begin();
     */

    std::vector<std::vector<FlagRecord*>*> * SRMPTPQueue = getSRMPTPQueue(
            circuitType);
    std::vector<std::vector<FlagRecord*>*> * SRMBQueue = getSRMBQueue(
            circuitType);

    //std::cout << "Starting Periodic send for: ";
    //this->printSysId((unsigned char *) this->sysId);
    //std::cout << endl;

    //PTP circuits
    for (std::vector<std::vector<FlagRecord*>*>::iterator it =
            SRMPTPQueue->begin(); it != SRMPTPQueue->end(); ++it) {
        for (std::vector<FlagRecord*>::iterator itRec = (*it)->begin();
                itRec != (*it)->end(); ++itRec) {

            //std::cout<<"sendLsp to:" << (*itRec)->index <<" : ";
            //this->printLspId(this->getLspID((*itRec)->lspRec->LSP));

            this->sendLSP((*itRec)->lspRec, getIfaceById((*itRec)->interfaceId));

            //DON'T clear SRMflag for PtP

            /* when the below code is commented:
             * it might be necessary to incorporate retransmit interval and
             * when checking if the SRM flag is not already set.
             * setting the flag multiple times may cause bloating appropriate flag queue.
             */
            //delete (*itRec);
            //itRec = (*it)->erase(itRec); //instead of ++itRec in for()!!!!!!
        }

    }

    //broadcast circuits
    //for each queue(interface) pick one LSP and send it
    for (std::vector<std::vector<FlagRecord*>*>::iterator it =
            SRMBQueue->begin(); it != SRMBQueue->end(); ++it) {
        int queueSize = (*it)->size();
        if (queueSize == 0) {
            continue;
        }
        int index = floor(uniform(0, queueSize)); /*!< Index to circuit's SRMQueue */

        //send random LSP from queue
        //TODO if queue.size() > 10 pick two LSPs (or something like it)
        //TODO maybe? better version would be with this->ISISIft.at((*it)->at(index)->index)
        this->sendLSP((*it)->at(index)->lspRec,
                getIfaceById((*it)->at(index)->interfaceId));

        //clear SRMflag
        this->clearSRMflag((*it)->at(index)->lspRec, (*it)->at(index)->interfaceId,
                circuitType);
        /*        (*it)->at(index)->lspRec->srmFlags.at((*it)->at(index)->index);

         delete (*it)->at(index);
         //and remove FlagRecord from queue
         (*it)->erase((*it)->begin() + index);
         */
        queueSize = (*it)->size();

        //TODO A2 Code below has been added just to speed up lsp redistribution on broadcast links, but it needs more dynamic solution
        if (queueSize == 0) {
            continue;
        }
        index = floor(uniform(0, queueSize)); /*!< Index to circuit's SRMQueue */

        //send random LSP from queue
        //TODO if queue.size() > 10 pick two LSPs (or something like it)
        //TODO maybe? better version would be with this->ISISIft.at((*it)->at(index)->index)
        this->sendLSP((*it)->at(index)->lspRec,
                getIfaceById((*it)->at(index)->interfaceId));

        //clear SRMflag
        this->clearSRMflag((*it)->at(index)->lspRec, (*it)->at(index)->interfaceId,
                circuitType);
        /*        (*it)->at(index)->lspRec->srmFlags.at((*it)->at(index)->index);

         delete (*it)->at(index);
         //and remove FlagRecord from queue
         (*it)->erase((*it)->begin() + index);
         */
        queueSize = (*it)->size();

    }
    //reschedule PERIODIC_SEND timer
    this->schedule(timer);
}

/*
 * This method actually sends the LSP out of this module to specified interface.
 * @param lspRec specify record in LSP database that needs to be send
 * @param interfaceId specify interface to which the lsp should be send
 */
void ISISMain::sendLSP(LSPRecord *lspRec, ISISinterface* iface) {

    /* TODO C1
     * incorporate this->lspInterval ... Minimum delay in ms between sending two LSPs.
     *
     */
    /*    std::cout<<"Sending LSP from: ";
     this->printSysId((unsigned char *)this->sysId);
     std::cout<< endl;
     std::cout<<"sendLsp to:" << interfaceId <<" : ";
     this->printLspId(this->getLspID(lspRec->LSP));

     this->printLSP(lspRec->LSP, "sendLSP");*/
    //update remainingLifeTime
    //TODO check if new remLifeTime is not 0 or smaller
    if (lspRec->deadTimer->getTimerKind() == LSP_DELETE_TIMER) {

    } else {
        double remLife = lspRec->simLifetime - simTime().dbl();
        if (remLife < 0) {
            EV
                      << "ISIS: Warning: Remaining lifetime smaller than zero in sendLSP"
                      << endl;
            lspRec->LSP->setRemLifeTime(1);
        } else {
            unsigned short rem = floor(remLife);
            lspRec->LSP->setRemLifeTime(rem);
        }
    }


    Packet* packet = new Packet();
//    auto tmpLSP = lspRec->LSP->dup();
//    //TODO add proper control Info for point-to-point
//
//    // set DSAP & NSAP fields
    auto sapTag = packet->addTag<Ieee802SapReq>();
    sapTag->setDsap(SAP_CLNS);
    sapTag->setSsap(SAP_CLNS);

    packet->addTag<PacketProtocolTag>()->setProtocol(&Protocol::isis);

    //set destination multicast address
    MacAddress ma;
    if (iface->circuitType == L1_TYPE)
    {
        ma.setAddress(ISIS_ALL_L1_IS);
        packet->setName("ISIS LSP L1");
    }
    else
    {
        ma.setAddress(ISIS_ALL_L2_IS);
        packet->setName("ISIS LSP L2");
    }

    auto macTag = packet->addTag<MacAddressReq>();
    macTag->setDestAddress(ma);

    //set interfaceID
    auto interfaceTag = packet->addTag<InterfaceReq>();
    interfaceTag->setInterfaceId(iface->entry->getInterfaceId());
//    lspRec->LSP->d
    packet->insertAtFront(lspRec->LSP->dupShared());
    sendDown(packet);

    EV << "ISIS::sendLSP: Source-ID: " << systemId << endl;
}

/**
 * Create or update my own LSP.
 * @param circuiType specify level e.g. L1 or L2
 * @return vector of generated LSPs
 */
std::vector<Ptr<ISISLSPPacket>>* ISISMain::genLSP(short circuitType) {
    LspID myLSPID = this->getLSPID();
    Ptr<ISISLSPPacket> LSP; // = new ISISLSPL1Packet;

//    if ((LSP = this->getLSPFromDbByID(myLSPID, circuitType)) != NULL)
    //we have at least "System-ID.00-00" LSP

    //generate LSP and then compare it with one found in database
    //if they differ get sequence number from the one found in database, increment it by one
    //and replace the original LSP entry -> if only one LSP is present in DB then it's easy,
    //but what to do when there are multiple fragments

    //how to compare two LSP and recognize

    //generate LSP only up to ISIS_LSP_MAX_SIZE
    /* after reaching ISIS_LSP_MAX_SIZE (minus few %) (and comparing the two LSPs and such)
     * 1.if there's more -> create new LSP
     * 2.if not, check if there's another fragment in DB (it should be enough to check only next fragment)
     *
     */

    //create new LSP
    std::vector<TLV_t *>* tlvTable = new std::vector<TLV_t *>;

    //if sizeOfNeighboursUp == 0 then return NULL
    if (!this->isAdjUp(circuitType)) {
        //FIXME
        return new std::vector<Ptr<ISISLSPPacket>>;
    }

    //TLV AREA ADDRESS
    this->addTLV(tlvTable, AREA_ADDRESS, circuitType);

    //TLV IS_NEIGHBOURS
    this->addTLV(tlvTable, IS_NEIGHBOURS_LSP, circuitType, NULL);

    //add other TLVs

    //now start putting informations from tlvTable into LSP Packets

    /*
     * while(! tlvTable->empty())
     *   get first tlv from tlvTable
     *   if tlv_entry.length > max_lsp_size (minus header)
     *     we won't fit it into single lsp
     *
     *   else if tlv_entry.length > available_space in current LSP
     *     allocate new lsp
     *   else
     *     start putting it in current LSP
     *     remove it from vector
     *     DONT FORGET to PUT EVERY GENERATED LSP INTO returning vector
     *     start new iteration
     */

    TLV_t * tmpTlv;
//    unsigned int tlvSize; //tmp variable
    unsigned int availableSpace = ISIS_LSP_MAX_SIZE; //available space in LSP

    //deleted at the end of generateLSP
    std::vector<Ptr<ISISLSPPacket>>* tmpLSPDb = new std::vector<Ptr<ISISLSPPacket>>;
    for (unsigned char fragment = 0; !tlvTable->empty(); fragment++) {
        availableSpace = ISIS_LSP_MAX_SIZE;
        if (circuitType == L1_TYPE) {
            LSP = makeShared<ISISLSPPacket>();
            LSP->setType(L1_LSP);
            //set PATTLSPDBOLIS
            LSP->setPATTLSPDBOLIS(0x02 + (this->att * 0x10)); //only setting IS type = L1 (TODO A2)
        } else if (circuitType == L2_TYPE) {
            LSP = makeShared<ISISLSPPacket>();
            LSP->setType(L2_LSP);
            //set PATTLSPDBOLIS
            LSP->setPATTLSPDBOLIS(0x0A); //bits 1(2) and 3(8) => L2
        } else {
            EV << "ISIS: ERROR: Wrong circuitType in genLSP()" << endl;
        }

        //set pduLength

        //set remLifeTime
        LSP->setRemLifeTime(this->lspMaxLifetime);

        //set lspID[8];
        myLSPID.setFragmentId(fragment);
        LSP->setLspID(myLSPID);
//        myLSPID[ISIS_SYSTEM_ID + 1] = fragment;
//        this->setLspID(LSP, myLSPID);

        //set seqNum
        LSP->setSeqNumber(1);

        //set checksum

        //set TLV

        //TLV_t tmpTLV;

        for (; !tlvTable->empty();) {
            tmpTlv = tlvTable->at(0);

            //TODO incorporate header size and mostly get actual MTU something like this->ISISIft.at(0).entry->getMTU()
            if (tmpTlv->length > ISIS_LSP_MAX_SIZE) {
                //tlv won't fit into single message (this shouldn't happen)
                EV
                          << "ISIS: Warning: TLV won't fit into single message (this shouldn't happen)"
                          << endl;
                tlvTable->erase(tlvTable->begin());
            } else if (tmpTlv->length > availableSpace) {
                //tlv won't fit into this LSP, so break cycle and create new LSP
//                tmpLSPDb->push_back(LSP);
                EV << "ISIS: Info: This TLV is full." << endl;
                break; //ends inner cycle

            } else {
                this->addTLV(LSP, tmpTlv);
                //tlvSize = LSP->getTLVArraySize();
                //LSP->setTLVArraySize(tlvSize + 1);

                //update availableSpace
                availableSpace = availableSpace - (2 + tmpTlv->length); // "2" means Type and Length fields

                //clean up
                delete[] tmpTlv->value;
                delete tmpTlv;
                tlvTable->erase(tlvTable->begin());

            }
        }
        //this->printLSP(LSP, "genLSP, non-pseudo");
        tmpLSPDb->push_back(LSP);

    }


    //what about pseudonode?
    /*
     * H   H  EEEEE  RRRR   EEEEE
     * H   H  E      R   R  E
     * HHHHH  EEEEE  RRRR   EEEEE
     * H   H  E      R  R   E
     * H   H  EEEEE  R   R  EEEEE
     */

    std::vector<bool> activeIface; //interfaces with adjacency in state UP

    for (std::vector<ISISinterface>::iterator it = this->ISISIft.begin();
            it != this->ISISIft.end(); ++it) {
        activeIface.push_back(this->isUp((*it).interfaceId, circuitType));

    }

    for (std::vector<ISISinterface>::iterator it = this->ISISIft.begin();
            it != this->ISISIft.end(); ++it) {
        //if at least one adjacency is UP and network type is broadcast (there is no DIS on PTP) AND I am DIS on this interface
        if (this->isUp((*it).interfaceId, circuitType) && (*it).network
                && this->amIDIS((*it).interfaceId, circuitType)) {
            tlvTable->clear();
            //tmpLSPDb->clear(); //Why? Why? Why, you fucked-up crazy-ass weirdo beaver?!?
            availableSpace = ISIS_LSP_MAX_SIZE;
//            unsigned char nsel;

            if (circuitType == L1_TYPE) {
                myLSPID.setCircuitId((*it).L1DIS.getCircuitId());
//                myLSPID[ISIS_SYSTEM_ID] = (*it).L1DIS[ISIS_SYSTEM_ID];
//                nsel = (*it).L1DIS[ISIS_SYSTEM_ID];
            } else if (circuitType == L2_TYPE) {
                myLSPID.setCircuitId((*it).L2DIS.getCircuitId());
//                myLSPID[ISIS_SYSTEM_ID] = (*it).L2DIS[ISIS_SYSTEM_ID];
//                nsel = (*it).L2DIS[ISIS_SYSTEM_ID];
            } else {
                EV << "ISIS: ERROR: Wrong circuitType in genLSP()" << endl;
            }

            //TLV IS_NEIGHBOURS
            this->addTLV(tlvTable, IS_NEIGHBOURS_LSP, circuitType, (*it).entry);

            //TLV AREA_ADDRESS
            this->addTLV(tlvTable, AREA_ADDRESS, circuitType);

            for (unsigned char fragment = 0; !tlvTable->empty(); fragment++) {
                availableSpace = ISIS_LSP_MAX_SIZE;
                if (circuitType == L1_TYPE) {
                    LSP = makeShared<ISISLSPPacket>(); //TODO based on circuitType
                    LSP->setType(L1_LSP);
                    //set PATTLSPDBOLIS
                    LSP->setPATTLSPDBOLIS(0x02 + (this->att * 0x10)); //setting L1 + Attached flag (bit 4) if set
                } else if (circuitType == L2_TYPE) {
                    LSP = makeShared<ISISLSPPacket>(); //TODO based on circuitType
                    LSP->setType(L2_LSP);
                    //set PATTLSPDBOLIS
                    LSP->setPATTLSPDBOLIS(0x0A); //bits 1(2) and 3(8) => L2
                } else {
                    EV << "ISIS: ERROR: Wrong circuitType in genLSP()" << endl;
                }

                //set pduLength

                //set remLifeTime
                LSP->setRemLifeTime(this->lspMaxLifetime);

                //set lspID[8];
//                myLSPID[ISIS_SYSTEM_ID + 1] = fragment;
                myLSPID.setFragmentId(fragment);
                LSP->setLspID(myLSPID);
//                this->setLspID(LSP, myLSPID);

                //set seqNum
                LSP->setSeqNumber(1);

                //set checksum

                //set TLV
                //TLV_t tmpTLV;

                for (; !tlvTable->empty();) {
                    tmpTlv = tlvTable->at(0);

                    //TODO incorporate header size and mostly get actual MTU something like this->ISISIft.at(0).entry->getMTU()
                    if (tmpTlv->length > ISIS_LSP_MAX_SIZE) {
                        //tlv won't fit into single message (this shouldn't happen)
                        EV
                                  << "ISIS: Warning: TLV won't fit into single message (this shouldn't happen)"
                                  << endl;
                        tlvTable->erase(tlvTable->begin());
                    } else if (tmpTlv->length > availableSpace) {
                        //tlv won't fit into this LSP, so break cycle and create new LSP
                        //                tmpLSPDb->push_back(LSP);
                        EV << "ISIS: This TLV is full." << endl;
                        break;                        //ends inner cycle

                    } else {
                        this->addTLV(LSP, tmpTlv);
                        //tlvSize = LSP->getTLVArraySize();
                        //LSP->setTLVArraySize(tlvSize + 1);

                        //update availableSpace
                        //2Bytes are Type and Length fields
                        availableSpace = availableSpace - (2 + tmpTlv->length);

                        //clean up
                        delete[] tmpTlv->value;
                        delete tmpTlv;
                        tlvTable->erase(tlvTable->begin());

                    }
                }
                //this->printLSP(LSP, "genLSP, pseudo");
                tmpLSPDb->push_back(LSP);

            }

        }
    }

//    delete[] myLSPID;
    delete tlvTable;
    return tmpLSPDb;
}

/*
 * Calls refresh for appropriate level specified by timer.
 * @timer incomming timer
 */
void ISISMain::refreshLSP(ISISTimer *timer) {
    //this->printLSPDB();

    if (this->isType == L1_TYPE || this->isType == L1L2_TYPE) {
        this->refreshLSP(L1_TYPE);
    }
    if (this->isType == L2_TYPE || this->isType == L1L2_TYPE) {
        this->refreshLSP(L2_TYPE);
    }
    //this->printLSPDB();

    this->schedule(timer);

}

/*
 * Increment sequence number for LSPs that belongs to this IS and set new remaining lifetime.
 * But only if their remaining lifetime is not already zero, or deadTimer set to LSP_DELETE_TIMER.
 * @param circuitType is level
 */
void ISISMain::refreshLSP(short circuitType) {
    std::vector<LSPRecord*>* lspDb = this->getLSPDb(circuitType);
    std::vector<LSPRecord*>::iterator it = lspDb->begin();
    LspID lspId;
    for (; it != lspDb->end(); ++it) {
        lspId = (*it)->LSP->getLspID();
        if (lspId.getSystemId() == systemId
                && (*it)->deadTimer->getTimerKind() != LSP_DELETE_TIMER
                && (*it)->LSP->getRemLifeTime() != 0) {
            //this->printLSP((*it)->LSP, "print from refreshLSP");
            //std::cout<<"RefreshLSP: seqNum: " <<(*it)->LSP->getSeqNumber() << endl;
            (*it)->LSP->setSeqNumber((*it)->LSP->getSeqNumber() + 1);
            //std::cout<<"RefreshLSP: seqNum: " <<(*it)->LSP->getSeqNumber() << endl;
            // this->printLSP((*it)->LSP, "print from refreshLSP");
            (*it)->LSP->setRemLifeTime(this->lspMaxLifetime);
            //cancel original deadTimer
            cancelEvent((*it)->deadTimer);

            this->setSRMflags((*it), circuitType);
            //TODO what about SSNflags?
            //this->clearSSNflags((*it), circuitType);

            (*it)->simLifetime = simTime().dbl() + (*it)->LSP->getRemLifeTime();

            //re-schedule dead timer
            this->schedule((*it)->deadTimer, (*it)->LSP->getRemLifeTime());
        }
//        delete lspId;
    }
}

/*
 * Call generateLSP for appropriate level.
 * @param timer is incomming timer.
 */
void ISISMain::generateLSP(ISISTimer *timer) {
    cancelEvent(timer);
    if (this->isType == L1_TYPE || this->isType == L1L2_TYPE) {
        this->generateLSP(L1_TYPE);
    }
    if (this->isType == L2_TYPE || this->isType == L1L2_TYPE) {
        this->generateLSP(L2_TYPE);
    }

    this->schedule(timer);

}

/*
 * Generate and if necessary replace existing LSPs.
 * @param circuitType is level.
 */
void ISISMain::generateLSP(short circuitType) {
    /*
     * I am not 100% sure that this method correctly handle situation in which:
     * There are DIS's LSPs in DB, but when the DIS resign, and we don't generate any DIS's LSPs
     * we don't even know that we should purge something.
     * In worst case scenario these LSPs age out.
     */

    LspID lspId;
    std::vector<LSPRecord *> *lspDb = this->getLSPDb(circuitType);
    //don't forget to delete it
    std::vector<Ptr<ISISLSPPacket>>* tmpLSPDb;

    tmpLSPDb = this->genLSP(circuitType);

    if (lspDb->empty()) {
        //our database is empty, so put everything from tmpLSPDb into L1LSPDb
        //LSPRecord* tmpLSPRecord;
        for (std::vector<Ptr<ISISLSPPacket>>::iterator it = tmpLSPDb->begin();
                it != tmpLSPDb->end(); ++it) {
            this->installLSP((*it), circuitType);

        }
        tmpLSPDb->clear();
    }

    else if (tmpLSPDb->empty()) {
        //nothing generated so purge all existing LSPs
        this->purgeMyLSPs(circuitType);
    } else {
        //something in between
        /*
         * 1. get System-ID + NSEL from LSP-ID
         * 2. after processing, check if the next generated (if any exist) has the same LAN-ID
         *      if not -> purge all remaining LSP fragments from database with same LAN-ID
         *      if they have same LAN-ID OR there is not another LSP fragment, continue in processing
         */

        //lspId = this->getLspID(tmpLSPDb->at(0));
        //we need to compare each LSP
        //because of the way we create LSP, from the first one that doesn't match we can discard/replace all successive
        LSPRecord * tmpLSPRecord; // = this->getLSPFromDbByID(this->getLspID(tmpLSPDb->at(0)), circuitType);
        for (; !tmpLSPDb->empty();) {
            lspId = tmpLSPDb->front()->getLspID();
            if ((tmpLSPRecord = this->getLSPFromDbByID(
                    tmpLSPDb->at(0)->getLspID(), circuitType)) != NULL) {
                if (this->compareLSP(tmpLSPRecord->LSP, tmpLSPDb->at(0))) {
                    //LSP match we don't need to do anything
                    //TODO ANSAINET4.0 Uncomment deleete?
//                    delete tmpLSPDb->at(0);
                    tmpLSPDb->erase(tmpLSPDb->begin());
                    tmpLSPRecord = NULL;
                    //continue;

                } else {
                    //install tmpLSPDb->at(0) into this->L1LSPDb
                    this->replaceLSP(tmpLSPDb->at(0), tmpLSPRecord,
                            circuitType);
                    //set SRMflag on all circuits (that have at least one adjacency UP)
                    //done in replaceLSP

                    //erase
                    tmpLSPDb->erase(tmpLSPDb->begin());

                    //set tmpLSPRecord to NULL
                    tmpLSPRecord = NULL;
                    //continue;
                }
            } else {
                //install tmpLSPDb->at(0)
                this->installLSP(*tmpLSPDb->begin(), circuitType);

                //erase
                tmpLSPDb->erase(tmpLSPDb->begin());
                //break;
            }
            /* if there is any generated LSP in tmpLSPDb, and the current LSP-ID
             *  and next LSP's LSP-ID doesn't match (without fragment-ID) => we don't have any LSPs from this LAN-ID,
             *  so purge all remaining LSPs with matching LAN-ID (System-ID + NSEL)
             */
            if (!tmpLSPDb->empty()
                    && lspId != (*tmpLSPDb->begin())->getLspID()) {
                //purge all remaining fragments with lspId (without fragment-ID)
//                lspId[ISIS_SYSTEM_ID + 1] = lspId[ISIS_SYSTEM_ID + 1] + 1;
                lspId.setFragmentId(lspId.getFragmentId() + 1);
                this->purgeRemainLSP(lspId, circuitType);
            }
            //tmpLSPRecord = this->getLSPFromDbByID(this->getLspID(tmpLSPDb->at(0)), circuitType);

//            delete[] lspId;
        }

    }

    delete tmpLSPDb;
}

/*
 * Purge successive LSPs for lspId. (lspId itself is not purged)
 * @param lspId is first LSP that should be purged.
 * @param circuitType is level
 */
void ISISMain::purgeRemainLSP(LspID lspId, short circuitType) {

    //lspId[ISIS_SYSTEM_ID + 1] = lspId[ISIS_SYSTEM_ID + 1] + 1;

    while (this->getLSPFromDbByID(lspId, circuitType) != NULL) {
        this->purgeLSP(lspId, circuitType);
        //this should increment fragment-ID for the next round check
        lspId.setFragmentId(lspId.getFragmentId() + 1);
//        lspId[ISIS_SYSTEM_ID + 1] = lspId[ISIS_SYSTEM_ID + 1] + 1;

    }
}

/*
 * Initiate purge of ALL LSPs generated by this IS on specified level.
 * @param circuitType is level.
 */
void ISISMain::purgeMyLSPs(short circuitType) {

    std::vector<LSPRecord *>* lspDb;

    lspDb = this->getLSPDb(circuitType);
    LspID lspId;
    for (std::vector<LSPRecord*>::iterator it = lspDb->begin();
            it != lspDb->end(); ++it) {
        lspId = (*it)->LSP->getLspID();
        if (lspId == systemId) {
            this->purgeLSP(lspId, circuitType);
        }
//        delete lspId;
    }
}

/*
 * This method is used for purging in-memory LSP's when the remaining Lifetime becomes zero.
 */
void ISISMain::purgeLSP(LspID lspId, short circuitType) {

    LSPRecord * lspRec;
    if ((lspRec = this->getLSPFromDbByID(lspId, circuitType)) != NULL) {
        //std::cout<<"purgeLSP seqNum: " << lspRec->LSP->getSeqNumber() <<endl;
        //lspRec->LSP->setSeqNumber(0);
        //TODO do we need to increment seqNum or equal seqNum with remLife == 0 will replace that LSP?
        //lspRec->LSP->setSeqNumber(lspRec->LSP->getSeqNumber() + 1);
        //std::cout<<"purgeLSP seqNum: " << lspRec->LSP->getSeqNumber() <<endl;
        /* 7.3.16.4.  */
        lspRec->LSP->setRemLifeTime(0);
        //delete all TLV from lspRec-LSP and set TLVArraySize = 0
        //TODO check if area address TLV is necessary
        //add only Area Address
        /*        for(unsigned int i = 0; i < lspRec->LSP->getTLVArraySize(); i++){
         if(lspRec->LSP->getTLV(i).value != NULL){
         delete lspRec->LSP->getTLV(i).value;
         }
         }*/

        lspRec->LSP->setTLVArraySize(0);
        //if TLV Area Addresses should be present even in purge LSP than add it
        //this->addTLV(lspRec->LSP, AREA_ADDRESS);

        this->setSRMflags(lspRec, circuitType);
        //TODO What about SSN?? ->clearSSNflags?

        //simLifetime now express the time when the LSP has been purged so after "N" seconds it can be removed completely
        lspRec->simLifetime = simTime().dbl();

        //remove the deadTimer from future events
        cancelEvent(lspRec->deadTimer);
        //change timer type
        lspRec->deadTimer->setTimerKind(LSP_DELETE_TIMER);
        //and reschedule it again
        this->schedule(lspRec->deadTimer);
    }

}

/*
 * Purge of an received LSP with zero Remaining Lifetime according to 7.3.16.4
 * Received lsp is deleted at the end. Only exception is when replacing current LSP then
 * early return is executed to avoid deleting received lsp.
 *
 */
void ISISMain::purgeLSP(Packet *packet, short circuitType) {

    auto lsp = packet->removeAtFront<ISISLSPPacket>();
    LSPRecord *lspRec;

      int interfaceId = packet->getTag<InterfaceInd>()->getInterfaceId();// = ift->getInterfaceById(ctrl->getInterfaceId())->getNetworkLayerGateIndex();

    ISISinterface* iface = this->getIfaceById(interfaceId);

    //std::vector<LSPRecord *> * lspDb = this->getLSPDb(circuitType);
    LspID lspId = lsp->getLspID();

    //if lsp is not in memory
    if ((lspRec = this->getLSPFromDbByID(lspId, circuitType)) == NULL) {

        //send an ack of the LSP on circuit C
        if (!this->getIfaceById(interfaceId)->network) {
            /* TODO uncomment warning when sending ack for lsp NOT in db is implemented */
            EV
                      << "ISIS: Warning: Should send ACK for purged LSP over non-broadcast circuit. (Not yet implemented)."
                      << endl;
            //explicit ack are only on non-broadcast circuits
            //this->setSSNflag(lspRec, interfaceId, circuitType);
        }
    } else

    { /*7.3.16.4. b) LSP from S is in the DB */
        if (lsp->getLspID().getSystemId() != systemId) {
            //if received LSP is newer
            //7.3.16.4 b) 1)
            if (lsp->getSeqNumber() > lspRec->LSP->getSeqNumber()
                    || (lsp->getSeqNumber() == lspRec->LSP->getSeqNumber()
                            && lspRec->LSP->getRemLifeTime() != 0)) {
                //store the new LSP in DB, overwriting existing LSP
                //replaceLSP

                this->replaceLSP(lsp, lspRec, circuitType);
                //lsp is installed in DB => don't delete it, so early return
//                delete[] lspId;
                delete packet;
                return;
            }
            /* 7.3.16.4. b) 2)*/
            else if (lsp->getSeqNumber() == lspRec->LSP->getSeqNumber()
                    && lsp->getRemLifeTime() == 0
                    && lspRec->LSP->getRemLifeTime() == 0) {
                /* clear SRMflag for C and
                 * if C is non-broadcast circuit, set SSNflag for C
                 */
                /* 7.3.16.4. b) 2) i.*/
                this->clearSRMflag(lspRec, interfaceId, circuitType);
                if (!this->getIfaceById(interfaceId)->network) {
                    /* 7.3.16.4. b) 2) ii.*/
                    //explicit ack are only on non-broadcast circuits
                    this->setSSNflag(lspRec, interfaceId, circuitType);
                }

            }
            /* 7.3.16.4. b) 3).*/
            else if (lsp->getSeqNumber() < lspRec->LSP->getSeqNumber()) {
                /* 7.3.16.4. b) 3) i.*/
                this->setSRMflag(lspRec, interfaceId, circuitType);
                /* 7.3.16.4. b) 3) ii.*/
                this->clearSSNflag(lspRec, interfaceId, circuitType);
            }

        }
        /*7.3.16.4. c) it's our own LSP */
        else {
            if (lspRec->LSP->getRemLifeTime() != 0) {
                if (lsp->getSeqNumber() >= lspRec->LSP->getSeqNumber()) {
                    //std::cout<<"purgeLSP seqNum: " << lspRec->LSP->getSeqNumber() <<endl;
                    lspRec->LSP->setSeqNumber(lsp->getSeqNumber() + 1);
                    //std::cout<<"purgeLSP seqNum: " << lspRec->LSP->getSeqNumber() <<endl;
                }
                /*7.3.16.4. c) 4) set srmFlags for all circuits */
                this->setSRMflags(lspRec, circuitType);
            }
        }
    }

    delete packet;
//    delete[] lspId;
}

/*
 * This method permanently removes LSP from DB.
 * @timer is timer.
 */
void ISISMain::deleteLSP(ISISTimer *timer) {
    short circuitType = timer->getIsType();
    LspID lspId = timer->getLSPid();
    std::vector<LSPRecord *> *lspDb = this->getLSPDb(circuitType);
    LspID tmpLspId;

    for (std::vector<LSPRecord*>::iterator it = lspDb->begin();
            it != lspDb->end(); ++it) {
        tmpLspId = (*it)->LSP->getLspID();
        if (tmpLspId == lspId) {
            //clearing *flags shoudn't be necessary, but just to be sure
            this->clearSRMflags((*it), timer->getIsType());
            this->clearSSNflags((*it), timer->getIsType());
            delete (*it);
            lspDb->erase(it);
//            delete lspId;
//            delete tmpLspId;
            return;
        }
//        delete tmpLspId;
    }

//    delete lspId;
}

/*
 * Returns pointer to LSP database specified by level
 * @param circuitType is level.
 * @return pointer to LSP database.
 */
std::vector<LSPRecord *> * ISISMain::getLSPDb(short circuitType) {

    if (circuitType == L1_TYPE) {
        return this->L1LSPDb;
    } else if (circuitType == L2_TYPE) {
        return this->L2LSPDb;
    }

    return NULL;
}

/*
 * Return pointer to adjacency table specified by level.
 * @param circuitType is level
 * @return pointer to adjacency table.
 */
std::vector<ISISadj> * ISISMain::getAdjTab(short circuitType) {

    if (circuitType == L1_TYPE) {
        return &(this->adjL1Table);
    } else if (circuitType == L2_TYPE) {
        return &(this->adjL2Table);
    }

    return NULL;
}

/*
 * Return pointer to ISO paths specified by level.
 * @param circuitType is level
 * @return pointer to ISO Paths.
 */
ISISPaths_t* ISISMain::getPathsISO(short circuitType) {

    if (circuitType == L1_TYPE) {
        return this->L1ISISPathsISO;
    } else if (circuitType == L2_TYPE) {
        return this->L2ISISPathsISO;
    }

    return NULL;
}

/*
 * Returns appropriate SRM Queues for point-to-point link.
 * @param circuitType is level.
 * @return vector (of vector) of SRM flags.
 */
FlagRecQQ_t * ISISMain::getSRMPTPQueue(short circuitType) {

    if (circuitType == L1_TYPE) {
        return (this->L1SRMPTPQueue);
    } else if (circuitType == L2_TYPE) {
        return (this->L2SRMPTPQueue);
    }

    return NULL;
}

FlagRecQQ_t * ISISMain::getSRMBQueue(short circuitType) {

    if (circuitType == L1_TYPE) {
        return (this->L1SRMBQueue);
    } else if (circuitType == L2_TYPE) {
        return (this->L2SRMBQueue);
    }

    return NULL;
}

FlagRecQQ_t * ISISMain::getSSNPTPQueue(short circuitType) {

    if (circuitType == L1_TYPE) {
        return (this->L1SSNPTPQueue);
    } else if (circuitType == L2_TYPE) {
        return (this->L2SSNPTPQueue);
    }

    return NULL;
}

FlagRecQQ_t * ISISMain::getSSNBQueue(short circuitType) {

    if (circuitType == L1_TYPE) {
        return (this->L1SSNBQueue);
    } else if (circuitType == L2_TYPE) {
        return (this->L2SSNBQueue);
    }

    return NULL;
}

/*
 * @param circuitType is probably redundant and will be removed
 */

void ISISMain::setSRMflag(LSPRecord * lspRec, int interfaceId, short circuitType) {
    ISISinterface * iface = getIfaceById(interfaceId);
    std::vector<FlagRecord*> *SRMQueue = this->getSRMQ(iface->network, interfaceId, circuitType);
    if (lspRec->getFlagsSize() == 0) {
        this->addFlags(lspRec, circuitType);
    }
    /* This should prevent from adding multiple FlagRecord for the same LSP-interfaceId pair */
    if (lspRec->getSrmFlag(interfaceId) == true) {
        return;
    }

    //set SRMflag on interface only if there is  at least one adjacency UP
    if (this->isUp(iface->interfaceId, circuitType)) {
        lspRec->setSrmFlag(interfaceId, true); //setting true (Send Routing Message) on every interface
        FlagRecord *tmpSRMRec = new FlagRecord;
        tmpSRMRec->lspRec = lspRec;
        tmpSRMRec->interfaceId = interfaceId;
        SRMQueue->push_back(tmpSRMRec);
        /*
         if (this->ISISIft.at(index).network)
         {
         //TODO how do you know it's L1 and not L2
         this->L1SRMBQueue->at(index)->push_back(tmpSRMRec);
         }
         else
         {
         this->L1SRMPTPQueue->at(index)->push_back(tmpSRMRec);
         }*/
    }

}

void ISISMain::setSRMflags(LSPRecord *lspRec, short circuitType) {
    if (lspRec->getFlagsSize() == 0) {
        this->addFlags(lspRec, circuitType);
    }
    for (unsigned int i = 0; i < lspRec->getFlagsSize(); i++) {
        this->setSRMflag(lspRec, lspRec->getInterfaceId(i), circuitType);
    }

}

void ISISMain::setSRMflagsBut(LSPRecord *lspRec, unsigned int interfaceId,
        short circuitType) {
    if (lspRec->getFlagsSize() == 0) {
        this->addFlags(lspRec, circuitType);
    }
    for (unsigned int i = 0; i < lspRec->getFlagsSize(); i++) {
        if (lspRec->getInterfaceId(i) == interfaceId) {
            this->clearSRMflag(lspRec, lspRec->getInterfaceId(i), circuitType);
        } else {
            this->setSRMflag(lspRec, lspRec->getInterfaceId(i), circuitType);
        }
    }

}

void ISISMain::clearSRMflag(LSPRecord *lspRec, int interfaceId,
        short circuitType) {

    std::vector<FlagRecord*>* srmQ;
    srmQ = this->getSRMQ(getIfaceById(interfaceId)->network,
            interfaceId, circuitType);

    //clear flag
    lspRec->clearSrmFlag(interfaceId);

    //and remove FlagRecord from Queue
    std::vector<FlagRecord*>::iterator it = srmQ->begin();
    for (; it != srmQ->end();) {
        if ((*it)->interfaceId == interfaceId && (*it)->lspRec == lspRec) {
            it = srmQ->erase(it);
            //break;
        } else {
            ++it;
        }
    }
}

void ISISMain::clearSRMflags(LSPRecord *lspRec, short circuitType) {

    if (lspRec->getFlagsSize() == 0) {
        this->addFlags(lspRec, circuitType);
    }
    for (unsigned int i = 0; i < lspRec->getFlagsSize(); i++) {
        this->clearSRMflag(lspRec, lspRec->getInterfaceId(i), circuitType);
    }

}

void ISISMain::clearSRMflagsBut(LSPRecord *lspRec, unsigned int interfaceId,
        short circuitType) {
    if (lspRec->getFlagsSize() == 0) {
        this->addFlags(lspRec, circuitType);
    }
    for (unsigned int i = 0; i < lspRec->getFlagsSize(); i++) {
        if (i == interfaceId) {
            this->setSRMflag(lspRec, lspRec->getInterfaceId(i), circuitType);
        } else {
            this->clearSRMflag(lspRec, lspRec->getInterfaceId(i), circuitType);
        }
    }

}

FlagRecQ_t* ISISMain::findQueue(FlagRecQQ_t* queue, int interfaceId){

    for(int i = 0; i < queue->size(); i++){
        for(int j = 0; j < queue->at(i)->size(); j++){
        if(queue->at(i)->at(j)->interfaceId == interfaceId){
            return queue->at(i);
        }
        }
    }

    return nullptr;
}

std::vector<FlagRecord*>* ISISMain::getSRMQ(bool network, int interfaceId,
        short circuitType) {
    if (circuitType == L1_TYPE) {
        if (network) {
            return getIfaceById(interfaceId)->l1srmBQueue;
//            return findQueue(this->L1SRMBQueue, interfaceId);
        } else {
            return getIfaceById(interfaceId)->l1srmPTPQueue;
//            return findQueue(this->L1SRMPTPQueue, interfaceId);

        }

    } else if (circuitType == L2_TYPE) {
        if (network) {
            return getIfaceById(interfaceId)->l2srmBQueue;
//            return findQueue(this->L2SRMBQueue, interfaceId);
        } else {
            return getIfaceById(interfaceId)->l2srmPTPQueue;
//            return findQueue(this->L2SRMPTPQueue, interfaceId);

        }
    }
    return NULL;
}

void ISISMain::setSSNflag(LSPRecord * lspRec, int interfaceId,
        short circuitType) {

    ISISinterface * iface = getIfaceById(interfaceId);
    std::vector<FlagRecord*> *SSNQueue = this->getSSNQ(iface->network,
            interfaceId, circuitType);

    if (lspRec->getFlagsSize() == 0) {
        this->addFlags(lspRec, circuitType);
    }

    /* This should prevent from adding multiple FlagRecord for the same LSP-interfaceId pair */
    if (lspRec->getSsnFlag(interfaceId) == true) {
        return;
    }
    //set SSNflag on interface only if there is  at least one adjacency UP
    if (this->isUp(iface->interfaceId, circuitType)) {

        lspRec->setSsnFlag(interfaceId, true); //setting true (Send Routing Message) on every interface
        FlagRecord *tmpSSNRec = new FlagRecord;
        tmpSSNRec->lspRec = lspRec;
        tmpSSNRec->interfaceId = interfaceId;
        SSNQueue->push_back(tmpSSNRec);
//        if (this->ISISIft.at(index).network)
//        {
//            this->L1SSNBQueue->at(index)->push_back(tmpSSNRec);
//        }
//        else
//        {
//            this->L1SSNPTPQueue->at(index)->push_back(tmpSSNRec);
//        }
    }

}

void ISISMain::setSSNflags(LSPRecord *lspRec, short circuitType) {
    if (lspRec->getFlagsSize() == 0) {
        this->addFlags(lspRec, circuitType);
    }

    for (unsigned int i = 0; i < lspRec->getFlagsSize(); i++) {
        this->setSSNflag(lspRec, lspRec->getInterfaceId(i), circuitType);
    }

}

void ISISMain::setSSNflagsBut(LSPRecord *lspRec, unsigned int interfaceId,
        short circuitType) {
    if (lspRec->getFlagsSize() == 0) {
        this->addFlags(lspRec, circuitType);
    }
    for (unsigned int i = 0; i < lspRec->getFlagsSize(); i++) {
        if (lspRec->getInterfaceId(i) == interfaceId) {
            this->clearSSNflag(lspRec, lspRec->getInterfaceId(i), circuitType);
        } else {
            this->setSSNflag(lspRec, lspRec->getInterfaceId(i), circuitType);
        }
    }

}

void ISISMain::clearSSNflag(LSPRecord *lspRec, int interfaceId,
        short circuitType) {

    std::vector<FlagRecord*>* ssnQ;
    ssnQ = this->getSSNQ(getIfaceById(interfaceId)->network,
            interfaceId, circuitType);

    //clear flag
    lspRec->clearSsnFlag(interfaceId);

    //and remove FlagRecord from Queue
    std::vector<FlagRecord*>::iterator it = ssnQ->begin();
    for (; it != ssnQ->end();) {
        if ((*it)->interfaceId == interfaceId && (*it)->lspRec == lspRec) {
            it = ssnQ->erase(it);
            //break;
        } else {
            ++it;
        }
    }
}

void ISISMain::clearSSNflags(LSPRecord *lspRec, short circuitType) {

    if (lspRec->getFlagsSize() == 0) {
        this->addFlags(lspRec, circuitType);
    }
    for (unsigned int i = 0; i < lspRec->getFlagsSize(); i++) {
        this->clearSSNflag(lspRec, lspRec->getInterfaceId(i), circuitType);
    }

}

void ISISMain::clearSSNflagsBut(LSPRecord *lspRec, unsigned int interfaceId,
        short circuitType) {
    if (lspRec->getFlagsSize() == 0) {
        this->addFlags(lspRec, circuitType);
    }
    for (unsigned int i = 0; i < lspRec->getFlagsSize(); i++) {
        if (i == interfaceId) {
            this->setSSNflag(lspRec, lspRec->getInterfaceId(i), circuitType);
        } else {
            this->clearSSNflag(lspRec, lspRec->getInterfaceId(i), circuitType);
        }
    }

}

std::vector<FlagRecord*>* ISISMain::getSSNQ(bool network, int interfaceId,
        short circuitType) {
    if (circuitType == L1_TYPE) {
        if (network) {
            return getIfaceById(interfaceId)->l1ssnBQueue;
//            return findQueue(this->L1SSNBQueue, interfaceId);

        } else {
            return getIfaceById(interfaceId)->l1ssnPTPQueue;
//            return findQueue(this->L2SSNPTPQueue, interfaceId);
        }

    } else if (circuitType == L2_TYPE) {
        if (network) {
            return getIfaceById(interfaceId)->l2ssnBQueue;
//            return findQueue(this->L2SSNBQueue, interfaceId);
        } else {
            return getIfaceById(interfaceId)->l2ssnPTPQueue;
//            return findQueue(this->L2SSNPTPQueue, interfaceId);
        }
    }
    return NULL;
}
/**
 * Replaces existing lsp in LSP record
 * @param lsp is received LSP
 * @param is local lsp Record
 * @param circuitType is circuit type
 */
void ISISMain::replaceLSP(Ptr<ISISLSPPacket>lsp, LSPRecord *lspRecord,
        short circuitType) {

    //increase sequence number
    //std::cout<<"replaceLSP seqNum: "<< lspRecord->LSP->getSeqNumber() <<endl;
    /*if(lsp->getSeqNumber() < lspRecord->LSP->getSeqNumber()){
     lsp->setSeqNumber(lspRecord->LSP->getSeqNumber() + 1);
     }else{
     lsp->setSeqNumber(lsp->getSeqNumber() + 1);
     }*/
    lsp->setSeqNumber(lspRecord->LSP->getSeqNumber() + 1);
    //now we can delete the previous LSP

    //TODO ANSAINET4.0 Uncomment delete?
//    delete lspRecord->LSP;

    //set new lsp
    lspRecord->LSP = lsp;
    //std::cout<<"replaceLSP seqNum: "<< lspRecord->LSP->getSeqNumber() <<endl;
    //create new timer
    //lspRecord->deadTimer = new ISISTimer("LSP Dead Timer");
    //select appropriate type (this could be performed based on lsp)
    this->cancelEvent(lspRecord->deadTimer);

    lspRecord->deadTimer->setTimerKind(LSP_DEAD_TIMER);

    lspRecord->deadTimer->setIsType(circuitType);
    lspRecord->deadTimer->setLSPid(lspRecord->LSP->getLspID());


    /* need to check  this for replacing LSP from genLSP */

    if (lsp->findTag<InterfaceInd>() != nullptr) {
        //received so don't set SRM flag on that interface
//        /* 7.3.15.1 e) 1) ii. and iii. */ /* 7.3.16.4 b) 1) ii. and iii. */
//        this->setSRMflagsBut(lspRecord, interfaceId , circuitType);
//        /* 7.3.16.4 b) 1) v.  MODIFIED*/
//        this->clearSSNflags(lspRecord, circuitType);
//        //for non-broadcast /* 7.3.16.4 b) 1) iv. */
//        if(!this->ISISIft.at(interfaceId).network){
//            this->setSSNflag(lspRecord, interfaceId, circuitType);
//        }
        //received so don't set SRM flag on that interface

        int interfaceId = lsp->getTag<InterfaceInd>()->getInterfaceId();// = ift->getInterfaceById(ctrl->getInterfaceId())->getNetworkLayerGateIndex();
//        int interfaceId = lsp->getArrivalGate()->getIndex();
        ISISinterface* iface = this->getIfaceById(interfaceId);

        this->setSRMflagsBut(lspRecord, interfaceId, circuitType);
        //for non-broadcast /* 7.3.16.4 b) 1) iv. */
        if (!getIfaceById(interfaceId)->network) {
            this->setSSNflag(lspRecord, interfaceId, circuitType);
        }
        /* 7.3.16.4 b) 1) v. */
        this->clearSSNflagsBut(lspRecord, interfaceId, circuitType);

    } else {
        //generated -> set SRM on all interfaces
        this->setSRMflags(lspRecord, circuitType);
        //TODO what with SSN?
        //this->clearSSNflags(lspRecord, circuitType);
    }

    //set simulation time when the lsp will expire
    lspRecord->simLifetime = simTime().dbl() + lspRecord->LSP->getRemLifeTime();

    this->schedule(lspRecord->deadTimer, lsp->getRemLifeTime());
    //TODO B1 use fireNotification
    this->runSPF(circuitType);

}

void ISISMain::addFlags(LSPRecord *lspRec, short circuitType) {

    //add flags
    if (lspRec->getFlagsSize() != 0) {
        EV << "ISIS: Warning: Adding *flags to non-empty vectors." << endl;
        return;

    }
    for (std::vector<ISISinterface>::iterator intIt = this->ISISIft.begin();
            intIt != this->ISISIft.end(); ++intIt) {

        lspRec->addFlags(false, false, intIt->interfaceId);


    }
    //set SRM
    //this->setSRMflags(lspRec, circuitType);
}

/**
 * Installs @param lsp in local database.
 * @param lsp is received LSP
 * @param circuitType is circuit type
 */
LSPRecord * ISISMain::installLSP(Ptr<ISISLSPPacket> lsp, short circuitType) {

    LSPRecord * tmpLSPRecord = new LSPRecord;
    std::vector<LSPRecord*> * lspDb = this->getLSPDb(circuitType);

    tmpLSPRecord->LSP = lsp;
    //TODO B1
    /* if lsp is my LSP
     *   then set all flags
     * else
     *   set all BUT the received circuts
     */

    if (lsp->findTag<InterfaceInd>() != nullptr) {
//      Ieee802Ctrl* ctrl = static_cast<Ieee802Ctrl*>(lsp->getControlInfo());

        int interfaceId = lsp->getTag<InterfaceInd>()->getInterfaceId();//ift->getInterfaceById(ctrl->getInterfaceId())->getNetworkLayerGateIndex();
//        int interfaceId = lsp->getArrivalGate()->getIndex();
        ISISinterface* iface = this->getIfaceById(interfaceId);

        /* 7.3.15.1 e) 1) ii. */
        this->setSRMflagsBut(tmpLSPRecord, interfaceId, circuitType);
        /*TODO A2 7.3.15.1 e) 1) iii. -> redundant?? */
        //this->clearSRMflag(tmpLSPRecord, lsp->getArrivalGate()->getIndex(), circuitType);
        /* change from specification */
        /* iv. and v. are "switched */
        /* 7.3.15.1 e) 1) v. */
        this->clearSSNflags(tmpLSPRecord, circuitType);

        if (!getIfaceById(interfaceId)->network) {
            /* 7.3.15.1 e) 1) iv. */
            this->setSSNflag(tmpLSPRecord, interfaceId, circuitType);
        }

    } else {
        /* Never set SRM flag for lsp with seqNum == 0 */
        if (lsp->getSeqNumber() > 0) {
            /* installLSP is called from genLSP */
            this->setSRMflags(tmpLSPRecord, circuitType);
        }
        //TODO what about SSNflags?
        //this->clearSSNflags(tmpLSPRecord, circuitType);
    }

    tmpLSPRecord->deadTimer = new ISISTimer("LSP Dead Timer");
    tmpLSPRecord->deadTimer->setTimerKind(LSP_DEAD_TIMER);
    tmpLSPRecord->deadTimer->setIsType(circuitType);
    tmpLSPRecord->deadTimer->setLSPid(tmpLSPRecord->LSP->getLspID());


    tmpLSPRecord->simLifetime = simTime().dbl()
            + tmpLSPRecord->LSP->getRemLifeTime();

    this->schedule(tmpLSPRecord->deadTimer, lsp->getRemLifeTime());

    lspDb->push_back(tmpLSPRecord);

    return lspDb->back();

    //TODO B1 use fireNotification
    this->runSPF(circuitType);
}

/*
 * Returns true if packets match
 */
bool ISISMain::compareLSP(Ptr<ISISLSPPacket>lsp1, Ptr<ISISLSPPacket>lsp2) {

    //pduLength
    //so far this field is not used, or at least not properly

    //remLifeTime
    //newly generated LSP will have different remaining time

    //lspID
    //lspIDs have to match, because lsp2 is selected based on matching LSP-ID

    //seqNumber
    //newly generated LSP will have seqNumber set to 1

    //checksum
    //not used

    //PATTLSPDBOLIS
    //so far this field is set only to 0x01 = only IS Type = L1, but check anyway
    if (lsp1->getPATTLSPDBOLIS() != lsp2->getPATTLSPDBOLIS()) {
        return false;
    }

    if (lsp1->getTLVArraySize() != lsp2->getTLVArraySize()) {
        return false;
    }

    for (unsigned int i = 0; i < lsp1->getTLVArraySize(); i++) {
        if (lsp1->getTLV(i).type != lsp2->getTLV(i).type
                || lsp1->getTLV(i).length != lsp2->getTLV(i).length
                || !this->compareArrays(lsp1->getTLV(i).value,
                        lsp2->getTLV(i).value, lsp1->getTLV(i).length)) {
            return false;
        }
    }

    //packets match
    return true;
}

/*
 * Returns LSPRecord from LSP database specified by LSP-ID.
 * @param LSPID is ID of LSP to be returned.
 * @param circuitType is level.
 */
LSPRecord* ISISMain::getLSPFromDbByID(LspID LSPID, short circuitType) {

    std::vector<LSPRecord *> *lspDb;

    if (LSPID.toInt() == 0) {
        return NULL;
    }

    lspDb = this->getLSPDb(circuitType);
//    LspID lspId;
    for (std::vector<LSPRecord*>::iterator it = lspDb->begin();
            it != lspDb->end(); ++it) {
        if ((*it)->LSP->getLspID() == LSPID) {
            return (*it);
        }
//        lspId = this->getLspID((*it)->LSP);
//        if (this->compareArrays(lspId, LSPID, ISIS_SYSTEM_ID + 2))
//        {
//            delete[] lspId;
//            return (*it);
//        }
//        delete[] lspId;
    }

    return NULL;
}

/*
 * Returns "first" LSP-ID of this system eg. SYSTEM-ID.00-00, because this LSP-I
 */
LspID ISISMain::getLSPID() {

    return LspID(systemId);
//    unsigned char *myLSPID = new unsigned char[8];
//
//    this->copyArrayContent((unsigned char*) this->sysId, myLSPID, ISIS_SYSTEM_ID, 0, 0);
//    myLSPID[6] = 0;
//    myLSPID[7] = 0;
//    return myLSPID;
}

/**
 * Since this method is used for point-to-point links only one adjacency is expected
 * @param interfaceId index to global interface table
 */

ISISadj* ISISMain::getAdjByInterfaceId(int interfaceId, short circuitType, int offset) {

    if (circuitType == L1_TYPE || circuitType == L1L2_TYPE) //L1L2_TYPE option is there for L1L2 PTP links, because for such links there should be adjacency in both tables
            {
        for (std::vector<ISISadj>::iterator it = this->adjL1Table.begin();
                it != this->adjL1Table.end(); ++it) {
            if ((*it).interfaceId == interfaceId) {
                if (offset == 0) {
                    return &(*it);
                } else {
                    offset--;
                }

            }
        }
    } else if (circuitType == L2_TYPE) {
        for (std::vector<ISISadj>::iterator it = this->adjL2Table.begin();
                it != this->adjL2Table.end(); ++it) {
            if ((*it).interfaceId == interfaceId) {
                if (offset == 0) {
                    return &(*it);
                } else {
                    offset--;
                }
            }
        }
    }
    return NULL;

}

/**
 * Returns interface for provided gate index.
 * @param interfaceId index to global interface table
 */

ISISinterface* ISISMain::getIfaceById(int interfaceId) {
    /*
     * interfaceId != interfaceIndex therefore we cannot use just return &(this->ISISIft.at(interfaceId));
     */

    for (std::vector<ISISinterface>::iterator it = this->ISISIft.begin();
            it != this->ISISIft.end(); ++it) {

        if ((*it).interfaceId == interfaceId) {
            return &(*it);
        }
    }
    return NULL;
}

/**
 * @param interfaceIndex is index to ISISIft vector
 */
unsigned short ISISMain::getHoldTime(int interfaceId, short circuitType) {

    int dis1, dis2;
    dis1 = dis2 = 1;
    ISISinterface *tmpIntf = getIfaceById(interfaceId);
    //TODO check whether this interface is DIS and if so, return only 1/3
    if (this->amIL1DIS(interfaceId)) {
        dis1 = 3;
    }

    if (this->amIL2DIS(interfaceId)) {
        dis2 = 3;
    }

    if (circuitType == L1_TYPE) {
        if (tmpIntf->L1HelloInterval == 0) {
            return 1;
        } else {
            return (tmpIntf->L1HelloMultiplier * tmpIntf->L1HelloInterval) / dis1;
        }
    } else if (circuitType == L2_TYPE) {
        if (tmpIntf->L2HelloInterval == 0) {
            return 1;
        } else {
            return (tmpIntf->L2HelloMultiplier * tmpIntf->L2HelloInterval) / dis2;
        }

    } else if (circuitType == L1L2_TYPE) {
        //return smaller of L1 and L2 hold timers
        return (this->getHoldTime(interfaceId, L1_TYPE)
                < this->getHoldTime(interfaceId, L2_TYPE) ?
                this->getHoldTime(interfaceId, L1_TYPE) :
                this->getHoldTime(interfaceId, L2_TYPE));

    } else {
        return 1;
        EV << "deviceId " << deviceId
                  << ": Warning: Unrecognized circuitType used in ISIS::getHoldTime()\n";
    }
    return 1; //this is here just so OMNeT wouldn't bother me with "no return in non-void method" warning
}

/*
 * Returns Hello interval for interface and level.
 * If this IS is DIS on specified interface, returns only one third of the value.
 * @param interfaceIndex is index to ISISIft
 * @param circuitType is level
 * @return number of seconds.
 */
double ISISMain::getHelloInterval(int interfaceId, short circuitType) {

    //check if interface is DIS for specified circuitType (only on broadcast interfaces)
    //for DIS interface return one third of actual value
    int dis1, dis2;
    dis1 = dis2 = 1;
    ISISinterface* tmpIntf = getIfaceById(interfaceId);
    //TODO check whether this interface is DIS and if so, return only 1/3
    if (this->amIL1DIS(interfaceId)) {
        dis1 = 3;
    }

    if (this->amIL2DIS(interfaceId)) {
        dis2 = 3;
    }

    if (circuitType == L1_TYPE) {
        if (tmpIntf->L1HelloInterval == 0) {
            return 1 / tmpIntf->L1HelloMultiplier;
        } else {
            return (tmpIntf->L1HelloInterval) / dis1;
        }
    } else if (circuitType == L2_TYPE) {
        if (tmpIntf->L2HelloInterval == 0) {
            return 1 / tmpIntf->L2HelloMultiplier;
        } else {
            return (tmpIntf->L2HelloInterval) / dis2;
        }

    } else if (circuitType == L1L2_TYPE) {
        EV << "deviceId " << deviceId
                  << ": Warning: Are you sure you want to know Hello interval for L1L2_TYPE ?!? in ISIS::getHelloInterval()\n";
        //return smaller of L1 and L2 hold timers
        return (this->getHelloInterval(interfaceId, L1_TYPE)
                < this->getHelloInterval(interfaceId, L2_TYPE) ?
                this->getHelloInterval(interfaceId, L1_TYPE) :
                this->getHelloInterval(interfaceId, L2_TYPE));

    } else {
        return 1;
        EV << "deviceId " << deviceId
                  << ": Warning: Unrecognized circuitType used in ISIS::getHelloInterval()\n";
    }

    return 0.0;

}
/**
 * Return true if this system is DIS for interface specified by @param interfaceIndex for specified level.
 * @param entry Pointer to interface record in interfaceTable
 * @param intElement XML element of current interface in XML config file
 */
bool ISISMain::amIDIS(int interfaceId, short circuitType) {

    if (circuitType == L1_TYPE) {
        return this->amIL1DIS(interfaceId);
    } else if (circuitType == L2_TYPE) {
        return this->amIL2DIS(interfaceId);
    } else {
        return this->amIL1DIS(interfaceId) || this->amIL2DIS(interfaceId);;
        EV << "deviceId " << deviceId
                  << ": ISIS: WARNING: amIDIS for unknown circuitType " << endl;
    }
    return false;
}
/*
 * Determines if this IS is DIS on specified interface for L1
 * @param interfaceIndex is index to ISISIft
 * return true if this IS is DIS on specified interface for L1.
 */
bool ISISMain::amIL1DIS(int interfaceId) {
    //if this is not broadcast interface then no DIS is elected
    if (!getIfaceById(interfaceId)->network) {
        return false;
    }

    return (systemId == getIfaceById(interfaceId)->L1DIS.getSystemId());

}

/*
 * Determines if this IS is DIS on specified interface for L2
 * @param interfaceIndex is index to ISISIft
 * return true if this IS is DIS on specified interface for L2.
 */
bool ISISMain::amIL2DIS(int interfaceId) {
    //if this is not broadcast interface then no DIS is elected
    if (!getIfaceById(interfaceId)->network) {
        return false;
    }

    return systemId == getIfaceById(interfaceId)->L2DIS;
//    return (compareArrays((unsigned char *) this->sysId, getIfaceById(interfaceId)->L2DIS, ISIS_SYSTEM_ID));

}

/*
 * Returns pointer to TLV array in ISISMessage specified by tlvType.
 * @param inMsg message to be parsed
 * @param tlvType is type of TLV
 * @param offset is n-th found TLV
 * @return pointer to TLV array.
 */
TLV_t* ISISMain::getTLVByType(const ISISMessage* inMsg, enum TLVtypes tlvType,
        int offset) {

    for (unsigned long int i = 0; i < inMsg->getTLVArraySize(); i++) {
        if (inMsg->getTLV(i).type == tlvType) {
            if (offset == 0) {
                //TODO ANSAINET4.0 changed in the process; does it work?
//                return &(inMsg->getTLVForUpdate(i));
                return const_cast<TLV_t*>(&inMsg->getTLV(i));
            } else {
                offset--;
            }
        }
    }

    return NULL;
}

/*
 * Returns pointer to TLV_t::value within TLV where starts specified subTLV
 * @param TLV_t* TLV to be parsed
 * @param subTLVType is type of subTLV to find
 * @param offset is n-th found sub-TLV
 * @return pointer to TLV::value.
 */
unsigned char* ISISMain::getSubTLVByType(TLV_t *tlv, enum TLVtypes subTLVType,
        int offset) {
    /* Implementation assumes only correctly formated TLVs eg
     * if minimal length of such TLV is 2B (for example TLV_MT_PORT_CAP)
     * and specified lenght is more than 2B then there has to be sub-TLV that is at least
     * 2B long (type + length, value can be zero). If such sub-TLV would have only 1B eg
     * malformed, memory corruption could occur.
     */

    int skip = 0;
    if (tlv->type == TLV_MT_PORT_CAP) {
        skip = 2; //TLV_MT_PORT_CAP is itself 2B long

        for (skip = 2; tlv->length > skip;) {
            if (tlv->value[skip] != subTLVType) {
                skip += 2 + tlv->value[skip + 1];

            } else {
                return &(tlv->value[skip]);
            }
        }
    }

//    for (unsigned int i = 0; i < inMsg->getTLVArraySize(); i++)
//    {
//        if (inMsg->getTLV(i).type == subTLVType)
//        {
//            if (offset == 0)
//            {
//                return &(inMsg->getTLV(i));
//            }
//            else
//            {
//                offset--;
//            }
//        }
//    }

    return NULL;
}

/*


 TLV_t* ISIS::getTLVByType(ISISL1HelloPacket *msg, enum TLVtypes tlvType, int offset)
 {

 for(unsigned int i = 0; i < msg->getTLVArraySize(); i++){
 if(msg->getTLV(i).type == tlvType){
 if(offset == 0){
 return &(msg->getTLV(i));
 }else{
 offset--;
 }
 }
 }

 return NULL;
 }

 TLV_t* ISIS::getTLVByType(ISISL2HelloPacket *msg, enum TLVtypes tlvType, int offset)
 {

 for(unsigned int i = 0; i < msg->getTLVArraySize(); i++){
 if(msg->getTLV(i).type == tlvType){
 if(offset == 0){
 return &(msg->getTLV(i));
 }else{
 offset--;
 }
 }
 }

 return NULL;
 }

 TLV_t* ISIS::getTLVByType(Ptr<ISISPTPHelloPacket> msg, enum TLVtypes tlvType, int offset)
 {

 for (unsigned int i = 0; i < msg->getTLVArraySize(); i++)
 {
 if (msg->getTLV(i).type == tlvType)
 {
 if (offset == 0)
 {
 return &(msg->getTLV(i));
 }
 else
 {
 offset--;
 }
 }
 }

 return NULL;
 }
 */
/*
 * Checks if the System-ID length and Maximum Areas fields in incomming message are supported.
 * @pram inMsg is incomming message
 * @return true if message is OK.
 */
bool ISISMain::isMessageOK(const Ptr<const ISISMessage> inMsg) {

    if (inMsg->getIdLength() != ISIS_SYSTEM_ID && inMsg->getIdLength() != 0) {
        return false;
    }

    if (this->mode == L2_ISIS_MODE) {
        if (inMsg->getMaxAreas() != 1) {
            return false;
        }
    } else {
        if (inMsg->getMaxAreas() != ISIS_MAX_AREAS
                && inMsg->getMaxAreas() != 0) {
            return false;
        }
    }

    return true;
}

/*
 * Compares Area-ID of this IS with Area-ID TLV.
 * @param areaAddressTLV is TLV with area address TLV.
 * @return true if at least one area-id match.
 */
bool ISISMain::isAreaIDOK(TLV_t *areaAddressTLV, AreaId compare) {
    if (compare.toInt() == 0) {
        compare = this->areaID;
    }

    if (areaAddressTLV->length == 0) {
        return false;
    }

    AreaId tmpAreaID;
    tmpAreaID.fromTLV(areaAddressTLV->value);

    return tmpAreaID == compare;
//    for (unsigned int i = 0; i < areaAddressTLV->length;)
//    {
//        if (this->compareArrays(compare, &areaAddressTLV->value[i + 1], areaAddressTLV->value[i]))
//        {
//            //if one address match return false
//            return true;
//        }
//        i += areaAddressTLV->value[i] + 1;
//
//    }
//    //not even single areaID match, so return false
//    return false;
}

///*
// * Returns index to ISISIft for specified interface.
// * @param interface is pointer to interface
// * @return index to ISISIft vector
// */
//int ISISMain::getIfaceIndex(ISISinterface *interface) {
//
//    for (unsigned int i = 0; i < this->ISISIft.size(); i++) {
//
//        if (interface == &(ISISIft.at(i))) {
//            return i;
//        }
//    }
//    return 0;
//}

/*
 * Generates and adds specified TLV to the message.
 * @param inMsg message
 * @param tlvType is TLV type to be generated.
 */
void ISISMain::addTLV(Ptr<ISISMessage> inMsg, enum TLVtypes tlvType, short circuitType,
        int interfaceId) {
    std::vector<TLV_t*> tmpTLV;
    switch (tlvType) {
    case (AREA_ADDRESS):

        tmpTLV = this->genTLV(AREA_ADDRESS, circuitType); //second parameter doesn't matter for Area Address
//            this->addTLV(inMsg, tmpTLV);

        break;
    case (IS_NEIGHBOURS_HELLO):
        tmpTLV = this->genTLV(IS_NEIGHBOURS_HELLO, circuitType, interfaceId); //second parameter doesn't matter for Area Address
//            this->addTLV(inMsg, tmpTLV);

        break;

    case (TLV_TRILL_NEIGHBOR):
        tmpTLV = this->genTLV(TLV_TRILL_NEIGHBOR, circuitType, interfaceId);
//            this->addTLV(inMsg, tmpTLV);

        break;

    case (PTP_HELLO_STATE):
        tmpTLV = this->genTLV(PTP_HELLO_STATE, circuitType, interfaceId); //second parameter doesn't matter for Area Address
//            this->addTLV(inMsg, tmpTLV);

        break;

    default:
        EV
                  << "ISIS: ERROR: This TLV type is not (yet) implemented in addTLV(ISISMessage*, enum TLVtypes)"
                  << endl;
        break;

    }

    for (std::vector<TLV_t *>::iterator it = tmpTLV.begin(); it != tmpTLV.end();
            ++it) {
        this->addTLV(inMsg, (*it));
    }

}

/*
 * Copy already generated TLV to message and sets correct TLV array size.
 * @param inMsg message
 * @param tlv pointer to TLV to be added.
 */
void ISISMain::addTLV(Ptr<ISISMessage> inMsg, TLV_t *tlv) {
    TLV_t tmpTlv;
    unsigned int tlvSize;

    tmpTlv.type = tlv->type; // set type
    tmpTlv.length = tlv->length; //set length
    tmpTlv.value = new unsigned char[tlv->length]; //allocate appropriate space
    this->copyArrayContent(tlv->value, tmpTlv.value, tlv->length, 0, 0); //copy it

    tlvSize = inMsg->getTLVArraySize(); //get array size
    inMsg->setTLVArraySize(tlvSize + 1); // increase it by one
    inMsg->setTLV(tlvSize, tmpTlv); //finally add TLV at the end
}

std::vector<TLV_t *> ISISMain::genTLV(enum TLVtypes tlvType, short circuitType,
        int interfaceId, enum TLVtypes subTLVType, InterfaceEntry *ie) {
    TLV_t * myTLV;
    std::vector<TLV_t *> myTLVVector;
    AdjTab_t * adjTab;
    TrillInterfaceData *d;
    int count = 0;
    if (tlvType == AREA_ADDRESS) {
        /*************************************
         * TLV #1 Area Addreses              *
         * Code 1                            *
         * Length: length of the value field *
         * Value:                            *
         * ***********************************
         * Address length       *     1B     *
         * Area Address   * Address Length   *
         *************************************
         * Address length       *     1B     *
         * Area Address   * Address Length   *
         *************************************
         */
        //TODO B1 Area-ID comparison is done for fixed (ISIS-L3-MODE style) Area address length. To fully support RFC 6326 Area-ID verification check would need to incorporate that.
        /* RFC 6326 4.2. */
//        if (this->mode == ISIS::L2_ISIS_MODE) {
//            myTLV = new TLV_t;
//            myTLV->type = AREA_ADDRESS;
//            myTLV->length = 2;
//            myTLV->value = new unsigned char[myTLV->length];
//            myTLV->value[0] = 1;
//            myTLV->value[1] = 0;
//            myTLVVector.push_back(myTLV);

//        } else {
            //TODO add support for multiple Area Addresses
            myTLV = new TLV_t;
            myTLV->type = AREA_ADDRESS;
            myTLV->length = ISIS_AREA_ID + 1;
            myTLV->value = new unsigned char[ISIS_AREA_ADDRESS_TLV_LEN];
//            myTLV->value[0] = ISIS_AREA_ID; //first byte is length of area address
            this->copyArrayContent(areaID.toTLV(), myTLV->value,
                    ISIS_AREA_ADDRESS_TLV_LEN, 0, 0);
            myTLVVector.push_back(myTLV);
//        }
    } else if (tlvType == IS_NEIGHBOURS_HELLO) {
        /* IS Neighbours in state "UP" or "Initializing" eg. ALL in adjTab. */
        /*************************************
         * TLV #6 IS Neighbours              *
         * Code 6                            *
         * Length: length of the value field *
         * Value:                            *
         * ***********************************
         * LAN Address (MAC)    *     6B     *
         * LAN Address (MAC)    *     6B     *
         * LAN Address (MAC)    *     6B     *
         *************************************
         */
        //TODO there could more neighbours than could fit in one TLV
        adjTab = this->getAdjTab(circuitType);

        /* stupid solution */
        for (unsigned int h = 0; h < adjTab->size(); h++) {
            if (adjTab->at(h).interfaceId == interfaceId) {
                count++;
            }
        }

        myTLV = new TLV_t;
        myTLV->type = IS_NEIGHBOURS_HELLO;
        myTLV->length = count * 6; //number of records * 6 (6 is size of system ID/MAC address)
        myTLV->value = new unsigned char[myTLV->length];
        count = 0;
        for (unsigned int h = 0; h < adjTab->size(); h++) {
            if (adjTab->at(h).interfaceId == interfaceId) {
//                    this->copyArrayContent(adjTab->at(h).mac.getAddressBytes(), myTLV->value, 6, 0, h * 6);
                adjTab->at(h).mac.getAddressBytes(&(myTLV->value[count * 6]));
                count++;
            }
        }
        myTLVVector.push_back(myTLV);

    } else if (tlvType == TLV_TRILL_NEIGHBOR) {
        /* TRILL Neighbor TLV to be used in TRILL IIH PDUs */
        /*************************************
         * TLV #145 TRILL Neighbor           *
         * Code 145                          *
         * Length: 1 + 9 * n                 *
         * Value:                            *
         * ***********************************
         * S|L| Reserved        *     1B     *
         *************************************
         * F| Reserved          *     1B     *
         * MTU                  *     2B     *
         * MAC Address          *     6B     *
         *************************************
         */

        //TODO A2 check length and if necessary, create new TLV_t
        adjTab = this->getAdjTab(circuitType);

        /* stupid solution */
        for (unsigned int h = 0; h < adjTab->size(); h++) {
            if (adjTab->at(h).interfaceId == interfaceId) {
                count++;
            }
        }
        myTLV = new TLV_t;
        myTLV->type = TLV_TRILL_NEIGHBOR;
        myTLV->length = 1 + count * 9;
        myTLV->value = new unsigned char[myTLV->length];
        //TODO A2 S and L bit should be dynamic
        myTLV->value[0] = 192; //both bits set
        count = 0;
        for (unsigned int h = 0; h < adjTab->size(); h++) {
            if (adjTab->at(h).interfaceId == interfaceId) {
                //TODO MTU test
                myTLV->value[1 + count * 9] = 0; // F | Reserved
                myTLV->value[1 + count * 9 + 1] = 0; //MTU
                myTLV->value[1 + count * 9 + 2] = 0; //MTU

                adjTab->at(h).mac.getAddressBytes(
                        &(myTLV->value[1 + (count * 9) + 3]));
                count++;
            }
        }

        myTLVVector.push_back(myTLV);
    } else if (tlvType == PTP_HELLO_STATE) {
        //ptp adjacency state TLV #240
        myTLV = new TLV_t;
        myTLV->type = PTP_HELLO_STATE;
        myTLV->length = 1;
        myTLV->value = new unsigned char[myTLV->length];
        ISISadj* tempAdj;
        //if adjacency for this interface exists, then its state is either UP or INIT
        //we also assumes that on point-to-point link only one adjacency can exist
        //TODO we check appropriate level adjacency table, but what to do for L1L2? In such case there's should be adjacency in both tables so we check just L1
        if ((tempAdj = this->getAdjByInterfaceId(interfaceId, circuitType)) != NULL) {
            if (tempAdj->state == ISIS_ADJ_DETECT) {
                myTLV->value[0] = PTP_INIT;
                EV << "ISIS::sendPTPHello: Source-ID: " << systemId;
//                for (unsigned int i = 0; i < 6; i++)
//                {
//                    EV<< std::setfill('0') <<  std::setw(2) << std::dec << (unsigned int) this->sysId[i];
//                    if (i % 2 == 1)
//                    EV << ".";
//                }
                EV << "sending state PTP_INIT " << endl;
            } else {
                myTLV->value[0] = PTP_UP;
                EV << "ISIS::sendPTPHello: Source-ID: " << systemId;

//                for (unsigned int i = 0; i < 6; i++)
//                {
//                    EV << std::setfill('0') <<  std::setw(2) << std::dec << (unsigned int) this->sysId[i];
//                    if (i % 2 == 1)
//                    EV << ".";
//                }
                EV << "sending state PTP_UP " << endl;
                EV << "ISIS::sendPTPHello: sending state PTP_UP " << endl;
            }

        } else {
            //if adjacency doesn't exist yet, then it's for sure down
            myTLV->value[0] = PTP_DOWN;
            EV << "ISIS::sendPTPHello: Source-ID: " << systemId;
//            for (unsigned int i = 0; i < 6; i++)
//            {
//                EV << std::setfill('0') <<  std::setw(2) << std::dec << (unsigned int) this->sysId[i];
//                if (i % 2 == 1)
//                EV << ".";
//            }
            EV << "sending state PTP_DOWN " << endl;

        }

        myTLVVector.push_back(myTLV);

    } else if (tlvType == TLV_MT_PORT_CAP) {

        /* Multi-Topology-aware Port Capability */
        /*************************************
         * TLV #143 MTPORTCAP                *
         * Code 143                          *
         * Length: 2 + sub-TLVs length       *
         * Value:                            *
         * ***********************************
         * R|R|R|R| Topology Id *     2B     *
         *************************************
         *   Sub-TLVs           * variable B *
         *************************************
         */


        if (ie != NULL) {
            d = ie->getProtocolData<TrillInterfaceData>();
        }

        myTLV = new TLV_t;
        myTLV->type = TLV_MT_PORT_CAP;
        myTLV->length = 0;
        myTLV->value = new unsigned char[255];
        myTLV->value[0] = 0; //reserved + zero topology Id
        myTLV->value[1] = 0;
        myTLV->length = 2;

        /* Special VLANS and Flags Sub-TLV  */
        /*************************************
         * TLV #143 MTPORTCAP                *
         * subTLV Code 1                     *
         * Length: 8                         *
         * Value:                            *
         * ***********************************
         * Port ID              *     2B     *
         * Sender Nickname      *     2B     *
         * AF|AC|VM|BY| Outer.VLAN *  2B     *
         * TR| R| R| R| Desig.VLAN *  2B     *
         *************************************
         */

        myTLV->value[myTLV->length] = TLV_MT_PORT_CAP_VLAN_FLAG;
        myTLV->value[myTLV->length + 1] = 8;
        myTLV->length += 2; //size of "head" of subTLV
        //TODO change it interfaceId
        //Port ID
        myTLV->value[myTLV->length] = interfaceId & 0xFF;
        myTLV->value[myTLV->length + 1] = interfaceId >> 8;

        //TODO B1 Nickname
//        memcpy(&(myTLV->value[myTLV->length + 2]), &(this->sysId[ISIS_SYSTEM_ID - 3]), 2);
        myTLV->value[myTLV->length + 2] = this->nickname.getNickname() & 0xFF;
        myTLV->value[myTLV->length + 3] = (this->nickname.getNickname() >> 8) & 0xFF;
        //Outer.VLAN
        //TODO B1 what to do when the port is configured to strip VLAN tag
        //TODO B1 in some cases one hello is sent on EVERY VLAN (0 - 4096)

        myTLV->value[myTLV->length + 4] = d->getVlanId() & 0xFF;
        myTLV->value[myTLV->length + 5] = (d->getVlanId() >> 8) & 0x0F;
        //AF
        myTLV->value[myTLV->length + 5] |= (d->isAppointedForwarder(d->getVlanId(), this->nickname) << 7);
        //AC
        myTLV->value[myTLV->length + 5] |= (d->isAccess() << 6);
        //VM VLAN Mapping TODO unable to detect so setting false
        myTLV->value[myTLV->length + 5] |= (d->isVlanMapping() << 5);
        //BY Bypass pseudonode
        myTLV->value[myTLV->length + 5] |= (d->isVlanMapping() << 4);

        //Design.VLAN
        myTLV->value[myTLV->length + 6] = d->getDesigVlan() && 0xFF;
        myTLV->value[myTLV->length + 7] = (d->getDesigVlan() >> 8) && 0x0F;
        //TR flag
        myTLV->value[myTLV->length + 7] |= (d->isTrunk() << 7);

        myTLV->length += 8;

        /* Appointed Forwarders Sub-TLV     */
        /*************************************
         * TLV #143 MTPORTCAP                *
         * subTLV Code 3                     *
         * Length: n * 6B                    *
         * Value:                            *
         * ***********************************
         * Appointee Nickname   *     2B     *
         * R|R|R|R| Start. VLAN *     2B     *
         * R|R|R|R| End. VLAN   *     2B     *
         *************************************
         * ...                               *
         */
        //TODO A2
        if (amIL1DIS(interfaceId)) {
            //TODO A2 before appointing someone, DRB should wait at least Holding timer interval (to ensure it is the DRB).
            myTLV->value[myTLV->length] = TLV_MT_PORT_CAP_APP_FWD;
            myTLV->value[myTLV->length + 1] = 1 * 6;
            myTLV->length += 2; //size of subTLV header (type, length)
            //Appointee Nickname


            myTLV->value[myTLV->length + 0] = this->nickname.getNickname() & 0xFF; //Appointee Nickname
            myTLV->value[myTLV->length + 1] = (this->nickname.getNickname() >> 8) & 0xFF; //Appointee Nickname
            myTLV->value[myTLV->length + 2] = 1; //Start VLAN
            myTLV->value[myTLV->length + 3] = 0; //Start VLAN
            myTLV->value[myTLV->length + 4] = 1; //End VLAN
            myTLV->value[myTLV->length + 5] = 0; //End VLAN

            myTLV->length += 6; //size of one record

        }

        //end of subTLV

        myTLVVector.push_back(myTLV);
    } else {
        EV
                  << "ISIS: ERROR: This TLV type is not (yet) implemented in genTLV(enum TLVtypes tlvType, short circuitType, int interfaceId, enum TLVtypes subTLVType, InterfaceEntry *ie)"
                  << endl;

    }
    return myTLVVector;
}

/*
 * Generates and adds specified TLV type to the set of TLVs
 * @param tlvTable set of TLVs
 * @param tlvType TLV type to be generated
 * @param circuitType is level.
 */
void ISISMain::addTLV(std::vector<TLV_t *> *tlvTable, enum TLVtypes tlvType, short circuitType, InterfaceEntry *ie)
{
  std::vector<TLV_t *> myTLVVector;
  TLV_t * myTLV;

  if (tlvType == AREA_ADDRESS)
  {
    myTLVVector = this->genTLV(AREA_ADDRESS, circuitType);
    for (std::vector<TLV_t *>::iterator it = myTLVVector.begin(); it != myTLVVector.end(); ++it)
    {
      tlvTable->push_back((*it));
    }

  }
  else if (tlvType == IS_NEIGHBOURS_LSP)
  {

    if (ie == NULL)
    {
      std::vector<LSPneighbour> neighbours;
      ISISadj *tmpAdj;
      for (std::vector<ISISinterface>::iterator it = this->ISISIft.begin(); it != this->ISISIft.end(); ++it)
      {
        LSPneighbour neighbour;
        tmpAdj = this->getAdjByInterfaceId((*it).interfaceId, circuitType);
        //if there's not adjacency in state "UP" for specified interface, then skip this interface
        if (tmpAdj == NULL || tmpAdj->state != ISIS_ADJ_REPORT)
        {
          continue;
        }

        if ((*it).network)
        {
          PseudonodeID DIS;
          if (circuitType == L1_TYPE)
          {
            DIS = (*it).L1DIS;
          }
          else
          {
            DIS = (*it).L2DIS;
          }

          neighbour.LANid = DIS;
//                    this->copyArrayContent(DIS, neighbour.LANid, ISIS_SYSTEM_ID + 1, 0, 0);
        }
        else
        {
          neighbour.LANid.set(tmpAdj->sysID, 0);

//                    this->copyArrayContent(tmpAdj->sysID, neighbour.LANid, ISIS_SYSTEM_ID, 0, 0);
//                    neighbour.LANid[ISIS_SYSTEM_ID] = 0;
        }

        //find interface which is DIS connected to and set metrics
        neighbour.metrics.defaultMetric = (*it).metric; //default = 10
        neighbour.metrics.delayMetric = 128; //disabled;
        neighbour.metrics.expenseMetric = 128; //disabled
        neighbour.metrics.errortMetric = 128; //disabled

        neighbours.push_back(neighbour);
        neighbour.metrics.errortMetric = 128; //disableds

      }
      //we have vector of neighbours

      //now compute size needed in tlv
      // virtual link + ((metrics + System-ID + 1) * number_of_neighbours

      int entrySize = (4 + ISIS_SYSTEM_ID + 1);
      //run until there's any neighbour
      for (; !neighbours.empty();)
      {
        myTLV = new TLV_t;
        myTLV->type = IS_NEIGHBOURS_LSP;
        myTLV->length = 1 + ((4 + ISIS_SYSTEM_ID + 1) * neighbours.size());
        myTLV->value = new unsigned char[ISIS_LSP_MAX_SIZE];

        //virtualFlag
        myTLV->value[0] = 40; //TODO should be 0
        //inner loop for separate TLV; after reaching ISIS_LSP_MAX_SIZE or empty neighbours stop filling this tlv
        // 2 bytes for type and length fields and 1 byte for virtual circuit
        for (unsigned int i = 0; ((i + 1) * entrySize) + 1 < 255 && 2 + 1 + (entrySize * i) + entrySize < ISIS_LSP_MAX_SIZE && !neighbours.empty(); i++)
        {
          myTLV->value[(i * 11) + 1] = neighbours.at(0).metrics.defaultMetric;
          myTLV->value[(i * 11) + 2] = neighbours.at(0).metrics.delayMetric;
          myTLV->value[(i * 11) + 3] = neighbours.at(0).metrics.expenseMetric;
          myTLV->value[(i * 11) + 4] = neighbours.at(0).metrics.errortMetric;
          this->copyArrayContent(neighbours.at(0).LANid.toTLV(), myTLV->value, ISIS_SYSTEM_ID + 1, 0, (i * 11) + 5); //set system ID
          myTLV->length = 1 + ((4 + ISIS_SYSTEM_ID + 1) * (i + 1));
          //delete first entry
          neighbours.erase(neighbours.begin());
        }
        //this tlv is full or no other neighbour entry is present
        tlvTable->push_back(myTLV);
        //TODO do i need to create new myTLV and allocate value?
        //myTLV = new TLV_t;

      }
      return;
    }
    else
    //pseudonode
    {
      //let's assume that nsel is interfaceIndex +1
//            int interfaceIndex = nsel - 1;

      ISISinterface *iface = this->getIfaceById(ie->getInterfaceId());

      std::vector<LSPneighbour> neighbours;

      ISISadj *tmpAdj;
//            for (std::vector<ISISinterface>::iterator it = this->ISISIft.begin(); it != this->ISISIft.end(); ++it)
//            {

      for (int offset = 0; (tmpAdj = this->getAdjByInterfaceId(iface->interfaceId, circuitType, offset)) != NULL; offset++)
      {
        LSPneighbour neighbour;
        if (tmpAdj->state < ISIS_ADJ_2WAY || !iface->network)
        {
          continue;
        }

        neighbour.LANid = PseudonodeID(tmpAdj->sysID, 0);

        neighbour.metrics.defaultMetric = 0; //metric from DIS is 0
        neighbour.metrics.delayMetric = 128; //disabled;
        neighbour.metrics.expenseMetric = 128; //disabled
        neighbour.metrics.errortMetric = 128; //disabled

        neighbours.push_back(neighbour);
      }
//            }
      //add also mine non-pseudonode interface as neighbour
      LSPneighbour neighbour;
      neighbour.LANid = PseudonodeID(systemId, 0);
//            this->copyArrayContent((unsigned char*) this->sysId, neighbour.LANid, ISIS_SYSTEM_ID, 0, 0);
//            neighbour.LANid[ISIS_SYSTEM_ID] = 0;
      neighbour.metrics.defaultMetric = 0; //metric to every neighbour in pseudonode LSP is always zero!!!
      neighbour.metrics.delayMetric = 128; //disabled;
      neighbour.metrics.expenseMetric = 128; //disabled
      neighbour.metrics.errortMetric = 128; //disabled

      neighbours.push_back(neighbour);

      //we have vector neighbours for pseudo

      int entrySize = (4 + ISIS_SYSTEM_ID + 1);
      //run until there's any neighbour
      for (; !neighbours.empty();)
      {
        myTLV = new TLV_t;
        myTLV->type = IS_NEIGHBOURS_LSP;
        myTLV->length = 1 + ((4 + ISIS_SYSTEM_ID + 1) * neighbours.size());
        myTLV->value = new unsigned char[ISIS_LSP_MAX_SIZE];

        //virtualFlag
        myTLV->value[0] = 40; //TODO should be 0
        //inner loop for separate TLV; after reaching ISIS_LSP_MAX_SIZE or empty neighbours stop filling this tlv
        // 2 bytes for type and length fields and 1 byte for virtual circuit
        for (unsigned int i = 0; ((i + 1) * entrySize) + 1 < 255 && 2 + 1 + (entrySize * i) + entrySize < ISIS_LSP_MAX_SIZE && !neighbours.empty(); i++)
        {
          myTLV->value[(i * 11) + 1] = neighbours.at(0).metrics.defaultMetric;
          myTLV->value[(i * 11) + 2] = neighbours.at(0).metrics.delayMetric;
          myTLV->value[(i * 11) + 3] = neighbours.at(0).metrics.expenseMetric;
          myTLV->value[(i * 11) + 4] = neighbours.at(0).metrics.errortMetric;
          this->copyArrayContent(neighbours.at(0).LANid.toTLV(), myTLV->value, ISIS_SYSTEM_ID + 1, 0, (i * 11) + 5); //set system ID
          myTLV->length = 1 + ((4 + ISIS_SYSTEM_ID + 1) * (i + 1));
          //delete first entry
          neighbours.erase(neighbours.begin());
        }
        //this tlv is full or no other neighbour entry is present
        tlvTable->push_back(myTLV);
        //TODO do i need to create new myTLV and allocate value?
        //myTLV = new TLV_t;

      }
    }

    //end of TLV IS_NEIGHBOURS_LSP
  }
  else if (tlvType == TLV_MT_PORT_CAP)
  {

    //subTLV VLAN FLAGS
    myTLVVector = genTLV(TLV_MT_PORT_CAP, circuitType, ie->getInterfaceId(), TLV_MT_PORT_CAP_VLAN_FLAG, ie);
    for (std::vector<TLV_t *>::iterator it = myTLVVector.begin(); it != myTLVVector.end();)
    {
      tlvTable->push_back((*it));
      it = myTLVVector.erase(it);
    }

    //end of TLV_MT_PORT_CAP
  }
  else if (tlvType == TLV_TRILL_NEIGHBOR)
  {
    myTLVVector = genTLV(TLV_TRILL_NEIGHBOR, circuitType, ie->getInterfaceId());
    for (std::vector<TLV_t *>::iterator it = myTLVVector.begin(); it != myTLVVector.end();)
    {
      tlvTable->push_back((*it));
      it = myTLVVector.erase(it);
    }
//        myTLVVector.clear();
  }
  else
  {
    EV << "ISIS: ERROR: This TLV type is not (yet) implemented in addTLV" << endl;
  }
}

/*
 * Returns true if this System have ANY adjacency UP (now with extension 2WAY or REPORT) on level specified by circuitType
 * @param circuitType is level.
 */
bool ISISMain::isAdjUp(short circuitType) {
    std::vector<ISISadj>* adjTable = this->getAdjTab(circuitType);

    for (std::vector<ISISadj>::iterator it = adjTable->begin();
            it != adjTable->end(); ++it) {
        if ((*it).state >= ISIS_ADJ_2WAY) //TODO A1
                {
            return true;
        }
    }
    return false;
}

/*
 * This method returns true if there is ANY adjacency on interface identified by interfaceId in state UP (now with extension 2WAY or REPORT)
 * Method does NOT check SNPA (aka MAC).
 * Can be used to check if we should send LSP on such interface.
 * @param interfaceId is gate index
 * @param circuitType is level
 */
bool ISISMain::isUp(int interfaceId, short circuitType) {
    std::vector<ISISadj> *adjTable;

    adjTable = this->getAdjTab(circuitType);
    for (std::vector<ISISadj>::iterator it = adjTable->begin();
            it != adjTable->end(); ++it) {
        if ((*it).interfaceId == interfaceId && (*it).state >= ISIS_ADJ_2WAY) {
            return true;
        }
    }
    return false;

}

/*
 * Verify if adjacency for specified message is in state UP.
 * @param msg incomming message
 * @param circuitType is level.
 */
bool ISISMain::isAdjUp(Packet* packet, short circuitType) {

    auto msg = packet->peekAtFront<ISISMessage>();
    /* Pretty messy code, please clean up */
    std::vector<ISISadj> *adjTable = this->getAdjTab(circuitType);

    int interfaceId = packet->getTag<InterfaceInd>()->getInterfaceId();// = ift->getInterfaceById(ctrl->getInterfaceId())->getNetworkLayerGateIndex();

//    int interfaceId = msg->getArrivalGate()->getIndex();


    MacAddress tmpMac = packet->getTag<MacAddressInd>()->getSrcAddress();// = ctrl->getSrc();
    SystemID sysID;

    sysID = this->getSysID(msg.get());
    for (std::vector<ISISadj>::iterator it = adjTable->begin();
            it != adjTable->end(); ++it) {
        //System-ID match?
        /* Exception for LSP is here because to satisfy  7.3.15.1 a) 6) is enough to check SNPA and interface e.g interfaceId */
        if (sysID == (*it).sysID || msg->getType() == L1_LSP
                || msg->getType() == L2_LSP) {
            //MAC Address and interfaceId
            //we need to check source (tmpMac) and destination interface thru we received this hello
            if (tmpMac.compareTo((*it).mac) == 0 && interfaceId == (*it).interfaceId
                    && (*it).state >= ISIS_ADJ_2WAY) {

//                delete[] sysID;
                return true;

            }
        }
    }
//    delete[] sysID;

    return false;
}

/**
 * Computes distribution tree for each system in LSP DB.
 * @param circuitType is circuit type
 */
void ISISMain::spfDistribTrees(short int circuitType) {

//    std::map<int, ISISPaths_t *> distribTrees;
    //    std::vector<unsigned char *> idVector;
    this->distribTrees.clear();
    std::map<SystemID, int> systemIdVector;  //vector of all ISs's systemId

    systemIdVector = this->getAllSystemIdsFromLspDb(circuitType);

    for (std::map<SystemID, int>::iterator iter = systemIdVector.begin();
            iter != systemIdVector.end(); ++iter) {

        ISISCons_t initial;
        ISISPaths_t *ISISPaths = new ISISPaths_t; //best paths
        ISISPaths_t ISISTent; //
        ISISPath * tmpPath;
//        short circuitType;

        //let's fill up the initial paths with supported-protocol's reachability informations

        //fill ISO
        bool result;
//        circuitType = timer->getIsType();
        result = this->extractISO(&initial, circuitType);
        if (!result) {
            //there was an error during extraction so cancel SPF
            //TODO B5 reschedule
//            this->schedule(timer);
            //TODO B5 clean
            return;
        }

        //put current systemId from systemIdVector on TENT list
        PseudonodeID lspId; // = (unsigned char *)iter->first.c_str(); //returns sysId + 00
        lspId.set(iter->first, 0);
        tmpPath = new ISISPath;
//        tmpPath->to = new unsigned char[ISIS_SYSTEM_ID + 2];
        tmpPath->to = lspId;
//        this->copyArrayContent(lspId, tmpPath->to, ISIS_SYSTEM_ID, 0, 0);
//        tmpPath->to[ISIS_SYSTEM_ID] = 0;
//        tmpPath->to[ISIS_SYSTEM_ID + 1] = 0;

        tmpPath->metric = 0;

        ISISNeighbour *neighbour = new ISISNeighbour;
        neighbour->id = lspId;
//        neighbour->id = new unsigned char[ISIS_SYSTEM_ID + 2];
//        this->copyArrayContent(lspId, neighbour->id, ISIS_SYSTEM_ID, 0, 0);

//        neighbour->id[ISIS_SYSTEM_ID] = 0;
//        neighbour->id[ISIS_SYSTEM_ID + 1] = 0;
        neighbour->type = false; //not a leaf
        tmpPath->from.push_back(neighbour);

        ISISTent.push_back(tmpPath);

        //TODO shoudn't i put myself in PATH list directly?
        for (; !ISISTent.empty();) {
            //tmpPath = this->getBestPath(&(this->ISISTent));

            //this->moveToPath(tmpPath);
            this->bestToPathDT(&initial, &ISISTent, ISISPaths);

        }
        std::sort(ISISPaths->begin(), ISISPaths->end(), ISISPath());
        for (ISISPaths_t::iterator it = ISISPaths->begin();
                it != ISISPaths->end();) {
            if ((*it)->to.getCircuitId() != 0) {
                it = ISISPaths->erase(it);
            } else {
                ++it;
            }
        }

        //TODO B8 If it works, delete commented line
//        this->distribTrees.insert(std::make_pair(lspId[ISIS_SYSTEM_ID - 1] + lspId[ISIS_SYSTEM_ID - 2] * 0xFF, ISISPaths));

        this->distribTrees.insert(std::make_pair(TRILLNickname(lspId.getSystemId()), ISISPaths));


    }

}

/*
 * Moves best path from ISISTent to ISISPaths and initiate move of appropriate connections from init to ISISTent
 * @param initial is set of connections
 * @param ISISTent is set of tentative paths
 * @param ISISPaths is set of best paths from this IS
 */
void ISISMain::bestToPathDT(ISISCons_t *init, ISISPaths_t *ISISTent,
        ISISPaths_t *ISISPaths) {

    ISISPath *path;
    ISISPath *tmpPath;
    //sort it
    std::sort(ISISTent->begin(), ISISTent->end(), ISISPath());
    //save best in path
    path = ISISTent->front();
    //mov
    this->moveToTentDT(init, path, path->to, path->metric, ISISTent);

    ISISTent->erase(ISISTent->begin());

    if ((tmpPath = this->getPath(ISISPaths, path->to)) == NULL) {
        //path to this destination doesn't exist, so simply push

        ISISPaths->push_back(path);
    } else {
        if (tmpPath->metric > path->metric) {
            if (tmpPath->metric > path->metric) {
                EV
                          << "ISIS: Error during SPF. We got better metric than the one PATHS."
                          << endl;
                //we got better metric so clear "from" neighbours
                tmpPath->from.clear();
            }
            EV
                      << "ISIS: Error during SPF. I think we shouldn't have neither same metric."
                      << endl;
            //append
            tmpPath->metric = path->metric;
            std::cout << "pathb metric: " << tmpPath->metric << endl;
            tmpPath->from.clear();
            for (ISISNeighbours_t::iterator it = path->from.begin();
                    it != path->from.end(); ++it) {
                tmpPath->from.push_back((*it));
            }

        }
    }

}

/* Moves connections with matching "from" from init to Tent
 * @param initial is set of connections
 * @param from specify which connections to move
 * @param metric is metric to get to "from" node
 * @param ISISTent is set of tentative paths
 */
void ISISMain::moveToTentDT(ISISCons_t *initial, ISISPath *path, PseudonodeID from,
        uint32_t metric, ISISPaths_t *ISISTent) {

    ISISPath *tmpPath;
    ISISCons_t *cons = this->getCons(initial, from);
    /*       if(cons->empty()){
     EV <<"ISIS: Error during SPF. Didn't find my own LSP"<<endl;
     //TODO clean
     //           delete cons;
     //         return;
     }*/

    for (ISISCons_t::iterator consIt = cons->begin(); consIt != cons->end();
            ++consIt) {
        if ((tmpPath = this->getPath(ISISTent, (*consIt)->to)) == NULL) {
            //path to this destination doesn't exist, so create new
            tmpPath = new ISISPath;
            tmpPath->to = (*consIt)->to;
//            tmpPath->to = new unsigned char[ISIS_SYSTEM_ID + 2];
//            this->copyArrayContent((*consIt)->to, tmpPath->to, ISIS_SYSTEM_ID + 2, 0, 0);
            tmpPath->metric = (*consIt)->metric + metric;
//               std::cout << "patha metric: " << tmpPath->metric << endl;
            if ((*consIt)->from.getCircuitId() != 0) {
                //"from" is pseudonode and i guess that's not desirable
                for (ISISNeighbours_t::iterator nIt = path->from.begin();
                        nIt != path->from.end(); ++nIt) {
                    //if nextHop (from) should be pseudonode, set nextHop as the "to" identifier
                    if ((*nIt)->id.getCircuitId() != 0) {
                        ASSERT(false); //this basically says if you get to this part of the code, it's trouble
                        ISISNeighbour *neigh = (*nIt)->copy();
                        neigh->id = (*consIt)->to;
//                        memcpy(neigh->id, (*consIt)->to, ISIS_SYSTEM_ID + 2);

                        tmpPath->from.push_back(neigh);
                    } else {
                        //                       if(this->compareArrays((*nIt)->id, neighbour->id, ISIS_SYSTEM_ID + 2)){
                        //this neighbour is already there
                        //                           delete neighbour;
                        tmpPath->from.push_back((*nIt)->copy());
                        //                           return;
                        //                       }
                    }
                }
            } else {
                ISISNeighbour *neighbour = new ISISNeighbour;
                neighbour->id = (*consIt)->from;
//                neighbour->id = new unsigned char[ISIS_SYSTEM_ID + 2];
//                this->copyArrayContent((*consIt)->from, neighbour->id, ISIS_SYSTEM_ID + 2, 0, 0);
                neighbour->entry = (*consIt)->entry;
                neighbour->type = false; //not a leaf
                tmpPath->from.push_back(neighbour);
            }

            ////// END
            /* if @param from is THIS IS then next hop (neighbour->id) will be that next hop */
//            if (this->compareArrays((*consIt)->from, (unsigned char *) this->sysId, ISIS_SYSTEM_ID)
//                    && (*consIt)->from[ISIS_SYSTEM_ID] == 0)
//            {
//                this->copyArrayContent((*consIt)->to, neighbour->id, ISIS_SYSTEM_ID + 2, 0, 0);
//                neighbour->entry = (*consIt)->entry;
////                ASSERT(neighbour->entry != NULL);
//                tmpPath->from.push_back(neighbour);
//            }
//            else
//            {
//                //TODO neighbour must be THIS IS or next hop, therefore we need to check whether directly connected
////                   this->copyArrayContent(path->from, neighbour->id, ISIS_SYSTEM_ID + 2, 0, 0);
//                for (ISISNeighbours_t::iterator nIt = path->from.begin(); nIt != path->from.end(); ++nIt)
//                {
//                    //if nextHop (from) should be pseudonode, set nextHop as the "to" identifier
//                    if ((*nIt)->id[ISIS_SYSTEM_ID] != 0)
//                    {
//
//                        ISISNeighbour *neigh = (*nIt)->copy();
//                        memcpy(neigh->id, (*consIt)->to, ISIS_SYSTEM_ID + 2);
//                        tmpPath->from.push_back(neigh);
//                    }
//                    else
//                    {
////                       if(this->compareArrays((*nIt)->id, neighbour->id, ISIS_SYSTEM_ID + 2)){
//                        //this neighbour is already there
////                           delete neighbour;
//                        tmpPath->from.push_back((*nIt)->copy());
////                           return;
////                       }
//                    }
//                }
//            }
//               tmpPath->from = neighbour;
            ISISTent->push_back(tmpPath);
        } else {
            if (tmpPath->metric > (*consIt)->metric + metric) {
                /* true in expression below makes the result tree and not a graph (only single "from" allowed) */
                if (tmpPath->metric > (*consIt)->metric + metric || true) {
                    //we got better metric so clear "from" neighbours
                    tmpPath->from.clear();
                }
                //append
                tmpPath->metric = (*consIt)->metric + metric;
                std::cout << "path metric: " << tmpPath->metric << endl;

                if ((*consIt)->from.getCircuitId() != 0) {
                    for (ISISNeighbours_t::iterator nIt = path->from.begin();
                            nIt != path->from.end(); ++nIt) {
                        //if nextHop (from) should be pseudonode, set nextHop as the "to" identifier
                        if ((*nIt)->id.getCircuitId() != 0) {
                            ASSERT(false); //this basically says if you get to this part of the code, it's trouble
                            ISISNeighbour *neigh = (*nIt)->copy();
                            neigh->id = (*consIt)->to;
//                            memcpy(neigh->id, (*consIt)->to, ISIS_SYSTEM_ID + 2);
                            tmpPath->from.push_back(neigh);
                        } else {
                            //                       if(this->compareArrays((*nIt)->id, neighbour->id, ISIS_SYSTEM_ID + 2)){
                            //this neighbour is already there
                            //                           delete neighbour;
                            tmpPath->from.push_back((*nIt)->copy());
                            //                           return;
                            //                       }
                        }
                    }
                } else {
                    /* the @param from is regular IS so create classic neighbor */
                    ISISNeighbour *neighbour = new ISISNeighbour;
                    neighbour->id = (*consIt)->from;
                    neighbour->entry = (*consIt)->entry;
//                    neighbour->id = new unsigned char[ISIS_SYSTEM_ID + 2];
//                    this->copyArrayContent((*consIt)->from, neighbour->id, ISIS_SYSTEM_ID + 2, 0, 0);
                    neighbour->type = false; //not a leaf

                    for (ISISNeighbours_t::iterator nIt = tmpPath->from.begin();
                            nIt != tmpPath->from.end(); ++nIt) {
                        if ((*nIt)->id == neighbour->id) {
                            //this neighbour is already there
                            delete neighbour;
                            return;
                        }
                    }
                    tmpPath->from.push_back(neighbour);
                }

//                for (ISISNeighbours_t::iterator nIt = path->from.begin(); nIt != path->from.end(); ++nIt)
//                {
//                    bool found = false;
//                    for (ISISNeighbours_t::iterator neIt = tmpPath->from.begin(); neIt != tmpPath->from.end(); ++neIt)
//                    {
//                        //is such nextHop with matching ID AND entry (we may have adjacency with same IS over multiple interfaces)
//                        if (this->compareArrays((*neIt)->id, (*nIt)->id, ISIS_SYSTEM_ID + 2)
//                                && (*neIt)->entry == (*nIt)->entry)
//                        {
//                            //this neighbour is already there
////                             delete neighbour;
////                             continue;
//                            found = true;
//                            break;
//                        }
//
//                    }
//                    if (!found)
//                    {
//                        //if such nextHop from @param path is not already present (not found), add it to nextHops(from)
//                        tmpPath->from.push_back((*nIt)->copy());
//                    }
//
////
//                }

            }

        }

    }
}


std::vector<SystemID> *ISISMain::getSystemIDsFromTreeOnlySource(TRILLNickname nickname, SystemID systemId) {

    std::vector<SystemID> *systemIDs = this->getSystemIDsFromTree(nickname, systemId);

//    it = this->distribTrees.find(nickname);

    for (std::vector<SystemID>::iterator it = systemIDs->begin();
            it != systemIDs->end();) {
        if ((*it) == systemId ) {
            it = systemIDs->erase(it);
        } else {
            ++it;
        }
//            for(ISISNeighbours_t::iterator neighIt = (*pathIt)->from.begin(); neighIt != (*pathIt)->from.end(); ++neighIt){
//                if (memcmp(systemId, (*neighIt)->id, ISIS_SYSTEM_ID) == 0)
//                {
//                    treePaths->push_back((*pathIt)->copy());
//                    break;
//                }
//            }

    }

    return systemIDs;

}

/**
 * Returns system-ids of all neighbours for in distribution tree for specified nickname
 * @param nickname specify tree
 * @param system-id to look for in distribution tree
 * @return vector of system-ids
 */

std::vector<SystemID> *ISISMain::getSystemIDsFromTree(TRILLNickname nickname, SystemID systemId) {

  std::vector<SystemID> *systemIDs = new std::vector<SystemID>;

  std::map<TRILLNickname, ISISPaths_t *>::iterator it;
  if (this->distribTrees.find(nickname) == this->distribTrees.end()) {
    this->spfDistribTrees(L1_TYPE);
  }

  it = this->distribTrees.find(nickname);
  if (it != this->distribTrees.end()) {
    for (ISISPaths_t::iterator pathIt = it->second->begin();  pathIt != it->second->end(); ++pathIt) {
      //if destination match -> push_back
      if ((*pathIt)->to.getSystemId() == systemId) {
        for (ISISNeighbours_t::iterator neighIt = (*pathIt)->from.begin(); neighIt != (*pathIt)->from.end(); ++neighIt) {
          //                    unsigned char *tmpSysId = new unsigned char[ISIS_SYSTEM_ID];
          //                    memcpy(tmpSysId, (*neighIt)->id, ISIS_SYSTEM_ID);
          SystemID tmpSysId;
          tmpSysId = (*neighIt)->id;
          systemIDs->push_back(tmpSysId);

        }

        continue;
      }
      //if at least one "from" match -> push_back (actually there should be only one "from")
      for (ISISNeighbours_t::iterator neighIt = (*pathIt)->from.begin();
          neighIt != (*pathIt)->from.end(); ++neighIt) {
        if (systemId == (*neighIt)->id) {
          //                    unsigned char *tmpSysId = new unsigned char[ISIS_SYSTEM_ID];
          //                    memcpy(tmpSysId, (*pathIt)->to, ISIS_SYSTEM_ID);
          SystemID tmpSysId;
          tmpSysId = (*pathIt)->to.getSystemId();
          systemIDs->push_back(tmpSysId);

          break;
        }
      }

    }

  }
  return systemIDs;

}

/**
 * Returns all system-id from LSP database
 * @param circuitType is circuit type.
 */
std::map<SystemID, int> ISISMain::getAllSystemIdsFromLspDb(short circuitType) {
    LSPRecQ_t * lspDb = this->getLSPDb(circuitType);
    std::map<SystemID, int> systemIdMap;
    for (LSPRecQ_t::iterator it = lspDb->begin(); it != lspDb->end(); ++it) {
        SystemID sysId;
        sysId = (*it)->LSP->getLspID().getSystemId();
//        std::string sysId((char *)systemId, ISIS_SYSTEM_ID);

        systemIdMap[sysId] = 1;
    }

    return systemIdMap;
}

void ISISMain::runSPF(short circuitType, ISISTimer *timer) {

    if (timer != NULL) {
        circuitType = timer->getIsType();
    }

    if (circuitType == L1_TYPE) {
        timer = this->spfL1Timer;
    } else if (circuitType == L2_TYPE) {
        timer = this->spfL2Timer;
    }

    cancelEvent(timer);
    fullSPF(timer);
//    this->schedule(timer);
}

/*
 * Performs full run of SPF algorithm. For L2 ISIS also computes distribution trees.
 * @param timer is initiating timer.
 */
void ISISMain::fullSPF(ISISTimer *timer) {
    if (this->mode == L2_ISIS_MODE) {
        this->spfDistribTrees(timer->getIsType());
    }

    ISISCons_t initial;
    ISISPaths_t *ISISPaths = new ISISPaths_t; //best paths
    ISISPaths_t ISISTent; //
    ISISPath * tmpPath;
    short circuitType;

    //let's fill up the initial paths with supported-protocol's reachability informations

    //fill ISO
    bool result;
    circuitType = timer->getIsType();
    result = this->extractISO(&initial, circuitType);
    if (!result) {
        //there was an error during extraction so cancel SPF
        //TODO B5 reschedule
        this->schedule(timer);
        //TODO B5 clean
        return;
    }

    //put myself (this IS) on TENT list
    LspID lspId = this->getLSPID(); //returns sysId + 00

    tmpPath = new ISISPath;
//    tmpPath->to = new unsigned char[ISIS_SYSTEM_ID + 2];
//    this->copyArrayContent(lspId, tmpPath->to, ISIS_SYSTEM_ID + 2, 0, 0);
    tmpPath->to.setSystemId(systemId);

    tmpPath->metric = 0;

    ISISNeighbour *neighbour = new ISISNeighbour;
//    neighbour->id = new unsigned char[ISIS_SYSTEM_ID + 2];
//    this->copyArrayContent(lspId, neighbour->id, ISIS_SYSTEM_ID + 2, 0, 0);
    neighbour->id = lspId.getPseudonodeID();
    neighbour->type = false; //not a leaf
    tmpPath->from.push_back(neighbour);

    ISISTent.push_back(tmpPath);

//    ISISCons_t *cons = this->getCons(&initial, lspId);
//    if(cons->empty()){
//        EV <<"ISIS: Error during SPF. Didn't find my own LSP"<<endl;
//        //TODO clean
//        delete cons;
//        return;
//    }
//

    //add my connections as a starting point
    /*  for(ISISCons_t::iterator it = cons->begin(); it != cons->end(); ++it){
     if ((tmpPath = this->getPath(&(ISISTent), (*it)->to)) == NULL)
     {
     //path to this destination doesn't exist, co create new
     tmpPath = new ISISPath;
     tmpPath->to = new unsigned char[ISIS_SYSTEM_ID + 2];
     this->copyArrayContent((*it)->to, tmpPath->to, ISIS_SYSTEM_ID + 2, 0, 0);
     tmpPath->metric = (*it)->metric;

     ISISNeighbour *neighbour = new ISISNeighbour;
     neighbour->id = new unsigned char[ISIS_SYSTEM_ID + 2];
     this->copyArrayContent((*it)->from, neighbour->id, ISIS_SYSTEM_ID + 2, 0, 0);
     neighbour->type = false; //not a leaf
     tmpPath->from.push_back(neighbour);

     ISISTent.push_back(tmpPath);
     }
     else
     {
     if(tmpPath->metric >= (*it)->metric){
     if(tmpPath->metric > (*it)->metric){
     //we got better metric so clear "from" neighbours
     tmpPath->from.clear();
     }
     //append
     tmpPath->metric = (*it)->metric;
     ISISNeighbour *neighbour = new ISISNeighbour;
     neighbour->id = new unsigned char[ISIS_SYSTEM_ID + 2];
     this->copyArrayContent((*it)->from, neighbour->id, ISIS_SYSTEM_ID + 2, 0, 0);
     neighbour->type = false; //not a leaf
     tmpPath->from.push_back(neighbour);

     }

     }

     }*/

    //TODO shoudn't i put myself in PATH list directly?
    for (; !ISISTent.empty();) {
        //tmpPath = this->getBestPath(&(this->ISISTent));

        //this->moveToPath(tmpPath);
        this->bestToPath(&initial, &ISISTent, ISISPaths);

    }
    std::sort(ISISPaths->begin(), ISISPaths->end(), ISISPath());

    this->printPaths(ISISPaths);

    //find shortest metric in TENT

    /*  ISISPaths_t *ISISPathsISO = getPathsISO(circuitType);
     if(ISISPathsISO != NULL){
     delete ISISPathsISO;
     }

     ISISPathsISO = ISISPaths;
     */
    if (circuitType == L1_TYPE) {
//        if (this->L1ISISPathsISO != NULL)
//        {
//            this->L1ISISPathsISO->clear();
//            delete this->L1ISISPathsISO;
//            this->L1ISISPathsISO->swap(*ISISPaths);
        //delete ISISPaths;
//        }else{
        this->L1ISISPathsISO = ISISPaths;
//        }
    } else if (circuitType == L2_TYPE) {
        if (this->L2ISISPathsISO != NULL) {
            delete this->L2ISISPathsISO;
        }
        this->L2ISISPathsISO = ISISPaths;
    }

    ISISAPaths_t *areas = new ISISAPaths_t;
    ISISPaths_t *ISISPathsISO = getPathsISO(circuitType);
    if (circuitType == L2_TYPE) {
        this->extractAreas(ISISPathsISO, areas, circuitType);
        std::cout << "Print Areas\n";
        this->printAPaths(areas);

        for (ISISAPaths_t::iterator it = areas->begin(); it != areas->end();
                ++it) {

//            if ((*it)->to.getCircuitId() != 0) {
//                //skip all pseudonodes, put only real nodes to routing table
//                continue;
//            }
            //for every neighbour (nextHop)
            for (ISISNeighbours_t::iterator nIt = (*it)->from.begin();
                    nIt != (*it)->from.end(); ++nIt) {
                //getAdjacency by systemID and circuitType, then find iface by interfaceId and finaly get the InterfaceEntry
                //if it's a pseudonode then find interface based on matching DIS
                if ((*nIt)->id.getCircuitId() != 0) {
                    for (ISISInterTab_t::iterator tabIt = this->ISISIft.begin();
                            tabIt != this->ISISIft.end(); ++tabIt) {
                        if (circuitType == L1_TYPE) {
                            if ((*tabIt).L1DIS == (*nIt)->id) {
                                (*nIt)->entry = (*tabIt).entry;
                            }
                        } else {
                            if ((*tabIt).L2DIS == (*nIt)->id) {
                                (*nIt)->entry = (*tabIt).entry;
                            }
                        }
                    }

                } else {

                    ISISadj *tmpAdj = getAdjBySystemID((*nIt)->id, circuitType);
                    if (tmpAdj != NULL) {
                        (*nIt)->entry = this->getIfaceById(
                                tmpAdj->interfaceId)->entry;
                    } else {
                        (*nIt)->entry = NULL;
                    }
                }
                //            (*nIt)->entry = this->getIfaceById((this->getAdjBySystemID((*nIt)->id, circuitType))->interfaceId)->entry;
            }

            CLNSRoute* entry = new CLNSRoute();
            ClnsAddress dest;
            dest.set((*it)->to.getAreaId(), 0);
            ClnsAddress nextHop;
            nextHop.set(0,(*(*it)->from.begin())->id.getSystemId().getSystemId());
            entry->setDestination(dest);
            entry->setGateway(nextHop);
            entry->setInterface((*(*it)->from.begin())->entry);
            entry->setSourceType(IRoute::SourceType::ISIS);
            entry->setAdminDist(CLNSRoute::RouteAdminDist::dISIS);
            entry->setMetric((*it)->metric);

            bool found = false;
            for(int i = 0; i < clnsrt->getNumRoutes(); i++){
              auto iter = clnsrt->getRoute(i);
              //find route in routing table
              if(dest == iter->getDestination()){
                //found
                found = true;
                //same administrative distance?
                if(iter->getAdminDist() == CLNSRoute::RouteAdminDist::dISIS){
                  iter->setMetric(entry->getMetric());
                  iter->setGateway(entry->getGateway());
                  iter->setInterface(entry->getInterface());
                  delete entry;
                }else if (iter->getAdminDist() > CLNSRoute::RouteAdminDist::dISIS){
                  clnsrt->removeRoute(iter);
                  clnsrt->addRoute(entry);
                }
              }
            }

            if(!found){
              clnsrt->addRoute(entry);
            }
//            this->clnsTable->addRecord(
//                    new CLNSRoute((*it)->to, ISIS_SYSTEM_ID + 1, (*it)->from,
//                            (*it)->metric));

        }
    }

    //initiate search for closest attached L1_L2 IS
    if (!this->att) {
        this->setClosestAtt();
    }
//    this->clnsTable->dropTable();
    //TODO B1 rewrite this mess

    for (ISISPaths_t::iterator it = ISISPaths->begin(); it != ISISPaths->end();
            ++it) {

        if ((*it)->to.getCircuitId() != 0) {
            //skip all pseudonodes, put only real nodes to routing table
            continue;
        }
        //for every neighbour (nextHop)
        for (ISISNeighbours_t::iterator nIt = (*it)->from.begin();
                nIt != (*it)->from.end(); ++nIt) {
            //getAdjacency by systemID and circuitType, then find iface by interfaceId and finaly get the InterfaceEntry
            //if it's a pseudonode then find interface based on matching DIS
            if ((*nIt)->id.getCircuitId() != 0) {
                for (ISISInterTab_t::iterator tabIt = this->ISISIft.begin();
                        tabIt != this->ISISIft.end(); ++tabIt) {
                    if (circuitType == L1_TYPE) {
                        if ((*tabIt).L1DIS == (*nIt)->id) {
                            (*nIt)->entry = (*tabIt).entry;
                        }
                    } else {
                        if ((*tabIt).L2DIS == (*nIt)->id) {
                            (*nIt)->entry = (*tabIt).entry;
                        }
                    }
                }

            } else {

                ISISadj *tmpAdj = getAdjBySystemID((*nIt)->id, circuitType);
                if (tmpAdj != NULL) {
                    (*nIt)->entry =
                            this->getIfaceById(tmpAdj->interfaceId)->entry;
                } else {
                    (*nIt)->entry = NULL;
                }
            }
//            (*nIt)->entry = this->getIfaceById((this->getAdjBySystemID((*nIt)->id, circuitType))->interfaceId)->entry;
        }

        if((*it)->to.getSystemId() == systemId){
          continue;
        }
        CLNSRoute* entry = new CLNSRoute();
        ClnsAddress dest;
        dest.set(areaID.getAreaId(), (*it)->to.getSystemId().getSystemId());
        ClnsAddress nextHop;
        nextHop.set(areaID.getAreaId(),(*(*it)->from.begin())->id.getSystemId().getSystemId());
        entry->setDestination(dest);
        entry->setGateway(nextHop);
        entry->setInterface((*(*it)->from.begin())->entry);
        entry->setSourceType(IRoute::SourceType::ISIS);
        entry->setAdminDist(CLNSRoute::RouteAdminDist::dISIS);
        entry->setMetric((*it)->metric);

        if(entry->getInterface() == nullptr)
        {
          ISISadj* adj = getAdjBySystemID((*(*it)->from.begin())->id.getSystemId(), circuitType);
          int interfaceId = adj->interfaceId;
          ISISinterface* isisIface = getIfaceById(interfaceId);
          entry->setInterface(isisIface->entry);

        }

        bool found = false;
        for(int i = 0; i < clnsrt->getNumRoutes(); i++){
          auto iter = clnsrt->getRoute(i);
          //find route in routing table
          if(dest == iter->getDestination()){
            //found
            found = true;
            //same administrative distance?
            if(iter->getAdminDist() == CLNSRoute::RouteAdminDist::dISIS){
              iter->setMetric(entry->getMetric());
              iter->setGateway(entry->getGateway());
              iter->setInterface(entry->getInterface());
              delete entry;
            }else if (iter->getAdminDist() > CLNSRoute::RouteAdminDist::dISIS){
              clnsrt->removeRoute(iter);
              clnsrt->addRoute(entry);
            }
          }
        }

        if(!found){
          clnsrt->addRoute(entry);
        }

//        this->clnsTable->addRecord(
//                new CLNSRoute((*it)->to, ISIS_SYSTEM_ID + 1, (*it)->from,
//                        (*it)->metric));

    }

    this->schedule(timer);
    delete ISISPaths;
}
/**
 * Extracts areas from LSP DB for every path in @param paths
 * @param paths is a set of paths to be searched
 * @param areas output variable to store extracted paths
 * @param circuitType is either L1 or L2
 */
void ISISMain::extractAreas(ISISPaths_t* paths, ISISAPaths_t* areas, short circuitType) {

    Ptr<ISISLSPPacket> lsp;
    LSPRecord* lspRec;
    TLV_t* tmpTLV;
    ISISAPath *tmpPath;

  for (ISISPaths_t::iterator pathIt = paths->begin(); pathIt != paths->end(); ++pathIt)
  {
    //for each record in best paths extract connected Areas
    lspRec = this->getLSPFromDbByID(LspID((*pathIt)->to), circuitType);
        if (lspRec == NULL) {
            continue;
        }
        lsp = lspRec->LSP;
        for (int offset = 0; (tmpTLV = this->getTLVByType(lsp.get(), AREA_ADDRESS, offset)) != NULL; offset++)
        {
          AreaId tmpAreaId;
          tmpAreaId.fromTLV(tmpTLV->value);

          if ((tmpPath = this->getAPath(areas, tmpAreaId)) == NULL) {
            //path to this address doesn't exist, so create new
            tmpPath = new ISISAPath;
            tmpPath->to = tmpAreaId;
//            tmpPath->to = new unsigned char[ISIS_SYSTEM_ID + 2];
//            this->copyArrayContent(tmpTLV->value, tmpPath->to, ISIS_AREA_ID, i + 1, 0);
//            //I guess this just fills the rest of 'to' field with zeros
//            for (unsigned int b = ISIS_AREA_ID; b < ISIS_SYSTEM_ID + 2;  b++)
//            {
//              tmpPath->to[b] = 0;
//            }
            tmpPath->metric = (*pathIt)->metric;

            //copy all neighbours from 'from'
            for (ISISNeighbours_t::iterator nIt = (*pathIt)->from.begin(); nIt != (*pathIt)->from.end(); ++nIt)
            {
              tmpPath->from.push_back((*nIt)->copy());
            }

            areas->push_back(tmpPath);

          } else {
            //path to this address already exists, so check metric
            if (tmpPath->metric >= (*pathIt)->metric) {
              if (tmpPath->metric > (*pathIt)->metric) {
                //we got better metric for this area so clear 'from' neighbours
                tmpPath->from.clear();
              }
              tmpPath->metric = (*pathIt)->metric;
              //copy all neighbours from 'from'
              for (ISISNeighbours_t::iterator nIt =
                  (*pathIt)->from.begin();
                  nIt != (*pathIt)->from.end(); ++nIt) {
                tmpPath->from.push_back((*nIt)->copy());
              }

            }
          }

        }
    }

}
/**
 * Sets attached system to closest system that advertise att flag
 */
void ISISMain::setClosestAtt(void) {

    uint32_t metric = UINT32_MAX;
    ISISNeighbours_t * attIS;

    if (this->att || this->L1ISISPathsISO == NULL) {
        return;
    }

//    if(attIS != nullptr){
//      delete attIS;
//    }
    attIS = new ISISNeighbours_t;
    for (LSPRecQ_t::iterator it = this->L1LSPDb->begin();
            it != this->L1LSPDb->end(); ++it) {
        if ((*it)->LSP->getPATTLSPDBOLIS() && 0x10) {
            //we found LSP with Attached flag set
            //find originator of this LSP in bestPaths
            for (ISISPaths_t::iterator pIt = this->L1ISISPathsISO->begin();
                    pIt != this->L1ISISPathsISO->end(); ++pIt) {
                LspID lspID = (*it)->LSP->getLspID();
                /* if LSP-ID match AND metric is smaller or same as the current one, add that IS as closest L1L2 IS */
                if (lspID.getSystemId() == (*pIt)->to.toSysID()) {
                    if ((*pIt)->metric < metric) {
                        //clear attIS
                        //TODO deallocate neighbours ID
                        attIS->clear();
                    }

                    if ((*pIt)->metric <= metric) {
                        //TODO replace NULL with interfaceEntry
                      InterfaceEntry *entry = ((*pIt)->from.at(0))->entry;
                        ISISNeighbour *att = new ISISNeighbour(
                                lspID.getPseudonodeID(), false, entry);
                        attIS->push_back(att);
//                        metric = (*pIt)->metric = metric;
                        metric = (*pIt)->metric;
                    }
                }

//                delete[] lspID;
            }
        }
    }

    if (this->attIS != NULL && !this->attIS->empty()) {
        this->attIS->clear();
    }
    this->attIS = attIS;

}

/*
 * Print best paths informations to stdout
 * @param paths is set of best paths.
 */
void ISISMain::printPaths(ISISPaths_t *paths) {

    std::cout << "Best paths of IS: " << areaID << ".";
    //print area id
//    for (unsigned int i = 0; i < ISIS_AREA_ID; i++)
//    {
//        std::cout << std::setfill('0') <<  std::setw(2) << std::dec << (unsigned int) this->areaId[i];
//        if (i % 2 == 0)
//            std::cout << ".";
//
//    }

    //print system id
    std::cout << systemId;
//    for (unsigned int i = 0; i < 6; i++)
//    {
//        std::cout << std::setfill('0') <<  std::setw(2) << std::dec << (unsigned int) this->sysId[i];
//        if (i % 2 == 1)
//            std::cout << ".";
//    }

    //print NSEL
    std::cout << "." << "00";
//    std::cout << std::setfill('0') <<  std::setw(2) << std::dec << (unsigned int) this->NSEL[0] << "\tNo. of paths: " << paths->size()
    std::cout<< endl;
    for (ISISPaths_t::iterator it = paths->begin(); it != paths->end(); ++it) {
        std::cout << "To: " << (*it)->to;
//        this->printSysId((*it)->to);

//        for (unsigned int i = 0; i < 6; i++)
//        {
//            std::cout << std::setfill('0') <<  std::setw(2) << std::dec << (unsigned int) (*it)->to[i];
//            if (i % 2 == 1)
//                std::cout << ".";
//        }

//        std::cout << std::setfill('0') <<  std::setw(2) << std::dec << (unsigned short) (*it)->to[6];
        std::cout << "\t\t metric: " << (*it)->metric << "\t via: ";
        if ((*it)->from.empty()) {
            std::cout << "EMPTY";
        }
        for (ISISNeighbours_t::iterator nIt = (*it)->from.begin();
                nIt != (*it)->from.end(); ++nIt) {
//            std::cout << "\t\t\t\t\t";
            this->printSysId((*nIt)->id);
            std::cout << ".";
            std::cout << std::setfill('0') << std::setw(2) << std::dec
                    << (unsigned short) (*nIt)->id.getCircuitId();
            std::cout << " ";
        }
        std::cout << endl;
    }
    std::cout << endl;
}


/*
 * Print best paths informations to stdout
 * @param paths is set of best paths.
 */
void ISISMain::printAPaths(ISISAPaths_t *paths) {

    std::cout << "Best paths of IS: " << areaID << ".";

    //print system id
    std::cout << systemId << ".";


    //print NSEL
    std::cout << "." << "00";

    std::cout<< endl;
    for (ISISAPaths_t::iterator it = paths->begin(); it != paths->end(); ++it) {
        std::cout << "To: " << (*it)->to;
        std::cout << "\t\t metric: " << (*it)->metric << "\t via: ";
        if ((*it)->from.empty()) {
            std::cout << "EMPTY";
        }
        for (ISISNeighbours_t::iterator nIt = (*it)->from.begin();
                nIt != (*it)->from.end(); ++nIt) {

            this->printSysId((*nIt)->id);

            std::cout << std::setfill('0') << std::setw(2) << std::dec
                    << (unsigned short) (*nIt)->id.getCircuitId();
            std::cout << " ";
        }
        std::cout << endl;
    }
    std::cout << endl;
}

/*
 * Moves best path from ISISTent to ISISPaths and initiate move of appropriate connections from init to ISISTent
 * @param initial is set of connections
 * @param ISISTent is set of tentative paths
 * @param ISISPaths is set of best paths from this IS
 */
void ISISMain::bestToPath(ISISCons_t *init, ISISPaths_t *ISISTent, ISISPaths_t *ISISPaths) {

    ISISPath *path;
    ISISPath *tmpPath;
    //sort it
    std::sort(ISISTent->begin(), ISISTent->end(), ISISPath());
    //save best in path
    path = ISISTent->front();
    //mov
    this->moveToTent(init, path, path->to, path->metric, ISISTent);

    ISISTent->erase(ISISTent->begin());

    if ((tmpPath = this->getPath(ISISPaths, path->to)) == NULL) {
        //path to this destination doesn't exist, so simply push

        ISISPaths->push_back(path);
    } else {
        if (tmpPath->metric >= path->metric) {
            if (tmpPath->metric > path->metric) {
                EV
                          << "ISIS: Error during SPF. We got better metric than the one PATHS."
                          << endl;
                //we got better metric so clear "from" neighbours
                tmpPath->from.clear();
            }
            EV
                      << "ISIS: Error during SPF. I think we shouldn't have neither same metric."
                      << endl;
            //append
            tmpPath->metric = path->metric;
            std::cout << "pathb metric: " << tmpPath->metric << endl;
            for (ISISNeighbours_t::iterator it = path->from.begin();
                    it != path->from.end(); ++it) {
                tmpPath->from.push_back((*it));
            }

        }
    }

}

/* Moves connections with matching "from" from init to Tent
 * @param initial is set of connections
 * @param from specify which connections to move
 * @param metric is metric to get to "from" node
 * @param ISISTent is set of tentative paths
 */
void ISISMain::moveToTent(ISISCons_t *initial, ISISPath *path, PseudonodeID from,
        uint32_t metric, ISISPaths_t *ISISTent) {

    ISISPath *tmpPath;
    ISISCons_t *cons = this->getCons(initial, from);
    /*       if(cons->empty()){
     EV <<"ISIS: Error during SPF. Didn't find my own LSP"<<endl;
     //TODO clean
     //           delete cons;
     //         return;
     }*/

    for (ISISCons_t::iterator consIt = cons->begin(); consIt != cons->end();
            ++consIt) {
        if ((tmpPath = this->getPath(ISISTent, (*consIt)->to)) == NULL) {
            //path to this destination doesn't exist, so create new
            tmpPath = new ISISPath;
            tmpPath->to = (*consIt)->to;
//            tmpPath->to = new unsigned char[ISIS_SYSTEM_ID + 2];
//            this->copyArrayContent((*consIt)->to, tmpPath->to, ISIS_SYSTEM_ID + 2, 0, 0);
            tmpPath->metric = (*consIt)->metric + metric;
//               std::cout << "patha metric: " << tmpPath->metric << endl;

            ISISNeighbour *neighbour = new ISISNeighbour;

//            neighbour->id = new unsigned char[ISIS_SYSTEM_ID + 2];
            /* if @param from is THIS IS then next hop (neighbour->id) will be that next hop */
            if ((*consIt)->from.getSystemId() == systemId
                    && (*consIt)->from.getCircuitId() == 0) {
//                this->copyArrayContent((*consIt)->to, neighbour->id, ISIS_SYSTEM_ID + 2, 0, 0);
                neighbour->id = (*consIt)->to;
                neighbour->entry = (*consIt)->entry;
//                ASSERT(neighbour->entry != NULL);
                tmpPath->from.push_back(neighbour);
            } else {
                //TODO neighbour must be THIS IS or next hop, therefore we need to check whether directly connected
//                   this->copyArrayContent(path->from, neighbour->id, ISIS_SYSTEM_ID + 2, 0, 0);
                for (ISISNeighbours_t::iterator nIt = path->from.begin();
                        nIt != path->from.end(); ++nIt) {
                    //if nextHop (from) should be pseudonode, set nextHop as the "to" identifier
                    if ((*nIt)->id.getCircuitId() != 0) {

                        ISISNeighbour *neigh = (*nIt)->copy();
                        neigh->id = (*consIt)->to;
//                        memcpy(neigh->id, (*consIt)->to, ISIS_SYSTEM_ID + 2);
                        tmpPath->from.push_back(neigh);
                    } else {
//                       if(this->compareArrays((*nIt)->id, neighbour->id, ISIS_SYSTEM_ID + 2)){
                        //this neighbour is already there
//                           delete neighbour;
                        tmpPath->from.push_back((*nIt)->copy());
//                           return;
//                       }
                    }
                }
            }
            neighbour->type = false; //not a leaf
//               tmpPath->from = neighbour;

            ISISTent->push_back(tmpPath);
        } else {
            if (tmpPath->metric >= (*consIt)->metric + metric) {
                if (tmpPath->metric > (*consIt)->metric + metric) {
                    //we got better metric so clear "from" neighbours
                    tmpPath->from.clear();
                }
                //append
                tmpPath->metric = (*consIt)->metric + metric;
                std::cout << "path metric: " << tmpPath->metric << endl;
//                   ISISNeighbour *neighbour = new ISISNeighbour;
//                   neighbour->id = new unsigned char[ISIS_SYSTEM_ID + 2];
//                   this->copyArrayContent((*consIt)->from, neighbour->id, ISIS_SYSTEM_ID + 2, 0, 0);
//                   neighbour->type = false; //not a leaf

//                   for(ISISNeighbours_t::iterator nIt = tmpPath->from.begin(); nIt != tmpPath->from.end(); ++nIt){
//                       if(this->compareArrays((*nIt)->id, neighbour->id, ISIS_SYSTEM_ID + 2)){
//                           //this neighbour is already there
//                           delete neighbour;
//                           return;
//                       }
//                   }
//                   tmpPath->from.push_back(neighbour);

                for (ISISNeighbours_t::iterator nIt = path->from.begin();
                        nIt != path->from.end(); ++nIt) {
                    bool found = false;
                    for (ISISNeighbours_t::iterator neIt =
                            tmpPath->from.begin(); neIt != tmpPath->from.end();
                            ++neIt) {
                        //is such nextHop with matching ID AND entry (we may have adjacency with same IS over multiple interfaces)
                        if ((*neIt)->id == (*nIt)->id
                                && (*neIt)->entry == (*nIt)->entry) {
                            //this neighbour is already there
//                             delete neighbour;
//                             continue;
                            found = true;
                            break;
                        }

                    }
                    if (!found) {
                        //if such nextHop from @param path is not already present (not found), add it to nextHops(from)
                        tmpPath->from.push_back((*nIt)->copy());
                    }

//
                }

            }

        }

    }
}

/*
 * Returns pointer to besh path in provided vector.
 * @param paths is vector of paths
 * @return pointer to path with best metric
 */
ISISPath * ISISMain::getBestPath(ISISPaths_t *paths) {

    std::sort(paths->begin(), paths->end(), ISISPath());
    return paths->front();

}

void ISISMain::getBestMetric(ISISPaths_t *paths) {

    for (ISISPaths_t::iterator it = paths->begin(); it != paths->end(); ++it) {

    }
}

/*
 * This methods extract ISO information from LSP database.
 * @param initial is set of unidirectional connections
 * @return false if extractict fails
 */
bool ISISMain::extractISO(ISISCons_t *initial, short circuitType) {

  ISISLspDb_t *lspDb = this->getLSPDb(circuitType);
  LspID lspId;

  ISISCon* connection;

  for (ISISLspDb_t::iterator it = lspDb->begin(); it != lspDb->end(); ++it)
  {

    //TODO if LSP's remaining time equals to zero then continue to next lsp;
    if ((*it)->LSP->getRemLifeTime() == 0 || (*it)->LSP->getSeqNumber() == 0)
    {
      continue;
    }
    //getLspId
    lspId = (*it)->LSP->getLspID();

    //check if it's a zero-th fragment. if not try to find it -> getLspFromDbByID

    if (lspId.getFragmentId() != 0)
    {
      uint backup = lspId.getFragmentId();
      lspId.setFragmentId(0);
      //if doesn't exist -> continue to next lsp
      if (this->getLSPFromDbByID(lspId, circuitType) == NULL)
      {
        continue;
      }
      lspId.setFragmentId(backup);

    }
    //else

    TLV_t *tmpTLV;
    for (int offset = 0; (tmpTLV = this->getTLVByType((*it)->LSP.get(), IS_NEIGHBOURS_LSP, offset)) != NULL; offset++)
    {
      for (unsigned int i = 1; i + 11 <= tmpTLV->length; i += 11)
      {
        connection = new ISISCon;
        connection->from = lspId.getPseudonodeID();
//                connection->from = new unsigned char[ISIS_SYSTEM_ID + 2];
//                this->copyArrayContent(lspId, connection->from, ISIS_SYSTEM_ID + 1, 0, 0);
//                connection->from[ISIS_SYSTEM_ID + 1] = 0;
        connection->to.fromTLV(&(tmpTLV->value[i + 4])); //TODO A! check if it is the same format as PseudonodeID a.k.a LAN ID in TLV
//                connection->to = new unsigned char[ISIS_SYSTEM_ID + 2];
//                this->copyArrayContent(tmpTLV->value, connection->to,
//                        ISIS_SYSTEM_ID + 1, i + 4, 0);
//                connection->to[ISIS_SYSTEM_ID + 1] = 0;
        connection->metric = tmpTLV->value[i]; //default metric
        //                    std::cout << "connection metric : " << connection->metric << endl;
        connection->type = false; //it's not a leaf

        //if this LSP has been generated by this IS (lspId == sysId)
        if (lspId == systemId)
        {
          //TODO FIX
          //this destroys everything i was trying to accomplish by ingoring multiple adjacencies between two ISs
          //if destination of this connection (entry in LSP) is to Pseudonode
          if (connection->to.getCircuitId() != 0)
          {
            //if System-ID part of Pseudonode's LAN-ID belongs to this system
            if (connection->to.getSystemId() == systemId)
            {
              //then the connection entry index equals to Pseudonode-ID byte minus 1
              connection->entry = getIfaceById(connection->to.getCircuitId())->entry;
            }
            else
            {
              //if not, we have too search through Adjacency table to find adjacency with matching destination
              ISISadj *tmpAdj = getAdjBySystemID(connection->to, circuitType);
              if (!tmpAdj)
              {
                return false;
              }
              connection->entry = getIfaceById(tmpAdj->interfaceId)->entry;

              //                            connection->entry = this->ISISIft.at(connection->to[ISIS_SYSTEM_ID] - 1).entry;
            }
          }
          //if it is Pseudonode's LSP
          else if (lspId.getCircuitId() != 0)
          {
//                        connection->entry = this->ISISIft.at(lspId[ISIS_SYSTEM_ID] - 1).entry;
            connection->entry = getIfaceById(lspId.getCircuitId())->entry;
          }
          else
          {
            ISISadj *tmpAdj = getAdjBySystemID(connection->to, circuitType);
            if(tmpAdj == nullptr){
              connection->entry = nullptr;
            }else {
            connection->entry = getIfaceById(tmpAdj->interfaceId)->entry;
            }
          }
        }
        else
        {
          connection->entry = NULL;
        }

        initial->push_back(connection);
        //path->neighbours.push_back(neighbour);
      }

    }

  }

  this->twoWayCheck(initial);

  return true;
}

/*
 * This methods performs two-way check of reported connections.
 * @param cons is vector of connections.
 */
void ISISMain::twoWayCheck(ISISCons_t *cons) {
//    ISISCons_t *tmpCons;
    for (ISISCons_t::iterator it = cons->begin(); it != cons->end();) {
        //if there is not reverse connection
        //TODO is this enough? there could be two one-way connections between two ISs
        if (!this->isCon(cons, (*it)->to, (*it)->from)) {
            it = cons->erase(it);
        } else {
            ++it;
        }
    }
}

/*
 * @return vector of connections with matching from in @param cons.
 */
ISISCons_t * ISISMain::getCons(ISISCons_t *cons, PseudonodeID from) {

    ISISCons_t *retCon = new ISISCons_t;
    for (ISISCons_t::iterator it = cons->begin(); it != cons->end();) {
        if ((*it)->from == from) {
            retCon->push_back((*it));
            it = cons->erase(it);
        } else {
            ++it;
        }
    }

    return retCon;
}
/*
 * Check if connection from @param from to @param to exists.
 * @return true if there is connection with matching field from and to
 *
 */
bool ISISMain::isCon(ISISCons_t *cons, PseudonodeID from,  PseudonodeID to) {

    for (ISISCons_t::iterator it = cons->begin(); it != cons->end(); ++it) {
        if ((*it)->from == from && (*it)->to == to) {
            return true;
        }
    }
    return false;
}

/*
 * Returns path with specified id.
 * @param paths vector of paths
 * @param id is identificator of desired path
 * @return path
 */
ISISPath * ISISMain::getPath(ISISPaths_t *paths, PseudonodeID id) {

    for (ISISPaths_t::iterator it = paths->begin(); it != paths->end(); ++it) {

        if (id == (*it)->to) {
            return (*it);
        }
    }

    return NULL;
}

/*
 * Returns path with specified id.
 * @param paths vector of paths
 * @param id is identificator of desired path
 * @return path
 */
ISISAPath * ISISMain::getAPath(ISISAPaths_t *paths, AreaId id) {

    for (ISISAPaths_t::iterator it = paths->begin(); it != paths->end(); ++it) {

        if (id == (*it)->to) {
            return (*it);
        }
    }

    return NULL;
}


void ISISMain::sendDown(Packet* packet) {

    if(mode == ISIS_MODE::L2_ISIS_MODE){
        packet->getTag<MacAddressReq>()->setDestAddress(MacAddress(ALL_IS_IS_RBRIDGES));
        packet->addTagIfAbsent<PacketProtocolTag>()->setProtocol(&Protocol::l2isis);
//        packet->addTagIfAbsent<DispatchProtocolReq>()->setProtocol(&Protocol::trill);
        packet->addTagIfAbsent<DispatchProtocolReq>()->setProtocol(&Protocol::ethernetMac);

        send(packet, "lowerLayerOut");
    }else{
        send(packet, "lowerLayerOut");
    }


}




void ISISMain::setHelloCounter(unsigned long helloCounter) {
    this->helloCounter = helloCounter;
}

void ISISMain::setL1CSNPInterval(int l1CsnpInterval) {
    L1CSNPInterval = l1CsnpInterval;
}

void ISISMain::setL1HelloInterval(int l1HelloInterval) {
    L1HelloInterval = l1HelloInterval;
}

void ISISMain::setL1HelloMultiplier(short l1HelloMultiplier) {
    L1HelloMultiplier = l1HelloMultiplier;
}

void ISISMain::setL1LspGenInterval(int l1LspGenInterval) {
    L1LspGenInterval = l1LspGenInterval;
}

void ISISMain::setL1LspInitWait(int l1LspInitWait) {
    L1LspInitWait = l1LspInitWait;
}

void ISISMain::setL1LspSendInterval(int l1LspSendInterval) {
    L1LspSendInterval = l1LspSendInterval;
}

void ISISMain::setL1PSNPInterval(int l1PsnpInterval) {
    L1PSNPInterval = l1PsnpInterval;
}

void ISISMain::setL1SpfFullInterval(int l1SpfFullInterval) {
    L1SPFFullInterval = l1SpfFullInterval;
}

void ISISMain::setL2CSNPInterval(int l2CsnpInterval) {
    L2CSNPInterval = l2CsnpInterval;
}

void ISISMain::setL2HelloInterval(int l2HelloInterval) {
    L2HelloInterval = l2HelloInterval;
}

void ISISMain::setL2HelloMultiplier(short l2HelloMultiplier) {
    L2HelloMultiplier = l2HelloMultiplier;
}

void ISISMain::setL2LspGenInterval(int l2LspGenInterval) {
    L2LspGenInterval = l2LspGenInterval;
}

void ISISMain::setL2LspInitWait(int l2LspInitWait) {
    L2LspInitWait = l2LspInitWait;
}

void ISISMain::setL2LspSendInterval(int l2LspSendInterval) {
    L2LspSendInterval = l2LspSendInterval;
}

void ISISMain::setL2PSNPInterval(int l2PsnpInterval) {
    L2PSNPInterval = l2PsnpInterval;
}

void ISISMain::setL2SpfFullInterval(int l2SpfFullInterval) {
    L2SPFFullInterval = l2SpfFullInterval;
}

void ISISMain::setLspInterval(int lspInterval) {
    this->lspInterval = lspInterval;
}

void ISISMain::setLspMaxLifetime(int lspMaxLifetime) {
    this->lspMaxLifetime = lspMaxLifetime;
}

void ISISMain::appendISISInterface(ISISinterface iface) {
    this->ISISIft.push_back(iface);
}

int ISISMain::getL1HelloInterval() const {
    return L1HelloInterval;
}

short ISISMain::getL1HelloMultiplier() const {
    return L1HelloMultiplier;
}

int ISISMain::getL2HelloInterval() const {
    return L2HelloInterval;
}

short ISISMain::getL2HelloMultiplier() const {
    return L2HelloMultiplier;
}

int ISISMain::getL1CsnpInterval() const {
    return L1CSNPInterval;
}

int ISISMain::getL1PsnpInterval() const {
    return L1PSNPInterval;
}

int ISISMain::getL2CsnpInterval() const {
    return L2CSNPInterval;
}

int ISISMain::getL2PsnpInterval() const {
    return L2PSNPInterval;
}

void ISISMain::setL1CsnpInterval(int l1CsnpInterval) {
    L1CSNPInterval = l1CsnpInterval;
}

void ISISMain::setL1PsnpInterval(int l1PsnpInterval) {
    L1PSNPInterval = l1PsnpInterval;
}

void ISISMain::setL2CsnpInterval(int l2CsnpInterval) {
    L2CSNPInterval = l2CsnpInterval;
}

ISISMain::ISIS_MODE ISISMain::getMode() const {
    return mode;
}

void ISISMain::setL2PsnpInterval(int l2PsnpInterval) {
    L2PSNPInterval = l2PsnpInterval;
}

int ISISMain::getLspInterval() const {
    return lspInterval;
}

short ISISMain::getIsType() const {
    return isType;
}

SystemID ISISMain::getSystemId() const {
    return systemId;
}

void ISISMain::setLspRefreshInterval(int lspRefreshInterval) {
    this->lspRefreshInterval = lspRefreshInterval;
}

void ISISMain::setIsType(short isType) {
    this->isType = isType;
}

AreaId ISISMain::getAreaId() const {
    return areaID;
}

void ISISMain::setAreaId(AreaId areaId) {
    areaID = areaId;
}

void ISISMain::setSystemId(const SystemID& systemId) {
    this->systemId = systemId;
}

TRILLNickname ISISMain::getNickname() const {
    return nickname;
}

void ISISMain::setAtt(bool att) {
    this->att = att;
}

unsigned int ISISMain::getISISIftSize(void) {
    return this->ISISIft.size();
}

/**
 * Generate NET address based on first non-zero MAC address it could find in
 * interface table. In case there's not any non-zero MAC address, one is generated.
 * This method is for ISIS::L2_ISIS_MODE
 */
void ISISMain::generateNetAddr() {

////    unsigned char *a = new unsigned char[6];
//    char *tmp = new char[25];
//    MacAddress address;
//
//    for (int i = 0; i < ift->getNumInterfaces(); i++)
//    {
//        if ((address = ift->getInterface(i)->getMacAddress()).getInt() != 0)
//        {
//            break;
//        }
//
//    }
//    /* If there's not any interface with non-zero MAC address then generate one.
//     * This is not likely to happen.*/
//    if (address.getInt() == 0)
//    {
//        std::cout << "Warning: didn't get non-zero MAC address for NET generating" << endl;
//        address.generateAutoAddress();
//    }
////
////    this->areaId = new unsigned char[3];
////    this->sysId = new unsigned char[6];
////    this->NSEL = new unsigned char[1];
//
//    this->netAddr = std::string(tmp);
//    std::stringstream addressStream;
//    addressStream << std::hex << address;
//    std::string aS = addressStream.str();
//
////    this->areaId[0] = (unsigned char)atoi("49");
////    this->areaId[1] = (unsigned char)atoi("00");
////    this->areaId[2] = (unsigned char)atoi("01");
//
////    address.getAddressBytes(this->sysId);
//
////    this->NSEL[0] = (unsigned char)atoi("00");
////    this->sysId = string(aS.substr(0,2) + aS.substr(3,2) + aS.substr(0,2) + aS.substr(3,2)  ).c_str();
//    this->netAddr = "00.0000." + aS.substr(0, 2) + aS.substr(3, 2) + "." + aS.substr(6, 2) + aS.substr(9, 2) + "."
//            + aS.substr(12, 2) + aS.substr(15, 2) + ".00";
//
//    if (!this->parseNetAddr())
//    {
//        throw cRuntimeError("Unable to parse auto-generated NET address.");
//    }

}

}        //end namespace inet

