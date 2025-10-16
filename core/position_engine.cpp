#include "position_engine.h"
#include <iostream>

PositionEngine::PositionEngine()
    : scheduleModule_(nullptr),
      activeTrainCount_(0) {
}

void PositionEngine::init(ScheduleModule* scheduleModule) {
    scheduleModule_ = scheduleModule;

    // Initialize all trains as inactive
    for (uint8_t i = 0; i < 20; i++) {
        trains_[i].isActive = false;
    }

    std::cout << "[PositionEngine] Initialized" << std::endl;
}

void PositionEngine::updateAllTrains(time_t currentTime) {
    if (scheduleModule_ == nullptr) {
        return;
    }

    uint16_t minuteOfDay = scheduleModule_->getCurrentMinuteOfDay(currentTime);

    // Check if in service gap (01:00 - 05:00)
    if (!scheduleModule_->isServiceHours(minuteOfDay)) {
        // Remove all active trains during gap period
        for (uint8_t i = 0; i < 20; i++) {
            trains_[i].isActive = false;
        }
        activeTrainCount_ = 0;
        return;
    }

    // Update existing trains
    for (uint8_t i = 0; i < 20; i++) {
        if (trains_[i].isActive) {
            calculateTrainPosition(&trains_[i], currentTime);
        }
    }

    // Remove completed trains
    removeCompletedTrains();

    // Spawn new trains based on schedule
    spawnNewTrains(currentTime);

    // Build train positions array for display
    activeTrainCount_ = 0;
    for (uint8_t i = 0; i < 20; i++) {
        if (trains_[i].isActive) {
            // Get LED indices for current and next stations
            const Station* currentStation = scheduleModule_->getStation(trains_[i].currentStation);
            const Station* nextStation = scheduleModule_->getStation(trains_[i].nextStation);

            if (currentStation != nullptr && nextStation != nullptr) {
                // Interpolate LED position based on progress
                float currentLED = currentStation->ledIndex;
                float nextLED = nextStation->ledIndex;
                float interpolatedLED = currentLED + (nextLED - currentLED) * trains_[i].progress;

                // Round to nearest LED index
                uint8_t ledIndex = (uint8_t)(interpolatedLED + 0.5);

                // Clamp to valid range (0-99)
                if (ledIndex > 99) ledIndex = 99;

                trainPositions_[activeTrainCount_].ledIndex = ledIndex;
                trainPositions_[activeTrainCount_].isNorthbound = trains_[i].isNorthbound;
                trainPositions_[activeTrainCount_].isActive = true;
                activeTrainCount_++;
            }
        }
    }
}

void PositionEngine::calculateTrainPosition(Train* train, time_t currentTime) {
    if (train == nullptr || !train->isActive || scheduleModule_ == nullptr) {
        return;
    }

    // Calculate elapsed time since departure
    int32_t elapsedSeconds = currentTime - train->departureTime;
    if (elapsedSeconds < 0) {
        elapsedSeconds = 0;
    }

    uint8_t stationCount = scheduleModule_->getStationCount();

    // Determine total route time
    uint8_t startStation = train->isNorthbound ? 0 : (stationCount - 1);
    uint8_t endStation = train->isNorthbound ? (stationCount - 1) : 0;
    uint16_t totalRouteTime = scheduleModule_->getTravelTime(startStation, endStation);

    // Check if train has completed route
    if (elapsedSeconds >= totalRouteTime) {
        train->isActive = false;
        return;
    }

    // Find which segment the train is currently in
    uint16_t accumulatedTime = 0;
    uint8_t currentSeg = train->isNorthbound ? 0 : (stationCount - 1);

    while (currentSeg != endStation) {
        uint8_t nextSeg = train->isNorthbound ? (currentSeg + 1) : (currentSeg - 1);
        uint16_t segmentTime = scheduleModule_->getTravelTime(currentSeg, nextSeg);

        if (accumulatedTime + segmentTime > elapsedSeconds) {
            // Train is in this segment
            train->currentStation = currentSeg;
            train->nextStation = nextSeg;

            // Calculate progress through this segment (0.0 to 1.0)
            uint16_t timeInSegment = elapsedSeconds - accumulatedTime;
            train->progress = (float)timeInSegment / (float)segmentTime;

            // Clamp progress
            if (train->progress < 0.0) train->progress = 0.0;
            if (train->progress > 1.0) train->progress = 1.0;

            break;
        }

        accumulatedTime += segmentTime;
        currentSeg = nextSeg;
    }
}

uint8_t PositionEngine::mapPositionToLED(float position) {
    // Position parameter is not used in this implementation
    // Instead, we use the train's currentStation, nextStation, and progress
    // This function is kept for API compatibility but mapping is done inline
    return 0;
}

const TrainPosition* PositionEngine::getActiveTrainPositions(uint8_t* count) {
    *count = activeTrainCount_;
    return trainPositions_;
}

void PositionEngine::spawnNewTrains(time_t currentTime) {
    if (scheduleModule_ == nullptr) {
        return;
    }

    // Get current schedule
    const TrainSchedule* schedule = scheduleModule_->getCurrentSchedule(currentTime);
    if (schedule == nullptr) {
        return;
    }

    uint16_t minuteOfDay = scheduleModule_->getCurrentMinuteOfDay(currentTime);
    uint8_t stationCount = scheduleModule_->getStationCount();

    // Calculate total route time to know how far back to check for active trains
    uint16_t totalRouteTime = scheduleModule_->getTravelTime(0, stationCount - 1);
    uint16_t routeTimeMinutes = (totalRouteTime + 59) / 60;  // Round up to minutes

    // For each potential train departure
    // We'll check if a train should be spawned or is already in transit

    // Northbound trains (start at station 0)
    // Spawn all trains that should currently be in transit
    if (minuteOfDay >= schedule->firstTrainMinutes) {
        // Check trains that departed in the last [route time] to see if they're still in transit
        for (uint16_t checkMinute = schedule->firstTrainMinutes; checkMinute <= minuteOfDay && checkMinute <= schedule->lastTrainMinutes; checkMinute += schedule->headwayMinutes) {
            // Calculate how long ago this train departed
            uint16_t minutesSinceDeparture = minuteOfDay - checkMinute;

            // Skip if train has completed route
            if (minutesSinceDeparture * 60 >= totalRouteTime) {
                continue;
            }

            // Calculate proper departure time
            struct tm* timeinfo = localtime(&currentTime);
            if (timeinfo != nullptr) {
                timeinfo->tm_hour = checkMinute / 60;
                timeinfo->tm_min = checkMinute % 60;
                timeinfo->tm_sec = 0;
                time_t thisDepartureTime = mktime(timeinfo);

                // Check if we already have this train
                bool alreadyExists = false;
                for (uint8_t j = 0; j < 20; j++) {
                    if (trains_[j].isActive &&
                        trains_[j].isNorthbound &&
                        trains_[j].departureTime == thisDepartureTime) {
                        alreadyExists = true;
                        break;
                    }
                }

                if (!alreadyExists) {
                    // Find an inactive train slot
                    for (uint8_t i = 0; i < 20; i++) {
                        if (!trains_[i].isActive) {
                            trains_[i].id = i;
                            trains_[i].isNorthbound = true;
                            trains_[i].currentStation = 0;
                            trains_[i].nextStation = 1;
                            trains_[i].progress = 0.0;
                            trains_[i].departureTime = thisDepartureTime;
                            trains_[i].isActive = true;
                            std::cout << "[PositionEngine] Spawned northbound train ID " << (int)i
                                      << " departing at minute " << checkMinute << std::endl;
                            break;
                        }
                    }
                }
            }
        }
    }

    // Southbound trains (start at last station)
    // Add 15 minute offset for southbound trains (typical stagger)
    uint16_t southboundFirstTrain = schedule->firstTrainMinutes + 15;
    if (minuteOfDay >= southboundFirstTrain) {
        // Check trains that departed in the last [route time] to see if they're still in transit
        for (uint16_t checkMinute = southboundFirstTrain; checkMinute <= minuteOfDay && checkMinute <= schedule->lastTrainMinutes; checkMinute += schedule->headwayMinutes) {
            // Calculate how long ago this train departed
            uint16_t minutesSinceDeparture = minuteOfDay - checkMinute;

            // Skip if train has completed route
            if (minutesSinceDeparture * 60 >= totalRouteTime) {
                continue;
            }

            // Calculate proper departure time
            struct tm* timeinfo = localtime(&currentTime);
            if (timeinfo != nullptr) {
                timeinfo->tm_hour = checkMinute / 60;
                timeinfo->tm_min = checkMinute % 60;
                timeinfo->tm_sec = 0;
                time_t thisDepartureTime = mktime(timeinfo);

                // Check if we already have this train
                bool alreadyExists = false;
                for (uint8_t j = 0; j < 20; j++) {
                    if (trains_[j].isActive &&
                        !trains_[j].isNorthbound &&
                        trains_[j].departureTime == thisDepartureTime) {
                        alreadyExists = true;
                        break;
                    }
                }

                if (!alreadyExists) {
                    // Find an inactive train slot
                    for (uint8_t i = 0; i < 20; i++) {
                        if (!trains_[i].isActive) {
                            trains_[i].id = i;
                            trains_[i].isNorthbound = false;
                            trains_[i].currentStation = stationCount - 1;
                            trains_[i].nextStation = stationCount - 2;
                            trains_[i].progress = 0.0;
                            trains_[i].departureTime = thisDepartureTime;
                            trains_[i].isActive = true;
                            std::cout << "[PositionEngine] Spawned southbound train ID " << (int)i
                                      << " departing at minute " << checkMinute << std::endl;
                            break;
                        }
                    }
                }
            }
        }
    }
}

void PositionEngine::removeCompletedTrains() {
    // Remove trains that are marked as inactive
    // Note: Trains are marked as inactive in calculateTrainPosition when they complete their route
    // This function is a placeholder for any additional cleanup logic
    // The trains array is already being filtered in updateAllTrains() when building trainPositions_
}
