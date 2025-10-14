#include "schedule_module.h"

ScheduleModule::ScheduleModule()
    : stationCount_(0) {
}

void ScheduleModule::loadSchedule() {
    // TODO: Implement schedule loading
    // Will populate stations_ array with station data
    Serial.println("[ScheduleModule] loadSchedule() - stub");

    // Stub: Set station count for now
    stationCount_ = 18;
}

const Station* ScheduleModule::getStation(uint8_t index) {
    // TODO: Implement get station
    if (index >= stationCount_) {
        return nullptr;
    }
    return &stations_[index];
}

uint8_t ScheduleModule::getStationCount() {
    return stationCount_;
}

uint16_t ScheduleModule::getTravelTime(uint8_t fromStation, uint8_t toStation) {
    // TODO: Implement travel time calculation
    // For now return stub value (2 minutes average)
    return 120;
}

const TrainSchedule* ScheduleModule::getCurrentSchedule(time_t currentTime) {
    // TODO: Implement schedule selection based on time
    static TrainSchedule stubSchedule = {
        300,   // 5:00 AM
        1500,  // 1:00 AM next day
        10,    // 10 minute headway
        false  // weekday
    };
    return &stubSchedule;
}

uint16_t ScheduleModule::getCurrentMinuteOfDay(time_t currentTime) {
    struct tm* timeinfo = localtime(&currentTime);
    if (timeinfo == nullptr) {
        return 0;
    }
    return timeinfo->tm_hour * 60 + timeinfo->tm_min;
}

bool ScheduleModule::isServiceHours(uint16_t minuteOfDay) {
    // TODO: Implement service hours check with wrap-around logic
    // For now, simple stub
    if (minuteOfDay >= 60 && minuteOfDay < 300) {
        return false;  // Gap period: 1:00 AM - 5:00 AM
    }
    return true;
}
