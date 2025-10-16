#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "../../core/schedule_module.h"
#include "../../core/position_engine.h"

namespace py = pybind11;

PYBIND11_MODULE(link_rail_core, m) {
    m.doc() = "Link Light Rail simulation core";

    // Station struct binding
    py::class_<Station>(m, "Station")
        .def(py::init<>())
        .def_property("name",
            [](const Station &s) { return std::string(s.name); },
            [](Station &s, const std::string &name) {
                strncpy(s.name, name.c_str(), 31);
                s.name[31] = '\0';
            })
        .def_readwrite("ledIndex", &Station::ledIndex)
        .def_readwrite("distanceFromStart", &Station::distanceFromStart);

    // TrainSchedule struct binding
    py::class_<TrainSchedule>(m, "TrainSchedule")
        .def(py::init<>())
        .def_readwrite("firstTrainMinutes", &TrainSchedule::firstTrainMinutes)
        .def_readwrite("lastTrainMinutes", &TrainSchedule::lastTrainMinutes)
        .def_readwrite("headwayMinutes", &TrainSchedule::headwayMinutes)
        .def_readwrite("isWeekend", &TrainSchedule::isWeekend);

    // TrainPosition struct binding
    py::class_<TrainPosition>(m, "TrainPosition")
        .def(py::init<>())
        .def_readwrite("ledIndex", &TrainPosition::ledIndex)
        .def_readwrite("isNorthbound", &TrainPosition::isNorthbound)
        .def_readwrite("isActive", &TrainPosition::isActive);

    // ScheduleModule class binding
    py::class_<ScheduleModule>(m, "ScheduleModule")
        .def(py::init<>())
        .def("loadSchedule", &ScheduleModule::loadSchedule)
        .def("getStation", &ScheduleModule::getStation,
             py::return_value_policy::reference)
        .def("getStationCount", &ScheduleModule::getStationCount)
        .def("getTravelTime", &ScheduleModule::getTravelTime)
        .def("getCurrentSchedule", &ScheduleModule::getCurrentSchedule,
             py::return_value_policy::reference)
        .def("getCurrentMinuteOfDay", &ScheduleModule::getCurrentMinuteOfDay)
        .def("isServiceHours", &ScheduleModule::isServiceHours);

    // PositionEngine class binding
    py::class_<PositionEngine>(m, "PositionEngine")
        .def(py::init<>())
        .def("init", &PositionEngine::init)
        .def("updateAllTrains", &PositionEngine::updateAllTrains)
        .def("getActiveTrainPositions", [](PositionEngine& self) {
            uint8_t count = 0;
            const TrainPosition* positions = self.getActiveTrainPositions(&count);
            // Convert to Python list
            py::list result;
            for (uint8_t i = 0; i < count; i++) {
                result.append(positions[i]);
            }
            return result;
        });
}
