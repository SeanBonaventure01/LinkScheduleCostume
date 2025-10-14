#include "schedule_module.h"

ScheduleModule::ScheduleModule()
    : stationCount_(0) {
}

void ScheduleModule::loadSchedule() {
    Serial.println("[ScheduleModule] Loading Link Light Rail 1 Line schedule...");

    // Total line distance: approximately 45 km from Lynnwood City Center to Angle Lake
    // 100 LEDs (0-99) representing the full line

    stationCount_ = 23;

    // Station 0: Lynnwood City Center
    strncpy(stations_[0].name, "Lynnwood City Center", 31);
    stations_[0].name[31] = '\0';
    stations_[0].ledIndex = 0;
    stations_[0].distanceFromStart = 0.0;

    // Station 1: Mountlake Terrace
    strncpy(stations_[1].name, "Mountlake Terrace", 31);
    stations_[1].name[31] = '\0';
    stations_[1].ledIndex = 7;
    stations_[1].distanceFromStart = 3.0;

    // Station 2: Shoreline North / 185th
    strncpy(stations_[2].name, "Shoreline North/185th", 31);
    stations_[2].name[31] = '\0';
    stations_[2].ledIndex = 13;
    stations_[2].distanceFromStart = 6.0;

    // Station 3: Shoreline South / 148th
    strncpy(stations_[3].name, "Shoreline South/148th", 31);
    stations_[3].name[31] = '\0';
    stations_[3].ledIndex = 18;
    stations_[3].distanceFromStart = 8.0;

    // Station 4: Northgate
    strncpy(stations_[4].name, "Northgate", 31);
    stations_[4].name[31] = '\0';
    stations_[4].ledIndex = 22;
    stations_[4].distanceFromStart = 10.0;

    // Station 5: Roosevelt
    strncpy(stations_[5].name, "Roosevelt", 31);
    stations_[5].name[31] = '\0';
    stations_[5].ledIndex = 28;
    stations_[5].distanceFromStart = 12.4;

    // Station 6: U District
    strncpy(stations_[6].name, "U District", 31);
    stations_[6].name[31] = '\0';
    stations_[6].ledIndex = 31;
    stations_[6].distanceFromStart = 13.8;

    // Station 7: University of Washington
    strncpy(stations_[7].name, "University of Washington", 31);
    stations_[7].name[31] = '\0';
    stations_[7].ledIndex = 34;
    stations_[7].distanceFromStart = 15.2;

    // Station 8: Capitol Hill
    strncpy(stations_[8].name, "Capitol Hill", 31);
    stations_[8].name[31] = '\0';
    stations_[8].ledIndex = 39;
    stations_[8].distanceFromStart = 17.5;

    // Station 9: Westlake
    strncpy(stations_[9].name, "Westlake", 31);
    stations_[9].name[31] = '\0';
    stations_[9].ledIndex = 44;
    stations_[9].distanceFromStart = 19.8;

    // Station 10: Symphony
    strncpy(stations_[10].name, "Symphony", 31);
    stations_[10].name[31] = '\0';
    stations_[10].ledIndex = 46;
    stations_[10].distanceFromStart = 20.5;

    // Station 11: Pioneer Square
    strncpy(stations_[11].name, "Pioneer Square", 31);
    stations_[11].name[31] = '\0';
    stations_[11].ledIndex = 47;
    stations_[11].distanceFromStart = 21.2;

    // Station 12: Int'l District / Chinatown
    strncpy(stations_[12].name, "Intl Dist/Chinatown", 31);
    stations_[12].name[31] = '\0';
    stations_[12].ledIndex = 49;
    stations_[12].distanceFromStart = 21.9;

    // Station 13: Stadium
    strncpy(stations_[13].name, "Stadium", 31);
    stations_[13].name[31] = '\0';
    stations_[13].ledIndex = 51;
    stations_[13].distanceFromStart = 23.0;

    // Station 14: SODO
    strncpy(stations_[14].name, "SODO", 31);
    stations_[14].name[31] = '\0';
    stations_[14].ledIndex = 55;
    stations_[14].distanceFromStart = 24.8;

    // Station 15: Beacon Hill
    strncpy(stations_[15].name, "Beacon Hill", 31);
    stations_[15].name[31] = '\0';
    stations_[15].ledIndex = 60;
    stations_[15].distanceFromStart = 26.9;

    // Station 16: Mount Baker
    strncpy(stations_[16].name, "Mount Baker", 31);
    stations_[16].name[31] = '\0';
    stations_[16].ledIndex = 65;
    stations_[16].distanceFromStart = 29.2;

    // Station 17: Columbia City
    strncpy(stations_[17].name, "Columbia City", 31);
    stations_[17].name[31] = '\0';
    stations_[17].ledIndex = 70;
    stations_[17].distanceFromStart = 31.5;

    // Station 18: Othello
    strncpy(stations_[18].name, "Othello", 31);
    stations_[18].name[31] = '\0';
    stations_[18].ledIndex = 75;
    stations_[18].distanceFromStart = 33.8;

    // Station 19: Rainier Beach
    strncpy(stations_[19].name, "Rainier Beach", 31);
    stations_[19].name[31] = '\0';
    stations_[19].ledIndex = 80;
    stations_[19].distanceFromStart = 36.1;

    // Station 20: Tukwila Int'l Blvd
    strncpy(stations_[20].name, "Tukwila Intl Blvd", 31);
    stations_[20].name[31] = '\0';
    stations_[20].ledIndex = 89;
    stations_[20].distanceFromStart = 40.0;

    // Station 21: SeaTac / Airport
    strncpy(stations_[21].name, "SeaTac/Airport", 31);
    stations_[21].name[31] = '\0';
    stations_[21].ledIndex = 96;
    stations_[21].distanceFromStart = 43.0;

    // Station 22: Angle Lake
    strncpy(stations_[22].name, "Angle Lake", 31);
    stations_[22].name[31] = '\0';
    stations_[22].ledIndex = 99;
    stations_[22].distanceFromStart = 45.0;

    Serial.print("[ScheduleModule] Loaded ");
    Serial.print(stationCount_);
    Serial.println(" stations");
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
    // Validate station indices
    if (fromStation >= stationCount_ || toStation >= stationCount_) {
        return 0;  // Invalid stations
    }

    if (fromStation == toStation) {
        return 0;  // Same station
    }

    // Calculate distance between stations
    float distance = stations_[toStation].distanceFromStart - stations_[fromStation].distanceFromStart;
    if (distance < 0) {
        distance = -distance;  // Handle reverse direction
    }

    // Average speed: ~35 km/h including stops
    // This translates to approximately 1.7 minutes per km, or 102 seconds per km
    const float SECONDS_PER_KM = 102.0;

    uint16_t travelTimeSeconds = (uint16_t)(distance * SECONDS_PER_KM);

    // Add minimum dwell time at intermediate stations (if traveling through multiple stations)
    uint8_t intermediateStations = 0;
    if (fromStation < toStation) {
        intermediateStations = toStation - fromStation - 1;
    } else {
        intermediateStations = fromStation - toStation - 1;
    }

    // Add 20 seconds per intermediate station for dwell time
    const uint16_t DWELL_TIME_PER_STATION = 20;
    travelTimeSeconds += intermediateStations * DWELL_TIME_PER_STATION;

    return travelTimeSeconds;
}

const TrainSchedule* ScheduleModule::getCurrentSchedule(time_t currentTime) {
    struct tm* timeinfo = localtime(&currentTime);
    if (timeinfo == nullptr) {
        // Return weekday schedule as default
        static TrainSchedule weekdaySchedule = {
            300,   // 5:00 AM (first train)
            1500,  // 1:00 AM next day (25:00, or 1440 + 60)
            10,    // 10 minute headway during peak
            false  // weekday
        };
        return &weekdaySchedule;
    }

    int dayOfWeek = timeinfo->tm_wday;  // 0 = Sunday, 6 = Saturday

    // Saturday schedule
    if (dayOfWeek == 6) {
        static TrainSchedule saturdaySchedule = {
            330,   // 5:30 AM (first train)
            1470,  // 12:30 AM next day (24:30)
            12,    // 12 minute headway
            true   // weekend
        };
        return &saturdaySchedule;
    }

    // Sunday schedule
    if (dayOfWeek == 0) {
        static TrainSchedule sundaySchedule = {
            360,   // 6:00 AM (first train)
            1440,  // 12:00 AM (midnight)
            15,    // 15 minute headway
            true   // weekend
        };
        return &sundaySchedule;
    }

    // Weekday schedule (Monday-Friday)
    static TrainSchedule weekdaySchedule = {
        300,   // 5:00 AM (first train)
        1500,  // 1:00 AM next day (25:00, or 1440 + 60)
        10,    // 10 minute headway during peak
        false  // weekday
    };
    return &weekdaySchedule;
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
