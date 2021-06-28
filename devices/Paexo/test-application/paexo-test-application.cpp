/*
 * Copyright (C) 2021 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * GNU Lesser General Public License v2.1 or any later version.
 */

#include <stdio.h>
#include <memory>
#include <yarp/os/Network.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/LogStream.h>
#include <yarp/sig/Vector.h>

#include <thrift/WearableActuatorCommand.h>

#define MIN_MOTOR_POSITION 27
#define MAX_MOTOR_POSITION 55
#define MOTOR_STEP_SIZE 0.5 //Could go as low as 0.2
using namespace yarp::os;
using namespace wearable;
using YarpBufferedPort = yarp::os::BufferedPort<yarp::os::Bottle>;

int main() {

    yarp::os::Network::init();

    std::string inPort = "/Paexo/WearableActuatorsCommand/input:i";
    std::string outPort = "/Paexo/WearableActuatorsCommand/output:o";

    std::vector<std::string> PaexoActuators = {"Paexo::motor::LeftMotor", "Paexo::motor::RightMotor"};

    std::vector<std::string> PaexoActuatorPortNames = {"/Paexo/motor/LeftMotor"};
    std::vector<std::unique_ptr<YarpBufferedPort>> PaexoActuatorPorts;

//    if (!yarp::os::Network::exists(inPort))
//    {
//        yError() << "Port " << inPort << " does not exists";
//        return -1;
//    }

//    BufferedPort<wearable::msg::WearableActuatorCommand> port;
//    if(!port.open(outPort))
//    {
//        yError() << "Failed to open port " << outPort;
//        return -1;
//    }

//    if (!yarp::os::Network::connect(outPort,inPort))
//    {
//        yError() << "Failed to connect " << outPort << " to " << inPort;
//        return -1;
//    }

    // Yarp buffered ports for motor control
    for (const auto& portName : PaexoActuatorPortNames)
    {
        std::unique_ptr<YarpBufferedPort> port = std::make_unique<YarpBufferedPort>();

        if (!port->open(portName + ":o"))
        {
            yError() << "Failed to open port " << (portName + ":o");
            return -1;
        }

        if (!yarp::os::Network::connect(portName + ":o", portName + ":i"))
        {
            yError() << "Failed to connect ports " << (portName + ":o") << " and " << (portName + ":i");
            return -1;
        }

        PaexoActuatorPorts.push_back(std::move(port));
    }


    while (true) {

//        for (const auto& actuator : PaexoActuators)
//        {
//            wearable::msg::WearableActuatorCommand& wearableActuatorCommand = port.prepare();

//            // Add wearable actuator command
//            wearableActuatorCommand.info.name = actuator;
//            wearableActuatorCommand.info.type = wearable::msg::ActuatorType::MOTOR;
//            wearableActuatorCommand.info.status = wearable::msg::ActuatorStatus::OK;

//            wearableActuatorCommand.duration = 10;
//            wearableActuatorCommand.value = 40;

//            yInfo() << "Command " << wearableActuatorCommand.info.name << " to position "
//                    <<  wearableActuatorCommand.value << " deg";

//            // Send the actuator command to the output port
//            port.write(true);

//            Time::delay(2);
//        }

        for (const auto& port : PaexoActuatorPorts)
        {
            double current_motor_position = MIN_MOTOR_POSITION;

            while (current_motor_position < MAX_MOTOR_POSITION)
            {
                yarp::os::Bottle& cmd = port->prepare();

                cmd.clear();

                yInfo() << "Sending motor position " << current_motor_position;
                cmd.addDouble(current_motor_position);

                port->write(true);

                Time::delay(0.1);

                current_motor_position += MOTOR_STEP_SIZE;
            }
        }
    }

    return 0;
}
