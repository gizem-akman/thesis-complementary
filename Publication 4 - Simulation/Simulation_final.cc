#include "ns3/core-module.h"
#include "ns3/mobility-module.h"
#include "ns3/animation-interface.h"
#include <vector>
#include <tuple>
#include <iostream>
#include <algorithm>
#include <random>

#include "ns3/internet-module.h"
#include "ns3/config-store-module.h"

using namespace ns3;

AnimationInterface* anim = nullptr;
int count1[6] = {0, 0,0,0,0,0};
int count2[30] = {0, 0,0,0,0,0,0, 0,0,0,0,0,0, 0,0,0,0,0,0, 0,0,0,0,0,0, 0,0,0,0,0};


void CalculateAndSetCentroid(NodeContainer& users, NodeContainer& centroidNode, uint32_t startIndex, uint32_t endIndex, uint32_t centroidIndex) {
    double xSum = 0.0;
    double ySum = 0.0;
    double zSum = 0.0;

    int j=0;
    for (uint32_t i = startIndex; i < endIndex; i++) {
        Ptr<Node> node = users.Get(i);
        Ptr<MobilityModel> mobility = node->GetObject<MobilityModel>();
        Vector pos = mobility->GetPosition();
        xSum += pos.x;
        ySum += pos.y;
        zSum += pos.z;
        j++;
    }

    std::cout<<"j is "<<j<<std::endl;

    double centroidX = xSum / (5);
    double centroidY = ySum / (5);
    double centroidZ = 0;

    Ptr<MobilityModel> centroidMob = centroidNode.Get(centroidIndex)->GetObject<MobilityModel>();
    centroidMob->SetPosition(Vector(centroidX, centroidY, centroidZ));
}

void Update(NodeContainer& car, NodeContainer& user, NodeContainer& centroidNode) {
    for (uint32_t i = 0; i < car.GetN(); i++) {
        Ptr<ConstantVelocityMobilityModel> mob = car.Get(i)->GetObject<ConstantVelocityMobilityModel>();
        Ptr<MobilityModel> centroidMob = centroidNode.Get(i)->GetObject<MobilityModel>();
        Vector centroidPos = centroidMob->GetPosition();
        Vector carPos = mob->GetPosition();
        // if (i==1)
        // {
        //     std::cout<<carPos.x<<" "<<carPos.y<<" "<<centroidPos.x<<" "<<centroidPos.y<<std::endl;
        // }
        if ((abs(carPos.x-centroidPos.x)<0.01) && (abs(carPos.y-centroidPos.y)<0.01))
        {
            if (count1[i] == 0)
            {
                std::cout<<"Car "<<i+1<<" has reached the centroid node at "<<Simulator::Now().GetSeconds()<<std::endl;
                Vector velocity = Vector(0.0, 0.0, 0.0); // Speed of car is 15 m/s
                mob->SetVelocity(velocity);
                count1[i] = 1;
            }
        }
    }


    for (uint32_t i = 0; i < user.GetN(); i++) {
        Ptr<ConstantVelocityMobilityModel> mob = user.Get(i)->GetObject<ConstantVelocityMobilityModel>();
        Ptr<MobilityModel> centroidMob = centroidNode.Get(i / 5)->GetObject<MobilityModel>(); // Each car has 5 users
        Vector centroidPos = centroidMob->GetPosition();
        Vector userPos = mob->GetPosition();
        if ((abs(userPos.x-centroidPos.x)<0.01) && (abs(userPos.y-centroidPos.y)<0.01))
        {
            if (count2[i] == 0)
            {
                std::cout<<"User "<<i+1<<" has reached the centroid node at "<<Simulator::Now().GetSeconds()<<std::endl;
                Vector velocity = Vector(0.0, 0.0, 0.0); // Speed of car is 15 m/s
                mob->SetVelocity(velocity);
                count2[i] = 1;
            }
        }
    }
}


int main(int argc, char* argv[]) {
    int carNum = 6;
    int userNum = 5 * carNum;
    int setseed = 5;

    RngSeedManager::SetSeed(setseed);

    CommandLine cmd(__FILE__);
    cmd.Parse(argc, argv);

    NodeContainer car;
    car.Create(carNum);

    // Create the centroid nodes
    NodeContainer centroidNode;
    centroidNode.Create(carNum);

    NodeContainer user;
    user.Create(userNum);

    Ptr<UniformRandomVariable> rand = CreateObject<UniformRandomVariable>();

    // Set up mobility for users
    MobilityHelper userMobility;
    userMobility.SetMobilityModel("ns3::ConstantVelocityMobilityModel");
    userMobility.Install(user);

    for (uint32_t i = 0; i < user.GetN(); i++) {
        double x = rand->GetValue(0, 100);
        double y = rand->GetValue(0, 100);
        Ptr<MobilityModel> mob = user.Get(i)->GetObject<MobilityModel>();
        mob->SetPosition(Vector(x, y, 0.0));
    }

    // Set up mobility for cars
    MobilityHelper carMobility;
    carMobility.SetMobilityModel("ns3::ConstantVelocityMobilityModel");
    carMobility.Install(car);

    double x = 0.0, y = 0.0;
    double a = 100.0, b = 100.0;
    for (uint32_t i = 0; i < car.GetN(); i++) {
        Ptr<MobilityModel> mob = car.Get(i)->GetObject<MobilityModel>();
        if (i < 3) {
            x += 6;
            y = 0;
            mob->SetPosition(Vector(x, y, 0.0));
        } else {
            a += 6;
            mob->SetPosition(Vector(a, b, 0.0));
        }
    }


    // Install mobility model for centroid nodes
    MobilityHelper mobility;
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(centroidNode);

    // Calculate and set positions for centroid nodes
    CalculateAndSetCentroid(user, centroidNode, 0, 5, 0);
    CalculateAndSetCentroid(user, centroidNode, 5, 10, 1);
    CalculateAndSetCentroid(user, centroidNode, 10, 15, 2);
    CalculateAndSetCentroid(user, centroidNode, 15, 20, 3);
    CalculateAndSetCentroid(user, centroidNode, 20, 25, 4);
    CalculateAndSetCentroid(user, centroidNode, 25, 30, 5);

    for (uint32_t i = 0; i < car.GetN(); i++) {
        Ptr<ConstantVelocityMobilityModel> mob = car.Get(i)->GetObject<ConstantVelocityMobilityModel>();
        Ptr<MobilityModel> centroidMob = centroidNode.Get(i)->GetObject<MobilityModel>();
        Vector centroidPos = centroidMob->GetPosition();
        Vector carPos = mob->GetPosition();

        Vector direction = centroidPos - carPos;
        // std::cout<<direction.x<<" "<<direction.y<<std::endl;
        double distance = std::sqrt(direction.x * direction.x + direction.y * direction.y);
        Vector unitDirection = Vector(direction.x / distance, direction.y / distance, 0.0);
        Vector velocity = Vector(unitDirection.x * 15.0, unitDirection.y * 15.0, 0.0); // Speed of car is 15 m/s
        mob->SetVelocity(velocity);
    }

    for (uint32_t i = 0; i < user.GetN(); i++) {
        Ptr<ConstantVelocityMobilityModel> mob = user.Get(i)->GetObject<ConstantVelocityMobilityModel>();
        Ptr<MobilityModel> centroidMob = centroidNode.Get(i / 5)->GetObject<MobilityModel>(); // Each car has 5 users
        Vector centroidPos = centroidMob->GetPosition();
        Vector userPos = mob->GetPosition();

        Vector direction = centroidPos - userPos;
        double distance = std::sqrt(direction.x * direction.x + direction.y * direction.y);
        Vector unitDirection = Vector(direction.x / distance, direction.y / distance, 0.0);
        Vector velocity = Vector(unitDirection.x * 2.0, unitDirection.y * 2.0, 0.0); // Speed of UE is 2 m/s
        mob->SetVelocity(velocity);
    }

    AsciiTraceHelper asciiTraceHelper;
    Ptr<OutputStreamWrapper> stream = asciiTraceHelper.CreateFileStream ("simulation-new.txt");
    *stream->GetStream() << "Object\tX-coordinate\tY-coordinate\tVelocity-X-axis\tVelocity-Y-axis\n";

    // For printing X and Y coordinates of all nodes
    std::cout<<"Getting the X and Y coordinates of all the centroid nodes"<<std::endl;
    for (uint32_t i=0; i<centroidNode.GetN() ; i++)
    {
        Ptr<MobilityModel> mob = centroidNode.Get(i)->GetObject<MobilityModel>();
        Vector centroidPos = mob->GetPosition();
        std::cout<<"Centroid Node "<<i+1<<" has "<<"X-coordinate:"<<centroidPos.x<<"and Y-coordinate:"<<centroidPos.y<<std::endl;
        *stream->GetStream() << "Centroid Node "<<i+1<<"\t"<<centroidPos.x<<"\t"<<centroidPos.y<<"\t"<<mob->GetVelocity().x<<"\t"<<mob->GetVelocity().y<<"\n";
    }

    for (uint32_t i=0;i<car.GetN();i++)
    {
        Ptr<ConstantVelocityMobilityModel> mob = car.Get(i)->GetObject<ConstantVelocityMobilityModel>();
        Vector carPos = mob->GetPosition();
        std::cout<<"Car "<<i+1<<" has "<<"X-coordinate:"<<carPos.x<<"and Y-coordinate:"<<carPos.y<<std::endl;
        *stream->GetStream() << "Car "<<i+1<<"\t"<<carPos.x<<"\t"<<carPos.y<<"\t"<<mob->GetVelocity().x<<"\t"<<mob->GetVelocity().y<<"\n";
    }

    for (uint32_t i=0;i<user.GetN();i++)
    {
        Ptr<ConstantVelocityMobilityModel> mob = user.Get(i)->GetObject<ConstantVelocityMobilityModel>();
        Vector userPos = mob->GetPosition();
        std::cout<<"User "<<i+1<<" has "<<"X-coordinate:"<<userPos.x<<"and Y-coordinate:"<<userPos.y<<std::endl;
        *stream->GetStream() << "User "<<i+1<<"\t"<<userPos.x<<"\t"<<userPos.y<<"\t"<<mob->GetVelocity().x<<"\t"<<mob->GetVelocity().y<<"\n";
    }


    anim = new AnimationInterface("part1.xml");

    for (uint32_t i = 0; i < car.GetN(); i++) {
        anim->UpdateNodeColor(car.Get(i), 255, 0, 0);
        anim->UpdateNodeSize(car.Get(i), 5.0, 5.0);
        anim->UpdateNodeDescription(car.Get(i), std::to_string(i+1));
    }

    // for (uint32_t i = 0; i < user.GetN(); i++) {
    //     anim->UpdateNodeColor(user.Get(i), 0, 255, 0);
    //     anim->UpdateNodeSize(user.Get(i), 2.0, 2.0);
    // }

    // for (uint32_t i = 0; i < centroidNode.GetN(); i++) {
    //     anim->UpdateNodeColor(centroidNode.Get(i), 0, 0, 255);
    //     anim->UpdateNodeSize(centroidNode.Get(i), 3.0, 3.0);
    // }

    for (uint32_t i = 0; i < user.GetN(); i++) {
        if (i>24) {
            anim->UpdateNodeColor(user.Get(i), 122, 0, 0);
        }
        else if (i>19) {
            anim->UpdateNodeColor(user.Get(i), 255, 255, 0);
        }
        else if (i>14) {
            anim->UpdateNodeColor(user.Get(i), 0, 255, 0);
        }
        else if (i>9) {
            anim->UpdateNodeColor(user.Get(i), 0, 255, 255);
        }
        else if (i>4) {
            anim->UpdateNodeColor(user.Get(i), 0, 0, 255);
        }
        else {
            anim->UpdateNodeColor(user.Get(i), 255, 0, 255);
        }
        anim->UpdateNodeSize(user.Get(i), 2.0, 2.0);
        anim->UpdateNodeDescription(user.Get(i), std::to_string(i+1));
    }

    for (uint32_t i = 0; i < centroidNode.GetN(); i++) {
        if (i==5) {
            anim->UpdateNodeColor(centroidNode.Get(i), 122, 0, 0);
        }
        else if (i==4) {
            anim->UpdateNodeColor(centroidNode.Get(i), 255, 255, 0);
        }
        else if (i==3) {
            anim->UpdateNodeColor(centroidNode.Get(i), 0, 255, 0);
        }
        else if (i==2) {
            anim->UpdateNodeColor(centroidNode.Get(i), 0, 255, 255);
        }
        else if (i==1) {
            anim->UpdateNodeColor(centroidNode.Get(i), 0, 0, 255);
        }
        else {
            anim->UpdateNodeColor(centroidNode.Get(i), 255, 0, 255);
        }
        anim->UpdateNodeSize(centroidNode.Get(i), 4.0, 4.0);
        anim->UpdateNodeDescription(centroidNode.Get(i), std::to_string(i+1));
    }

    Simulator::Stop(Seconds(35.0));
    for (double i = 0; i < 35.0; i+=0.0001)
    {   
        Simulator::Schedule(Seconds(i), &Update, car, user, centroidNode);  
    }
    Simulator::Run();
    Simulator::Destroy();

    return 0;
}
