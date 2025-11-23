#ifndef MQTTCLIENT_HPP
#define MQTTCLIENT_HPP

#include <thread>
#include <atomic>
#include <mutex>
#include <vector>
#include <memory>
#include <MQTTAsync.h>
#include "YouBotBase.hpp"
#include "YouBotManipulator.hpp"

class MqttClient {
public:
    static MqttClient& getInstance();

    void start();
    void stop();
    ~MqttClient();

    // Delete copy constructor and assignment operator
    MqttClient(const MqttClient&) = delete;
    MqttClient& operator=(const MqttClient&) = delete;

    void publishValue(std::string value, std::string topic, MQTTAsync client);

    static void onSendSuccess(void* context, MQTTAsync_successData* response);
    static void onSendFailure(void* context, MQTTAsync_failureData* response);

    static void onConnect(void* context, MQTTAsync_successData* response);
    static void onConnectFailure(void* context, MQTTAsync_failureData* response);

    void setYoubotBase(youbot::YouBotBase* base) {     
        youbotBase = base;
    }

    void setYoubotManipulator(std::shared_ptr<youbot::YouBotManipulator> manipulator) {
        std::lock_guard<std::mutex> lock(mutex);
        if (youBotManipulators.size() >= 2) {
            std::cout << "Only two manipulators supported!" << std::endl;
            return;
        }

        if (manipulator) {
            youBotManipulators.push_back(manipulator);
        }
    }


protected:
    virtual void onTick();

private:
    MqttClient();
    std::atomic<bool> running;
    std::atomic<bool> connected;

    std::thread worker;
    std::mutex mutex;

    MQTTAsync client;

    youbot::YouBotBase* youbotBase;
    std::vector<std::shared_ptr<youbot::YouBotManipulator>> youBotManipulators;

};

std::string getExecutableName();

std::string toStr(double value, int precision = 2);

#endif // MQTTCLIENT_HPP
