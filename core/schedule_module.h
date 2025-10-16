#ifndef SCHEDULE_MODULE_H
#define SCHEDULE_MODULE_H

#include <cstdint>
#include <ctime>
#include <cstring>

/**
 * Station structure
 */
struct Station {
    char name[32];
    uint8_t ledIndex;
    float distanceFromStart;  // Kilometers
};

/**
 * Train Schedule structure
 */
struct TrainSchedule {
    uint16_t firstTrainMinutes;   // Minutes since midnight
    uint16_t lastTrainMinutes;
    uint8_t headwayMinutes;       // Time between trains
    bool isWeekend;
};

/**
 * Inter-station segment structure
 */
struct InterStationSegment {
    uint8_t stationA;
    uint8_t stationB;
    uint16_t travelTimeSeconds;
};

/**
 * Schedule Module
 * Stores and provides access to Link Light Rail schedule data
 */
class ScheduleModule {
public:
    ScheduleModule();

    /**
     * Load schedule data
     */
    void loadSchedule();

    /**
     * Get station by index
     * @param index Station index
     * @return Pointer to station
     */
    const Station* getStation(uint8_t index);

    /**
     * Get total number of stations
     * @return Station count
     */
    uint8_t getStationCount();

    /**
     * Get travel time between two stations
     * @param fromStation Starting station index
     * @param toStation Ending station index
     * @return Travel time in seconds
     */
    uint16_t getTravelTime(uint8_t fromStation, uint8_t toStation);

    /**
     * Get current schedule based on time
     * @param currentTime Current time
     * @return Pointer to schedule
     */
    const TrainSchedule* getCurrentSchedule(time_t currentTime);

    /**
     * Convert time to minutes since midnight
     * @param currentTime Current time
     * @return Minutes since midnight
     */
    uint16_t getCurrentMinuteOfDay(time_t currentTime);

    /**
     * Check if currently in service hours
     * @param minuteOfDay Minutes since midnight
     * @return true if in service, false otherwise
     */
    bool isServiceHours(uint16_t minuteOfDay);

private:
    Station stations_[23];  // Static allocation for 23 stations (Lynnwood City Center to Angle Lake)
    uint8_t stationCount_;
};

#endif // SCHEDULE_MODULE_H
