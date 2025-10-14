#ifndef POSITION_ENGINE_H
#define POSITION_ENGINE_H

#include <Arduino.h>
#include <time.h>
#include "schedule_module.h"

/**
 * Train structure
 */
struct Train {
    uint8_t id;
    bool isNorthbound;
    uint8_t currentStation;
    uint8_t nextStation;
    float progress;           // 0.0 to 1.0 between stations
    time_t departureTime;
    bool isActive;
};

/**
 * Train Position structure
 */
struct TrainPosition {
    uint8_t ledIndex;
    bool isNorthbound;
    bool isActive;
};

/**
 * Position Engine
 * Calculates real-time position of all active trains
 */
class PositionEngine {
public:
    PositionEngine();

    /**
     * Initialize position engine
     * @param scheduleModule Pointer to schedule module
     */
    void init(ScheduleModule* scheduleModule);

    /**
     * Update all train positions
     * @param currentTime Current time
     */
    void updateAllTrains(time_t currentTime);

    /**
     * Calculate individual train position
     * @param train Pointer to train
     * @param currentTime Current time
     */
    void calculateTrainPosition(Train* train, time_t currentTime);

    /**
     * Map position to LED index
     * @param position Position value
     * @return LED index
     */
    uint8_t mapPositionToLED(float position);

    /**
     * Get active train positions
     * @param count Output parameter for number of active trains
     * @return Pointer to train positions array
     */
    const TrainPosition* getActiveTrainPositions(uint8_t* count);

    /**
     * Spawn new trains based on schedule
     * @param currentTime Current time
     */
    void spawnNewTrains(time_t currentTime);

    /**
     * Remove completed trains
     */
    void removeCompletedTrains();

private:
    ScheduleModule* scheduleModule_;
    Train trains_[20];  // Static allocation for max 20 trains
    TrainPosition trainPositions_[20];
    uint8_t activeTrainCount_;
};

#endif // POSITION_ENGINE_H
