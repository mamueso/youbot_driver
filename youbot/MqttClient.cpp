#include "MqttClient.hpp"
#include <iostream>
#include <chrono>
#include "generic-joint/JointData.hpp"
#include "one-dof-gripper/OneDOFGripperData.hpp"

#define MQTT_BROKER_ADDRESS     "localhost:1883"
#define MQTT_DEFAULT_CLIENTID   "YoubotPublisher"
#define MQTT_DEFAULT_QOS         1

MqttClient::MqttClient() : running(false), connected(false) {
    // init connection

    MQTTAsync_connectOptions conn_opts = MQTTAsync_connectOptions_initializer;
    MQTTAsync_create(&client, MQTT_BROKER_ADDRESS, MQTT_DEFAULT_CLIENTID, MQTTCLIENT_PERSISTENCE_NONE, nullptr);

    conn_opts.onSuccess = &MqttClient::onConnect;
    conn_opts.onFailure = &MqttClient::onConnectFailure;
    conn_opts.context = this;

    int rc;
    if ((rc = MQTTAsync_connect(client, &conn_opts)) != MQTTASYNC_SUCCESS) {
        std::cout << "MQTT connection error: " << rc << std::endl;
    }
}

MqttClient& MqttClient::getInstance() {
    static MqttClient instance;
    return instance;
}

void MqttClient::start() {
    std::lock_guard<std::mutex> lock(mutex);
    if (!running) {
        running = true;
        worker = std::thread([this]() {
            while (running) {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                onTick();
            }
        });
    }
}

void MqttClient::stop() {
    std::lock_guard<std::mutex> lock(mutex);
    running = false;
    if (worker.joinable()) {
        worker.join();
    }
}

MqttClient::~MqttClient() {
    stop();
    if (connected) {
        // reset values
        publishValue("", "youbot/application", client);
        publishValue("false", "youbot/base/available", client);
        publishValue("0", "youbot/manipulator/number", client);
    }
}


void MqttClient::onTick() {
    // publish cyclic values

    if (connected) {
        if (youbotBase != nullptr) {

            publishValue("true", "youbot/base/available", client);

            quantity<si::velocity> v_long;
            quantity<si::velocity> v_trans;
            quantity<si::angular_velocity> v_angular;

            youbotBase->getBaseVelocity(v_long, v_trans, v_angular);

            publishValue(toStr(v_long.value()), "youbot/base/velocity/longitudinal", client);
            publishValue(toStr(v_trans.value()), "youbot/base/velocity/transversal", client);

            
            publishValue(toStr(v_angular.value() * (180.0 / M_PI)), "youbot/base/velocity/angular", client);
        } else {
            publishValue("false", "youbot/base/available", client);
        }
        
        publishValue(std::to_string(youBotManipulators.size()), "youbot/manipulator/number", client);


        for (size_t i = 0; i < youBotManipulators.size(); ++i) {
            std::string baseTopic = "youbot/manipulator/" + std::to_string(i) + "/";

            youbot::YouBotManipulator* manipulator = youBotManipulators[i];
            if (manipulator) {
                
                std::vector<youbot::JointSensedAngle> jointData;
                manipulator->getJointData(jointData);

                publishValue(std::to_string(jointData.size()), baseTopic + "joint/number" , client);

                // alternatively we could also publish all values together as json ...

                for (size_t j = 0; j < jointData.size(); ++j) {
                    // const youbot::JointSensedAngle& joint = jointData[j];
                    publishValue(std::to_string(jointData[j].angle.value() * (180.0 / M_PI) ), baseTopic + "joint/" + std::to_string(j) + "/angle" , client);
                }

                youbot::YouBotGripper& gripper = manipulator->getArmGripper();

                youbot::GripperSensedBarPosition barPosition;
                gripper.getGripperBar1().getData(barPosition);
                publishValue(toStr(barPosition.barPosition.value()), baseTopic + "gripper/bar/0/position", client);
                gripper.getGripperBar2().getData(barPosition);
                publishValue(toStr(barPosition.barPosition.value()), baseTopic + "gripper/bar/1/position", client);
            }
        }
    }
}


std::string toStr(double value, int precision) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(precision) << value;
    return oss.str();
}


void MqttClient::onSendSuccess(void* context, MQTTAsync_successData* response) {
    // todo: add if (debug)
    // std::cout << " - OK" << std::endl;
}

void MqttClient::onSendFailure(void* context, MQTTAsync_failureData* response) {
    // std::cerr << " - Error!" << std::endl;
}

void MqttClient::onConnect(void* context, MQTTAsync_successData* response) {
    std::cout << "Publisher MQTT-Connection OK" << std::endl;

    MqttClient* self = static_cast<MqttClient*>(context);
    self->connected = true;    

    self->publishValue(getExecutableName(), "youbot/application", self->client);

    self->start();
}

void MqttClient::onConnectFailure(void* context, MQTTAsync_failureData* response) {
    if (response) {
        std::cerr << "Publisher MQTT connect failed. Code: " << response->code;
        if (response->message) {
            std::cerr << ", Message: " << response->message;
        }
        std::cerr << std::endl;
    } else {
        std::cerr << "Publisher MQTT connect failed." << std::endl;        
    }

    // do nothing
}

void MqttClient::publishValue(std::string value, std::string topic, MQTTAsync client)
{
    std::string payload = value;
    MQTTAsync_message msg = MQTTAsync_message_initializer;
    msg.payload = (void *)payload.c_str();
    msg.payloadlen = payload.length();
    msg.qos = MQTT_DEFAULT_QOS;
    msg.retained = 1;

    MQTTAsync_responseOptions opts = MQTTAsync_responseOptions_initializer;
    opts.onSuccess = onSendSuccess;
    opts.onFailure = onSendFailure;
    opts.context = this;

    MQTTAsync_sendMessage(client, topic.c_str(), &msg, &opts);
}

std::string getExecutableName() {
    char path[PATH_MAX];
    ssize_t count = readlink("/proc/self/exe", path, PATH_MAX);
    if (count != -1) {
        path[count] = '\0';  // Null-terminate the path
        return std::string(basename(path));
    } else {
        return "";  // Return empty string on failure
    }
}


/*
#include <iostream>
#include <thread>
#include <chrono>
#include <unistd.h>
#include <limits.h>
#include <libgen.h>
#include <MQTTAsync.h>
#include "MqttClient.hpp"

#define ADDRESS     "localhost:1883"
#define CLIENTID    "SimplePublisher"
#define TOPIC       "youbot/toggle"
#define QOS         1

volatile int connected = 0;
volatile int finished = 0;

bool kbhit() {
    timeval tv = {0, 0};
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(STDIN_FILENO, &readfds);
    return select(STDIN_FILENO + 1, &readfds, nullptr, nullptr, &tv) > 0;
}

std::string getExecutableName() {
    char path[PATH_MAX];
    ssize_t count = readlink("/proc/self/exe", path, PATH_MAX);
    if (count != -1) {
        path[count] = '\0';  // Null-terminate the path
        return std::string(basename(path));
    } else {
        return "";  // Return empty string on failure
    }
}


void onSendSuccess(void* context, MQTTAsync_successData* response) {
    std::cout << " - OK" << std::endl;
}

void onSendFailure(void* context, MQTTAsync_failureData* response) {
    std::cerr << " - Fehler!" << std::endl;
}

void onConnect(void* context, MQTTAsync_successData* response) {
    std::cout << "Verbindung OK" << std::endl;
    connected = 1;

    finished = 1;
}

void onConnectFailure(void* context, MQTTAsync_failureData* response) {
    std::cout << "Verbindung fehlgeschlagen!" << std::endl;
    finished = 1;
}

int main() {
    MQTTAsync client;
    MQTTAsync_connectOptions conn_opts = MQTTAsync_connectOptions_initializer;
    MQTTAsync_create(&client, ADDRESS, CLIENTID, MQTTCLIENT_PERSISTENCE_NONE, nullptr);

    conn_opts.onSuccess = onConnect;
    conn_opts.onFailure = onConnectFailure;
    conn_opts.context = client;

    int rc;
    if ((rc = MQTTAsync_connect(client, &conn_opts)) != MQTTASYNC_SUCCESS) {
        printf("Fehler beim Starten der Verbindung: %d\n", rc);
        return -1;
    }

    // Warten bis Callback ausgelöst wird
    while (!finished) {
        sleep(1); // einfache Wartezeit
    }

    if (connected) {

        std::cout << "Drücke ENTER zum Beenden...\n";
        std::cin.clear();
        std::cin.sync();
        
        MQTTAsync_responseOptions opts = MQTTAsync_responseOptions_initializer;
        opts.onSuccess = onSendSuccess;
        opts.onFailure = onSendFailure;
        opts.context = client;

        publishValue(getExecutableName(), "youbot/application", client, opts);

        int value = 0;
        while (true) {
            // Nachricht senden
            publishValue(std::to_string(value), TOPIC, client, opts);

            value = 1- value;

            // 1 Sekunde warten
            std::this_thread::sleep_for(std::chrono::seconds(1));

            // Prüfen, ob eine Taste gedrückt wurde
            if (kbhit()) {
                char c;
                std::cin.get(c); // Taste wird gelesen und damit "konsumiert"

                std::cout << "Beendet.\n";
                break;
            }
        }
    }

    MQTTAsync_disconnect(client, nullptr);
    MQTTAsync_destroy(&client);

    return 0;
}

void publishValue(std::string value, std::string topic, MQTTAsync client, MQTTAsync_responseOptions &opts)
{
    std::string payload = value;
    MQTTAsync_message msg = MQTTAsync_message_initializer;
    msg.payload = (void *)payload.c_str();
    msg.payloadlen = payload.length();
    msg.qos = QOS;
    msg.retained = 1;

    // Response-Optionen mit Callback

    MQTTAsync_sendMessage(client, topic.c_str(), &msg, &opts);
    std::cout << "wird gesendet: " << value;
}
*/